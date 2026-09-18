# Collision proof for ChainHash-x86 v1

The specification has a mathematical proof using the existing ChainHash
lemmas. This deliverable does **not** claim a newly machine-checked Lean
theorem for the 137-word adjacent-pair C implementation. The existing concrete
Lean theorem covers the old 41-word, 256-byte strided family. The adaptation
below requires new byte-layout and key-layout definitions and correspondence
proofs, but no new probability, integer-NH, or universal-reduction lemma.

Write q=2^64 and F=GF(2)[X]/(X^64+X^4+X^3+X+1). Keys are the independent,
uniform words specified in SPEC.md. Fix two distinct byte strings of lengths
below 2^64 before sampling the key.

## 1. Adjacent PH has the required full-width bound

For a fixed set of pair indices I and distinct padded word arrays, define

`C_k(m) = sum_{i in I} (m[2i]+k[2i])(m[2i+1]+k[2i+1])`

in the polynomial ring GF(2)[X]. Addition is XOR and multiplication is
unreduced. The key-key terms cancel in `C_k(m)+C_k(m')`. Choose a differing
message word. Conditional on every key word except its partner's key word,
the difference has the form `D*k_partner + E`, with nonzero polynomial D.
The polynomial ring is an integral domain; multiplication by D is injective.
For any target C, at most one of the 2^64 partner words hits C. Therefore

`Pr[C_k(m)+C_k(m')=C] <= 1/q`.

This is exactly `clnh_difference_bound` in `Carryless.lean`, with w=64 and
pair index `(i,false)/(i,true)` corresponding to words `2i/2i+1`. The lemma
is independent of the previous strided layout and independent of the number
of pairs. `clnh_natDegree_le` supplies degree at most 126, so low/high splitting
is injective into F x F; no reduction of the level-1 output occurs.

For nested unequal active pair sets and **nonzero** target C, use
`clnh_nested_nonzero_bound` in `CarrylessVariable.lean`. Its hypotheses allow
arbitrary data in the common positions. This is not an AXU claim at target
zero for unequal pair counts; that claim would be false.

## 2. The encoded streams collide with probability at most 1/q

Let `s_k(m)` be the list of `(a_j,b_j)` pairs after the final length XOR.
For equal block counts, choose the following cases.

* **Equal byte lengths:** distinct messages have a differing block. Its
  equal-size padded byte-to-word encoding is injective. The active pair sets
  are equal, and the length masks cancel. Equality of whole streams implies
  equality of this block's PH sum, an event of probability at most 1/q.
* **Different byte lengths, equal final pair counts:** the target difference
  of the last raw PH sums is
  `C=(ell XOR ell')*(1+X^64)`, which is nonzero. If the padded last blocks
  coincide, equality is impossible. Otherwise apply the full-width bound in
  step 1 with this target.
* **Different final pair counts:** the final active sets are nested prefixes.
  The same length-derived C is nonzero; apply
  `clnh_nested_nonzero_bound`.

The zero-length message uses the empty active set and a single empty block;
the nested-set argument includes it. XOR into both halves preserves the
nonzero target for any two distinct lengths below 2^64. These are the same
cases used by `Stream.lean` and `FieldStream.lean`.

Repeated PH keys across blocks do not introduce a union over blocks: equality
of streams implies one selected block equality. No independence between block
collision events is asserted or needed. The recurrence keys are independent
of the entire PH key.

If block counts differ, list lengths differ, so the encoded streams are
different for every key.

Required layout adaptation in Lean: define `adjacentWordPosition(i,b)=2*i+b`,
its inverse by quotient/remainder by 2, and blocks of 128 words with active
prefix length `ceil(r/16)`. Reuse byte/word injectivity and length injectivity
from `ByteEncoding.lean` and `WordRepresentation.lean`. Instantiate the stream
proof with these definitions. These are finite indexing/encoding proofs, not
new hash universality lemmas.

## 3. The existing recurrence bounds apply unchanged

For a list `s=[(a_1,b_1),...,(a_n,b_n)]`, define the polynomial in independent
field keys U,Y,Z by

`P_0=Z; P_j=a_j+(b_j+Y)(P_{j-1}+U)`.

`Recurrence.keyPolynomial_injective` proves that different lists yield
different formal key polynomials. It does not claim injectivity for every
sampled key. For equal list lengths n, the difference has total degree at most
n; `Recurrence.collision_bound` bounds its zero probability by n/q.
`Recurrence.collision_bound_any_length` supplies `(max(n,n')+1)/q` when list
lengths can differ. Both count over the whole field, including zero keys.

For equal block counts, condition on PH keys yielding different streams,
and then use the recurrence's independent uniform `(u,y,z)` keys. Adding the
stream collision event gives `(n+1)/q` before finalization.
For unequal block counts, the streams are always distinct and the recurrence
contributes `(max(n,n')+1)/q` directly.

## 4. Twist and finalizer add exactly one field collision term

For fixed tau, `v -> (v+tau) mod 2^64` is a bijection on words, hence on F
under the bit representation. This is `integerTwist_bijective` in
`Finalizer.lean`, with the word correspondence in `ReferenceOperations.lean`.

The finalizer circuit in SPEC.md is exactly the existing circuit. Its
coefficient map is bijective, so independent uniform `c0,...,c4` produce a
uniform monic quintic. Conditional on any two distinct upstream values and
on tau, `chain5_collision_exact` gives collision probability exactly 1/q.
The finalizer parameters remain independent after this conditioning.

Thus in both block-count cases, with `N=max(n,n')`,

`Pr[H_K(m)=H_K(m')] <= (N+2)/q`.

This is `chainhash_equal_length_from_stages` or
`chainhash_different_lengths_from_stages` in `Composition.lean`, after the
new stream instance is supplied. Generic product-key conditioning is provided
by `uniformProb_prod`, `uniformProb_prod_le`, and `compose_collision_bound`.

The five-wise finalizer result may also be reused conditional on five distinct
upstream values; it does not establish unconditional five-wise independence
of the whole hash.

## 5. Concrete arithmetic, keys, score, and implementation boundary

`BinaryRabin.lean`, the `Modulus*.lean` certificates, and `ConcreteWords.lean`
already establish irreducibility of the exact degree-64 polynomial and its
carry-less multiplication representation. No new field is introduced.

Define `Key137 = Fin 137 -> Word 64`, with the positions in SPEC.md, and
adapt the `KeyLayout.lean` product equivalence. The separate key segments
then have exactly the independence used above. Adapt `ReferenceChainHash.lean`
and `ByteInterface.lean` to the new byte loop and 1024-byte block size.
The implementation avoids the old `len+B-1` expression, so the length
assumption is simply `len<2^64` plus ordinary valid-memory requirements.

For positive 8-byte-word budget L, `N<=max(1,ceil(L/128))`. Since
`max(1,ceil(L/128))+2<=3L`, the certificate score is at least
`64-log2(3)`, with equality in that expression at L=1. Hence it exceeds 62.

The C/SIMD equivalence is tested rather than verified by Lean. Tests compare
all available entry points with the C99 bit-serial reference on independent
random raw keys and inputs, block/pair boundaries, null-empty input, guard-page
tails, and degenerate keys. The 64-bit-seeded SMHasher subfamily is measured
but carries no ideal-key theorem.

## Why the other candidates were not needed for this proof

Integer NH32 requires the standard unsigned NH almost-difference-universality
lemma over Z/(2^64), and independent lanes give a 2^-64 equality bound on the
128-bit concatenation. The existing `NH.lean` is a **field** theorem and does
not prove this ring statement. An XOR length target cannot be substituted
for its additive target without another argument. A separate length pair in
the recurrence would give the clean `(n+3)/2^64` alternative discussed during
exploration, with score exactly 62.

Packed IFMA52 needs the analogous unsigned NH lemma at w=52, exact carry
normalization of each 104-bit sum, and retention or universal reduction of
both independently keyed results. Using only the low 52-bit product, dropping
input bits, or treating two accumulators as a 104-bit sum without carries would
not implement that theorem. The preexpanded microbenchmark is explicitly only
an arithmetic diagnostic and is not an injective byte hash.

Splitting a message between PH and NH does not multiply their collision bounds:
two messages can differ only in one half. Raw concatenation has the maximum
of the two compressor bounds; independent universal reduction adds its own
bound. Any 64-bit reductions and the final 128-to-64 reducer must be accounted
for. The selected adjacent PH variant needs none of those new ring or reduction
lemmas.

For the standard unsigned integer-NH result, see Krovetz's original
[Software-Optimized Universal Hashing and Message Authentication](https://cr.yp.to/bib/2000/krovetz-thesis.pdf),
Theorem 2.4.2 and its discussion of additive differences. The selection here
uses the local, already checked carry-less lemmas instead.
