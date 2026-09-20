# Independent verification: XXH3-64 (xxHash v0.8.3) under the random-secret key model

Date: 2026-09-19. Verifier harness: `xxh3_verify.c` (this directory), written from scratch against the
upstream header (fresh download of `xxhash.h` from github.com/Cyan4973/xxHash tag v0.8.3, sha256
`17973c0dc49d9854ca26caa191f0e12f7a424b68858d9a78de3860d959d85e4b`). It shares no code with the searcher's
`xxh3_rs.c`: per-thread SplitMix64 streams instead of xoshiro256**, its own fold64, its own key layout, and
every witness is re-hashed through the public API in the main thread before it is printed (`recheck=ok`).
Host: Xeon Platinum 8375C, gcc 11.5.0, `nice -n 10 taskset -c 24-31`, 8 threads (cores shared with sibling
jobs). Intervals: Clopper-Pearson 95% (`ci.py`). Metric: bits = min_L log2(L/eps), L = ceil(len/8).

## 1. Facts checked in the upstream source (v0.8.3)

- `XXH3_64bits_withSecretandSeed` (xxhash.h L6204-6210): `if (length <= XXH3_MIDSIZE_MAX) return
  XXH3_64bits_internal(input, length, seed, XXH3_kSecret, sizeof(XXH3_kSecret), NULL);` - the custom secret is
  ignored for len <= 240. Confirmed empirically: the `ss` and `stream` runs below reproduce the default-secret
  rate of the page's NAF pair, and at 2^16 keys the streaming digest matched `ss` hit-for-hit per thread.
- Streaming (`XXH3_64bits_reset_withSecretandSeed` + `digest`, L6582-6595): `if (state->useSeed) return
  XXH3_64bits_withSeed(buffer, totalLen, seed);` for totalLen <= 240 - same behaviour.
- `XXH3_64bits_withSecret` reads only secret bytes 0..135 for len <= 240 (`XXH3_SECRET_SIZE_MIN = 136`);
  every 16-byte block of a 17..240-byte input is `fold64(w_lo ^ (S[2i] + seed), w_hi ^ (S[2i+1] - seed))`
  (`XXH3_mix16B`, L4732), so with a uniform secret the pair (S[2i]+seed, S[2i+1]-seed) is uniform whatever the
  seed: the `secret` (seed = 0) and `both` (seed uniform) models have identical fixed-pair rates at these lengths.
- The searcher's `both` model uses the internal `XXH3_64bits_internal(in, len, seed, S, 192, ...)`, which no
  public one-shot API exposes for len <= 240; I call the same internal function (it is the only way to key the
  seed and a custom secret together at short lengths).

## 2. Results

All keys fresh per trial; pairs fixed before sampling; every first witness re-hashed through the public API (`recheck=ok`
in every log). Wall times: 2^35 runs 383-594 s on 8 shared threads. Salts 9101-9110 (`run_verify.sh`).

| pair | API / key model | keys | hits | eps (95% CI) | L | cap bits (95% CI) | log |
|---|---|---|---|---|---|---|---|
| NAF 32 B (the row's current pair) | `XXH3_64bits_internal`, seed + 192-B secret uniform (`both`) | 2^35 | 332 | 2^-26.63 [2^-26.78, 2^-26.47] | 4 | 28.63 [28.47, 28.78] | naf_both_2p35 |
| NAF 32 B | `XXH3_64bits_withSecret`, 192-B secret uniform | 2^33 | 83 | 2^-26.63 [2^-26.95, 2^-26.32] | 4 | 28.63 [28.32, 28.95] | naf_secret_2p33 |
| NAF 32 B | `withSeed`, default kSecret (control) | 2^28 | 188388 | 2^-10.477 [2^-10.483, 2^-10.470] | 4 | 12.477 [12.470, 12.483] | naf_seed_2p28 |
| NAF 32 B | `withSecretandSeed`, 192-B secret uniform + seed (control) | 2^28 | 188643 | 2^-10.475 [2^-10.481, 2^-10.468] | 4 | 12.475 [12.468, 12.481] | naf_ss_2p28 |
| NAF 32 B | streaming `reset_withSecretandSeed` (control) | 2^24 | 11864 | 2^-10.466 [2^-10.492, 2^-10.440] | 4 | 12.466 | naf_stream_2p24 |
| W0 24 B (proposed: 0^24 vs ff^8 0^16) | `XXH3_64bits_withSecret`, 192-B secret uniform | 2^35 | 260 | 2^-26.98 [2^-27.16, 2^-26.80] | 3 | 28.56 [28.39, 28.74] | w0_secret_2p35 |
| W0 24 B | `XXH3_64bits_internal`, seed + secret uniform (`both`) | 2^35 | 267 | 2^-26.94 [2^-27.12, 2^-26.77] | 3 | 28.52 [28.35, 28.70] | w0_both_2p35 |
| W0 24 B | `withSeed`, default kSecret (control) | 2^32 | 42 | 2^-26.61 [2^-27.08, 2^-26.17] | 3 | 28.19 [27.76, 28.67] | w0_seed_2p32 |
| fold (M,M) | own fold64, x,y uniform | 2^34 | 165 | 2^-26.63 [2^-26.86, 2^-26.41] | - | - | fold_MM_2p34 |
| fold (M,0) | own fold64, x,y uniform | 2^34 | 144 | 2^-26.83 [2^-27.08, 2^-26.59] | - | - | fold_M0_2p34 |

Pooled (this harness only): NAF pair, random secret, 415 / (2^35 + 2^33): eps = 2^-26.63 [2^-26.77, 2^-26.49],
cap 28.62 [28.49, 28.77]. W0 pair, 527 / 2^36: eps = 2^-26.96 [2^-27.08, 2^-26.84], cap 28.54 [28.42, 28.67].
Pooled with the searcher's fold samples (results_fold.md 157 and 147 at 2^34) and the searcher's own 2^35 `both`
run of the NAF pair (349 hits, 2^-26.55): (M,M) 2^-26.64 [2^-26.75, 2^-26.54] -> NAF cap 28.64 [28.54, 28.75];
(M,0) 2^-26.91 [2^-27.01, 2^-26.81] -> W0 cap 28.49 [28.40, 28.59].

Verdict.
- The row's current pair does NOT collide at 2^-10.47 under the random-secret model; it collides at 2^-26.6
  (the (M,M) fold differential), cap 28.6 bits. The searcher's statement is confirmed. The 12.47-bit figure is the
  figure for `withSeed`, `withSecretandSeed` and the streaming API at this length (secret ignored), also confirmed.
- The proposed W0 pair's rate is 2^-26.96 [2^-27.08, 2^-26.84] in the full API (2^36 keys over the two
  equivalent models), slightly below the searcher's fold-based point estimate 2^-26.80 but inside their interval
  [2^-27.04, 2^-26.57]. Its cap is 28.5 bits, not 28.4: 28.54 [28.42, 28.67] from this harness alone, 28.49
  [28.40, 28.59] pooled with all (M,0) samples.
- W0 (L=3) beats the current NAF pair (L=4) by only ~0.1 bits in cap (28.5 vs 28.6) and the 95% intervals
  overlap, so "best pair" is not statistically separated from the current pair under this model; both give
  cap ~28.5-28.6 by the same mechanism. Either is fine for the row; I would print 28.5 with the W0 pair (shorter,
  single changed word, mechanism transparent) and state that the NAF pair scores 28.6 under the same model.


## 3. Mechanism cross-check (analytic, heuristic only)

`fold_dp.py` is a bit-serial carry DP for the fold differential that treats lo(xy), hi(xy) and the inputs as
independent uniform words. It gives 2^-26.69 for (M,M) and 2^-27.15 for (M,0) at 64 bits; against the exact
small-width table of the paper's `results_fold.md` it is 0.3-0.5 bits pessimistic (w=16: DP 2^-7.23 / 2^-6.77
vs exact 2^-6.96 / 2^-6.65), because the product's low and high halves are correlated. So the "complementary
carry chains, (3/4)^64" story is the right mechanism but not an exact number; the sampled rates stand.

## Reproduction

```sh
curl -sSLO https://raw.githubusercontent.com/Cyan4973/xxHash/v0.8.3/xxhash.h   # sha256 17973c0d...
cc -O3 -std=c11 -pthread -march=native xxh3_verify.c -lm -o xxh3_verify
./run_verify.sh                       # full plan, logs in logs/ (salts 9101-9110 are in the script)
# single checks
./xxh3_verify pair both   35 9101 8 0000000000000000000000000000000051151210404400000000008204000105 00000000000000000000000000000000aeeaedefbfbbffffffffff7dfbfffefa
./xxh3_verify pair secret 35 9102 8 000000000000000000000000000000000000000000000000 ffffffffffffffff00000000000000000000000000000000
./xxh3_verify fold 34 9110 8 ffffffffffffffff 0000000000000000
python3 ci.py HITS LOG2N L
python3 fold_dp.py
```
