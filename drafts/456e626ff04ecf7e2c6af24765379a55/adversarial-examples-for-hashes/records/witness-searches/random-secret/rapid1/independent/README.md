# rapid1: independent verification under the random-secret model

Date: 2026-09-19.  Hash: rapidhash v1.0, upstream `rapidhash.h` fetched directly from
GitHub at tag `rapidhash_v1.0` (saved here as `rapidhash_upstream.h`, sha256
f57de5bf8eb6f0b86be9a6b3ea0893b56fd9a27709a903d21b235dbc8dc3b8b5, identical to the
searcher's copy).  Entry point: `rapidhash_internal(key, len, seed, secret)` with the
caller-supplied secret triplet.  Default build configuration (RAPIDHASH_FAST,
RAPIDHASH_UNROLLED, `__uint128_t` multiply).

Harness `rs_verify.cpp` is a fresh implementation of the sampling, written without
reference to the searcher's `rs_rapid1.cpp`: per-thread `std::mt19937_64` seeded through
`std::seed_seq` from (rngseed, thread index); each key is four consecutive 64-bit draws
= seed, secret[0], secret[1], secret[2], all uniform.  Modes: `random` (all four words
uniform), `default` (uniform seed, compiled-in `rapid_secret`), `odd` (uniform words | 1,
the paper harness's convention).  Exact Poisson (Garwood) 95% intervals.  8 threads,
Xeon 8375C, `nice -n 10 taskset -c 24-31` (cores shared with other jobs); whole pipeline
910 s.

Selftest (all PASS, `logs/00_selftest.txt`): verify-package vector
`"message digest"`, len 14, seed 3 -> 0031cdc21324150f; row witness seed
3788f2419a81e2d6 -> 8f7a71ffebd4a14b for both messages of pair A; the searcher's two
reported random-secret example keys (pair A: seed 3879cdfddc782ad3, secret
{d0d81d65fd961dff, a9132a2f5b5d4f54, d72e7f4d4f7270f5} -> 4329ec0defb7f826; pair B24:
seed 8f934f0d969d8c38, secret {b928485235f145d4, e19da6ded88cdbd0, 64eacf717f4fb9ce}
-> e69e5ea677632e22) reproduce from the upstream header; `rapidhash_withSeed` agrees with
`rapidhash_internal(..., rapid_secret)`.

## Pairs

    A   (32 B, L=4, words 0 and 1 complemented)
        M  = 9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c
        M' = 642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c
    B24 (24 B, L=3, word 0 complemented)
        M  = 9bd4604137366abec688a63706aa4a2188d35499de169df6
        M' = 642b9fbec8c99541c688a63706aa4a2188d35499de169df6

## Results (random model = uniform seed + 3 uniform secret words, 256-bit key)

| log | pair | model | rngseed | hits / keys | log2 rate [95%] | cap bits [95%] |
|---|---|---|---|---|---|---|
| 01 | A   | random  | 1 | 40 / 2^32 | -26.68 [-27.16, -26.23] | 28.68 [28.23, 29.16] |
| 03 | A   | random  | 2 | 82 / 2^33 | -26.64 [-26.97, -26.33] | 28.64 [28.33, 28.97] |
| pooled | A | random | 1+2 | 122 / 3*2^32 | -26.65 [-26.92, -26.40] | 28.65 [28.40, 28.92] |
| 02 | B24 | random  | 1 | 25 / 2^32 | -27.36 [-27.98, -26.79] | 28.94 [28.38, 29.57] |
| 04 | B24 | random  | 2 | 68 / 2^33 | -26.91 [-27.28, -26.57] | 28.50 [28.16, 28.86] |
| pooled | B24 | random | 1+2 | 93 / 3*2^32 | -27.05 [-27.36, -26.75] | 28.63 [28.34, 28.94] |
| 05 | A   | default secret | 3 | 47 / 2^32 | -26.45 [-26.89, -26.03] | 28.45 [28.03, 28.89] |
| 05 | A   | odd secret     | 3 | 49 / 2^32 | -26.39 [-26.82, -25.98] | 28.39 [27.98, 28.82] |
| 05 | B24 | default secret | 3 | 32 / 2^32 | -27.00 [-27.55, -26.50] | 28.59 [28.09, 29.13] |
| 06 | A-word0-only (32 B) | random | 3 | 36 / 2^32 | -26.83 [-27.34, -26.36] | 28.83 [28.36, 29.34] |
| 06 | A-word1-only (32 B) | random | 3 | 34 / 2^32 | -26.91 [-27.44, -26.43] | 28.91 [28.43, 29.44] |
| 06 | null control (bit 0 of byte 7 flipped) | random | 3 | 0 / 2^32 | < -30.12 (floor) | > 32.12 |

Searcher's figures for comparison: A 38/2^32 (-26.75, cap 28.75); B24 32/2^32 (-27.00,
cap 28.59).  Both lie inside the independent intervals.  Pooling searcher + independent:
A 160/2^34 = 2^-26.68, cap 28.68 [28.46, 28.91]; B24 125/2^34 = 2^-27.03, cap 28.62
[28.37, 28.88].

## Reading

* Rate is independent of the secret model.  Pair A under uniform secret (2^-26.65),
  odd secret (2^-26.39) and the compiled-in default (2^-26.45) agree within the
  intervals; the differential is a property of mix(A,B) = lo ^ hi of A*B over uniform
  operands, so randomising the secret words does not help.  The default-secret figure
  reproduces the row's 12/2^30 = 2^-26.4 (28.4 bits) at 4x the sample.
* A (28.65 [28.40, 28.92]) and B24 (28.63 [28.34, 28.94]) are statistically tied; the
  L=3 gain of 0.415 bits is cancelled by a fold rate ~0.4 bits lower.  Citing 28.6 with
  the interval is supported; neither pair is distinguishable from the other at 2^33.
* Single-word complements at 32 B (word 0 only, word 1 only) collide at the same rate
  as the two-word complement (within CI), consistent with the fold-differential model.
* Null control at 0/2^32 sets the resolution floor 2^-30.1 for a 2^32 sample.

## Reproduce

    g++ -O3 -std=c++17 -march=native -pthread rs_verify.cpp -o rs_verify
    ./rs_verify selftest
    ./rs_verify pair 9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c \
                     642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c 32 1 random 8   # 40/2^32
    ./rs_verify pair 9bd4604137366abec688a63706aa4a2188d35499de169df6 \
                     642b9fbec8c99541c688a63706aa4a2188d35499de169df6 32 1 random 8                   # 25/2^32
    ./rs_verify pair <A> <A'> 33 2 random 8      # 82/2^33
    ./rs_verify pair <B> <B'> 33 2 random 8      # 68/2^33
    CORES=0-7 NICE=0 ./run_verify.sh             # full pipeline, logs/00..06 + DONE

Results are deterministic for a given (rngseed, threads): thread t of an 8-thread run
draws from mt19937_64(seed_seq{rngseed_lo, rngseed_hi, t, 0x9e3779b9, 0x7f4a7c15}).
