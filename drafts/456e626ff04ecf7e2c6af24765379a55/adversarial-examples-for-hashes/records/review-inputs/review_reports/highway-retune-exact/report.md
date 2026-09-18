# Exact counting and constant optimisation of the HighwayHash byte-1 trail

The retuned packet word has exact conditional trail probability

\[
 p_{\rm retuned}=\frac{1025709214045}{2^{64}}
                 =2^{-24.100241072819\ldots}.
\]

The exact maximum in the lane-0 constant family defined below is

\[
 \boxed{p_* = \frac{4412514617845172193590}{2^{96}}
             =2^{-24.097908949592\ldots}},
\]

attained by packet words **`p1 = 0x241a0d29f0cb31d1`**, **`p2 = 0x24192a2ac586b59d`**. This includes a small, nonzero correction to the usual one-byte-pad argument. With packet 1 held at the original value, the exact best packet 2 is `0x24192a2ab49eb59d` and its conditional probability is `(1027353388893 * 2^32 + 51510) / 2^96`. The joint gain over the retuned pair is only 0.002332123228 bits; optimising p1 after p2 contributes 0.000021386669 bits of that gain.

The requested 2^30-class-key confirmation, implemented as 16 sequential experiments of 2^26 keys each, found **56 verified full-state collisions**, all also full 64-, 128-, and 256-bit output collisions. The exact expectation is **59.8007241849**. The measured log2 rate is **−24.192645077942**, with approximate one-standard-error uncertainty 0.1928 bits. This confirms consistency, not the tiny improvement over the retuned pair; the optimisation follows from exact counting.

**The 72-byte qualification matters.** The 8-byte remainder conversion is not automatic: it occasionally fails because `Rotate32By(8, ...)` does not preserve an additive difference across the relevant byte boundary. I exhibit an actual counterexample below. The exact 96-byte trail probability therefore cannot simply be assigned to the 72-byte pair. I give a proved 72-byte lower bound and a measured estimate; its exact probability remains open.

**Threat model and scope.** The source's seed API is `const uint64_t key[4]`: the hidden seed is uniformly random in 256 bits. The attacker fixes both messages before that seed is sampled. The construction uses the public fixed `init0`/`init1` constants in `HighwayHashReset`; there is no additional secret or default-secret buffer in these supplied sources. This is not a claim about a different wrapper that expands a 64-bit seed into the four key words. Such a wrapper is not supplied.

Let `C` be `hi32(key[0]) = 0xdbe6d5d5`. Its exact probability is 2^-32. Given C, the relevant random inputs are `lo32(key[0])` and all of `key[1]` (96 bits). `key[2]` and `key[3]` are irrelevant to this absorption trail because the zipper keeps lane pairs {0,1} and {2,3} separate until finalisation.

The optimisation is exhaustive over **both 64-bit lane-0 packet constants**, with the other words of the first two packets zero, and the fixed additive difference pattern

```text
packet 1, lane 0:  +0x0000000000000100
packet 2, lane 0:  -0x0000000001000200
packet 3, lane 0:  +0x0000000000000100
```

It concerns the carry-free byte-1, unchanged-multiplier trail of the prior lemma, including the necessary carry restrictions. It is not a proof of optimality among other differences, arbitrary common words in other lanes, other trail shapes, or other message lengths. The published exact probability is for this sufficient collision event. **Full-output collision probability can be larger**, through other events; an equality for the total collision probability is not proved.

**Resulting epsilon and score.** Put

```text
N*     = 4412514617845172193590
N72_lb = 4378109172422319339830
```

The following bounds apply simultaneously to HighwayHash64, HighwayHash128, and HighwayHash256:

| Realisation | Guaranteed full-output collision probability over uniform 256-bit seeds | log2 of that lower bound | Upper bound on log2(L / eps) |
|---|---|---:|---:|
| 96 bytes, L = 12 | `N* / 2^128` | −56.097908949592 | **59.682871450313** |
| 72 bytes, L = 9 | `N72_lb / 2^128` | −56.109202077390 | **59.279127078832** |

The 96-byte row is the exact contribution of the optimised trail. The 72-byte row is a certified lower bound on its surviving contribution, not an exact count. Among 979,935 constructed E2 keys, 20 failed the remainder conversion: estimated loss `20/979935 = 2^-15.580398436647`, estimated retention 0.999979590483. Combining this estimate with the exact E2 probability gives a **72-byte trail-score estimate 59.267863396043 bits**. Ignoring the remainder loss would give 59.267833951034 bits, which is not a proved attainable value for this pair. In particular, the task's minimum over all lengths is at most 59.279127078832 from the certified L = 9 construction; its global minimum is not determined here.

**Reference and implementation validation.** [work/hh_own.h](work/hh_own.h) is a new transcription of the supplied C implementation, with the zipper implemented as explicit byte-index maps. I copied `ref/*.c` and `ref/*.h` unchanged into [ref/c/](ref/c/) as requested. The original reference and the independent implementation pass all 65 length-0..64 published 64-bit vectors plus the separate 33-byte vector. Thus validation uses the allowed published-vector route, not an invented 64-bit seed expansion for the SMHasher procedure.

[work/check.txt](work/check.txt) records 100,000 additional random messages of lengths 0..255 with identical pre-finalisation states and identical 64-, 128-, and 256-bit outputs between the new implementation and reference. [work/validate_bytes.txt](work/validate_bytes.txt) records 3,145,728 packet-1 formula/reference comparisons on 1,048,576 class keys across the known, retuned, and joint-optimum constants, with zero mismatches. The same run evaluated both full 96-byte HighwayHash64 outputs on all 1,048,576 keys: zero collisions, including zero outside E2. That last check has only 2^20 resolution and cannot rule out other collision events.

The confirmation sampler evaluates the exact E2 predicate on every sampled key, then runs both complete messages through the independent and reference implementations on every hit, checking complete states and all three output widths. It counts verified trail collisions; it does not pretend to have evaluated the full hash on rejected keys. The pseudorandom generator and stream identifiers are recorded in the source and logs; the probability proofs do not rely on that generator.

**Mechanism from the source.** Write `I00 = 0xdbe6d5d5fe4cce2f`, `I10 = 0x3bd39e10cb0ef593`; their lane-1 counterparts are `I01 = 0xa4093822299f31d0`, `I11 = 0xc0acf169b5f18a8c`. `HighwayHashReset` gives `hi32(v0[0]) = 0` on C. In `Update`, the first operation is `v1 += mul0 + packet`, followed by `mul0 ^= lo32(v1) * hi32(v0)`. Thus packet 1's `+2^8` difference meets a zero multiplier operand. It does not change the second multiplication because it remains in byte 1 of `v1[0]`.

`ZipperMergeAndAdd` sends that byte-1 difference to byte 5 of `v0[0]`, and its second invocation sends byte 5 to byte 3 of `v1[0]`. After packet 1, the only differences are `dv0[0] = +2^40` and `dv1[0] = +2^8 + 2^24`; both multiplier arrays and the other lanes are equal.

The packet-2 message difference changes the latter to `−2^8`. If `x = lo32(v1[0])` at the packet-2 `mul0` multiplication and `h = hi32(v0[0])`, its two products are `xh` and `(x−256)(h+256)`. On the counted event **E2: `x−h = 256 mod 2^32`**, the relevant operands do not wrap and these products agree as exact 64-bit products. The second multiplier also agrees. The first zipper cancels `+2^40` with `−2^40`; the second zipper adds identical words. The only remaining difference is `dv1[0] = −256`.

A third full packet with lane-0 difference `+256` cancels this before any further multiplication. The entire 1024-bit state is then identical, so `HighwayHashFinalize64`, `HighwayHashFinalize128`, and `HighwayHashFinalize256` necessarily produce identical full outputs. Equal subsequent input processing also preserves equality. For the constants attaining the maximum below, the successful branch has `v1[0]` byte 1 after the first add equal to 0x20 and `h` byte 1 equal to 0xbe, so all the required zipper and multiplication carry conditions hold.

**Notation avoids the prior constant ambiguity.** Here `p1,p2` mean actual little-endian packet words. Set `c2 = I00 + p2`, and let `s` be the low 32 bits of packet 1's `v1[0]` immediately after its add. For the original packet 1, `s = 0x10e82046`. The retuned word `p2 = 0x24192a2ab49e8900` therefore means `c2 = 0x00000000b2eb572f`, not that same 64-bit packet word reused as an additive offset. The target for the byte system is

\[
 R=256-c_2-s\pmod {2^{32}}.
\]

| Pair family | p1 | p2 | s | R |
|---|---|---|---|---|
| Known | `24192a2a01b331d1` | `24192a2ab4b332d1` | `10e82046` | `3c17dfba` |
| Retuned | `24192a2a01b331d1` | `24192a2ab49e8900` | `10e82046` | `3c2c898b` |
| Exact best p2, original p1 | `24192a2a01b331d1` | `24192a2ab49eb59d` | `10e82046` | `3c2c5cee` |
| Exact joint optimum | `241a0d29f0cb31d1` | `24192a2ac586b59d` | `00002046` | `3c2c5cee` |

All entries in this table are hex integers, without the `0x` prefix. Packet-word high halves of p2 do not affect E2 or its 96-byte probability; the table chooses `hi32(c2)=0`.

**Exact byte count.** Byte indices start at zero at the least significant byte. Put

\[
 a=\mathtt{fe4cce2f}\oplus\operatorname{lo32}(K_0),\qquad
 B=(a\oplus\mathtt{c59f503f})+d\pmod {2^{32}}.
\]

For any p1, its first added `v1[0]` is `(B,s)`, for unique constants s,d. Original p1 gives d = 0; the joint optimum gives d = 0x0000e300. Write `w_j = W1[j]` for

\[
 W_1=(I_{11}\oplus\operatorname{rot32}(K_1))+I_{01}.
\]

Let U0,U1 be v0 lanes 0,1 after packet 1's first zipper, and let `u2 = U0[2]`, `u3 = U0[3]`. The first zipper's low word in lane 0 has bytes `(s3,w4,s2,B1)`. Therefore

\[
 n=\left\lfloor\frac{256a_1+a_0+\mathtt{f593}+s_3+256w_4}{65536}\right\rfloor,
\quad D=14+s_2+n,
\quad u_2=(a_2+D)\bmod256,
\]

\[
 q=203+B_1+\lfloor(a_2+D)/256\rfloor,
\quad u_3=(a_3+q)\bmod256,
\quad \gamma=\lfloor(a_3+q)/256\rfloor\in\{0,1,2\}.
\]

Here `s_j` denotes a byte, whereas D in this display is an integer byte offset, not the earlier notes' word difference. The high word h of U0 has bytes obtained by adding

```text
0x3bd39e10 + (s0 << 24) + (s1 << 8) + (w7 << 16) + w6 + gamma.
```

In particular `h0 = (16+w6+gamma) mod 256`, with `e0 = [16+w6+gamma >= 256]`. The second zipper contributes low-word bytes `(u3, U1[4], u2, h1)`. The four exact equations for E2, with subtraction borrows g0,g1,g2, are

\[
 u_3=(h_0+R_0)\bmod256,\quad g_0=\lfloor(h_0+R_0)/256\rfloor,
\]
\[
 U_1[4]=(h_1+R_1+g_0)\bmod256,\quad g_1=\lfloor(h_1+R_1+g_0)/256\rfloor,
\]
\[
 u_2=(h_2+R_2+g_1)\bmod256,\quad g_2=\lfloor(h_2+R_2+g_1)/256\rfloor,
\quad R_3=(h_1-h_3-g_2)\bmod256.
\]

For fixed a0,a1,w4,u2, the 256 choices of a3 give every u3 once, with
`gamma = floor(q/256) + [u3 < q mod 256]`. The first equation fixes w6 uniquely. The exact count with e0 = 0 is

\[
 F(q,R_0)=240-\sigma-
 [((R_0+16+\sigma)\bmod256)<q\bmod256],\qquad \sigma=\lfloor q/256\rfloor.
\]

This is the carry dependency that prevents a universal exact factor 239/256.

For the retuned target, R = 0x3c2c898b. Its successful branch is exactly e0 = 0, g1 = 1, and u2 = w7, for every w7. The other branch cannot satisfy the top-byte equation. With R0 = 0x8b,

\[
 F(q,\mathtt{8b})=239-[B_1+\mathrm{carry}_2\ge210].
\]

For the original p1, `D = 246+n`, so the carry into byte 3 is 0 or 1. B1 is a permutation of a1. There are 46 always-rejected B1 values; the boundary value B1 = 209 corresponds to a1 = 0x81. The useful identity

\[
 \sum_{w_4=0}^{255}\left\lfloor\frac{256a_1+a_0+\mathtt{f593}+s_3+256w_4}{65536}\right\rfloor
 =a_1+\left\lfloor\frac{a_0+\mathtt{f593}+s_3}{256}\right\rfloor
\]

gives the exact reduced count

\[
 M_r=239\,2^{32}-46\,2^{24}-246\,2^{16}
       -\mathtt{f593}-\mathtt{10}-256\,\mathtt{81}
    =1025709214045.
\]

For this target the remaining byte equation costs exactly 2^-8, including its exceptional carry case (below), so `Pr[E2|C] = Mr/2^64`. [work/opt.c](work/opt.c) and the separately written Python integer counter [work/exact.py](work/exact.py) agree exactly; see [work/python_count_retuned.txt](work/python_count_retuned.txt). The same C counter reproduces the prior count `M = 942292336640 = 235*239*2^24` for the known target.

**Why the optimisation is exhaustive in the stated family.** The ordinary pad model gives `M/2^64`; the exact correction will be less than one unit of M, so first maximise this integer M. I retain the full split of F by e0 and g0, not just its e0 marginal.

For any packet-1 constants and any fixed a0,w4, B1 runs through all 256 byte values as a1 varies. The carry into byte 3 is always in {0,1,2}. Allowing it to be chosen independently for each B1,u2 gives an upper bound. After translating the irrelevant top-byte constant, there are only three distinct patterns for g1 on the four groups `(e0,g0)`:

```text
all equal; only group (1,1) increased by one;
all groups except (0,0) increased by one.
```

Each top-byte equation selects an interval of u2 in each group. [work/bound.c](work/bound.c) exhausts all 256 R0 values, all three carry patterns, all 256 interval boundaries, and all possible resulting top-byte values. It maximises over the relaxed {0,1,2} carry separately before summing over B1. No probabilistic assumption enters this upper bound. Removing invalid h1=0xff branches can only reduce it; the s1=0xff case is outside this carry-free difference shape.

The largest bound for any R0 other than 0xee is **1027352821760**, at R0 = 0xed. For R0 = 0xee, every pattern except the family selecting all e0 = 0 and u2 = w7 has bound at most **1027349413888**. Both are below even the attainable optimum with original p1. The two equivalent winning carry patterns have bound 1027369598976. See [work/bound.txt](work/bound.txt) and [work/bound_detail.txt](work/bound_detail.txt). Thus the only family still needing exact optimisation is R0 = 0xee, e0 = 0, u2 = w7.

In this family

\[
 F(q,\mathtt{ee})=239+[q\le254]
                =239+[B_1+\mathrm{carry}_2\le51].
\]

Its count is maximised by minimising the carry at the boundary B1 = 51. Since s2,s3,a1 are nonnegative, the minimum occurs at s2 = s3 = 0 and a1 = 0 at that boundary for every a0. This is achieved, and equality forces, `d mod 65536 = 0xe300`. For each fixed a0,w4 there are at most 52 eligible B1 values; a larger s2 or s3 cannot restore a lost one. The identity above gives

\[
 M_* =239\,2^{32}+52\,2^{24}-14\,2^{16}-\mathtt{f593}
     =1027368618605.
\]

With original p1 instead, the same formula includes s2=232, s3=16, and boundary a1=0x63:

\[
 M_{\rm fixed}=239\,2^{32}+52\,2^{24}-246\,2^{16}
               -\mathtt{f593}-\mathtt{10}-256\,\mathtt{63}
              =1027353388893.
\]

The unused upper 16 bits of d and allowed choices of s0,s1 do not change these counts. I choose d=0xe300 and s=0x00002046. They give `I00+p1 = 0x0000e2ffef180000`, hence the advertised p1. The full fixed-p1 and joint reduced searches are also logged in [work/opt_original.txt](work/opt_original.txt) and [work/opt_joint.txt](work/opt_joint.txt).

**The exact pad correction, not an independence assumption.** Let `z = (h1+R1+g0) mod 256`. For fixed a and the high word W of W1, inversion of the key-1 addition has branch carry `beta = [lo32(W1) < 0x299f31d0]`. All pairs `(w3,w2)` except `(0x29,0x9f)` give a uniform pad after summing w0,w1. On that exceptional pair define

\[
 H(t)=\#\{(w_0,w_1):256w_1+w_0<\mathtt{31d0},\
 ((w_0-\mathtt{d0})\bmod256)\oplus\mathtt{ae}+w_1\equiv t\pmod{256}\},
\]

where the XOR expression is evaluated before adding w1. Exact enumeration of the 65,536 byte pairs gives H = 49 on inclusive intervals `[0xb1,0xc0]` and `[0xd1,0xf0]`, and H = 50 elsewhere.

For branch b in {0,1}, write

\[
 c_b=\left\lfloor\frac{
 ((W-\mathtt{a4093822}-b)\bmod2^{32})\oplus\mathtt{e933c0b9}
 +\mathtt{b5f18a8c}+w_5\,2^{24}+\mathtt{9f0000}+B_0\,256+\mathtt{29}
 }{2^{32}}\right\rfloor,
\]

again performing the XOR before the following additions. The exact number of low words W1 satisfying the byte-1 equation is

\[
 2^{24}+H(z-\mathtt{69}-c_1)-H(z-\mathtt{69}-c_0).
\]

This formula explicitly includes the key-1 carries; the correction is not assumed zero. For the retuned target, all possible H arguments are in 0xdc..0xdf, a constant part of H, so its correction is exactly zero.

[work/pad_edges.py](work/pad_edges.py) generates every `(B0,W)` with `c0 != c1`: **5,104** possibilities. It obtains them by decomposing an XOR-threshold preimage into dyadic intervals, including the special decrement across the low-24-bit boundary. For each such entry, B0 fixes a0; for each of 256 a1 values, at most the three gamma values need testing, and a2,a3 are then fixed by the byte equations. Thus for *any* constants the absolute correction to the 96-bit assignment count is at most

\[
 5104\cdot256\cdot3=3919872<2^{32}.
\]

Consequently it cannot overturn even a one-unit gap in M. This makes the preceding integer optimisation valid for the actual correlated key distribution as well.

For all packet-1 constants attaining M*, [work/pad_count.c](work/pad_count.c) computes the correction as a function of `z0 = (h1+R1) mod 256`:

\[
 \Delta(z_0)=51510\,[H(z_0+\mathtt{96})-2H(z_0+\mathtt{97})+H(z_0+\mathtt{98})].
\]

The maximum is **51510**, attained at z0 in `{0x1a,0x29,0x3a,0x59}`. The same correction holds for the original-p1 optimum. Choosing z0=0x1a and h1=0xbe gives R1=0x5c, g1=1, R2=0x2c, R3=0x3c. Therefore the exact maximum is `(M* * 2^32 + 51510)/2^96`, as reported. This last integer count and the universal bound are computer-assisted proofs over explicitly enumerated small byte systems, not seed-rate estimates. Their complete sources and logs are included. [work/check_edges.txt](work/check_edges.txt) independently checks the edge generator against all 2^24 low words for each of B0 = 0x00, 0x7f, 0xff, with exact agreement. Independently, 16 selected exceptional entries were checked through the reference on all 65,536 low-word assignments each; every one of the 4,096 resulting histogram entries matches this H formula.

**Why the 72-byte pair sometimes fails, and its certified bound.** `HighwayHashUpdateRemainder` adds the same length-dependent word to both v0 arrays, then calls `Rotate32By` on v1 before absorbing the padded tail. Let V be message A's v1[0] after packet 2. On E2, message B has V−256. The rotation transforms this into precisely a −65536 difference when

\[
 V\bmod2^{24}\ \ge\ 256.
\]

Then tail byte 2 equal to 0x01 in B cancels it. If the inequality fails, the subtraction borrows through the byte moved by the rotation; the advertised cancellation no longer holds.

A concrete failure for the joint-optimum pair has seed

```text
dbe6d5d5779174a6 33d9b96989b484fd 059924621dc8a750 10d7daccb4ae88b5
```

and post-packet-2 `V = 0x828697155d000024`. Both 96-byte HighwayHash64 outputs are `1a294093e955e314`, whereas the 72-byte outputs are **`04f737caa7fc4bac`** and **`7f90607073576435`**. The 128-/256-bit outputs differ too; [work/constructed.txt](work/constructed.txt) records them and two more examples. Reference states independently confirm these failures.

A rigorous lower bound does not require assuming that V is uniform. In packet 2's first zipper, the low word added to v0[0] has bytes `(h3, lane1_v1_byte4, h2, lane0_v1_byte5)`. Let m be the low word of mul1[0] after packet 1. Since u2=w7, a failure's byte-1 equation in the final v1 addition forces carry 1 into byte 2. Its byte-2 equation then requires

\[
 \kappa\equiv -w_7-m[2]-2h_2-1\pmod{256},
\]

where kappa is the carry into byte 2 of the first zipper addition. With
`t = lo16(U0)+lo16(m)+h3`, and allowing the unknown lane-1 byte to be any value 0..255, this requires

\[
 0\le\kappa\le2,\qquad t<(\kappa+1)65536,\qquad t+65280\ge\kappa65536.
\]

[work/remainder_bound.c](work/remainder_bound.c) counts this necessary condition, retaining the exact byte-1 event count over a3. It sums a0,a1,n,w7 and counts eligible w4 by integer interval intersection, rather than enumerating seeds. The result is `Mbad_upper = 8010642003`. Allowing the worst possible pad correction above proves

\[
 \#\{E_2\text{ and successful remainder cancellation}\}
 \ge (1027368618605-8010642003)2^{32}+51510-3919872
 =4378109172422319339830
\]

among the 2^96 relevant class assignments. This certifies retention at least 0.9922027577464084 and the L=9 row of the score table. The bound deliberately discards further packet-2 byte dependencies; the observed retention is much higher. The constructed-key experiment checked the necessary condition on every observed failure.

**Experiments and resolution.** All counts below are new runs in this directory. The 16 confirmation streams are 20261001 through 20261016. The individual hit counts were

```text
5, 1, 3, 7, 5, 5, 1, 2, 2, 8, 3, 3, 4, 3, 3, 1
```

Each count has denominator 2^26; their sum is 56 with denominator 2^30. All 56 also survived at 72 bytes. That small set cannot resolve a remainder loss around 2^-16. Logs are [work/confirm_00.txt](work/confirm_00.txt) through [work/confirm_15.txt](work/confirm_15.txt), summarised in [work/confirm_summary.txt](work/confirm_summary.txt).

| Pair / experiment | Trials | Verified 96-byte trail collisions | log2 measured conditional rate | Also verified at 72 bytes |
|---|---:|---:|---:|---:|
| Known, stream 20260919 | 2^26 | 6 | −23.415037499 | 6 |
| Retuned, stream 20260918 | 2^26 | 4 | −24.000000000 | 4 |
| Original-p1 optimum, stream 20260920 | 2^26 | 3 | −24.415037499 | 3 |
| Joint optimum, 16 streams | 2^30 total | 56 | −24.192645078 | 56 |

The controls are far too small to distinguish these close probabilities. Their noisy ordering is not evidence against the exact optimisation. Log2 rates over all uniform seeds for the counted event are the conditional rates minus 32; e.g. the joint confirmation estimates −56.192645078. No direct unconditioned seed brute force at that resolution was attempted.

The constructive experiment used 2^20 proposals, resulting in 979,935 E2 keys and 20 failures at 72 bytes. It chooses the free bytes uniformly, solves the other equations, and enumerates every valid exceptional-pad branch, rather than rejecting roughly 2^24 seeds per success. Every E2 assignment has a unique free-byte proposal; the sampling estimates the distribution within E2. Its 65,536 initial accepted states and every failure were compared to the reference. It is distinct from the unconditioned-within-C confirmation and is not counted toward the latter's 2^30 denominator.

Sampling experiments use at most two workers and at most 2^26 keys per invocation. The 2^30 request was interpreted as the aggregate of 16 such experiments. Exact counters are single-threaded. One early pilot overlapped the single-thread Python counter briefly; all confirmation batches and subsequent experiments ran serially. An earlier three-hit pilot using arbitrary irrelevant lanes 2,3 is retained as `work/sample_opt_00.txt` but is excluded from all confirmation totals above.

**What failed and what remains open.** The known and retuned constants both fail to maximise the exact count; the exhaustive byte search resolves every target byte, not just a 12-bit histogram bucket. The prior 239/256 ceiling and a universally independent byte pad both miss small effects: the boundary a1 carry changes the former, and the 5,104 exceptional-pad entries change the latter. These are exact arithmetic discrepancies, although too small to establish by the allowed seed-rate experiment. The claim that the 72-byte conversion always preserves E2 is false; the constructive search finds explicit full-output noncollisions at 2^20 proposals. No new SAT/SMT search or broader trail-shape search was run, and heuristic exclusions in the prior notes are not promoted to proofs here.

Still open are the exact remainder-survival count, optimisation specifically for 72 bytes, possible improvements from common words in other lanes or other difference shapes, and collisions outside C or outside this trail. In particular neither full-output epsilon nor the minimum score over all lengths is claimed to be exactly determined. The exact retuned count and exact lane-0 constant optimisation for the 96-byte trail are complete; the 72-byte result is deliberately stated as a bound plus a measured estimate because of the exhibited counterexample.

**Reproduction and artifacts.** Run `bash work/reproduce.sh` from this directory. It copies the references, builds the independent implementation and counters, checks the vectors, repeats the exact counts and validation, then runs the capped sampling experiments sequentially. No network, downloaded dependency, or external data is used. [work/pairs.json](work/pairs.json) contains all eight explicitly instantiated pairs below. Output words below use the API's lane order; each word is printed as a hex integer. All arithmetic in packet words is modulo 2^64.

**known.** Seed as four API `uint64_t` words:

```text
dbe6d5d5d80c98b5 061951046889ce9c f2c82434aaa4cd89 99683129bbc586d7
```

| Length | HighwayHash64(A) | HighwayHash64(B) |
|---:|---|---|
| 72 | `8eb9979158f14e9a` | `8eb9979158f14e9a` |
| 96 | `93938103d4f099db` | `93938103d4f099db` |

**retuned.** Seed as four API `uint64_t` words:

```text
dbe6d5d5052eed5f 4230e71d7538ad31 293fb5be5ccb0a97 851d6fe16b1bf604
```

| Length | HighwayHash64(A) | HighwayHash64(B) |
|---:|---|---|
| 72 | `caaeaf4f8379ac8b` | `caaeaf4f8379ac8b` |
| 96 | `f0741e0129fcfd89` | `f0741e0129fcfd89` |

**fixed-p1 optimum.** Seed as four API `uint64_t` words:

```text
dbe6d5d5d7f4947f 89ff68becb5b04f8 9f647cd205514249 ab23f3611204d1bc
```

| Length | HighwayHash64(A) | HighwayHash64(B) |
|---:|---|---|
| 72 | `cf50c921a12bdb93` | `cf50c921a12bdb93` |
| 96 | `7e6b998471fb1ab0` | `7e6b998471fb1ab0` |

**joint optimum.** Seed as four API `uint64_t` words:

```text
dbe6d5d5e725652c ea5870a41516d3be 375bfc01a4e18b68 a4cdf15614eb572f
```

| Length | HighwayHash64(A) | HighwayHash64(B) |
|---:|---|---|
| 72 | `2f10e0b88041d399` | `2f10e0b88041d399` |
| 96 | `7405786fe7218742` | `7405786fe7218742` |

For the joint-optimum witness above, both complete larger outputs are also identical, as follows (words in API order):

```text
72-byte HighwayHash128(A) = 6aaf5d7975f7978a 402cd8465a03fd4f
72-byte HighwayHash128(B) = 6aaf5d7975f7978a 402cd8465a03fd4f
72-byte HighwayHash256(A) = 0f82fb124b9e8aa6 150dd46d887e09fa 68d71eb75e40a343 40034fcf94697f00
72-byte HighwayHash256(B) = 0f82fb124b9e8aa6 150dd46d887e09fa 68d71eb75e40a343 40034fcf94697f00
96-byte HighwayHash128(A) = 8a725c3132dc9129 6780df5e9f826ff7
96-byte HighwayHash128(B) = 8a725c3132dc9129 6780df5e9f826ff7
96-byte HighwayHash256(A) = e068fb3ff0f3d9f3 056d2ebaec337a66 3982c5d5a1bcbe38 8b0a5ea40afbbb84
96-byte HighwayHash256(B) = e068fb3ff0f3d9f3 056d2ebaec337a66 3982c5d5a1bcbe38 8b0a5ea40afbbb84
```

Every explicitly instantiated pair follows as exact byte hex. The labels “optimum” refer to the proved 96-byte lane-0 trail optimisation, not an optimisation of the remainder-survival probability. The constant-family equations above specify the larger set considered in the exhaustive arithmetic search.

**known, 72 bytes (L = 9).**

```text
A = d131b3012a2a1924000000000000000000000000000000000000000000000000d132b3b42a2a19240000000000000000000000000000000000000000000000000000000000000000
B = d132b3012a2a1924000000000000000000000000000000000000000000000000d130b3b32a2a19240000000000000000000000000000000000000000000000000000010000000000
```

**known, 96 bytes (L = 12).**

```text
A = d131b3012a2a1924000000000000000000000000000000000000000000000000d132b3b42a2a19240000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
B = d132b3012a2a1924000000000000000000000000000000000000000000000000d130b3b32a2a19240000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000
```

**retuned, 72 bytes (L = 9).**

```text
A = d131b3012a2a192400000000000000000000000000000000000000000000000000899eb42a2a19240000000000000000000000000000000000000000000000000000000000000000
B = d132b3012a2a192400000000000000000000000000000000000000000000000000879eb32a2a19240000000000000000000000000000000000000000000000000000010000000000
```

**retuned, 96 bytes (L = 12).**

```text
A = d131b3012a2a192400000000000000000000000000000000000000000000000000899eb42a2a19240000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
B = d132b3012a2a192400000000000000000000000000000000000000000000000000879eb32a2a19240000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000
```

**fixed-p1 optimum, 72 bytes (L = 9).**

```text
A = d131b3012a2a19240000000000000000000000000000000000000000000000009db59eb42a2a19240000000000000000000000000000000000000000000000000000000000000000
B = d132b3012a2a19240000000000000000000000000000000000000000000000009db39eb32a2a19240000000000000000000000000000000000000000000000000000010000000000
```

**fixed-p1 optimum, 96 bytes (L = 12).**

```text
A = d131b3012a2a19240000000000000000000000000000000000000000000000009db59eb42a2a19240000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
B = d132b3012a2a19240000000000000000000000000000000000000000000000009db39eb32a2a19240000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000
```

**joint optimum, 72 bytes (L = 9).**

```text
A = d131cbf0290d1a240000000000000000000000000000000000000000000000009db586c52a2a19240000000000000000000000000000000000000000000000000000000000000000
B = d132cbf0290d1a240000000000000000000000000000000000000000000000009db386c42a2a19240000000000000000000000000000000000000000000000000000010000000000
```

**joint optimum, 96 bytes (L = 12).**

```text
A = d131cbf0290d1a240000000000000000000000000000000000000000000000009db586c52a2a19240000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
B = d132cbf0290d1a240000000000000000000000000000000000000000000000009db386c42a2a19240000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000
```

