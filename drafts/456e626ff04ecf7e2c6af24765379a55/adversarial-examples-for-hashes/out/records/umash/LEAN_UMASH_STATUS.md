STATUS: PARTIAL

The full obligation ledger, corrections and exact propositions are in
[STATUS.md](STATUS.md). This lane does not certify either the unconditional
25.6-bit all-pairs bound or the published 55-bit bound.

Implemented the literal UMASH word/byte model and proved the 852-mask cardinality,
projection cover, finalizer inverse, polynomial interpretation/root counting,
encoding validity and lengths, distinct-key acceptance, exact bound arithmetic, and fingerprint-to-primary
probability transfer. The actual block-case probability proofs remain incomplete.

The checked end-to-end implications are:

```lean
SharpPrimaryProjection → Published64
WeakPrimaryProjection → ShortOneWordBound → CertifiedAllPairs64
WeakPrimaryProjection → ShortOneWordBound → CertifiedAllPairs128
CertifiedAllPairs64 → CertifiedAllPairs128
```

`OpenENHOnly` and `OpenPHENH` remain ordinary definitions of `Prop`. They have not
been assumed or proved. No open-case breakthrough is claimed. The primary
projection gap is recorded independently from these joint fingerprint cases.

Reproduction: `bash reproduce.sh` in `~/agents/lean-umash` on the Xeon, using the
copied Mathlib cache, CPU set 56–63 and eight Lean threads. The final build log,
axiom reports, source hashes and model smoke checks are mirrored under `lean/`.
Final audit and commit metadata are recorded in STATUS.md.

Combined build: PASS. Axiom audit: PASS for 187 local theorems/lemmas (including
inherited modules), using only `propext`, `Classical.choice`, and `Quot.sound`.
Reference comparison: PASS for 120 outputs. Verified proof-source commit:
`c85c49bef9ac5b71f17980874adddc66c9146376`.
