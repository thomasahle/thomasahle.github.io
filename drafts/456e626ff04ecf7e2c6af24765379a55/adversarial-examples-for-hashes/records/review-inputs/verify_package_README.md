# verify: reader-runnable reproductions of the collision pairs

Eleven directories, one per hash.  Each holds a single C11 program, a README and (for some)
the raw logs of the runs quoted there.  A program re-implements or embeds the hash, proves at
startup that its implementation is the real one (it recomputes the SMHasher3 verification
value with SMHasher3's own procedure, plus upstream test vectors where they exist, and exits
non-zero on any mismatch), prints the published message pair(s), hashes both messages under
millions of random seeds from a fixed-seed RNG, and prints the collision count, the rate, and
explicit colliding seeds with both hash values.  Everything is single-threaded, needs only a C
compiler and libm, and writes nothing but stdout/stderr.

| hash | version | pair length | mechanism | measured rate | how to run |
|---|---|---|---|---|---|
| `cityhash-64` | CityHash v1.1.1 (SMHasher3 `CityHash_64`, 64-bit seed) | 8 B; 32 B | The seed enters only through `HashLen16(CityHash64(m) - k2, seed)`, a bijection of the unseeded value, so any unseeded collision (pair A: Brent rho at 2^32.35; pair B: analytic inversion of `HashLen17to32`, a 2^192 multicollision) collides for every seed. | 1 (2^0) on 2^24 seeds and 2^24 `(seed0, seed1)` pairs, both pairs; key-free | `cd cityhash-64 && ./cityhash64_verify [N or 2^k] [rng seed]` (0.3 s) |
| `farmhash-64` | FarmHash v1.1 NA (`FarmHash_64__NA`) | 8 B; 32 B; 32 B | Same final-combine bijection as CityHash: `Hash64WithSeed(m, s) = HashLen16(Hash64(m) - k2, s)`.  Pair A from a rho walk, pair B solved in closed form (`HashLen17to32` is affine-invertible in its last two words), pair C is Orson Peters' published pair. | 1 (2^0) on 2^24 seeds, all three pairs, both seed interfaces; key-free | `cd farmhash-64 && ./farmhash64_pairs [log2 N] [rng seed]` (1.5 s) |
| `murmurhash3-128` | MurmurHash3_x64_128, 2012 final (`MurmurHash3_128`, seed truncated to 32 bits) | 24 B; 32 B | A difference of exactly 2^63 in a lane survives xor, add, rotate-into-top-bit and `5x + c`; chosen word differences park `(2^63, 0)` in the state after block 1 for every seed and the tail cancels it (Aumasson-Bernstein top-bit trick); the 32-byte pair returns the state to `(0, 0)`, giving multicollisions. | 1 (2^0) on 2^24 seeds, both pairs (panel: all 2^32 seeds); key-free | `cd murmurhash3-128 && ./murmurhash3_128_verify [log2 N] [rng seed] [--exhaustive]` (1.2 s) |
| `gxhash-64` | gxhash v3, commit 55bde47 (`gxhash_64` and `gxhash`) | 24 B; 15 vs 16 B | The seed is applied only after `compress_all(m)`.  For 17..32 bytes that is `SR(SB(hv)) ^ P2(v0)` with two *public* AES rounds, inverted in constant time to cancel a flipped byte; for <= 16 bytes it is zero-padding plus the length added to every byte, so every short message has a 16-byte twin. | 1 (2^0) on 2^24 seeds, both pairs, 64- and 128-bit output; key-free | `cd gxhash-64 && ./gxhash64_verify [log2 N] [rng seed]` (0.8 s) |
| `museair` | MuseAir v0.3 (all four SMHasher3 variants) | 17 B; 24 B | Bytes 16..len-1 enter the 17..32-byte path through a public function `P(u, v)` XORed onto the bijectively loaded 16-byte head; replacing the head by `head ^ (P(T) ^ P(T'))` for any two tails gives identical state before the seed touches it. | 1 (2^0) on 2^24 seeds, both pairs, all four variants; key-free | `cd museair && ./museair_verify [log2 N] [rng seed]` (2 s) |
| `komihash` | komihash 5.34 (`komihash.h` embedded verbatim; SMHasher3 `komihash` value) | 64 B; 15 B | Pair 1: with `m1 = m0 ^ IVAL2`, `m5 = m4 ^ IVAL6` two lanes compute the same product for every seed and cancel in the fold; flipping bit 0 of both words survives unless a carry crosses the half (0.64 + 0.36 * 3/4).  Pair 2: the one seed whose S5 equals the tail word zeroes the final multiply. | 0.9106 (2^-0.135) on 2^24 seeds (panel 0.910588 at 2^32); pair 2: one weak seed, density 2^-64 | `cd komihash && ./komihash_pair [log2 N] [rng seed hex]` (0.5 s) |
| `spookyhash2-64` | SpookyHash V2 (`SpookyHash2_64`, h1 = h2 = seed) | 286 B (two pairs) | Add 2^63 to Mix word 11 of the last full block; the step-11 xor and rotate turn it into `s10 += 2^63` and `s11 += ±2^45`, cancelled by End words 10 and 11 of the tail.  The sign is bit 63 of `s11`, so pair 1 collides on one half of the seeds and pair 2 on the other. | 0.4999 (2^-1.000) and 0.5001 on 2^24 seeds; exactly one of the two pairs collides for every seed | `cd spookyhash2-64 && ./spookyhash2_64_pair [log2 N] [rng seed] [seed mode 0/1]` (4 s) |
| `rust-ahash` | aHash 0.8.12 (`rust_ahash` AES path, `rust_ahash_fb` fallback) | 56 B; 16 B | A: a 3-S-box differential trail through `aesdec` plus shuffle-add, byte differences chosen so InvShiftRows/InvMixColumns leave one byte per block to cancel (4/256 * 4/256 * 2/256 * carries).  B: complementing both words of a one-block message makes the halves of the folded 128-bit product move oppositely. | A: 26/2^24 = 2^-19.3 (panel 2^-19.74); B: 7/2^30 = 2^-27.2 in both secret models (panel 2^-26.2; discrepancy noted in the README) | `cd rust-ahash && ./ahash_pairs [log2 N] [rng seed] [A or B]` (2 s; `30 1 B` for pair B) |
| `t1ha2-64` | t1ha v2.1 `t1ha2_atonce` (`t1ha2_64`) | 16 vs 11 B; 13 vs 16 B | For 9..16 bytes only two words are absorbed; a signed-digit difference on the public word-0 product is cancelled by the tail word's carries through the `P1` multiply when 26 (27) bits of `L` and four carry signs take fixed values.  `L` is a bijection of the seed, so the class is sampled exactly. | class density 2^-26 (2^-27) x conditional rate 2^-4.00 (2^-4.01) on 2^24 class seeds = 2^-30.00 (2^-31.01), a lower bound; 0 uniform hits at 2^24 as expected | `cd t1ha2-64 && ./t1ha2_64_verify [log2 N] [rng seed]` (1.3 s) |
| `a5hash` | a5hash v5.21 (`a5hash`, `a5hash_128`) | 23 B; 8 B; 25 B (128-bit) | The seed expansion `umul128` has thousands of preimage seeds for a well-chosen `s1_0`; those seeds zero the first block's operand (pair 1) or the tail multiply (pair 2, both hashes 0).  Pair 3: on the 128-bit 17..32-byte path the last 16 bytes enter only through a product with *public* constants. | 156800/2^64 = 2^-46.74 and 7291/2^64 = 2^-51.17 (classes enumerated exactly, every member collides); pair 3: 1 (2^0) on 2^24 seeds | `cd a5hash && ./a5hash_verify [log2 N] [rng seed]` (0.5 s) |
| `highwayhash` | HighwayHash, frozen (`c/highwayhash.c` embedded verbatim; `HighwayHash_64/128/256`) | 96 B (three packets) | In the key class `hi32(key[0]) = 0xdbe6d5d5` packet 1's lane-0 multiply is 0, so a +2^8 lane-0 difference survives as one byte; packet 2's -(2^9 + 2^24) collapses it when a 32-bit relation of the packet-2 multiplier holds (event E_2, Pr = 235 * 239 / 2^40) and packet 3's +2^8 cancels it: full 1024-bit state collision at every width. | class density 2^-32 x conditional 2^-24.22 = 2^-56.22 over uniform 256-bit keys; E_2 screen 47/2^30 = 2^-24.45 measured, every hit a full-state collision | `cd highwayhash && ./highwayhash_verify [log2 N] [rng seed] [sm3]` (16 s) |

Timings are single-threaded on an Apple M2 Pro for the default 2^24 seeds.  Every program
takes `[log2 N]` as its first argument except `cityhash-64`, which takes `N` or `2^k`.  Each
README gives the exact variant and source, the mechanism in full, the build line, and the
program's expected output; the sub-directory READMEs and programs are the reference, this file
is the index.

## Building and checking

    make            # builds every directory's program in place
    make check      # runs every program with 2^20 seeds and compares its collision counts
                    # with the table below; exit status 0 only if all eleven agree
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
`-maes` (x86-64) for the two programs with a hardware-AES path (`gxhash-64`, `rust-ahash`);
both also build and validate without those flags.  `CC`, `CFLAGS` and `LDLIBS` can be
overridden on the command line.  The whole `make check` takes about five seconds.

## make check reference

Expected collision counts at 2^20 seeds, in the order the program prints them.  The
Makefile parses this table: keep the directory in column 1, the command in column 2 and the
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
| `rust-ahash` | `./ahash_pairs 20` | `1 2 0 0` |
| `spookyhash2-64` | `./spookyhash2_64_pair 20` | `524207 524207 524369 524369 1048576` |
| `t1ha2-64` | `./t1ha2_64_verify 20` | `0 65629 0 65159` |

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
* `rust-ahash`: pair A in models rs4 and smh, pair B in rs4 and smh (2^-26 is invisible at 2^20).
* `spookyhash2-64`: pair 1 64-bit and 128-bit, pair 2 64-bit and 128-bit, seeds on which the
  bit-63 predictor was right (all).
* `t1ha2-64`: pair A uniform (2^-30 is invisible at 2^20), pair A class, pair B uniform, pair
  B class.

The counts that are exactly 0 or exactly N would be the same for any RNG seed; the others
(`komihash`, `spookyhash2-64`, `rust-ahash`, `t1ha2-64`, and the `highwayhash` screen) are
specific to the fixed default seed, which is what makes the check exact.

## What every program has in common

* **Validation before anything else.**  The SMHasher3 verification value of the exact
  registered variant is recomputed with the `lib/Hashinfo.cpp` `_ComputedVerifyImpl`
  procedure (keys `{}`, `{0}`, `{0,1}`, … of length 0..255 with seed `256 - i`, the
  little-endian outputs concatenated and hashed with seed 0, first four bytes little-endian).
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

* **Our code is MIT.**  Every program (`*_verify.c`, `*_pairs.c`, `*_pair.c`), every README
  and this file: Copyright (c) 2026 Thomas Dybdahl Ahle, MIT License.  Each source file carries
  the full MIT text in its header.
* **komihash** (`komihash/komihash_pair.c`) embeds `komihash.h` release 5.34 **verbatim** (MIT,
  Copyright (c) 2021-2026 Aleksey Vaneev; sha256 of the embedded block equals upstream's, see that
  README).  Its license notice is inside the embedded block.
* **HighwayHash** (`highwayhash/highwayhash_verify.c`) embeds the reference `c/highwayhash.c`
  **verbatim** (Apache License 2.0, Copyright 2017 Google Inc.), with its first line (an
  `#include`) replaced by a comment; the Apache notice and the statement of that one
  modification are in the file header.
* The other hashes are re-implemented from their references and validated against them.  Where
  the code is close enough to count as a derivative the upstream copyright line is reproduced
  next to ours: CityHash (Google, Inc. 2011, MIT), FarmHash (Google, Inc. 2014 and Frank J. T.
  Wojcik 2021-2022, MIT), gxhash (Frank J. T. Wojcik 2025 and Olivier Giniaux 2023, MIT), a5hash
  (Aleksey Vaneev 2025 and Frank J. T. Wojcik 2021-2025, MIT), aHash (Tom Kaitchuck 2018, MIT OR
  Apache-2.0, two constants), and t1ha (Positive Technologies / Leonid Yuriev 2016-2020, zlib:
  the self-check table, test pattern and probe schedule, with the zlib notice reproduced).
  MurmurHash3 and SpookyHash are public domain; MuseAir is CC0 1.0.
