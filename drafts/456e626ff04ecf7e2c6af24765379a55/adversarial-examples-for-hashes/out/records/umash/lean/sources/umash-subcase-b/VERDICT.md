# Independent verification of the third UMASH review

**The hard-subcase theorem and all four additional cases are CONFIRMED.** Every integer and every printed four-decimal ratio in the review's table agrees, including the exact maximum

\[
 B=\frac{345763417}{2^{116}}<2^{-87},\qquad -\log_2 B=87.63479000611594\ldots.
\]

The independently derived single-word bound is stronger:

\[
 \Pr[\text{primary reduced collision}]
 \le \frac8q+\frac{128}{q^2}<\frac9q<\frac{65}q.
\]

There are two proof clarifications. The **attainable** shuffler parameter is `2 <= h <= 15`; the review's inclusion of `h=1` is harmless. Its stated conditioning argument directly preserves the rounded `< 2^-87` conclusion, but does not by itself preserve the exact constant `B`. Removing a double count in the same argument supplies sufficient slack to preserve **that exact constant too** under the reference generator's rejection of repeated words. Details appear below.

## Scope and claim ledger

Here `q=2^64`, `p=2^61-1`; messages, tags, and seeds are fixed independently of the keys. Except for the explicit distinct-word transfer, probabilities use 34 independent uniform OH words. Reduction means reducing **each 64-bit lane** modulo `p`. These are compressor-level statements, not a completion of every end-to-end UMASH case or a theorem about a short seeded key expansion.

| Claim | Verdict | Exact result |
|---|---|---|
| Restricted carryless multiplication rank | CONFIRMED | Rank at least `w-h`, including `h=0,w` |
| Shuffler kernel | CONFIRMED | `ker(s_i+s_j)=ker T^h`; actual range `2..15` |
| Raw joint point bound | CONFIRMED | `2^-(128-h)`, worst `2^-113` |
| Difference-mask set | CONFIRMED | `|D|=852` |
| All `N_h,S_h` and printed table ratios | CONFIRMED | All 15 rows agree to their last printed digit |
| Product bounds (4), (5), and `rho_h` | CONFIRMED | Both product bounds are attained at zero |
| Equal-checksum fresh-product claim | CONFIRMED | Conditional and unconditional independence from the original chunk keys in the IID model |
| Low-bit target constraint, including ENH | CONFIRMED | Constant includes `(I+T) Delta_ENH`; it need not be zero |
| Hard reduced joint bound, IID | CONFIRMED | `345763417/2^116` |
| Same exact hard bound with distinct OH words | CONFIRMED, with the conditioning proof supplied below | Sharper IID bound `1381533379/2^118` absorbs the conditioning factor |
| Different counts: primary | CONFIRMED | `81(2q-1)/q^2 < 162/q` |
| Different counts: joint | CONFIRMED | `< 117596449/q^2 < 2^-101` |
| Different checksums, a differing PH chunk | CONFIRMED | Primary `<=725904/q`; joint `<=526936617216/q^2 < 2^-89` |
| An odd component difference in a PH chunk | CONFIRMED | Primary `<=17/q`; exhaustive toy low-lane maxima are 16 |
| One changed word in one PH chunk, everything else identical | CONFIRMED, strengthened | `<=8/q+128/q^2 <9/q`, hence `<65/q` |
| Extension of the 17-key conditional bound to even differences | REFUTED | Full two-lane examples have 18 keys at `(w,p)=(8,31)` and 24 at `(12,509)` |

The last row is the requested control experiment, **not a claim made by the review**. No reviewed numerical upper bound is refuted. All additional-case constants in the ledger are IID statements; an unconditional general transfer to distinct OH words divides an IID bound by the acceptance probability.

## 1. Rank lemma and raw joint bound

Use binary polynomials with indeterminate `t`; `⊙` is their unreduced product. Write `d=t^v g`, where `g(0)=1` and `0<=v<w`. Keep product positions

`0,...,w-h-1` and `w,...,2w-h-1`.

Set `r0=max(w-h-v,0)` and `r1=min(v,w-h)`, so `r0+r1=w-h`. Select these input coefficients of `z`:

* positions `0,...,r0-1`, with output rows `v,...,v+r0-1` in the low lane;
* positions `w-v,...,w-v+r1-1`, with output rows `w,...,w+r1-1` in the high lane.

The second group contributes zero to the selected low rows. In each diagonal block, multiplication by the constant coefficient 1 of `g` gives a triangular matrix with unit diagonal. The combined minor is nonsingular. This proves rank at least `w-h`, with no assumption about the other coefficients of `d`. Applying `T^h`, a left shift within each lane, has exactly the same rank as this restriction.

**Exhaustive check:** independent binary Gaussian elimination checked every nonzero `d` and every `h=0,...,w`, for `w=8,12,16`: **1,169,625 `(w,d,h)` cases**, zero failures. All minima equal `w-h` (for example `d=1` attains it). The JSON files include the complete rank histograms, rather than just pass flags: [rank-8](verification/results/rank-8.json), [rank-12](verification/results/rank-12.json), [rank-16](verification/results/rank-16.json).

### Actual shufflers

Number the `n` original chunks `1,...,n`, with PH positions `1,...,n-1` and ENH position `n`. The reference calls `shuffle(mixed_chunk,i+1,n)`; its executable definition gives

\[
 s_r=T+T^{n-r}\ (r<n-1),\quad s_{n-1}=T,\quad s_n=I.
\]

See [reference shuffle and compressor](umash_reference.py#L1112) and the production accumulation in [umash.c](umash-src/umash.c#L570). `v128_shift` is lane-wise addition of a vector to itself, so there is **no cross-lane carry**.

For `i<j<n-1`,

\[
 s_i+s_j=T^{n-j}(I+T^{j-i});\qquad h=n-j.
\]

For `j=n-1`, `s_i+s_j=T^{n-i}`, and `h=n-i`. The map `I+T^a` is invertible, with inverse the finite geometric series in the nilpotent `T^a`. It commutes with `T`; hence

`T^h(I+T^a)x=(I+T^a)T^h x=0` iff `T^h x=0`.

Thus the kernels are literally equal, not merely equal in dimension. For a pair of distinct PH positions and `n<=16`, **`h` ranges exactly over `2,...,15`**. The review's larger stated interval `1,...,15` remains a valid enclosure.

All **560** triples `(n,i,j)` were checked against the imported reference on all 128 basis vectors. Each computed rank is `128-2h`; every basis vector of `ker T^h` is annihilated. See [shufflers.json](verification/results/shufflers.json).

### Joint raw probability

A differing PH chunk has difference

`Delta = d_a⊙K_b XOR d_b⊙K_a XOR constant`.

Choose a nonzero component difference, leave its opposite key word free, and condition on the companion word. Multiplication by a nonzero polynomial is injective, so every specified full `Delta` has probability at most `2^-64`. The rank lemma bounds every specified `T^h Delta` by `2^-(64-h)`.

Condition on the companion words of two differing PH chunks and all other original chunk keys. Equal checksums cancel the twisting product in the raw secondary difference. For prescribed primary/secondary XOR differences `(A0,B0)`, eliminating one chunk gives one prescribed value of `(s_i+s_j)Delta_j`, which costs at most `2^-(64-h)`. For each surviving `Delta_j`, the primary equation fixes `Delta_i`, costing at most `2^-64` independently. Consequently

\[
 \Pr[A=A_0,B=B_0]\le 2^{-(128-h)}.
\]

This is a point bound on **differences**. It is not an unsupported projection-fibre multiplier applied to an AU equality bound.

## 2. Independent mask count and exact table

Define

`D={x XOR y: 0<=x,y<q, x≡y (mod p)}`.

To enumerate it I used ordinary binary **addition**, not GAP.md's signed-digit recurrence. For each `j=1,...,8`, run an automaton for `y=x+jp`, branching on each bit of `x`. Its state records the partial XOR mask and carry. If the current constant bit is `b`, input bit is `a`, and incoming carry is `c`, the output bit is `(a+b+c) mod 2`, the XOR-mask bit is its XOR with `a`, and the next carry is `floor((a+b+c)/2)`. Merge identical states, retaining one realizing `x`. After 64 bits reject overflow. Every legal addition follows a branch; every accepted branch gives a legal pair. Reversing a pair covers negative multiples; add mask zero separately.

The distinct-mask counts for `j=1,...,8`, **before union**, are

`184, 123, 361, 62, 355, 121, 178, 1`.

The union with zero contains **852** masks. [masks.json](verification/results/masks.json) records every mask with a realizing pair, plus automaton state counts. The automaton was cross-checked against direct congruent-word-pair enumeration at `(8,31)`, `(12,509)`, and the auxiliary `(12,511)`, obtaining **68, 231, 124** masks respectively. The last modulus is only an automaton control; none of the requested prime-509 tests substitutes 511.

### Exact definitions

For `h>=5`, a mask `v` is **long** iff every bit numbered `3,...,h-1` is 1:

`v & (2^h-8) == 2^h-8`.

There are no long masks for `h<5`. The secondary difference is the variable tested for longness. Define, for **every** `c=0,...,2^h-1`,

\[
 n_h(c)=\#\{(u,v)\in D^2:(v\oplus(u\ll1))\mathbin{\&}(2^h-1)=c\},
\]

\[
 s_h(c)=\#\{(u,v)\in D^2:v\text{ not long},\ (v\oplus(u\ll1))\mathbin{\&}(2^h-1)=c\},
\quad N_h=\max_c n_h(c),\quad S_h=\max_c s_h(c).
\]

Here `u` is one lane of the primary XOR difference and `v` the corresponding lane of the secondary XOR difference. This is a **bit constraint**, not the integer congruence `v-2u≡c`: bit zero is `v_0=c_0`, and bit `r>=1` is `v_r XOR u_(r-1)=c_r`. Each lane has its own possibly different `c`.

Every ordered pair of masks was counted directly, for all 15 `h`. The following ratios use exactly the review's combination

\[
 B_h=2^{-(128-h)}\min\{N_h^2,S_h^2+\rho_h N_h^2\}.
\]

| h | N_h | S_h | rho_h | Computed B_h / 2^-87 | Review, 4 decimals | Comparison |
|---:|---:|---:|---:|---:|---:|---|
| 1 | 514608 | 514608 | 1 | 0.2408536544535309 | 0.2409 | exact agreement |
| 2 | 278664 | 278664 | 1 | 0.1412511208327487 | 0.1413 | exact agreement |
| 3 | 153492 | 153492 | 1 | 0.0857100315042771 | 0.0857 | exact agreement |
| 4 | 104060 | 104060 | 1 | 0.0787875876994804 | 0.0788 | exact agreement |
| 5 | 99768 | 2966 | 1 | 0.1448447266593575 | 0.1448 | exact agreement |
| 6 | 96425 | 2916 | 1 | 0.2706010309339035 | 0.2706 | exact agreement |
| 7 | 93140 | 2866 | 1 | 0.5049549276009202 | 0.5050 | exact agreement |
| 8 | 89913 | 2816 | 9/16 | 0.5303154889916186 | 0.5303 | exact agreement |
| 9 | 86744 | 2766 | 5/16 | 0.5492623280733824 | 0.5493 | exact agreement |
| 10 | 83633 | 2716 | 11/64 | 0.5632418583481922 | 0.5632 | exact agreement |
| 11 | 80580 | 2666 | 3/32 | 0.5735448501072824 | 0.5735 | exact agreement |
| 12 | 77585 | 2616 | 13/256 | 0.5821096686195233 | 0.5821 | exact agreement |
| 13 | 74648 | 2566 | 7/256 | 0.5921445330604911 | 0.5921 | exact agreement |
| 14 | 71769 | 2516 | 15/1024 | 0.6093179585805046 | 0.6093 | exact agreement |
| 15 | 68948 | 2466 | 1/128 | 0.6440345514565706 | 0.6440 | exact agreement |

Every `N_h` maximum occurs at `c=1`. For `h>=5`, the `S_h` maximum occurs at `c=2^h-7`; these need not occur at the same `c`, and taking their separate maxima is conservative. Exact rational bounds and comparisons are in [table.json](verification/results/table.json); **all** per-`c` counts are in [target-histograms.json](verification/results/target-histograms.json). Decimal columns are displays; the fractions are the certificates.

The maximum is at `h=15`:

\[
 (2466^2+68948^2/128)2^{-113}
 =345763417/2^{116}.
\]

## 3. Product probabilities, long masks, and the two structural claims

### Product bounds (4) and (5), independently proved

Let `X,Y` be independent uniform `w`-bit binary polynomials, `Z=X⊙Y`, and `0<=k<=w`.

**(4), low lane.** For `0<=v<k`, the event `v_t(X)=v` has probability `2^(-v-1)`. Conditional on it, the low `k` product bits are uniform on the subspace divisible by `t^v`, so each point has probability at most `2^-(k-v)`. The event `t^k|X` has probability `2^-k` and makes these product bits zero. Thus

\[
 \max_z\Pr[Z_{lo}\bmod2^k=z]
 \le \sum_{v=0}^{k-1}2^{-v-1}2^{-(k-v)}+2^{-k}
 =(k+2)2^{-(k+1)}. \tag{4}
\]

Every conditional image contains zero, so equality holds there.

**(5), high lane.** If `deg X=m`, multiplication by `X` followed by keeping product positions `w,...,w+k-1` has rank `min(m,k)`. Its nonzero rows are independent by triangularity using the leading coefficient of `X`. There are `2^m` such `X`; the zero polynomial contributes separately. Consequently

\[
 \max_z\Pr[Z_{hi}\bmod2^k=z]
 \le \frac1{2^w}+\sum_{m=0}^{k-1}\frac{2^m}{2^w}2^{-m}
       +\frac{2^w-2^k}{2^w}2^{-k}
 =2^{-k}+\frac{k}{2^w}. \tag{5}
\]

Again equality holds at zero. “Low `k` bits of the high lane” means product positions **`w,...,w+k-1`**, not the most significant `k` product bits.

The implementation enumerated **all 65,536 ordered factor pairs** at `w=8` and **all 16,777,216** at `w=12`. For every `k=0,...,w`, it computed every output-bin count, and checked exact equality of the maximum to (4) and (5), attained at zero. Full histograms are in [products-8.json](verification/results/products-8.json) and [products-12.json](verification/results/products-12.json). These two bounds do not depend on the reduction prime.

### Why long masks give rho_h

If a word `x` collides with `x XOR d` modulo `p`, then for some integer `j` with `|j|<=8`,

\[
 2(x\mathbin{\&}d)=d+jp.
\]

For a long `d`, its low `h` bits are in `[2^h-8,2^h-1]`. Since `p≡-1 (mod 2^h)` for the `h<=15` under consideration, the low-bit equation is `2(x&d)≡d-j`. The integer `d_low-j` lies in `[2^h-16,2^h+7]`. After division by two modulo `2^(h-1)`, bits `3,...,h-2` of `x&d`, hence of `x`, must be **all zero or all one**. This follows for either sign of `j`. The certificate also checks every legal differing-bit assignment for every long 64-bit mask and each `h=5,...,15`.

Conditional on the original chunk keys, one secondary lane is a fixed XOR translate of `Z_lo` or `Z_hi`. The two possible patterns on bits `3,...,h-2` leave at most **16** possibilities for its low `h-1` bits: two patterns times eight free bottom bits. Using (4) with `k=h-1` gives `16(h+1)2^-h=(h+1)2^(4-h)`. Using (5) gives

`16(2^(1-h)+(h-1)/q) <= (h+1)2^(4-h)`,

because `q>=2^h`. Therefore

\[
 \rho_h=\min\{1,(h+1)2^{4-h}\}.
\]

### Structural claim (a): the checksum product really is fresh

In [umash_reference.py](umash_reference.py#L1089), the loop computes

`lrc = XOR_r (message_chunk_r XOR original_key_pair_r)`

**before** ENH's additive translation. The secondary appends

`gfmul(lrc[0] XOR key[-2], lrc[1] XOR key[-1])`.

In [umash.c](umash-src/umash.c#L588), `lrc` is initialized with the two twisting words, then XORed with every original chunk and its key pair, including the final chunk **before** its additive ENH treatment. The final `v128_clmul_cross(lrc)` computes exactly the same product. The original positions use words 0 through 31; the twisting words are 32 and 33.

For fixed original keys, `X=lrc_lo XOR twist_lo` and `Y=lrc_hi XOR twist_hi` are independent uniform words. Their conditional distribution does not depend on those original keys, so `(X,Y)` is also independent of the original keys unconditionally. With equal counts, equality of the two XOR message checksums gives equality of their keyed `lrc` values. Thus the **same** product `Z=X⊙Y` occurs in both secondary outputs and cancels from `B`.

The keys inside `lrc` do not invalidate the argument: an independent uniform XOR translation removes that dependence. For the distinct-word distribution this independence is not asserted; the global conditioning transfer is used instead.

### Structural claim (b): the low-bit constraint and ENH

Let `Delta_r` be the PH output differences and `Delta_E` the difference of the **actual** ENH outputs, including their tags and high/low XOR fold. For equal checksums,

\[
 A=\bigoplus_{r<n}\Delta_r\oplus\Delta_E,\qquad
 B=\bigoplus_{r<n}s_r(\Delta_r)\oplus\Delta_E.
\]

Therefore, exactly,

\[
 B\oplus T(A)=\bigoplus_{r<n-1}T^{n-r}\Delta_r
                \oplus (I+T)\Delta_E. \tag{6}
\]

After conditioning on all chunks except the selected `i,j`, the contributions from those two chunks vanish in the low `h` bits of each lane. Everything else on the right is a fixed lane-wise constant `c`. Hence `(v XOR (u<<1)) mod 2^h=c` is exactly the necessary constraint used in the target count. **The ENH term is generally nonzero.** Its contribution is `(I+T)Delta_E`, together with any other conditioned PH contributions.

For clarity, the actual ENH value for product `P=((a+k_a) mod q)((b+k_b) mod q)` and high-word tag `tau` is

`E_lo=P mod q`,

`E_hi=((floor(P/q)+tau) mod q) XOR E_lo`.

This is what both executable paths implement; replacing it by the unfolded product before reduction would be wrong.

An independent formula implementation was compared with **both** the supplied reference and compiled production C for **964 cases**, covering every byte count 16 through 256 and tags including zero, the top bit, and all ones. Another **560 equal-checksum cases**, one for each shuffler pair, deliberately changed the ENH chunk and its tag and checked (6), its low-bit constants, the equal twisting product, and compensation of checksum changes by twisting-key changes. See [source-checks.json](verification/results/source-checks.json). These finite comparisons support the source inspection; the algebra above supplies the general argument.

## 4. Combination and distinct-word sampling

Condition first on the other original chunk keys and the two companion words used in the raw proof. The low-bit constraints leave at most `N_h^2` four-lane targets `(A0,B0)`. At most `S_h^2` have **both** secondary lanes non-long. The raw joint point bound applies to every target. If a target has a long secondary lane, choose one such lane deterministically. After fixing all original keys, a reduced collision in that lane costs at most `rho_h` over the twisting words. No factor 2 is required: there is a designated necessary long-lane event for each target.

Summing these bounds proves the review's

`2^-(128-h) min(N_h^2, S_h^2+rho_h N_h^2)`.

In fact, if the actual total and non-long target counts are `M,m`, then their contribution is at most `m+rho_h(M-m)`. Both coefficients are nonnegative, so `M<=N_h^2` and `m<=S_h^2` give the stronger bound

\[
 B'_h=2^{-(128-h)}\big((1-\rho_h)S_h^2+\rho_h N_h^2\big).
\]

Its maximum is also at `h=15` and equals

\[
 B'=1381533379/2^{118}<B.
\]

The reference rejection sampler produces IID words conditioned on all 34 being distinct. Its exact acceptance probability is

\[
 a=\prod_{r=0}^{33}(1-r/q)\ge1-561/q.
\]

Thus the safe transferred bound is `B'/a <= B'/(1-561/q)`. Exact integer arithmetic verifies

\[
 \frac{B'}{1-561/q}<\frac{345763417}{2^{116}}.
\]

Equivalently, the relative slack is `1520289/1383053668`, larger than `561/q`. This establishes the **same advertised exact constant** for the distinct-word sampler. The review's simpler `a>2/3` and table-ratios `<2/3` argument establishes `<2^-87`; the refinement here supplies what is needed for the exact-constant reading. The exact acceptance fraction and both conditioned bounds are recorded in [additional-cases.json](verification/results/additional-cases.json).

## 5. The four additional cases

### Different chunk counts

Condition on every original key except the longer block's final pair. Those two words are unused by the shorter block and unused by the longer block's earlier chunks. After additive message translation, the final ENH operands are independent uniform words.

For an ordinary product of two words, the zero product has `2q-1` preimages; a nonzero product has at most `q-1`, because fixing a nonzero first factor determines the second. The ENH tag and fold, and the fixed XOR contribution of preceding chunks, are bijections of the full output. A prescribed reduced pair has at most `9^2=81` raw representatives. Therefore

\[
 \Pr[\text{primary collision}]\le81(2q-1)/q^2<162/q.
\]

One can improve this to `(82q-81)/q^2` by allowing at most one zero-product representative, but that improvement is not needed.

The checksum difference contains at least one fresh uniform original key pair, so checksum equality has probability **exactly `q^-2`**. This event may correlate with primary collision; it is simply bounded separately. Conditional on all original keys and unequal checksums, the twisting-product difference is a differing PH difference. Its every raw target has probability at most `1/q`; the `D^2` argument costs at most `852^2/q=725904/q` for secondary reduced equality. Consequently

\[
 \Pr[\text{joint collision}]
 \le q^{-2}+\Pr[\text{primary collision}]\,725904/q
 <\frac{1+162\cdot725904}{q^2}
 =\frac{117596449}{q^2}<2^{-101}.
\]

### Equal counts, different checksums, a differing PH chunk

Condition on all keys except one word in that original PH chunk, chosen to have nonzero multiplying difference. Its raw primary XOR difference is injective in this word. Every reduced collision requires a mask in `D^2`, so primary collision costs at most `725904/q`.

Then condition on **all** original keys. Distinct checksum inputs to the twisting PH give a secondary reduced collision bound `725904/q`, regardless of whether primary collision occurred. Averaging yields

\[
 \Pr[\text{joint collision}]\le725904^2/q^2
 =526936617216/q^2=852^4/q^2<2^{-89}.
\]

This is a conditional-product argument, not an independence assertion about the completed hashes.

### Odd difference in at least one PH component

Leave the opposite key word `z` free and condition on every other original key. Absorb the message difference in that free operand into a fixed offset. The two complete primary low lanes have forms

`L(z)=low(u⊙z) XOR A`,

`L'(z)=low(u'⊙z) XOR B`,

where `u XOR u'` is odd; `A,B` may differ arbitrarily. Each output bit numbered `r` depends only on `z_0,...,z_r`. In the XOR difference at bit `r`, the coefficient of `z_r` is 1.

Reduced equality implies the integer equation `L(z)-L'(z)=jp` for some `j=-8,...,8`. Fix `j`. Assuming bits below `r` of `z` are known, the borrow entering subtraction bit `r` is known. The required bit of `jp`, together with that borrow, fixes `L_r XOR L'_r`, and therefore fixes `z_r` uniquely. Induction gives **at most one** free word for each signed `j`. Checking the full integer difference can only reject candidates. There are 17 possibilities, proving `17/q`. The high-lane condition can only decrease the probability. Other chunks and tags may differ.

#### Exhaustive odd/even controls

For odd `u XOR u'`, exactly one coefficient is a unit modulo `t^w`. Reparameterizing by its affine low-lane output reduces every possible slice to

`z ≡ (a⊙z mod 2^w) XOR b (mod p)`,

with **every even `a`** and **every offset `b`**. The program exhausts both, counting all free words `z`; it also checks that each signed multiple `jp` occurs at most once in each slice. This is exhaustive over the low-lane necessary event for **all affine PH slices**, including all fixed offsets and changes to the other component, not a selection of message differences. Hence its maximum is a uniform upper bound on the complete two-lane collision count.

For even differences I also exhausted all such slices where both coefficients may be nonunits: remove their common valuation `r`, normalize a unit at width `w-r`, enumerate every possible pair of fixed bottom-`r` offsets, and multiply the free-word count by `2^r`. The largest count is `3q/4`. See [odd-even-8](verification/results/odd-even-8.json), [odd-even-12](verification/results/odd-even-12.json), [even-all-affine-8](verification/results/even-all-affine-8.json), and [even-all-affine-12](verification/results/even-all-affine-12.json).

| w | p | 17-analogue | Odd: exact maximum for all affine low-lane slices | Even: exact maximum for all affine low-lane slices | Full two-lane maxima, all coefficients but zero offsets: odd / even |
|---:|---:|---:|---:|---:|---:|
| 8 | 31 | 17 | 16 | 192 | 16 / 16 |
| 12 | 509 | 17 | 16 | 3072 | 15 / 24 |

The low-lane counts are explicitly **necessary-event** maxima; they are not claimed as attainable full two-lane maxima. The last column independently counts both lanes for all coefficient pairs `u!=u'` and all free words with offsets zero.

Concrete full two-lane even counterexamples to a 17-key extension are:

* `w=8,p=31`: `u=65,u'=67,A=1,B=32`, with **18** keys: `0,32,33,35,39,47,63,66,67,70,71,78,79,96,99,103,111,127`.
* `w=12,p=509`: `u=3589,u'=3591,A=B=0`, with **24** keys: `0,511,515,519,527,543,575,639,767,1022,1030,1038,1054,1086,1150,1278,1534,1543,1551,1567,1599,1663,1791,2044`.

Here compare the two full values `(u⊙z) XOR A` and `(u'⊙z) XOR B`, reducing both lanes. The key lists are complete for those slices; see [even-witnesses-8](verification/results/even-witnesses-8.json) and [even-witnesses-12](verification/results/even-witnesses-12.json). These conditional examples do not refute the next, averaged-over-both-words statement.

### Exactly one changed PH word: a stronger proof

Condition on all other chunk keys. The two primary outputs are

`(U⊙V) XOR C` and `((U XOR delta)⊙V) XOR C`,

where `delta!=0`, `U,V` are independent uniform words, and the **same** fixed 128-bit `C` includes all unchanged chunks and the unchanged ENH/tag. Let `v` be the polynomial valuation of `delta`.

For `M=2^L`, define

\[
 f_M(d)=M^{-1}\#\{x<M:x\equiv x\oplus d\pmod p\},\qquad
 G(M)=\sum_{d\ne0}f_M(d)
 =\frac{2}{M}\sum_{j=1}^{\lfloor(M-1)/p\rfloor}(M-jp).
\]

The last identity counts ordered unequal congruent pairs. It uses no mask recurrence.

**Case `v<=3`.** For fixed nonzero `V` of valuation `r`, the low lane of `U⊙V` is uniform on words whose bottom `r` bits vanish. Its XOR difference `d=low(delta⊙V)` also has those bits zero. Collision depends only on positions set in `d`, since `x-(x XOR d)=2(x&d)-d`; these positions are jointly uniform. Therefore the conditional low-lane probability is exactly `f_q(d)`, independently of `C`. This remains true for `d=0`, with value 1.

As `V` varies uniformly, `d` is uniform on multiples of `2^v`, with `2^v` preimages per value. Thus

\[
 \Pr[\text{primary collision}]
 \le\frac{2^v}{q^2}\left(q+2\sum_{j=1}^{\lfloor(q-1)/(2^vp)\rfloor}(q-j2^vp)\right).
\]

For `v=0,1,2,3`, this is respectively

`8/q+72/q^2`, `8/q+80/q^2`, `8/q+96/q^2`, `8/q+128/q^2`.

**Case `v>=4`.** The two low words share their bottom `v` bits. Their integer difference is divisible by both `2^v` and `p`. Since `2^vp>q-1`, reduced equality forces actual low-word equality. Writing `delta=t^v delta_0` with `delta_0` odd, this forces

`V=t^(w-v) W`, where `W` has `v` bits.

Each such `V` has probability `1/q`; `W=0` contributes `1/q`. For nonzero `W` of degree `s<v`, the high lane of `U⊙V=(U⊙W)t^(w-v)` is uniform on its bottom

`L=w-v+s`

bits, by triangularity using the leading coefficient of `W`. Its remaining bits are zero before XOR with `C`. The high difference is `d=delta_0⊙W`, nonzero and strictly below `2^L`. Fixed upper bits of `C` cancel in the integer difference, and the bottom bits remain uniform. Its conditional collision probability is `f_(2^L)(d)`.

For each fixed degree `s`, the map `W -> delta_0⊙W` is injective. Consequently summing those conditional probabilities is at most `G(2^L)`. Terms with `L<=60` vanish because `2^L<=p`. The only possible contributions are

`G(2^61)=2/2^61`,

`G(2^62)=1+6/2^62`,

`G(2^63)=3+20/2^63`.

Adding these and the `W=0` contribution gives

\[
 \Pr[\text{primary collision}]\le\frac{5+80/q}{q}.
\]

Combining the cases proves the asserted stronger uniform bound `8/q+128/q^2 <9/q`, for every nonzero `delta` and every common fixed `C`. Thus it applies after averaging the unchanged chunks' keys as well.

As finite controls, the program checks the two multiplication marginal ranks for every nonzero multiplier at `w=8,12`, and enumerates both lanes for every `delta` and every pair `(U,V)` when `C=0`. Every resulting probability satisfies the corresponding independently computed bound. The maximum counts are **2120/65536** at `(8,31)` and **32984/16777216** at `(12,509)`, both attained at `delta=1`. These are controls of the proof, not an enumeration over all possible common masks `C`; independence of `C` in the marginal argument establishes that generality.

## 6. Code, certificates, and reproduction

All computation code under [verification](verification/README.md) was written for this verification; it does not import or invoke `prior-toy`, the reviewer's missing program, or GAP.md's recurrence. The supplied reference and production C are executed only for explicit source correspondence checks.

All exhaustive computation and compilation ran on **hardware.normalcomputing.net**, Intel Xeon Platinum 8375C, in `~/agents/umash-subcase-b`, at **nice 10**, with at most **32 computational threads**. No exhaustive suite was run on the Mac. The main suite uses only Python's standard library, GCC/G++, OpenMP, and x86 PCLMUL; it refuses to run on macOS.

The principal machine-readable entry point is [certificates.json](verification/results/certificates.json). It links exact fractions, all masks with witnesses, all target histograms, rank histograms, product distributions, the full odd/even slice maxima, complete even witness key lists, source checks, and additional-case arithmetic. Reproduction commands, scopes, and SHA-256 verification are in [verification/README.md](verification/README.md).
