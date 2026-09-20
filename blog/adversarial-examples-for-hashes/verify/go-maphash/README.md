# go-maphash: a cross-length pair that collides for every map seed in one process out of 2^24

**Hash.** The Go runtime map hash, go1.27.1 (tag commit `862c888e612ac346c7c4d99c9392bdfd265f33b0`,
released 2026-08-28, the newest release when this was written; `memhash_amd64.s` is unchanged
on master).  One function, `internal/runtime/maps.memHashAES` in
`src/internal/runtime/maps/memhash_amd64.s`, backs `map[string]` and every other map whose key
hasher reaches `runtime.memhash`, and `hash/maphash.Bytes`, `String` and `Hash`.  It is the
default on every AES-NI x86-64 host (`AlgInit` in `runtime_alg.go`).  Verbatim copies of the
assembly, of `runtime_alg.go` (the key set-up) and of `hash/maphash/maphash.go` (the 128-byte
chaining and `MakeSeed`) are in `upstream/`, with Go's BSD-3 `LICENSE`:

```text
a56cd4208a307a96aa1076b8e413a2692ec175dc438f6e7a30dc8f3d56dee47d  upstream/memhash_amd64.s
67587da3e7b8800883b3db62e51e860e9476cb75ca07888a2e3bf752a95c9209  upstream/runtime_alg.go
d83033f300c448b16a49cd8bffa2e916fb63ac807c36483b8ed2987c087e9ebf  upstream/maphash.go
911f8f5782931320f5b8d1160a76365b83aea6447ee6c04fa6d5591467db9dad  upstream/LICENSE
```

`go_maphash_verify.c` is a C **port** of the assembly, not the upstream code (Go's assembly cannot
be linked into a C program).  It is validated bit-exactly against the real runtime: the program
embeds 2 x 2883 outputs recorded by `vectors.go` from the real go1.27.1 runtime (memhash on 312
lengths x 5 seeds, the page-end load branch on 15 lengths x 5 seeds, and `maphash.Bytes` on 312
lengths x 4 seeds, once with a fixed key and once with the process's own random key) and exits
non-zero unless all 5766 match.  `real_check.go` re-runs the pair on the real runtime itself:
it reaches the per-process key `internal/runtime/maps.aeskeysched` and `runtime.memhash` through
`go:linkname` (hence `-ldflags=-checklinkname=0`), and calls the public `maphash.Bytes` and
`maphash.String` on the witness.

**Seed protocol, as Go uses it.** Two layers, both drawn by the runtime, neither settable by the
program: per process, `initAlgAES` fills `aeskeysched` (128 bytes = 16 x `bootstrapRand()`, a
ChaCha8 stream seeded from OS randomness at start-up); per hash table, `map.go` draws a 64-bit
`seed` at map creation (and on `clear`), and `maphash.MakeSeed` draws a non-zero 64-bit value
from the same runtime generator.  A message of at most 16 bytes sees only the first 16 key
bytes `K0` and the seed.  The sampling model is therefore a fresh uniformly random 128-byte key
**and** a fresh uniformly random 64-bit seed per trial.  The collision event below depends only
on `aeskeysched[8]`, `[10]`, `[12]`, `[14]`: in a process where it fires, the pair collides in
every map and under every `maphash.Seed`; in every other process it never collides.

**What the pair exploits.** For 1..16 bytes the hash is `lo64(F^3(pad16(m) ^ S0))` with
`S0 = F(X0 ^ K0)`, `X0 = [seed | len16 x4]` (the 16-bit length repeated in words 4..7, so its
low byte sits at bytes 8, 10, 12, 14) and `F(x) = AESENC(x, x)`, a keyless AES round with
feed-forward.  Lengths 15 and 16 give pre-round states `y` and `y ^ 0x1f` at those four bytes,
`y = X0 ^ K0` uniform.  Each of the four active S-boxes maps the input difference `0x1f` to the
output difference `0xa3` for exactly 4 of its 256 inputs (the largest entry of the AES S-box
difference table), so with probability exactly `(4/256)^4 = 2^-24` over `K0` the difference
`S0 ^ S0'` is the constant `Delta* = MC(SR(0xa3 at 8,10,12,14)) ^ (0x1f at 8,10,12,14) =
a3fe5da3a3fe5da342a3bcfe42a3bcfe`.  Setting `m' = pad16(m) ^ Delta*` then makes the inputs of
the keyless `F^3` identical, for every seed.  The event is a property of the key alone, so it
is also a property of the process: about one Go process in 16.8 million hashes these two
strings identically in all its maps.  The same construction works for every length L = 1..15
against 16 (`Delta*(L)` from the difference-table entry of `L ^ 16`); all 16 bytes of the
longer message are forced, so the longer message must be exactly 16 bytes.

## Pair

| # | mechanism | m | m' | rate over random (key, seed) |
|---|-----------|---|----|------------------------------|
| 1 | 15 vs 16 bytes, length byte through the one keyless round that makes `S0` | `000000000000000000000000000000` | `a3fe5da3a3fe5da342a3bcfe42a3bcfe` | `(4/256)^4 = 2^-24` exactly, over the per-process key; measured 68 / 2^30 = 2^-23.91 here, 66 / 2^30 in the row's independent sample |

Explicit witness (from the program, no search): the key whose bytes 8, 10, 12, 14 are
`0f 10 ab b4` (the four difference-table solutions `y ^ 0x0f`), the other 124 bytes the
vectors' fixed key, `aeskeysched[0..15] = 97f3c8345b1b9e040fb510c3ab89b493`.  Under seed
`0x12c3be9a410c165a` (the row's example seed) both messages hash to `1e9f1988569a717a`; under
seed `0x4796ebcf1459430f` both hash to `76d93afbe704ae76`.  The real runtime gives the same
values through `maphash.Bytes`, `maphash.String` and `runtime.memhash` (`real_check_xeon.txt`).
Score: `log2(2 words / 2^-24) = 25.0` bits (the row's cap); the 2^30 sample here gives
`log2(2 / (68/2^30)) = 24.91`.

## Build and run

    cc -O2 -std=c11 -o go_maphash_verify go_maphash_verify.c -lm   # portable table AES; add -maes (x86-64) or
                                                                   # -march=armv8-a+crypto (arm64) for hardware AES
    ./go_maphash_verify            # 2^24 random (key, seed) samples (default), about 3 s of CPU with hardware AES
    ./go_maphash_verify 30         # the row's sample size, about 3.5 min of CPU with AES-NI
    ./go_maphash_verify 20 7       # different RNG master seed
    make && make check             # same, with the Makefile in this directory (expects `65536 65536 0 0` at 2^20)

`-DGO_PORTABLE` forces the table implementation (the default build without target flags is the
same code; `make portable` builds it as `go_maphash_verify_portable`).  Arguments:
`[log2 N (0..40), default 24] [RNG master seed, default 1]`; a non-numeric or out-of-range
argument is rejected with status 2.  The program is single-threaded, reads no files and writes
only stdout/stderr.

The program aborts unless it reproduces all 5766 embedded real-runtime outputs (with a hardware
AES path it first cross-checks that round against the table round on 4096 blocks), finds the
largest difference-table entry to be 4, re-derives `Delta*(15)` and therefore `m'` from the
difference table, and reproduces the witness value `1e9f1988569a717a` on the constructed key.
It then asserts that the constructed key collides under 65536 random map seeds, under 65536
re-draws of the other 124 key bytes with the seed random, and never on a control key (byte 8
changed), and that the first sampled hit, if any, has difference-table solutions in all four
key bytes.  Exit status 0 only if every check passes.

## Expected output

Intel Xeon Platinum 8375C, gcc 11.5, `gcc -O2 -std=c11 -maes`, `./go_maphash_verify 20`
(`run_2p20.txt`; the portable build prints the same apart from the `impl =` word and the missing
hardware-vs-table line):

```
Go runtime map hash / hash/maphash cross-length pair check  (impl = x86-aesni)
hash: go1.27.1 (862c888e) internal/runtime/maps memHashAES, memhash_amd64.s: the amd64 AES-NI path behind
      map[string], map[[]byte]-style keys (typ.Hasher -> memhash) and hash/maphash.Bytes/String/Hash
validation: x86-aesni round vs portable table round: 0 mismatches / 4096 OK
validation: real go1.27.1 runtime vectors, key A (vectors_go fixed aes): 2883 / 2883 match (memhash 1560, page-end loads 75, maphash.Bytes 1248) OK
validation: real go1.27.1 runtime vectors, key B (vectors_go random aes, a real process key): 2883 / 2883 match (memhash 1560, page-end loads 75, maphash.Bytes 1248) OK
validation: AES S-box difference table: largest entry 4 (so one active S-box passes with probability at most 4/256) OK
validation: L = 15 vs 16: input difference 0x1f -> output difference 0xa3 for 4 of 256 inputs (y = 00, 1f, a4, bb)
validation: Delta*(15) re-derived from the difference table = a3fe5da3a3fe5da342a3bcfe42a3bcfe (published a3fe5da3a3fe5da342a3bcfe42a3bcfe) OK

Pair: 15-byte vs 16-byte cross-length pair (L = 2 words), per-process key event
   mechanism: for 1..16 bytes h = lo64(F^3(pad16(m) ^ S0)), S0 = F(X0 ^ K0), X0 = [seed | len16 x4], F(x) = AESENC(x,x).
   The length enters X0 only as its low byte at bytes 8,10,12,14, so lengths 15 and 16 feed y and y ^ 0x1f into
   the one keyless round that makes S0.  When the four S-boxes at those bytes all take the difference-table-4
   output (probability (4/256)^4 = 2^-24 over K0 = aeskeysched[0..15]), S0 ^ S0' is the constant Delta*, and
   m' = pad16(m) ^ Delta* makes the F^3 inputs identical: the pair collides for every map seed in that process.
  m  (15 bytes) = 000000000000000000000000000000
  m' (16 bytes) = a3fe5da3a3fe5da342a3bcfe42a3bcfe
  re-deriving m' from m (difference table only, no search): matches the published hex
  constructed colliding key (no search): aeskeysched[0..15] = 97f3c8345b1b9e040fb510c3ab89b493
    bytes 8,10,12,14 = 0f 10 ab b4 (each solution y ^ 0x0f); a process whose aeskeysched has these four
    bytes (one in 2^24) collides on this pair in every map and under every maphash.Seed
    witness seed 12c3be9a410c165a (the row's example seed): maphash.Bytes(m) = 1e9f1988569a717a  maphash.Bytes(m') = 1e9f1988569a717a (expected 1e9f1988569a717a, the real runtime's value) OK
    that key, 65536 random map seeds: collisions = 65536 / 65536 (e.g. seed b3f2af6d0fc710c5 -> e0d02dc9f4f636e8 for both) OK
    same four bytes, the other 124 key bytes and the seed random, 65536 draws: collisions = 65536 / 65536 OK
    control: byte 8 of that key changed, 65536 random seeds: collisions = 0 / 65536 OK
  random samples: N = 1048576 (2^20), a fresh 128-byte aeskeysched and a fresh 64-bit seed per trial (rng master seed 1)
    collisions = 0 / 1048576; rate = 0; log2(rate) = -inf (zero hits; one-sided 95% upper limit 2^-18.117)
    analytic rate (4/256)^4 = 2^-24.000; cap log2(2 words / 2^-24) = 25.0 bits
    no sampled collision at this size (expected N/2^24 = 0.0625); the constructed key above is the explicit witness
    first hit's bytes 8,10,12,14 are all solutions of the difference table: n/a (no hit) OK

ALL CHECKS PASSED: implementation matches the real runtime, the constructed key collides for every seed,
and the sampled rate is consistent with (4/256)^4.
```

`make check` compares the four counts `65536 65536 0 0`.  **2^20 is a smoke run** for the
random-sample line: 0.06 hits are expected, so its zero says nothing about the rate; the rate
is carried by the constructed key (exact, every seed) and by the 2^30 run below.

## The 2^30 run (the row's sample size)

`./go_maphash_verify 30` on the same host, single thread, RNG master seed 1 (`run_2p30_xeon.txt`,
3 min 16 s of CPU; the wall time in the log is longer because the cores were shared):

```
  random samples: N = 1073741824 (2^30), a fresh 128-byte aeskeysched and a fresh 64-bit seed per trial (rng master seed 1)
    collisions = 68 / 1073741824; rate = 6.33299e-08; log2(rate) = -23.913; exact 95% Poisson interval [2^-24.277, 2^-23.570]
    analytic rate (4/256)^4 = 2^-24.000; cap log2(2 words / 2^-24) = 25.0 bits
    first colliding sample: aeskeysched[0..15] = 334e1e4862edff14abf71082102eb40b (bytes 8,10,12,14 = ab 10 10 b4), seed 90a1f2d8981b24dd: memhash(m) = memhash(m') = 4cadf64a7de30baa
    first hit's bytes 8,10,12,14 are all solutions of the difference table: yes OK
```

The row's own independent 2^30 sample (a different program and RNG) gave 66 / 2^30 = 2^-23.96
with interval [2^-24.33, 2^-23.61]; the pooled L = 1..7 vs 16 family gave 454 / (7 x 2^30) =
2^-23.98.  Both intervals contain the analytic 2^-24.  This stream is deterministic: the same
arguments give the same 68 on any machine.

## Real runtime check

`real_check.go` needs the go1.27.1 toolchain (`go version go1.27.1 linux/amd64` here) and a
host on the AES path (`UseAeshash=true`); build with `make real` or
`go build -ldflags=-checklinkname=0 -o real_check real_check.go`.  `real_check_xeon.txt` records:

* `./real_check show` twice: two different `aeskeysched[0:16]` values, i.e. the key is
  per-process random;
* `./real_check witness <key> 12c3be9a410c165a <m> <m'>` with the constructed key: `COLLIDE`
  under both seeds, with the values quoted above from the C program;
* `./real_check sample <m> <m'> 28`: the pair through `runtime.memhash` with all 128 key bytes
  and the seed re-drawn per sample from a ChaCha8 stream seeded by `crypto/rand` (the runtime's
  own generator family): **13 / 2^28 = 2^-24.30** (16 expected; the row's verifier got 20 / 2^28,
  its search lane 12 / 2^28), each hit shown through the public `maphash.Bytes` under two seeds.
  Re-filling the global between samples is equivalent to 2^28 process starts because `memhash`
  reads it on every call.

`vectors.go` is the program that produced the embedded vectors
(`go build -ldflags=-checklinkname=0 -o vectors_go vectors.go`; `./vectors_go fixed aes` and
`./vectors_go random aes`); its `M`, `E` and `B` lines are what the C program checks, its `S`
lines (streaming `maphash.Hash`) equal the `B` lines by construction, and its `H64`/`H32`/`C*`
lines cover `memhash64`/`memhash32`, which this pair does not touch.

## Scope

Only the amd64 AES path is scored.  `map[uint64]`, `map[uint32]` and `maphash.Comparable` use
`memhash64`/`memhash32` (three keyed AES rounds) and are not affected by this pair.  The arm64
AES path builds its seed vector from the full 64-bit length and has a different, weaker-looking
trail (about 2^-32 in the row, measured under emulation only); the no-AES fallback has no
finding.  The witness key is a legitimate output of `initAlgAES` (any 128 bytes are), and the
`go:linkname` route only installs it; nothing in the hash is patched.

## Files

- `go_maphash_verify.c` -- single-file C11 program (MIT, full text in the header); the hash is a
  transcription of `upstream/memhash_amd64.s` and the embedded vectors are real-runtime outputs.
- `real_check.go`, `vectors.go` -- the real-runtime programs (MIT), go1.27.1.
- `Makefile` -- build, `check`, `portable`, `real`, `clean` for this directory.
- `run_2p20.txt`, `run_2p30_xeon.txt`, `real_check_xeon.txt` -- the runs quoted above, exactly
  as printed (with `time` blocks).
- `upstream/` -- the four go1.27.1 files named at the top, verbatim, under Go's BSD-3 license.
- `v_fixed_aes.txt`, `v_random_aes.txt` -- the two raw `vectors.go` dumps (5031 lines each, with the
  `KS` key line) from which the C program's embedded table was taken; the `S` and `H64`/`H32`/`C*`
  lines are the parts it does not use.
- `gomh.c`, `pair_real.go`, `verify.log` -- the row's own independent verifier (an AES-NI intrinsic
  transcription with `vectors`, `delta`, `measure` and `seedfree` modes, OpenMP), its real-runtime
  sampler, and the log behind the row's 66 / 2^30, the pooled L = 1..7 counts and its 20 / 2^28
  real-runtime figure.  Supplied as evidence; `go_maphash_verify.c` and `real_check.go` above were
  written separately and are what the commands in this README run.

## Attribution and licensing

The Go runtime sources in `upstream/` are Copyright The Go Authors, BSD 3-Clause
(`upstream/LICENSE`); the C transcription is close enough to the assembly to count as a
derivative, so that notice applies to the `memhash_aes` function as well.  Driver code, the Go
checkers and this README are Copyright (c) 2026 Thomas Dybdahl Ahle, MIT.

The finding is new (no prior published fixed-pair result for the Go AES map hash was found);
the hash/maphash documentation says the hash is "intended to be collision-resistant, even for
situations where an adversary controls the byte sequences being hashed" and "not
cryptographically secure", and states no numeric bound.
