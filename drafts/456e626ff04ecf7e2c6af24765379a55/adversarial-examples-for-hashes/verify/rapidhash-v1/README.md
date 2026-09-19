# rapidhash-v1: standalone fixed-pair reproduction

rapidhash v1.0. Pairs: A (32 B). Two key models: `default` (uniformly sampled 64-bit API seed, compiled-in three-word secret; the historical experiment) and `random-secret` (uniform seed and three uniform secret words per trial through the transcribed `rapidhash_internal(key, len, seed, secret)`, 256 hidden bits; the model the article scores, added 2026-09-19).

## Build and run

```sh
cc -O2 -std=c11 -o rapidhash_v1_verify rapidhash_v1_verify.c -lm
./rapidhash_v1_verify                 # default 2^20-seed smoke run
./rapidhash_v1_verify 20 1            # explicit sample size and RNG seed
./rapidhash_v1_verify 30 1            # optional larger sample, a different stream from historical records
./rapidhash_v1_verify 20 1 random-secret   # random-secret model: seed + three secret words per trial
./rapidhash_v1_verify 30 1 random-secret   # 2^30 keys, about 30 s
```

Arguments: `[log2 N (0..40), default 20] [RNG seed, default 1] [default|random-secret]`.
Each pair receives exactly N seeds. The program runs in one thread, reads no files,
and writes only stdout/stderr. GCC/Clang C11 and libm suffice; the transcribed
multiplication code uses the `unsigned __int128` extension.

## Implementation and startup validation

Complete C transcription of `rapidhash_ref` from `../../harness/hashes.h`, with explicit little-endian loads, the default public three-word secret, and the v1.0 fast multiplication path.

Startup checks `"message digest"`, length 14, seed 3, against **0x0031cdc21324150f**. This literal was obtained by running the pre-existing `source/fast-polynomials/tools/bench/adversarial/selftest` and inspecting its `selftest.cpp` source. The recorded pair output below is also asserted. The local SMHasher3 `rapidhash` registration is v3, so its verification constant is not used for v1.

Before sampling, each recorded colliding seed is hashed on both messages and checked
against its literal expected output. A mismatch exits nonzero. These witnesses are
separate from the random sample and never added to its collision count. In `random-secret`
mode the program additionally asserts the recorded random-model witness: seed
`3879cdfddc782ad3` with secret words `d0d81d65fd961dff, a9132a2f5b5d4f54, d72e7f4d4f7270f5`
hashes both messages to `4329ec0defb7f826` (from the 2026-09-19 measurement,
`../../records/witness-searches/random-secret/rapid1/`).

## Pairs and historical measurements

Pair A complements both of the first two little-endian words and keeps the final 16 bytes fixed. For some seeds the two multiplication products have equal high-half XOR low-half folds, merging the states before the common suffix.

Historical default-secret measurement supplied with the package: **12 / 2^30 = 2^-26.415037**.
The article's score uses the random-secret model: pooled **160 / 2^34 = 2^-26.68**, cap 28.7 bits
[28.5, 28.9] (search harness 38/2^32; independent verifier 40/2^32 and 82/2^33 through the upstream
v1.0 header), recorded in `../../records/witness-searches/random-secret/rapid1/`. The fold
differential never uses the secret value, so the two models give the same rate; the compiled-in
secret additionally admits the every-seed annihilation of issue #10 (the word at len-16 equal to
secret[1]), which the random-secret model excludes.
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
default. Seeds use all 64 bits. In `default` mode no secret words are randomized; in
`random-secret` mode each trial draws the seed and then the three secret words from the same
stream. Each pair restarts the same stream; multiple pair results are deliberately correlated.
`make check` compares the exact deterministic 2^20 counts of the default mode, in printed order: **`0`**.
The random-secret smoke run `./rapidhash_v1_verify 20 1 random-secret` prints **`0`** collisions
(`run_2p20_random_secret.txt`); the 2^30 run `./rapidhash_v1_verify 30 1 random-secret` printed
**16 / 2^30 = 2^-26.0** on the Xeon (`run_2p30_random_secret.txt`, 2026-09-19, `nice -n 10
taskset -c 24-31`), within the interval of the pooled 2^-26.68 above.

The following EXPECTED block is the actual local default run, also saved in
`run_2p20.txt`:

```text
harness message digest, seed 3: 0031cdc21324150f expected 0031cdc21324150f PASS
key model: uniform 64-bit API seed; shipped public secret words (default)

paper pair A / rapidhash v1.0
M (32 B) = 9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c
M' (32 B) = 642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c
recorded colliding seed 3788f2419a81e2d6: H(M)=8f7a71ffebd4a14b H(M')=8f7a71ffebd4a14b
collisions = 0 / 1048576; rate = 0; log2(rate) = -inf (zero hits; no population-rate estimate)
no sampled collision; the recorded witness above was checked separately
```

Random-secret mode, 2^30 keys on the Xeon (`run_2p30_random_secret.txt`):

```text
key model: uniform 64-bit seed and three uniform 64-bit secret words per trial (256 bits), rapidhash_internal(key,len,seed,secret)
recorded colliding key (random-secret model) seed 3879cdfddc782ad3 secret d0d81d65fd961dff,a9132a2f5b5d4f54,d72e7f4d4f7270f5: H(M)=4329ec0defb7f826 H(M')=4329ec0defb7f826
collisions = 16 / 1073741824; rate = 1.49011611938e-08; log2(rate) = -26.000000; sampled score = 28.000000
first sampled colliding seed 026addb8f92f83ed secret c50fafef0c9a2104,f1f68dfeb46c85c0,b6e8720c836a00ed: H(M)=80978569e905b172 H(M')=80978569e905b172
```

## Attribution and licensing

rapidhash v1.0 is Copyright (c) 2024 Nicolas De Carli, MIT, based on wyhash by Wang Yi. The full MIT terms and attribution are included in the C file; driver additions are Copyright (c) 2026 Thomas Dybdahl Ahle, MIT.
The supplied `collision_driver.cpp`, `harness/hashes.h`, and JSON evidence records
are credited as the sources of the implementation and reproduction data.
