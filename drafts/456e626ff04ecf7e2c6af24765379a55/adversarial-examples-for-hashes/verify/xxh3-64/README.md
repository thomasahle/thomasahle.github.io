# xxh3-64: standalone fixed-pair reproduction

XXH3-64, xxHash 0.8.3, default public secret/constants, uniformly sampled 64-bit API seeds. Pairs: A (32 B); base-1143 (128 B).

## Build and run

```sh
cc -O2 -std=c11 -o xxh3_64_verify xxh3_64_verify.c -lm
./xxh3_64_verify                 # default 2^20-seed smoke run
./xxh3_64_verify 20 1            # explicit sample size and RNG seed
./xxh3_64_verify 30 1            # optional larger sample, a different stream from historical records
```

Arguments: `[log2 N (0..40), default 20] [RNG seed, default 1]`.
Each pair receives exactly N seeds. The program runs in one thread, reads no files,
and writes only stdout/stderr. GCC/Clang C11 and libm suffice; the transcribed
multiplication code uses the `unsigned __int128` extension.

## Implementation and startup validation

The C file embeds `../../ref/xxhash.h` **verbatim** with `XXH_INLINE_ALL` and calls `XXH3_64bits_withSeed`. No external header or library is required. The embedded version must be 803. Header SHA-256:

```text
17973c0dc49d9854ca26caa191f0e12f7a424b68858d9a78de3860d959d85e4b
```

Startup recomputes SMHasher3 verification **0x1AAEE62C**, read from the local `/Users/ahle/repos/smhasher3-mr-seeddiff/hashes/xxhash.cpp`. Verification encodes outputs in canonical big-endian order (high half first for 128 bits), matching that wrapper.

The SMHasher3 check hashes byte prefixes of lengths 0..255 with seeds 256..1, concatenates the encoded outputs, hashes that array with seed 0, and reads the first four output bytes little-endian. It therefore exercises short and long input paths. No seed fixup is applied.

It additionally checks all ten official XXH3-64 sanity vectors: lengths 12, 24, 48, 80 and 195 with seeds 0 and PRIME64 = 11400714785074694797 (`0x9e3779b185ebca8d`). These are transcribed from `/Users/ahle/repos/fast-polynomials/tools/bench/adversarial/selftest.cpp`, which credits xxHash `cli/xsum_sanity_check.c`. The buffer starts with byteGen = 2654435761; each byte is byteGen >> 56, followed by byteGen *= PRIME64 modulo 2^64.

Before sampling, each recorded colliding seed is hashed on both messages and checked
against its literal expected output. A mismatch exits nonzero. These witnesses are
separate from the random sample and never added to its collision count.

## Pairs and historical measurements

Both pairs complement their first two words and leave the suffix unchanged. In these length paths, equality of the first 16-byte multiplication fold survives the common sum and final avalanche. The exact 128-byte messages come from `../../xxh3_128b_pairs.json`: id 1143, variant 0, tail 0, length 128, stages confirm1 and confirm2 (identical messages). This is the selected base-1143 pair, not the 128-byte extension of pair A in `supplied_rows.json`.

Historical measurements supplied with the package: **A: 11 / 2^30 = 2^-26.540568; base-1143: 504 / 2^30 = 2^-21.022720 and 526 / 2^30 = 2^-20.961081**.
The 32-byte pair, seed and output literals come from `../../rows.json` (also retained
in `supplied_rows.json`). The base-1143 records, where applicable, come from
`../../xxh3_128b_pairs.json`. These historical 2^30 counts were **not rerun here**.
Their original sampling streams differ from this standalone program; running with
argument 30 estimates the same seed-domain probability but need not give the same counts.

**2^20 is only a smoke run**, not a useful estimate of these rare-event rates.
Expected hits at that size are near zero (about 0.5 for base-1143, below 0.012
for the other pairs). Zero hits reports log2(sample rate) = -inf, not evidence that
the population collision probability is zero. An isolated hit is also a noisy estimate.

## Sampling and EXPECTED output

The RNG is xoshiro256**, initialized from four splitmix64 outputs with seed 1 by
default. Seeds use all 64 bits and no secret words are randomized. Each pair restarts
the same stream; multiple pair results are deliberately correlated.
`make check` compares the exact deterministic 2^20 counts, in printed order: **`0 0`**.

The following EXPECTED block is the actual local default run, also saved in
`run_2p20.txt`:

```text
SMHasher3 XXH3-64 0.8.3: 1AAEE62C expected 1AAEE62C PASS
XXH3 sanity len=12 seed=0000000000000000: a713daf0dfbb77e7 expected a713daf0dfbb77e7 PASS
XXH3 sanity len=12 seed=9e3779b185ebca8d: e7303e1b2336de0e expected e7303e1b2336de0e PASS
XXH3 sanity len=24 seed=0000000000000000: a3fe70bf9d3510eb expected a3fe70bf9d3510eb PASS
XXH3 sanity len=24 seed=9e3779b185ebca8d: 850e80fc35bdd690 expected 850e80fc35bdd690 PASS
XXH3 sanity len=48 seed=0000000000000000: 397da259ecba1f11 expected 397da259ecba1f11 PASS
XXH3 sanity len=48 seed=9e3779b185ebca8d: adc2cbaa44acc616 expected adc2cbaa44acc616 PASS
XXH3 sanity len=80 seed=0000000000000000: bcdefbbb2c47c90a expected bcdefbbb2c47c90a PASS
XXH3 sanity len=80 seed=9e3779b185ebca8d: c6dd0cb699532e73 expected c6dd0cb699532e73 PASS
XXH3 sanity len=195 seed=0000000000000000: cd94217ee362ec3a expected cd94217ee362ec3a PASS
XXH3 sanity len=195 seed=9e3779b185ebca8d: ba68003d370cb3d9 expected ba68003d370cb3d9 PASS

paper pair A / XXH3-64 0.8.3
M (32 B) = 9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c
M' (32 B) = 642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c
recorded colliding seed cd362ec99e1e6259: H(M)=bf1f613669d6c8ae H(M')=bf1f613669d6c8ae
collisions = 0 / 1048576; rate = 0; log2(rate) = -inf (zero hits; no population-rate estimate)
no sampled collision; the recorded witness above was checked separately

base-1143 pair / XXH3-64 0.8.3
M (128 B) = 8912a3da9fc464368202a1be238b7d11a56186f8dc03bd1f78bfbd2ba3386ced7093d3639fc8f076abe3f9a757e5c84290720e6340b2e7820a16a5633b9b0344773006232e3abc7d4727eafba248b46cf2a6516698382bd2fa890528924cd1b618df4aa87d1af4bcec13f92bf8a1163e657fb0885deddf709a0252fbe3ae169d
M' (128 B) = 76ed5c25603b9bc97dfd5e41dc7482eea56186f8dc03bd1f78bfbd2ba3386ced7093d3639fc8f076abe3f9a757e5c84290720e6340b2e7820a16a5633b9b0344773006232e3abc7d4727eafba248b46cf2a6516698382bd2fa890528924cd1b618df4aa87d1af4bcec13f92bf8a1163e657fb0885deddf709a0252fbe3ae169d
recorded colliding seed c8eae1baae13330b: H(M)=448e3716c8effb94 H(M')=448e3716c8effb94
collisions = 0 / 1048576; rate = 0; log2(rate) = -inf (zero hits; no population-rate estimate)
no sampled collision; the recorded witness above was checked separately
```

## Attribution and licensing

The embedded xxHash header retains Yann Collet’s copyright and full BSD 2-Clause license verbatim. Driver additions are Copyright (c) 2026 Thomas Dybdahl Ahle, MIT, with the full notice in the C file.
The supplied `collision_driver.cpp`, `harness/hashes.h`, and JSON evidence records
are credited as the sources of the implementation and reproduction data.
