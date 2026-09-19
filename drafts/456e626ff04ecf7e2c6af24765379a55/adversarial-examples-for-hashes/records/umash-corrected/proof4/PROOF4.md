# UMASH-64: coefficient 58, and a proof of the published coefficient 64

## 1. Statement, model and scope

Let q=2^64 and p=2^61-1. Use the literal primary reference hash, with a
fixed seed, 34 OH words sampled uniformly without replacement, and an
independent multiplier f uniform on {2,...,p-1}. The result also holds with
IID OH words. For distinct byte strings x,y of lengths at most 8L, L>=1,

\[
 \boxed{\Pr[\operatorname{UMASH}_{64}(x)=\operatorname{UMASH}_{64}(y)]
       < {58\lceil L/512\rceil\over2^{61}}.}                 \tag{1}
\]

Thus the published primary coefficient **64 is proved**, with room to spare.
The proof uses the implemented accumulator modulo **8p**, including its
three bits beyond the field reduction. It is not a proof of the stronger
all-message `SharpPrimaryProjection` premise, whose numerator is 162.
No fingerprint theorem or counterexample is claimed. No theorem about
Salsa20 expansion or a nonuniform production key sampler is asserted.

A more informative proved envelope is as follows:

\[
 A={205\over q-561},\quad S={435\over q-561}+{2\over p-2},\quad
 \rho(L)=\min\left(1,{2\lceil L/32\rceil\over p-2}\right),
\]
\[
 \epsilon(1)={1\over q-561},\qquad
 \epsilon(L)=\max\{A+(1-A)\rho(L),S\}\quad(L\ge2).          \tag{2}
\]

Probabilities may be clipped at one. With the supplied score convention
inf_L log2(L/max(2^-64,min(1,epsilon(L)))), this envelope has score
**strictly between 56.18 and 56.19 bits**, attained at L=2. This is a score
of an upper envelope, not an attained collision rate.

The supplied PDF states on p. 6 that reduction "loses slightly more than
6 bits of entropy". Its explicit p. 19 bound is
ceil(q/p)^2 * 2^-63 = 162/q. Its primary headline on pp. 1 and 6 is
ceil(s/4096)*2^-55 for s bytes, hence 64 ceil(L/512)/2^61 in the present
units. This proof establishes the headline directly. It does not validate
the quoted 162/q intermediate step. The use of 8p below is material:
replacing the literal accumulator by a final reduction modulo p would
remove Lemma 7.1. See the supplied
[paper](materials/umash-lemma/umash.pdf),
[reference](materials/umash-lemma/umash_reference.py), and
[reported projection gap](https://github.com/backtrace-labs/umash/issues/40).

All new finite constants have integer or rational certificates in `checks/`.
The proof is on paper with exact computation, not a completed Lean proof.
The requisite Lean conclusion is a new direct proof of `Published64`, rather
than an application of `conditional55_with_primary`.

## 2. Definitions and imported results

Words are integers in [0,q). XOR is addition of binary word polynomials.
The notation a odot b denotes their unreduced carryless product in F_2[X].
For an ENH pair, after translating its independent additive keys, write

\[
 A'=(A+\delta)\bmod q,\quad B'=(B+\eta)\bmod q,
 \quad AB=qH+Y,\quad A'B'=qH'+Y',
\]
\[
 U=(H+\tau)\bmod q,\quad U'=(H'+\tau')\bmod q,
 \quad E=(Y,U\mathbin\oplus Y).
\]

Here A,B are independent uniform words. Tags are fixed before sampling.
For valid tags, |tau'-tau|<=255. A zero increment has valuation 64.
Whenever final data differ, exchange the operands so delta is nonzero
and r=v2(delta)<=v2(eta). A common PH offset means an independent XOR
contribution that is the same in the two messages.

Set

\[
 D=\{x\oplus y:0\le x,y<q,\ x\equiv y\pmod p\},
\]
\[
 P(m)=\{(m+jp)/2:-8\le j\le8,\ m+jp\ge0,\ m+jp\text{ even},
                       ((m+jp)/2)\mathbin\&m=(m+jp)/2\}.
                                                               \tag{3}
\]

For m=x XOR y, x-y=2(x&m)-m. Thus projected equality requires, and for
that actual word pair is equivalent to, x&m in P(m). The complete tables are

\[
 |D|=852,\quad \#(D\cap2^r\mathbb Z)=(852,248,64,2,1)
                    \quad(r=0,1,2,3,4),                       \tag{4}
\]

with D intersect 16Z={0}, and q-8 the only nonzero mask divisible by eight.
The numbers of masks with high bit 0 and 1 are 248 and 604. Every nonzero
mask has highest bit at least 60. There are at most eight patterns per mask.
The number of ordered congruent word pairs with mask valuation 1 is 2q+16;
with mask valuation 2 it is q+8. These last two counts also follow directly
by summing q-|j|p over j=+/-2,+/-6 and j=+/-4, respectively.

The following are previously established inputs, with their hypotheses.
Their proofs are not repeated here.

* `PROOF.md`, Lemmas 4.1, 5.1, 5.2 and `PROOF3.md`, equation (7): a
  minimum-valuation r ENH low additive difference is uniform on
  2^r Z/qZ; a nonzero PH difference has an injective one-word slice;
  an odd PH difference has primary bound 17/q. If the minimum PH
  coordinate valuation is s, the total PH low difference Z is uniform
  on 2^s Z/qZ, independently of ENH. Consequently, for every low mask d,
  with M=2^s,
  \[
   q\Pr[Z\oplus(Y\oplus Y')=d]
       =M\Pr[(Y\oplus Y')\bmod M=d\bmod M].                \tag{5}
  \]
* `PROOF3.md`, Lemmas 5.1--5.3: for a nonzero increment of valuation
  r>=4, put R=2^r, Q=q/R. On low equality, for fixed A and B-wrap beta,
  the necessary equation
  \[
   dB+eA+Rde\equiv Q(f_{m,t}(AB\bmod q)-\Delta\tau)\pmod q,
   \quad f_{m,t}(l)=m-2((l\mathbin\&m)\oplus t),            \tag{6}
  \]
  has at most one B, where d=(delta-alpha q)/R is odd,
  e=(eta-beta q)/R, alpha=floor((A+delta)/q). With a common high offset
  M_hi, replace l by l XOR M_hi. Functions are needed only modulo R.
  Their complete labels are (m mod 2^(r-1),(m-2t) mod R).
  For any fixed additive product target modulo q^2, the two changed
  products hit it with probability at most 1/q, including eta=0.
* `PROOF2.md`, Lemma 4.1: an integer quadratic with nonzero leading
  coefficient a has at most 2 sqrt(m ell/|a|)+2m integer arguments in
  the preimage of m disjoint half-open intervals of length ell. On
  one monotone side of its vertex, the corresponding bound is
  sqrt(m ell/|a|)+m; this is the single-side part of that proof.
  Its Lemma 4.3 supplies the dense high-XOR bound
  276*2^(64-popcount(m))/q for two nonzero ENH increments and low equality.
* The verified one-word ENH lemma in
  `materials/umash-enh-verify/VERDICT.md`, Section 3/Lemma 2, gives
  Pr[Y=Y', U XOR U'=m] <= kappa(m)/q for one changed word, with
  \[
   \kappa(m)=\min\{2^h,\ 5+2^{71-h},\
           5+\lceil4q/(g(m)-255)\rceil\ \text{if }g(m)>255\},
   \quad g(m)=2^{\operatorname{bitlength}(m)}-m.             \tag{7}
  \]
  Here h=popcount(m), tags are valid, and the lemma permits every common
  PH offset. The zero mask can instead use the additive bound 1/q.
* `PROOF2.md`, Theorem 9.2: tag-only primary probability is <1/(8q).
  `PROOF.md`, Sections 6, 8 and 9: a fresh final ENH pair has projected
  point mass <82/q; expanded chunks with valid tags encode long messages
  injectively; unequal block counts have identity bound <82/q;
  short/long identity has bound 9/q. Short/short actual collisions have
  bound 1/q under IID keys, and are impossible at equal length.
  The long finalizer is bijective. The comparison polynomial has degree
  at most twice the block count. The prime-field root bound applies on
  the p-2 independent multipliers. Distinct-key acceptance is >=1-561/q.

All probability arguments until Section 8 use IID OH words. Different
conditionings are combined by minima or explicit conditioning, never by
multiplying incompatible marginal bounds.

## 3. Sharper low ENH counts

This section uses any width n>=4, M=2^n. Truncate independent operands and
increments modulo M. Let delta=Ra, eta=Rb, R=2^r, a odd, r>=1, and r<n.
The second increment may be zero. Put

\[
 C=aB+bA+Rab\pmod M,\qquad
 aY=-bA^2+(C-Rab)A\pmod M.                                 \tag{8}
\]

For fixed A the change B -> C is a bijection. For prescribed Y,Y',
C is fixed modulo M/R and has R lifts modulo M; each root A in (8)
determines exactly one B.

**Lemma 3.1.** If an XOR target z has valuation r, then
Pr[Y XOR Y'=z]=R/M. If r=1 and v2(z)=2, the probability is at most 3/M.

**Proof.** For the first assertion all C lifts are odd, and C-Rab is odd.
A quadratic with odd linear coefficient and even quadratic coefficient is
a permutation modulo 2^n. With odd quadratic coefficient it has exactly
two roots for each even value and none for odd values. To verify these
facts, check the two residues modulo two; each existing root has exactly
one lift at every subsequent bit, since its derivative is odd. Thus if b
is even, each of M possible Y and R lifts gives one A. If b is odd, only
M/2 even Y occur, each with two roots per lift. Both counts are RM.

For the second assertion, R=2 and C has valuation one. If b is even,
divide (8) by two modulo M/2. Its linear coefficient is odd. If b/2 is
even, only even Y occur and there are at most two A modulo M for each
lift C. If b/2 is odd, Y must be divisible by four, and there are at most
four A per lift. In either case the total is at most 2M operand pairs.

If b is odd, A,B have the same parity. Even-even pairs contribute exactly
1/M to every z divisible by four: dividing operands by two and products
by four leaves an odd-first-increment low XOR map at width n-2, which is
triangular and uniform. On odd-odd pairs write c=C-2ab, divisible by four.
The equation implies
(bA-c/2)^2=(c/2)^2-abY modulo M. The square root is odd, so there are at
most four roots. Moreover put z=4v, v odd, and Y=1+2y_1+4y_2 modulo 8.
Since C=z/2-(Y&z) modulo M/2, one has
(c/2)^2-abY=(v-ab)^2-ab(1+2y_1) modulo 8.
The y_2 term cancels because ab is odd. Only one of the two y_1 values
can give an odd square, which is 1 modulo 8. Thus there are at most M/4
eligible odd Y, two C lifts, and four roots: at most 2M pairs. Together
with the even-even contribution this is 3/M. For n=4 the C lifts differ
by eight, so the same modulo-eight calculation remains valid. ∎

**Lemma 3.2 (dense target).** If r=1, Pr[Y XOR Y'=M-8]<=4/M.
If r=2, this probability is at most 6/M.

**Proof.** Set K=M/2, u=delta/2, v=eta/2, T=A+u modulo K and S=B+v
modulo K. These factors are independent uniform modulo K. The XOR event
implies Y+Y'=M-8+2(Y mod 8), so TS belongs to the eight consecutive
cyclic residues I={-4+i-uv:0<=i<8} modulo K. Since K is divisible by eight,
comparison with Y=TS-uS-vT+uv modulo 8 also gives uS+vT=4 modulo 8.

Write H0=2^(4-r), h=4-r. Dividing the last congruence gives
\[
 aS+bT=H0/2\pmod {H0}.
\]
For each T residue t modulo H0, S has exactly one possible residue s.
There are 8/H0 targets in I with residue ts modulo H0. If t!=0, fix T:
there are K/H0 possible T and at most 2^v2(t) solutions S for each product
target. If t=0, then s=H0/2; fix S instead, giving K/H0 choices and at
most 2^(h-1) solutions T per target. The sum of these powers is
\[
 \sum_{t=1}^{H0-1}2^{v_2(t)}+2^{h-1}=(h+1)H0/2.
\]
The probability is therefore at most
8(h+1)/(H0 M), namely 4/M and 6/M. No assumption about a nonzero product
or a nonzero second increment was used. ∎

**Lemma 3.3 (small ENH valuations in a block).** Suppose either all PH
chunks agree, or the minimum nonzero PH coordinate valuation is s>=4.
If the ENH minimum valuation is r=0,1,2,3, the primary probability is,
respectively, at most
\[
 17/q,\quad19/q+64/q^2<20/q,\quad14/q+32/q^2<15/q,\quad16/q.
                                                               \tag{9}
\]
The r=0 assertion holds without any restriction on the PH valuations.

**Proof.** For r=0, condition all PH keys and A. At every bit of B, the
two low products have opposite coefficients on the new bit. For each
signed target jp, -8<=j<=8, the low-word integer subtraction, including
its already determined borrow, therefore fixes B successively. There
is at most one B per target, for any two fixed PH offsets.

For r>=1 condition the PH keys when counting the nonzero masks of
valuation one or two. Their two low offsets agree modulo 16. Thus the
valuation of the actual ENH XOR difference equals that of the total
low mask whenever that valuation is at most three.

For r=1 and a valuation-one total mask, (8) gives at most two operand
pairs for each prescribed Y,Y' if b is even; if b is odd it gives at
most four, with Y even. There are 2q+16 ordered congruent output pairs,
and exactly half have the required first-word parity in the latter
case. Both bounds give 4/q+32/q^2.

For a valuation-two mask and b even, division by two as in Lemma 3.1
bounds each prescribed pair by four with Y even, or by eight with Y
divisible by four, according as b/2 is even or odd. The q+8 congruent
pairs are evenly distributed among their first-word residues modulo
four. Either alternative gives 2/q+16/q^2.

If b is odd, on the odd-odd part the completed square has an odd root,
so each Y,Y' has at most eight operand preimages. Exactly half of the
q+8 output pairs have the required odd-product parity, contributing
4/q+32/q^2. Bound the entire even-even projected low event separately:
its probability is 1/4, and after dividing products by four the operands
are uniform at width 62 with an odd first increment. The fixed two low
output bits allow at most five of the 17 integers j. Each then fixes
one second operand by the odd triangular argument. Its unconditional
contribution is at most (1/4)*5/(q/4)=5/q.

It remains to count the zero mask and q-8. If all PH chunks agree these
are direct ENH low-XOR events; if PH changes occur, apply (5) at width s.
Low equality has probability R/M and Lemma 3.2 applies to the truncated
target M-8. Their contributions for r=1 are 2/q and 4/q. The worse
b-odd sum is 19/q+64/q^2. Overcounting the even-even event here is harmless.

For r=2, the valuation-two mask has at most four preimages per prescribed
Y,Y' when b is even, or eight restricted to even Y when b is odd. This
gives 4/q+32/q^2. Zero and q-8 contribute 4/q and 6/q, yielding the stated
bound. For r=3 there are only zero and q-8; equality and Lemma 3.1 each
contribute 8/q. These statements also cover eta=0. ∎

## 4. A PH primary bound of 151/q

The new step is to keep the unused randomness in a PH pair after fixing
its difference. All other chunk keys may be conditioned arbitrarily.

**Lemma 4.1 (conditional PH product).** Let a,b be two nonzero coordinate
XOR differences, let g=gcd(a,b) in F_2[X], a=gu, b=gv, and
t=max(deg u,deg v). Put N=64-t, h=uv. For independent uniform word
polynomials U,V, their full PH difference is gT where
\[
 T=uV+vU+gh.
\]
T is uniform on all polynomials of degree <64+t. Given T, the first
product is a fixed polynomial plus
\[
                    S Z+hZ^2,\qquad S=T+gh,               \tag{10}
\]
where Z is uniform of degree <N. Also deg h<=2t and v2(h)<=t.

**Proof.** The kernel of uV+vU consists precisely of (U,V)=(uZ,vZ),
since u,v are coprime. Word-degree restrictions give deg Z<64-t.
The rank is 128-(64-t)=64+t, the dimension of the entire possible output
space, so the map is onto and uniform. For a particular solution U0,V0,
expand (U0+uZ)(V0+vZ). Its linear coefficient is
uV0+vU0=T+gh, proving (10). Coprimality makes at least one of u,v odd;
hence v2(h) is at most the degree of the other factor, at most t. ∎

If exactly one coordinate changes, the difference is dV with V uniform,
and conditional on V the first product is U odot V with U still uniform.
These cases are separate from (10).

**Lemma 4.2 (certifiable leading rows).** The following bounds apply to
selected output bits m of the linear map (10).

For low output bit n in {0,...,63}, let v be the valuation of S (64+t
when S=0), and let e_0,e_1 be the lowest even and odd set-bit positions
of h, with absent positions assigned infinity. Only use n<N+v. Its
multiplication term has largest input index n-v if n>=v, and its square
term has largest input index (n-e_(n mod 2))/2 when defined. If these
indices coincide, discard the row; otherwise the larger defined index
is its known largest nonzero input index. Rows with different such
indices are independent. For t<=11 all square indices in these low
rows are <N.

For a high bit n=64+j, use the degree d of S (-1 when S=0), and the
highest even and odd positions H_0,H_1 of h. Discard n<d. The known
smallest indices are n-d for multiplication, and (n-H_(n mod 2))/2
for the square term, including only indices in [0,N). Discard equality;
otherwise the smaller defined index is the known smallest nonzero
input index. Rows with distinct such indices are independent.

**Proof.** In the multiplication row the coefficient at input i is
S_(n-i); in the square row it is h_(n-2i). This gives the stated extreme
indices directly. Unknown intervening coefficients cannot change a
known uncancelled extreme. In a nontrivial linear relation among rows
with distinct extreme indices, take the largest (respectively smallest)
extreme occurring. Its coefficient cannot cancel. ∎

A rank lower bound k on the bits selected by m gives, for any fixed
translated pattern set, probability at most min(1,|P(m)|/2^k).

**Lemma 4.3 (finite low tables).** If the minimum nonzero PH valuation is
s=1,2,3, and both ENH increments are even, the primary probability is
less than 147/q. This permits arbitrary changes in all other chunks and tags.

**Proof and exact finite specification.** Select a PH chunk attaining s
and condition all other keys. Let C be the conditioned low XOR offset.
It is even. A possible total mask m must satisfy m=C modulo 2^s.

First assume both coordinates of this chunk differ. The gcd g has
valuation s. For m!=C put k=v2(m XOR C), v_T=k-s. The raw low target
determines T modulo X^(64-s), leaving 2^(s+t) full T values, each of
probability 2^(-64-t). If b=v2(h), the valuation of S in (10) is
min(v_T,s+b) when these differ; when equal, it belongs to
{s+b+1,...,64+t}, the final value including S=0.

For t<=9 form all pairs (e_0,e_1) of an even and an odd position <=2t,
allowing either to be absent, with min(e_0,e_1)<=t. They include every
possible h. For each m,k take the smallest leading-row rank from
Lemma 4.2 over these pairs and the stated possibilities for v(S).
Let w(m,k) be min(1,|P(m)|/2^rank). Assign weight one when m=C.
The conditional primary probability is at most
\[
 {2^s\over q}\sum_{m\in D,\ m\equiv C\ (2^s)}
   \begin{cases}1&m=C,\\ w(m,v_2(m\oplus C))&m\ne C.\end{cases}       \tag{11}
\]
The high projection was not used.

Here is a complete finite maximization over C, not a search over chosen
centres. For a set I of masks agreeing on the bits below k, partition it
into I_0,I_1 by bit k. The maximum remaining sum is
\[
 V(I,k)=\max_{b=0,1}\left(\sum_{m\in I_{1-b}}w(m,k)+V(I_b,k+1)\right).
                                                               \tag{12}
\]
An empty set contributes zero; at k=64 a singleton contributes one.
Start at k=s for each prefix a modulo 2^s. Only even a are needed.
The certificate checks every s=1,2,3 and t=0,...,9: (11)'s numerator
is <145. The slightly larger tables through t=11 are retained as checks.

For t>=10, use the whole-difference rank 64+t. After conditioning ENH
there are at most 248 even low masks and 604 high masks with the prescribed
top bit. Thus the numerator is at most
248*604/2^t <= 4681/32 <147. This includes every t up to 63.

For a one-coordinate change, put v=v2(V). Conditional low PH product bits
at and above v are independent uniform bits and lower bits are zero. Its
pattern probability is at most
\[
 w_L(m,v)={\max_z\#\{t\in P(m):t\bmod2^v=z\}
                 \over2^{\operatorname{popcount}(m\mathbin{>>}v)}}.
                                                               \tag{13}
\]
Use v=k-s for m!=C and one for m=C in (11)--(12). The complete tables
for s=1,2,3, including odd prefixes too, give maxima
78.75, 81.125, 84.53125. Each is <85. This proves the lemma in every
coordinate-change case. ∎

**Lemma 4.4 (finite high tables).** Suppose all PH and ENH low differences
are divisible by 16 and some PH chunk differs. The primary probability
is at most 151/q.

**Proof and exact finite specification.** A projected low equality must
be raw low equality by (4). Condition ENH and all but one differing PH
chunk. If both its coordinates differ and t>=2, a full prescribed PH
XOR difference has probability 2^(-64-t); at most 604 high masks are
possible once its low target and top bit are fixed. The bound is
604/(2^t q)<=151/q.

For t=0 or 1, use (10). Write D0=deg g, so 4<=D0<=63-t. The possible h
are 1 at t=0, and X,1+X,X+X^2 at t=1, because u,v are coprime and have
maximum degree one. Let C be the conditioned high XOR offset. Its top
bit is also the top bit of every possible mask m. For m!=C put
j=bitlength(m XOR C). Then
\[
 \deg T=64-D0+j-1.                                         \tag{14}
\]
If this is >=64+t the target is impossible and receives weight zero.
Otherwise deg S is the maximum of this degree and D0+deg h when unequal;
when equal it is any of -1,...,D0+deg h-1. Minimize the high leading-row
rank of Lemma 4.2 over the indicated h and degrees. Give m weight
min(1,|P(m)|/2^rank), and give m=C weight one.

Maximize the weight sum over every C using the analogue of (12), reading
bits from 62 down to zero, with weight indexed by j=k+1. Use both choices
of the fixed top bit. Finally divide the sum by 2^t, from the probability
2^(-64-t) of each full target. The exact tables cover all 119 pairs
(t,D0) (60 at t=0 and 59 at t=1), both top bits, and every m and j.
Their maximum is <27. Impossible degrees in (14) are explicitly removed.

For one changed coordinate with multiplying difference d, the possible
V for m!=C has degree 64-deg d+j-1, at least j. Conditional on nonzero V,
the high PH product is uniform on its lowest deg V bits and zero above.
Define
\[
 w_H(m,j)={\max_z\#\{t\in P(m):t\mathbin{>>}j=z\}
                \over2^{\operatorname{popcount}(m\mathbin\&(2^j-1))}}.
                                                               \tag{15}
\]
This is nonincreasing in j: freeing an additional selected bit at most
doubles the numerator and doubles the denominator. The same high trie
with weight w_H(m,j) therefore bounds the target sum, with one at m=C.
Its maxima at the two fixed top bits are 8 and 11.5. Every alternative
is below 151, proving the lemma. ∎

**Theorem 4.5.** Same-count blocks with at least one differing PH chunk
have primary probability <=151/q, allowing every other data or tag change.

**Proof.** An odd PH coordinate uses the imported 17/q result. Otherwise
all PH changes are even. An odd ENH increment uses Lemma 3.3's 17/q
argument, which allows arbitrary PH offsets. For minimum PH valuation
s=1,2,3 and even ENH use Lemma 4.3. For s>=4 and ENH valuation 1,2,3 use
Lemma 3.3. The remaining ENH valuations are >=4 (including no ENH change),
so Lemma 4.4 applies. These cases are disjoint and exhaustive. ∎

## 5. A sharper high ENH XOR count

**Lemma 5.1.** With two nonzero ENH increments, arbitrary tags, and h set
bits in m,
\[
 \Pr[Y=Y',\ U\oplus U'=m]
 \le {K(m)\over q},\quad
 K(m)=\min\{2^{h-m_{63}},\ 276\,2^{64-h},\
                      6(2^{\lceil h/2\rceil}+1)\}.          \tag{16}
\]

**Proof.** The first bound follows from the imported 1/q additive target
bound: the high XOR mask permits 2^(h-m_63) distinct signed differences
modulo q. The middle bound is the imported dense-mask result.

For the last bound, prescribe z=U&m. Low equality implies a fixed residue
of A'B'-AB modulo q^2. In a fixed operand-wrap rectangle the signed
increments a=A'-A and b=B'-B are nonzero and constant. The difference
is aB+bA+ab. The side lengths of the rectangle are q-|a| and q-|b|;
its difference range has length strictly less than
\[
 |a|(q-|b|)+|b|(q-|a|)\le q^2.
\]
The inequality follows on writing x=|a|/q,y=|b|/q in [0,1], since
x+y-2xy<=1. Consequently there is at most **one** integer difference
representing the prescribed residue in each rectangle.

On the resulting integer line, if a solution exists, parameterize
A=A0+(a/g)t, B=B0-(b/g)t with g=gcd(|a|,|b|). The product is a quadratic
with nonzero integer leading coefficient -ab/g^2. The restriction U&m=z
permits 2^(64-h) values of H, hence that many disjoint product intervals
of length q. In the two same-sign wrap rectangles use the two-sided
quadratic interval bound. In each of the two opposite-sign rectangles
A and B change in the same direction along the line. On A,B>=0 their
product lies on one monotone side of the quadratic's vertex; use the
one-sided bound. The sum of the side counts is 2+2+1+1=6.

There are 2^h patterns z. Summing yields
6(2^(h/2)+1)/q, bounded by the last expression in (16). Empty lines,
zero operands, endpoint points and tag wraps only remove possibilities.
There is no second integer-representative factor. ∎

## 6. ENH-only blocks with at least one common PH chunk

**Lemma 6.1 (forced-pattern PH weight).** With one independent common PH
product, for any fixed other offsets and a high XOR mask m, the conditional
probability of the high projection pattern is at most
\[
 W(m)=\min\left(1,{1\over q}+\sum_{j=0}^{63}{2^j\over q}w_H(m,j)\right),
                                                               \tag{17}
\]
where w_H is (15).

**Proof.** Translate the fixed chunk data into its two independent uniform
XOR-key operands. The first operand is zero with probability 1/q. If it
has degree j, probability 2^j/q, the high product is uniform on its lowest
j bits and zero above: its j output rows have distinct pivots in the
second operand. After the fixed XOR translation, only patterns with one
specified upper-bit assignment are possible. At most the numerator in
(15) survive. Average these conditional bounds. ∎

**Theorem 6.2.** If all PH chunks agree, at least one PH chunk exists,
and ENH data differ, primary probability is <205/q. For one changed ENH
word the high-valuation part is <134/q.

**Proof.** Small ENH valuations are covered by Lemma 3.3, with the common
PH offsets fixed; for r=4,...,7 raw low equality alone gives <=128/q.
For r>=4 projection forces Y=Y'. Fix a nonzero high mask m. Averaging over
the common PH product after the ENH keys gives bound K(m)W(m)/q for two
changed words, or kappa(m)W(m)/q for one. A separate conditioning on all
PH keys, followed by the imported lifting lemma for each pattern and
B-wrap, gives 2|P(m)|/q. Take the minimum of these two valid bounds.
The zero mask has probability <=1/q by the additive target bound.

The complete exact sums are
\[
 1+\sum_{m\in D\setminus\{0\}}\min(2|P(m)|,K(m)W(m))
  ={944062127676554878353\over4611686018427387904}<205,       \tag{18}
\]
\[
 1+\sum_{m\in D\setminus\{0\}}\min(2|P(m)|,\kappa(m)W(m))<134.
                                                               \tag{19}
\]
They use one common PH chunk; any additional common chunks are conditioned
into the fixed offset. All 851 nonzero terms, their weights and both
bounds are in `enh_certificate.json`. ∎

## 7. A final one-chunk ENH difference and the actual multiplier

This section concerns two messages whose preceding encoded blocks agree
exactly, and whose final blocks each consist of one ENH chunk. Assume their
final data differ. Their tags may differ too.

**Lemma 7.1 (necessary multiplier divisibility).** On projected final-block
identity and r>=4, let the two raw high outputs have difference jp,
-8<=j<=8. An actual full-hash collision requires 8 to divide fj.

**Proof.** Low projection forces raw low equality. The common preceding
blocks have identical literal accumulator values. The last reference
update, modulo q-8=8p, is
\[
 (f^2\bmod p)(\mathrm{acc}+Y)+f(U\oplus Y).
\]
Its difference is f times the high-word difference, namely fjp. Equality
modulo 8p is equivalent to 8|fj. The finalizer is bijective. ∎

For j!=0 define
\[
 d_j=2^{\max(0,3-v_2(j))},\quad
 n_j=\lfloor(p-1)/d_j\rfloor-\lfloor1/d_j\rfloor,
 \quad a_j=n_j/(p-2).                                     \tag{20}
\]
This is the exact probability that the multiplier meets that necessary
condition. For j=0 use one. The zero high mask, together with low equality,
is a fixed additive product target, of probability <=1/q.

**Lemma 7.2 (restricted-function count).** For every r>=8 the probability
of projected final-block identity **and actual collision**, jointly over
IID OH and the independent multiplier, is <435/q.

**Proof and complete finite count.** Use (6) with M_hi=0. Fix A, its
wrap alpha, and a possible B-wrap beta. Fix also a candidate residue
B0 modulo 16. Every such B has AB modulo 16 equal to l=A B0 modulo 16.
On inputs with these four low bits fixed, the affine function f_(m,t)
modulo R has the following sufficient, exact coefficient label:
\[
 \begin{split}
 b_m&=m\mathbin\&(R/2-1)\mathbin\&\mathord\sim15,\\
 b_t&=t\mathbin\&b_m\mathbin\&(R/4-1),\\
 c&=m-2((l\mathbin\&m)\oplus t)\pmod R.
 \end{split}                                               \tag{21}
\]
Indeed its constant is c. Its coefficient at bit i of the remaining
input is +/-2^(i+1), nonzero precisely at b_m; the sign is determined
by t_i except at i=r-2, where the two signs coincide. Higher coefficients
vanish. This proves (21) directly, including the last-sign convention.

The prefix B0 is itself uniquely determined by (6) modulo 16 for each
(m,t): its left coefficient d is odd and the varying right-hand term
has an extra factor 2Q. Determine its four bits in increasing order.
Group all nonzero mask/pattern pairs by (B0,b_m,b_t,c). Within one group
the full equation is identical on its allowed B prefix, so the imported
uniqueness lemma permits at most one full B. If a group contains several
j=(2t-m)/p, assign weight max n_j over its members. Any actual collision
in this group has multiplier probability at most that weight/(p-2).
Consequently the sum of group weights is a rigorous bound for fixed A
and wrap, not a collision count and not an independence assumption.

Here is the entire production-width enumeration, using only sixteen
operand residues and finitely many small wrap parameters.

* If Q>=16 (8<=r<=60), the prefix equation is dB+eA=0 modulo 16.
  Thus B0=uA modulo 16, u=-e/d modulo 16, independently of (m,t), and
  l=uA^2 modulo 16. Let G_r(l) be the sum of maximum n_j over labels
  (21) at this l, omitting m=0. Enumerate all l=0,...,15 and all u=0,...,15.
  Both A-wrap intervals have lengths divisible by 16. Allowing both
  B-wraps, a valid numerator is
  \[
   B_r=1+{2\over16(p-2)}\max_{0\le u<16}
                          \sum_{a=0}^{15}G_r(ua^2\bmod16). \tag{22}
  \]
  This also bounds the case eta=0, where only one B-wrap exists.
* For r=61,62,63, Q=8,4,2. Write delta=RD, eta=RE and enumerate every
  odd D in [1,Q), every E in [0,Q), and every tag gap modulo 16/Q.
  Enumerate alpha=0,1 and beta=0,1, except only beta=0 if E=0. Set
  d=D-alpha Q, e=E-beta Q. For every a=0,...,15 solve the four-bit
  prefix equation, form all labels (B0,b_m,b_t,c), and sum their
  maximum n_j; call this T(alpha,beta,a).
  The normalized length of the A-wrap interval is
  l_alpha=Q-D if alpha=0, and D otherwise. The exact upper numerator is
  \[
   B_r=1+{1\over16Q(p-2)}\max_{D,E,\Delta\tau\bmod16/Q}
            \sum_{\alpha,\beta,a}l_\alpha T(\alpha,\beta,a).       \tag{23}
  \]
  The A residues are uniform on each interval because R is divisible
  by 16. The four-bit equation only depends on the tag gap modulo 16/Q.
  Thus every full-width increment, tag and operand is covered.

The finite certificates compute all 56 values, not an extrapolated formula:

| r range | Certified strict upper bound on B_r |
|---|---:|
| 8,...,60 | 428 |
| 61 | 435 |
| 62 | 421 |
| 63 | 424 |

All are <435. The leading one in (22)--(23) is the separately bounded
zero high mask. For m!=0 the lifting function f_(m,t)(l) cannot equal
zero modulo R: its valuation is that of m, at most three, whereas r>=8.
The groups therefore omit no
zero-class ambiguity. Division by q for the remaining uniform B key,
and then averaging A and its wraps, proves the lemma. ∎

**Corollary 7.3.** For the message pair in this section, under distinct OH
words, the actual primary collision probability is at most S from (2).

**Proof.** At r=0,1,2,3 the no-PH projected bounds from the imported low
additive count are 17,18,20,24 in units 1/q. At r=4,...,7 low equality
has probability <=128/q. At r>=8 use Lemma 7.2. Thus the collision-and-
identity part is bounded by 435/q jointly over OH and multiplier. Dividing
by distinct-key acceptance gives 435/(q-561).

Outside identity, the common prefix cancels. The field comparison is the
nonzero polynomial f^2(Y-Y')+f((U XOR Y)-(U' XOR Y')), of degree at most
two, giving at most 2/(p-2) after averaging the OH keys. Adding proves S.
The term 435/(q-561) is a weighted full-hash event bound; it is not a
primary projection bound. ∎

## 8. Exhaustive all-message assembly

Let x!=y. Short/short pairs use their direct actual bound from Section 2.
Short/long pairs have identity probability <=9/(q-561). For two long
messages with unequal block counts, the leading coefficients of the
longer comparison give the imported <82/(q-561) identity bound.

Now suppose both are long and their block counts agree. Every nonfinal
block has sixteen chunks. The following decision list is exhaustive.

1. If a differing aligned expanded-block/tag tuple has unequal chunk
   counts, use the fresh-pair <82/q identity bound on that block.
2. If a differing aligned tuple with at least two chunks exists, use
   Theorem 4.5 if PH data differ, Theorem 6.2 if only ENH data differ,
   and the imported tag-only theorem if only its tag differs.
3. Otherwise every differing aligned tuple has one chunk. It must be
   the final block, since all preceding blocks have sixteen chunks.
   The preceding tuples therefore agree exactly. If its data agree,
   use the tag-only theorem; if its data differ, use Corollary 7.3.

Select each witnessing tuple from the fixed messages, before sampling keys.
Encoding injectivity guarantees a differing tuple. Formal identity requires
its two coefficients to agree; no independence of blocks using shared OH
keys is claimed. All cases except the last one-chunk data change have
identity probability at most A=205/(q-561).

For those ordinary cases the independent root bound gives
A+(1-A)rho(L), where each message has at most ceil(L/32) blocks. The special
case has the bound S. This proves (2). At L=1 only short messages occur.

For H=ceil(L/512), ceil(L/32)<=16H and H>=1. The ordinary term is at most
\[
 H\left({205\over q-561}+{32\over p-2}\right)
       <{58H\over2^{61}},                                 \tag{24}
\]
while S<57/2^61<=58H/2^61. Short/short is smaller. Exact rational checks give
\[
 57<2^{61}\left({205\over q-561}+{32\over p-2}\right)<58,
 \quad56<2^{61}S<57.                                      \tag{25}
\]
This proves (1), hence the published coefficient 64, for every L without an
upper length cap. The same argument for IID keys has smaller denominators.

For the score, ceil(L/32)<=L/2 for every L>=2. Hence the ordinary envelope
divided by L is at most A/2+(1-A)/(p-2), strictly less than S/2. The special
term divided by L is at most S/2, with equality at L=2. The floor 1/q and
clipping at one cannot make a worse ratio. At L=1 the ratio q-561 is larger.
Thus the exact score is log2(2/S). The integer-power checks
\[
 (2/S)^{50}>2^{2809},\qquad(2/S)^{100}<2^{5619}              \tag{26}
\]
prove the stated interval without a floating-point logarithm.

## 9. Certificates, checks and formalization boundaries

Run `bash checks/run_xeon.sh` from the project directory on the specified
Xeon. Every compilation and computation runs with `nice -n 10 taskset -c
40-47`, OMP_NUM_THREADS=8 and a 32,000,000,000-byte address-space cap. The
Mac is used only for editing, reading and file transfer. No sub-agents
or messages to third parties are used.

The exact finite computations are part of the proof as follows.

* `prepare_masks.py` enumerates (3) by the imported complete signed-digit
  recursion. Soundness plus the disjoint-pair identity
  sum_(m,t) 2^(64-popcount(m))=q+2 sum_(j=1)^8(q-jp)=8q+72 independently
  certifies completeness. `mask_certificate.json` lists every mask/pattern.
* `ph_low_table.cpp` implements exactly Lemmas 4.2--4.3, for all s=1,2,3,
  t=0,...,11, possible parity-leading positions and valuation cancellations,
  and all prefix classes. It computes (12) in dyadic integers of common
  denominator 2^64. The bound after taking the full-rank minimum is
  4681/32. `verify_ph_low.cpp` independently builds literal monomial
  matrix rows and maximizes over explicit centres: every listed mask and
  each mask with one bit flipped. These centres represent every occupied
  leaf and every first empty branch of the binary trie, so the maximum
  is complete. All relevant even-prefix entries agree exactly.
* `certify_ph_linear.py` and `certify_ph_high.py` evaluate the other
  complete tries, with the exact rank and degree rules in Section 4.
  `verify_certificates.py` independently constructs high linearized rows
  by multiplying basis monomials, computes their extreme indices, and
  reproduces every high table entry.
* `certify_enh_weights.py` evaluates all terms of (18)--(19) as rational
  numbers. The independent reader recounts the compatible upper patterns,
  recomputes every K and kappa and sums the fractions again.
* `certify_tail.py` computes all contexts in (22)--(23). The independent
  reader uses literal affine-function evaluations on zero and basis inputs
  instead of labels (21), and tries all sixteen B residues instead of the
  bit-lifting solver. It reconstructs every table and every maximum.
* `validate_new.cpp` independently enumerates all even increment pairs
  of minimum valuation 1,2,3 and all operand pairs at widths 4,...,8.
  It checks the strengthened XOR counts, dense bounds and joint point
  multiplicities. It also constructs literal carryless linearized maps
  at widths 4,...,8 with the stated N>w/2 hypothesis, checks every retained
  extreme row, and checks ranks on the complete scaled mask sets for the
  declared S profiles. It verifies the wrap-rectangle range inequality
  over all nonzero increments at widths 2,...,6. These scaled checks
  validate algebra and implementations; they are not extrapolated counts.
* `certify_envelope.py` checks (25)--(26) with exact rational/integer
  arithmetic, the complete case maxima, and additional length boundaries.
  Section 8, rather than a finite length search, covers all L.

The final scopes, visit counts and zero-failure status are in
`validation.json` and `independent_verification.json`; `manifest.json`
records source hashes and the execution limits. The exploratory `probe_*`
files are not proof dependencies.

The proof establishes the ideal-key primary headline and the improved
coefficient 58. It does not establish `PrimaryIdentityBound(162/(q-561))`:
in particular the unweighted one-chunk ENH projection and the reduced
short/short constant-polynomial issue are not silently replaced by actual
collision estimates. A Lean formalization should first formalize the new
rank/table and restricted-function lemmas, then the literal modulo-8p
suffix lemma, and finally the decision list in Section 8. No old open
premise is used as an axiom in this argument.
