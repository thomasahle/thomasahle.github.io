# xxh3-64-pair: witness search for XXH3-64 v0.8.3 (64-bit seed, default secret)

Metric: bits = min over L (8-byte words) of log2(L / eps), eps = P over a uniform 64-bit API
seed that a FIXED pair collides (XXH3_64bits_withSeed, default secret, exactly the row's key
model: "Uniform 64-bit API seed ...; fixed default public secret/constants").

Everything ran on the Xeon 8375C (<xeon-host>) under
`<xeon-work>/witness/xxh3-64-pair/`, `nice -n 10 taskset -c 24-31`, 8 cores, ~1.5 wall hours
in total (cores 24-31 were shared with another user's job, so per-process CPU was ~40-70%). Programs and logs are copied here.

## 1. Reproduction of the fairness-pass pair (cap 17.17)

Pair (memo item 10): M = `b8fe6c3923a44bbe83fe7ed308de52e3` || 0^16 = (K0, ~K1) || 0^16,
M' = bitwise complement of the first 16 bytes, K0,K1 = first two little-endian secret words.

`xxh3_64_pair_check.c` follows the verify/xxh3-64 pipeline: the 0.8.3 header embedded
verbatim (lines 1-7264 are byte-identical to `verify/xxh3-64/xxh3_64_32B_check.c`), SMHasher3
verification 0x1AAEE62C, the ten xsum sanity vectors, literal startup witnesses, two logical
xoshiro256** streams seeded from SplitMix64(salt*0x9e3779b97f4a7c15 + 7919*worker + 1),
N/2 full-width seeds each, all hashes through a non-inlined full-API wrapper, an independent
fold-event check (`event_mismatch_32_fold`), and a 32/128-byte length-normalisation check.
Three fresh 2^30 runs (salts 0x2026091901000477, ...02..., ...03...; source sha256
8d7cfd13... = the same program with the (K0,~K1) pair as default, see `confirm_2p30_salt0*.txt`):

| salt | hits / 2^30 | log2 eps | 95% CI (Clopper-Pearson) | cap bits (L=4) |
|---|---|---|---|---|
| ...01... | 29132 | -15.170 | [-15.186, -15.153] | 17.170 |
| ...02... | 29227 | -15.165 | [-15.182, -15.148] | 17.165 |
| ...03... | 29478 | -15.153 | [-15.169, -15.136] | 17.153 |
| pooled | 87837 / 3*2^30 | -15.162 | [-15.172, -15.153] | 17.162 [17.153, 17.172] |

Reproduces the memo's 29261 / 28996 / 29147 (cap 17.17). Seed protocol matches the row:
64-bit API seed, default secret, no secret-word randomisation; seed 0 collides
(H = ab88e1e38f85bdca, recorded as a startup witness). 16-byte prefix: 0 hits
(2^30, a different code path); 24-byte prefix: 0 hits (both folds overlap the changed
block); 128 bytes with the block at 0..15: identical event (mismatch 0).

## 2. Mechanism (why the neighbourhood is structured)

For 17..128 bytes, block i of the message (16 bytes) contributes
`fold(w0 ^ (K[2i] + s), w1 ^ (K[2i+1] - s))`, fold(a,b) = lo64(ab) ^ hi64(ab), and the
avalanche is bijective, so complementing one block and leaving the rest collides iff
`fold(A, B) == fold(~A, ~B)` for that block's multiplicands. Writing B = ~b:

* `delta.c`: for uniform A and b = A ^ delta, P[collision] = 1/3 per single-bit delta at
  bits 3..61 (2^-1.585), 1/2 at bits 62-63, 2^-2.6 at bits 1-2, 2^-4.6 at bit 0; two random
  words collide at 2^-22..-27. So the rate is governed by how often A and b agree.
* With w0 = x and w1 = x - D, D := K[2i] + K[2i+1] + 1 (mod 2^64), a seed shift
  s' = s - (x - K[2i]) gives A = x ^ (x + s') and b = G ^ (G + s'), G = x - D: both are
  carry masks of the same s' and differ only where the carry chains of x+s' and G+s' differ.
  The chains can diverge only at bit positions where x and G differ (each with probability
  1/2), i.e. at the digits of a signed-digit representation D = x - G. The
  fairness-pass pair is x = K0 (29 differing positions); `deltastat.c` shows its collisions
  come from the low-popcount tail of delta (P[coll | popcount p] ~ 2^-(p+1)).
* Minimum number of differing positions = weight of the non-adjacent form (NAF) of D
  (digits at position 64 vanish mod 2^64): block 0 (secret words 0,1): 21; block 1
  (words 2,3, used for bytes 16..31 of a 32-byte input): 18; blocks 2-7 (only at 64 or 128
  bytes, L >= 8): 25, 25, 22, 20, 23, 26 (`naf.py`).
* Taking x = P (sum of the +1 digits) and x - D = N (sum of the -1 digits) makes both
  words sparse; `gen_naf.py` also tried all-ones fill (equal rate), the secret words as
  fill and 1500 random fills per block (0.5-1 bit worse).

## 3. Search (fold-only, common random numbers, `foldsearch.c`, 2^24 seeds per candidate)

* `candidates.txt` (18 964): for blocks 0 and 1, the fairness-pass form with single-bit and
  two-bit XOR perturbations of either word (u1/v1/uv1), the seed-shift family
  (K0+t, ~(K1-t)) for |t| <= 2048 and t = +-2^j, moving one word only (a0/a1); plus the
  (K, ~K') form at all eight secret-word offsets. Best: 849/2^24 = 2^-14.27 (block 0,
  t = 2^61), i.e. < 1 bit over the base 408/2^24 = 2^-15.0. `sweep_24.txt`.
* `cand_naf.txt` (5 298): NAF family at all eight offsets. Best per offset (hits / 2^24):
  block 0: 4779 (2^-11.78); block 1: 11762 (2^-10.48); blocks 2-7: 1034, 1450, 2861,
  4275, 2062, 733. `sweep_naf_24.txt`.
* `climb.c`: single-bit hill climbs (flip a bit of w0, of w1, or of x with w1 := x - D;
  accept at > 3 sigma; sample size doubled to 2^28 when stuck) from the block-1 NAF pair
  (zero fill and ones fill), from the best random fill, and from the block-0 NAF pair:
  the NAF zero-fill pair is a local optimum at 2^28 (188646/2^28 = 2^-10.475; ones fill
  188789, indistinguishable); the random fill climbed to 2^-10.54; block 0 reached 2^-11.64.
  `climb_A.txt` .. `climb_E.txt`.
* `climb2.c`: all 18 336 two-move neighbours of the NAF pair at 2^24: best 11994 vs 11832 for the pair itself (sigma ~ 109, +1.5 sigma), i.e. no two-move neighbour is distinguishable from it: the pair is a local optimum under one- and two-move neighbourhoods. `climb2_naf.txt`.
* Not searched: other differentials than the complement (no product identity), other
  lengths (17..31 bytes make the two folds overlap so any change hits both, measured 0/2^30
  at 24 bytes; 33..64 and 65..128 bytes need L >= 8 with weaker NAF weights; the 9..16 path
  adds swap64(lo) + hi which breaks the identity, 0/2^30 at 16 bytes for the fairness pair).

## 4. Best found: block-1 NAF pair, cap 12.47 bits

M  = `00000000000000000000000000000000 51151210404400000000008204000105`
M' = `00000000000000000000000000000000 aeeaedefbfbbffffffffff7dfbfffefa`
(32 bytes each; bytes 16..31 = little-endian (P, N) with P = 0x0000444010121551,
N = 0x0501000482000000, P - N = D = K2 + K3 + 1 = 0xfaff443b8e121551 mod 2^64, the 18 NAF
digits of D; M' complements bytes 16..31.)
Deterministic witness: seed P - K2 = 0x2468b3bc26a44073 gives H(M) = H(M') =
dd686b61e2f6dc69 (both multiplicands of the changed fold are 0 and ~0). Seed 0 does not
collide.

Full-API confirmation, `xxh3_64_pair_check.c` default pair, three fresh 2^30 streams
(salts 0x2026091904000477, ...05..., ...06...), `confirmB_2p30_salt0*.summary.txt`
(source sha256 56c53a9f...; the shipped source 1d5b05ee... differs only by a comment and the
added startup witness, and `final_2p30_salt0*` reruns it on the same salts):

| salt | hits / 2^30 | log2 eps | 95% CI | cap bits (L=4) |
|---|---|---|---|---|
| ...04... | 754887 | -10.4741 | [-10.4774, -10.4708] | 12.474 |
| ...05... | 754932 | -10.4740 | [-10.4773, -10.4708] | 12.474 |
| ...06... | 754106 | -10.4756 | [-10.4788, -10.4723] | 12.476 |
| pooled | 2263925 / 3*2^30 | -10.4746 | [-10.4764, -10.4727] | 12.475 [12.473, 12.476] |

All runs: `event_mismatch_32_fold = 0` (the full-API 32-byte event equals the fold event for
every seed), `event_mismatch_32_128 = 0` (block moved to bytes 112..127 at 128 bytes, L=16
gives 14.47), 24-byte prefix 0 hits, 16-byte prefixes identical (not a pair).
Rerun of the shipped source (sha256 1d5b05ee...) on the same three salts (`final_2p30_salt0*.summary.txt`): identical counts 754887 / 754932 / 754106 (the program is deterministic given the salt), wall 3.8-4.7 min each, 1.5 MB RSS.

## 5. Reproduce

```sh
# Xeon: <xeon-work>/witness/xxh3-64-pair/ ; all files also in this directory
gcc -O3 -std=c11 -Wall -o xxh3_64_pair_check xxh3_64_pair_check.c -lm
nice -n 10 taskset -c 24 ./xxh3_64_pair_check 20                        # smoke (2^20)
nice -n 10 taskset -c 24 ./xxh3_64_pair_check 30 0x2026091904000477     # 754887 hits, ~1-5 min
nice -n 10 taskset -c 25 ./xxh3_64_pair_check 30 0x2026091905000477     # 754932
nice -n 10 taskset -c 26 ./xxh3_64_pair_check 30 0x2026091906000477     # 754106
# fairness-pass pair through the same program:
./xxh3_64_pair_check 30 0x2026091901000477 b8fe6c3923a44bbe83fe7ed308de52e300000000000000000000000000000000 470193c6dc5bb4417c01812cf721ad1c00000000000000000000000000000000   # 29132
# search:
gcc -O3 -std=c11 -o foldsearch foldsearch.c && python3 gen_candidates.py all > candidates.txt && ./run_sweep.sh 24 candidates.txt sweep_24.txt "24 25 26 27 28 29 30 31"
python3 gen_naf.py 1500 > cand_naf.txt && ./run_sweep.sh 24 cand_naf.txt sweep_naf_24.txt "24 25 26"
gcc -O3 -fopenmp -o climb climb.c -lm && OMP_NUM_THREADS=2 ./climb 1 0000444010121551 0501000482000000 24 28 3 7
gcc -O3 -fopenmp -o climb2 climb2.c -lm && OMP_NUM_THREADS=4 ./climb2 1 0000444010121551 0501000482000000 24 7 30
gcc -O3 -o delta delta.c -lm && ./delta 22 ; gcc -O3 -o deltastat deltastat.c -lm && ./deltastat 1 0000444010121551 0501000482000000 26
python3 summarize.py confirmB_2p30_salt04.summary.txt confirmB_2p30_salt05.summary.txt confirmB_2p30_salt06.summary.txt   # exact CIs (scipy)
```

## 6. Adoption notes for the row

* pair: M/M' above, `lengths_bytes [32, 32]`, `L_words 4`, `bits 12.47` (pooled
  2^-10.475, CI [12.473, 12.476]); key model unchanged (uniform 64-bit seed, default secret).
* mechanism text: "bytes 16..31 hold the positive and negative parts (P, N) of the
  non-adjacent form of D = K2+K3+1, the secret words XXH3_mix16B applies to the second block
  of a 32-byte input; M' complements that block. After the seed shift s' = s - (P - K2) the
  two multiplicands are the carry masks P ^ (P+s') and ~(N ^ (N+s')), which agree unless the
  carry chains diverge at one of the 18 digit positions; when they agree the changed fold is
  identical for M and M' (fold(a,~a) = fold(~a,a)), and near-agreement still collides with
  probability about 3^-popcount. Measured 2^-10.47 over the seed; a search result, not a bound."
* Keep (K0,~K1)||0^16 (2^-15.16) and base-1143 (2^-21.0) as historical/supporting pairs.
* The verify program for the row: `xxh3_64_pair_check.c` (header verbatim, XXH_VERSION_NUMBER
  803 pinned as before); startup witnesses include seed P-K2 for the new pair.
