# wyhash final v4.3 under the random-secret key model

Date: 2026-09-19. Host: x86 Xeon (8 cores, `nice -n 10 taskset -c 24-31`).
Program: `wyrs.cpp` (this directory), built against the upstream header
`wyhash_upstream.h` = https://raw.githubusercontent.com/wangyi-fudan/wyhash/master/wyhash.h
(final v4.3, `WYHASH_CONDOM=1`, 64x64->128 mum). Startup recomputes the SMHasher3
verification value 0x9DAE7DD3 and refuses to run otherwise.

## 1. Secret mechanism and its size

`wyhash(const void *key, size_t len, uint64_t seed, const uint64_t *secret)` takes a
caller-supplied 4-word secret array (the default is the public `_wyp[4]`). Two ways to
supply one:

* `make_secret(uint64_t seed, uint64_t *secret)`: derives the four words from a 64-bit
  seed with wyrand; each word is a concatenation of 8 bytes of popcount 4 drawn from a
  70-entry table, rejected unless odd, prime, and at Hamming distance exactly 32 from every
  earlier word. Entropy <= 64 bits (the input seed), from a set of at most 70^8 ~ 2^49
  words per slot.
* any `uint64_t secret[4]` the caller fills: 256 bits of secret words.

Total hidden key material under the strongest model the API supports: 64-bit API seed
plus 4 x 64 = 256 secret bits = 320 bits. The author's model for the row treats every
secret word as independent and uniform (mode 1 below); make_secret (mode 3) and odd
words (mode 2, the old harness convention) are reported as controls.

Key models in `wyrs`:
mode 0 = default public secret, uniform seed; mode 1 = uniform seed + 4 uniform words;
mode 2 = uniform seed + 4 uniform odd words; mode 3 = uniform seed + make_secret(uniform
64-bit) drawn from a precomputed table of 2^17 secrets (make_secret costs ~5 ms per call).

## 2. Metric

bits = min over L of log2(L / eps), L = ceil(max byte length / 8), eps = collision
probability of the fixed pair over the random key. Intervals: exact central 95% Poisson
(Garwood) on the hit count, transformed to bits; the bits interval is [log2(L/hi),
log2(L/lo)]. Where the count is 0 the 97.5% one-sided bound gives a floor.

## 3. Results (key model = mode 1 unless stated; all counts are in the *.log files here)

### 3.1 The row's current pair A (32 B, L=4) under the random-secret model

Pair A: M  = 9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c,
        M' = 642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c
(words w0 and w1 complemented, 16-byte suffix unchanged).

| key model | hits / keys | rate | bits (L=4), 95% Garwood |
|---|---|---|---|
| mode 1, uniform seed + 4 uniform words, fresh 2^35 (salt 301) | 357 / 2^35 | 2^-26.520 [2^-26.674, 2^-26.371] | **28.52 [28.37, 28.67]** |
| mode 1, all runs pooled (2^32 + 2^30 + 2^35) | 401 / 2^35.21 | 2^-26.562 [2^-26.707, 2^-26.421] | 28.56 [28.42, 28.71] |
| mode 3, make_secret(uniform 64-bit) | 21 / 2^31 | 2^-26.61 [2^-27.30, 2^-26.00] | 28.61 [28.00, 29.30] |
| mode 2, uniform odd words | 14 / 2^31 | 2^-27.19 [2^-28.06, 2^-26.45] | 29.19 [28.45, 30.06] |
| mode 0, default public secret (same program) | 28 / 2^31 | 2^-26.19 [2^-26.78, 2^-25.66] | 28.19 [27.66, 28.78] |

Historical default-secret figure on the page: 9 / 2^30 = 2^-26.83, 28.8 bits [27.9, 30.0].
Witness (mode 1, fresh run): seed 6c58e2bbe0b8c2ef, secret {2e87eccce404b9e7, 27807013cee858cb,
064c5e57e4a52845, bda1eaa3841f1235} -> both hash to bf0d6eb792755aad.

Why the models agree: in the loop fold t = fold((w0^secret[1]) * (w1^seed')) the message words are
XORed with key material, so complementing both words is the same (X,Y) -> (~X,~Y) differential whatever
the key values are; the rate is an average over uniform X, Y. With the public secret X = w0^secret[1] is a
fixed known value and the rate is the conditional rate at that X (it varies with X, see 3.4).

### 3.2 Structural search (batch_struct.txt, 2^30 keys each; struct.log)

Only one mechanism produces collisions above the floor: a single fold differential at a loop word
pair whose words are not re-read by the tail (a = word at len-16, b = word at len-8). Every such
site sits at 2^-26.2 .. 2^-27.0:

| pair | L | hits / 2^30 | rate |
|---|---|---|---|
| len 32, w0 and w1 complemented (pair A / fresh base) | 4 | 12, 13 | 2^-26.4, 2^-26.3 |
| len 32, w0 only / w1 only complemented | 4 | 11 / 9 | 2^-26.5 / 2^-26.8 |
| len 24, w0 only complemented (three bases incl. all-zero) | 3 | 8, 8, 7 | 2^-27.0, 2^-27.0, 2^-27.2 |
| len 40 (w0,w1) / w2 only | 5 | 13 / 8 | 2^-26.3 / 2^-27.0 |
| len 48 lane 1 (w0,w1) / lane 2 (w2,w3) | 6 | 8 / 13 | 2^-27.0 / 2^-26.3 |
| len 64 (w0,w1) | 8 | 10 | 2^-26.7 |

Zero hits at 2^30 (rate <= 2^-28.12 at 97.5%): partial-word complements at len 17/20/23 (bytes
0..0, 0..3, 0..6), low/high 32-bit halves, top bit, alternating masks and ~M^1 at len 24; two-site
variants (w0+w2, w1+w2 at len 24); every short pair tried (len 3/4/8/12/16 whole complement, a-part
or b-part complement at len 16, random pairs at 16 and 32 B); cross-length all-zero pairs (16/15,
8/7, 4/3, 12/11, 16/9, 2/1, 16/8, 10/5). (~M^2 at len 24: 3/2^30, i.e. the same class as M.)

### 3.3 Site scans and length sweeps

* batch_site24.txt (206 masks delta on w0 at len 24, eps = 0, 2^29 each; site24.log): non-zero only
  for delta = all-ones (4 hits) and all-ones^2 (5 hits). All 63 other single-bit neighbours of
  all-ones, all 2^k-1 and ~(2^k-1) masks (k < 64) and single bits: 0 hits (<= 2^-27.1 each).
* batch_site32.txt (164 (delta, eps) combinations on (w0, w1) at len 32, 2^29 each; site32.log):
  non-zero only for (M, M) (8 hits) and (M^2, M^2) (2 hits); the 64 single-bit neighbours (M, M^bit)
  and 98 other structured combinations: 0 hits.
* batch_sweep_w.txt (lengths 1..64: whole-message complement, each aligned word complement, one random
  pair; 2^29 each; sweep_w.log): hits only at word complements of loop-only words for len >= 24
  (sweep24_w0M 5/2^29 ... sweep64), none at len <= 23, none for whole-message complements or random pairs.
* batch_sweep_b.txt (every single-byte complement at every position, lengths 1..64, 2080 pairs, 2^26
  each; sweep_b.log): 0 hits everywhere (each <= 2^-24.1).
* batch_xlen.txt (all pairs of all-zero messages of lengths 1..16, same (a,b) = (0,0), only len
  differs in the final fold; 2^28 each; xlen.log): 0 hits in all 120 pairs. batch_final.txt reruns
  all 136 such pairs including the empty message at 2^31 (L=1) / 2^30 (L=2), plus 1..8-byte whole
  complements and random pairs at 2^31: see final.log and section 3.5.

Exact small-n analogue (foldx.cpp): fold(XY) vs fold(~X ~Y) has mean P = 2^-3.215, 2^-4.911,
2^-6.646 at n = 8, 12, 16 (2^(-0.40..0.415 n)); fold(XY) vs fold(~X Y) has 2^-3.496, 2^-5.177,
2^-6.956 (2^(-0.43..0.435 n)). At n = 64 the measured rates are 2^-26.52 and 2^-26.83.

### 3.4 Best pair under the random-secret model

24-byte single complement (L=3): M  = 38ae2f8efa92bafc796ada2e8cdcc1f30ce3f58993dd4f2e,
                                   M' = c751d071056d4503796ada2e8cdcc1f30ce3f58993dd4f2e
(w0 complemented; w1, w2 unchanged; w1 = a and w2 = b of the tail).
Fresh 2^35 (salt 302): 289 / 2^35 = 2^-26.825 [2^-26.996, 2^-26.659]; bits = log2(3) + 26.825 =
**28.41 [28.24, 28.58]**. Witness: seed 046f3d47982ae318, secret {a871a957d2941a44, 089e8e1312e207e3,
86314fbbd0cb1fe0, cf74732da4a6d3ce} -> d794ef632d51ad81.

Pair A gives 28.52 [28.37, 28.67]. The rate of the single-word site is lower (2^-26.83 vs 2^-26.52,
about 2.7 sigma on the rate difference), and the L = 3 gain (0.415 bits) just outweighs it: the two
scores differ by 0.11 bits with overlapping intervals, i.e. they are statistically tied. No pair beat
the ~2^-26.5..26.8 fold-differential class: all other candidates are at their resolution floors.

With the public secret the same site is far weaker: choosing w0 = 0x5555555555555555 ^ secret[1]
(file pair_X5555_default_secret.txt) makes the double complement collide with P = 2^-1.586 = 1/3
(1,396,657 / 2^22; 3.59 bits at L=4), and the annihilation w = secret[1] gives P = 1 seed-independent
collisions at 12, 16 and 32 bytes (records/fairness-pass/wyhash-verify/keyfree.log): key-free, 1 bit at
L=2. Under random secret words both disappear (0 / 2^22 for the 0x5555 pair).

### 3.5 Floors for what was not found

Each zero-hit run gives rate <= 2^-(lg - 1.883) at 97.5% (Garwood upper bound 3.689 for k = 0):
2^30 -> 2^-28.12, 2^31 -> 2^-29.12, 2^29 -> 2^-27.12, 2^28 -> 2^-26.12, 2^26 -> 2^-24.12.
Ranked against the 28.4-bit score: a length <= 8 pair (L=1) would need rate > 2^-28.4, a 9..16-byte
pair (L=2) rate > 2^-27.4, a 17..24-byte pair (L=3) rate > 2^-26.8. The final batch (batch_final.txt -> final.log: all 136 cross-length all-zero pairs of lengths 0..16 and
1..8-byte whole complements and random pairs, 152 lines, 0 hits) puts every L=1 candidate tried at
<= 2^-29.1 and every L=2 candidate at <= 2^-28.1, i.e. bits >= 29.12 for all of them, above 28.4; the site scans at 2^29 exclude rates above 2^-27.1 for the 206 + 164 structured masks.

## 4. Recommendation

Row score under the random-secret model (seed and four secret words uniform): **28.4 bits, measured
cap** (24-byte pair, 289 / 2^35, 95% [28.2, 28.6]); the shipped 32-byte pair A measures 28.5
[28.4, 28.7] under the same model, statistically tied, so keeping pair A at "28.5 (sampled; 357 / 2^35)"
is equally defensible and avoids churn in verify/wyhash. Notes to keep: the historical
default-secret figure for pair A, 9 / 2^30 = 28.8 [27.9, 30.0], is consistent with the random-secret
rate (the mechanism does not use the secret value); with the public default secret wyhash also has
seed-independent P = 1 collisions (w = secret[1], 1 bit at L=2) and a P = 1/3 seed-dependent class
(w0 = 0x5555...5 ^ secret[1]), both excluded by the random-secret model.

## 5. Reproduction

    curl -sLo wyhash_upstream.h https://raw.githubusercontent.com/wangyi-fudan/wyhash/master/wyhash.h
    g++ -O2 -std=c++17 -pthread -o wyrs wyrs.cpp
    A=9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c
    B=642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c
    ./wyrs pair $A $B 35 1 301 8        # pair A, uniform secret words, 2^35 keys -> 357 hits
    ./wyrs pair 38ae2f8efa92bafc796ada2e8cdcc1f30ce3f58993dd4f2e c751d071056d4503796ada2e8cdcc1f30ce3f58993dd4f2e 35 1 302 8   # -> 289 hits
    ./wyrs pair $A $B 31 3 104 8 17     # make_secret model (table of 2^17 secrets) -> 21 hits
    ./wyrs pair $A $B 31 0 103 8        # default secret -> 28 hits
    python3 gen.py                      # regenerates the batch_*.txt candidate files
    ./run_all.sh; ./run_final.sh        # stage A + all batches (~2.5 h on 8 cores); final floor batch
    python3 pool.py 4 "pair A" confirmA_mode1_2p35.log:=pair   # Garwood intervals in bits

The sampler is xoshiro256** seeded per thread from splitmix64(salt, thread); results are exact
for the same (salt, threads). `./wyrs` refuses to run unless the upstream header reproduces the
SMHasher3 verification value 0x9DAE7DD3. g++ 11.5, x86-64; runtime ~13 s per 2^30 keys on 8 cores.

