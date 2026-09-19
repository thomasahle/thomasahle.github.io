# verify: reader-runnable reproductions of the collision pairs

26 collision-reproduction directories, one per hash or separately tested output/version, plus the separate HalftimeHash audit directory.  Each holds a primary C11 program, a README and (for some)
the raw logs of the runs quoted there.  A program re-implements or embeds the hash, checks its implementation at
startup against the listed verification values and available test vectors (it recomputes the SMHasher3 verification
value with SMHasher3's own procedure or checks a reference vector, and exits
non-zero on any mismatch), prints the published message pair(s), hashes both messages under
millions of random seeds from a fixed-seed RNG, and prints the collision count, the rate, and
explicit colliding seeds with both hash values.  The original programs are single-threaded; the two foldhash additions use POSIX threads (one worker in the smoke commands). Programs need a C
compiler, libm or pthreads as documented, and writes nothing but stdout/stderr.

| hash | version | pair length | mechanism | measured rate | how to run |
|---|---|---|---|---|---|
| `cityhash-64` | CityHash v1.1.1 (SMHasher3 `CityHash_64`, 64-bit seed) | 8 B; 32 B | The seed enters only through `HashLen16(CityHash64(m) - k2, seed)`, a bijection of the unseeded value, so any unseeded collision (pair A: Brent rho at 2^32.35; pair B: analytic inversion of `HashLen17to32`, a 2^192 multicollision) collides for every seed. | 1 (2^0) on 2^24 seeds and 2^24 `(seed0, seed1)` pairs, both pairs; key-free | `cd cityhash-64 && ./cityhash64_verify [N or 2^k] [rng seed]` (0.3 s) |
| `farmhash-64` | FarmHash v1.1 NA (`FarmHash_64__NA`) | 8 B; 32 B; 32 B | Same final-combine bijection as CityHash: `Hash64WithSeed(m, s) = HashLen16(Hash64(m) - k2, s)`.  Pair A from a rho walk, pair B solved in closed form (`HashLen17to32` is affine-invertible in its last two words), pair C is Orson Peters' published pair. | 1 (2^0) on 2^24 seeds, all three pairs, both seed interfaces; key-free | `cd farmhash-64 && ./farmhash64_pairs [log2 N] [rng seed]` (1.5 s) |
| `murmurhash3-128` | MurmurHash3_x64_128, 2012 final (`MurmurHash3_128`, seed truncated to 32 bits) | 24 B; 32 B | A difference of exactly 2^63 in a lane survives xor, add, rotate-into-top-bit and `5x + c`; chosen word differences park `(2^63, 0)` in the state after block 1 for every seed and the tail cancels it (Aumasson-Bernstein top-bit trick); the 32-byte pair returns the state to `(0, 0)`, giving multicollisions. | 1 (2^0) on 2^24 seeds, both pairs (panel: all 2^32 seeds); key-free | `cd murmurhash3-128 && ./murmurhash3_128_verify [log2 N] [rng seed] [--exhaustive]` (1.2 s) |
| `gxhash-64` | gxhash v3, commit 55bde47 (`gxhash_64` and `gxhash`) | 24 B; 15 vs 16 B | The seed is applied only after `compress_all(m)`.  For 17..32 bytes that is `SR(SB(hv)) ^ P2(v0)` with two *public* AES rounds, inverted in constant time to cancel a flipped byte; for <= 16 bytes it is zero-padding plus the length added to every byte, so every short message has a 16-byte twin. | 1 (2^0) on 2^24 seeds, both pairs, 64- and 128-bit output; key-free | `cd gxhash-64 && ./gxhash64_verify [log2 N] [rng seed]` (0.8 s) |
| `museair` | MuseAir v0.3 (all four SMHasher3 variants) | 17 B; 24 B | Bytes 16..len-1 enter the 17..32-byte path through a public function `P(u, v)` XORed onto the bijectively loaded 16-byte head; replacing the head by `head ^ (P(T) ^ P(T'))` for any two tails gives identical state before the seed touches it. | 1 (2^0) on 2^24 seeds, both pairs, all four variants; key-free | `cd museair && ./museair_verify [log2 N] [rng seed]` (2 s) |
| `komihash` | komihash 5.34 (`komihash.h` embedded verbatim; SMHasher3 `komihash` value) | 64 B; 15 B | Pair 1: with `m1 = m0 ^ IVAL2`, `m5 = m4 ^ IVAL6` two lanes compute the same product for every seed and cancel in the fold; flipping bit 0 of both words survives unless a carry crosses the half (0.64 + 0.36 * 3/4).  Pair 2: the one seed whose S5 equals the tail word zeroes the final multiply. | 0.9106 (2^-0.135) on 2^24 seeds (panel 0.910588 at 2^32); pair 2: one weak seed, density 2^-64 | `cd komihash && ./komihash_pair [log2 N] [rng seed hex]` (0.5 s) |
| `spookyhash2-64` | SpookyHash V2 (`SpookyHash2_64`, h1 = h2 = seed) | 275 B selected; 286 B (two supporting pairs) | Add 2^63 to Mix word 11 of the last full block; the step-11 xor and rotate turn it into `s10 += 2^63` and `s11 += ±2^45`, cancelled by End words 10 and 11 of the tail.  The sign is bit 63 of `s11`, so the two full-state collision conditions are complementary; exact balance is unproved. | 0.4999 (2^-1.000) and 0.5001 on 2^24 seeds; at least one pair has a full-state collision for every covered initialization | `cd spookyhash2-64 && ./spookyhash2_64_pair [log2 N] [rng seed] [seed mode 0/1]` (4 s) |
| `rust-ahash` | aHash 0.8.12 AES | A_2: 56 B; L=7 | Three-S-box trail plus additive cancellation; independent key words | 12,627/2^33 ≈ 2^-19.376; cap 22.18 bits | `cd rust-ahash && ./ahash_pairs 24 1 A_2` |
| `t1ha2-64` | t1ha2_atonce-64 v2.1.4 | F60: 13/16 B; L=2 | 24-bit seed class plus sampled carries | 2^-24 × 58,715,203/2^30 ≈ 2^-28.19; cap 29.19 bits | `cd t1ha2-64 && ./t1ha2_64_verify 24` |
| `a5hash` | a5hash v5.21 (`a5hash`, `a5hash_128`) | 23 B; 8 B; 25 B (128-bit) | The seed expansion `umul128` has thousands of preimage seeds for a well-chosen `s1_0`; those seeds zero the first block's operand (pair 1) or the tail multiply (pair 2, both hashes 0).  Pair 3: on the 128-bit 17..32-byte path the last 16 bytes enter only through a product with *public* constants. | 156800/2^64 = 2^-46.74 and 7291/2^64 = 2^-51.17 (classes enumerated exactly, every member collides); pair 3: 1 (2^0) on 2^24 seeds | `cd a5hash && ./a5hash_verify [log2 N] [rng seed]` (0.5 s) |
| `highwayhash` | HighwayHash, frozen (`c/highwayhash.c` embedded verbatim; `HighwayHash_64/128/256`) | 96 B (three packets) | In the key class `hi32(key[0]) = 0xdbe6d5d5` packet 1's lane-0 multiply is 0, so a +2^8 lane-0 difference survives as one byte; packet 2's -(2^9 + 2^24) collapses it when a 32-bit relation of the packet-2 multiplier holds (event E_2, Pr = 235 * 239 / 2^40) and packet 3's +2^8 cancels it: full 1024-bit state collision at every width. | class density 2^-32 x conditional 2^-24.22 = 2^-56.22 over uniform 256-bit keys; E_2 screen 47/2^30 = 2^-24.45 measured, every hit a full-state collision | `cd highwayhash && ./highwayhash_verify [log2 N] [rng seed] [sm3]` (16 s) |
| `pengyhash` | pengyhash v0.3 | 32/32 B, 32/1 B | Lines 3–7 compress each block before the seed enters on line 10. | 1 for every sampled seed; structural identity | `cd pengyhash && ./pengyhash_verify 20` |
| `nmhash32` | nmhash32 v2 | 64/64 B | The changed lane is j = 0. | See sub-README; 2^20 smoke check is separate from the scored historical rate | `cd nmhash32 && ./nmhash32_verify 20` |
| `nmhash32x` | nmhash32x v2 | 28/28 B | The first two words differ by Δ = 0x08008008. | 1 for every sampled seed; structural identity | `cd nmhash32x && ./nmhash32x_verify 20` |
| `mx3` | mx3 v3 | 1/8 B, 7/8 B | Every message term in lines 8–10 goes through the public bijection g. | 1 for every sampled seed; structural identity | `cd mx3 && ./mx3_verify 20` |
| `mir` | mir.exact, mir.inexact | 8/8 B, 16/16 B | For eight-byte messages, the only message term is the public mum(w,p1) on line 11. | 1 for every sampled seed; structural identity | `cd mir && ./mir_verify 20` |
| `fasthash` | fasthash-64, fasthash-32 SMHasher3 64-bit seed, fasthash-32 upstream 32-bit seed | 7/8 B, 16/16 B | The seed enters on line 4, but every message word first passes through the public bijection mix. | 1 for every sampled seed; structural identity | `cd fasthash && ./fasthash_verify 20` |
| `mum` | MUM v3 exact unroll3, MUM v3 exact unroll4 | 8/8 B | Paper pair G: public _mum(w,p0) terms are identical. | 1 for every sampled seed; structural identity | `cd mum && ./mum_verify 20` |
| `rapidhash-v3` | rapidhash v3, rapidhash v3 micro, rapidhash v3 nano | 32/32 B, 48/48 B | Paper pairs A (32 B) and D (48 B), in standard, micro and nano v3. | See sub-README; 2^20 smoke check is separate from the scored historical rate | `cd rapidhash-v3 && ./rapidhash_v3_verify 20` |
| `wyhash` | wyhash final v4.3 | A (32 B) | First-product XOR-fold differential; fixed default secret. | Historical 9 / 2^30 = 2^-26.830075; 2^20 is a smoke run only | `cd wyhash && ./wyhash_verify [log2 N] [rng seed]` |
| `rapidhash-v1` | rapidhash v1.0 | A (32 B) | First-product XOR-fold differential; fixed default secret. | Historical 12 / 2^30 = 2^-26.415037; 2^20 is a smoke run only | `cd rapidhash-v1 && ./rapidhash_v1_verify [log2 N] [rng seed]` |
| `xxh3-64` | XXH3-64 0.8.3 | selected 32 B; L=4 | Block-1 complement, NAF carry cancellation | 2,264,081/(3·2^30) ≈ 2^-10.4745; cap 12.47 bits | `cd xxh3-64 && ./xxh3_64_pair_check 20` |
| `museair-v2` | MuseAir v2, crate 0.6.0 | 32 B; L=4 | Tail-product carry differential | 36,060/(1.5·2^32) ≈ 2^-17.447; cap 19.45 bits | `cd museair-v2 && ./museair_v2_verify 24` |
| `xxh3-128` | XXH3-128, xxHash 0.8.3 | F (32 B) | Complementary first-word swap; equality of both output halves. | Historical 5 / 2^30 = 2^-27.678072 (full 128-bit collisions); 2^20 is a smoke run only | `cd xxh3-128 && ./xxh3_128_verify [log2 N] [rng seed]` |
| `go-maphash` | Go runtime map hash / hash/maphash, go1.27.1 (862c888e), amd64 AES path (`memhash_amd64.s`; C port validated on 5766 real-runtime outputs) | 15 vs 16 B | The 16-bit length is repeated into the seed vector and passes through one keyless AES round before the message is XORed in; when the four active S-boxes take their difference-table-4 output (probability (4/256)^4 over the per-process key) the seed-state difference is a constant, cancelled by the 16-byte message.  Depends only on `aeskeysched[8,10,12,14]`: in one process in 2^24 the pair collides in every map and under every `maphash.Seed`. | exactly 2^-24 over the per-process key (a constructed key collides on 65536/65536 seeds); sampled 68 / 2^30 = 2^-23.91 (row: 66 / 2^30); real runtime 13 / 2^28 | `cd go-maphash && ./go_maphash_verify [log2 N] [rng seed]` (3 s with AES-NI) |
| `dotnet-marvin` | Marvin32, .NET 10.0.12 `string.GetHashCode()` (`Marvin.cs` at tag v10.0.12) | A: 12/12 B (L = 2); B: 8/8 B (L = 1) | The 64-bit seed is only the initial state and every later step is a keyless bijection; one ARX Block between word injections lets a three-word additive differential cancel inside Block 2 (pair A) for one seed in 480. | Historical A: 8945794 / 2^32 = 2^-8.907 (cap 9.91 bits); B: 2^-22.5; 2^20 gives 2274 and 0 (smoke) | `cd dotnet-marvin && ./marvin32_verify [log2 N] [rng seed] [A, B or AB]` (0.2 s; `32 1 A` for the row's sample size) |
| `abseil-hash` | absl::Hash, abseil-cpp 73d2688 (= LTS 20260817.0), `absl::Hash<std::string_view>` / SwissTable default hasher | 0 vs 8 B; 1 vs 8 B; 16 B | For `len <= 8` the hash is `Mix(seed ^ D(len) ^ v, kMul)`: the seed and the length mix `D(len)` are XORed into the same multiplicand of one fixed map, so a cross-length pair with equal `v ^ D(len)` collides for every seed; for `9..16` bytes a last word equal to `kMul` zeroes the other multiplicand (both hashes 0). | 1 (2^0) on the 32 SwissTable seeds and 2^28 uniform 64-bit seeds, all three pairs, scalar and AES-NI builds, and inside real `flat_hash_set` tables; key-free | `cd abseil-hash && ./abseil_hash_verify [log2 N] [rng seed]` (0.1 s; `28` for the row's sample size) |

| `foldhash-fast` | 0.2.0 | 8/8 B; L = 1 | complement both xor-keyed operands via overlapping reads | 2757 / 2^38 | [README](foldhash-fast/README.md) |
| `foldhash-quality` | 0.2.0 | 8/8 B; L = 1 | same pair; deterministic final fold preserves equality | 696 / 2^36 | [README](foldhash-quality/README.md) |

Timings are single-threaded on an Apple M2 Pro for the default 2^24 seeds.  Every program
takes `[log2 N]` as its first argument except `cityhash-64`, which takes `N` or `2^k`. New pass-2 programs default to 2^20 trials; the historical timing sentence applies only to the original eleven.  Each
README gives the exact variant and source, the mechanism in full, the build line, and the
program's expected output; the sub-directory READMEs and programs are the reference, this file
is the index.

## Building and checking

    make            # builds every directory's program in place
    make check      # runs every program with 2^20 seeds and compares its collision counts
                    # with the table below; exit status 0 only if all 24 agree
    make clean      # removes the binaries

`make check` reads, for each directory, the command and the expected counts from the table in
the next section, runs the command in that directory, extracts the integers that follow
"collisions" on the sampling lines (the exact pattern per directory is in the Makefile), and
compares the two lists element by element.  A program that exits non-zero fails the check
even if its counts agree.  The comparison is exact by default: every program seeds its RNG
(splitmix64 into xoshiro256**) from a fixed constant, so the counts are deterministic on any
machine and compiler.  `make check CHECK_TOL=n` accepts a difference of up to `n` per count
instead, for readers who edit a program's RNG seed or sample size and want a sanity check
rather than a byte-exact one.  `DIRS=<subset>` restricts any target, e.g.
`make check DIRS="komihash t1ha2-64"`.

The Makefile compiles with `cc -O2 -std=c11 … -lm`, adding `-march=armv8-a+crypto` (arm64) or
`-maes` (x86-64) for the two programs with a hardware-AES path (`gxhash-64`, `rust-ahash`, `go-maphash`);
all three also build and validate without those flags.  `CC`, `CFLAGS` and `LDLIBS` can be
overridden on the command line.  All sampling is single-threaded; use `make -j2` to limit parallel compilation to two jobs and `make -j1 check` to run the checks serially.

## make check reference

Expected collision counts at 2^20 seeds, in the order the program prints them.  The
Makefile parses this table: keep the directory in column 1, the command (starting with `./`,
which is how the Makefile tells this table from the index above) in column 2 and the
space-separated counts in column 3.

| directory | command (run inside the directory) | expected collision counts |
|---|---|---|
| `a5hash` | `./a5hash_verify 20` | `0 156800 0 7291 1048576` |
| `cityhash-64` | `./cityhash64_verify 2^20` | `1048576 1048576 1048576 1048576 0` |
| `farmhash-64` | `./farmhash64_pairs 20` | `1048576 1048576 1048576 1048576 1048576 1048576` |
| `gxhash-64` | `./gxhash64_verify 20` | `1048576 1048576 1048576 1048576` |
| `highwayhash` | `./highwayhash_verify 20` | `0 0 1 1` |
| `komihash` | `./komihash_pair 20` | `954642 674611 280031 0` |
| `murmurhash3-128` | `./murmurhash3_128_verify 20` | `1048576 0 1048576 0 1048576` |
| `museair` | `./museair_verify 20` | `1048576 1048576 1048576 1048576 1048576 1048576 1048576 1048576 0` |
| `rust-ahash` | `./ahash_pairs 20` | `0 2 0 2 1 0 1 2 0 0` |
| `spookyhash2-64` | `./spookyhash2_64_pair 20` | `524207 524207 524369 524369 1048576 524103 524103 524103` |
| `t1ha2-64` | `./t1ha2_64_verify 20` | `0 57150 0 65785 0 65084` |
| `pengyhash` | `./pengyhash_verify 20` | `1048576 1048576` |
| `nmhash32` | `./nmhash32_verify 20` | `262968` |
| `nmhash32x` | `./nmhash32x_verify 20` | `1048576` |
| `mx3` | `./mx3_verify 20` | `1048576 1048576` |
| `mir` | `./mir_verify 20` | `1048576 1048576 1048576 1048576` |
| `fasthash` | `./fasthash_verify 20` | `1048576 1048576 1048576 1048576 1048576 1048576` |
| `mum` | `./mum_verify 20` | `1048576 1048576` |
| `rapidhash-v3` | `./rapidhash_v3_verify 20` | `0 0 0 0 0 0` |
| `wyhash` | `./wyhash_verify 20` | `0` |
| `rapidhash-v1` | `./rapidhash_v1_verify 20` | `0` |
| `xxh3-64` | `./xxh3_64_pair_check 20` | `0 725 725` |
| `museair-v2` | `./museair_v2_verify 20` | `3 3 0 0` |
| `xxh3-128` | `./xxh3_128_verify 20` | `1` |
| `go-maphash` | `./go_maphash_verify 20` | `65536 65536 0 0` |
| `dotnet-marvin` | `./marvin32_verify 20` | `2274 0` |
| `abseil-hash` | `./abseil_hash_verify 20` | `32 1048576 32 1048576 32 1048576` |

What the counts are:

* `a5hash`: pair 1 uniform, pair 1 class members colliding (all 156800), pair 2 uniform,
  pair 2 class members colliding (all 7291), pair 3 uniform.
* `cityhash-64`: pair A 64-bit seeds, pair A `(seed0, seed1)`, pair B 64-bit, pair B
  `(seed0, seed1)`, control pair (must be 0).
* `farmhash-64`: pairs A, B, C, each on `Hash64WithSeed` then `Hash64WithSeeds`.
* `gxhash-64`: pair 1 `gxhash_64`, pair 1 `gxhash` (128-bit), pair 2 `gxhash_64`, pair 2 `gxhash`.
* `highwayhash`: uniform keys (2^-56 is invisible at 2^20), class keys hashed in full (0.05
  expected), E_2 hits in the 2^24-key screen, and how many of those hits are full-state
  collisions (must equal the hits).
* `komihash`: pair 1 collisions, collisions given no carry (all), collisions given a carry
  (about 3/4), pair 2 uniform (0: one weak seed in 2^64).
* `murmurhash3-128`: pair 1, its control, pair 2, its control, the 4-way multicollision.
* `museair`: the 17-byte pair in the four variants, the 24-byte pair in the four variants, the
  control (0 of 4096).
* `rust-ahash`: A_2, A_1, B_0, historical A, fallback B; each in rs4 then correlated smh models.
* `spookyhash2-64`: pair 1 64-bit and 128-bit, pair 2 64-bit and 128-bit, seeds on which the
  bit-63 predictor was right (all); then selected 275-byte pair at 32, 64 and 128 bits.
* `t1ha2-64`: selected F60, historical A, historical B; each uniform then class.

* `wyhash`, `rapidhash-v1`: pair A.
* `xxh3-64`: selected 24/32/128-byte length checks; identical 16-byte prefixes are excluded.
* `museair-v2`: hash, bfast::hash, hash128, bfast::hash128.
* `xxh3-128`: pair F, full 128-bit equality (one hit in this fixed smoke stream).
* `go-maphash`: the constructed key under 65536 random map seeds (all), the same four key bytes with
  the other 124 bytes and the seed random (all), a control key (0), then the random (key, seed)
  sample (2^-24 is invisible at 2^20; the 2^30 run is in its README).
* `dotnet-marvin`: pair A (12-byte strings, L = 2), then pair B (8-byte, L = 1; 2^-22.5 is invisible
  at 2^20).
* `abseil-hash`: for each of pairs 1, 2, 3: the 32 SwissTable seeds (all collide), then the 2^20
  uniform seeds (all collide).

Counts justified by an every-seed identity remain N for any RNG seed. Other reference counts, including zero-hit rare-event samples, can change with the RNG seed. The exact check uses the fixed default stream.

## What every program has in common

* **Validation before anything else.**  The SMHasher3 verification value of the exact
  registered variant is recomputed with the `lib/Hashinfo.cpp` `_ComputedVerifyImpl`
  procedure (keys `{}`, `{0}`, `{0,1}`, … of length 0..255 with seed `256 - i`, the
  encoded outputs concatenated and hashed with seed 0, first four bytes little-endian;
  XXH3 uses canonical big-endian output encoding). Rapidhash-v1 instead checks the
  local harness’s recorded reference vector; the local SMHasher3 registration is v3.
  Where upstream ships test vectors they are checked too (komihash's README vectors, t1ha's 81
  known answers, HighwayHash's 33-byte vector, Peters' FarmHash string).  Any mismatch ends the
  run with a non-zero status before a pair is touched; each README states the code.
* **The published data are asserted, not just printed.**  Explicit seeds or keys from the
  records must reproduce the published hash values, pair recipes are re-derived where there is
  one (CityHash pair B, gxhash, MuseAir), and pairs that should collide on every seed fail the
  run if a single sampled seed does not.
* **Deterministic sampling.**  All programs use their own splitmix64-seeded xoshiro256** with a
  fixed default seed, overridable from the command line; the sub-README says which output lines
  change with it.
* **Nothing is written to disk** and nothing outside the directory is read.  Building per a
  sub-README's `cc … -o` line, or with this Makefile, leaves only the binary next to the source.
* **Arguments are validated**: non-numeric or out-of-range values are rejected with a usage
  message and a non-zero status rather than silently taken as 0.

## Licensing

* **Original driver code is MIT.** The original eleven programs’ driver additions, READMEs
  and this file: Copyright (c) 2026 Thomas Dybdahl Ahle, MIT License.  Each source file carries
  the full MIT text in its header.
* **komihash** (`komihash/komihash_pair.c`) embeds `komihash.h` release 5.34 **verbatim** (MIT,
  Copyright (c) 2021-2026 Aleksey Vaneev; sha256 of the embedded block equals upstream's, see that
  README).  Its license notice is inside the embedded block.
* **HighwayHash** (`highwayhash/highwayhash_verify.c`) embeds the reference `c/highwayhash.c`
  **verbatim** (Apache License 2.0, Copyright 2017 Google Inc.), with its first line (an
  `#include`) replaced by a comment; the Apache notice and the statement of that one
  modification are in the file header.
* **Go map hash** (`go-maphash/`) keeps verbatim copies of four go1.27.1 runtime files under `upstream/`
  (Copyright The Go Authors, BSD 3-Clause, `upstream/LICENSE`); the C transcription of the assembly is a
  derivative under that notice.
* The other hashes are re-implemented from their references and validated against them.  Where
  the code is close enough to count as a derivative the upstream copyright line is reproduced
  next to ours: CityHash (Google, Inc. 2011, MIT), FarmHash (Google, Inc. 2014 and Frank J. T.
  Wojcik 2021-2022, MIT), gxhash (Frank J. T. Wojcik 2025 and Olivier Giniaux 2023, MIT), a5hash
  (Aleksey Vaneev 2025 and Frank J. T. Wojcik 2021-2025, MIT), aHash (Tom Kaitchuck 2018, MIT OR
  Apache-2.0, two constants), and t1ha (Positive Technologies / Leonid Yuriev 2016-2020, zlib:
  the self-check table, test pattern and probe schedule, with the zlib notice reproduced).
  MurmurHash3 and SpookyHash are public domain; MuseAir is CC0 1.0.


## Integration pass 2

The added standalone programs credit the supplied independent implementations
under `experiment/verify-*/` and the validated driver/records under
`paper_rows/`. Per-hash READMEs include complete expected output. The new RNG
uses one stream per case (default seed 1), so these small checks are distinct
from the earlier large, sometimes multithreaded measurements. No large counts
are silently replaced by small-run estimates.

Added-case count order is exactly the order of each sub-README's pairs table.
All new programs use explicit little-endian loads. NMHASH32/32X use unsigned
32-bit intermediates for narrow products. The exact multiply implementations
require the GCC/Clang 128-bit integer extension.

Pengyhash v0.3 code is GPLv3-or-later (the supplied notice is retained), NMHASH
is BSD 2-Clause, mx3 is CC0, and the other added algorithm notices are retained
in their source files. These are not relicensed by the MIT driver additions.
The full supplied pengyhash license is included as `pengyhash/COPYING`.

The selected 275-byte SpookyHash pair has L=35 and estimated cap 6.13; it
collides at all three output widths at the same measured rate. The earlier
286-byte examples remain in the program. Exact half balance of either
predictor class is unproved, even though the older two classes complement
one another. Native Hash32's 32-bit seed domain differs from the scored
SMHasher3 duplicated 64-bit seed model.

## Completed dependency additions

The four formerly blocked directories now contain standalone C programs and full
EXPECTED output: [wyhash](wyhash/README.md), [rapidhash-v1](rapidhash-v1/README.md),
[XXH3-64](xxh3-64/README.md), and [XXH3-128](xxh3-128/README.md).
Wyhash and rapidhash-v1 transcribe the supplied harness; both XXH3 files embed
xxHash 0.8.3 verbatim with its BSD 2-Clause license. All four default to 2^20
seeds and verify a recorded collision separately. This is only a smoke run;
the 2^30 measurements in the index and sub-READMEs remain historical evidence.
The full package check output is recorded in [REPORT.md](../REPORT.md).

## Scope of the polished package

The 23 primary programs reproduce their deterministic README counts. Matching
known answers checks consistency; it does not prove full equivalence to upstream
on every input. The archive also includes a5hash `selected_pairs.c`, MuseAir
`additional_pairs.c`, and the exact carry-state class counter `count_class.py`.
Their per-hash READMEs give commands and limits.

Sampled rates assume the deterministic pseudorandom streams behave like independent
uniform draws. A zero count is not a hard population bound. Under that assumption,
0/N gives an approximate one-sided 95% upper limit of 3/N. Historical large runs
and exact-count claims are identified separately from the small default checks.

Page scores consistently use L = ceil(max(byte lengths)/8), allowing unequal
lengths. Exact caps round upward; empirical estimates round to nearest.

Numbers pass: [XXH3-64 at 32 bytes](xxh3-64/CHECK_32B.md), program [xxh3_64_32B_check.c](xxh3-64/xxh3_64_32B_check.c); [HalftimeHash independent execution harness](halftime/README.md).
