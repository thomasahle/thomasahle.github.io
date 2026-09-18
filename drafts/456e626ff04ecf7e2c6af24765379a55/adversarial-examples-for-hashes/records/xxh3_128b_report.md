# XXH3-64 fixed-pair collision reproduction

The high collision-rate phenomenon **reproduces**, and exceeds the appendix's approximately 2^-23.3 rate. For the requested 128-byte, complement-both-words family, base **1143** gives **504 and 526 collisions in two separate runs of 2^30 seeds**. Pooling only those two confirmation runs gives 1030 / 2^31 = 4.79631125927e-07, or **2^-20.9916**, with **bits = 24.9916**. This is 32.19 times 2^-26 and 4.95 times the rounded published rate. The original lost pair is not recovered; these are newly generated explicit pairs.

The proposed explanation involving the rest of the message is incorrect for this family: **none of the other chunks' products can affect collision equality at lengths 64, 96, or 128**. At 128 bytes there are **eight** `mix16B` calls (four pairs), not four calls. Changing the first two words changes the collision rate; changing only the remaining 112 bytes cannot. Both the source proof and the controls below establish this distinction.

## Method and results

Used only the supplied local xxhash 0.8.3 header, `XXH_INLINE_ALL`, `XXH3_64bits_withSeed`, its default secret, and two execution threads (main plus one worker). No network access, additional hash library, or imported collision counts. The supplied `collision_driver.cpp` has missing harness dependencies here, so [scan.cpp](scan.cpp) is a standalone harness preserving its full-API comparison and two-stream conventions. Every sampled trial compares both complete 64-bit API outputs. An additional first-fold equality assertion is made on every collision; it is not a replacement for the API comparison.

Each of three 128-byte families was screened over **2000 random bases × 2^22 seeds**. All 16 words were randomized, with w1 overwritten for family 2. The 20 highest counts were remeasured at 2^28, then the top three by that new measurement were tested twice at 2^30. Ties use increasing base ID. Confirmation runs use fresh streams distinct from screening, refinement, and each other. We also completed the entire 2000 → 20 → 3 pipeline at 64 and 96 bytes for the complement-both family.

| Family | Base ID | Screen / 2^22 | Refine / 2^28 | Run 1 / 2^30 | Run 2 / 2^30 | Pooled log2(rate) | Pooled bits | 95% rate interval |
|---|---:|---:|---:|---:|---:|---:|---:|---|
| both | 1143 | 2 | 146 | 504 | 526 | -20.9916 | 24.9916 | [4.51e-07, 5.1e-07] |
| both | 710 | 2 | 71 | 327 | 300 | -21.7077 | 25.7077 | [2.7e-07, 3.16e-07] |
| both | 62 | 1 | 30 | 138 | 133 | -22.9179 | 26.9179 | [1.12e-07, 1.42e-07] |
| w0 only | 171 | 1 | 5 | 10 | 10 | -26.6781 | 30.6781 | [5.69e-09, 1.44e-08] |
| w0 only | 332 | 1 | 5 | 8 | 5 | -27.2996 | 31.2996 | [3.22e-09, 1.04e-08] |
| w0 only | 96 | 1 | 4 | 7 | 16 | -26.4764 | 30.4764 | [6.79e-09, 1.61e-08] |
| related base | 364 | 1 | 6 | 12 | 11 | -26.4764 | 30.4764 | [6.79e-09, 1.61e-08] |
| related base | 125 | 1 | 5 | 10 | 6 | -27.0000 | 31.0000 | [4.26e-09, 1.21e-08] |
| related base | 69 | 1 | 4 | 7 | 11 | -26.8301 | 30.8301 | [4.97e-09, 1.32e-08] |

Neither the w0-only variant nor the related-base variant produced a confirmed rate above approximately 2^-25 after its 2000-base scan. This is a search result, not an upper bound on all possible pairs.

Rate estimates and intervals in the table use only the two confirmation runs (2^31 total seeds per row); screen/refinement results are selection data and are excluded. Intervals are 95% exact Poisson intervals for these rare counts, without simultaneous-comparison correction. They quantify sampling uncertainty for these selected pairs, not a global maximum. Selecting the best three before confirmation avoids reusing the noisy screen as an estimate. A 2^22 screen is sparse and can miss strong candidates; the scan is not exhaustive.

`pairs.json` contains 32 measurement records: both 2^30 confirmation runs for every finalist at every tested family/length, plus two representative tail controls. Each has `m_hex`, `m2_hex`, `length`, `trials_log2`, `count`, `rate_log2`, `bits`, `example_seed`, and `outputs`, plus every collision witness and a reproduction command. Repeated records of the same pair are separate runs, not distinct pairs. Hex is byte order; each 64-bit word is little-endian. As requested, `bits = log2(16) - rate_log2 = 4 - rate_log2` throughout, including the shorter-length control records. It is the requested derived statistic, not the 64-bit output width. Zero-count rows in raw JSONL use null for the unbounded log-rate and bits and for the absent witness.

## Why the first words matter and the remaining chunks do not

The API dispatches through [xxhash.h:6199](ref/xxhash.h#L6199) to [XXH3_len_17to128_64b](ref/xxhash.h#L4766), not `XXH3_len_129to240_64b`. At 128 bytes the input/secret offsets of its eight calls are:

| Input byte offset | Input words | Secret byte offset |
|---:|---|---:|
| 0 | w0,w1 | 0 |
| 16 | w2,w3 | 32 |
| 32 | w4,w5 | 64 |
| 48 | w6,w7 | 96 |
| 64 | w8,w9 | 112 |
| 80 | w10,w11 | 80 |
| 96 | w12,w13 | 48 |
| 112 | w14,w15 | 16 |

Set M=2^64. For each fixed seed s, let C(s) be `length * XXH_PRIME64_1` plus the seven unchanged folds, all modulo M. Then the two accumulators are C(s)+F(s) and C(s)+F'(s). Addition of the same C(s) is a bijection modulo M, so it cancels **exactly**, including all carries and wraparound. The [final avalanche](ref/xxhash.h#L4583) consists of right-xorshifts by 37 and 32 and multiplication by the odd constant 0x165667919e3779f9. Each operation is invertible on 64-bit words. Therefore:

```
H_s(m) = H_s(m2)  iff  F_s(w0,w1) = F_s(w0',w1').
```

The exact same equivalence holds at 64 bytes (four calls) and 96 bytes (six calls): the first 16 bytes occur in just one mix call with secret offset zero. The unchanged tail and length alter the common hash output, but never the set of colliding seeds. This argument applies when the changed bytes occur only in that first call; it should not be extrapolated to overlapping short inputs or the differently mixed 129..240 path.

Inside [mix16B](ref/xxhash.h#L4732), let k0 and k1 be the first two little-endian secret words:

```
a = w0 XOR ((k0 + s) mod M)
b = w1 XOR ((k1 - s) mod M)
F = low64(a*b) XOR high64(a*b)
```

Complementing input words complements the corresponding mixed operands. Thus family 0/2 compares `fold(a*b)` with `fold((M-1-a)*(M-1-b))`; family 1 compares `fold(a*b)` with `fold((M-1-a)*b)`. This is cancellation **within the high/low multiplication fold**, not between different message chunks. If `P=a*b=H*M+L` and `P'=H'*M+L'`, collision is equivalent to `L XOR L' = H XOR H'`.

There is a simple reason the first-word choices can change rates despite a uniform seed: the pre-XOR operands x=(k0+s) mod M and y=(k1-s) mod M have fixed sum k0+k1 mod M. They are correlated, not two independent random words. The pair w0,w1 changes how XOR maps that one-dimensional seed distribution into a,b, and hence the carries and equal high/low XOR differences in the two products. For both complements, the exact integer identity is

```
P' - P = (M-1)*(M-1-a-b).
```

For w0 alone, `P' = (M-1)*b - P`. These identities locate the relevant arithmetic entirely in the first chunk. The empirical scan establishes how large the dependence on w0,w1 can be; it does not derive a closed-form probability for each prefix.

For a concrete witness from base 1143, first confirmation run:

```
w0       = 0x3664c49fdaa31289
w1       = 0x117d8b23bea10282
seed     = 0xc8eae1baae13330b
a        = 0xb15241423d23234a
b        = 0x42bfcb1fc0ccccf3
P        = 0x2e3c0de3fc6fa644382ac105a771773e
P'       = 0x3a2a0181fe7fb6062c3ccd67a561677c
L XOR L' = 0x14160c6202101042
H XOR H' = 0x14160c6202101042
fold(P)  = fold(P') = 0x1616cce65b1ed17a
H_s(m)   = H_s(m2) = 0x448e3716c8effb94
```

The products themselves differ; the equal XOR differences make their folds identical. The full messages and all other witnesses are in `pairs.json`.

## Tail and length controls

To test the literal “scan the rest of the message” proposal, we also held base 1143's first 16 bytes fixed and generated **2000 independently randomized 112-byte tails**, each tested at **2^22 seeds using the same seed stream** as its original screen. Every tail produced exactly **2 collisions at precisely the same seed/index values**. The raw evidence is `scan_tails.jsonl`; tail IDs 1 and 2000 are included in `pairs.json`. Reusing seeds here is deliberate, to test event-by-event invariance. These repeated trials are not independent evidence for estimating a collision probability.

The 64- and 96-byte pipelines reuse the 128-byte complement-both prefixes, truncated messages, and seed streams. **All 2026 measurement records per length have exactly the same counts and collision seed/index lists as their 128-byte counterparts**, across all screening, refinement, and confirmation stages. Their common collision outputs change with length. These controls must not be pooled with the 128-byte data as additional independent observations.

Thus the appendix's rate magnitude is reproducible, but its statement that the rate “depends on the rest of the message” is incompatible with this source path if “rest” means bytes 16 onward. If it meant changing other bytes within the modified first chunk (for example choosing a different w1), that is consistent with these results. The original unpreserved experiment cannot be diagnosed further from the excerpt.

## Audit and reproduction

| Raw file | Length | Family | Screen histogram {count: number of bases} | Total screen hits | Measured seconds, all stages |
|---|---:|---:|---|---:|---:|
| scan_both.jsonl | 128 | 0 | {0: 1931, 1: 60, 2: 7, 3: 2} | 80 | 108.84 |
| scan_w0.jsonl | 128 | 1 | {0: 1936, 1: 61, 2: 3} | 67 | 110.09 |
| scan_related.jsonl | 128 | 2 | {0: 1909, 1: 91} | 91 | 109.27 |
| scan_64.jsonl | 64 | 0 | {0: 1931, 1: 60, 2: 7, 3: 2} | 80 | 58.88 |
| scan_96.jsonl | 96 | 0 | {0: 1931, 1: 60, 2: 7, 3: 2} | 80 | 86.53 |

Family numbers: 0=both complements, unrelated words; 1=w0 only; 2=both complements after setting w1=~w0. There were 109,387,448,320 seed/pair trials total, including deliberately repeated-seed controls, and 520.99 seconds summed inside measurement calls. Each trial hashes both inputs. The 2000-tail control alone took 47.39 measured seconds. Environment: Darwin arm64, Apple clang 17.0.0 (clang-1700.6.4.2), `-O3 -std=c++17 -pthread`.

The generator is xoshiro256** initialized using four SplitMix64 outputs. Base ID i uses initialization 0x6261736500000000+i and emits 16 little-endian words. Tail ID t uses 0x7461696c00000000+t. The trial stream salt is 0x2026091700000000 + variant*0x100000000 + stage*0x1000000 + base_id, with stages 0=screen, 1=refinement, 2/3=confirmations. Worker t initializes xoshiro with `(salt * 0x9e3779b97f4a7c15 + 7919*t + 1) mod 2^64`, as in the supplied harness convention, and supplies half the trials. These are deterministic pseudorandom samples of full-width 64-bit API seeds, not an exhaustive enumeration or a claim of mathematically independent PRNG streams. Each persisted salt is a hex string to avoid JSON integer precision loss.

The supplied header reports version 803 and SHA-256 `17973c0dc49d9854ca26caa191f0e12f7a424b68858d9a78de3860d959d85e4b`. Validation reproduced the supplied driver's XXH3-64 verification value **0x1AAEE62C**, and passed **1,800,000** comparisons of full API equality against first-fold equality over three families, three lengths, and randomized tails. In addition, [verify_pairs.py](verify_pairs.py) independently reconstructs every recorded full hash output in Python arbitrary-precision integer arithmetic, reading the default secret bytes from the header. `validation.txt` records its results. `build_report.py` checks selection order, record counts, and exact seed/index equality for the length and tail controls before producing this report.

```sh
c++ -O3 -std=c++17 -pthread scan.cpp -o scan
./scan validate
./scan pipeline 0 scan_both.jsonl
./scan pipeline 1 scan_w0.jsonl
./scan pipeline 2 scan_related.jsonl
./scan pipeline 0 scan_64.jsonl 2000 64
./scan pipeline 0 scan_96.jsonl 2000 96
./scan tails 1143 2000 scan_tails.jsonl
python3 build_report.py
python3 verify_pairs.py scan_both.jsonl scan_w0.jsonl scan_related.jsonl scan_64.jsonl scan_96.jsonl scan_tails.jsonl pairs.json
```

Run these commands sequentially to retain the two-thread limit. Python report generation uses locally available SciPy solely for confidence intervals. Individual measurements can be reproduced with the `reproduce_cmd` attached to every record in `pairs.json`, without SciPy. The confirmation runs in that file use `trials_log2=30`; pooled 2^31 estimates appear only in this report.
