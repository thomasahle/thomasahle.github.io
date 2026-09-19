# wyhash: standalone fixed-pair reproduction

wyhash final v4.3. Pairs: A (32 B). Two key models: `default` (uniformly sampled 64-bit API seed, shipped public secret words; the historical experiment) and `random-secret` (uniform seed and four uniform secret words per trial through `wyhash(key, len, seed, secret)`, 320 hidden bits; the model the article scores, added 2026-09-19).

## Build and run

```sh
cc -O2 -std=c11 -o wyhash_verify wyhash_verify.c -lm
./wyhash_verify                 # default 2^20-seed smoke run
./wyhash_verify 20 1            # explicit sample size and RNG seed
./wyhash_verify 30 1            # optional larger sample, a different stream from historical records
./wyhash_verify 20 1 random-secret   # random-secret model: seed + four secret words per trial
./wyhash_verify 30 1 random-secret   # 2^30 keys, about 30 s
```

Arguments: `[log2 N (0..40), default 20] [RNG seed, default 1] [default|random-secret]`.
Each pair receives exactly N seeds. The program runs in one thread, reads no files,
and writes only stdout/stderr. GCC/Clang C11 and libm suffice; the transcribed
multiplication code uses the `unsigned __int128` extension.

## Implementation and startup validation

Complete C transcription of `wyhash_ref` from `../../harness/hashes.h`, with explicit little-endian loads and the default public four-word secret. Configuration: final v4.3, `WYHASH_CONDOM=1`, 64-by-64-bit multiplication.

Startup recomputes SMHasher3 `wyhash` verification **0x9DAE7DD3**, read from the local `source/smhasher3-mr-seeddiff/hashes/wyhash.cpp`. That registration is labelled v4.2; the supplied final-v4.3 implementation reproduces its value. Output encoding is little-endian. It also checks `"message digest"`, length 14, seed 3, against **0x786d1f1df3801df4**, confirmed by the local harness selftest.

The SMHasher3 check hashes byte prefixes of lengths 0..255 with seeds 256..1, concatenates the encoded outputs, hashes that array with seed 0, and reads the first four output bytes little-endian. It therefore exercises short and long input paths. No seed fixup is applied.

Before sampling, each recorded colliding seed is hashed on both messages and checked
against its literal expected output. A mismatch exits nonzero. These witnesses are
separate from the random sample and never added to its collision count. In `random-secret`
mode the program additionally asserts the recorded random-model witness: seed
`6c58e2bbe0b8c2ef` with secret words `2e87eccce404b9e7, 27807013cee858cb, 064c5e57e4a52845,
bda1eaa3841f1235` hashes both messages to `bf0d6eb792755aad` (from the 2026-09-19 measurement,
`../../records/witness-searches/random-secret/wyhash/`).

## Pairs and historical measurements

Pair A complements both of the first two little-endian words and leaves the remaining 16 bytes unchanged. A collision in the XOR fold of the 128-bit product merges the two states; the identical suffix then preserves equality.

Historical default-secret measurement supplied with the package: **9 / 2^30 = 2^-26.830075**.
The article's score uses the random-secret model: pooled **1088 / 3·2^35 = 2^-26.50**, cap 28.5 bits
[28.4, 28.6] (search harness 357/2^35; independent verifier 388 and 343 per 2^35), recorded in
`../../records/witness-searches/random-secret/wyhash/`. The mechanism, `fold(X, Y) = fold(~X, ~Y)` on
the first message product, never uses the secret value, so the two models give the same rate; the
shipped secret additionally admits every-seed pairs (a message word equal to secret[1]) and a
one-in-three class (w0 = 0x5555555555555555 ^ secret[1]), which the random-secret model excludes.
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
`random-secret` mode each trial draws the seed and then the four secret words from the same
stream. Each pair restarts the same stream; multiple pair results are deliberately correlated.
`make check` compares the exact deterministic 2^20 counts of the default mode, in printed order: **`0`**.
The random-secret smoke run `./wyhash_verify 20 1 random-secret` prints **`0`** collisions
(`run_2p20_random_secret.txt`); the 2^30 run `./wyhash_verify 30 1 random-secret` printed
**12 / 2^30 = 2^-26.415** on the Xeon (`run_2p30_random_secret.txt`, 2026-09-19, `nice -n 10
taskset -c 24-31`), consistent with the pooled 2^-26.50 above.

The following EXPECTED block is the actual local default run, also saved in
`run_2p20.txt`:

```text
SMHasher3 wyhash final v4.3: 9DAE7DD3 expected 9DAE7DD3 PASS
harness message digest, seed 3: 786d1f1df3801df4 expected 786d1f1df3801df4 PASS
key model: uniform 64-bit API seed; shipped public secret words (default)

paper pair A / wyhash final v4.3
M (32 B) = 9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c
M' (32 B) = 642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c
recorded colliding seed 131b854bbd1f5b12: H(M)=a7dd61b404363777 H(M')=a7dd61b404363777
collisions = 0 / 1048576; rate = 0; log2(rate) = -inf (zero hits; no population-rate estimate)
no sampled collision; the recorded witness above was checked separately
```

Random-secret mode, 2^30 keys on the Xeon (`run_2p30_random_secret.txt`):

```text
key model: uniform 64-bit seed and four uniform 64-bit secret words per trial (320 bits), wyhash(key,len,seed,secret)
recorded colliding key (random-secret model) seed 6c58e2bbe0b8c2ef secret 2e87eccce404b9e7,27807013cee858cb,064c5e57e4a52845,bda1eaa3841f1235: H(M)=bf0d6eb792755aad H(M')=bf0d6eb792755aad
collisions = 12 / 1073741824; rate = 1.11758708954e-08; log2(rate) = -26.415037; sampled score = 28.415037
first sampled colliding seed fc6d0a7af7b68145 secret ab42970e95de9e00,c0924c9b4cace882,c08758c29463b9b9,6ed4e8b045a434ac: H(M)=83a9ff23e362154f H(M')=83a9ff23e362154f
```

## Attribution and licensing

wyhash is attributed to Wang Yi; the local upstream source declares Unlicense/public domain. The C transcription and reproduction driver carry the full MIT notice. This distinguishes the requested MIT transcription from the upstream license.
The supplied `collision_driver.cpp`, `harness/hashes.h`, and JSON evidence records
are credited as the sources of the implementation and reproduction data.
