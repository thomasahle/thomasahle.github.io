# The remaining PH+ENH joint case

## 1. Result and scope

Write q=2^64 and p=2^61−1. For the literal reference/Lean block model,
with independent uniform OH words, this document proves:

**Theorem 1.** If two valid blocks have the same chunk count, equal data XOR
checksums, exactly one differing nonfinal PH chunk, and both final ENH words
differ, then, whenever the minimum valuation r of those two final differences
is positive,

\[
 \Pr[\text{both lane-wise projected compressors agree}]
 \le {170906186782\over q^2}<2^{-90}<2^{-87}.                 \tag{1}
\]

In particular, this proves the exact proposition `OpenPHENH` in
`materials/UMASHObligations.lean`, including r=1,2,3 and every 36≤r≤63.
The strict bound 2^-90 also survives conditioning all 34 OH words to be
distinct. No independence between the completed compressors is assumed.

The proof uses two new quadratic counts: an integer-line count for high ENH
XOR differences, and a modular quadratic count for low ENH XOR differences.
It does not apply the common-offset lemma from `materials/PROOF.md` to two
different PH offsets. The previously closed ENH-only case is not reproved.

Section 9 proves an additional sharp primary *tag-only* bound, less than
1/(8q), with any common PH offset. This removes one obstruction in the
primary case ledger. **`SharpPrimaryProjection` remains open.** Accordingly,
this is a partial resolution of the requested pair of obligations, not a
proof or refutation of either published full-hash headline.

All numerical certificates are exact integers or rationals. Reproduction is
`bash checks/run_xeon.sh` on the specified Xeon. The finite sums are upper
bounds, not attained collision probabilities.

## 2. Model, masks, and the shuffler elimination

A word is an integer in [0,q). Bitwise operations apply to its 64 digits.
Carryless multiplication, written a⊙b, is multiplication of word polynomials
in F₂[X], with its unreduced product stored in two lanes. Ordinary products
are denoted AB.

For the final chunk, translating the independent additive keys gives
independent uniform A,B. There are fixed nonzero increments δ,ε in [1,q)
such that

\[
 A'=(A+\delta)\bmod q,\quad B'=(B+\epsilon)\bmod q,
 \quad AB=qH+L,\quad A'B'=qH'+L'.
\]

For fixed tags τ,τ' in [0,q), put U=(H+τ) mod q and U'=(H'+τ') mod q.
The implemented ENH outputs are (L,U XOR L) and (L',U' XOR L'). The arguments
for Theorem 1 allow arbitrary fixed tags.

Define the finite mask set and its patterns by

\[
\begin{split}
 D&=\{x\mathbin\oplus y:0\le x,y<q,\ x\equiv y\pmod p\},\\
 P(d)&=\{(d+jp)/2:-8\le j\le8,\ d+jp\ge0,\ d+jp\text{ even},\
 &\hspace{43mm}((d+jp)/2)\mathbin\&d=(d+jp)/2\}.
\end{split}                                                     \tag{2}
\]

For words x,y and d=x XOR y, the exact integer identity

\[
 x-y=2(x\mathbin\&d)-d                                      \tag{3}
\]

shows that projected equality is equivalent to x&d∈P(d). It also proves
that P(d) is nonempty exactly when d∈D: any pattern t is realized by the
words t and d XOR t, with arbitrary common bits outside d.

The certificate proves

\[
 |D|=852,\quad 1\le |P(d)|\le8\ (d\in D),\qquad
 (|D\cap2^r\mathbb Z|)_{r=0}^4=(852,248,64,2,1).             \tag{4}
\]

The assertion D∩16ℤ={0} also follows directly: if 16 divides x XOR y,
then 16 divides x−y=jp, so 16 divides j; |j|≤8 forces j=0. Every nonzero
mask has highest bit at least 60, since p≤|x−y|≤x XOR y.

Let T be a one-bit left shift with truncation, separately in each lane.
The shuffler at a changed PH position is one of

\[
 S_1=T,\qquad S_k=T+T^k\quad(2\le k\le15),                 \tag{5}
\]

where + in formulas for these maps means XOR. These are precisely the maps
given by `shuffle(mixed_chunk,i+1,n)` in the reference. Its last original
chunk is ENH and is unshifted. Since S is strictly triangular in ascending
bit order, I+S is invertible; solving successive bits constructs its inverse.

Condition on all unchanged original chunk keys. Let ΔP be the XOR difference
of the changed PH products, and ΔE the ENH XOR difference. Equal data
checksums imply equal keyed checksums, because both messages use the same
original keys at the same positions. Their twisting PH products therefore
agree. The two raw compressor differences are exactly

\[
 \Delta P+\Delta E,\qquad S\Delta P+\Delta E.                \tag{6}
\]

In either lane, prescribing their masks u,v determines the separate
differences uniquely:

\[
 x=(I+S)^{-1}(u+v),\qquad e=u+x.                            \tag{7}
\]

Equal checksums and exactly one changed PH chunk also imply that the PH
coordinate XOR differences equal the ENH coordinate XOR differences.
For distinct words, their XOR and their additive difference modulo q have
the same valuation, namely the index of their lowest differing bit. Thus
both PH coordinate differences are divisible by R=2^r, at least one with
valuation exactly r; the same is true of δ,ε. Exchange ENH operands if
necessary so v₂(δ)=r≤v₂(ε).

If r≥4, both low differences in (6) are divisible by 16. Their projected
equalities force both to be zero. Invertibility of I+S then gives

\[
 \Delta P_{lo}=0,\qquad L=L'.                              \tag{8}
\]

In the high lane, e=U XOR U' because the common L cancels in the fold. Also
x<2^63: a 64-by-64 carryless product has degree at most 126, so its high
lane has bit 63 zero. For r≤3, we use only the two low lanes; their masks
u,v belong to D∩Rℤ, and (7) supplies their PH and ENH low differences.

## 3. PH point bounds and the independent checksum weight

**Lemma 3.1.** For a changed PH chunk and IID keys, any full prescribed XOR
difference has probability at most 1/q. Its low XOR difference is uniform on
the multiples of R when the coordinate differences have minimum valuation r.

**Proof.** Condition on one companion key, choosing the other key V so its
coefficient d is a nonzero coordinate difference, and v₂(d)=r for the second
assertion. Expansion gives d⊙V XOR c. Multiplication by nonzero d in F₂[X]
is injective. Modulo X^64, d=X^r g with g(0)=1, so multiplication has image
the multiples of X^r and exactly R preimages per image. The affine constant
is itself divisible by X^r, since both original products have the same low
r bits. This proves both assertions, including after averaging the companion
key. ∎

For a word mask v put h(v)=popcount(v), and define

\[
\begin{split}
 f_H(v)&=q^{-1}+\sum_{j=0}^{63}2^{j-64}
          2^{-h(v\mathbin\&(2^j-1))},\\
 f_L(v)&=q^{-1}+\sum_{j=0}^{63}2^{-j-1}2^{-h(v\mathbin{>>}j)},\\
 W_H(v)&=\min(1,|P(v)|f_H(v)),\quad
 W_L(v)=\min(1,|P(v)|f_L(v)).
\end{split}                                                     \tag{9}
\]

**Lemma 3.2.** After every original key has been fixed, secondary projected
equality in a lane with XOR mask v has conditional probability at most
W_H(v) or W_L(v), respectively, over the twisting keys.

**Proof.** XOR translation by the now fixed common checksum makes the two
twisting operands independent uniform words C,D. Their common PH product
Z=C⊙D is added by XOR to the two secondary outputs. Other terms are fixed.
For fixed nonzero C of degree j, any selected high positions below j are
independent linear functions of D: use the leading coefficient of C and
eliminate in reverse bit order. Positions at or above j are zero. Degree j
has probability 2^j/q. The separate C=0 case has probability 1/q. This gives
f_H(v) as an upper bound for each prescribed pattern of the high word on v.
For fixed C of valuation j, low positions at least j are independent by
forward triangular elimination; its probability is 2^(-j-1). Including C=0
gives f_L(v). There are |P(v)| acceptable translated patterns by (3).
A union bound and clipping at one prove (9). This argument is conditional
on every original key and is uniform over their values. ∎

## 4. Integer quadratics and high ENH XOR masks

**Lemma 4.1 (quadratic interval count).** Let F(t)=at²+bt+c with integer
coefficients and a≠0. Let J be the union of at most m disjoint half-open
real intervals, each of length at most ℓ, with m≥1 and ℓ>0. Then

\[
 \#\{t\in\mathbb Z:F(t)\in J\}
 \le 2\sqrt{m\ell/|a|}+2m.                                \tag{10}
\]

**Proof.** Complete the square over the rationals, then translate and reflect
the value axis if a<0. On each side of the vertex the inverse is a translated
square root, scaled by 1/√|a|. After intersecting the value intervals with
the image of that side, their total length is at most mℓ. For disjoint
intervals [a_i,b_i] in [0,∞), the sum of their square-root lengths is at most
the square root of their total length. Indeed, moving each interval left to
remove the preceding gaps increases √b_i−√a_i, since
√(x+s)−√x=s/(√(x+s)+√x) decreases in x; the resulting adjacent intervals
telescope. Thus the real preimage has total length at most
2√(mℓ/|a|) and at most 2m interval components. An interval of length d
contains at most d+1 integers, with any choice of its endpoint inclusions.
Summing proves (10). Degenerate intersections at the vertex are covered by
the same component allowance. ∎

**Lemma 4.2 (two changed ENH words).** For nonzero increments δ,ε, arbitrary
tags and every word e of popcount h,

\[
 \Pr[L=L',\ U\mathbin\oplus U'=e]
 \le {16(2^{h/2}+1)\over q}
 \le {16(2^{\lceil h/2\rceil}+1)\over q}.                   \tag{11}
\]

**Proof.** Specify z=U&e, one of 2^h submasks of e. Equation (3) gives
U'−U=e−2z. On L=L',

\[
 A'B'-AB\equiv q(e-2z-(\tau'-\tau))\pmod {q^2}.             \tag{12}
\]

The left side is strictly between −q² and q², so a fixed residue permits at
most two integer differences. Split each operand into its two wrap branches;
there are at most four branch pairs. In each branch a=A'−A and b=B'−B
are fixed nonzero signed integers. A fixed difference in (12) imposes

\[
 aB+bA+ab=\text{fixed integer}.                             \tag{13}
\]

If there is an integer solution (A₀,B₀), all integer solutions are

\[
 A=A_0+(a/g)t,\quad B=B_0-(b/g)t,\quad t\in\mathbb Z,
 \qquad g=\gcd(|a|,|b|).
\]

This follows from coprimality of a/g and b/g after subtracting two solutions.
Their product is a quadratic in t with leading coefficient −ab/g², a
nonzero integer. The restriction U&e=z permits exactly m=2^(64−h) high
words H, since H↦(H+τ) mod q is a bijection. These specify m disjoint product
intervals [qH,q(H+1)), each of length q. Lemma 4.1 bounds the number of
integer points on the line by 2√(mq)+2m. Restricting operands and wrap
branches can only reduce this count.

Multiply by 2^h patterns, two difference representatives and four branch
pairs, and divide by q². The resulting bound is exactly
16(2^(h/2)+1)/q. This includes zero products and all tag and operand wraps. ∎

Two complementary bounds are useful in the finite sum.

**Lemma 4.3 (sparse mask and dense mask).** Under Lemma 4.2's hypotheses,

\[
 \Pr[L=L',U\mathbin\oplus U'=e]\le K_H(e)/q,
\]

where b₆₃(e) is the top bit and

\[
 K_H(e)=\min\{2^{1+h(e)-b_{63}(e)},\quad
                  276\,2^{64-h(e)},\quad
                  16(2^{\lceil h(e)/2\rceil}+1)\}.           \tag{14}
\]

**Proof.** The last term is Lemma 4.2. For a prescribed difference of
products modulo q², fix A. On each of B's at most two wrap intervals its
slope is the nonzero A'−A. Two distinct values in that interval cannot agree
modulo q², since their ordinary difference has magnitude less than q².
Thus the point probability is at most 2/q. The high XOR pattern e permits
at most 2^(h−b₆₃(e)) differences U'−U modulo q: toggling the top sign changes
the integer difference by q. Equation (12) proves the first term.

For the second, U+U'=e+2z with z=U&~e, giving at most 2^(64−h) possibilities
modulo q. For any one of them, H+H' has at most two possible integer values
in [0,2q). On L=L', AB+A'B'=q(H+H')+2L lies in an interval of length 2q.
Fix A and a wrap branch for B. The sum of products is affine in B with
positive slope s=A+A'>0. An interval of length 2q contains at most 2q/s+1
such integers B. Two B branches and two high sums give at most 4(2q/s+1).

For A,A'>0, 1/(A+A')≤(1/A+1/A')/4. Each of the two zero-factor positions
contributes at most one to the reciprocal sum. The harmonic sum over
1,…,q−1 is at most 64 by its 64 dyadic intervals. Consequently
Σ_A 1/(A+A')≤34. Summing 4(2q/s+1) over A gives at most 276q operand pairs
for each prescribed sum modulo q. Multiplication by 2^(64−h) and division
by q² give the middle term. ∎

## 5. Modular quadratics and low ENH XOR masks

**Lemma 5.1 (bits of a modular quadratic).** Let n≥1 and X be uniform modulo
2^n. Let F(X)=bX²+cX+d modulo 2^n, with b or c odd. For any h selected bit
positions and any specified assignment to them,

\[
 \Pr[\text{the assignment holds}]\le
              \min(1,4\,2^{-\lfloor h/2\rfloor}).           \tag{15}
\]

**Proof.** If c is odd, each solution modulo 2 lifts uniquely from width j
to j+1 for j≥1: changing X by 2^j changes F by 2^j modulo 2^(j+1).
There are at most two initial roots. Every output therefore has at most two
preimages, so a set of 2^(n−h) outputs has probability at most 2^(1−h),
which is no larger than the bound in (15).

Otherwise b is odd and c is even. Invert b modulo 2^n and complete the
square: F(X)=b(X+a)²+d' modulo 2^n. Translation makes X+a uniform.
For v₂(X+a)=t with 2t<n, its odd part squared is uniform on the odd squares
modulo 2^(n−2t). At widths at least three these are exactly 1 modulo 8,
each with four roots. Here is a direct proof of that fact. Width three is
immediate. A root modulo 2^m with m≥3 has two lifts differing by 2^m;
these have equal squares modulo 2^(m+1). Of the four roots modulo 2^m,
pairing a root with its translate by 2^(m−1) changes the next square bit,
because the square difference is 2^m times an odd integer modulo 2^(m+1).
Thus exactly two old roots, hence four lifts, produce each admissible next
bit. This proves the assertion by induction. Widths one and two have fixed
odd squares and need no free-bit assertion.

It follows that, conditional on t, F is uniform on a coset modulo
2^min(n,2t+3). Odd multiplication and addition preserve this uniform coset
distribution. At least max(0,h−2t−3) selected positions are above its fixed
part. Their conditional probability is at most 2^-max(0,h−2t−3).
The valuation t has probability 2^(-t−1); valuations for which 2t≥n,
including X+a=0, may be bounded by probability one for the assignment.

For h=2m, sum the bounds for t<m−1 and bound the tail t≥m−1 by its full
mass. This gives at most
2^(-m+1)−2^(-2m+2)+2^(-m+1)≤4·2^-m.
For h=2m+1 the corresponding sum is at most 3·2^-m.
For h≤3 the stated bound is already at least one. These calculations cover
all widths, since 2t≥n occurs only in the tail. ∎

**Lemma 5.2 (a low XOR target).** Suppose δ,ε are nonzero with minimum
valuation r≥1, R=2^r, and e is a word divisible by R. Then

\[
 \Pr[L\mathbin\oplus L'=e]\le K_{L,r}(e)/q,                \tag{16}
\]

where K_{L,r}(0)=R, and for e≠0 of popcount h,

\[
 K_{L,r}(e)=\min\{R2^{h-b_{63}(e)},\quad
                   65\,2^{64-h},\quad
                   4R2^{\lceil h/2\rceil}\}.               \tag{17}
\]

**Proof.** Choose δ=Ra with a odd and write ε=Rb. Put

\[
 C=aB+bA+Rab\pmod q,\qquad Y=AB\pmod q.
\]

The transformation (A,B)↦(A,C) is a bijection, and

\[
 aY=-bA^2+(C-Rab)A\pmod q.                                \tag{18}
\]

For e≠0 fix t=Y&e. The equation L'=L XOR e imposes
C≡(e−2t)/R modulo q/R. There are R lifts for C modulo q, and 2^h possible
patterns t. Write v=v₂(e). Every lift C has valuation v−r: the integer
e−2t has exactly valuation v. If s is the common valuation of b and
C−Rab, then

\[
 s=\min(v_2(b),v-r)\le v-r.                               \tag{19}
\]

To see the equality, if v₂(b)≤v−r both coefficients have at least that
valuation and b has exactly it; otherwise C−Rab has exactly valuation v−r.
The zero coefficient convention causes no problem since C has finite
valuation below 64−r and b≠0.

Equation (18) expresses Y as an odd multiple of a quadratic divisible by
2^s. Divide by this common factor. Modulo 2^(64−s), at least one of its
quadratic or linear coefficients is odd. Every bit selected by e is at
position at least v≥s+r>s, so all h selected bits survive the division.
Uniform A remains uniform modulo this smaller power of two. Lemma 5.1
bounds the number of A realizing each fixed C,t by
4q·2^-floor(h/2). Summing R lifts and 2^h patterns, then dividing by q²,
proves the third term of (17).

For the first term, the low additive difference is
δB+εA+δε modulo q, uniform on Rℤ/qℤ with point mass R/q after conditioning
A. The XOR mask e permits at most 2^(h−b₆₃(e)) additive targets modulo q.
For e=0 there is one target, giving exactly R/q.

For the middle term, e being even makes L+L' even, and

\[
 (L+L')/2=(A+\delta/2)(B+\epsilon/2)+\delta\epsilon/4
                      \pmod {q/2}.                        \tag{20}
\]

Since L+L'=e+2(Y&~e), its half-sum has at most 2^(64−h) possible values
modulo q/2. The product of two independent uniform words modulo 2^n has
point mass at most (n+2)/2^(n+1). Indeed, for first-factor valuation j<n,
each attainable target contributes 1/2^(n+1) after averaging that factor;
the zero first factor adds 1/2^n only for target zero. The maximum, at zero,
is (n+2)/2^(n+1). With n=63 this is 65/q, proving (17). ∎

## 6. Finite sums for all shufflers

For r=1,2,3 let D_r=D∩2^rℤ. Define, using (7),

\[
 C_{r,k}=2^r\sum_{u,v\in D_r}K_{L,r}\bigl(u+(I+S_k)^{-1}(u+v)\bigr)W_L(v).
                                                                  \tag{21}
\]

For every r≥4 define the same valuation-independent high-lane constant

\[
 C_{H,k}=\sum_{\substack{u,v\in D\\
                 x=(I+S_k)^{-1}(u+v)<2^{63}}}
                    K_H(u+x)W_H(v).                       \tag{22}
\]

All additions inside the arguments of masks in (21)–(22) mean XOR.

**Lemma 6.1.** The joint projected probability is at most C_{r,k}/q² for
r=1,2,3, and at most C_{H,k}/q² for every r≥4.

**Proof.** For r≤3, a collision gives one of the pairs u,v∈D_r in the low
lanes. Formula (7) fixes their separate low differences x,e. PH and ENH
keys are disjoint. Lemma 3.1 bounds the PH target probability by R/q;
Lemma 5.2 bounds the ENH target probability by K_{L,r}(e)/q. After these
original keys are all fixed, the necessary secondary low projection has
conditional probability at most W_L(v), by Lemma 3.2. Multiply this uniform
conditional bound and sum the possible raw targets. This is (21).

For r≥4, (8) holds. A pair of high masks u,v∈D fixes ΔP=(0,x) and
ΔE=(0,e), with x<2^63. The PH full-target bound is 1/q, independently of
the ENH event. Lemma 4.3 bounds the latter by K_H(e)/q. The conditional
secondary high projection costs W_H(v). Summing gives (22). All unused
primary or secondary equalities are simply dropped as necessary conditions;
neither PH offset is replaced by a common one. ∎

The exact sums have the following maxima over all fifteen k:

| Range | Maximizing k | Ceiling of maximum C |
|---|---:|---:|
| r=1 | 2 | 27196168693 |
| r=2 | 2 | 13437880698 |
| r=3 | 2 | 268435521 |
| every r≥4 | 2 | 170906186782 |

Every one of the 60 exact fractions, including its target count and integer
numerator and denominator, is in `checks/open_ph_enh_certificate.json`.
Since 170906186782<2^38, Lemma 6.1 proves (1).

## 7. Completeness and exactness of the certificate

The only enumeration relevant to the 64-bit theorem is over the finite
mask set and the fifteen fixed linear maps, not over a selection of message
families. Here is the entire mathematical algorithm.

To generate D, let R(n,w) be supports of signed binary representations of n
with w digits in {−1,0,1}. At width zero return {0} for n=0 and the empty set
otherwise. At positive width return the empty set if |n|≥2^w. For even n,
return {2a:a∈R(n/2,w−1)}. For odd n, return
{2a+1:a∈R((n−1)/2,w−1)∪R((n+1)/2,w−1)}. Inspection of the low digit proves
this recursion sound and complete by induction. Negation preserves support,
so D=⋃_{j=0}^8 R(jp,64), by (3).

Independently, the program enumerates bits in y=x+jp for j=0,…,8, retaining
states (carry,partial XOR mask). For each state and either next x bit, the
next y bit and carry are determined; after 64 bits accept carry zero. Every
accepted state has a nonoverflowing witness, and every such pair follows
one of these paths. It therefore generates exactly D and must agree with
the signed-digit algorithm. Formula (2) enumerates every pattern for each
mask. Direct enumeration of all congruent word pairs supplies additional
checks at widths 5 through 10 with p=2^(w−3)−1.

For each k, solve x+(x<<1)+(x<<k)=y bit by bit, omitting the last term for
k=1. Direct substitution checks every inverse value. Linearity permits
(I+S)⁻¹(u+v) to be computed by XOR of the two inverse values. Iterate every
pair in D_r² for (21), and every pair in D² satisfying x<2^63 for (22).

The weights in (9) use exact integer denominators:

\[
\begin{split}
 qf_H(v)&=1+\sum_{j=0}^{63}2^{j-h(v\&(2^j-1))},\\
 q^2f_L(v)&=q+\sum_{j=0}^{63}2^{127-j-h(v>>j)}.
\end{split}
\]

Thus (22) has denominator q and (21) denominator q² before division by the
outer q² in the probability. The minimum bounds in (14), (17) are integers;
ceilings are computed by integer division. No floating-point square root
is used: the safe inequality 2^(h/2)≤2^ceil(h/2) is already in the lemma.
The certificate's 60 assertions compare integer cross-products with 2^41
and with the stronger, distinct-key-adjusted 2^38 threshold.

`checks/validate_quadratics.cpp` is an independent literal check of the new
algebra and bounds. It enumerates all nonzero even increment pairs and all
operand pairs at width eight, checks every low XOR target, and checks every
high XOR target on raw low equality for four specified tag profiles. It
also checks the integer-line parameterization and product quadratic on those
events, and the modular quadratic pattern lemma at widths three through
seven. These checks support the general proofs; their scaled results are
not extrapolated into a 64-bit claim.

The recorded run passed 7,916,544 modular quadratic pattern checks,
1,057,030,144 NH operand-pair visits, 1,775,714 low-XOR bound checks,
16,516,096 high-XOR bound checks, and 48,103,424 integer-line identities.
There were zero failures.

`checks/verify_certificate.py` is a separate certificate reader. It proves
completeness of the supplied masks and patterns from sound, disjoint word-pair
witnesses and the independent identity

\[
 \sum_{d\in D}|P(d)|2^{64-h(d)}=8q+72.
\]

The right side counts all congruent ordered word pairs: eight residue classes
have nine representatives and the other p−8 have eight. The reader recomputes
weights with rational numbers and shuffler inverses using
(I+S)⁻¹=(I+S)(I+S²)(I+S⁴)(I+S⁸)(I+S¹⁶)(I+S³²).
It agrees on all sixty sums. An additional 1,248,480 literal interval tests
verify (10) with positive and negative leading coefficients, using squared
integer inequalities rather than floating-point square roots. Outputs and
source hashes are retained in `checks/`.

## 8. Exact obligation and key-model match

`x.Valid`, `y.Valid` and `sameCount` supply equal chunk counts at most sixteen
and therefore a shuffler in (5). `phDiffCount=1` supplies exactly one PH
difference and common contributions everywhere else. `dataChecksum x =
dataChecksum y` supplies equal twisting operands before the independent
twist-key translations, and the equality of PH and ENH coordinate XOR
differences. `enhChanges=2` supplies two nonzero increments, as required by
the nondegenerate quadratic in Lemma 4.2. `enhValuation` agrees with r by
the lowest-differing-bit argument in Section 2.

The exceptional set in `OpenPHENH` is a subset of r=1,…,63, all of which are
covered here. Its `uniformProb` samples all 34 OH words independently and
uniformly; unused key words average out. Its `jointEvent` is exactly the
four lane equalities bounded in Lemma 6.1. The fixed seed produces fixed
tags, a special case of the arbitrary tags allowed above. The strict
inequality demanded by that Lean proposition follows from (1).

For the optional distinct-key transfer, among 34 independent words there
are 561 pairs of positions, each equal with probability 1/q. Acceptance of
all words being distinct therefore has probability at least 1−561/q.
Any event's conditional probability increases by at most its reciprocal.
The certificate checks

\[
 {170906186782\over1-561/q}<2^{38}.
\]

This preserves (1)'s strict 2^-90 conclusion for distinct OH words, without
assuming conditional independence of any key positions.

## 9. Additional primary result: tag-only pairs

**Lemma 9.1 (elementary divisor bound).** Every integer 1≤n<2^128 has fewer
than 2^36 positive divisors.

**Proof.** Write n=∏ℓ^{a_ℓ}. For ℓ≥17, (a+1)^4≤16^a≤ℓ^a, since a+1≤2^a
for a≥1. For the six smaller primes 2,3,5,7,11,13, each exponent is at most
127. The exact maxima of (a+1)^4/ℓ^a for 0≤a≤127 occur respectively at
a=5,3,2,1,1,1 and are

\[
 81/2,\quad256/27,\quad81/25,\quad16/7,\quad16/11,\quad16/13.
\]

Their product is 127401984/25025<16^4, checked by integer cross-products.
Consequently d(n)^4≤(127401984/25025)n<16^4 n, and
d(n)<16n^(1/4)<2^36. The finite check enumerates all 128 exponents for each
of the six primes, so no unproved maximization is used. ∎

**Theorem 9.2.** If the two expanded block chunk lists agree and their valid
tags differ, their primary projected collision probability is less than
1/(8q), under IID keys, with any fixed common PH offset.

**Proof.** The tags are τ=seed XOR b and τ'=seed XOR b', with b,b' bytes.
Their nonzero signed difference d has |d|≤255, and they share all upper
56 bits. Both products and low lanes agree. The high XOR mask is
m=(H+τ mod q) XOR (H+τ' mod q). A collision needs a nonzero m∈D.

Such m has highest bit k≥60. Moving by signed d on the word circle must
cross a boundary at a multiple of 2^60, so H has at most 16|d|≤4080 possible
values. This counts only the fixed direction of d, not both directions.

Put g=2^(k+1)−m. Any integer difference between words with XOR mask m has
absolute value at least g, since the highest changed bit cannot be canceled
by the lower changed bits. Its circular distance modulo q is also at least
g: its magnitude is at most m, and q−m≥g. Therefore g≤|d|≤255. All bits
8,…,k of m are set, and at least one of its lower eight bits is set, since
g is nonzero and below 256. Thus h(m)≥(k−7)+1≥54.

For each fixed H and its m, the primary high word is M XOR L XOR U for a
fixed common M and fixed U. Formula (3) permits at most |P(m)|≤8 patterns
of L on m. There are consequently at most 8·2^(64−54)=8192 possible low
words L, hence that many product targets N=qH+L. Across all H there are at
most 4080·8192 product targets.

Target N=0 cannot be a collision: then H=L=0 and m=τ XOR τ'≤255, whereas
every nonzero member of D has highest bit at least 60. For each remaining
N, the number of ordered pairs A,B of words with AB=N is at most d(N),
because each possible first factor is a positive divisor and determines
the second factor. Lemma 9.1 bounds this by 2^36. Thus the total number of
colliding pairs is strictly less than

\[
 4080\cdot8192\cdot2^{36}=255\cdot2^{53}<q/8.
\]

Divide by q². The bound is uniform over every fixed common PH offset, so it
also holds after averaging the independent unchanged PH keys. ∎

This theorem addresses only the tag-only primary case. The small constant
for general even PH changes, the remaining ENH primary cases, and the
all-message `PrimaryIdentityBound` assembly are not supplied here.

## 10. Further reductions of the remaining primary problem

These are sufficient conditions and partial cases, not an assertion of the
all-message `SharpPrimaryProjection` proposition.

**Lemma 10.1 (PH difference rank).** Suppose at least one PH chunk changes,
and the F₂-linear map from all PH key bits to the primary PH XOR difference
has rank at least 76. Then the primary projected probability is at most
604²/2^76<162/q, regardless of the other data or tags.

**Proof.** If a PH coordinate difference is odd, the established 17/q bound
in `materials/PROOF.md`, Lemma 5.2, already suffices. Otherwise all these
differences are even. Condition all ENH keys. The raw primary XOR difference
is uniform on a translate of the rank-dimensional PH image. Its bit 0 is
fixed, since all multiplying coefficients are even, and its bit 127 is fixed,
since every PH product has degree at most 126. There are at most 604 masks
of D with any prescribed bit 0, and at most 604 with any prescribed bit 63,
as verified by the mask certificate. Thus at most 604² raw targets in that
translate can satisfy projected equality. Each has probability at most
2^-76. The certificate verifies 604²<162·2^12. ∎

For a single PH chunk with both coordinate differences a,b nonzero, this
rank is exactly 64+t, where g=gcd(a,b) in F₂[X] and
t=max(deg(a/g),deg(b/g)). Indeed the kernel of aV+bU consists exactly of
U=(a/g)T, V=(b/g)T: coprimality gives these divisibilities, and the converse
is immediate. The word-degree restrictions are precisely deg T<64−t.
The kernel therefore has dimension 64−t and the map rank 64+t. In particular,
t≥12 suffices for Lemma 10.1. This does not settle the cases t≤11 or a
zero coordinate difference, nor does it assert their extremality.

**Lemma 10.2 (ENH-only primary, small valuation).** Suppose all PH data agree
and both final ENH words change with minimum valuation r≤3. If there is a
PH chunk, the primary projected probability has respective upper bounds

\[
 (14,144,56,9)/q\quad\text{for }r=0,1,2,3.                  \tag{23}
\]

If there is no PH chunk, the respective bounds are (17,18,20,24)/q. For
4≤r≤7, with any number of common PH chunks, the bound is 2^r/q≤128/q.

**Proof.** With no PH chunk, low projection means L'−L=jp, |j|≤8.
The low products agree modulo R, so j must be divisible by R. The low
additive difference is uniform on Rℤ/qℤ. A union over
2 floor(8/R)+1 signed targets gives the stated four constants.

With a PH chunk, its common low word supplies the independent product mask
in Lemma 3.2; other common PH keys may be conditioned into a fixed offset.
For each possible low XOR mask d∈D∩Rℤ, the probability of a primary low
projection with this mask is at most K_{L,r}(d)W_L(d)/q when r≥1.
When r=0, the low ENH XOR difference is uniform, giving W_L(d)/q instead.
To prove that uniformity, fix A with δ odd. At bit j of B, the two low
products have opposite coefficients on the new B bit, since A and A' have
opposite parity; addition of ε into B contributes only already determined
lower-bit carries. Their XOR's bit j thus determines B_j uniquely after the
preceding bits. This is a triangular bijection.

A second bound for this same mask event is R|P(d)|/q. To see it, condition
the common PH low word M. For each t∈P(d), the low ENH word satisfies
L&d=(M&d) XOR t. Consequently L'−L=d−2((M&d) XOR t) is a fixed additive
target modulo q, of probability at most R/q. Union over t, then average M.
Taking the minimum of the two valid bounds for each mask and summing gives

\[
 \frac1q\sum_{d\in D\cap R\mathbb Z}
       \min(R|P(d)|,K_{L,r}(d)W_L(d)),                       \tag{24}
\]

with K_{L,0}(d)=1. The certificate's exact ceilings are (23). This takes a
minimum of two separately justified counts; it does not multiply bounds
derived under incompatible conditionings.

For 4≤r≤7, common-mask low projected equality forces L=L' by D∩16ℤ={0}.
The low additive difference is uniform on Rℤ/qℤ, so the event has probability
exactly R/q before the high condition is imposed. ∎

The primary ENH-only two-word cases left by these reductions have r≥8.
General primary pairs with PH differences of low rank, and the short/short
constant-polynomial identity issue in the exact Lean definition, still need
a proof. None is replaced by the joint bound proved in Sections 2–8.
