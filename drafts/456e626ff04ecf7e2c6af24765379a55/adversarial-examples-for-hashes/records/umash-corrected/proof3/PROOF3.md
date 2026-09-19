# A primary block constant of 3125 and a 53.38-bit UMASH-64 score

## 1. Result and precise scope

Put

\[
 q=2^{64},\qquad p=2^{61}-1,\qquad C=3125,\qquad
 A=\frac{3125}{q-561}.
\]

For the literal reference/Lean construction in the supplied materials, with
a fixed seed, 34 uniformly sampled distinct OH words, and an independent
uniform polynomial multiplier in \(\{2,\ldots,p-1\}\), this document proves

\[
 \boxed{\Pr[\operatorname{UMASH}_{64}(x)=\operatorname{UMASH}_{64}(y)]
 \le \frac{423\lceil L/512\rceil}{2^{61}}}                 \tag{1}
\]

for every positive integer L and distinct byte strings x,y of length at
most 8L. It also proves the more informative envelope

\[
 \epsilon(1)=\frac1{q-561},\qquad
 \epsilon(L)=A+(1-A)\min\left(1,
             \frac{2\lceil L/32\rceil}{p-2}\right)
             \quad(L\ge2).                               \tag{2}
\]

These bounds hold for IID OH words too. With the score convention in
`materials/VERDICT.md`, the envelope (2) has score strictly between
**53.38 and 53.39 bits**, attained at L=2. This improves the previous
46.52-bit envelope. It does **not** reach 55 bits. The coefficient in
(1) is exactly 423/64 times the published coefficient; this comparison
is between upper bounds, not between attained collision probabilities.

The principal new primary estimates under IID keys are:

* If at least one PH chunk differs, the projected block collision
  probability is at most **1123/q**, for arbitrary other data and tags.
* If every PH chunk agrees and the ENH data differ, the probability is
  at most **3125/q**, with any fixed common PH XOR offset and arbitrary
  fixed tags. At minimum ENH valuation r>=4 the sharper numerator is
  \(\min(2^r,2F_r-1)\), where \(F_4=36\) and
  \(F_r=26r-75\) for every integer 5<=r<=63.

This is the requested **corrected-primary-constant outcome**. It does not
prove `SharpPrimaryProjection`, whose numerator is 162. It does not
refute the published 55-bit claim, and it makes no new fingerprint claim.
In particular, no upper-bound certificate below is described as an exact
collision count for a production message pair.

As requested, the already established results of `materials/PROOF.md` and
`materials/PROOF2.md` are not reproved. Section 2 states precisely the
proved lemmas imported from those proof modules. Sections 3--7 prove the
new assertions and the complete corrected full-hash result. All new
numerical claims have integer or rational certificates in `checks/`.

## 2. Definitions and existing proved inputs

A word is an integer in [0,q). XOR and AND are bitwise operations.
Write a 128-bit output as (low,high). Carryless multiplication is
unreduced multiplication in F_2[X]. The PH value of a chunk (a,b) is
\((a\mathbin\oplus k)\odot(b\mathbin\oplus l)\).
The final ENH value, after translating its independent additive keys,
is

\[
 AB=qH+L,\qquad U=(H+\tau)\bmod q,\qquad E=(L,U\oplus L).
                                                               \tag{3}
\]

The other message uses A'=(A+delta) mod q, B'=(B+epsilon) mod q,
and its fixed tag tau'. At least one increment is nonzero whenever
the final data differ. A zero increment has valuation 64. XOR differences
and nonzero additive differences modulo q have the same 2-adic valuation.
The primary output is the XOR of the nonfinal PH values and (3).
Projection reduces each word modulo p separately.

Define the complete congruence masks and their admissible patterns by

\[
\begin{split}
D&=\{x\oplus y:0\le x,y<q,\ x\equiv y\pmod p\},\\
P(m)&=\{(m+jp)/2:-8\le j\le8,\ m+jp\ge0,\ m+jp\text{ even},
                   \ ((m+jp)/2)\mathbin\&m=(m+jp)/2\}.
\end{split}                                                    \tag{4}

The exact identity x-y=2(x&m)-m, for m=x XOR y, proves that a projected
equality is equivalent to x&m belonging to P(m). The supplied mask proof
and its new independent checks here give

\[
\begin{gathered}
 |D|=852,\quad \sum_{m\in D}|P(m)|=2771,\quad
 |D\cap2^r\mathbb Z|=(852,248,64,2,1)\quad(r=0,1,2,3,4),\\
 D\cap16\mathbb Z=\{0\},\qquad
 \{m\in D\setminus\{0\}:8\mid m\}=\{q-8\},\\
 |\{m\in D:v_2(m)=1\}|=184,\quad
 |\{m\in D:v_2(m)=2\}|=62,\\
 |\{m\in D:m_{63}=0\}|=248,\quad
 |\{m\in D:m_{63}=1\}|=604.
\end{gathered}                                                \tag{5}

Here are the existing proved inputs, including their necessary hypotheses.

1. **PH slice and low image** (`PROOF.md`, Lemmas 5.1 and 7.1).
   Leaving a key word paired with a nonzero PH coordinate difference d
   free, and conditioning all other keys, makes the full XOR difference
   an injective affine function d odot V XOR c. If v_2(d)=s, its low
   linear image is precisely the multiples of 2^s, with 2^s preimages
   per value. A PH product has bit 127 zero. An odd PH difference has
   projected primary probability at most 17/q, even with arbitrary
   differences in all conditioned terms (Lemma 5.2).
2. **ENH low difference and lifting** (`PROOF.md`, Lemmas 4.1--4.3).
   With delta nonzero of minimum valuation r, R=2^r, the additive low
   difference is uniform on RZ/qZ. With any common fixed XOR offset,
   the primary probability for r=0,1,2,3 is at most respectively
   (2771,1230,508,24)/q. If low words agree, fix A and the wrap indicator
   beta of B+epsilon. For each fixed mask/pattern the necessary lifting
   equation stated in Section 5 has at most one B. Its uniqueness proof
   uses only that its mask expression preserves agreement of low bits.
3. **Tag-only primary** (`PROOF2.md`, Theorem 9.2).
   Identical expanded chunks with distinct valid block tags have primary
   probability less than 1/(8q), for every fixed common PH offset. The
   tags must be seed XOR (block size modulo 256), hence differ as signed
   integers by at most 255. Arbitrary unrelated tags are not included
   in this imported result.
4. **Point mass, encoding and full-hash assembly** (`PROOF.md`, Lemmas
   6.3, 8.1--8.4 and 9.1--9.2). A fresh final ENH pair with any fixed
   tag and XOR offset has projected point probability less than 82/q.
   Expanded chunks and valid tags determine long messages. A long
   message with n blocks contributes a field polynomial with positive
   powers, degree at most 2n, and the two coefficients of its first
   block at degrees 2n and 2n-1. The long finalizer is bijective.
   Distinct short messages of equal length never actually collide;
   different short lengths have IID actual collision probability 1/q.
   A fixed short inverse-finalized word is uniform; residue zero has
   probability 9/q. Distinct-key acceptance is at least 1-561/q.
   The number p is prime and a nonzero degree-d comparison polynomial
   has at most d roots.

These are previously proved mathematical results, not the open Lean
probability propositions. In particular we import neither
`SharpPrimaryProjection` nor any claimed all-message short/short
constant-polynomial identity bound.

## 3. Low ENH XOR counts at a truncated width

These width-uniform statements make the new PH convolution argument
precise. Their hypotheses allow the second increment to be zero.

**Lemma 3.1.** Let n>=4, M=2^n, A,B independent uniform modulo M, and
delta nonzero modulo M with r=v_2(delta)<=v_2(epsilon). Define

\[
 Y=AB\bmod M,\quad Y'=(A+\delta)(B+\epsilon)\bmod M,
 \quad Z=Y\oplus Y'.
\]

Then:

(a) If r=0, Z is uniform on all M words.

(b) For every r, Pr[Z=0]=2^r/M.

(c) If 1<=r<n and v_2(e)=r, Pr[Z=e]<=2^(r+1)/M.

(d) If r=1 and v_2(e)=2, Pr[Z=e]<=5/M.

(e) If both increments are even, Pr[Z=M-8]<=(n+12)/M.

**Proof.** For (a), fix A. Since A and A+delta have opposite parity,
bit j of the XOR of the two products has coefficient one on the newly
chosen bit B_j. Its other terms, including the carry in B+epsilon,
depend only on earlier bits of B. Thus every target determines B
successively and uniquely. Assertion (b) is the additive low-difference
lemma: equality of words is the zero additive target.

For (c) and (d), write delta=R a, epsilon=R b, with a odd, and put

\[
 C=aB+bA+Rab\pmod M,\qquad
 aY=-bA^2+(C-Rab)A\pmod M.                                \tag{6}
\]

For fixed A the map B to C is a bijection. For a fixed candidate Y
and XOR target e, the difference (Y XOR e)-Y specifies C modulo M/R,
so there are exactly R candidate lifts of C modulo M.

If v_2(e)=r, all these C are odd. Since R is even, C-Rab is odd.
A quadratic with odd linear coefficient has at most two roots modulo
2^n: there are at most two choices modulo 2, and each lifts uniquely
at every subsequent bit, since changing its argument by 2^j changes
its value by 2^j modulo 2^(j+1) for j>=1. For each of the M possible
Y and R lifts there are consequently at most two A, each determining
one B. This proves (c).

For (d), R=2 and C has valuation exactly one. There are two C lifts
for each Y. If b is even, both quadratic coefficients in (6) are even
and C-2ab has valuation one. Thus Y must be even. Divide (6) by 2
modulo M/2; its linear coefficient is odd, giving at most two roots
modulo M/2 and at most four A modulo M. Counting M/2 possible Y and
two C lifts gives at most 4M operand pairs.

If b is odd, C-2ab is divisible by 4, and A,B have the same parity.
On the even-even part write A=2A0, B=2B0. After dividing products by
four, their XOR at width n-2 is an odd-first-increment instance;
the residues of A0,B0 at that width are uniform and independent.
By (a) this contributes exactly (1/4)/2^(n-2)=1/M for every target
divisible by four. The triangular proof in (a) applies also at this
smaller width. On the odd-odd part, complete the square in (6).
The center is even because C-2ab is divisible by 4; the square root
is therefore odd. An odd target has at most four square roots modulo
2^n: dividing by one root reduces to z^2=1; one of z-1,z+1 has
valuation one, forcing z=1 or -1 modulo 2^(n-1), with at most four
lifts modulo 2^n. There are M/2 odd Y and two C lifts, so this part
contributes at most 4/M. The total is at most 5/M.

For (e), Z=M-8 forces complementary upper n-3 bits and equal bottom
three bits. Consequently

\[
 Y+Y'\equiv M-8+2(Y\bmod8)\pmod M.
\]

Dividing by two gives at most eight targets modulo M/2 for

\[
 (A+\delta/2)(B+\epsilon/2)+\delta\epsilon/4.
\]

The two factors are independent uniform modulo K=M/2. For a nonzero
target z modulo K of valuation v, their product has probability exactly
(v+1)/M. Indeed, for each valuation j=0,...,v of the first factor,
its probability 2^(-j-1) times the conditional target probability
2^j/K is 1/M; larger valuations cannot contribute. Target zero has
probability (n+1)/M by the same sum over j=0,...,n-2, plus the zero
first factor of probability 1/K.

The eight targets in question are **consecutive cyclic residues** modulo
K, after translating by -delta epsilon/4: they are -4+i-delta epsilon/4,
i=0,...,7. Since K>=8, they are distinct. Exactly four are odd, two
have valuation one, one has valuation two, and one is divisible by
eight. The last has point probability at most (n+1)/M. Their total
mass is at most [4+2 times 2+3+(n+1)]/M=(n+12)/M. This proves (e).
All bounds remain valid when they exceed one. ∎

## 4. The PH bound: 1123/q

**Theorem 4.1.** Two same-count blocks with at least one differing PH
chunk have primary projected probability at most 1123/q, under IID
keys, for arbitrary changes in the other chunks and arbitrary fixed tags.

**Proof.** Let s be the minimum valuation among all nonzero PH coordinate
XOR differences. It is an integer between 0 and 63. The total PH low
XOR difference Z is uniform on the multiples of 2^s, and is independent
of the ENH keys. Indeed, expose a free PH word whose multiplying
difference has valuation s. Its low linear image is this whole subspace;
all other PH differences, including the affine constant, belong to the
same subspace. Averaging other PH keys preserves uniformity. Let E
be the ENH low XOR difference.

If s=0 the imported odd-PH lemma gives 17/q. For every s>=1, set
M=2^s and let n_s(c) count masks in D with bottom s bits equal to c.
Conditioning E first and then averaging gives the exact identity for
the necessary mask event

\[
 q\Pr[Z\oplus E\in D]
       =M\sum_{d\in D}\Pr[E\bmod M=d\bmod M].             \tag{7}
\]

If 1<=s<=3, this is a complete finite calculation on two M-valued
operands and two fixed increments modulo M. For fixed increments
delta,epsilon the right side is exactly

\[
 \frac1M\sum_{a=0}^{M-1}\sum_{b=0}^{M-1}
 n_s\bigl((ab\bmod M)\oplus((a+\delta)(b+\epsilon)\bmod M)\bigr).
                                                               \tag{8}
\]

Enumerate all M^2 increment pairs, including zero increments. At
M=2,4,8, the respective maximum integer sums before division by M
are 1704,3408,6816. Each therefore gives 852/q. These are all
4+16+64=84 cases, with 16+256+4096=4368 operand-pair visits;
the complete tables and independent literal checks are in `checks/`.
Reduction of full-width independent operands modulo M is uniform,
so this enumeration covers every full-width increment in these cases.

Now suppose s>=4. If both ENH increments are divisible by 16
(including two zero increments), Z XOR E is divisible by 16.
Projection forces Z XOR E=0 by (5). Condition the ENH keys and
all but one PH key word with nonzero multiplying difference. The
full primary XOR difference is injective in that word, and its bit
127 is fixed. It can meet at most 604 targets (0,d), d in D, with
the prescribed top bit. The bound is therefore 604/q. This step
counts the two lanes of the same PH slice together; it does not
treat them as independent random variables.

It remains that the ENH minimum valuation r is 0,1,2, or 3, less
than s. The bottom s bits of E have exactly the law in
Lemma 3.1 at width s: truncate the uniform operands and increments.
Choose the operand order with delta of minimum valuation. Truncation
retains that nonzero valuation. Apply (7).

For r=0, the right side is 852. For r>=1 only masks divisible
by 2^r contribute. Every nonzero such mask has valuation at most
three, which is below s, so none becomes zero upon truncation.
Using (5) and Lemma 3.1 gives the following complete count:

| r | zero mask | valuation-1 masks | valuation-2 masks | mask q-8 | total |
|---|---:|---:|---:|---:|---:|
| 1 | 2 | 184 times 4 | 62 times 5 | s+12 | 1060+s <=1123 |
| 2 | 4 | 0 | 62 times 8 | s+12 | 512+s <=575 |
| 3 | 8 | 0 | 0 | 16 | 24 |

For q-8, the truncated mask is M-8. When r=3, use (c) rather
than (e). Repeated truncated masks present no problem: (7) sums
over full targets d, and each is a different possible value of
Z XOR E. All displayed numerators are at most 1123. The cases
s=0, s=1..3, and s>=4 with either ENH valuation range exhaust
all possible PH differences. ∎

No rank assumption on the two-key PH difference is made. No assertion
that a single arbitrary conditioned PH fibre has at most 162 keys is
used; the known fibres of sizes 183 and 242 are consistent with this proof.

## 5. Grouping the ENH lifting functions

The earlier lifting count used all 2771 full mask/pattern pairs. Its
necessary congruence actually depends on fewer functions.

**Lemma 5.1 (exact function classes).** Fix r>=2, R=2^r, and word
pairs (m,t) with t a submask of m. Define a function of a word l by

\[
 f_{m,t}(l)=m-2((l\mathbin\&m)\oplus t)\pmod R.            \tag{9}
\]

Two such functions are equal on every l if and only if their labels

\[
 \left(m\bmod2^{r-1},\ (m-2t)\bmod2^r\right)             \tag{10}
\]

are equal. Substituting l XOR M for l, with any fixed common word M,
preserves exactly the same equivalence classes.

**Proof.** As an ordinary affine function of the separate binary digits
l_j, the constant in (9) is c=m-2t. At a set bit j of m its coefficient
is -2^(j+1) if t_j=0, and +2^(j+1) if t_j=1; other coefficients vanish.
Coefficients at j>=r-1 vanish modulo R. The coefficient at j=r-2
is 2^(r-1) for either sign. At j<r-2 its sign is determined by
t_j, and those t_j are determined from

\[
 2t\equiv m-c\pmod {2^{r-1}}.
\]

Thus (10) determines the constant and every coefficient. Conversely,
evaluate the function at 0 and at each 2^j: its constant is recovered,
and the nonzero coefficients identify exactly the set bits of m below
r-1. This recovers (10). The input translation by XOR M is a bijection,
which proves the last assertion. ∎

Let F_r be the number of labels (10) arising from m in D and t in P(m).
The complete exact finite count is

\[
 F_4=36,\qquad F_r=26r-75\quad(5\le r\le63),\qquad
 \max_{4\le r\le63}F_r=1563.                              \tag{11}
\]

Equation (11) is a finite certificate for each of its sixty integer
arguments, not an extrapolation of a pattern. Section 8 gives its
complete enumeration and independent verifier. The class (0,0)
contains only the pair (m,t)=(0,0): its second coordinate requires
2^r to divide jp for some |j|<=8, hence j=0; then m=2t and t a
submask of m force m=t=0, as also follows from the signed-difference
identity for equal words.

**Lemma 5.2 (one B per function class and wrap).** In the ENH setting,
take a nonzero increment delta of minimum valuation r, 4<=r<=63,
and fix A, a wrap beta in {0,1}, and one class (10). There is at most
one B that produces low equality and high projection equality with
a mask/pattern in that class, for any fixed common high offset M.

**Proof.** Write R=2^r, Q=q/R, alpha=floor((A+delta)/q), and

\[
 d=(\delta-\alpha q)/R,\quad e=(\epsilon-\beta q)/R.
\]

The integer d is odd since Q is even. In this branch A'=A+Rd,
B'=B+Re, so

\[
 A'B'-AB=R(dB+eA+Rde).
\]

On low equality, let m=U XOR U' and let
t=(M XOR L XOR U)&m be the projected first high-word pattern.
Then t belongs to P(m),

\[
 U\mathbin\&m=((AB\mathbin\&m)\oplus(M\mathbin\&m)\oplus t),
\]

and the necessary lifting congruence of `PROOF.md`, Lemma 4.3, is

\[
 dB+eA+Rde\equiv Q\bigl(f_{m,t}((AB)\bmod q\ \oplus M)
                            -(\tau'-\tau)\bigr)\pmod q.  \tag{12}
\]

Only f modulo R is used because QR=q. By Lemma 5.1 all members
of the chosen class give the identical equation (12), for every B.
For completeness, its uniqueness uses precisely the existing lifting
argument: if B1,B2 first differ at bit j, the two low products agree
below j. Their masked-XOR expressions agree below j, so the difference
of the f values is divisible by 2^(j+1). The right-hand difference in
(12) is therefore divisible by 2^(j+1), whereas d(B1-B2) has valuation
exactly j. This is impossible modulo q, since j<64. The branch and
the original high equation can only discard the unique candidate. ∎

The zero class admits a stronger count over both wraps together.

**Lemma 5.3 (additive product target).** For independent uniform words
A,B, increments delta!=0 and arbitrary epsilon, every fixed target
for A'B'-AB modulo q^2 has probability at most 1/q.

**Proof.** Fix A, hence distinct A,A'. Within each of the at most two
B-wrap intervals, the difference is an affine function of B with
nonzero slope A'-A. Differences of two values have absolute magnitude
less than q^2 and are nonzero. For B1<B2 in different intervals,
the difference between their two product differences is

\[
 (A'-A)(B2-B1)-qA'.
\]

Since 0<B2-B1<q, this lies strictly between -q^2 and 0, including
the cases A=0 or A'=0. It cannot vanish modulo q^2. Thus at most
one B hits any specified residue, across both intervals combined.
Average the bound 1/q over A. ∎

**Theorem 5.4 (ENH bound).** With identical PH data and different ENH
data, the primary projected probability is at most 3125/q, uniformly
over fixed common PH offsets and arbitrary fixed tags. For r>=4 it
is at most min(2^r,2F_r-1)/q.

**Proof.** Condition every PH key, obtaining a common fixed offset.
Exchange ENH operands if necessary to choose delta as a nonzero
increment of minimum valuation. For r<=3 use the already proved
numerators (2771,1230,508,24), all below 3125.

For r>=4, the low products agree modulo 16. Primary low projection
forces their equality. For each of the F_r-1 nonzero classes,
Lemma 5.2 allows at most q choices of A times two wraps, hence 2q
operand pairs. The zero class means m=0, so U=U' as well as L=L'.
It implies the fixed additive target

\[
 A'B'-AB\equiv-q(\tau'-\tau)\pmod {q^2}.
\]

Lemma 5.3 bounds that class by q operand pairs. The complete count
is therefore at most [2(F_r-1)+1]q=(2F_r-1)q. Every actual collision
has a class and a wrap; uniqueness applies to the class, not just
to a selected representative. Divide by q^2. The separate low-equality
probability is 2^r/q, so one may take the minimum. Finally (11) gives
2F_r-1<=3125. Average over the initially conditioned PH keys. ∎

This includes no PH chunk, one changed ENH word, two changed words,
all operand and tag wraps, and all common XOR offsets. It does not
require an extremality assertion about a searched family.

## 6. Complete block ledger and message identity cases

**Theorem 6.1.** For valid blocks with different expanded chunk lists
or different valid tags, the IID primary projected probability is
at most 3125/q.

The following cases are disjoint and exhaustive. Their bounds are
combined by taking a maximum, not a sum.

| Case | Numerator in units of 1/q |
|---|---:|
| Different chunk counts | <82, by the fresh final ENH point bound |
| Same count, at least one PH chunk differs | <=1123, Theorem 4.1 |
| Same count, PH chunks agree, final data differ | <=3125, Theorem 5.4 |
| Same expanded data, different valid tags | <1/8, imported tag-only theorem |

If neither data nor tag differ the theorem does not apply. If the
counts differ, condition all keys except the longer block's final
pair, which is unused by the shorter block; this explains the fresh
point-bound hypothesis in the first row. All other rows allow zero
values of their random operands and arbitrary changes allowed by the
case predicate.

**Corollary 6.2.** For distinct messages with at least one long message
(length greater than eight bytes), the primary comparison-polynomial
identity probability is at most 3125/q under IID OH words.

For two long messages with equal block counts, choose a differing
aligned expanded-block/tag tuple using encoding injectivity, before
sampling keys. Polynomial identity requires both of its projected
coefficients to agree, so Theorem 6.1 applies. With unequal block
counts, the first block of the longer message has two degrees above
the shorter polynomial's degree. Identity forces both coefficients
to zero, and the imported fresh-product estimate is <82/q.
For short/long comparisons, the long polynomial has zero constant
term. Identity requires the inverse-finalized short word to be zero
modulo p, an event of probability 9/q. This is a necessary event;
no independence from the long computation is asserted. These three
cases exhaust the corollary's hypotheses.

Under uniform distinct OH words, the corollary becomes A=3125/(q-561)
by conditioning on the acceptance event of probability at least
1-561/q. The multiplier remains independent.

For two short messages use their actual collision bound 1/(q-561),
or zero at the same length. **This is not an inference about equality
of their reduced constant comparison polynomials.** The exact
all-message `PrimaryIdentityBound(162/(q-561))` remains unproved,
including that separate short/short issue. The corrected full-hash
theorem does not need it.

## 7. Full hash, envelope and score

For any pair with a long message, let a<=A be its conditioned identity
probability. Both messages of length at most 8L have at most
ceil(L/32) blocks. Given a nonzero comparison polynomial, the independent
uniform multiplier gives collision probability at most

\[
 \rho(L)=\min(1,2\lceil L/32\rceil/(p-2)).
\]

Thus the total is at most a+(1-a)rho(L)<=A+(1-A)rho(L). For
short/short pairs the direct bound is smaller. At L=1 only short
messages are possible. This proves (2) for all positive L.

For h=ceil(L/512)>=1, ceil(L/32)<=16h, so (2) is at most

\[
 A+\frac{32h}{p-2}\le
 h\left(A+\frac{32}{p-2}\right)<\frac{423h}{2^{61}}.       \tag{13}
\]

The strict final comparison is an exact rational certificate:

\[
 422<2^{61}\left(\frac{3125}{2^{64}-561}
                    +\frac{32}{2^{61}-3}\right)<423.
\]

No cap on L is used. An upper bound may be clipped at one.

For the score define

\[
 s=\inf_{L\ge1}\log_2\frac{L}
 {\max(2^{-64},\min(1,\epsilon(L)))}.
\]

For L>=2, ceil(L/32)<=L/2 implies

\[
 \frac{\epsilon(L)}{L}
 \le\frac A2+\frac{1-A}{p-2}=\frac{\epsilon(2)}2.
\]

At L=2 this is equality. Clipping at one cannot worsen the score;
the lower floor contributes at most 1/(qL), which is smaller than
epsilon(2)/2. At L=1 the score is log_2(q-561), larger than this
minimum. Consequently

\[
 s=\log_2 R_*,\qquad
 R_* = \frac{2}
 {\frac{3125}{q-561}+
   (1-\frac{3125}{q-561})\frac2{p-2}}.
\]

The exact integer-power checks are

\[
 R_*^{50}>2^{2669},\qquad R_*^{100}<2^{5339}.
\]

They prove 53.38<s<53.39, without relying on floating-point logarithms.
The certificate also records R_* as an exact fraction and a decimal
logarithm for display. This is a score of a proved envelope; no message
pair is claimed to attain it. In particular it supplies no refutation
of the [published primary bound](https://github.com/backtrace-labs/umash).

## 8. Exact certificates and independent validation

The production-width part uses only integer and rational operations.
`checks/certify_primary.py` performs the following complete calculations.

1. Enumerate D by the signed-digit recursion from `PROOF.md`: even
   n removes a zero bottom digit; odd n branches to (n-1)/2 and
   (n+1)/2, marking a nonzero bottom digit. Start with jp, j=0..8,
   and width 64. Independently enumerate x+jp by a binary addition
   automaton, retaining the carry and accumulated XOR mask and rejecting
   final overflow. The two full sets agree.
2. For every mask enumerate every j=-8..8 in (4). Check completeness
   by the exact identity
   \(\sum_{m\in D}|P(m)|2^{64-\operatorname{popcount}(m)}=8q+72\).
   Each summand counts a disjoint set of ordered congruent word pairs.
   The right side independently counts all such pairs as
   \(q+2\sum_{j=1}^8(q-jp)\).
3. Count all mask prefixes for every s=1..64. Enumerate the labels
   (10) for every r=4..63, and check the formula and maximum in (11).
   Independently form the functions' affine coefficient signatures
   from their values at zero and the basis inputs; their equivalence
   classes agree exactly. `primary_certificate.json` contains all
   masks, all patterns, all prefix counts, and a representative and
   multiplicity for every function class at every r.
4. Check all 84 small-width PH convolution cases and every numerator
   for s=4..63, all old small-r
   ENH numerators, the block maximum, the exact rational inequality
   (13), and the score inequalities. Additional length checks exercise
   small lengths and boundaries up to and beyond 2^64; the symbolic
   proof in Section 7 covers every length.

`checks/verify_certificate.py` imports none of that generator. It checks
the validity of all listed mask patterns and proves completeness from
the ordered-pair total, computes function classes from literal coefficient
signatures, and independently repeats the rational comparisons. At
widths 5..9 it constructs masks from actual congruent word pairs and
compares the entire function tables for every label, every r=4..w-1,
and three common offsets. These include the sign ambiguity at the last
nonzero coefficient, which is essential to the improvement.

`checks/validate_primary.cpp` independently checks Lemma 3.1 and (8)
by visiting all operand pairs for every nonzero even first increment
of valuation 1..3 and every second increment of at least that valuation,
including zero, at widths 4..8. Its convolution weights are the complete
production-width prefix counts. It also checks all 84 increment pairs
at widths 1..3, with every operand pair. At widths 5..9 it checks every increment
pair whose minimum valuation is at least four, in the chosen operand
order, on the complete low-equality grid, with three declared tag/offset
profiles. Every actual projected collision is inserted into its
(A,beta,function-class) fibre and duplicate B values are rejected.
The stronger zero-class count is checked separately. It also checks
every conditional additive-product slice at widths 3..5.

These scaled checks validate the algebra and implementation of the
certificate. They are not extrapolated probabilities. Actual run totals
and zero-failure reports are in `checks/validation.json` and
`checks/independent_verification.json`; source hashes, affinity, niceness
and memory limits are in `checks/manifest.json`.

Reproduction on the Xeon is `bash checks/run_xeon.sh`, from
`<xeon-work>/umash-goal3`. Every computation and compilation runs under
`nice -n 10 taskset -c 40-47`, with at most eight threads and a
32,000,000,000-byte address-space limit. The editing Mac runs no
enumerations. No sub-agents or external messages are used.

## 9. Remaining sharp obligation

The new proof removes the 604^2 bottleneck uniformly, but its replacement
1123 and the grouped ENH constant 3125 both exceed 162. The sharp PH
problem now has a complete valuation/convolution formulation; the
remaining loss includes discarding the high projection in the small
valuation cases and discarding the pattern condition in (7). The ENH
count still allows two candidates per nonzero function class without
using all of the high-word restrictions. Reducing these is a further
problem, not a proved extremality statement.

`SharpPrimaryProjection` and the published 55-bit primary bound remain
unproved here; neither is refuted. The completed result of this round
is the unconditional corrected primary theorem (1)--(2), with the
strict 53.38--53.39-bit envelope score.
