# ChainHash-128

ChainHash-128 uses the same three levels as `appendix_chainhash.tex`, with
128-bit words and a 128-bit result. The default is **512-byte blocks, W=32,
S=1**. A fully specified 256-byte variant uses W=16, S=1. The block size is a
family parameter: the two configurations need not return the same hash.
`REPORT.md` compares them and derives the throughput and recurrence ceilings.

## Definition in the paper's notation

Let R=F₂[X], R<d its polynomials of degree below d, and

\[
 \Pi=X^{128}+X^7+X^2+X+1,\quad F=R/(\Pi),\quad q=|F|=2^{128}.
\]

Bit i is the coefficient of X^i. A word is 16 bytes, least significant byte
first; multiplication in R is carry-less multiplication, and multiplication
in F is that product reduced modulo Pi. This is the GCM polynomial, with
reduction constant 0x87. Our byte/bit encoding follows integer little endian;
it is **not** the external GHASH bit-string convention. See [NIST SP 800-38D,
§6.3](https://nvlpubs.nist.gov/nistpubs/legacy/sp/nistspecialpublication800-38d.pdf).
`tests/irreducibility.json` supplies a separately checked Rabin certificate.

Write B=16W, W_s=W/S, B_s=B/S; S=1 in both implementations. The paper's more
general S formulation is valid when W_s is divisible by four. The ideal key is

\[
 \kappa=(k_0,\ldots,k_{W-1})\in F^W,\quad
 \theta=(u,y,z)\in F^3,\quad c=(c_0,\ldots,c_4)\in F^5,\quad\tau\in F,
\]

all independent uniform, **including zero**. It has W+9 words: 656 bytes for
512-byte blocks and 400 bytes for 256-byte blocks. Key words are in exactly the
order displayed. Key setup is excluded from the speed measurements.

A message m is a byte string of length 0≤ell<q. Define

\[
 n(m)=\max(1,\lceil\ell/B\rceil),\quad p(m)=Sn(m),\quad
 r_t=\min(B_s,\max(0,\ell-(t-1)B_s)),\quad G_t=\lceil r_t/64\rceil.
\]

Pad the r_t existing bytes of sub-block t with zeros to 64G_t bytes, decode
4G_t words omega, and apply the involution pi swapping positions 4j+1 and
4j+2. Thus m_t=pi(omega) has w_t=2G_t pairs. The same permutation applies to
the key segment for sub-block position i(t)=(t-1) mod S. In original position
order the products for group j are

\[
 (\omega_{4j}+k_{iW_s+4j})(\omega_{4j+2}+k_{iW_s+4j+2})+
 (\omega_{4j+1}+k_{iW_s+4j+1})(\omega_{4j+3}+k_{iW_s+4j+3}).
\]

These are **unreduced 128×128→256-bit products**, XOR-accumulated in R<256;
in fact their degree is at most 254. Equivalently,

\[
 C_t=\operatorname{CLNH}_{\kappa^{(i(t))}}(m_t)
     =\sum_{s<w_t}(g_{2s}+k'_{2s})(g_{2s+1}+k'_{2s+1}).
\]

No pair is evaluated for an empty sub-block. Every nonempty 64-byte group
evaluates both strided pairs, including pairs consisting entirely of padding.
For 1–16 bytes the second product is key-only. Split C_t=lo(C_t)+X^128 hi(C_t).
Interpreting the byte length as a 128-bit word, form

\[
 a_t=\operatorname{lo}(C_t)+[t=p]\ell,\qquad
 b_t=\operatorname{hi}(C_t)+[t=p]\ell.
\]

The length enters **both 128-bit halves of the final stream pair**, exactly
once. The empty message has one empty block and length zero.

The three-key recurrence and finalizer are unchanged:

\[
 P_0=z,\quad P_t=a_t+(b_t+y)(P_{t-1}+u),\quad V=P_p,
\]
\[
 v=V\boxplus\tau,\quad G_1=v^2,\quad
 G_2=(G_1+c_0)(v+G_1+c_1),\quad
 H(m)=(v+c_2)(G_2+c_3)+c_4.
\]

Here + is XOR, products are in F, and boxplus is **integer addition modulo
2^128**, including the carry from the low 64 bits into the high 64 bits.
The complete result is serialized as 16 little-endian bytes. This is a keyed
almost-universal family, not a cryptographic digest or a MAC.

The C API takes size_t lengths on ordinary 32/64-bit targets, so it implements
the subdomain ell<2^64 with the high length limb zero. The mathematical
128-bit length domain is larger than the representable C input domain. The
implementation subtracts remaining bytes; it never computes len+B-1 and has
no block-count addition overflow. Input may be NULL only when len=0; key must
be valid. No input alignment or readable padding is required.

## Ideal-key collision theorem

For fixed distinct messages m,m' independent of the key, with lengths below
q and n(m),n(m')≤n, n≥1,

\[
 \boxed{\Pr[H(m)=H(m')]\le\min\{1,(Sn+2)/2^{128}\}.}
\]

In the default S=1 case this is
(max(1,ceil(ell_max/512))+2)/2^128. At 512 B, 1 KiB and 1 MiB the numerators
are 3, 4 and 2050. At 256-byte blocks they are 4, 6 and 4098.

For equal stream lengths p, the stream, recurrence and finalizer contribute
at most 1/q, p/q and 1/q. For unequal stream lengths, the streams are always
different; the recurrence contributes at most (max(p,p')+1)/q and the
finalizer 1/q. Both cases give the displayed **+2** constant. Reuse of PH key
words across blocks costs no union-bound factor: stream equality implies
any one selected nontrivial block equation.

### Why raw PH has 2^-128, not 2^-64 or 2^-256, universality

For equal pair counts and different data, expand the XOR difference in R.
The key-key terms cancel. A differing word delta≠0 multiplies its partner's
**whole 128-bit key word** k. Condition on every other key word. A target
equation is delta*k=C'. Because R is an integral domain, multiplication by
delta is injective: at most one of q choices of k works. Thus the raw,
256-bit-output family is 1/q-XOR-universal. The 64-bit physical instruction
lanes do not change the sampling space. The bound can be attained, e.g. a
single differing multiplicand and the zero target.

For nested, unequal pair counts and a fixed **nonzero** target C, use the
same induction as `lem:ph:clnh(ii)`. Expose one new pair A*B. A is zero with
probability 1/q; otherwise at most one B works. If the previous target atom
has mass at most 1/q, the new mass is at most

\[
 (1-1/q)/q+(1/q)(1/q)=1/q.
\]

The induction starts either with the equal-count lemma or an impossible
0=C. The restriction C≠0 is essential: one fresh product is zero with
probability 2/q-1/q². For different byte lengths the stream equation supplies
C=(ell XOR ell')(1+X^128)≠0. This argument is word-width generic, rather than
a new assumption about 128-bit CLMUL.

### Recurrence, finalizer and twist

The formal stream decoder in `lem:ph:injective` works over any field of
characteristic two. Writing P_p=A(Y)+Z B(Y)+U C(Y), its monic suffix-product
pivots recover the ordered stream. The universal leading homogeneous part
is (Z+U)Y^p. Equal-length differences therefore have total degree ≤p;
unequal-length differences have degree max(p,p')+1. The field-generic
Schwartz–Zippel count with three independent uniform keys supplies the
bounds above. Injectivity here is of the **formal stream-to-polynomial map**;
it does not assert injectivity at each sampled key.

The quintic coefficient bijection also carries over without new algebra.
Writing b=c0+c1 and d=c0*c1, the monic coefficients are

\[
 e_4=1+c_2,\ e_3=b+c_2,\ e_2=c_0+c_2b,
 \quad e_1=d+c_3+c_0c_2,\quad e_0=c_4+c_2(d+c_3).
\]

An explicit inverse is

\[
 c_2=e_4+1,\quad b=e_3+c_2,\quad c_0=e_2+c_2b,\quad
 c_1=b+c_0,\quad d=c_0c_1,\quad
 c_3=e_1+d+c_0c_2,\quad c_4=e_0+c_2(d+c_3).
\]

All pivots are one. No characteristic-zero theorem or Frobenius-root
assumption is invoked. Consequently c uniform on F^5 gives a uniform monic
quintic, exact collision probability 1/q at two distinct inputs, and exact
five-wise independence at five distinct inputs. For the entire hash,
five-wise independence is conditional on distinct upstream V values;
unconditional independence is only approximate, with the usual union bound
on upstream pair collisions.

For each fixed tau, integer translation in Z/(2^128) is a bijection, with
inverse subtraction of tau. Transfer through the bit/field representation
preserves this fact. After conditioning on upstream keys and tau, c is still
uniform, so the twist preserves both finalizer statements. The two-limb carry
implementation needs verification; the mathematical argument is identical.

## Key model A: one field seed for PH only

Sample s,u,y,z,c0,…,c4,tau independently and uniformly in F (160 bytes), and
set **k_i=s^(i+1)**, 0≤i<W, using field multiplication. The expanded key
remains W+9 words. This is `chainhash128_key_from_bytes`. The ideal-key theorem
does not apply to this correlated PH key; the following separate bounds do.

For a selected necessary stream equation, reducing the raw equality modulo
Pi yields a nonzero polynomial in s. Word differences at positions
(4j,4j+1,4j+2,4j+3) multiply exponents (4j+3,4j+4,4j+1,4j+2), respectively.
With identical pair counts the degree is at most W. With different group
counts, the largest active group count g leaves a unique monic key-only term
s^(8g-2); degree ≤2W-2. A raw equality implies this reduced equality; the
converse is never required. The length mask reduces to
(ell XOR ell')*(1+0x87)=(ell XOR ell')*0x86, nonzero for different lengths.
Thus the proof in `SEEDED_THEOREMS.md`, model A, transfers.

A convenient all-length envelope, n=max(n(m),n(m')), is

\[
 \boxed{\varepsilon_A\le\min(1,(n+2W-1)/q).}
\]

For unequal block counts a sharper (n+2)/q bound holds; for common block count
n the PH contribution is at most (2W-2)/q, then n/q and 1/q. The default is
(n+63)/q; the 256-byte configuration is (n+31)/q. The following more precise
bounds matter for the minimum-over-length score.

Let L≥1 count **16-byte words**, both lengths at most 16L<q,
n=ceil(L/W), R=L-W(n-1), G=ceil(R/4). Define

\[
 E_A(L)=\begin{cases}8G,&n=1,\\
 n+2W-2+[R\ge W-3],&n\ge2.
 \end{cases}
\]

Then epsilon_A^≤(L)=min(1,E_A(L)/q) is valid for arbitrary byte lengths up to
16L, including the empty message. For n≥2 this takes the maximum over the
last partial block and an earlier full block with a smaller common block
count, as in the supplied proof. In particular the one-block constant is
8ceil(L/4), not the fixed-length constant.

For both lengths **exactly** 16L, put h=min(L,W). Define d_W(1)=1, d_W(2)=2;
for L≥3 use 4floor((h-1)/4)+3 if h≡1 mod4, and
4floor((h-1)/4)+4 otherwise. Then

\[
 \varepsilon_A^=(L)\le\min(1,(d_W(L)+n+1)/q).
\]

The one-word PH difference is delta*s³ as a raw product and vanishes only
at s=0; two words reduce to s³(delta0+delta1*s) and have at most two distinct
roots. These explain d=1,2. Do not substitute these refinements for unequal
lengths. Neither a deterministic 128-bit seed expanding the entire key nor
the benchmark's SplitMix64 expansion supplies the required independent
recurrence and five finalizer words. No such seeded guarantee is claimed.

## Comparison with two independent ChainHash-64 instances and scores

Concatenating two **independently keyed** ideal ChainHash-64 outputs gives
collision probability at most ((Sn+2)/2^64)², by independence of the two
collision events. Its proof is immediate and it performs two full hashes
(about 2× work; actual timing also reflects instruction overlap and overhead).
Both instances need separate PH, recurrence, finalizer and twist randomness.
Reusing a seed or just adding a different finalizer does not justify squaring
the bound. Its quadratic growth with n makes the all-length score much worse
than the field-128 construction, despite the same 128-bit output width.

A score below uses the stated **upper-bound certificate**, not a proof of an
attaining worst pair. With L positive 16-byte words, the ideal score
min_L log2(L/epsilon(L)) is **128-log2(3)=126.4150374993 bits**, minimized at
L=1. Model A has **126.4150374993** for fixed lengths and **125** for arbitrary
lengths up to 16L, again minimized at 1 using the refined envelopes. For
n=1, E_A=8ceil(L/4)≤8L; for n≥2, E_A≤8L; equality in E_A/L occurs at L=1.
Using only the coarse (n+63)/q envelope would report 122 bits, unnecessarily
losing the short-length refinement.

For direct comparison to the supplied 64-bit write-up use its common unit
**L=8-byte words** and 1≤L≤2^61-1 (byte length <2^64). The ideal and model-A
at-most scores above are unchanged: use E_A(ceil(L/2)); its numerator is ≤8L
and equals 8 at L=1. ChainHash-64's ideal score is 64-log2(3)=62.4150374993.
For two independent ideal 256-byte-block ChainHash-64 hashes, the certificate
score is

\[
 \min_{1\le L\le L_{max}}
 [128+\log_2 L-2\log_2(\lceil L/32\rceil+2)].
\]

It is about **77 bits** on this full common domain, with the last block-start
L=2^61-31 minimizing (the score differs from 77 by less than 2e-15). Within
each block the expression increases with L, and the block-start values
increase initially and then decrease, so the minimum is at the first or last
block start. The legacy implementation's stricter cap L≤2^61-32 gives the
previous block start, L=2^61-63, and the same displayed 77 bits. A restricted
maximum message length must be specified for any higher score for the doubled
construction. Squaring epsilon does **not** double this normalized score.

For a concrete restricted domain of at most 1 MiB, L_max=131072. The last
block start L=131041 has n=4096 and minimizes the doubled certificate, giving
128+log2(131041)-2log2(4098)=**120.99825 bits**. The ideal ChainHash-128
score on this same domain is still 126.4150374993.

## Proof transfer and required Lean work

| Component | What transfers | What must be instantiated or checked |
|---|---|---|
| Raw CLNH | Integral-domain conditioning and fresh-pair induction; Lean `clnh_difference_bound` and `clnh_nested_nonzero_bound` are parametric in word width | Instantiate b=128; raw degree ≤254 and lossless split below 256, not a reduced-NH substitution |
| Byte stream | Padded encoding, selected block equation and length-mask argument | 16-byte words, 64-byte groups, W=16/32, 128-bit length injectivity and empty-block convention |
| Recurrence | Ordered formal decoder, equal-length leading-term cancellation, Schwartz–Zippel | Field cardinality q=2^128 and p=Sn; no new decoder |
| Finalizer | Characteristic-two coefficient map, explicit inverse, interpolation and 1/q collision | Instantiate the existing field-generic results; the circuit is unchanged |
| Twist | Translation in integer residues is bijective | Word128↔ZMod(2^128) correspondence and the carry between two UInt64 limbs |
| Concrete field | Polynomial quotient representation and reduction framework | Prove this Pi irreducible, degree 128 and cardinality; prove 0x87 reduction correspondence |
| Key layout/composition | Independent product measure and composition cases | Key25 or Key41 of Word128; 400/656 ideal bytes; C API length subdomain |
| Model A | Supplied reduced necessary-equation/root-count argument | A separate seeded theorem; ideal-key independence does not cover it |

For Pi of degree 128, Rabin needs X^(2^128)=X mod Pi and
GCD(X^(2^64)-X,Pi)=1, since 2 is the only prime divisor of 128. The delivered
certificate includes all 128 squaring residues and Bézout witnesses. A Lean
extension should generate ordinary ring identities and check them in the
kernel, following `BinaryRabin.lean`/`Modulus*.lean`; Python success is not a
Lean proof. Parameterize or duplicate `ConcreteWords`, byte encoders, stream
splitting and `ReferenceChainHash`, then specialize the composition theorem.
Audit the final byte-level theorem and its axioms as for ChainHash-64.

**Status:** the argument above proves the mathematical generalization; no
new Lean theorem or compiler/SIMD correctness proof is claimed. `LEAN_CHAINHASH_STATUS.md`
is evidence for the 64-bit theorem and reusable lemmas, not machine checking
of this 128-bit header. Backend equivalence is supported by the delivered
cross-architecture tests, independent oracle and memory-safety checks.
