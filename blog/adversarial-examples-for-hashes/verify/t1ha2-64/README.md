# t1ha2_atonce-64 v2.1.4: selected F60 pair

The selected pair has lengths 13/16 bytes, L = 2, and estimated class contribution 2^-28.1928, giving a 29.19-bit cap. The public API accepts one uniform 64-bit seed. This cross-length result does not apply unchanged to an equal-length-only model.

```text
M  = c4c0cc2284cd239ede36a18fa6
M' = fbc31866049ec02dea0c4e2be580d3cf
key words = 4c240c2749cd4915
output = c35b49165b1a4480
```

Let `l0 = lo((len + w0)*P2)`, `t = tail`, and `Lseed = lo(((seed ^ l0) + t)*P1)`. The sufficient class is `(Lseed & a5845081f808f44a) == 2080000118003408`. The map from seed to Lseed is bijective, so its 24 fixed bits give density exactly 2^-24. The five-digit route is +2^60 +2^54 -2^48 -2^9 -2^1. The remaining multiplication carries cancel at a measured conditional rate of 58,715,203/2^30 = 2^-4.1928 against upstream, with score uncertainty about ±0.0004 bits. Collisions outside this sufficient class could only strengthen the cap; total equality is not proved.

```sh
cc -O2 -std=c11 t1ha2_64_verify.c -lm -o check
taskset -c 24-31 nice -n 10 ./check 24
```

The program validates all 81 upstream known answers and SMHasher3 0x8F16C948. It samples full seeds and the class with xoshiro256**/SplitMix64, and checks the explicit witness. F60 is first; historical A and B remain controls, with RNG seed `1 + pair index`.

Observed on the Xeon: F60 **916,680 / 2^24 class samples**, 0 / 2^24 uniform seeds. Historical A and B gave 1,048,557 and 1,039,989 class hits, with zero uniform hits. All three explicit witnesses collided. [Full output](run_selected_xeon.txt). The deterministic `20` run gave uniform/class counts `0 57150 0 65785 0 65084` ([log](run_check_xeon.txt)). Small uniform samples are not rate estimates.

[Search](../../records/witness-searches/t1ha-digits/README.md); [independent upstream verifier](../../records/witness-searches/t1ha-digits-verify/README.md), which measured 226 and 258 hits per 2^36 fresh uniform seeds; searcher 217/2^36, all in class. [Historical A/B documentation](HISTORICAL.md) retains older figures and run seeds. Original zlib notices are retained in the source.

All page-pass executions were run on the Xeon on 2026-09-19 with `taskset -c 24-31 nice -n 10`. These small reruns validate the package and deterministic counts; the larger independently reproduced records linked below supply the published estimates. Messages are fixed before the key is sampled.
