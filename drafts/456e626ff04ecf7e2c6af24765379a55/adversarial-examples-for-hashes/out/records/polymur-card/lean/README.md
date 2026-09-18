# PolymurHash 2.0: verified ideal collision bound

Lean 4.24.0 and Mathlib `v4.24.0`, commit
`f897ebcf72cd16f89ab4577d0c826cd14afaafc7`.

The authoritative work directory is `~/agents/lean-polymur/` on
`thomas-ahle@hardware.normalcomputing.net`. The local copy contains sources and
verification reports, without toolchains, dependency checkouts, or build caches.

On that server:

```bash
cd ~/agents/lean-polymur
source env.sh
cd lean
bash build_polymur.sh
bash build.sh
```

`build_polymur.sh` builds the full library, generates signature and axiom checks
for every Polymur theorem/lemma and named proof instance, and validates the
output in `PolymurAxioms.txt`. It rejects proof placeholders, declared axioms,
and `native_decide`. `build.sh` also prints the inherited project's audits to
`Audit.txt` and `FullAudit.txt`, and checks that no local proof uses a placeholder,
a declared axiom, or `native_decide`. All work runs at niceness 10 with CPU
affinity restricted to cores 72–79 and `LEAN_NUM_THREADS=8`. The server-specific environment keeps elan
and Mathlib's cache inside the permitted workspace; its CA bundle is used for
Mathlib's bundled curl. The existing Mathlib cache is retained; no compilation
runs on the Mac.

The Polymur library is imported by `ProvenHashes.lean` through
`ProvenHashes/Polymur.lean`:

- `Polymur/Bytes.lean`: primality of 2^61−1; byte packing and overlapping reads.
- `Polymur/Algebra.lean`: expanded block/tail polynomials and source identities.
- `Polymur/Prefix.lean`: Horner loop, decoding, prefix/tail separation.
- `Polymur/Tail.lean`: all three tail branches and byte recovery.
- `Polymur/Encoding.lean`: byte-list polynomial injectivity and piecewise D(n).
- `Polymur/Keys.lean`: exact ideal acceptance set, generator 37, nonemptiness.
- `Polymur/Mix.lean`: the source's 64-bit mixer and additive-secret bijection.
- `Polymur/Collision.lean`: D(n)/|K| bounds and lazy-representative transfer.
- `Polymur/Cardinality.lean`: `K0`, the proposition `CardinalityCertificate`,
  and the original denominator-transport lemmas.
- `Polymur/CharacterSum.lean`: Gauss-sum norm, exact character correlations,
  and the dyadic completion bound `65295510750` for every nontrivial character.
- `Polymur/KeyCount.lean`: subgroup indicators, Möbius inversion, exact-order
  discrepancy, and the certified totient and prime factors of `(p-1)/7`.
- `Polymur/KeyCardinality.lean`: the acceptance interval, six distinct primitive
  seventh-root lifts, and `cardinalityCertificate : K0 ≤ keyCard`.
- `Polymur/Metric.lean`: D(8L) ≤ 9L, minimum at L=1, and numeric log estimates.
- `Polymur/Certified.lean`: unconditional core and idealHash bounds with
  denominator `K0 = 189729088763903999`, and score ≥54.2267.

See `../LEAN_POLYMUR_STATUS.md` for exact scope, milestone commits, signatures,
axioms, and outstanding seed-distribution/C-arithmetic refinement obligations.
The theorem covers ideal uniform multipliers. It does not assert the shipped
initializer's distribution, the published numerical headline, or ASU.

The inherited ProvenHashes modules remain available:

- `Probability.lean`: exact finite uniform probability and coordinate counting.
- `Polynomial.lean`: coefficient-vector polynomial hashes, prime fields, GF64.
- `NH.lean`: exact difference-universality of field NH.
- `Tabulation.lean`: exact XOR simple-tabulation universality.
- `Decoder.lean`: the paper's explicit recurrence coefficient decoder.
- `Recurrence.lean`: key-polynomial decoder, recurrence equivalence, and both
  equal-length and arbitrary-length Schwartz–Zippel bounds.
- `Composition.lean`: independent-key composition and conditional ChainHash bounds.
- `MultiplyShift.lean`: two proved reductions and an **unproved proposition**
  recording the full multiply-shift claim. It does not assert that claim.

See the original `LEAN_STATUS.md` in the delivery for inherited theorem statements
and limitations. In particular,
polynomial messages need a length qualification; field NH is not integer NH or
unreduced CLNH; and the conditional composition theorems do not discharge the
paper's concrete stream-encoding and finalizer obligations.

`uniformProb E` is the nonnegative rational `#{k | E k}/#K`. Uniform keys in a
full product or function space model independent uniform components.
