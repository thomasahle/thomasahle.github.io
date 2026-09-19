# a5hash v5.21 (64-bit): search of the unexplored lengths -- no improvement

Row: `a5` (a5hash v5.21, 64-bit), key model "Uniform 64-bit UseSeed" (the hidden seed is uniform over
2^64; the attacker fixes the pair). Metric: bits = log2(L / eps), L = ceil(len/8) words, eps = Pr over
the seed of a full 64-bit collision. Current witness: len 6615, first-block dead operand
W* = 0x8e5caae000000000, class 118 x 2^19 seeds, eps = 2^-38.117, L = 827, cap 47.81 bits.

Result: **nothing beats 47.81.** The three unexplored points named in the record (length 4567, heavy
values of the second state word, lengths <= 16) and the 52 next-ranked (len, v) points all give
caps between 48.22 and 59.19 bits. The 6615/37 row stays.

## Mechanism (unchanged from the record)

`S1_init = lo64(X*Y)` with `X = K2^len ^ (seed & AA..)`, `Y = K1^len ^ (seed & 55..)`. With `v2(X) = p`,
`v2(Y) = q`, `v = p+q`, the value is `2^v * (X''Y'' mod 2^m)`, `m = 64-v`. The seed bits that matter are
`cost` (bits forcing the low zeros and the leading ones) plus the free bits of `X''`, `Y''` below `m`;
`S = cost + fx + fy`, and `Pr[S1_init = W*] = (number of free-bit assignments hitting W*) / 2^S`, exact.
Which `(p, q)` are feasible is decided by the bits of `K2^len` (even positions) and `K1^len` (odd
positions), so `len` selects the reachable `v`. A pair whose first block word is `W*` (len >= 17) or whose
tail word A is `W*` (len 8..16, second message = A^1, both hash to 0; len 1..3, A < 2^(8 len)) collides on
the whole class. For len 4..7 both tail operands are the same message word and no second message
collides on the class, so those lengths are excluded by construction.

## Method

1. `candidates.py`: every feasible `(len, p, q)` for len <= 4096 plus every length pattern below 2^22
   (the constrained low bits of len, with up to two extra free bits set), ranked by
   `log2(L) + S - h_est(m)` with `h_est` fitted to the earlier exact enumerations (v odd; even v is
   near-uniform, enum 6615/36 gave mode 1). Output in `candidates_out.txt`.
2. `modebeam.c`: for a `(len, v)` the histogram of the low `min(m, 33)` residual bits is computed
   EXACTLY (2^33 x uint16 = 16 GB, all 2^32 assignments of the free bits below level 33); when `m <= 33`
   that is the exact mode. Otherwise the 65,536 heaviest level-33 prefixes are extended one residual bit
   at a time (beam width 65,536, children ranked by exact count x 2^(remaining free bits)); the final
   count for the chosen `W*` is exact for that value, so `eps` is exact, and the beam is only a lower
   bound on the maximum over `W*`.
3. Verification in the same run: the whole seed class of `W*` is enumerated (the S determined bits from
   the assignment list, the 64-S don't-care bits exhaustively) and both messages are hashed with the
   validated a5hash-64 (`a5.h`, SMHasher3 verification 0xADDE79B3 / 0x89406B11 / 0x14AD402C reproduced by
   `verify.c`); every class seed must give `S1_init == W*` and a full-output collision; control on
   uniform random seeds (2^24, or 2^32/(len+1) for long messages).
4. Cross-checks: 6615/37 reproduces the record exactly (118 x 2^-45, all 61,865,984 class seeds collide);
   len 23 gives 0x37eb13a49993e000 / 168,320 seeds, identical to the earlier width-32768 beam
   (`~/agents/a5hash/beam23_w32768.txt`); len 8 gives pair 2 of verify/a5hash (7290 seeds); len 15 and
   len 16 give the values of `beam15_w65536.txt` and `a5hash-weakseed/fiber_len16.txt` (26688, 5626).
5. `s2mode.c`: 2^31 uniform seeds at len 17, 23, 6615, second state word `S2_init = hi64(X*Y)`
   radix-sorted; maximal multiplicity.

Machine: Xeon 8375C (hardware.normalcomputing.net), `nice -n 10 taskset -c 24-31`, 8 threads, 16 GB
per run, 22:44-00:09 UTC 2026-09-18/19 for the 55-point list (log per point in `logs/<len>_<v>.log`,
`logs/driver.log` is the sequence), plus the wide-beam reruns below (00:09-00:13 UTC). Total Xeon time about 1 h 30 min on 8 cores.

## Results (all 55 points; `summary_table.txt` is the machine-made version)

| cap (bits) | len | v | (p,q) | method | W* | class seeds (all collide) | eps |
|---|---|---|---|---|---|---|---|
| **47.81** | 6615 | 37 | (20,17) | exact | 0x8e5caae000000000 | 61,865,984 / 61,865,984 | 2^-38.117 |
| 48.22 | 23 | 13 | (6,7) | beam | 0x37eb13a49993e000 | 168,320 | 2^-46.639 |
| 48.67 | 18 | 7 | (0,7) | beam | 0xc6d1ae53c99a9380 | 123,776 | 2^-47.083 |
| 49.09 | 135 | 15 | (4,11) | beam | 0x023cf182f19f8000 | 524,032 | 2^-45.001 |
| 49.27 | 87 | 15 | (8,7) | beam | 0xc52b2e6c944c8000 | 299,264 | 2^-45.809 |
| 49.46 | 471 | 23 | (12,11) | beam | 0x304ba0de44800000 | 1,400,832 | 2^-43.582 |
| 49.57 | 55 | 11 | (6,5) | beam | 0x6293044a87e38800 | 154,816 | 2^-46.760 |
| 49.60 | 6615 | 35 | (19,16) | exact | 0x642783e800000000 | 17,825,792 | 2^-39.913 |
| 49.71 | 31 | 9 | (6,3) | beam | 0xd468e47b45b04e00 | 80,352 | 2^-47.706 |
| 49.78 | 215 | 19 | (8,11) | beam | 0xde4a2e6badc80000 | 514,048 | 2^-45.028 |
| 49.81 | 5079 | 29 | (20,9) | beam | 0x59d07d4620000000 | 11,862,016 | 2^-40.500 |
| 49.89 | 19 | 9 | (2,7) | beam | 0x87332d40c0ae6200 | 53,184 | 2^-48.301 |
| 50.00 | 2519 | 29 | (12,17) | beam | 0xa88c01f920000000 | 5,144,576 | 2^-41.705 |
| **50.16** | **4567** | 31 | (20,11) | **exact** | 0x58f097a780000000 | 8,388,608 | 2^-41.000 |
| 50.30 | 15 | 7 | (4,3) | beam | 0x4f8b21ad70d30380 | 26,688 | 2^-49.296 |
| 51.17 | 8 | 1 | (0,1) | beam | 0x1248299ed31fa942 | 7,290 | 2^-51.168 |
| 53.12 | 3 | 9 | (2,7) | beam, A < 2^24 | 0x0000000000198e00 | 1,888 | 2^-53.117 |
| 55.09 | 2 | 7 | (0,7) | beam, A < 2^16 | 0x0000000000006a80 | 480 | 2^-55.093 |
| 59.19 | 1 | 3 | (2,1) | beam, A < 2^8 | 0x00000000000000a8 | 28 | 2^-59.193 |

The other 36 points (lens 9-14, 16, 17, 21, 22, 39, 71, 119, 131, 147, 151, 199, 343, 375, 407, 983,
2199, 2455, 2263, 3287, 3543, 4439, 4951, 6487, 6999, 12759, 14807, 22999 and second v's of 471, 2519,
4567) fall between 50.18 and 52.58 bits; see `summary_table.txt`. Every class was verified exhaustively
(class collide/tested equal in every row, S1_init mismatches 0) and every control count is 0, as the
class densities predict (expected 1e-6 .. 1e-10 per control run).

### The three named points

* **Length 4567 (33-bit residual, L = 571).** Exact: the heaviest residual has weight 128 out of 2^48
  (2^-41.000; the runner-up 120), cap 50.16 bits, 2.35 bits worse than 6615. v = 29 at the same length
  gives 51.29. The 33-bit residual simply does not concentrate enough to pay for the 16-bit cost.
* **Heavy values of the second state word.** `S2_init = hi64(X*Y)` never repeats more than twice in
  2^31 seeds at len 17, 23 and 6615 (5, 7, 5 duplicate pairs; a value with Pr = 2^-28 would show ~8
  hits). The duplicate counts give a collision entropy of about 58 bits, i.e. the high half behaves
  like a 59-63-bit product with the Benford-like top-bit skew already recorded (bit 62 = 1 w.p. 1/4),
  not like a many-to-one word. Structurally, hi64 has no 2-adic prefix structure (bit j of the high
  half depends on all lower bits through the carries), so the lifting/histogram mechanism that gives
  S1_init its heavy values does not exist for it; a dead W1 = S2_init class would need Pr >= 2^-46.2
  at L = 3 to matter. Sample, not a bound; stated at its resolution.
* **Lengths <= 16.** Lengths 8-16 (tail path, second message = A^1, both hash to 0) give 50.30 (len 15)
  to 52.54 bits; lengths 1-3 are killed by the A < 2^(8 len) constraint on the tail word (53.1-59.2);
  lengths 4-7 have no valid pair (both tail operands are the same word). The record's "57-bit residual"
  point is len 2 / (0,7): 55.09 bits. The per-length beam values at 8, 15, 16 coincide with the
  earlier independent beam/tower searches, so the beam width is not the limitation there.

### Wide-beam reruns of the two closest lengths

Beam width 2^19 (8x) on len 18 and len 23 -- see `logs/18_7_wide.log`, `logs/23_13_wide.log`:

| len | beam | W* | weight | class seeds (all collide) | eps | cap |
|---|---|---|---|---|---|---|
| 18 | 65,536 | 0xc6d1ae53c99a9380 | 7736 | 123,776 | 2^-47.083 | 48.67 |
| 18 | 524,288 | 0xc482a960c688ef80 | 7754 | 124,064 | 2^-47.079 | 48.66 |
| 23 | 65,536 | 0x37eb13a49993e000 | 1315 | 168,320 | 2^-46.639 | 48.22 |
| 23 | 524,288 | 0x52f58e6a5173e000 | 1326 | 169,728 | 2^-46.627 | 48.21 |

An 8x wider beam changes the heaviest class by 0.2 % (len 18) and 0.8 % (len 23): the beam is saturated,
and a factor 1.8 (len 18) or 1.3 (len 23) in class size would be needed to reach 47.81. Both wide classes
were verified exhaustively (124,064 / 124,064 and 169,728 / 169,728 collide, controls 0 / 2^24).

## Why no length can plausibly do it

The cap decomposes as `log2(L) + S - log2(mode)`. Increasing v lowers S by about v/2 but forces more
low bits of len, so the shortest admissible length grows about as 2^(max(p,q)); 6615 = 0x19d7 is the
lucky pattern where the fixed bits of K1 and K2 leave a 13-bit length with v = 37. Its mode 118 at
m = 27 is exact. Shorter lengths (L <= 3) only reach v <= 13, so S >= 57 and the residual mode
(<= 2^12.9 found, 2^13.8 needed for 47.81 at L = 3) would have to be about 2x heavier than anything the
beam finds at m = 51..57; the 8x wider beam on 18 and 23 is the check of that.

## Reproduce

```sh
ssh thomas-ahle@hardware.normalcomputing.net
cd ~/agents/witness/a5hash-lengths            # this directory's a5.h verify.c modebeam.c s2mode.c candidates.py run_all.sh
gcc -O2 -std=gnu11 -pthread -o verify verify.c -lm && ./verify           # three SMHasher3 values OK
gcc -O3 -march=native -std=gnu11 -pthread -o modebeam modebeam.c -lm
nice -n 10 taskset -c 24-31 ./modebeam 6615 37 -t 8    # record: 118 x 2^-45, 61865984/61865984 collide, 47.81
nice -n 10 taskset -c 24-31 ./modebeam 4567 31 -t 8    # exact: 128 x 2^-48, cap 50.16
nice -n 10 taskset -c 24-31 ./modebeam 23 13 -t 8      # beam:  168320 seeds, cap 48.22
nice -n 10 taskset -c 24-31 ./modebeam 18 7 -t 8 -b 524288   # wide beam
nice -n 10 taskset -c 24-31 ./modebeam 3 9 -t 8        # len<=3: A < 2^(8 len) constraint applied automatically
./run_all.sh                                           # the whole 55-point list, logs/<len>_<v>.log
gcc -O3 -march=native -std=gnu11 -pthread -o s2mode s2mode.c -lm && ./s2mode 23 31 8 2   # S2_init sample
python3 candidates.py > candidates_out.txt             # ranked candidate list (local, 13 s)
python3 summarize.py logs > summary_table.txt          # table from the logs
```

Every run is deterministic (the class enumeration is exhaustive; the control RNG is fixed); the only
run-to-run variation is wall time. The pair bytes of every point except the validation run 6615/37 (whose pair is the page's witness) are in
`logs/pair_<len>_<v>.txt` (both messages, hex, one per line).
