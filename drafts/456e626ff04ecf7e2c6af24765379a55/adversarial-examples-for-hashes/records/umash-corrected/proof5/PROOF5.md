# The published UMASH fingerprint bound, with independent polynomial multipliers

## 1. Result and scope

Let q=2^64, p=2^61-1, and let L be a positive integer. Fix a seed and two
distinct byte strings x,y of lengths at most 8L, independently of the key.
Use the two-compressor construction implemented by the supplied C library:
the compressors share 34 OH words, but have **independent polynomial
multipliers** f_0,f_1. Sample the OH words uniformly without replacement and
the multipliers independently and uniformly from {2,...,p-1}, independently
of OH. Then

\[
 \boxed{\Pr[\operatorname{fp}(x)=\operatorname{fp}(y)]
  < {81\over128}\left\lceil{L\over2^{23}}\right\rceil^2 2^{-83}
  < \left\lceil{L\over2^{23}}\right\rceil^2 2^{-83}.}       \tag{1}
\]

The first strict inequality also holds for IID OH words, and for multipliers
uniform on {1,...,p-1}, the nonzero range accepted by the C initializer.
The theorem concerns this ideal full-key distribution. It is not a theorem
about a short Salsa20 seed, or a distributional audit of `umash_params_prepare`.
The mathematical statement has no upper length cap; actual C inputs are
additionally subject to the implementation's `size_t` domain.

This proves the **published fingerprint headline**. The paper's p. 12 says
"combined collision probability" and gives
epsilon_fp < ceil(s/2^26)^2 2^-83, for an s-byte cap. Substitution s=8L gives
the second bound in (1). See the supplied
[paper](materials/umash-lemma/umash.pdf), p. 12, and
[literate source](materials/umash-lemma/umash_reference.py), lines 744--774.
It is the numerical headline that is proved here; the paper's original
entropy-loss inference is not used.

The new argument assembles the previously verified probability lemmas and
proves the secondary marginal needed for the independent root counts. It
does **not** require `SharpPrimaryProjection` or any result in `PROOF4.md`.
The imported theorems, with their full relevant hypotheses, are listed below;
their proofs are not repeated, as requested. In particular no open proposition
in `UMASHObligations.lean` is assumed.

This is a natural-language proof with exact computational certificates,
not a completed Lean formalization or a new external adversarial review.

## 2. The sharper envelope and its scores

Define

\[
 m=p-2=2^{61}-3,\quad C_0=3125,\quad C_1=2\cdot604^2=729632,
 \quad J=345763417\cdot4096=1416246956032,
\]
\[
 a={C_0+C_1\over q-561}={732757\over q-561},\qquad
 b={J\over q(q-561)},\qquad
 \rho(L)=\min\left(1,{2\lceil L/32\rceil\over m}\right).
\]

The more informative proved fingerprint bound is

\[
 E(1)={1\over q(q-561)},\qquad
 E(L)=b(1-\rho(L))^2+a\rho(L)(1-\rho(L))+\rho(L)^2
       \quad(L\ge2).                                    \tag{2}
\]

In particular 0<E(L)<=1. These are bounds on the actual collision event,
not just on equality after reducing the accumulator modulo p.

The score of (2), defined as min_L log2(L/E(L)), is:

| Length domain, in 64-bit words | Minimizing L | Exact expression | Decimal display |
|---|---:|---|---:|
| 1<=L<=2^23 (through 64 MiB) | 2 | log2(2/E(2)) | 88.634778062847499... |
| 1<=L<=2^61-1 (the post's full-word byte domain) | 2^61-31 | log2((2^61-31)/E(2^61-31)) | 68.999999999999140... |
| All positive integer L | 2^65-95 | log2((2^65-95)/E(2^65-95)) | 64.9999999999999999975364... |

Including L=2^61 to cover the final seven byte lengths below 2^64 does not
change the second minimum: it is in the same block-count plateau.
The optional scoring floor max(2^-128,E(L)) has no effect on these results.
The decimal displays are not numerical premises: Section 9 proves the
minimizers, and the certificate checks the intervals (88.63,88.64),
(68.99,69), and (64.99,65) by integer powers.

For comparison, the *published* quadratic formula itself scores exactly
83 on the first domain, but only

\[
 7+\log_2(2^{61}-2^{23}+1)=67.9999999999947515\ldots          \tag{3}
\]

on the second. Thus proving the published bound does not give an
unqualified 83-bit length-normalized score on every length domain.

The inherited linear bound from the supplied `PROOF4.md` is
58 ceil(L/512)/2^61, clipped at one. Its more precise envelope is recorded
in Section 10. The fingerprint bound (2) is smaller at every L before
rho reaches one, and both bounds equal one thereafter. There is no length
at which the inherited linear envelope improves (2).

## 3. Model and established inputs

Write a 128-bit compressor output as two 64-bit lanes (low,high). Projection
pi reduces each lane modulo p. Let odot be unreduced multiplication of word
polynomials in F_2[X]. A nonfinal PH chunk (u,v), with keys (k,l), produces

\[
 (u\mathbin\oplus k)\odot(v\mathbin\oplus l).
\]

For the final chunk, put A=(u+k) mod q, B=(v+l) mod q, AB=qH+Y, and

\[
 E_\tau=(Y,((H+\tau)\bmod q)\mathbin\oplus Y),\qquad
 \tau=\operatorname{seed}\mathbin\oplus(\operatorname{blockBytes}\bmod256).
                                                               \tag{4}
\]

Both tags have the same upper 56 bits, so their signed difference is at
most 255 in absolute value. This includes the low-to-high XOR fold.

For n chunks, 1<=n<=16, primary OH is the XOR of the n-1 PH values and (4).
The secondary is the XOR of (4), the shuffled PH values, and a checksum PH.
Let T shift left by one within each lane, truncating overflow. The PH
shufflers are S=T or S=T+T^k, 2<=k<=15, where + means XOR. Specifically
at position i<n-1 the map is T+T^(n-i), and at i=n-1 it is T.

The keyed checksum is the XOR of (data chunk XOR its original key pair),
including the final chunk *before* its additive treatment. Two fresh
twisting words, positions 32 and 33, are XORed into it before its PH.
For equal chunk counts the difference of keyed checksums equals the
difference of the data checksums, regardless of original keys.

All probability lemmas in Sections 3--6 first use 34 IID uniform OH words.
Original chunk keys occupy positions 0,...,31. Twisting keys are independent
of them. Arbitrary other fixed chunks and the permitted tags may differ
unless a lemma explicitly requires agreement.

The primality of p is an established input from PROOF.md's exact primality
certificate (also part of the supplied Lean field model). Thus the ordinary
root theorem over F_p used below has its field hypothesis satisfied.

The following established results are inputs to this round.

1. **Masks and affine PH slices.** For
   D={u XOR v: 0<=u,v<q, u=v mod p}, |D|=852. For either fixed value of
   bit 0, at most 604 masks are possible; the same is true of bit 63.
   If a PH input pair differs, its raw XOR difference has point mass at
   most 1/q, including after conditioning on one companion key word.
   With arbitrary fixed, possibly different, 128-bit XOR offsets, its
   projected collision probability is at most C/q, C=604^2=364816.
   When some coordinate difference is odd use the stronger 17/q slice
   bound; otherwise bit 0 and bit 127 of the full raw difference are fixed,
   and the injective slice has at most 604^2 permissible targets.
   These are [PROOF.md](materials/PROOF.md), Sections 3 and 5. The offset
   form is explicit in those proofs; no common-offset assumption is added.

2. **Primary block and identity bounds.** For any distinct valid expanded
   block/tag tuples, primary projected collision is at most 3125/q. If
   chunk counts agree and some PH chunk differs, 1123/q suffices. If only
   the tag differs, the bound is <1/(8q). For any distinct messages with
   at least one message longer than eight bytes, primary comparison-
   polynomial identity has probability <=3125/q.
   Sources: [PROOF3.md](materials/PROOF3.md), Theorem 6.1 and Corollary 6.2;
   the tag bound is [PROOF2.md](materials/PROOF2.md), Theorem 9.2.

3. **Joint ENH-only.** For equal chunk counts, all PH chunks equal, and
   one or both final data words changed, the two projected compressors
   collide with probability <=4721784/q^2, with every valuation 0,...,63.
   Tags may differ. This includes zero PH chunks. Source:
   [PROOF.md](materials/PROOF.md), Theorem 7.2.

4. **Joint equal-checksum PH cases.** Equal chunk counts and equal data
   checksums give the following bounds, allowing other changes and tags
   as specified by the case:
   - At least two PH chunks differ: <=345763417/2^116=J/q^2, with the ENH
     chunk arbitrary. Source: [subcase-b verdict](materials/umash-subcase-b/VERDICT.md),
     Sections 1--4. The attained shuffler index in its bound table is h=15;
     its actual possible range is 2,...,15.
   - Exactly one PH chunk differs, and one final word changes: <2^37/q^2.
     Source: [ENH verdict](materials/umash-enh-verify/VERDICT.md), Sections
     5 and 7. For r<=3 the numerator is (N_r 2^r)^2, N_r=(852,248,64,2);
     for r>=4 it is <=111924178297<2^37.
   - Exactly one PH chunk differs and both final words change: for r=0
     the numerator is at most 852^2; for 1<=r<=63 it is at most
     170906186782. Here r is the minimum valuation of the two nonzero
     additive final-word increments. Sources: the same ENH verdict,
     Section 7, and [PROOF2.md](materials/PROOF2.md), Theorem 1.

5. **Tags and point masses.** If expanded chunks agree but valid tags
   differ, joint probability is <11946240/2^116. Primary or secondary
   projection in this case requires H=floor(AB/q) to belong to at most
   4080 fixed values. Each such H has probability <48/q. The latter
   necessary event is independent of the choice of common XOR offset.
   Source: [ENH verdict](materials/umash-enh-verify/VERDICT.md), Section 6.
   Also, a fresh ENH product, after a fixed XOR offset, has any prescribed
   projected point with probability <=(82q-81)/q^2<82/q; see
   [PROOF.md](materials/PROOF.md), Lemma 6.3.

6. **Encoding and finalization.** Expanded block/tag lists encode long
   byte strings injectively. A string of length <=8L has at most ceil(L/32)
   blocks. The long finalizer is a bijection on words. Modulo p, the
   implemented double-pumped accumulator evaluates
   sum_(j=1)^(2n) z_j f^(2n+1-j), where z alternates low/high lanes.
   Short hashes of a fixed string are uniform words under IID keys, and
   their two fingerprint components are independent for that fixed string.
   Distinct equal-length short strings never collide; unequal short lengths
   have joint probability 1/q^2 in the IID model. See
   [PROOF.md](materials/PROOF.md), Section 8, and
   [GAP.md](materials/umash-lemma/GAP.md), Section 3.

These inputs are probability theorems with the above hypotheses, not numerical
conjectures inferred from small-word experiments. The new certificate also
recounts the masks and the subcase-b table and checks the imported ceilings.

## 4. A complete secondary marginal bound

**Lemma 4.1 (changed checksum).** Condition on all original chunk keys. If
the two keyed checksums differ, secondary projected collision has conditional
probability at most C/q over the twisting keys, C=604^2.

**Proof.** All shuffled PH and ENH outputs are now fixed. What remains is
PH of two different fixed checksum inputs using the independent twisting
pair, with the two fixed offsets supplied by those outputs. Input 1 of
Section 3 applies, including when the offsets differ. ∎

**Lemma 4.2 (equal checksums, a PH change).** Suppose chunk counts and data
checksums agree, and at least one PH chunk differs. Then the secondary
projected collision probability is at most 2C/q.

**Proof.** In raw XOR differences the two checksum products cancel, even
though their common value can depend on other keys. Select a changed PH
chunk and a nonzero coordinate difference d. Condition on all original
keys except the opposite key word V of this chunk. Its PH difference is
an injective affine map d odot V XOR c. Every possible value has bit 127
zero because carryless word products have degree at most 126.

Let W be the 127-dimensional binary space with bit 127 zero. On the full
128-bit space, ker T is spanned by the two lane top bits, positions 63 and
127. For S=T+T^k, write S=(I+T^(k-1))T. The first factor is invertible:
its inverse is the finite geometric sum in the nilpotent T^(k-1).
Consequently ker S=ker T. This also holds for S=T. Thus ker S intersect W
has exactly two elements, 0 and 2^63.

For any specified secondary raw XOR difference, at most two values in the
injective PH slice can produce it after shuffling. The contribution of all
other original chunks is fixed. Bit zero in each lane of S is zero, so
bit zero of each lane of the full secondary difference is fixed by that
contribution. There are at most 604 admissible masks in each lane, hence
at most C raw targets that can meet projection. Each has at most two V
preimages. The necessary raw-mask event has probability at most 2C/q.
It bounds projected equality regardless of the common checksum value.
Averaging the conditioned keys proves the result. ∎

**Lemma 4.3 (other block cases).** For different chunk counts, the secondary
marginal is at most q^-2+C/q. For equal counts and PH agreement with changed
ENH data it is at most C/q. For tag-only changes it is <195840/q.

**Proof.** In the unequal-count case the keyed checksum difference contains
a fresh pair of original key words used only by the longer block's extra
chunks. Conditional on all other original keys it is uniform on the pair
of words. Its probability of being zero is exactly q^-2. On its complement
apply Lemma 4.1 after exposing all original keys. We bound the exceptional
event by its whole probability; no independence with primary collision is
assumed.

With equal counts and all PH data equal, a different final pair changes
the data checksum, so Lemma 4.1 applies. If final data also agree, only the
tag can distinguish valid tuples. Both secondary outputs then have a
common offset, and their raw high XOR difference is
(H+tau mod q) XOR (H+tau' mod q). Input 5's necessary boundary event has
probability <4080*48/q=195840/q. Conditioning on a checksum value is not
needed for this implication. ∎

These cases are exhaustive for distinct block/tag tuples. Since C+1/q<2C
and 195840<2C, every such secondary block marginal is at most C_1/q.

## 5. Exhaustive joint block ledger

In the table, primary and secondary entries are numerators over q, and
joint entries are numerators over q^2. Strict imported bounds may be safely
weakened to non-strict bounds with the displayed numerator.

| Disjoint configuration of distinct block/tag tuples | Primary | Secondary | Joint |
|---|---:|---:|---:|
| Different chunk counts | 82 | C+1/q | 1+82C |
| Same count, different data checksums, a PH chunk differs | 1123 | C | 1123C |
| Same count/checksum, at least two PH chunks differ | 1123 | 2C | J |
| Same count/checksum, exactly one PH chunk and one ENH word differ | 1123 | 2C | 2^37 |
| Same count/checksum, exactly one PH chunk and two ENH words differ, r=0 | 1123 | 2C | 852^2 |
| Same count/checksum, exactly one PH chunk and two ENH words differ, 1<=r<=63 | 1123 | 2C | 170906186782 |
| Same count, PH chunks all agree, final data differ | 3125 | C | 4721784 |
| Same expanded data, only the tag differs | 1/8 | 195840 | 11946240*4096 |

**Lemma 5.1.** Every row is valid, every distinct pair is in one row, and
the column maxima are bounded by C_0, C_1, and J respectively.

**Proof.** The primary column is covered by Inputs 2 and 5. The secondary
column is Section 4. For unequal counts, outside checksum equality, expose
all original keys and multiply the primary event indicator by the uniform
conditional bound C/q in Lemma 4.1. The primary probability is <82/q because
the longer block's final ENH key pair is unused by the shorter block.
Checksum equality costs at most q^-2 separately. This proves the joint
bound (1+82C)/q^2. The same conditional argument gives 1123C/q^2 in the
different-checksum PH row. All remaining joint entries are Inputs 3--5.

For exhaustiveness first split by chunk count. At equal counts with any
PH change, split by checksum equality. If checksums agree and exactly one
PH chunk differs, the ENH pair must differ: otherwise the sole nonzero PH
coordinate XOR difference could not cancel in the data checksum. The ENH
change is in one or two coordinates; in the latter case its minimum
valuation lies in {0,...,63}, split as in the table. With no PH changes,
either final data differ, or all data agree and the differing tuple has
different tags. These alternatives are mutually exclusive.

The exact column comparisons are in `fingerprint_certificate.json`. In
particular J=1416246956032 exceeds every other joint numerator. Taking a
maximum over this decision list, rather than a sum of different message
cases, gives the claimed constants. ∎

## 6. From blocks to formal-polynomial identities

For a long message with n blocks let P_i(X) be the degree-at-most-2n
polynomial over F_p whose coefficients are the successive lanes of
compressor i, on powers 2n,...,1. There is no constant term. For a short
message, define P_i to be the constant obtained by undoing the long
finalizer on its i-th short output and reducing the resulting word modulo p.
These constants depend only on OH keys and the fixed input and seed.

For a pair with at least one long input, let Z_i mean that P_i(x)-P_i(y)
is the zero polynomial. The polynomial is formed **after** reducing its
coefficients modulo p. We prove under IID OH keys that

\[
 \Pr(Z_0)\le C_0/q,\qquad \Pr(Z_1)\le C_1/q,
 \qquad \Pr(Z_0\cap Z_1)\le J/q^2.                       \tag{5}
\]

**Equal numbers of long-input blocks.** Encoding injectivity supplies a
differing aligned block/tag tuple, chosen from the messages before drawing
keys. Polynomial identity forces its two coefficients to agree in the
relevant compressor. Joint identity forces both projected compressor
equalities. Apply the block ledger. OH keys may be reused across every
other block; no independence of blocks is asserted or needed.

**Different numbers of long-input blocks.** The first block of the longer
input occupies two degrees greater than the degree of the shorter input.
Identity forces both coefficients to zero. A prescribed primary projected
point costs <82/q by Input 5. Conditional on all original keys, the first
block's secondary checksum product is a fresh carryless product: uniform
twisting words XORed with any fixed checksum are independent uniform words.

Here is its required point count. A word residue has at most nine raw
representatives. A carryless product has bit 127 zero; after a fixed XOR
offset, the high word has one fixed top bit and hence at most five
representatives of a prescribed residue. There are at most 9*5=45 raw
targets. The zero product has 2q-1 preimages. Every nonzero full product has
at most q-1 preimages, since fixing a nonzero first factor determines at
most one second factor in the integral domain F_2[X]. Among the 45 targets
at most one is zero, so their total probability is at most
(46q-45)/q^2<46/q. This is uniform after conditioning original keys.
Thus the secondary marginal is <46/q, and the joint bound is
<82*46/q^2. Both are smaller than (5). Vanishing leading coefficients have
been handled as events, not excluded by an assumption.

**One short input and one long input.** Identity forces the short constant
term to be zero, because the long polynomial has zero constant term.
For a fixed short input, each short result is uniform on words; the
inverse long finalizer preserves this. Exactly nine words reduce to zero
modulo p. The primary and secondary short results use noise indices
length and length+4, which are different, so for that fixed input the
two results are independent uniform words. Consequently the two marginal
identity probabilities are at most 9/q, and joint identity at most 81/q^2.
The long computation may use the same OH words. The implication to the
two short constants suffices; no independence from the long computation
is used.

These three message configurations exhaust pairs with a long member and
prove (5). They also explain why equal-count block collision bounds alone
would not suffice for the whole-hash theorem.

For sampling without replacement, let A_dist be the event that the 34
IID OH words are all different. There are binom(34,2)=561 pairs of positions,
each equal with probability 1/q; the union bound gives
Pr(A_dist)>=1-561/q>0. For any OH event V,

\[
 \Pr(V\mid A_{dist})\le {\Pr(V)\over1-561/q}.              \tag{6}
\]

Thus after conditioning, the marginal sum in (5) is at most a, and the
joint probability at most b. The independent polynomial multipliers
remain independent of each other and of the conditioned OH keys. The
factor in (6) is applied **once** to a joint event, not once per compressor.

## 7. Independent multipliers and the full fingerprint

**Lemma 7.1 (composition).** For two comparison polynomials of degree at
most d, depending on OH keys but not on the two independent multipliers,
put r=min(1,d/(p-2)). If the identity marginals have sum at most a and their
joint probability is at most b, fingerprint collision has probability at
most b(1-r)^2+ar(1-r)+r^2.

**Proof.** Condition on the complete OH key. For a nonzero polynomial,
the field root theorem gives at most its degree many roots: division by
X-alpha at each distinct root and induction on the degree proves this
statement. Sampling over p-2 possible multipliers therefore costs at
most r. For two nonzero polynomials the multipliers are independent
after conditioning, so their simultaneous root probability is at most r^2.
Actual fingerprint equality implies both field equations, since equality
of the finalized accumulators implies equality modulo p.

Write z_i for the indicator that the i-th comparison polynomial is zero.
The conditional bound on collision is

\[
 [r+(1-r)z_0][r+(1-r)z_1]
 =r^2+r(1-r)(z_0+z_1)+(1-r)^2z_0z_1.
\]

This treats all four identity/nonidentity possibilities. Its coefficients
are nonnegative. Averaging and using the two bounds on the OH events
proves the lemma. No independence between compressors or identity events
is used. A zero field polynomial need not make the modulo-8p outputs
collide; assigning conditional upper bound one is nevertheless valid. ∎

For any pair with a long input, d=2 ceil(L/32) suffices by encoding. Apply
Lemma 7.1 with (6) to obtain (2) for these pairs. With multiplier set
{1,...,p-1}, the root denominator is p-1; using the smaller p-2 remains
conservative, so the same proof works.

For completeness, if both strings are short, use their actual collision
event. Equal lengths give no collisions. Different lengths have two
noise constraints on the pairs of indices {s,t} and {s+4,t+4}. These are
distinct edges forming a forest, even if they share a vertex. Transforming
each independent key by addition of the fixed seed preserves uniformity.
Exposing the forest from its roots gives exactly two 1/q factors in the
IID model. Hence joint collision is at most 1/[q(q-561)] after (6).
This is E(1); it is smaller than E(L) for L>=2. For example E(L), as a
quadratic in r, has constant term b and positive linear and quadratic
coefficients; and b>1/[q(q-561)]. This completes all input configurations.

Finally, a<1 and b<1 give 1-a+b>0. As a function of r in [0,1], (2) is
convex, with endpoint values b and 1. It is therefore at most one and
strictly less than one when r<1. The score computations need no additional
probability clipping for (2).

## 8. Exact rounding to the published headline

Let H=ceil(L/2^23)>=1 and r_0=2^19/m. Since L<=2^23 H,
ceil(L/32)<=2^18 H and rho(L)<=r_0 H. From (2), for L>=2,

\[
 E(L)\le b+a\rho(L)+\rho(L)^2
       \le b+a r_0 H+r_0^2H^2
       \le H^2(b+a r_0+r_0^2).                            \tag{7}
\]

The only final numerical inequality is

\[
 K:=2^{83}(b+a r_0+r_0^2)
 ={61555182062916914709644342474504392009252905535823413248
   \over
   98079714615416883696934812005564729285413570653510954055}
 <{81\over128}<1.                                        \tag{8}
\]

It is checked by multiplying the numerator by 128 and denominator by 81.
Its decimal display is 0.6276036008493975... . No floating computation is
used to decide (8). The short bound E(1) is also less than K*2^-83 by an
exact cross-product. Equations (7)--(8) prove (1) for every L.

In particular this proof does not substitute a joint probability for a
primary marginal, square the probability of one compressor, count modulo-p
fibre sizes as an entropy loss, or multiply probabilities under incompatible
conditionings. The single and joint identity quantities used in composition
are separately bounded.

## 9. Exact score minimization, including the length domain

This section proves the minima in Section 2 over every integer length in
the specified domains; finite boundary sampling is not used in place of
that argument. Write t=ceil(L/32). Below saturation define

\[
 F(t)=w+vt+ut^2,\quad w=b,\quad v={2(a-2b)\over m},\quad
 u={4(1-a+b)\over m^2}.                                  \tag{9}
\]

The certificate verifies u,v,w>0 by exact rational comparisons. For t=1,
the long-message branch has its largest E(L)/L at L=2. At t>=2 a block-count
plateau starts at L=32t-31, and E(L) is constant there. Thus its largest
E(L)/L occurs at that first length. For real t>31/32, polynomial division
gives

\[
 {F(t)\over32t-31}
 ={u\over32}t+{v\over32}+{31u\over1024}
   +{R\over32t-31},\qquad
 R=w+{31v\over32}+{961u\over1024}>0.                      \tag{10}
\]

The second derivative is 2*32^2*R/(32t-31)^3>0. A convex function on a
closed interval is at most the larger endpoint value, by its defining
secant inequality. Therefore, over all unsaturated t in {2,...,N}, only
t=2 and t=N need comparison. The exact checks are

\[
 {E(33)\over33}<{E(2)\over2},\quad
 {E(2^{23}-31)\over2^{23}-31}<{E(2)\over2}
 <{E(2^{61}-31)\over2^{61}-31}.                           \tag{11}
\]

L=1 contributes E(1), also smaller than the relevant maxima. For the cap
2^23, N=2^18; for the cap 2^61-1, N=2^56. This proves the first two
minimizing lengths in Section 2, since minimizing log2(L/E(L)) is equivalent
to maximizing E(L)/L.

For the unrestricted integer domain, m is odd. The last unsaturated
plateau has t=(m-1)/2=2^60-2 and starts at L_u=2^65-95. The first saturated
plateau starts at L_s=2^65-63. At L>=L_s, E(L)=1 and E(L)/L decreases. The
remaining exact comparisons are

\[
 {E(L_u)\over L_u}>{1\over L_s}>{E(2)\over2},\qquad
 E(L_u)=b/m^2+a(1-1/m)/m+(1-1/m)^2.                     \tag{12}
\]

This proves the unrestricted minimizing length. The first inequality is
strict in the displayed direction; replacing the exact calculation with
a rounded asymptotic would incorrectly choose L_s. The difference is very
small, but is retained in the certificate.

All displayed score intervals are certified without logarithm rounding.
For each exact ratio R_score=L/E(L), an interval (k/100,(k+1)/100) is
proved by the integer inequalities

\[
 (\operatorname{num}R_{score})^{100}
   >2^k(\operatorname{den}R_{score})^{100},\qquad
 (\operatorname{num}R_{score})^{100}
   <2^{k+1}(\operatorname{den}R_{score})^{100}.              \tag{13}
\]

For (2), k=8863,6899,6499 on the three domains. Decimal logarithms are
computed only for display after these assertions pass. E(1)>2^-128 and
E(L)>=b>2^-128 for L>=2, so adding the 128-bit output floor changes nothing.

For the published coarse formula, put H=ceil(L/2^23). Its plateaus start
at L=(H-1)2^23+1, including H=1,L=1. The ratio
H^2/((H-1)2^23+1) is convex as a real function of H>=1, by the same
positive-remainder division as (10). Its maximum on H=1,...,2^38 is at
an endpoint, and the latter endpoint is larger. This proves (3). On the
domain through 2^23 words H=1, so its score is 83 at L=1. Neither domain
reaches probability one, and the 128-bit floor does not change these scores.

The coarse strengthened bound (81/128)H^2 2^-83 has exactly log2(128/81)
more score than the published formula on these two domains. Its scores
are in (83.66,83.67) and (68.66,68.67), respectively. These bounds are
weaker than the corresponding scores of (2), because the rounding in (7)
keeps a constant error throughout a whole 64 MiB plateau.

## 10. The inherited linear bounds and pointwise comparison

Fingerprint equality implies primary equality for every fixed key, seed,
and pair of inputs. Consequently every valid primary upper bound is a
fingerprint upper bound too. In particular the verified `PROOF3.md` gives
423 ceil(L/512)/2^61, and the supplied `PROOF4.md` gives

\[
 E_{lin}(L)=\min\left(1,{58\lceil L/512\rceil\over2^{61}}\right). \tag{14}
\]

The latter proof's precise inherited envelope is

\[
 I(1)={1\over q-561},\quad A_4={205\over q-561},\quad
 S_4={435\over q-561}+{2\over m},
\]
\[
 I(L)=\min(1,\max\{A_4+(1-A_4)\rho(L),S_4\})\quad(L\ge2).
                                                               \tag{15}
\]

This optional comparison uses `PROOF4.md` as supplied (its external
verification was in progress at the start of this task). No conclusion
about (1) or (2) depends on it.
The direct PROOF4 inheritance uses its multiplier range {2,...,p-1}.
The fingerprint theorem (1)--(2) also permits {1,...,p-1}, as already proved.

The exact score of (14) is 61-log2(58), in (55.14,55.15), at L=1, on
either of the finite domains of Section 2 and on the unrestricted domain.
Indeed ceil(L/512)<=L for every positive integer L, with equality at one;
clipping at one cannot increase error per word. The score of (15) is the
supplied PROOF4 value log2(2/S_4)=56.183016376744619... at L=2, in
(56.18,56.19). Both ratios and both intervals are checked anew here.

The precise fingerprint envelope (2) **always dominates** these inherited
bounds. For L>=2, let r=rho(L) and U=A_4+(1-A_4)r. Direct subtraction gives

\[
 U-E(L)=(1-r)\{r(1-a)+A_4-b(1-r)\}.                       \tag{16}
\]

Here a<1 and A_4>b by exact comparisons. Thus (16) is positive when r<1,
and zero when r=1. Since U<=I(L)<=E_lin(L), and E(1)<I(1), the claimed
pointwise ordering follows. All quantities in (15) are below one before
saturation; alternatively the clipping cannot reverse E<=I since E<=1.

Therefore min(E,I,E_lin)=E at every length. Before L=2^65-63 the inequality
E<I is strict. From that length onward E=I=E_lin=1. In particular (2) is
the useful bound at **every** length in the C byte-length domain; there
is no switch to the linear inherited form there. The ordering in this
paragraph concerns the precise expression (2), rather than estimating a
crossing from asymptotic powers while discarding constants and clipping.

## 11. The C architecture and the Python shared-multiplier variant

The supplied [C header](materials/umash-lemma/umash-src/umash.h) declares
`poly[2][2]`; each row is {f_i^2 mod p,f_i}. In
[umash.c](materials/umash-lemma/umash-src/umash.c), `umash_fp_medium` and
`umash_fp_long` select row i for output i. Their recurrence reduces modulo
p to the polynomial in Section 6. `oh_varblock_fprint` computes the common
ENH, the lane-wise PH shufflers, and checksum before ENH addition exactly as
in (4). Full blocks use seed as tag; the final block uses
seed XOR (uint8_t)n_bytes. The short routine uses OH indices length and
length+4. These are the structural facts used in this theorem.

The checksum loop's accumulators also give a direct correspondence for
the shufflers: PH value at position i<n-1 is inserted into the repeatedly
shifted accumulator and receives T^(n-i); every PH value receives the
final T from the ordinary XOR accumulator. The last PH value receives
only T. The final ENH and checksum PH are not shifted. This is precisely
the compressor model of Section 3.

`checks/c_bridge.c` includes the unmodified supplied C source. The check
builds it both with `UMASH_LONG_INPUTS=0` and `UMASH_LONG_INPUTS=1`, and
compares actual fingerprints against two evaluations of the literal
Python arithmetic using **different** f_0,f_1 and the same OH words.
The 1,776 comparisons cover every size 0,...,273 and further chunk/block
boundaries through 8192 bytes; multiplier/tag profiles include 1, p-1,
p-2, and seed wrap. All agree. This is a finite implementation check
supporting the source correspondence, not a probabilistic proof or a
formal equivalence theorem for every optimized machine-code path.

The Python reference's `UmashKey` instead has just one `poly` field.
Calling its `umash` twice with the same key and different `secondary`
flags reuses that multiplier. The product root bound in Lemma 7.1 then
does not apply. The known block-swap fibre gives a rigorous distinction:

**Proposition 11.1 (reference variant).** Let A be 256 zero bytes and B
be 256 bytes of value 1. For x=A||B and y=B||A, at f=p-1 the reference
fingerprint collides for every OH key and every seed.

**Proof.** Both blocks are full, so their tags are seed, independent of
position. Also f^2 mod p=1. The literal modulo-8p update is therefore

    acc <- acc + low + (p-1)*high  (mod 8p).

Starting from zero, the two block contributions commute. This holds
separately for each compressor, with no hypothesis on its output values.
The two finalizer inputs are equal in both outputs. ∎

Under the reference sampler with distinct OH words, the exact number of
keys in this guaranteed bad fibre is (q)_34=product_(j=0)^33(q-j), while
the whole key space has (p-2)(q)_34 elements. The fibre has exact probability

\[
 {1\over p-2}>2^{-83},\qquad
 {2^{83}\over p-2}>2^{22}                                 \tag{17}
\]

times the published 512-byte bound. The same fibre probability holds for
IID OH words, replacing (q)_34 by q^34. This is an exact count of a
guaranteed collision fibre, not a claimed exact count of the entire
collision set. Its lower bound alone refutes the 83-bit headline for this
**shared-multiplier Python variant**. The code reproduces the full-width
identity on 16 literal output comparisons, while the algebra proves it
for all keys. The exact fibre integers and cross-product in (17) are
recorded in the certificate.

For two independent multipliers, this particular fibre requires both
to equal p-1 and has probability 1/(p-2)^2. It supplies no refutation of
the C-architecture theorem. The Python variant still inherits valid
primary linear bounds by event inclusion; (1)--(2) are asserted only
with independent multipliers.

## 12. Reproduction, proof dependencies, and formalization

Run `bash checks/run_xeon.sh` from this project on
`<xeon-host>`. Every compilation and numerical
computation uses `nice -n 10 taskset -c 40-47`, at most eight cores and
ulimit -v 31250000 (32,000,000,000 bytes). Python 3.12 and GCC are used;
there are no third-party Python requirements. This round ran in
`<xeon-work>/umash-goal5`; the Mac only read/edited files and transferred them.
No subagents or messages to third parties were used.

The mathematical roles of the checks are:

- `certify_fingerprint.py` computes D from the proved signed-digit
  recurrence; explicitly enumerates every legal pattern
  (d+jp)/2 on d, -8<=j<=8; and checks the bit-count constants.
  The independent pair count is
  sum_(d,t) 2^(64-popcount(d))=q+2 sum_(j=1)^8(q-jp)=8q+72.
  Each pattern is sound and the sets are disjoint, so this identity is
  also an independent completeness certificate.
- The same program recounts every ordered mask pair for every h=1,...,15
  in the subcase-b table, using
  N_h=max_c #{(u,v): (v XOR (u<<1)) mod 2^h=c}, and S_h restricted to
  v not having every bit 3,...,h-1 set (no long masks when h<5).
  It recomputes
  2^-(128-h) min(N_h^2,S_h^2+min(1,(h+1)2^(4-h))*N_h^2).
  The maximum is J/q^2 at h=15. The probability argument behind this
  table is the imported subcase-b theorem, not merely this numerical sum.
- The imported PROOF2 and one-word ENH tables are checked for their
  stated rational ceilings, and the verified primary and ENH-only
  constants are checked against their supplied certificates. This is
  a dependency check, not a repetition of all earlier probability proofs
  or their billion-visit scaled validations.
- The complete case ledger, distinct-key corrections, (8), (11)--(13),
  the pointwise comparison coefficients, both linear scores, and (17)
  use integers or `Fraction`. Supplementary boundary evaluations cover
  thousands of lengths; the all-length arguments are (7), (10), and (16).
- `verify_certificate.py` imports no generator code. It proves soundness
  and completeness of the supplied masks from explicit word-pair patterns
  and total cardinality, reconstructs the expanded quadratic independently,
  and checks eight score intervals and every endpoint comparison by integer
  cross-products. Decimal output is never read to decide a check.
- `check_c_model.py` performs the 1,776 C/reference comparisons, checks
  all 120 actual PH position/count shufflers on the 127-dimensional PH
  space, and exhausts 3,549 scaled carryless multiplication/shuffler slices
  at widths 2,...,8. These confirm the kernel calculation and source
  correspondence; no scaled probability is extrapolated to 64 bits.

`checks/fingerprint_certificate.json`, `checks/independent_verification.json`,
and `checks/model_checks.json` contain the final PASS records. `manifest.json`
records the source, imported certificate, and final artifact hashes.
`PROGRESS.md` records the preliminary interpreter mismatch and the rejected
unrestricted-score endpoint; neither was retained as a passing calculation.

A Lean formalization can proceed in this order: import the already proved
mask/PH, primary, ENH-only, PH+ENH, subcase-b and encoding theorems with the
hypotheses of Section 3; prove the secondary kernel and conditional-twist
lemmas in Section 4; formalize the disjoint case list; define the two
comparison polynomials with independent multiplier coordinates; prove
(5)--(6) and Lemma 7.1; use the rational certificate (8) to conclude
`Published128`. The short/short branch uses actual collisions, so no
unproved statement about reduced constant-polynomial identity is required.
This proof does not assert `SharpPrimaryProjection`, and it does not require
or infer independence of the two compressors themselves.
