# xxh3-128 under the random-secret model: independent verification (2026-09-19)

Verification lane, separate from the searcher's `../rs128.c` harness: a fresh sampler
(`verify_rs128.c`), a different RNG family (128-bit Lehmer MCG instead of xoshiro256**),
own pair constructions (`gen_pairs.py`, seed 91919), and the upstream header fetched
directly from GitHub on the compute host rather than the searcher's vendored copy.

## Source and self-checks
* `xxhash.h` = https://raw.githubusercontent.com/Cyan4973/xxHash/v0.8.3/xxhash.h,
  sha256 `17973c0dc49d9854ca26caa191f0e12f7a424b68858d9a78de3860d959d85e4b` (matches the
  searcher's copy and the blog's verify package). `XXH_versionNumber() = 803`.
* `verify_rs128 selftest` (log `logs/selftest.txt`): SMHasher3 verification 0x288DAA94 PASS;
  the blog row's witness seed `e130d569418d2efe` collides pair F with the recorded digest
  `77aee7ca6253510c ff9994fe21ab989a` PASS; API dispatch claim PASS over 54,400 checks:
  `XXH3_128bits_withSecretandSeed` equals `XXH3_128bits_withSeed` (custom secret ignored) for
  every length 0..240 and equals `XXH3_128bits_withSecret` (seed ignored) for lengths 241..400;
  the static `XXH3_128bits_internal(.., seed, secret, 192, XXH3_hashLong_128b_withSecret)`
  uses the custom secret for 17..240 B and agrees with `withSecret` above 240 B.
* `witness_check.c` links the public, non-inlined library (`xxhash.c` v0.8.3, sha256
  `5c3591fe6e6c86a619eb26760e9520e37a6fd5152882ab5ad93f912e2a855966`) and recomputes a sampled
  witness through `XXH3_128bits_withSecret`: full 128-bit collision reproduced (see section 3).

## Key model sampled
Per trial: 24 uniform 64-bit words -> 192-byte secret (little-endian), plus an independent
uniform 64-bit seed. `secret` mode calls `XXH3_128bits_withSecret(m, len, secret, 192)` (public
API, seed 0); `secretseed` mode calls `XXH3_128bits_internal` with the random seed AND the
random secret; `seed` mode is the row's current model (`XXH3_128bits_withSeed`, default kSecret).
A hit is equality of BOTH low64 and high64 for the pair under the same key.

## Results
### Current row pair F (32 B, L = 4 words) -- full 128-bit collisions
| leg (log) | key model | count / N | rate | 95% CI (log2) | cap bits, 95% CI |
|---|---|---|---|---|---|
| `F_secret_2p34_r1001` | uniform 192-B secret, `XXH3_128bits_withSecret` (seed 0) | 156 / 2^34 | 2^-26.71 | [-26.95, -26.49] | 28.71 [28.49, 28.95] |
| `F_secretseed_2p33_r1002` | uniform secret AND uniform seed (`XXH3_128bits_internal`) | 84 / 2^33 | 2^-26.61 | [-26.93, -26.30] | 28.61 [28.30, 28.93] |
| `F_seed_2p32_r1003` (control) | default kSecret, uniform 64-bit seed (row's current model) | 44 / 2^32 | 2^-26.54 | [-27.00, -26.12] | 28.54 [28.12, 29.00] |
| `family_secret_2p32_r1004` (F_row line) | uniform secret, second stream | 60 / 2^32 | 2^-26.09 | [-26.48, -25.73] | 28.09 [27.73, 28.48] |
| pooled, uniform secret (156+60+84) | secret and secret+seed | 300 / (2^34+2^32+2^33) | 2^-26.58 | [-26.75, -26.42] | 28.58 [28.42, 28.75] |

Searcher's own leg for the same model (`../logs/F_secret_2p34_s11.txt`, xoshiro stream 11):
179 / 2^34 = 2^-26.52 [-26.74, -26.30]; overlaps with every row above. Combined with my
withSecret legs (156+60+179 over 2^34+2^32+2^34): 395 / 2^35.32 = 2^-26.60.

Conclusion for (a): the random-secret rate of pair F equals the default-secret row figure
(165 / 2^34 = 2^-26.63, 95% [-26.86, -26.41]) within noise; the seed adds nothing on top of a
uniform secret (secretseed leg indistinguishable from the secret leg). Row score under the
random-secret model: bits ~28.6, direction estimated upper bound (sampled cap at L = 4).

### Proposed best pair / family (uniform 192-B secret, 2^32 keys each, log `family_secret_2p32_r1004`)
| pair (own construction unless noted) | len | count | rate | 95% CI (log2) |
|---|---|---|---|---|
| own_sum_-1_block0 (w1 = ~w0, both complemented) | 32 | 40 | 2^-26.68 | [-27.16, -26.23] |
| own_sum_-1_block0_b | 32 | 48 | 2^-26.42 | [-26.85, -26.01] |
| own_sum_2^63-1_block0 (w1 = ~w0 ^ 2^63) | 32 | 49 | 2^-26.39 | [-26.82, -25.98] |
| own_sum_-1_block1 (same on words 2,3) | 32 | 40 | 2^-26.68 | [-27.16, -26.23] |
| own_twin_sum_2^62-1_block0 | 32 | 8 | 2^-29.00 | [-30.21, -28.02] |
| own_twin_sum_2^63+2^62-1_block0 | 32 | 5 | 2^-29.68 | [-31.30, -28.46] |
| own_complement_random_sum_control (both words complemented, sum not preserved) | 32 | 0 | < 2^-30.12 (97.5%) | -- |
| own_random_pair_control | 32 | 0 | < 2^-30.12 | -- |
| xxh3-64 memo NAF pair (default-secret trick) | 32 | 0 | < 2^-30.12 | -- |
| own_64B_sum_-1_block0 | 64 | 43 | 2^-26.57 | [-27.04, -26.14] (L = 8: cap 29.6) |
| own_160B_sum_-1_block0 | 160 | 34 | 2^-26.91 | [-27.44, -26.43] (L = 20: cap 31.2) |

Conclusion for (b): every member of the sum -1 / sum 2^63-1 complement class sits at the
same ~2^-26.6 regardless of the words, the block, or the length, so the rate is pair-independent
and no member beats pair F; the best cap is the 32-byte one (L = 4), ~28.6 bits. The TWIN
classes (sum 2^62-1, 2^63+2^62-1) are NOT at the class rate: pooled 13 / 2^33 = 2^-29.3
[-30.21, -28.53], i.e. about 6x rarer, matching the fold 'differ by exactly 2^63' event
(35 / 2^34 = 2^-28.9 in `fold_check`). The controls (random pair, sum-breaking complement, the
xxh3-64 NAF pair) give zero hits in 2^32, so the NAF trick does not transfer to a uniform secret.

## Hash-independent check of the prediction
`fold_check 34 5 4` (`logs/fold_check_2p34.txt`): Pr[fold(A,B) = fold(~A,~B)] for independent
uniform A, B, fold(a,b) = lo64(ab) ^ hi64(ab): 165 / 2^34 = 2^-26.63 (twin-by-2^63 events: 35).
This is the event that decides pair F once the two 16-byte-block words are XORed with uniform
secret words, and it is the same number as the default-secret row figure (165 / 2^34).

## Reproduction (compute host: Xeon, `nice -n 10 taskset -c 24-31`, 8 threads)
    curl -sSL -o xxhash.h https://raw.githubusercontent.com/Cyan4973/xxHash/v0.8.3/xxhash.h
    curl -sSL -o xxhash.c https://raw.githubusercontent.com/Cyan4973/xxHash/v0.8.3/xxhash.c
    sha256sum xxhash.h xxhash.c
    cc -O2 -std=gnu11 -pthread -o verify_rs128 verify_rs128.c -lm
    cc -O2 -o witness_check witness_check.c xxhash.c
    cc -O2 -pthread -o fold_check fold_check.c -lm
    python3 gen_pairs.py                      # row.txt, family.txt
    ./run_verify.sh                           # selftest + the four legs below, logs in logs/
    ./verify_rs128 secret     34 1001 8 row.txt
    ./verify_rs128 secretseed 33 1002 8 row.txt
    ./verify_rs128 seed       32 1003 8 row.txt
    ./verify_rs128 secret     32 1004 8 family.txt
    ./fold_check 34 5 4
    python3 ci_check.py <count> <N> 4         # exact Poisson 95% CI, cap bits for L = 4 words
All counts are deterministic given (mode, log2N, rngseed, threads).
