# Independent verification of the fifth UMASH review

All six requested algebra and counting checks are **CONFIRMED**. All numerical constants quoted in the fifth summary are valid. No claimed inequality was refuted. The two-word ENH cases with valuation 1, 2, or 3 admit substantially stronger constants, proved below. The advertised sharp bound for the primary hash for every message pair is still open.

This is an independent derivation and implementation, not a rerun of the reviewer's programs. All enumerations ran on `<xeon-host>` (Xeon Platinum 8375C), in `<xeon-work>/umash-enh-verify`, at nice level 10 with at most 32 compute threads for this task. The Mac only edited, read, and transferred files. Certificates use integer counts and rational numbers; decimals are only explanatory. The supplied UMASH sources were not changed.

The full fifth review, including its numbered equations, was not supplied or located. The definition of the pattern weight and the linear bijection corresponding to the description of (13)–(14) are explicitly reconstructed below. They reproduce the reported constants exactly. Background results imported from the third review or GAP are identified separately from the new verification.

## Results and status

| Claim | Verdict | Independent result |
|---|---|---|
| Size and divisibility profile of D | **CONFIRMED** | 852; counts 852, 248, 64, 2, 1 for r=0,1,2,3,≥4 |
| Top bit of every nonzero mask | **CONFIRMED** | At least bit 60 |
| Lemma 1, one changed NH word | **CONFIRMED** | Uniform on multiples of 2^r, with mass 2^r/q |
| Exhaustive widths 8, 10, 12 | **CONFIRMED** | Every δ and every (A,B), including the degenerate δ=0 control; zero failures |
| Lemma 2 and its tag restriction | **CONFIRMED** | The stated κ is valid for one changed input word and |τ′−τ|≤255 |
| Sum of κ on D | **CONFIRMED** | 32042 |
| Checksum pattern bound and weight | **CONFIRMED** | ΣW = 101/2 + 909712/q < 51 |
| All 15 shufflers, restricted bijection, Φ | **CONFIRMED** | Largest ceiling 111924178297, at S=T |
| Tag source formula and boundary count | **CONFIRMED** | τ=seed XOR (size mod 256); 16·255=4080 boundary points |
| Product high-word point mass | **CONFIRMED** | Less than 48/q |
| Tag primary / conditional secondary / joint | **CONFIRMED** | 195840/q; 61·2^-52; 11946240·2^-116 |
| One-word ENH-only joint constants | **CONFIRMED** | (N_r 2^r)^2/q² for r≤3; 27299784/q² for r≥4 |
| One PH + one-word ENH, equal checksum | **CONFIRMED** | Same small-r bound; Φ_S/q² for r≥4 |
| Two-word NH raw low equality | **CONFIRMED** | Exactly 2^r/q |
| Two-word ENH-only, r=1,2,3, including the 113280 formula | **CONFIRMED, strengthened** | Joint numerators 777728, 261120, 384 over q², respectively; all <2^-108 |
| Two-word ENH-only, r≥4 | **CONFIRMED** | 852·2^r/q²; below 2^-87 through r=31 |
| One PH + two-word ENH, equal checksum, r≥4 | **CONFIRMED** | (ΣW)·2^r/q² <51·2^r/q²; below 2^-87 through r=35 |
| Odd two-word lifting, r=0 | **CONFIRMED** | Joint bound 852²/q² in both configurations |
| Distinct-word key correction | **CONFIRMED** | Divide IID event bounds by 1−561/q; every stated rounded strict inequality retains slack |

The strengthened constants are improvements, not evidence that the larger original constants were erroneous. There are no **REFUTED** rows and no numerical correction is required for the six requested checks.

## Model and source correspondence

Let q=2^64, p=2^61−1. Words and products below are unsigned unless a signed difference is displayed. All 34 OH key words are initially independent and uniform; messages and their common seed are fixed before drawing keys. A block is a sequence of expanded 16-byte chunks, with its final chunk mixed by ENH. Write

```
A′ = A+δ mod q,       B′ = B+ε mod q,
L = AB mod q,         H = floor(AB/q),
U = H+τ mod q,        E = (L, U XOR L).
```

For a one-word change, ε=0 and δ≠0; exchanging operands gives the other orientation. For a two-word change, both are nonzero and r=min(v2(δ),v2(ε)). The valuation of a nonzero integer difference modulo q equals the valuation of the corresponding input XOR difference. Tags affect no low word.

The ENH formula is in `umash_reference.py:1098–1106` and `umash-src/umash.c:557–568`. The reference's tag is qτ. Shuffling and checksums are in reference lines 1092–1158. These facts are also directly present in the source AST used by the shuffler certificate.

“Primary reduced collision” means equality of both primary OH lanes modulo p. “Joint” means the corresponding event for both compressors. A joint upper bound does not upper-bound the primary marginal. Neither is, by itself, an upper bound on collisions of the completed polynomial hash.

## 1. The exact mask set D

Define D={x XOR y : 0≤x,y<q, x≡y (mod p)}. To enumerate it independently, use y=x+jp for j=0,…,8, and a bitwise addition automaton. At bit i, for each carry c and choice of x_i, compute

```
s = x_i + (jp)_i + c,
y_i = s mod 2,       next carry = floor(s/2),
d_i = x_i XOR y_i.
```

Merge paths with the same carry and accumulated mask; retain only final carry zero. Every accepted path is an actual nonoverflowing pair, and every such pair follows an accepted path. Reversing x and y does not change the XOR, so j≥0 is complete. This is an addition-state enumeration, independent of the signed-support program described in GAP.

The counts before union, for j=0,…,8, are

```
1, 184, 123, 361, 62, 355, 121, 178, 1.
```

The union has 852 members. Its full list, admissible patterns, valuations, κ values, and weights are in [certificates/masks.json](certificates/masks.json). Direct enumeration of every congruent pair and every admissible pattern agrees at (w,p)=(8,31),(10,127),(12,509).

For each d, a word x with x XOR y=d satisfies

```
x−y = 2(x & d)−d.
```

Thus the possible patterns on the support of d are exactly

```
P(d) = {(d+jp)/2 : −8≤j≤8, d+jp even and nonnegative,
                    ((d+jp)/2) & d = (d+jp)/2}.
```

The profile is N_0=852, N_1=248, N_2=64, N_3=2, and N_r=1 for 4≤r≤64. In particular D∩8Z={0,q−8}. There are 184 masks of valuation exactly 1 and 62 of valuation exactly 2.

Two useful facts also have immediate noncomputational proofs. If 16 divides x XOR y, then 16 divides x−y=jp; since p is odd and |j|≤8, j=0. If a nonzero mask had top bit below 60, then |x−y|≤x XOR y<2^60<p, also impossible. Hence every nonzero mask has top bit at least 60.

## 2. Lemma 1

Let δ≠0, r=v2(δ), and let A,B be independent uniform w-bit words, Q=2^w. Write t=v2(B). If t≥w−r, both low products agree; the total probability of this case is 2^r/Q.

Otherwise write B=2^t b, where b is a uniform odd word modulo M=2^(w−t). Conditional on t,

```
C = A b mod M,
c = δ b mod M
```

are independent: C is uniform on all words and c is uniform on words of valuation r. This follows because multiplication by odd b permutes A, and multiplication by δ maps uniform odd b uniformly to the valuation-r class. The map

```
(C,c) ↦ (C, C XOR (C+c mod M))
```

is a bijection between the pairs with v2(c)=r and the pairs with XOR difference of valuation r. Therefore the original low XOR difference is uniform on words of valuation r+t, conditional on t. Each such word has unconditional mass

```
2^(-t-1) / 2^(w-r-t-1) = 2^r/Q.
```

Together with the zero case, this is exactly uniformity on all multiples of 2^r. The argument uses averaging over both A and B: it does not claim uniformity for every fixed A.

The literal enumerator [code/exhaustive.cpp](code/exhaustive.cpp) checked:

| Width | δ values | (δ,A,B) triples | Failures |
|---:|---:|---:|---:|
| 8 | 256 | 16777216 | 0 |
| 10 | 1024 | 1073741824 | 0 |
| 12 | 4096 | 68719476736 | 0 |

δ=0 is an additional degenerate control, with all mass at zero. Each per-δ histogram was compared entry by entry with its exact expected count. The JSON files retain traversal totals and histogram digests.

## 3. Lemma 2

This section concerns one changed word. Let Q=2^w and r=v2(δ). Low equality is equivalent to B=mQ/2^r, 0≤m<2^r. Partition A into its two wrap intervals, c=0 or 1, with A′=A+δ−cQ. In each interval the tagged high additive difference is

```
s = ((δ−cQ)/2^r) m + τ′−τ  mod Q.             (1)
```

Both coefficients of m are odd. A prescribed s has at most one m in each wrap interval; the lengths of the two A intervals sum to Q. Hence the event of low equality and a specified high additive difference has probability at most 1/Q. This supplies the needed additive fact directly, without importing an NH theorem.

Under low equality, ENH's triangular XOR cancels, so its high XOR difference equals U XOR U′. Put h=popcount(e), g=2^bitlength(e)−e, with g(0)=1.

**First bound.** U XOR U′=e implies s=e−2z modulo Q for z=U&e, a submask of e. There are at most 2^h additive targets. Thus the event has probability ≤2^h/Q.

**Second bound.** Fix a nonzero B=mQ/2^r and a wrap interval. Equation 2z=e−s modulo Q has at most two submask solutions. Consequently at most 2^(w−h+1) tagged high words are possible. Each high word floor(AB/Q) has at most ceil(Q/B) preimages A. Summing both intervals and all m≥1, and adding B=0, gives an event probability at most

```
[1 + 4·2^(r−h)·(H_(2^r−1)+1)]/Q
 ≤ [1 + 4·2^(r−h)·(r+1)]/Q.
```

Here H_(2^r−1)≤r by grouping its terms into dyadic ranges. For w=64 and r≤63 this is ≤(1+2^(71−h))/q, which is stronger than the stated second branch (5+2^(71−h))/q. The width-12 numerical check uses the safe analogue 5+2^(w+7−h).

**Third bound.** For a nonzero XOR mask e, every ordinary signed difference between words with that XOR has absolute value at least g. Its circular distance modulo Q is also at least g: the highest changed bit cannot be canceled by lower changed bits, and a wrap can only improve the lower bound to Q−e≥g. Before adding tags, the high-word difference has absolute value |δ−cQ|B/Q<B. Thus, if |τ′−τ|≤255 and g>255, the event requires B>g−255.

Alternatively enumerate each of the 2^h patterns z=U&e. Equation (1) permits at most one B in each wrap interval. For this z there are 2^(w−h) possible U values, each with at most ceil(Q/(g−255)) preimages A. The total event probability is therefore at most

```
2·ceil(Q/(g−255))/Q,
```

again stronger than the requested (5+ceil(4Q/(g−255)))/Q. These three arguments establish the stated κ exactly as an upper bound. No low/high independence is assumed.

Independently summing that stated κ on the complete D list gives **32042**. [code/lemma2.cpp](code/lemma2.cpp) checked all 4095 nonzero δ, all 4096 high XOR targets, and ten tag pairs at width 12. It enumerated 100663296 low-equal (δ,A,B) triples and checked 167731200 inequalities, with zero failures. Restricting B to the proved equality grid does not omit any relevant pair. The prime-509 mask set is independently certified, but this check is stronger: it tests every high XOR target, regardless of membership in D.

The selected tag pairs include zero gap, gaps ±1 and ±255, high-word wrap, and extra pairs spanning a low-byte page boundary that the production tag need not realize. This is not an exhaustive check of all tag pairs. [code/exact64.py](code/exact64.py) additionally counts 120 specified real 64-bit instances exactly, including e=0 and dense nonzero masks. It solves for U&e, enumerates only the at-most-eight unmasked bits, then counts A by integer interval endpoints. All satisfy κ/q; the certificate contains every exact numerator over q².

## 4. The common checksum mask

With equal XOR checksums, condition on all original chunk keys. The extra two twisting keys still make the common checksum product Z=U⊙V a product of independent uniform binary polynomials of degree below w. For a fixed nonzero U of degree k, output high-word bits j≥k are zero. Any selected high bits with j<k are independent linear functions of V: reverse triangular elimination uses the leading coefficient of U and distinct V_(w+j−k) pivots.

Writing J for a support mask, any prescribed pattern on J consequently has probability at most

```
f_H(J) = 2^(-w) + Σ_(k=0)^(w−1) 2^(k−w)·2^(-popcount(J mod 2^k)).       (2)
```

The first term handles U=0. Conditioning on additional common XOR masks only translates the target pattern. For nonzero v, there are at most 16 admissible reduction patterns P(v): j=0 cannot represent a nonempty signed binary sum of zero, leaving at most the 16 nonzero integers j∈[−8,8]. For v=0 the event is unrestricted. Therefore

```
W(v)=min(1,16 f_H(v))
```

is valid, including v=0. The exact pattern histogram actually has maximum 8, but the stated weight uses 16 and is the one audited here.

All summands in q f_H are integers. The independent sum is

```
Σ_D W(v) = 931560575722333266320 / 18446744073709551616
         = 101/2 + 909712/q < 51.
```

[code/patterns.cpp](code/patterns.cpp) also enumerates all 2^24 pairs U,V at width 12, forms the exact high-word distribution, and checks every target pattern for every one of the 4096 support masks. There are zero violations of (2). No claim of independence between low and high product words is needed.

## 5. Shufflers, the bijection, and Φ

The reference calls `shuffle(mixed_chunk, i+1, n)` with zero-based chunk index i and n original chunks. The final ENH chunk is unshifted. The possible nonfinal shufflers are exactly

```
S_1=T,     S_k=T+T^k,  2≤k≤15,
```

where T shifts one lane left by one bit with truncation. [code/certify.py](code/certify.py) extracts the actual `shuffle` function from the reference AST and evaluates every lane basis vector for every permitted chunk count and index. This produces exactly these 15 maps, with no cross-lane shift.

For a PH difference ΔP and ENH difference ΔE, the raw compressor differences are ΔP+ΔE and SΔP+ΔE, where addition in this paragraph means XOR. If all data differences have valuation at least 4, both low differences are divisible by 16. Reduced collisions force both low differences to zero by D∩16Z={0}; the invertibility of I+S then forces ΔP_lo=ΔE_lo=0.

Put x=ΔP_hi, e=ΔE_hi, and let u,v be the primary and secondary high XOR differences. The reconstructed bijection is

```
(x,e) ↦ (u,v)=(x+e, Sx+e),                                   (13)
(u,v) ↦ (x,e)=((I+S)^(-1)(u+v), u+(I+S)^(-1)(u+v)).           (14)
```

I+S has diagonal 1 and is triangular in bit order; equivalently S is nilpotent, so I+S+…+S^63 is its inverse. Equations (13) and (14) are inverse on all pairs of words. On the actual PH domain one must additionally require **x<2^63**: a full 64×64 carryless product has no bit 127, and XORing two such products cannot create that bit. The restriction is necessary, but is not asserted sufficient for an x to be attainable for a fixed message difference. Including unattainable x is a safe enlargement in an upper bound.

For one-word PH and ENH changes with equal checksums, the PH difference is a nonzero fixed polynomial times one uniform key word, so each full target (0,x) has probability at most 1/q. Its keys are independent of the ENH keys. Lemma 2 supplies κ(e)/q for (0,e). After those keys are fixed, the checksum keys supply W(v). Summing the necessary targets gives

```
Pr[joint reduced collision] ≤ Φ_S/q²,
Φ_S = Σ_(u,v∈D; x=(I+S)^−1(u+v)<2^63) κ(u+x)W(v).
```

[code/phi.cpp](code/phi.cpp) checks the forward/inverse equations for all 852² pairs for every shuffler, then sums with unsigned 128-bit integers. The denominator for every Φ is q; complete unreduced numerators and accepted/rejected pair counts are in [certificates/phi.json](certificates/phi.json).

| k | S | ceil(Φ_S) |
|---:|---|---:|
| 1 | T | 111924178297 |
| 2 | T+T² | 1815977733 |
| 3 | T+T³ | 327737110 |
| 4 | T+T⁴ | 67727364 |
| 5 | T+T⁵ | 25519452 |
| 6 | T+T⁶ | 9687856 |
| 7 | T+T⁷ | 5368944 |
| 8 | T+T⁸ | 2988706 |
| 9 | T+T⁹ | 4092938 |
| 10 | T+T¹⁰ | 3705368 |
| 11 | T+T¹¹ | 33055943 |
| 12 | T+T¹² | 8552106 |
| 13 | T+T¹³ | 12418925 |
| 14 | T+T¹⁴ | 10663708 |
| 15 | T+T¹⁵ | 37073909 |

The exact maximum is 2064636672698114052692213691888/q, whose ceiling is below 2^37. Thus the claimed joint bound <2^-91 follows.

For r≤3, both PH and one-word NH low XOR differences are independent uniform members of 2^rZ/qZ. The same invertible map preserves that subspace. There are N_r² possible low target pairs, giving (N_r2^r/q)²<2^-108.

## 6. Tag-only pairs

In the reference, `size_tag = block_size % (CHUNK_SIZE * BLOCK_SIZE)` and `tag = (seed ^ size_tag) * W` at lines 1156–1157. In production, full 256-byte blocks use `seed`; the final block uses `seed ^= (uint8_t)n_bytes` at `umash.c:902`, with the fingerprint equivalent at line 964. The 9–16 byte path uses `seed ^ n_bytes` at line 803, and the streaming path uses `seed ^ (uint8_t)(block_size + bufsz)` at line 1148. Thus the high-word tag is exactly τ=seed XOR (block_size mod 256). With a common fixed seed the high 56 tag bits agree, so the **ordinary signed** difference has magnitude at most 255, even when the seed is near q−1. This is not a statement about comparisons using different seeds.

For identical expanded chunks and distinct tags, the low words agree. Let d=τ′−τ≠0. The high XOR difference equals

```
e=(H+τ mod q) XOR (H+τ′ mod q).
```

Primary reduction requires a nonzero e∈D, hence a change in a bit numbered at least 60. Moving by signed d on the word circle must cross one of the 16 boundaries at multiples of 2^60. For a fixed direction there are exactly |d| starting words at each boundary. These sets are disjoint because |d|<2^60. Translating back by τ gives **16|d|≤4080** possible H values. Counting 255 points on both sides of each boundary would unnecessarily double the count; the sign is fixed.

For any fixed h, A=0 contributes at most q pairs. For each A>0 the interval

```
qh ≤ AB < q(h+1)
```

contains at most q/A+1 integers B. Consequently the total pair count is at most q(H_(q−1)+2)<q(64 ln 2+3)<48q. The last comparison needs no floating arithmetic: the first four positive series terms of exp(7/10) sum to 12013/6000>2, so ln 2<7/10 and 64 ln 2+3<239/5<48. Division by q² gives the claimed strict bound Pr[floor(AB/q)=h]<48/q. The boundary event therefore has probability <195840/q. No distributional assumption about a common PH XOR mask enters this step.

For the conditional secondary bound, fix all original chunk keys. The two checksum products agree and remain an independent random Z=U⊙V over the twisting keys. The circular distance between the tagged high words is |d|≤255, so the earlier gap argument implies g(e)≤255. If the top bit of e is k≥60, this forces every bit 8,…,k of e to be set. Ignore all but bits 8,…,60 in the pattern lemma. Formula (2) gives exactly

```
f_H(bits 8,…,60) = 15616/q = 61·2^-56,
16 f_H = 61·2^-52.
```

This is a bound conditional on the original keys and the primary event. Multiplication is therefore legitimate:

```
Pr[joint] < (195840/q)(61·2^-52)
          = 11946240·2^-116 < 2^-92.
```

The boundary checker exhausts all low seed bytes and all length-byte pairs, and checks all 511 signed gaps −255,…,255 at the width-12 analogue. The exact 64-bit constants above are certified separately. Tag-only distinct pairs can occur at unequal byte lengths; at equal length their tags agree.

## 7. One-word and two-word ENH consequences

If only ENH data change, common PH products cancel in the raw XOR difference even though they cannot be canceled through reduction. All arguments below use the necessary XOR-mask conditions, so an arbitrary common PH mask is allowed; zero PH chunks are allowed too.

For a one-word change with r≤3, Lemma 1 gives primary bound N_r2^r/q, with constants **852, 496, 256, 16**. Conditional on original chunk keys, the changed checksum PH has a low XOR difference uniform on the same multiples of 2^r. This supplies a second N_r2^r/q factor. For r≥4, low reduction forces equality; summing Lemma 2 gives primary bound 32042/q. A full checksum PH XOR target has probability at most 1/q, and its low target must be zero; its high difference can occupy only 852 targets. This gives 852·32042/q²=27299784/q².

For two changed ENH words,

```
L′−L = δB+εA+δε mod q.
```

Its image is exactly the multiples of 2^r, with every value having q2^r preimages. Hence Pr[L=L′]=2^r/q exactly. At r≥4 this is already a primary upper bound. The same conditional checksum argument gives joint ≤852·2^r/q².

If r=0, choose δ odd by swapping operands. For fixed A, bit j of L XOR L′ depends on bit j of B with coefficient A_0 XOR A′_0=1, and on already fixed lower bits. The map B↦L XOR L′ is triangular and bijective. Thus its low difference is uniform, even with both inputs changed. This gives primary ≤852/q and joint ≤852²/q². It also supplies the r=0 PH+ENH low-word argument.

**A stronger independent treatment of r=1,2,3.** Put δ=2^r a with a odd and ε=2^r b, and set

```
C = aB+bA+2^r ab mod q,      Y=AB mod q.
```

For fixed A the map B↦C is bijective, and

```
aY = −bA²+(C−2^r ab)A mod q.                         (3)
```

For a prescribed XOR target v divisible by 2^r, Y determines C modulo q/2^r, leaving 2^r lifts. If v2(v)=r, C is odd. The linear coefficient of (3) is then odd. A quadratic with odd linear coefficient has at most two roots modulo q: each root modulo 2 lifts uniquely at each bit. Summing q possible Y and 2^r lifts proves

```
Pr[L XOR L′=v] ≤ 2^(r+1)/q   when v2(v)=r≥1.          (4)
```

When r=1 and v2(v)=2, C≡2 (mod 4). If b is even, both quadratic coefficients in (3) are even and the linear coefficient has valuation 1. Divide by 2; the new linear coefficient is odd, so there are at most four A modulo q, and only q/2 possible even Y. The resulting bound is 4/q.

If b is odd, the linear coefficient is divisible by 4, and A and B have the same parity. On the even-even event, divide both inputs and both changes by 2. Their products, divided by 4, are an odd-change NH instance modulo q/4. Odd lifting gives an unconditional contribution exactly 1/q for each target divisible by 4. On the odd-odd event, complete the square in (3). The center is even and the square root is odd; an odd number has at most four square roots modulo 2^64. To see the latter fact, divide any root by one fixed odd root: t²=1, and one of t−1,t+1 has valuation exactly 1, forcing t≡1 or −1 modulo q/2. There are at most q/2 odd Y and two C lifts, so this contributes at most 4/q. Hence, uniformly in b,

```
Pr[L XOR L′=v] ≤ 5/q   for r=1 and v2(v)=2.           (5)
```

The only nonzero D mask divisible by 8 is v=q−8. For this target,

```
L+L′ = q−8+2(L mod 8) mod q.
```

Since δ,ε are even,

```
(L+L′)/2 = (A+δ/2)(B+ε/2)+δε/4 mod q/2.
```

The product of two independent uniform residues modulo q/2 has point probability at most 65/q, including zero. This follows by summing the valuation mixture; zero is the largest point mass. There are only eight target sums, so this XOR target has probability at most 520/q.

Using the exact valuation counts in D and the exact raw-equality term yields:

| r | Primary constant C_r in C_r/q | Joint constant C_r N_r2^r in units of 1/q² |
|---:|---:|---:|
| 1 | 2+184·4+62·5+520 = **1568** | **777728** |
| 2 | 4+62·8+520 = **1020** | **261120** |
| 3 | 8+16 = **24** | **384** |

These prove the review's looser 113280 N_r2^(2r)/q² inequality without relying on its omitted argument. [code/two_word.cpp](code/two_word.cpp) independently checks (4), (5), the q−8 bound, and low equality against literal all-key histograms for every even nonzero increment pair of minimum valuation 1–3 at width 8, plus structured width-10 and width-12 pairs. This includes odd/even normalized increments and wrap cases. All inequalities passed.

For one PH chunk plus a two-word ENH change with equal checksums and r≥4, both raw low differences must again vanish. A full PH difference has point mass at most 1/q, including the two-word case. For each fixed ENH high difference e, each secondary target v admits at most one x<2^63 solving Sx=e+v: S has only the kernel {0,2^63}, and the restricted domain excludes two points in one kernel coset. Summing W(v), then averaging the NH low-equality event, proves

```
Pr[joint] ≤ (Σ_D W(v)) 2^r/q² <51·2^r/q².
```

Thus the review's joint thresholds remain valid: ENH-only two-word changes are below 2^-87 for 0≤r≤31; one PH plus two-word ENH with equal checksums is below that threshold for r=0 and 4≤r≤35. The high-r inequalities continue to be valid outside those ranges, but no longer certify 2^-87. This verification does not settle the PH+ENH cases r=1,2,3 or r≥36 at that threshold.

## 8. Primary 64-bit hash: complete case ledger

The following is a ledger of **primary reduced-compressor or zero-polynomial bounds**, not joint fingerprint constants. All displayed probabilities are for IID words. Clip any bound at 1 and take the minimum of overlapping rows.

For aligned blocks with equal chunk counts:

| Configuration | Proved primary reduced-collision bound | Basis |
|---|---|---|
| At least one PH chunk differs, arbitrary other changes and tags | 725904/q | GAP's injective PH XOR-target argument, with independently recounted 852² |
| Some differing PH word has odd XOR difference | 17/q | Third-review borrow/bit-lifting argument |
| Exactly one word of one PH chunk changes; all other data and tags agree | <65/q | Third-review one-word PH result |
| Global minimum data-word valuation r is attained in a PH difference | N_r2^r/q, also 17/q when r=0 | Uniform low PH difference after conditioning the other keys; all offsets lie in the same subspace |
| PH chunks agree; one ENH word changes, r=0,1,2,3 | 852/q, 496/q, 256/q, 16/q respectively | Lemma 1 |
| PH chunks agree; one ENH word changes, r≥4 | min(32042,2^r)/q | Lemmas 1 and 2 |
| PH chunks agree; both ENH words change, r=0 | 852/q | Odd lifting |
| PH chunks agree; both ENH words change, r=1,2,3 | 1568/q, 1020/q, 24/q respectively | New quadratic argument above |
| PH chunks agree; both ENH words change, 4≤r≤63 | 2^r/q | Exact raw low equality and D∩16Z={0} |
| All expanded data agree; only the tag differs | <195840/q | Boundary and product counting |

In the PH minimum-valuation row, N_r=1 for r≥4, so the generic PH bound can be improved to min(725904,2^r)/q. To justify the row, expand the differing PH product: its XOR difference is a nonzero fixed polynomial times a free uniform key word plus a conditioned constant. A coefficient of valuation r maps that key uniformly to the low-word subspace of multiples of 2^r. Every other data difference also has valuation at least r, so the total conditioned offset belongs to that same subspace. This does not assume independence of output lanes or of the two compressors. In particular, it applies to one PH plus ENH with equal checksums, whether one or two ENH words change.

GAP additionally gives exact small primary families: one PH word XOR-changed by 1 has probability 8/q+72/q²; changing one PH chunk by (1,1) with other chunks unchanged gives 8/q+147/(2q²) at two chunks and <9/q for 2–16 chunks. Those prior exact-family computations were not rerun here; they are not claims about arbitrary even PH changes.

The 17/q odd-PH argument can be seen directly: condition all but the key word paired with an odd coefficient difference. For each signed multiple kp, −8≤k≤8, the low-lane equation determines that free key successively, one bit at a time, because the two product coefficients have opposite parity. Each target has at most one key. The separate <65/q single-PH-word result and the exact-family results above are carried from the supplied background, rather than newly certified by this run.

Length configurations, combining and sharpening the supplied third-review and GAP ledger:

| Configuration | Proved primary bound | Event being bounded |
|---|---|---|
| Different chunk counts in an aligned block | U=(82q−81)/q² <82/q | Reduced equality of that block |
| Different numbers of long-input blocks | U | Formal identity of the two primary polynomials |
| One short input (≤8 bytes), one long input | 9/q | Formal zero comparison polynomial after undoing the long finalizer |
| Distinct short inputs of equal length | 0 | Actual primary hash collision |
| Short inputs of different lengths | 1/q | Actual primary hash collision under IID words |

The <82/q value is stronger than the third summary's <162/q and was already justified in GAP: a reduced point has at most 81 raw preimages; at most one inverse product target is zero, with 2q−1 pairs, and each other target has at most q−1 pairs. This totals 82q−81 pairs. A longer aligned block has a fresh final ENH pair after the shorter block's keys are fixed. With unequal block counts, the longer polynomial's two leading coefficients must both vanish for formal identity, so the point bound applies without assuming a nonzero leading coefficient. For short/long, formal identity forces the inverse-finalized short word to have residue zero; its uniform word distribution has exactly nine such representatives.

For equal block counts, select a differing aligned expanded-block/tag tuple before sampling keys. Whole-polynomial identity implies reduced equality for that block. The encoding of chunks together with the length tag distinguishes the original byte strings. Therefore these rows exhaust the configurations needed for primary formal-identity bounds. No independence between different blocks is required.

For full primary UMASH, if B is the applicable zero-polynomial bound, the conservative root completion from GAP is

```
Pr[full primary collision] ≤ B + (1−B)·r_s,
r_s = min(1, 2 ceil(s/256)/(p−2)),
```

where s is the larger byte length. Distinct-key conditioning affects B, not the independent polynomial-root term. The production implementation permits multiplier 1; the denominator p−2 remains conservative. A claim about the final primary hash must include this step; the Φ or tag joint bounds cannot replace B.

**What remains open.** A reduced bound now exists in every configuration in this ledger, but some are far too weak to prove the published primary 55-bit envelope. In particular, no sharp all-pairs primary theorem follows for general even PH differences outside the established special cases, for general ENH changes at the large valuations, or for tag-only pairs. Upper constants such as 725904, 32042, 195840, and especially 2^r are not small-constant primary repairs. Even some small-r ENH bounds exceed the advertised budget; their being proved does not make them sharp. No counterexample to the advertised primary bound is produced here.

For the joint compressor threshold <2^-87, combining the third review with the new proofs covers different chunk counts; different checksums with a PH difference; equal checksums with at least two PH differences (the third review's 345763417/2^116); all one-word ENH-only cases; all one PH plus one-word ENH equal-checksum cases; tag-only pairs; and the two-word ranges just listed. The remaining joint threshold cases are precisely ENH-only two-word r=32,…,63, and exactly one PH plus two-word ENH with equal checksums at r∈{1,2,3}∪{36,…,63}. These are failures to certify that threshold, not counterexamples and not absence of any upper bound. Prior length and short/long completion results in GAP still apply; closing block cases alone must not be presented as a complete fingerprint headline theorem.

## 9. Key distribution and reproducibility

There are 34·33/2=561 pairs of OH key positions. Under IID sampling the probability of no duplicate is at least D0=1−561/q. For every OH-key event, conditioning on distinctness gives P(event | distinct)≤P(event)/D0. The rational checks in [certificates/arithmetic.json](certificates/arithmetic.json) verify the stated strict power-of-two bounds with this correction. Independence-based proofs above are performed before this conditioning. They are information-theoretic statements about the ideal full-key model, not a distribution theorem for a fixed-size Salsa20 seed.

Run [code/run_xeon.sh](code/run_xeon.sh) on the Xeon from this directory. It runs one compute job at a time, with OpenMP capped at 32 and nice level 10. No external Python packages are required. [certificates/manifest.json](certificates/manifest.json) records source hashes, environment, and certificate hashes. The individual JSON files include exact mask lists, all 15 Φ numerators, exhaustive enumeration counts, selected 64-bit exact counts, and the new small-even two-word checks. Assertions fail the run on a violated inequality or inconsistent certificate.

The finite certificates supplement the proofs; toy checks are not extrapolated into 64-bit claims. This is computer-assisted verification, not a proof-assistant formalization.
