# rapidhash-v1: standalone fixed-pair reproduction

rapidhash v1.0, default public secret/constants, uniformly sampled 64-bit API seeds. Pairs: A (32 B).

## Build and run

```sh
cc -O2 -std=c11 -o rapidhash_v1_verify rapidhash_v1_verify.c -lm
./rapidhash_v1_verify                 # default 2^20-seed smoke run
./rapidhash_v1_verify 20 1            # explicit sample size and RNG seed
./rapidhash_v1_verify 30 1            # optional larger sample, a different stream from historical records
```

Arguments: `[log2 N (0..40), default 20] [RNG seed, default 1]`.
Each pair receives exactly N seeds. The program runs in one thread, reads no files,
and writes only stdout/stderr. GCC/Clang C11 and libm suffice; the transcribed
multiplication code uses the `unsigned __int128` extension.

## Implementation and startup validation

Complete C transcription of `rapidhash_ref` from `../../harness/hashes.h`, with explicit little-endian loads, the default public three-word secret, and the v1.0 fast multiplication path.

Startup checks `"message digest"`, length 14, seed 3, against **0x0031cdc21324150f**. This literal was obtained by running the pre-existing `/Users/ahle/repos/fast-polynomials/tools/bench/adversarial/selftest` and inspecting its `selftest.cpp` source. The recorded pair output below is also asserted. The local SMHasher3 `rapidhash` registration is v3, so its verification constant is not used for v1.

Before sampling, each recorded colliding seed is hashed on both messages and checked
against its literal expected output. A mismatch exits nonzero. These witnesses are
separate from the random sample and never added to its collision count.

## Pairs and historical measurements

Pair A complements both of the first two little-endian words and keeps the final 16 bytes fixed. For some seeds the two multiplication products have equal high-half XOR low-half folds, merging the states before the common suffix.

Historical measurements supplied with the package: **12 / 2^30 = 2^-26.415037**.
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
`make check` compares the exact deterministic 2^20 counts, in printed order: **`0`**.

The following EXPECTED block is the actual local default run, also saved in
`run_2p20.txt`:

```text
harness message digest, seed 3: 0031cdc21324150f expected 0031cdc21324150f PASS

paper pair A / rapidhash v1.0
M (32 B) = 9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c
M' (32 B) = 642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c
recorded colliding seed 3788f2419a81e2d6: H(M)=8f7a71ffebd4a14b H(M')=8f7a71ffebd4a14b
collisions = 0 / 1048576; rate = 0; log2(rate) = -inf (zero hits; no population-rate estimate)
no sampled collision; the recorded witness above was checked separately
```

## Attribution and licensing

rapidhash v1.0 is Copyright (c) 2024 Nicolas De Carli, MIT, based on wyhash by Wang Yi. The full MIT terms and attribution are included in the C file; driver additions are Copyright (c) 2026 Thomas Dybdahl Ahle, MIT.
The supplied `collision_driver.cpp`, `harness/hashes.h`, and JSON evidence records
are credited as the sources of the implementation and reproduction data.
