STATUS: PARTIAL

## Remaining obligations

The new target statements are defined in `lean/ProvenHashes/ChainHash128.lean`; incremental kernel verification is running on the Xeon. The remaining full statements are:

```lean
Irreducible (Polynomial.X^128 + (Polynomial.X^7 + Polynomial.X^2 + Polynomial.X + 1) : Polynomial (ZMod 2))
Fintype.card ProvenHashes.ChainHash128.F = 2^128
```

In `ProvenHashes.ChainHash128`, with the types and functions imported in that module:

```lean
∀ (n : ℕ) (m m' : List UInt8),
  m.length < 2^128 → m'.length < 2^128 → m ≠ m' →
  max 1 ((m.length+511)/512) ≤ n → max 1 ((m'.length+511)/512) ≤ n →
  uniformProb (fun k : Key41 => hashBytes k m = hashBytes k m') ≤
    min 1 (((n+2 : ℕ) : ℚ≥0) / 2^128)

∀ (L : ℕ), 0 < L → 16*L < 2^128 →
  ∀ (m m' : List UInt8), m.length ≤ 16*L → m'.length ≤ 16*L → m ≠ m' →
  uniformProb (fun k : ModelA.Key =>
    hashBytes (ModelA.expandedKey k) m = hashBytes (ModelA.expandedKey k) m') ≤
      ModelA.epsilonAtMost L
```

The ported field certificate, stream, recurrence, twist, finalizer, and composition dependencies must finish incremental builds and axiom audits before these can be reported proved. Precise component signatures are in the Lean source mirror. No unproved declaration is being claimed as a theorem.

## Design selection

The inspected adjacent REPORT has no RECOMMENDED DEFAULT design and describes measurements in progress. Per the task's fallback, this lane formalizes `chainhash-128/SPEC.md` and its header: strided 512-byte blocks, W=32, S=1, full 128-bit PH words. This is not the adjacent SPEC's delivered default B. Selection evidence is preserved in `reference/design-selection/ADJACENT_REPORT.md`.

## Milestones

- Parent copied, including `.lake`, to `~/agents/lean-chainhash128`.
- CLHASH lane reported COMPLETE before this lane began using CPUs 48–55.
- 128-bit Rabin criterion and changed byte-encoding lemmas compile.
- 252 executable Lean/header vector comparisons passed for ideal and model-A keys, including empty input and tails; these are executable evidence, not a formal C refinement proof.
- Certificate identities and end-to-end proof builds are in progress.

## Axioms, commits, logs

No completed 128-bit axiom audit yet. Parent copied HEAD: `a3939c01b87d962ae170776b656a154300d5f3ca` (parent model-A report identifies proof snapshot `408d147c7f0aa7e123c6027b36a26821522e84a6`). Mathlib: `f897ebcf72cd16f89ab4577d0c826cd14afaafc7`; Lean 4.24.0. Current build log: `~/agents/lean-chainhash128/logs/incremental128.log`.

Expected specification certificate scores, not yet reported as fully Lean-certified bounds: ideal 126.4150374993 bits, model-A fixed-length 126.4150374993 bits, model-A at-most-length 125 bits, in 16-byte-word units.
