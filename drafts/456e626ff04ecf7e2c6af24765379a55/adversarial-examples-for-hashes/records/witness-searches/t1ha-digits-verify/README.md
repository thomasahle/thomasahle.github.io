# t1ha-digits-verify: independent check of the proposed F60 witness against upstream t1ha

Date 2026-09-19.  Verifier: separate from the searcher; nothing of the searcher's port is used.
Compute: Xeon 8375C (<xeon-host>), `<xeon-work>/witness/t1ha-digits-verify/`,
`nice -n 10 taskset -c 24-31`, 8 threads.  Wall time about 12 min of the 3 h cap.

## Claim under test

Pair F60 for t1ha2_atonce (64-bit output, 64-bit seed), t1ha v2.1.4:
```
m1 (13 B) = c4c0cc2284cd239ede36a18fa6
m2 (16 B) = fbc31866049ec02dea0c4e2be580d3cf
```
claimed eps = 2^-28.19 over a uniform 64-bit one-shot API seed (class density 2^-24 x conditional 2^-4.19),
217 hits at 2^36 uniform seeds (rng 4242), cap 29.19 bits at L = 2.

## Source used (ORIGINAL upstream, not the searcher's port)

Fresh `git clone https://github.com/erthink/t1ha` -> HEAD 00eb779b6c042ccd831ec2f1ae757409c73f39f6
(`git describe`: v2.1.4-10-g00eb779).  A second fresh clone from the primary host
https://gitflic.ru/project/erthink/t1ha.git has the same HEAD and `diff -rq` of `src/` is empty.
`git diff --stat v2.1.4 HEAD -- src/t1ha2.c src/t1ha_bits.h t1ha.h` touches only `src/t1ha_bits.h`
(4 lines, the 2022 ARM unaligned-load fix); `src/t1ha2.c` is byte-identical to tag v2.1.4.

The harness `vu.c` is compiled and linked with the upstream files `src/t1ha2.c`,
`src/t1ha2_selfcheck.c`, `src/t1ha_selfcheck.c` and calls the public API
`t1ha2_atonce(data, len, seed)` from `t1ha.h`.  At start-up it runs upstream's own
`t1ha_selfcheck__t1ha2_atonce()` (81 known answers) and the SMHasher3 verification value
(0x8F16C948); both pass.  Messages are exact-size malloc'ed buffers.

## Seed protocol

Uniform 64-bit seed passed as the third argument of `t1ha2_atonce`; the two fixed messages are
hashed with the same seed and a hit is full 64-bit equality.  RNG: xoshiro256** (the searcher used
raw splitmix64 streams), per-thread states seeded from (rngseed, thread); rng seeds 90210 and 31337,
neither used by the searcher (4242, 777, 20260919).

The class flag printed for each hit uses L = lo(((seed ^ l0) + t) * P1) with
l0 = lo((13 + w0(m1)) * P2) = cc872e34ea2e9b3b and t = zero-extended 5-byte tail of m1 = 000000a68fa136de,
read off the upstream 9..16-byte path (`t1ha2_tail_ab` -> `mixup64` with P2 then P1).  For the
conditional runs the seed is recovered from a uniformly drawn class member L by
seed = ((L * P1^-1) - t) ^ l0 (P1 is odd, so seed <-> L is a bijection and the class density is
exactly 2^-24).

## Results (all against upstream `t1ha2_atonce`)

| check | result |
|---|---|
| upstream selfcheck `t1ha_selfcheck__t1ha2_atonce()` | ok; SMHasher3 verification value 8f16c948 ok |
| explicit seeds 4c240c2749cd4915, a2b08b86e150940c | COLLIDE, H = c35b49165b1a4480 and b94c250505cd61c6 (as claimed) |
| explicit seeds e486a312cbae8cc1, 091e258d8d7468d1, 881464cd3ea41895 | COLLIDE (H = fa0188f928c98b01, 3de3a6479d7ae399, ac5c7c5bcf5b61ee) |
| control seeds 3c805cc67a687f30 (pair A's seed), 0 | differ |
| the searcher's 217 logged hits (rng 4242) | 217/217 COLLIDE under upstream |
| conditional, 2^28 class samples (rng 90210) | 14,675,107 hits, 2^-4.1931 => 2^-28.193 (searcher: 14,682,263, 2^-4.192) |
| conditional, 2^30 class samples (rng 31337) | 58,715,203 hits, 2^-4.1928 +- 0.0004 (exact 95%) => class contribution 2^-28.193 |
| fresh uniform 2^36 seeds, rng 90210 | **226 collisions, 226 in class**, 2^-28.180, exact 95% CI [2^-28.374, 2^-27.992]; 2m18s wall |
| fresh uniform 2^36 seeds, rng 31337 | **258 collisions, 258 in class**, 2^-27.989, exact 95% CI [2^-28.170, 2^-27.813]; 2m03s wall |
| pooled fresh, 2^37 seeds | 484 collisions, 2^-28.081, exact 95% CI [2^-28.213, 2^-27.953] (expected 449 at 2^-28.19) |
| pooled with the searcher's 217 @ 2^36 | 701 collisions in 3 x 2^36 seeds, 2^-28.132, exact 95% CI [2^-28.241, 2^-28.025] |

No seed appears in both fresh runs (disjoint samples).  Every one of the 484 fresh hits lies in the
stated 24-bit L class, so the class accounts for all observed collisions and eps is the class
contribution 2^-24 x 2^-4.193 = 2^-28.19, whose statistical error (+-0.0004 bits at 2^30) is far
below the uniform runs' resolution.  The second uniform run (258 vs 224 expected) is a +2.3 sigma
fluctuation (two-sided Poisson p ~ 0.03); the pooled 2^37 CI [2^-28.21, 2^-27.95] contains 2^-28.19.
There is no evidence of collisions outside the class that would raise eps further.

Verdict: CONFIRMED.  eps = 2^-28.19 (class contribution, 2^30 conditional samples); uniform-seed
rate 2^-28.08 [2^-28.21, 2^-27.95] at 2^37 fresh seeds.  bits = log2(2) + 28.19 = 29.19 -> row
score 29.2 (cap; sampled), replacing 31.0.  Adopt.

## Reproduction (Xeon, <xeon-work>/witness/t1ha-digits-verify)

```
git clone https://github.com/erthink/t1ha t1ha-erthink && cd t1ha-erthink    # HEAD 00eb779, v2.1.4-10
git diff --stat v2.1.4 HEAD -- src/t1ha2.c                                     # empty
cp ../vu.c . && gcc -O2 -std=gnu11 -I. -o vu vu.c src/t1ha2.c src/t1ha2_selfcheck.c src/t1ha_selfcheck.c -lpthread -lm
M="c4c0cc2284cd239ede36a18fa6 fbc31866049ec02dea0c4e2be580d3cf a5845081f808f44a 2080000118003408"
./vu selftest
./vu check c4c0cc2284cd239ede36a18fa6 fbc31866049ec02dea0c4e2be580d3cf 4c240c2749cd4915 a2b08b86e150940c e486a312cbae8cc1 091e258d8d7468d1 881464cd3ea41895
nice -n 10 taskset -c 24-31 ./vu cond $M 28 8 90210      # 14675107 hits, 2^-4.1931 (0.9 s)
nice -n 10 taskset -c 24-31 ./vu cond $M 30 8 31337      # 58715203 hits, 2^-4.1928 (4 s)
nice -n 10 taskset -c 24-31 ./vu uniform $M 36 8 90210   # 226 hits, all in class (2m18s)
nice -n 10 taskset -c 24-31 ./vu uniform $M 36 8 31337   # 258 hits, all in class (2m03s)
python3 poisson_ci.py 226 36 258 36 484 37               # exact Garwood 95% CIs
```

## Files

- `vu.c` harness (links upstream sources, no re-implementation); `poisson_ci.py` exact Poisson CI;
  `searcher_hits_F60.txt` the searcher's 217 logged seeds (plus the literal "4242" from its rngseed line, which is checked as seed 0x4242 and differs);
- `xeon_logs/`: `cond_F60_2p28_rng90210.txt`, `cond_F60_2p30_rng31337.txt`,
  `uniform_F60_2p36_rng90210.txt`, `uniform_F60_2p36_rng31337.txt` (every hit with seed, H, L, in-class flag),
  `run_uniform.sh`.
