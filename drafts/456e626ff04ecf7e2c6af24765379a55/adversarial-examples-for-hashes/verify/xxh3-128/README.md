# xxh3-128: standalone fixed-pair reproduction

XXH3-128, xxHash 0.8.3. Pairs: F (32 B). Two key models: `default` (uniformly sampled 64-bit API seed through `XXH3_128bits_withSeed`, public default secret; the historical experiment and the article's default-secret control) and `random-secret` (a fresh uniform 192-byte secret per trial through `XXH3_128bits_withSecret`, 1536 hidden bits; the model the article scores, added 2026-09-19).

## Build and run

```sh
cc -O2 -std=c11 -o xxh3_128_verify xxh3_128_verify.c -lm
./xxh3_128_verify                 # default 2^20-seed smoke run
./xxh3_128_verify 20 1            # explicit sample size and RNG seed
./xxh3_128_verify 30 1            # optional larger sample, a different stream from historical records
./xxh3_128_verify 20 1 random-secret   # random-secret model: fresh 192-byte secret per trial
./xxh3_128_verify 30 1 random-secret   # 2^30 keys, about 3 min
```

Arguments: `[log2 N (0..40), default 20] [RNG seed, default 1] [default|random-secret]`.
Each pair receives exactly N seeds. The program runs in one thread, reads no files,
and writes only stdout/stderr. GCC/Clang C11 and libm suffice; the transcribed
multiplication code uses the `unsigned __int128` extension.

## Implementation and startup validation

The C file embeds `../../ref/xxhash.h` **verbatim** with `XXH_INLINE_ALL` and calls `XXH3_128bits_withSeed` (default mode) or `XXH3_128bits_withSecret` with a 192-byte secret (random-secret mode). No external header or library is required. The embedded version must be 803. Header SHA-256:

```text
17973c0dc49d9854ca26caa191f0e12f7a424b68858d9a78de3860d959d85e4b
```

Startup recomputes SMHasher3 verification **0x288DAA94**, read from SMHasher3 `hashes/xxhash.cpp` (commit 7ad8939d). Verification encodes outputs in canonical big-endian order (high half first for 128 bits), matching that wrapper.

The SMHasher3 check hashes byte prefixes of lengths 0..255 with seeds 256..1, concatenates the encoded outputs, hashes that array with seed 0, and reads the first four output bytes little-endian. It therefore exercises short and long input paths. No seed fixup is applied.

Before sampling, each recorded colliding seed is hashed on both messages and checked
against its literal expected output. A mismatch exits nonzero. These witnesses are
separate from the random sample and never added to its collision count. In `random-secret`
mode the program additionally asserts the recorded random-model witness: the 192-byte secret
`f55eeab0159e547f...e8d568313cc9a5e7` (full hex in the C file) hashes both messages to
`high64 = ced3d509b74eee89, low64 = 033e7fcc84653051` through `XXH3_128bits_withSecret`
(from the 2026-09-19 measurement, `../../records/witness-searches/random-secret/xxh3-128/verify/`,
re-checked there through the non-inlined library).

## Pairs and historical measurements

Pair F starts with complementary words w1 = ~w0. Complementing both swaps them and preserves their sum. The program compares BOTH low64 and high64; a match of just one half is not counted. Printed 128-bit values concatenate high64 then low64.

Historical default-secret measurement supplied with the package: **5 / 2^30 = 2^-27.678072 (full 128-bit collisions)**;
the 2026-09-18 default-secret rerun below gave 165 / 2^34. The article's score uses the random-secret model:
pooled **479 / 2^35.46 = 2^-26.56**, cap 28.6 bits [28.4, 28.7] (independent verifier 156/2^34 through
`withSecret`, 84/2^33 with seed and secret both random, 60/2^32 on a second stream; search harness 179/2^34),
recorded in `../../records/witness-searches/random-secret/xxh3-128/`. The event `fold(A, B) = fold(~A, ~B)` on
both halves does not use the secret value, so the two models give the same rate. `XXH3_128bits_withSecretandSeed`
ignores the custom secret for inputs of at most 240 bytes; the random-secret mode checks this trial by trial.
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
`random-secret` mode each trial draws a seed (used only by the `withSecretandSeed` control) and
then 24 secret words from the same stream. Each pair restarts the same stream; multiple pair
results are deliberately correlated.
`make check` compares the exact deterministic 2^20 counts of the default mode, in printed order: **`1`**.
The random-secret smoke run `./xxh3_128_verify 20 1 random-secret` prints **`0`** collisions and a
passing `withSecretandSeed` control (`run_2p20_random_secret.txt`); the 2^30 run
`./xxh3_128_verify 30 1 random-secret` printed **15 / 2^30 = 2^-26.09** full 128-bit collisions on the
Xeon, with the control reporting 11 collisions in the default-secret `withSeed` stream and zero outputs
differing between `withSecretandSeed` and `withSeed` (`run_2p30_random_secret.txt`, 2026-09-19,
`nice -n 10 taskset -c 24-31`); both counts are consistent with the pooled 2^-26.56 above.

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

Random-secret mode, 2^30 keys on the Xeon (`run_2p30_random_secret.txt`, witness and secret lines abbreviated):

```text
recorded colliding key (random-secret model) 192-byte secret f55eeab0159e547f...e8d568313cc9a5e7: H(M)=ced3d509b74eee89033e7fcc84653051 H(M')=ced3d509b74eee89033e7fcc84653051
key model: uniform 192-byte secret per trial (24 words) through XXH3_128bits_withSecret (1536 bits); withSecretandSeed control with an independent uniform seed
collisions = 15 / 1073741824; rate = 1.39698386192e-08; log2(rate) = -26.093109; sampled score = 28.093109
first sampled colliding secret dcfb60af595fa2cf...e864669d60c00ed3: H(M)=e32a6a808f7393217de8f14871d0087b H(M')=e32a6a808f7393217de8f14871d0087b
withSecretandSeed control: collisions = 11 / 1073741824 with the default-secret withSeed stream; outputs differing from withSeed: 0 PASS (custom secret ignored at len<=240)
```

## Attribution and licensing

The embedded xxHash header retains Yann Collet’s copyright and full BSD 2-Clause license verbatim. Driver additions are Copyright (c) 2026 Thomas Dybdahl Ahle, MIT, with the full notice in the C file.
The supplied `collision_driver.cpp`, `harness/hashes.h`, and JSON evidence records
are credited as the sources of the implementation and reproduction data.

Rerun 2026-09-18 with this program on a Xeon 8375C: arguments `30 1` … `30 8` (built against xxHash dev 6cc7b4b, XXH3 core functionally identical to 0.8.3) gave 10, 12, 11, 10, 11, 4, 12, 16 = 86 / 2^33; arguments `30 9` … `30 16` (this embedded 0.8.3 header) gave 10, 11, 9, 11, 8, 8, 12, 10 = 79 / 2^33; pooled 165 / 2^34 = 2^-26.634. Counts are deterministic per (log2 N, RNG seed): rerunning `30 1` must print exactly 10.

The logs are in ../../records/fairness-pass/xxh3-128/runs/ and ../../records/fairness-pass/xxh3-128-verify/. The larger sample replaces the selected estimate; the original five-event run remains supporting evidence.
