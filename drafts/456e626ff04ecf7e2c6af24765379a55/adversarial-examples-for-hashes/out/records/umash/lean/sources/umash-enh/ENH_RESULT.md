This investigation proves a nontrivial primary UMASH bound. It does not prove the published 55-bit bound or disprove it.

For independent uniform OH key words, put `q=2^64`, `p=2^61-1`. If the encoded nonfinal chunks of two blocks agree and their final ENH data differ, then

```
Pr[both OH halves agree modulo p] <= 718333281557 / 2^64
                                 < 3.894092522164e-8.
```

This includes a block with no PH chunk. With at least one PH chunk, the stronger constant is `563730706526 / 2^64 < 3.055990283562e-8`. Tags may be any fixed high-word tags. Identical ENH data with different tags are treated separately below. Equal-length aligned blocks cannot have a tag-only difference.

Combining these results with GAP.md's other primary cases and the polynomial term gives an all-pairs UMASH-64 score of **25.6141375967 bits**, under the ideal full-key model and the length-cap convention in AUDIT_REVIEW.md. This is the score of a proved upper-bound envelope, not a measured collision rate or a claim that any pair attains the bound.

The exact rational certificates are in [proof_result.json](proof_result.json). The final proof programs are [ph_final.py](enh/ph_final.py), [certify_no_ph.py](enh/certify_no_ph.py), [modular_mass.py](enh/modular_mass.py), and [finalize_proof.py](enh/finalize_proof.py). Their finite enumerations use integer arithmetic. Decimal logarithms are display values.

**Measurement result.** The complete 349-pair sweep found 0 full primary collisions, with `2^36` parameter sets per pair. The worst per-pair exact Poisson 95% upper limit is `5.36802611039e-11`; this is too large to establish `2^-55`. Detailed counts and simultaneous intervals appear below.

**1. Model and the NH fact used.** Messages and the common seed are fixed before sampling keys. All 34 OH words are initially independent uniform 64-bit words; polynomial multipliers are independent of them. For a block, write

```
N(x) = ((x_lo+k_lo) mod q) * ((x_hi+k_hi) mod q),
N_t(x) = (N(x) + q*t) mod q^2,
T(L,H) = (L,H xor L),
E_t(x) = T(N_t(x)),
OH(x) = M xor E_t(x).
```

`M` is the XOR of the common PH products and is independent of the ENH key pair. The supplied C source has exactly this high-word tag addition and triangular XOR. Its bytes are unchanged; source hashes are recorded in result.json and logs.

For distinct final data, NH is `1/q` almost-additively-universal modulo `q^2`. Here is the needed argument, also given in AUDIT_REVIEW.md §4.1. Select a differing input word, and fix its key so its two translated factors are distinct `c,c'` in `[0,q)`. Expose the other key word as `k`. A prescribed difference becomes

```
c*k - c'*((k+d) mod q) = v  (mod q^2).
```

Within either interval separated by the wrap of `k+d`, the slope is nonzero and the difference between two values has magnitude less than `q^2`. Two solutions across the wrap would require

```
(c-c')*(k2-k1) + q*c' = 0 (mod q^2),  0 < k2-k1 < q,
```

but this expression is strictly between zero and `q^2`. Thus at most one of the `q` keys works. Adding fixed tags translates the target and preserves the bound. This is an additive bound, not an XOR-universality assertion. The standard ENH construction uses precisely this implication; see [Badger, §3](https://eprint.iacr.org/2004/319.pdf).

The theorem is information-theoretic for the ideal full-key distribution. The experiment uses the requested CSPRNG source for the full parameter material. A CSPRNG stream is not asserted to provide information-theoretically independent random bits.

**2. Exact signed patterns.** For `d=u xor v` and `z=u & d`, ordinary integer subtraction gives `u-v=2z-d`. Consequently the admissible differing-bit assignments are

```
P(d) = { (d+a*p)/2 : -8 <= a <= 8,
         d+a*p is even, 0 <= (d+a*p)/2 <= d,
         ((d+a*p)/2) & d = (d+a*p)/2 }.
```

Let `S={d:P(d) is nonempty}`. The proved signed-digit recurrence in GAP.md §2, implemented in prior-toy/algebra.py, gives exactly `|S|=852`. The independently checked pattern histogram is: one mask with one pattern, 435 with two, 357 with four, and 59 with eight. Thus the suggested bound 17 is valid but can be improved to eight for nonzero masks; `P(0)={0}`. Parity and the impossibility of a nonempty signed binary sum being zero also give the bound eight directly.

Put `D=(d_lo,d_hi)=E(x) xor E(y)`. Reduced equality implies `D in S x S`, and requires the differing bits of `M xor E(x)` to belong to `P(d_lo)` and `P(d_hi)`. All other bits are unrestricted. This is an exact condition.

**3. A PH projection bound including zero factors.** Select one common PH chunk and condition on all other PH keys. Its product is `a carryless_product v`, where `a,v` are independent uniform 64-bit binary polynomials. For fixed `a`, projection onto any selected output bits is linear over GF(2); a specified pattern has probability either zero or `2^-rank`.

If the least set bit of `a` is `l`, selected low-half output positions at least `l` are independent rows. This follows by triangular elimination using the coefficient of `v_(i-l)` in output bit `i`. Positions below `l` are zero. If `deg(a)=63-j`, selected high-half positions below `63-j` are independent rows by the reverse triangular argument; positions at or above `63-j` are zero. In particular, output bit 127 is always zero and is never counted as a free PH bit.

For a set of bit positions `J`, define

```
m_d(J) = max_z #{t in P(d) : t & J = z}.
```

Translation by the conditioned XOR offset does not change these multiplicities. Define the following integers:

```
A_lo(d) = q + sum_(l=0)^63
          m_d(2^l-1) * 2^(127-l-popcount(d >> l)),

A_hi(d) = q + sum_(j=0)^63
          m_d((q-1) xor (2^(63-j)-1))
          * 2^(127-j-popcount(d & (2^(63-j)-1))).
```

The leading `q`, divided by `q^2`, is the case `a=0`. The other terms average over its exact valuation or degree probabilities `2^(-l-1)` and `2^(-j-1)`. The multiplicity counts how many admissible targets can survive the forced zero bits. Therefore, conditionally on the ENH keys, the reduced collision probability over PH keys is at most

```
g_D = min(1, A_lo(d_lo)/q^2, A_hi(d_hi)/q^2).
```

Bounding either necessary half event suffices; no independence of the two halves is used. Additional PH chunks are conditioned into the XOR offset. A realized value `M=0` is covered without an exception. The case `v=0` is already part of the linear-map distribution.

**4. NH masks, a second conditioning, and the sum.** Undoing the triangular XOR gives the NH XOR mask `(d_lo,z)`, where `z=d_lo xor d_hi`. A word XOR mask with `r` set bits has at most `2^r` signed integer differences. At bit 127 the two signs coincide modulo `q^2`, so that bit saves a factor two. Thus, for nonzero `D`,

```
Pr[D] <= b_D = min(1, 2^e/q),
e = popcount(d_lo) + popcount(z) - bit_63(z).
```

The raw event `D=0` has probability at most `1/q` by NH additive universality.

There is a useful additional bound. Condition instead on all PH keys, so the entire common mask `M` is fixed. The low-half reduction leaves at most `|P(d_lo)|` possible low-word integer differences of `N_t(x),N_t(y)`. The high NH XOR mask is `z`; allowing every sign on that mask gives at most `2^(popcount(z)-bit_63(z))` high-word differences. Hence the probability of a reduced collision with this particular nonzero `D` is at most

```
h_D = |P(d_lo)| * 2^(popcount(z)-bit_63(z)) / q.
```

No extra factor `|P(d_hi)|` is needed. At each bit in `z`, exactly one of the two reduced-coordinate masks changes; the other coordinate can supply either high-word sign. Allowing every sign already includes every high admissible pattern. Fixed tags only translate the additive target set.

Let `r_D` be the probability of a reduced collision with mask `D`, and let `p_D=Pr[D]`. The separate conditioning arguments prove

```
r_D <= g_D*p_D,   r_D <= h_D,
p_D <= b_D,       sum_D p_D <= 1.
```

These bounds are combined as inequalities; the two differently conditioned probabilities are not multiplied. The total probability budget is essential. Merely summing an individual upper bound for every large mask loses several bits.

The final certificate maximizes the resulting elementary linear program. Write `G_D=q^2*g_D`, `B_D=q*b_D`, and `H_D=q*h_D`, all integers. On a probability grid with denominator `q^2`, use the upward-rounded capacity

```
K_D = min(B_D*q, ceil(H_D*q^3/G_D)).
```

For `D=0`, set `G_D=q^2`, `K_D=q`. Sorting masks by decreasing `G_D`, fill capacities until the total allocation is `q^2`. This greedy allocation maximizes `sum G_D*n_D/q^4` subject to `0<=n_D<=K_D` and `sum n_D<=q^2`. Upward rounding enlarges the feasible set, so it is safe. This is a finite upper-bound calculation, not a sample of ranks or keys.

[ph_final.py](enh/ph_final.py) obtains

```
B_PH = 11989226195148851570875316890129934569436235759441
       / 392318858461667547739736838950479151006397215279002157056
     = 3.055990283556631e-8
     <= 563730706526 / q.
```

This makes the proposed route rigorous for at least one PH chunk. It uses one chunk, not an assumption of 15 fresh chunks.

**5. No PH chunk: folded-product counting.** A PH argument alone cannot cover 9–16-byte messages. Here `M=0`. The following separate argument closes that case, including arbitrary fixed high-word tags.

For `N=A*B=q*H+L`, with independent uniform `A,B` in `[0,q)`, consider a constant run of bits `l,...,r` of

```
U = ((H+t) mod q) xor L,   K=2^(r+1).
```

If the run is zero, `(L-H) mod (q+1)` lies in at most `2^(l+1)` residue classes modulo `K`. If the run is one, `(L+H) mod (q-1)` lies in at most `2^(l+1)+1` such classes. To see this, equal bits give a difference of the lower `l` bits in `[-(2^l-1),2^l-1]`; complementary bits give a sum in an interval of length `2^(l+1)-1`. Canonical reduction modulo `q+1` adds either zero or `q+1`; reduction modulo `q-1` subtracts zero, one, or two copies of `q-1`. Modulo `K` these enlarge the intervals by one and two positions respectively. The tag translates the intervals because `q` is divisible by `K`.

Since `q=-1 mod (q+1)` and `q=1 mod (q-1)`, these are restrictions on the integer product modulo the indicated odd modulus. For an odd modulus `m`, a target set occupying at most `B` classes modulo `K` has at most

```
J_m(K,B) = sum_(d divides m)
           phi(d)*(m/d)*min(d, B*ceil(d/K))
```

preimages among all ordered pairs of residues modulo `m`. Indeed, there are `phi(d)` first factors of gcd `m/d`; their products range over multiples of that gcd, each with `m/d` preimages. Multiplication by the odd gcd permutes classes modulo `K`, giving the displayed count.

For `m=q+1`, our input interval is a subset of the full residue system, so divide `J_m` by `q^2`. For `m=q-1`, residue zero occurs twice in `[0,q)`, so add at most `2m+1` before dividing. This includes zero products. The factorizations are

```
q+1 = 274177 * 67280421310721,
q-1 = 3 * 5 * 17 * 257 * 641 * 65537 * 6700417.
```

The supplied validation checks every factor by trial division. No heuristic divisor estimate is used.

For the low product word, the exact probabilities that a run `l,...,r` of length `n=r-l+1` is zero or one are

```
P_zero = 2^-n + n*2^(-r-2),
P_one  = 2^-n -   2^(-r-2).
```

Condition on the 2-adic valuation of `A`; the remaining low bits of `A*B` are uniform. This proves both formulas, including `A=0`.

The finite split is as follows. For each `d in S`, let its run class be the largest `n` such that `d` contains positions `3,...,3+n-1` and every `t in P(d)` is constant on that run. Use class cutoff 28. If either mask has larger class, the corresponding word of `E(x)` has a constant run at positions **3 through 31**. Bound the union of the two possible run values by the formulas above.

For the remaining mask pairs, count a cover of their possible NH additive differences. Low reduction fixes the low integer difference to `a*p`, with `-8<=a<=8`. Split `z=d_lo xor d_hi` as `R+2^61*h`. Each signed high difference lies near a center `c*2^61`, where `c=(2u-h) mod 8` and `u` is a submask of `h`. If `R!=0` and `v2(R)=l`, its low signed part lies in `[-R,R]`, in one class modulo `2^(l+1)`. This arithmetic interval has `(R>>l)+1` points. Intervals for the same `(a,c,l)` are nested, so keep only the largest radius; `R=0` contributes a singleton. Tags translate the complete difference set and do not change its cardinality.

[certify_no_ph.py](enh/certify_no_ph.py) checks all relevant masks and patterns and emits these interval covers. The sum of their sizes is **447750340566**. This is an upper bound on the number of additive targets, not a claim that every covered target is realizable. NH additive universality now applies to their union.

With denominator `q^2`, the four contributions are exactly:

| Contribution | Numerator |
|---|---:|
| Raw collision | 18446744073709551616 |
| Large low mask: constant run | 2376844875427930127806318510080 |
| Large high mask: constant run | 2614529387718499859440264600769 |
| Both small: additive-target cover | 8259535941337293943031155654656 |

Their sum gives

```
B_noPH = 13250910204502170674351448317121 / 2^128
       = 3.894092522161426e-8
       <= 718333281557 / q.
```

**6. Tags, zero masks, and the remaining cases.** Equal-length aligned blocks have the same tag, because the common seed is XORed with the same block-length byte. Thus a tag-only difference is impossible in the requested equal-length case.

For all-pairs coverage, tag-only differences at unequal lengths do matter. Their integer tag difference has absolute value at most 255. Their low XOR mask is zero. Any nonzero high mask in `S` has highest bit at least 60. A signed sum using `t` bits with highest bit `h` has absolute value at least `2^(h-t+1)`. To represent a nonwrapping difference of absolute value at most 255 requires at least 54 bits. In a wrapping difference, the mask is at least `q-255` and has at least 56 bits. Thus only high masks with popcount at least 54 need consideration.

For a PH block, the maximum of the high projection bounds over those masks gives `8243/q`. Without PH, the same finite mask/pattern check gives a constant run at positions 3 through 49, and the folded-product lemma bounds the event by

```
10000468334135369360809153 / 2^128 < 2.939e-14.
```

Both bounds are smaller than the final ENH-data bound. Raw equality of distinct ENH data is already included as the `1/q` term. Tag-only raw equality is impossible. There is no assumption that `M`, a PH factor, or an integer NH factor is nonzero.

**7. All-pairs completion and score.** Let

```
c = 718333281557,
D0 = 1 - 561/q,
A = c/(q*D0),
r(s) = min(1, 2*ceil(s/256)/(p-2)).
```

The IID bound is `c/q`. Conditioning the 34 OH words on distinctness costs at most `1/D0`. The other primary cases in GAP.md are all smaller: a differing PH chunk has bound `852^2/q=725904/q`; unequal chunk/block cases have bound below `82/q`; the short/long case has bound `9/q`. The new tag-only bounds above are also smaller. Thus all formal zero-polynomial cases are covered. The root-counting completion gives

```
Pr[full primary collision] <= A + (1-A)*r(s).
```

The production initializer permits multiplier 1; using denominator `p-2` is conservative for its uniform nonzero field multiplier. No fingerprint theorem is asserted here.

For the score, `L` is a positive integer word cap: both byte strings have length at most `8L`, with `1<=L<=2^61-1`. At `L=1`, use the short-string bound `1/(q*D0)`. At `L>=2`, use the displayed envelope with `s=8L`, and the stated 64-bit output floor if desired. Since `ceil(L/32)<=L/2`, the ratio `L/epsilon(L)` is minimized at `L=2`. Its exact error bound there is

```
1656363775600634047357796737589
/ 42535295865117306584003665538960066195.
```

Therefore

```
min_L log2(L/epsilon(L)) = 25.614137596713487... .
```

The output floor does not change this minimum. The polynomial term has been included, not dropped as numerically small.

**8. Verification and reproduction.** Run `sh enh/reproduce_proof.sh` on the Xeon to regenerate the exact certificates and rational score. Final proof generation needs Python's standard library. The independent validation checks six signed-support sets, 189 pattern sets, 1296 PH pattern bounds including offsets, 1359 NH message differences, 240 exact low-run counts, 1280 tagged folded-run bounds, 3408 modular-product bands, and nine prime factors. A separate check also exhausts the second-conditioning additive-target sets at small widths and checks every signed interval membership at width eight. All passed. These checks supplement the arguments above; scaled measurements are not used to extrapolate a 64-bit theorem.

The original production self-test passed. A baseline compiler build and the inlined sweep build, using the same CSPRNG key, also agreed on batch checksums and collision counts for 1,048,576 parameter sets over all 349 pairs. The source SHA-256 for umash.c is `aad7d50dcca9d8939a67452788fe7028357a76b125ee7f11d50b622269789409`.

The sweep runs on the Xeon Platinum 8375C at `hardware.normalcomputing.net`, in `~/agents/umash-enh`, at nice level 10. It uses 47 sweep threads. Helpers are single-threaded. During one certificate correction, two helper runs overlapped, temporarily allowing 49 task compute threads; subsequent helpers ran sequentially. The Mac did file editing and small orchestration work, not the sweep or proof computations.

**9. Completed structured-pair measurement.** All **349 pairs** completed exactly **68,719,476,736 = 2^36** accepted full parameter sets each. There were **0 full 64-bit primary collisions** in **23,983,097,380,864 comparisons**. The worst observed rate was **0/68719476736 = 0**. 349 pairs tied for that maximum. Every batch ID was checked for uniqueness and complete coverage before this report was finalized.

| Quantity | Result |
|---|---:|
| Worst count per pair | 0 |
| Exact Poisson 95% interval, equal tailed, per pair | [0, 5.36802611039e-11] |
| Exact Poisson one-sided 95% upper limit | 4.35936420916e-11 |
| Bonferroni simultaneous 95% upper limit across all 349 pairs | 1.38882771372e-10 |
| Claimed rate for these 32-byte pairs, `2^-55` | 2.77555756156e-17 |
| Per-pair 95% upper limit / `2^-55` | 1934035.23 |
| Task-1 full-hash bound for one PH chunk, including distinctness and polynomial term | 3.05599028365e-08 |
| Per-pair 95% upper limit / that full-hash bound | 0.00175655863 |
| Per-pair 95% upper limit / the general reduced bound `718333281557/2^64` | 0.00137850503 |

These are Garwood intervals for a Poisson count, not normal approximations. For zero events, the two-sided interval is exactly `[0, -ln(0.025)/N]`; the one-sided upper limit is `-ln(0.05)/N`. The simultaneous column uses tail allocation `0.025/349`. The pairs share parameter sets, so their outcomes are dependent; no pooling across pairs or independence-based multiple-comparison calculation is used. The simultaneous intervals address selecting the worst observed pair.

Zero events do **not** certify `2^-55`. At that rate, the Poisson mean is only `2^-19 = 1.90734863281e-06` events per pair. The upper limit remains about 1.93404e+06 times larger than the claim. The data are compatible with the published claim and lie well below the much looser proved upper bound; neither fact establishes the unknown worst-pair collision probability.

The fixed message family is in [pairs.json](enh/pairs.json). The baseline is 32 zero bytes; every other message has the same first 16 bytes and differs only in the final chunk. The sweep includes all 64 single-bit changes in each final word; `3`, `5`, `2^61+1`, `2^62+1`, `2^63+1`, `p`, and the all-ones word in the low word, high word, and both words; and 200 fixed random 128-bit differences. Seed is zero and `which=0`.

The CSPRNG was **OpenSSL AES-256-CTR**, initialized from the operating system through OpenSSL `RAND_bytes`. Its public experiment key is saved in result.json and the first ledger record. Each batch uses a disjoint counter range. All **304 bytes** of each candidate `umash_params` were filled from the stream, followed by the original `umash_params_prepare`; `umash_params_derive` was not used for measurement. The complete run consumed **20,890,720,927,744 stream bytes**, with **0 failed candidate preparations**. The same full parameter set was applied to every fixed pair. The baseline `umash_full` value was reused, giving **24,051,816,857,600 full primary-hash calls**.

Recorded compute elapsed time was **6810.559 seconds** (1.892 hours), using 47 sweep threads at nice 10. The complete batch ledger is [logs/sweep.jsonl](logs/sweep.jsonl); stderr, environment, build hashes, replay checks, and proof checks are in [logs](logs/). [result.json](result.json) contains every pair, exact counts, intervals, the parameter-stream key, source hashes, and theorem constants. [enh/README.md](enh/README.md) gives reproduction commands.

