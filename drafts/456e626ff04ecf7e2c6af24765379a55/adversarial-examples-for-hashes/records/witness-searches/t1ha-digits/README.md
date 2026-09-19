# t1ha-digits: signed-digit route re-search for t1ha2_atonce-64 (v2.1.4)

Date 2026-09-19.  Compute: Xeon 8375C (<xeon-host>), `<xeon-work>/witness/t1ha-digits/`,
`nice -n 10 taskset -c 24-31`, 8 threads (cores shared with other witness jobs; wall times below are
under that contention).  Total Xeon budget used: about 75 min wall of the 3 h cap (22:29-23:41 server time).

Metric: bits = min over L (8-byte words) of log2(L / eps), eps = Pr over a uniform 64-bit one-shot
API seed that the fixed pair collides.  Both pairs below have L = 2 (9..16-byte messages), so
bits = 1 - log2(eps).

## Result

**Adopt.**  New worst pair ("F60"), 13 vs 16 bytes:

```
m1 (13 B) = c4c0cc2284cd239ede36a18fa6
m2 (16 B) = fbc31866049ec02dea0c4e2be580d3cf
```

| quantity | value | source |
|---|---|---|
| seed class | (L & a5845081f808f44a) == 2080000118003408, L = lo(((seed ^ l0) + t) * P1); 24 fixed bits, density exactly 2^-24 (L is a bijective image of the seed) | dp5 line, realised in C |
| conditional rate | 14,682,263 / 2^28 class samples = 2^-4.192 (+-0.001 bits) | `cond2 ... 28 8` |
| class contribution | 2^-24 x 2^-4.192 = **2^-28.19** | |
| fresh uniform seeds | 2^36 seeds, rng seed 4242: **217 collisions, all 217 inside the class**; rate 2^-28.24, exact Poisson 95% CI [2^-28.44, 2^-28.05]; 224 expected at 2^-28.19 | `direct_class ... 36 8 4242` |
| bits | log2(2) + 28.19 = **29.19** (cap; sampled) vs 31.0 on the page | |
| example seeds | 4c240c2749cd4915 (H = c35b49165b1a4480), a2b08b86e150940c (H = b94c250505cd61c6), e486a312cbae8cc1, 091e258d8d7468d1, 881464cd3ea41895 | direct hits; re-checked with the website's own verify implementation (`indep_check`) |

Mechanism (same family as the page's pair A, one more digit): 5-digit signed-digit route on the
word-0 product, l* - l = +2^60 +2^54 -2^48 -2^9 -2^1 (Da = 103efffffffffdfe), tail difference
du = e012803e9bacd40a with the u+du wrap taken (wrapu = 1, c64 = 0), x+d0 wrap (wrapx = 1),
length offset +3 (13-byte m1, 16-byte m2), carry pattern E = a5845081f808f44a of weight 24.
The model charges 24 (class) + 5 (digit signs) + 0.19 (wrap) = 29.19; the measured class rate is
2^-4.19, not 2^-5, because the two low digits (positions 1 and 9) have signs that are strongly
correlated with the fixed low bits of L (a = u - t with u = L * P1^-1; L's bits 1, 3, 6, 10, 12-15 are
fixed by the class), and the tail word t of the realised pair happens to be one that makes that
correlation favourable: over 4096 random tails at the same class, the median rate is 2^-5.19 and
the best 2^-4.19 (`tune`).  The 1-bit gain over the model is therefore a property of this specific
pair, which is what the metric scores.

Runner-up ("F1"), 10 vs 16 bytes, also confirmed:

```
m1 (10 B) = 9b207bd02ce8aca8e0ec
m2 (16 B) = 9520c1555c1fa8f0f12d3b5400000300
```
route -2^48 -2^42 +2^40 -2^23 +2^17, class (L & 4460a411424b5cd9) == 0000a40102030088 (24 bits),
conditional 8,784,412 / 2^28 = 2^-4.933 => 2^-28.93; fresh uniform 2^34 (rng 20260919): 43 hits, all
in class, 2^-28.57 [2^-29.04, 2^-28.14]; fresh 2^36 (rng 777): 153 hits, all in class, 2^-28.74
[2^-28.98, 2^-28.51].  Bits 29.93.  Tail tuning does nothing for this family (rate 2^-4.93 for every t).

## What was searched (all exhaustive unless stated)

Search program `dp5.c` (model of the panel's `dp.c`: word-0 signed-digit difference, du from the
hi-word condition, x-wrap and u-wrap variants, length offsets -7..7, carry-pattern DP), extended
with (a) exhaustive enumeration of 5-digit routes and T+3/T+4-digit routes, (b) an in-C realiser
for every DP candidate at or below a total-cost bound, (c) a direct 2^16-sample class check of every
realised pair against the hash itself.  `dp5b.c` refines the cost model: digits under the initial
1-run of E (L mod 2^r fixed) cost no sign bit, the realiser sets t's low r bits accordingly, and each
candidate gets a 24-tail mini-tune before its 2^16 check.

| run | routes | DP runs | candidates <= bound | realised | best measured est | wall |
|---|---|---|---|---|---|---|
| dp5 3T (bound 32) | 317,688 | 127M | 0 | 0 | - | 6 s |
| dp5 4 (bound 30.5) | 9,530,640 (all 4-digit) | 1.12G | 18 | 4 | 29.99 (= page pair A's route +-{2,36,47,49}) | 44 s |
| dp5 4T (bound 30.5) | 9,530,640 | 3.64G | 0 | 0 | - | 2 min |
| dp5 5 (bound 30.5) | 224,923,104 (all 5-digit) | 26.5G | 154 | 94 | **28.17 (F60)** | 37 min |
| dp5b 4 (bound 30.5) | 9,530,640 | 1.12G | 22 | 8 | 29.95 | 6 min |
| dp5b 5, third-lowest digit <= 9 (bound 31.5) | 5,655,744 | 667M | 16 | 16 | 29.65 | 2 min |
| dp5b 4T (bound 30.5) | 9,530,640 | 3.64G | 0 | 0 | - | 5 min |
| dp5b 5, second-lowest digit <= 15 (bound 30.5, with mini-tune) | 84,514,176 | 9.97G | 50 | 40 | 28.18 (F60 family again: its 16-vs-13 mirror) | 16 min |
| dp5b 5, third-lowest digit <= 15 (bound 31.5, mini-tune) | 22,248,576 | 2.62G | 118 | 111 | 29.39 (route +-{1,4,10,15,54}: tot 31.0, tail bias worth 1.6 bits, still short) | 4 min |
| dp5b 5, fourth-lowest digit <= 15 (bound 32.5, mini-tune) | 2,877,056 | 339M | 72 | 54 | 29.41 (same route) | 34 s |

Unrealisable candidates (the 4-digit 29.01 family +-{43,47,51,55} and five 5-digit 29.0x families,
listed as `UNREAL` lines) fail for a structural reason: the fixed high bits of B that the carry
pattern demands pin x = len + w0 to the wrong side of the x + d0 wrap, for the route and its mirror
alike, and the base length cannot move B by more than 7.

Not run: 5+T-digit routes (T never contributed at 3T/4T), 6-digit routes (4.35G routes, ~30 CPU-h,
outside the budget; the best-of-N trend 3 digits: 28 -> 4: 26 -> 5: 24 for wt(E) suggests 6 digits
would gain at most another 0.5-1 bit before the sixth sign bit is paid).

## Reproduction (Xeon, <xeon-work>/witness/t1ha-digits; sources in this directory)

```
gcc -O3 -march=native -o dp5 dp5.c -lpthread -lm          # includes the SMHasher3 verification self-test
gcc -O3 -march=native -o dp5b dp5b.c -lpthread -lm
gcc -O3 -march=native -o cond2 cond2.c -lpthread -lm
gcc -O3 -march=native -o direct_class direct_class.c -lpthread -lm
gcc -O3 -march=native -o tune tune.c -lpthread -lm
nice -n 10 taskset -c 24-31 ./dp5 5 8 30.5 > out_5.txt                 # exhaustive 5-digit, 37 min (shared cores)
nice -n 10 taskset -c 24-31 ./dp5b 5 8 31.5 0/1 63 9 > outb_5low3.txt   # refined model, third-lowest digit <= 9
nice -n 10 taskset -c 24-31 ./dp5b 5 8 30.5 0/1 15 > outb_5.txt         # refined model, second-lowest digit <= 15
nice -n 10 taskset -c 24-31 ./dp5b 5 8 31.5 0/1 63 15 > outb_5low3b.txt  # third-lowest digit <= 15
nice -n 10 taskset -c 24-31 ./dp5b 5 8 32.5 0/1 63 63 15 > outb_5low4.txt # fourth-lowest digit <= 15
# (on the Xeon the last two ran as ./dp5c, a copy of the final dp5b.c with the low3/low4 arguments)
# F60 confirmation
./cond2 c4c0cc2284cd239ede36a18fa6 fbc31866049ec02dea0c4e2be580d3cf a5845081f808f44a 2080000118003408 28 8
./direct_class c4c0cc2284cd239ede36a18fa6 fbc31866049ec02dea0c4e2be580d3cf a5845081f808f44a 2080000118003408 36 8 4242
./tune c4c0cc2284cd239ede36a18fa6 fbc31866049ec02dea0c4e2be580d3cf a5845081f808f44a 2080000118003408 4096 14 20 8 11
# F1 confirmation
./cond2 9b207bd02ce8aca8e0ec 9520c1555c1fa8f0f12d3b5400000300 4460a411424b5cd9 0000a40102030088 28 8
./direct_class 9b207bd02ce8aca8e0ec 9520c1555c1fa8f0f12d3b5400000300 4460a411424b5cd9 0000a40102030088 36 8 777
# independent implementation (website verify package's hash, locally):
cc -O2 -std=c11 -o indep_check indep_check.c verify_lib.c -lm
./indep_check c4c0cc2284cd239ede36a18fa6 fbc31866049ec02dea0c4e2be580d3cf 4c240c2749cd4915 a2b08b86e150940c
```

`direct_class` prints every hit with its seed, L and in-class flag; the rng streams are
splitmix64 seeded by (rngseed, thread).  The DP/realiser rng is xoshiro-free splitmix64 seeded per
thread, so `dp5` output is deterministic up to thread scheduling of the printed order.

## Files

- `dp5.c`, `dp5b.c` search programs; `t1ha2.h`, `common.h` (panel's implementation, validated by
  the SMHasher3 verification value at start-up); `cond2.c`, `direct.c` (panel's samplers);
  `direct_class.c`, `tune.c` (new); `indep_check.c` + `verify_lib.c` (website verify hash, main renamed).
- `xeon_logs/`: raw outputs of every run listed above (`out_*.txt`, `outb_*.txt`, `time*.txt`,
  `cond_*.txt`, `direct_*.txt`).

## Suggested row update (data.json, heuristics[id=t1ha])

pair.m_hex = c4c0cc2284cd239ede36a18fa6, pair.m_prime_hex = fbc31866049ec02dea0c4e2be580d3cf,
lengths_bytes [13, 16], L_words 2, example_seed_raw 4c240c2749cd4915, example_output_hex
c35b49165b1a4480; collision: class density 2^-24, conditional 2^-4.19 at 2^28, contribution
2^-28.19; fresh uniform 217 / 2^36 all in class; score 29.2 (cap; sampled); notes: 5-digit route
found by exhaustive DP over all 224.9M 5-digit routes; mechanism text as above.  The verify package
(`verify/t1ha2-64/t1ha2_64_verify.c`) needs the new pair, class mask/value and expected-output block.
