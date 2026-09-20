# rapidhash v3, random-secret model: verification results (2026-09-19, interim)

Hash: upstream `rapidhash.h` tag `rapidhash_v3` (sha256 807874ee...626e), `rapidhash_internal`
with a caller-supplied 8-word secret; default `RAPIDHASH_COMPACT` + `RAPIDHASH_FAST` build.
Key model: 64-bit seed and all eight 64-bit secret words uniform per key (576 bits).
Intervals: exact central 95 % Poisson (Garwood), `ci.py`.  "cap" = log2(L) - log2(rate).

## Self-checks (logs/00, 01)

* Witness re-asserted: seed `3187ae8a8617e034`, shipped secret, both 32-byte messages of pair A ->
  `a7ee6375a78a86f0`, via `rapidhash_internal` and via the public `rapidhash_withSeed`: PASS.
* Upstream `rapidhash_internal` == the study's port on 2 000 000 random tuples
  (lengths 0..300, half random secret, half shipped): 0 mismatches.  The study's hash body was
  therefore exactly upstream v3, which also disposes of the "Triplet" doc-comment ambiguity:
  the code reads `secret[0..7]`.

## Pair A (32 B, words 0 and 1 complemented, L = 4), seed + secret uniform

| run | RNG | keys | count | rate | cap (bits) |
|---|---|---|---|---|---|
| 10 | splitmix64-counter | 2^30 | 10 | 2^-26.68 [2^-27.74, 2^-25.80] | 28.68 [27.80, 29.74] |
| 13 | ChaCha20 | 2^30 | 5 | 2^-27.68 [2^-29.30, 2^-26.46] | 29.68 [28.46, 31.30] |
| 20 | splitmix64-counter | 2^34 (fresh) | 155 | 2^-26.72 [2^-26.96, 2^-26.50] | 28.72 [28.50, 28.96] |
| pooled 10+20 | | 2^34.09 | 165 | 2^-26.72 [2^-26.95, 2^-26.50] | 28.72 [28.50, 28.95] |
| 30 | splitmix64-counter | 2^35 (fresh) | pending | | |
| 41 | splitmix64-counter | 2^37 (fresh) | pending | | |

Study's claim for the same pair: 532 / 2^35.6 = 2^-26.56 [2^-26.69, 2^-26.44], cap 28.56 [28.44, 28.69];
primitive-based 28.59 [28.53, 28.64].  My 2^34 sample (155) agrees with the study's fresh 2^34
sample (161) and with the primitive prediction (2^34 x 2^-26.59 = 171 expected; 155 is -1.2 sigma).
Confirmed at the current precision.

## Proposed 24-byte pair (word 0 complemented, L = 3), seed + secret uniform

| run | RNG | keys | count | rate | cap (bits) |
|---|---|---|---|---|---|
| 11 | splitmix64-counter | 2^30 | 7 | 2^-27.19 [2^-28.51, 2^-26.15] | 28.78 [27.73, 30.09] |
| 14 | ChaCha20 | 2^30 | 7 | 2^-27.19 [2^-28.51, 2^-26.15] | 28.78 [27.73, 30.09] |
| pooled 11+14 | | 2^31 | 14 | 2^-27.19 [2^-28.06, 2^-26.45] | 28.78 [28.03, 29.65] |
| 21 | splitmix64-counter | 2^34 (fresh) | pending | | |
| 31 | splitmix64-counter | 2^35 (fresh) | pending | | |
| 42 | splitmix64-counter | 2^37 (fresh) | pending | | |

Study's claim: 428 / 2^35.6 = 2^-26.87 [2^-27.01, 2^-26.74], cap 28.46 [28.32, 28.60];
primitive-based P(M,0) 2^-26.95 -> 28.53 [28.49, 28.58].  Expected at 2^31 with rate 2^-26.95:
16.6; observed 14.  Consistent; the 2^34/2^35/2^37 samples decide.

## Pair D (48 B, L = 6): 11 / 2^30 (log 12), 2^-26.54, cap 29.13 [28.29, 30.13]; study: 11 / 2^30.

## Reading

Both rates are reproduced from the unmodified upstream source with a fresh sampler and two
different RNGs.  The mechanism is secret-independent (the colliding fold sees uniform operands
whichever way the secret is drawn), so the random-secret score equals the default-secret score:
28.5 bits displayed for either witness pair.  The remaining runs (logs/21, 30, 31, 40, 41, 42) are
executing unattended on the Xeon (`run_all.sh` then `run_stage2.sh`; markers `logs/ALL_DONE`,
`logs/STAGE2_DONE`); `python3 summarize.py` pools them once the logs are copied in.
