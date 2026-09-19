# XXH3-64 (xxHash v0.8.3) under the random-secret key model

Date: 2026-09-19. Host: Xeon Platinum 8375C (`taskset -c 24-31`, `nice -n 10`, 8 threads), gcc 11.5.0.
Source: upstream `xxhash.h` v0.8.3, sha256 `17973c0dc49d9854ca26caa191f0e12f7a424b68858d9a78de3860d959d85e4b`
(identical to the copy shipped with the paper's `tools/bench/adversarial/`). Harness: `xxh3_rs.c` (this directory).
Metric: bits = min over L of log2(L / eps), eps = collision probability of a FIXED pair over the random key,
L = ceil(max length / 8) words. Intervals are Clopper-Pearson 95% (`ci.py`).

## 1. The secret mechanism

| API (v0.8.3) | key material for len <= 240 | key material for len > 240 |
|---|---|---|
| `XXH3_64bits_withSeed(in, len, seed)` | 64-bit seed, public default `kSecret` (192 B) | secret derived from kSecret +- seed per 16 B |
| `XXH3_64bits_withSecret(in, len, secret, secretSize)` | the caller's secret only (seed fixed to 0); bytes 0..135 are read | the caller's secret (stripes advance 8 B; scramble word at secretSize-64; merge at +11) |
| `XXH3_64bits_withSecretandSeed(in, len, secret, secretSize, seed)` | **64-bit seed with the public default `kSecret` — the custom secret is ignored** (`xxhash.h` L6230-6235: `if (length <= XXH3_MIDSIZE_MAX) return XXH3_64bits_internal(input, length, seed, XXH3_kSecret, ...)`) | the caller's secret; the seed is ignored |
| `XXH3_generateSecret(buf, size, customSeed, customSeedSize)` | derives a secret of `size >= 136` bytes from an arbitrary-length seed, for use with `withSecret` | |

`XXH3_SECRET_SIZE_MIN = 136` bytes (1088 bits); `XXH3_SECRET_DEFAULT_SIZE = 192` bytes (1536 bits), the size
`XXH3_generateSecret_fromSeed` produces and the size used here. The random-secret model of this note is therefore
`XXH3_64bits_withSecret` with 192 uniformly random secret bytes (the seed is 0 by definition of that API); the
task's "seed AND secret both uniform" model is `XXH3_64bits_internal(in, len, seed, secret, 192)` (mode `both` in the
harness), which no public one-shot API exposes for len <= 240. Both are measured; for len <= 240 the two models give
the same fixed-pair rates because every 16-byte block is keyed as
`fold64(lo ^ (S[2i] + seed), hi ^ (S[2i+1] - seed))`, and `S[2i] + seed`, `S[2i+1] - seed` are independent uniform words
whenever `S` is uniform, whatever the seed. **Consequence for the published page:** the 32-byte NAF pair collides at
2^-10.47 under `withSecretandSeed` as well (the secret is not used at that length), so the default-secret figure is the
figure for two of the three keyed APIs; only `withSecret` (and streaming `reset_withSecret`) reaches the random-secret
regime for short inputs.

## 2. The row's current pair with seed and secret both random

The searcher's one completed run (`logs/pair_naf_both_2p35.txt`, host log `logs/env.txt`): the row's NAF pair under seed + 192-byte secret uniform (`both`), 349 / 2^35 = 2^-26.55; the first witness is recorded in the log. The 2^-10.47 default-secret rate does not survive a uniform secret; the pair is then an ordinary (M,M) block complement at the fold rate.

## 3. Bounded search for the best fixed pair under the random secret

### 3a. Structure

Every keyed operation of XXH3-64 on inputs of 17..240 bytes is `acc += fold64(w0 ^ K0, w1 ^ K1)` per 16-byte block,
with `(K0, K1)` uniform under a random secret, followed by a bijective avalanche (129..240 bytes: two such
stages). No two input words are combined before a secret word touches them, so no difference cancels "for free":
a fixed pair collides iff the sum of the per-block fold changes is 0 (mod 2^64). A pair that changes a single block
collides with probability P(d, e) = Pr_{x,y uniform}[fold64(x, y) = fold64(x ^ d, y ^ e)], independent of the
message content and of which block is changed; changing two blocks costs the product of two such probabilities
(or a 2^-64 cancellation). The XOR-difference table of `results_fold.md` (paper repo, tools/bench/adversarial)
found the all-ones family best: (M, M) 2^-26.71 and (M, 0) 2^-26.80 at 2^34 samples, and exhaustive search at 8 bits
found nothing above the all-ones family. Mechanism: complementing x turns the 128-bit product into
`2^64 y - (x+1) y`, so the low and high halves receive additive changes t and ~t (+carry); the XOR fold is unchanged
iff the two carry chains are complementary at every bit position, probability (3/4)^64 = 2^-26.6 for uniform inputs.

Shortest length at which one changed word sits in exactly one block: 24 bytes (blocks are bytes 0..15 and
len-16..len-1; for len <= 23 the tail block overlaps bytes 0..7). So the structural candidate is a 24-byte pair
differing by the complement of bytes 0..7 (XOR difference (M, 0) on block 0), L = 3, cap = log2(3) + 26.8.
The other short paths cannot do better: 1..3 bytes are injective before the key (the length sits in bits 8..15 of
the combined word and the map is then a bijection); 4..8 bytes are `rrmxmx(input64 ^ bitflip, len)` with `input64`
injective in the message for a fixed length; 9..16 bytes add `swap64(x) + y` to the fold, which destroys the
carry-complement mechanism (measured below); > 240 bytes: the only unkeyed combination is `acc[i^1] += word`, and the
best construction found (swap two complementary words of one lane so the unkeyed sums agree and the two keyed
`lo32 * hi32` products cancel) needs `lo1+hi1+lo2+hi2 = 2^33 - 2`, about 2^-32.6, at L >= 31 (cap ~37.5).

### 3b. Measurements

The fold-differential stage was not run by this harness on the Xeon; the independent verifier (`verify/`, `fold_MM_2p34` 165 / 2^34 = 2^-26.63 and `fold_M0_2p34` 144 / 2^34 = 2^-26.83) and the paper repository's `results_fold.md` table (157 and 147 per 2^34) supply the primitive rates.

The length scan was not run; the structural argument of section 3a fixes the 24-byte word-0 complement (W0) as the shortest single-block candidate.

No hill-climb was run: under a uniform secret only the XOR difference fed to one fold matters, and the paper repository's 8-bit exhaustive table found nothing above the all-ones family.

## 4. Recommendation

Row pair under the random-secret model: W0 (24 B, bytes 0..7 complemented, L = 3), measured by the independent verifier at 527 / 2^36 = 2^-26.96 over `XXH3_64bits_withSecret` and the seed-and-secret model, cap 28.5 bits [28.4, 28.7]. The NAF pair scores 28.6 under the same model (statistically tied) and keeps its 12.47-bit figure only for `withSeed`, `withSecretandSeed` (len <= 240) and the streaming `reset_withSecretandSeed`, which use the default secret at that length.

## Reproduction

```sh
curl -sSLO https://raw.githubusercontent.com/Cyan4973/xxHash/v0.8.3/xxhash.h   # sha256 17973c0d...
cc -O3 -std=c11 -pthread -march=native xxh3_rs.c -lm -o xxh3_rs
./run_all.sh            # the full run (logs/), about 2.5 h on 8 shared cores
# single checks:
./xxh3_rs pair secret 36 112 8 000000000000000000000000000000000000000000000000 ffffffffffffffff00000000000000000000000000000000
./xxh3_rs pair both   36 101 8 0000000000000000000000000000000051151210404400000000008204000105 00000000000000000000000000000000aeeaedefbfbbffffffffff7dfbfffefa
python3 ci.py HITS LOG2N L
```
Each trial draws a fresh key (seed word + 24 secret words) from a per-thread xoshiro256** stream seeded by
SplitMix64(salt); salts are in `run_all.sh`. The pair is fixed before any key is drawn.
