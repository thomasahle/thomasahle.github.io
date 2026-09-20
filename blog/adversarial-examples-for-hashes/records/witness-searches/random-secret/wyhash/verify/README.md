# wyhash final v4.3: independent re-measurement under the random-secret key model

Date: 2026-09-19. Host: x86 Xeon, 8 cores (`nice -n 10 taskset -c 24-31`).
Purpose: cross-check the search harness's two headline rates with a program written
from scratch (`wyverify.cpp`, this directory): different RNG (std::mt19937_64 and,
as a second stream, splitmix64), different threading and reporting, and the
upstream header used verbatim.

## Source and self-checks

`wyhash_upstream.h` = https://raw.githubusercontent.com/wangyi-fudan/wyhash/master/wyhash.h
(final v4.3, `WYHASH_CONDOM` left at its default 1, 64x64->128 mum),
md5 b1a3ba7377d00e05d0266cd044649f65, identical to the copy the searcher used.

At startup the program prints, and the run logs record:

* the seven upstream `test_vector.cpp` messages hashed with seed = index and the
  default secret; all seven equal SMHasher3's table for `wyhash`
  (93228a4de0eec5a2 "", c5bac3db178713c4 "a", a97f2f7b1d9b3314 "abc",
  786d1f1df3801df4 "message digest", ...);
* the SMHasher verification value, recomputed by this program's own implementation
  of the procedure (key[i]=i, length i, seed 256-i; hash the 2048-byte output array
  with seed 0; first four bytes little-endian): 0x9DAE7DD3.

`witness.cpp` recomputes both messages under an explicit (seed, secret[4]). The
searcher's two random-model witnesses reproduce:

* pair A, seed 6c58e2bbe0b8c2ef, secret {2e87eccce404b9e7, 27807013cee858cb,
  064c5e57e4a52845, bda1eaa3841f1235} -> bf0d6eb792755aad for both messages;
* 24-byte pair, seed 046f3d47982ae318, secret {a871a957d2941a44, 089e8e1312e207e3,
  86314fbbd0cb1fe0, cf74732da4a6d3ce} -> d794ef632d51ad81 for both messages.

## Key model

Per trial: one uniform 64-bit API seed and four independent uniform 64-bit secret
words, passed as `wyhash(key, len, seed, secret)`; 320 hidden key bits. Control:
the same seeds with the public default secret `_wyp`.

## Pairs

* Pair A (32 B, L = 4): M  = 9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c,
  M' = 642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c
  (first two words complemented, 16-byte suffix unchanged). This is the pair shipped in
  the page's verify/wyhash package.
* 24-byte pair (L = 3): M  = 38ae2f8efa92bafc796ada2e8cdcc1f30ce3f58993dd4f2e,
  M' = c751d071056d4503796ada2e8cdcc1f30ce3f58993dd4f2e (first word complemented).

## Metric

bits = log2(L / eps), L = ceil(len / 8), eps = collision probability of the fixed pair
over the random key. Intervals: exact central 95% Poisson (Garwood) on the hit count,
transformed to bits (`ci.py`).

## Results

Random-secret model (uniform seed + four uniform secret words), 2^35 keys per run:

| pair | RNG stream (salt) | hits / keys | rate, 95% Garwood | bits (cap) |
|---|---|---|---|---|
| A (32 B, L=4) | mt19937_64 (1001) | 388 / 2^35 | 2^-26.400 [2^-26.547, 2^-26.257] | 28.40 [28.26, 28.55] |
| A (32 B, L=4) | splitmix64 (2001)  | 343 / 2^35 | 2^-26.578 [2^-26.735, 2^-26.425] | 28.58 [28.43, 28.73] |
| A (32 B, L=4) | both, this program | 731 / 2^36 | 2^-26.486 [2^-26.593, 2^-26.382] | **28.49 [28.38, 28.59]** |
| A (32 B, L=4) | + searcher's 357 / 2^35 | 1088 / 2^36.58 | 2^-26.497 [2^-26.585, 2^-26.412] | 28.50 [28.41, 28.58] |
| 24 B (L=3) | mt19937_64 (1002) | 258 / 2^35 | 2^-26.989 [2^-27.170, 2^-26.813] | 28.57 [28.40, 28.76] |
| 24 B (L=3) | splitmix64 (2002)  | 300 / 2^35 | 2^-26.771 [2^-26.939, 2^-26.608] | 28.36 [28.19, 28.52] |
| 24 B (L=3) | both, this program | 558 / 2^36 | 2^-26.876 [2^-26.998, 2^-26.756] | **28.46 [28.34, 28.58]** |
| 24 B (L=3) | + searcher's 289 / 2^35 | 847 / 2^36.58 | 2^-26.859 [2^-26.958, 2^-26.762] | 28.44 [28.35, 28.54] |

Default public secret (control, uniform seed only), 2^31 keys:

| pair | hits / keys | rate | bits (cap) |
|---|---|---|---|
| A (32 B) | 21 / 2^31 | 2^-26.61 [2^-27.30, 2^-26.00] | 28.61 [28.00, 29.30] |
| 24 B | 15 / 2^31 | 2^-27.09 [2^-27.93, 2^-26.37] | 28.68 [27.96, 29.52] |

Witnesses found by this program (random model): pair A, seed eca3dbf2eb354b3f,
secret {86e6b475eba4ee77, aa0dc193c4330d23, 3a2e441049ca861a, f6522f7b0f34a242}
-> ff5bb57ee51084c0; 24 B, seed 803bf66adba542cc, secret {a486454611792926,
58c5c7e7b418f10a, 2d32929492fbbcb2, 3330776b19c87085} -> 46ea80522165a0d6.

## Reading

* Both of the searcher's rates are confirmed by an independent program and two
  independent RNG streams: pair A 2^-26.49 here vs 2^-26.52 (searcher); 24 B
  2^-26.88 here vs 2^-26.83 (searcher). Every per-run interval overlaps the
  searcher's, and the pooled intervals contain the searcher's point estimates.
* The two pairs' collision rates genuinely differ (pair A / 24 B ratio 1.31,
  4.8 sigma on this program's 2^36 counts alone), but the L=3 credit
  (log2(4/3) = 0.415 bits) cancels the difference almost exactly: 28.49 vs 28.46
  here, 28.52 vs 28.41 for the searcher. The two caps are statistically tied; the
  ordering even flips between RNG streams (mt: A lower; sm: 24 B lower). Either
  pair is a defensible row score of 28.4-28.5 bits.
* The rate is the same under the default public secret (21/2^31 = 2^-26.61 vs
  the page's historical 9/2^30 = 2^-26.83 and the searcher's 28/2^31), consistent
  with the mechanism (fold of the seeded 128-bit product) not using the secret value.
  The default-secret model additionally admits the P = 1 key-free annihilation and
  the P = 1/3 seed-dependent class, both excluded under random secret words.
* Sampling resolution: with 2^36 keys the 95% interval on bits is about +-0.11.
  Nothing below 2^-27 for these pairs is at issue; no zero-hit floors were needed.


## Reproduction

```
curl -sLo wyhash_upstream.h https://raw.githubusercontent.com/wangyi-fudan/wyhash/master/wyhash.h
g++ -O2 -std=c++17 -pthread -o wyverify wyverify.cpp
g++ -O2 -std=c++17 -o witness witness.cpp
./run_verify.sh            # the six runs below, ~40 min on 8 cores
python3 ci.py *.log        # intervals
```

Runs are deterministic for the same (salt, threads, rng): each thread seeds its own
generator from (salt, thread index) and draws 2^lg / threads keys.
