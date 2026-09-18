# wyhash: standalone fixed-pair reproduction

wyhash final v4.3, default public secret/constants, uniformly sampled 64-bit API seeds. Pairs: A (32 B).

## Build and run

```sh
cc -O2 -std=c11 -o wyhash_verify wyhash_verify.c -lm
./wyhash_verify                 # default 2^20-seed smoke run
./wyhash_verify 20 1            # explicit sample size and RNG seed
./wyhash_verify 30 1            # optional larger sample, a different stream from historical records
```

Arguments: `[log2 N (0..40), default 20] [RNG seed, default 1]`.
Each pair receives exactly N seeds. The program runs in one thread, reads no files,
and writes only stdout/stderr. GCC/Clang C11 and libm suffice; the transcribed
multiplication code uses the `unsigned __int128` extension.

## Implementation and startup validation

Complete C transcription of `wyhash_ref` from `../../harness/hashes.h`, with explicit little-endian loads and the default public four-word secret. Configuration: final v4.3, `WYHASH_CONDOM=1`, 64-by-64-bit multiplication.

Startup recomputes SMHasher3 `wyhash` verification **0x9DAE7DD3**, read from the local `source/smhasher3-mr-seeddiff/hashes/wyhash.cpp`. That registration is labelled v4.2; the supplied final-v4.3 implementation reproduces its value. Output encoding is little-endian. It also checks `"message digest"`, length 14, seed 3, against **0x786d1f1df3801df4**, confirmed by the local harness selftest.

The SMHasher3 check hashes byte prefixes of lengths 0..255 with seeds 256..1, concatenates the encoded outputs, hashes that array with seed 0, and reads the first four output bytes little-endian. It therefore exercises short and long input paths. No seed fixup is applied.

Before sampling, each recorded colliding seed is hashed on both messages and checked
against its literal expected output. A mismatch exits nonzero. These witnesses are
separate from the random sample and never added to its collision count.

## Pairs and historical measurements

Pair A complements both of the first two little-endian words and leaves the remaining 16 bytes unchanged. A collision in the XOR fold of the 128-bit product merges the two states; the identical suffix then preserves equality.

Historical measurements supplied with the package: **9 / 2^30 = 2^-26.830075**.
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
SMHasher3 wyhash final v4.3: 9DAE7DD3 expected 9DAE7DD3 PASS
harness message digest, seed 3: 786d1f1df3801df4 expected 786d1f1df3801df4 PASS

paper pair A / wyhash final v4.3
M (32 B) = 9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c
M' (32 B) = 642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c
recorded colliding seed 131b854bbd1f5b12: H(M)=a7dd61b404363777 H(M')=a7dd61b404363777
collisions = 0 / 1048576; rate = 0; log2(rate) = -inf (zero hits; no population-rate estimate)
no sampled collision; the recorded witness above was checked separately
```

## Attribution and licensing

wyhash is attributed to Wang Yi; the local upstream source declares Unlicense/public domain. The C transcription and reproduction driver carry the full MIT notice. This distinguishes the requested MIT transcription from the upstream license.
The supplied `collision_driver.cpp`, `harness/hashes.h`, and JSON evidence records
are credited as the sources of the implementation and reproduction data.
