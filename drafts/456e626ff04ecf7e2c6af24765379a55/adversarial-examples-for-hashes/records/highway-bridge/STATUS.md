STATUS: COMPLETE

## Remaining obligations

None. `ExactTrailCount`, the unconditional state and all three output collision
bounds, and a numerical enclosure for the score are proved. No mathematical
hypothesis remains beyond the three permitted standard Lean axioms.

## Milestones

- M1 and M2: the existing frozen model, vector checks, and complete algebraic
  weak-key state-collision trail are preserved.
- M3: the complete key-to-byte coordinate bridge is proved. The actual E₂
  predicate is equivalent to the counted byte predicate, including the two pad
  targets in `[50,53]` and their dependence on the low byte through the carry
  branch. All 56 free active bits and the remaining 128 key bits are retained.
  The full count is exactly `56165 * 2^184`.
- M4: the exact trail probability and unconditional state/output bounds compile.
  The score lies strictly between 59.807 and 59.808 bits, justifying 59.81 bits.
- Final reproduction: `lake build` succeeded (7,391 jobs including cached jobs),
  166 declarations passed the axiom audit, and all 12 complete C/Lean vector rows
  matched. The pinned Mathlib commit and frozen reference hashes were verified.

## Signatures

All names below are in namespace `ProvenHashes.Highway`.

```lean
full_count : (Finset.univ.filter Trail).card = 56165 * 2^184
exact_trail_count : ExactTrailCount
trail_probability : uniformProb Trail = 56165 / (2 : ℚ≥0)^72

state_collision_bound :
  56165 / (2 : ℚ≥0)^72 ≤ uniformProb StateCollision
output_collision64_bound :
  56165 / (2 : ℚ≥0)^72 ≤ uniformProb OutputCollision64
output_collision128_bound :
  56165 / (2 : ℚ≥0)^72 ≤ uniformProb OutputCollision128
output_collision256_bound :
  56165 / (2 : ℚ≥0)^72 ≤ uniformProb OutputCollision256

trail_score_from_probability :
  Real.logb 2 (12 / (uniformProb Trail : ℝ)) = trailScore
trail_score_bounds :
  (59807 / 1000 : ℝ) < trailScore ∧ trailScore < (59808 / 1000 : ℝ)
```

`trailScore` is `Real.logb 2 (12 * (2 : ℝ)^72 / 56165)`.
`output_collision_bounds` packages the three output bounds. The old
`state_collision_bound_from_count` and `output_collision_bounds_from_count`
implications are retained and instantiated with the unconditional
`exact_trail_count` theorem. The exact cardinality is for the trail; the state
and output collision probabilities are bounded below by its probability.

The full-key equivalence `fullKeyCoordinates` and the 96-bit equivalence
`byteCoordinates` compose to retain all 256 coordinates. `assembled_e2_iff`
connects actual E₂ to `ReducedCount`; `assembled_count` and `two_word_count`
then supply the cardinalities used in `full_count`.

## Axioms and finite checks

[HighwayAudit.lean](lean/HighwayAudit.lean) and
[HighwayAudit.txt](lean/HighwayAudit.txt) contain signatures and `#print axioms`
for every explicit Highway theorem, all seven proof-bearing coordinate
equivalences, and the state extensionality theorems: 166 declarations in total.
Every axiom set is a subset of `{propext, Classical.choice, Quot.sound}`.
There are no proof placeholders, custom axioms, unsafe declarations,
`native_decide`, or `Lean.ofReduceBool` dependencies in the reported proof.

The inherited kernel `decide` residue checks remain below `2^20` cases:
131,584 carry-window cases, 256 top-byte cases, and 4,096 pad-boundary cases
plus four boundary solutions. The new bridge uses algebra and bijections.
The score uses exact power inequalities checked by `norm_num` and logarithm
monotonicity, with no floating-point oracle.

## Corrections

None. The original count `56165 * 2^184`, probability `56165 / 2^72`, and
rounded score 59.81 bits are confirmed. The prior count qualification is removed.

## Commits and environment

Authoritative repository: `<xeon-host>:<xeon-work>/lean-highway`,
on its existing `master` branch.

- `f2c7f51`: inherited recorded checkpoint, based on `59bb429`.
- `697cf50`: recovered compiled coordinate and predicate bridge.
- `36a9173`: exact full-key count and unconditional probability bounds.
- `bd15afcdd8a8d56fef506dc417691f29ddbc6ae6`: checked score, complete axiom audit,
  reproduction script and successful final build log.

Lean 4.24.0; Mathlib v4.24.0 at
`f897ebcf72cd16f89ab4577d0c826cd14afaafc7`. The existing `.lake` cache was retained.
All compilation and proof computation ran on the Xeon with CPUs 80–87,
`nice -n 10`, and `LEAN_NUM_THREADS=8`; the Mac handled edits and transfers.

## Build log and reproduction

Final log: [lean/BridgeFinalBuild.txt](lean/BridgeFinalBuild.txt), authoritative
path `/home/thomas-ahle/agents/lean-highway/lean/BridgeFinalBuild.txt`.
Incremental history: [lean/BridgeBuildHistory.txt](lean/BridgeBuildHistory.txt).
The complete Lean sources, audit inputs/outputs, scripts and logs are mirrored
under `./lean/`.

Run on the Xeon:

```bash
cd <xeon-work>/lean-highway/lean
./build-highway.sh
```

[build-highway.sh](lean/build-highway.sh) verifies the pinned dependencies,
reference hashes, full build, complete axiom audit and vector comparisons.
[LEAN_HIGHWAY_STATUS.md](LEAN_HIGHWAY_STATUS.md) contains the updated lane report
and proof decomposition; [lean/README.md](lean/README.md) is the source guide.
