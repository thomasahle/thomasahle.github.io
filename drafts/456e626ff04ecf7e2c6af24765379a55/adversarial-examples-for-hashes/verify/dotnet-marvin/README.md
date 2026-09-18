# dotnet-marvin: Marvin32, the hash behind .NET `string.GetHashCode()`

**Hash.** Marvin32 as shipped in .NET: `System.Marvin.ComputeHash32` in dotnet/runtime
`src/libraries/System.Private.CoreLib/src/System/Marvin.cs`.  The pinned upstream source is
vendored here verbatim as `Marvin.cs` from tag **v10.0.12** (commit `4271d88e`, the runtime
executed for the vectors below; sha256
`a713890909dc99fe5f57880b6d997a8b26e78b1b75399f0dfad075bd29da4c14`).  The algorithm has not
changed since commit `ddb22ef2` (2022-02-20); `main` at `aec1470a` (2026-09-18) differs from
the vendored file only by a `[MethodImpl(NoInlining)]` attribute line (sha256
`dd2aeb3d1df37410a2cc81f6f3a17668c6dabcf7b146ebd8dc4854b5a4f23869`).  `string.GetHashCode()`,
`string.GetHashCode(ReadOnlySpan<char>)`, `StringComparer.Ordinal.GetHashCode` and
`RandomizedStringEqualityComparer` (the comparer `Dictionary<string,_>` and `HashSet<string>`
switch to after a 100-long bucket chain) all call this function; there is no ISA-specific path.

The program `marvin32_verify.c` transcribes `ComputeHash32` line by line (main 8-byte loop,
4..7-byte tail, overlapping last-4-byte read carrying the `0x80` pad byte, 0..3-byte path, and
`Block` = xor, rotl 20, add, rotl 9, xor, rotl 27, add, rotl 19; output `p1 ^ p0`).  The
transcription is validated at startup against three independent sources, and the run stops
with a non-zero status on any mismatch:

* the 30 vectors of dotnet/corefx `MarvinTests.cs` (64-bit legacy output `p1 << 32 | p0`,
  seeds `4fb61a001bdbcc`, `804fb61a001bdbcc`, `804fb61a801bdbcc`);
* floodyberry's `Marvin32.c` vector `"Abcdefg"` (UTF-16LE), seed `5d70d359c498b3f8` ->
  `ba627c81`;
* 64 vectors from the **real .NET 10.0.12 runtime** (`dotnet_check.txt`, produced by
  `dotnet_check.cs` through reflection on `System.Marvin.ComputeHash32`): four splitmix64 seeds
  times lengths 0..15, whose seeds and message bytes the C program regenerates from the same
  stream state `4d617276696e3332`.  The C port was written on x86-64 and checked against the
  runtime there; the C# is scalar and endian-branched, so arm64 runs the same path.

**Seed protocol, as the ecosystem uses it.**  `String.Comparison.cs`:

    ulong seed = Marvin.DefaultSeed;
    return Marvin.ComputeHash32(ref Unsafe.As<char, byte>(ref _firstChar), (uint)_stringLength * 2, (uint)seed, (uint)(seed >> 32));

`Marvin.DefaultSeed` is 64 random bits drawn once per process (`Interop.GetRandomBytes`:
`arc4random_buf` on macOS and glibc >= 2.36, `BCryptGenRandom` on Windows, `getrandom()`
XOR an `lrand48` stream elsewhere; uniform in every configuration) and enters only as the
initial state, `p0` = low 32 bits, `p1` = high 32 bits.  `RandomizedStringEqualityComparer`
draws its own 64-bit seed the same way per comparer instance.  The message is the string's
UTF-16LE code units.  The program therefore draws a uniform 64-bit value per trial from
xoshiro256** and splits it exactly as `GetHashCode` splits `DefaultSeed`; `dotnet_check.cs`
does the same in the real runtime with operating-system random bytes
(`RandomNumberGenerator.Fill`) and also prints its own process's `DefaultSeed` and the public
`string.GetHashCode()` values of the four strings, checking them against `ComputeHash32`.

**What the pair exploits.**  The seed is only the initial 64-bit state and every later step is
a keyless bijection, so a fixed pair collides exactly when its word differences cancel in the
state.  Only one 8-operation `Block` separates consecutive word injections.  Pair A is a
three-word additive differential (`+0x80000000`, `+0x8c0c4100`, `-0x400`): after Block 1 the
state difference is `(84084100, 08040040)`; adding the second word difference leaves
`(08040000, 08040040)`, whose shared bits 27 and 18 cancel in Block 2's first xor, the remaining
bit cancels against `rotl(p0, 20)` in the first add, the second xor clears `p1`, and the third
word difference removes the single surviving `p0` bit.  This happens for about one seed in
480; the 64-bit state then agrees and so does everything downstream (every sampled hash
collision below is a full-state collision).  Pair B is the one-word (L = 1) witness,
`(+0x11111111, -0x11111111)`, a period-4 pattern fixed by `rotl 20`, at about 2^-22.5.

## Pairs

| # | strings | m (UTF-16LE) | m' (UTF-16LE) | L | rate over random 64-bit seeds |
|---|---------|--------------|---------------|---|-------------------------------|
| A | `"aa" U+542F U+166D "bb"` vs `"a" U+8061 U+952F U+A279 U+FC62 "a"` | `610061002f546d1662006200` | `610061802f9579a262fc6100` | 2 | 2^-8.91 (scored: cap 9.91 bits) |
| B | `"aaaa"` vs `U+1172 U+1172 U+EF50 U+EF4F` | `6100610061006100` | `7211721150ef4fef` | 1 | 2^-22.5 (22.5 bits) |

Both are same-length six- and four-character strings of valid BMP code units.  Recorded
colliding seeds (asserted by both programs, not merely printed): pair A, seed
`9bcb44c8bff5b6f1` -> `7eec677c` for both strings; pair B, seed `c4cec22cffed4464` ->
`22e699d5` for both.  The two seeds are the first hits of the program's default stream.

Deployment meaning of the pair-A rate: about one process in 480 has a `DefaultSeed` under
which the two strings share a hash code in `string.GetHashCode()`, `StringComparer.Ordinal`,
and every `Dictionary<string,_>`/`HashSet<string>` that has switched to randomized hashing
(each of those draws its own seed, with the same odds).  The 32-bit output caps every pair at
32 bits regardless.

## Build and run

    cc -O2 -std=c11 -o marvin32_verify marvin32_verify.c -lm
    ./marvin32_verify              # 2^20 seeds, rng seed 1, both pairs (smoke run, 0.2 s)
    ./marvin32_verify 32 1 A       # pair A at the row's sample size, 2^32 seeds (single-threaded)
    ./marvin32_verify 34 1 B       # pair B at 2^34 seeds
    ./marvin32_verify 24 7         # a different RNG stream

Arguments: `[log2 N (0..40), default 20] [rng seed, default 1] [pairs: A, B or AB, default AB]`.
Single-threaded, reads no files, writes only stdout/stderr; C11 and libm suffice.  The RNG is
xoshiro256** seeded from splitmix64 (default seed 1); each pair restarts the same stream.
Zero hits for pair B at 2^20 are expected (0.18 expected hits) and say nothing about the
population rate; the recorded witness is checked separately.  `make check` compares the
deterministic 2^20 counts, in printed order: **`2274 0`**.

Real-runtime check (needs the .NET 10 SDK; the C program does not):

    dotnet build dotnet_check.csproj -c Release -o out
    dotnet out/dotnet_check.dll 24        # 2^24 OS-random seeds per pair; 4 s

`dotnet_check.txt` is the output of that command on the machine that produced the vectors
(.NET 10.0.12, x86-64, RHEL 9.8).  Its `V` lines are the table embedded in the C program.

## Expected output

Intel Xeon Platinum 8375C, gcc 11.5, `cc -O2 -std=c11`, default arguments (2^20 seeds,
0.2 s; the file `run_2p20.txt`):

```
Marvin32 (.NET string.GetHashCode) fixed-pair collision check
hash: dotnet/runtime System/Marvin.cs at tag v10.0.12 (commit 4271d88e), transcribed; runtime executed for the vectors: .NET 10.0.12
validation: corefx MarvinTests.cs 64-bit vectors (seeds 4fb61a001bdbcc, 804fb61a001bdbcc, 804fb61a801bdbcc): 30/30 PASS
validation: floodyberry Marvin32.c "Abcdefg" (UTF-16LE) seed 5d70d359c498b3f8: ba627c81 expected ba627c81 PASS
validation: .NET 10.0.12 runtime vectors from dotnet_check.txt (4 seeds x lengths 0..15): 64/64 PASS
seed protocol: string.GetHashCode() = Marvin.ComputeHash32(UTF-16LE bytes, 2*Length, (uint)seed, (uint)(seed >> 32)) with seed = Marvin.DefaultSeed,
  64 random bits drawn once per process (per instance for RandomizedStringEqualityComparer); sampled here as uniform 64-bit values from xoshiro256**.

pair A / Marvin32 .NET 10.0.12 (L = 2 words)
M  (12 B) = 610061002f546d1662006200   "aa" U+542F U+166D "bb"
M' (12 B) = 610061802f9579a262fc6100   "a" U+8061 U+952F U+A279 U+FC62 "a"
  mechanism: LE words m = 00610061 166d542f 00620062, m' = 80610061 a279952f 0061fc62, additive
  word differences 0x80000000, 0x8c0c4100, -0x400.  After Block 1 the state difference is
  (84084100, 08040040); adding the second difference leaves (08040000, 08040040), which
  cancels inside Block 2 for about one seed in 480; the third difference removes the last bit.
  The 64-bit state then agrees and every later step is keyless, so the hash codes agree.
recorded colliding seed 9bcb44c8bff5b6f1: H(M)=7eec677c H(M')=7eec677c expected 7eec677c PASS
random seeds: N = 1048576 (2^20), rng seed 1
collisions = 2274 / 1048576; rate = 0.00216865539551; log2(rate) = -8.848983; sampled score = log2(L) - log2(rate) = 9.848983
of which full 64-bit state collisions: 2274
first sampled colliding seed 9bcb44c8bff5b6f1: H(M)=7eec677c H(M')=7eec677c

pair B / Marvin32 .NET 10.0.12 (L = 1 word)
M  (8 B) = 6100610061006100   "aaaa"
M' (8 B) = 7211721150ef4fef   U+1172 U+1172 U+EF50 U+EF4F
  mechanism: one-word pair, word differences (+0x11111111, -0x11111111), a period-4 pattern
  fixed by rotl 20; cancels for about one seed in 2^22.5 (the L = 1 witness).
recorded colliding seed c4cec22cffed4464: H(M)=22e699d5 H(M')=22e699d5 expected 22e699d5 PASS
random seeds: N = 1048576 (2^20), rng seed 1
collisions = 0 / 1048576; rate = 0; log2(rate) = -inf (zero hits; no population-rate estimate)
of which full 64-bit state collisions: 0
no sampled collision; the recorded witness above was checked separately
```

## Full-size runs

The page row quotes pair A at 2^32 seeds.  Reruns of this program at the row's sample sizes on
the same Xeon (single-threaded, `run_2p32_xeon.txt` and `run_2p34_xeon.txt`, timing blocks
retained):

* pair A, `./marvin32_verify 32 1 A`: **collisions = @@A32@@ / 2^32 = 2^@@A32LOG@@**
  (all full-state), sampled score @@A32SCORE@@ bits; @@A32TIME@@.
  The row's independent samples: 8,945,794 / 2^32 = 2^-8.907 (95% Poisson [2^-8.908, 2^-8.906])
  and 8,947,470 / 2^32 = 2^-8.91 from a different RNG, and 34,855 / 2^24 = 2^-8.91 through the
  real runtime.
* pair B, `./marvin32_verify 34 1 B`: **collisions = @@B34@@ / 2^34 = 2^@@B34LOG@@**;
  @@B34TIME@@.  The row's samples: 1398 / 2^33 and 2857 / 2^34 (pooled 2^-22.53).
* real runtime, `dotnet out/dotnet_check.dll 24` (`dotnet_check.txt`): pair A
  **34,923 / 2^24 = 2^-8.908**, pair B 1 / 2^24; both recorded seeds reproduce their hash codes
  through `System.Marvin.ComputeHash32`, and `string.GetHashCode()` equals
  `ComputeHash32(UTF-16LE bytes, DefaultSeed)` for all four strings.

The three C-side estimates and the runtime estimate agree within their Poisson intervals
(the standard deviation of a 2^32 count at this rate is about 2990).

## Files

- `marvin32_verify.c` — single-file C11 program (MIT, full text in the header; the transcribed
  Marvin32 reproduces the .NET Foundation's MIT notice).
- `Marvin.cs` — the upstream source at tag v10.0.12, verbatim (MIT, .NET Foundation).
- `dotnet_check.cs`, `dotnet_check.csproj` — the real-runtime check (MIT); `dotnet_check.txt`
  is its output on .NET 10.0.12.
- `run_2p20.txt`, `run_2p32_xeon.txt`, `run_2p34_xeon.txt` — the runs quoted above, exactly as
  the program printed them.
- `README.md` — this file.
