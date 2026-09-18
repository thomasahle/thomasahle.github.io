STATUS: PARTIAL

## Remaining obligations

The following are precise Lean `Prop` definitions in
[RefinementObligations.lean](lean/ProvenHashes/Halftime/RefinementObligations.lean):

- `ProvenHashes.Halftime.StyleRefinement`: universal executable/M5 agreement for an admissible public schedule. Finite C++/Lean vectors agree; a universal connection remains unproved. The upstream header also has reproducible table-view undefined behavior.
- `ProvenHashes.Halftime.Encode3Distance`: unconditional distance three for the concrete fixed executable encoder. Exact-header generator agreement and exhaustive external ranks pass; no kernel-checked certificate transfer yet.
- `ProvenHashes.Halftime.FixedHeaderBound`: the exact fixed executable's all-length, all-target probability is at most `6804 / (2 : ℚ≥0)^96` under its ideal uniform key array. Concrete encoder, lane/layout and terminal-length premises have not all been discharged in Lean.
- `ProvenHashes.Halftime.SeededStyleBound`: the Style normalized bound under the supplied adapter's uniform 64-bit seed. Functional expansion checks pass, but M5's independent-uniform-key hypothesis does not transfer. `no_uniform_seed_expansion_two_words` proves the distribution-preservation obstruction; it does not refute this separate bound.

Full propositions, scope, corrections and construct-by-construct evidence:
[LEAN_NH_HALFTIME_STATUS.md](LEAN_NH_HALFTIME_STATUS.md).

## Milestones

- **Task A checked:** full 128-bit integer NH with 64-bit wrapped input/key additions, arbitrary target difference, fixed original length, and optional one-word zero padding. Sharp normalized score **64** with padding; **65** for unpadded pairs.
- **Task B partial:** executable Lean model, exact NH primitive bridge, fixed/upstream C++ comparison, encoder rank checks, seed-expansion comparison, and concrete upstream defects reproduced. These do not justify labelling the exact fixed-header probability theorem “✓ checked”.

## Corrections

`2^-65` is the unpadded **length-normalized** value. Raw collision probability
can equal `2^-64`, as the checked `(0,0)` versus `(1,0)` witness proves.
The given appendix checkout contains no relevant chart row, so the explicitly
permitted full 128-bit NH output was used. No unspecified 64-bit reduction is
certified. The supplied smhasher3 path is absent; the permitted upstream
header fallback is pinned to `8b03edf` and recorded by SHA-256.

## Signatures and axioms

See [NH64.lean](lean/ProvenHashes/Halftime/NH64.lean),
[ExecutableBridge.lean](lean/ProvenHashes/Halftime/ExecutableBridge.lean), and
[SeedExpansion.lean](lean/ProvenHashes/Halftime/SeedExpansion.lean).
Complete elaborated signatures and per-theorem `#print axioms`:
[HalftimeAudit.txt](lean/HalftimeAudit.txt) and [FullAudit.txt](lean/FullAudit.txt).
Only `propext`, `Classical.choice`, and `Quot.sound` are allowed.

## Reproduction and provenance

Xeon: `~/agents/lean-nh-halftime`, copied with `.lake` from `~/agents/lean-halftime`.
All builds/checks use CPUs 88–95, nice 10, eight Lean threads. Mathlib was cached.
Run [reproduce.sh](reproduce.sh) on the Xeon. Sources are mirrored in `./lean/`.
Parent commit: `c456cedba8c2a8f639f7ee614f8b01dbd49cb4f4`.
Mathlib commit: `f897ebcf72cd16f89ab4577d0c826cd14afaafc7`.
Fixed-header fork commit: `e1b52cf9efd4016c25a144bc6e2e9e805fe632ab`.

Proof/test commit: `6b18383da5f034667a9dc1f0c96e7069109878e5`.

Final verification passed: `lake build` (7,390 jobs), **251 Halftime** and
**57 inherited** theorem axiom audits; **1,216 C++/Lean cases** (4,864 output
words), scalar/SIMD/UBSan agreement for the fixed header, 640 encoder survivor
subsets, and 36,000 seed-expanded words. The upstream table-view UB and broken
Encode3 witness reproduce as expected diagnostics.

Build/audit/vector log: [evidence/reproduction.log](evidence/reproduction.log),
also `~/agents/lean-nh-halftime/evidence/reproduction.log` on the Xeon.
Tool/source provenance: [evidence/PROVENANCE.txt](evidence/PROVENANCE.txt).
Per-lemma build logs are retained as `evidence/build-*.log`; failed exploratory
attempts are superseded by the successful final reproduction.

