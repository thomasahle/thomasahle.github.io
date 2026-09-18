# xxh3-128: standalone fixed-pair reproduction

XXH3-128, xxHash 0.8.3, default public secret/constants, uniformly sampled 64-bit API seeds. Pairs: F (32 B).

## Build and run

```sh
cc -O2 -std=c11 -o xxh3_128_verify xxh3_128_verify.c -lm
./xxh3_128_verify                 # default 2^20-seed smoke run
./xxh3_128_verify 20 1            # explicit sample size and RNG seed
./xxh3_128_verify 30 1            # optional larger sample, a different stream from historical records
```

Arguments: `[log2 N (0..40), default 20] [RNG seed, default 1]`.
Each pair receives exactly N seeds. The program runs in one thread, reads no files,
and writes only stdout/stderr. GCC/Clang C11 and libm suffice; the transcribed
multiplication code uses the `unsigned __int128` extension.

## Implementation and startup validation

The C file embeds `../../ref/xxhash.h` **verbatim** with `XXH_INLINE_ALL` and calls `XXH3_128bits_withSeed`. No external header or library is required. The embedded version must be 803. Header SHA-256:

```text
17973c0dc49d9854ca26caa191f0e12f7a424b68858d9a78de3860d959d85e4b
```

Startup recomputes SMHasher3 verification **0x288DAA94**, read from SMHasher3 `hashes/xxhash.cpp` (commit 7ad8939d). Verification encodes outputs in canonical big-endian order (high half first for 128 bits), matching that wrapper.

The SMHasher3 check hashes byte prefixes of lengths 0..255 with seeds 256..1, concatenates the encoded outputs, hashes that array with seed 0, and reads the first four output bytes little-endian. It therefore exercises short and long input paths. No seed fixup is applied.

Before sampling, each recorded colliding seed is hashed on both messages and checked
against its literal expected output. A mismatch exits nonzero. These witnesses are
separate from the random sample and never added to its collision count.

## Pairs and historical measurements

Pair F starts with complementary words w1 = ~w0. Complementing both swaps them and preserves their sum. The program compares BOTH low64 and high64; a match of just one half is not counted. Printed 128-bit values concatenate high64 then low64.

Historical measurements supplied with the package: **5 / 2^30 = 2^-27.678072 (full 128-bit collisions)**.
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
`make check` compares the exact deterministic 2^20 counts, in printed order: **`1`**.

The following EXPECTED block is the actual local default run, also saved in
`run_2p20.txt`:

```text
SMHasher3 XXH3-128 0.8.3: 288DAA94 expected 288DAA94 PASS

paper pair F / XXH3-128 0.8.3
M (32 B) = 9bd4604137366abe642b9fbec8c9954188d35499de169df633e0964e8c04600c
M' (32 B) = 642b9fbec8c995419bd4604137366abe88d35499de169df633e0964e8c04600c
recorded colliding seed e130d569418d2efe: H(M)=77aee7ca6253510cff9994fe21ab989a H(M')=77aee7ca6253510cff9994fe21ab989a
collisions = 1 / 1048576; rate = 9.53674316406e-07; log2(rate) = -20.000000; sampled score = 22.000000
first sampled colliding seed 2ac74c721ddeaca6: H(M)=9ce669c18ec555215a4b197d8fef97d4 H(M')=9ce669c18ec555215a4b197d8fef97d4
```

## Attribution and licensing

The embedded xxHash header retains Yann Collet’s copyright and full BSD 2-Clause license verbatim. Driver additions are Copyright (c) 2026 Thomas Dybdahl Ahle, MIT, with the full notice in the C file.
The supplied `collision_driver.cpp`, `harness/hashes.h`, and JSON evidence records
are credited as the sources of the implementation and reproduction data.

Rerun 2026-09-18 with this program on a Xeon 8375C: arguments `30 1` … `30 8` (built against xxHash dev 6cc7b4b, XXH3 core functionally identical to 0.8.3) gave 10, 12, 11, 10, 11, 4, 12, 16 = 86 / 2^33; arguments `30 9` … `30 16` (this embedded 0.8.3 header) gave 10, 11, 9, 11, 8, 8, 12, 10 = 79 / 2^33; pooled 165 / 2^34 = 2^-26.634. Counts are deterministic per (log2 N, RNG seed): rerunning `30 1` must print exactly 10.

The logs are in ../../records/fairness-pass/xxh3-128/runs/ and ../../records/fairness-pass/xxh3-128-verify/. The larger sample replaces the selected estimate; the original five-event run remains supporting evidence.
