# HighwayHash Lean verification

Completed verification, 2026-09-18. **M1–M4 are complete.** The exact trail
count is `56165 * 2^184` over the full uniform 256-bit key space. The state and
all three output collision bounds are unconditional. The score is checked in
Lean to lie strictly between 59.807 and 59.808 bits, justifying 59.81 bits.
There are no remaining mathematical obligations and no numerical corrections.

## Environment and verification

Authoritative repository:
`thomas-ahle@hardware.normalcomputing.net:~/agents/lean-highway`.
Lean project: `~/agents/lean-highway/lean`.

Lean 4.24.0; Mathlib `v4.24.0`, commit
`f897ebcf72cd16f89ab4577d0c826cd14afaafc7`. The original Mathlib cache from
`~/agents/lean-hash/lean/.lake` was retained. All Lean builds, proof computation,
C compilation and execution ran on the Xeon with `taskset -c 80-87`,
`nice -n 10`, and `LEAN_NUM_THREADS=8`. The Mac handled source edits and transfers.
The inherited `LEAN_STATUS.md` describes the earlier project; its old CPU/thread
settings do not apply to this work.

The final recorded check passes:

- frozen C/header SHA-256 checks;
- `lake build`, 7,391 jobs including cached jobs;
- signatures and `#print axioms` for **166 proof-bearing declarations**;
- all **12 complete C/Lean vector rows**, each containing 16 state words and
  the 7 words returned across the three output widths.

Reports: [build output](lean/BridgeFinalBuild.txt),
[signatures and axioms](lean/HighwayAudit.txt),
[reproduction script](lean/build-highway.sh), [source guide](lean/README.md).

## Milestones

### M1 — complete

`ProvenHashes/Highway/Model.lean` implements the sixteen 64-bit state words as
`BitVec 64`, reset/key injection, explicit 32-bit extraction, both 32×32→64
products, the C zipper masks, complete-packet absorption, little-endian packet
decoding, and all three finalizers. Finalization uses 4, 6 and 10 rounds, including
the 256-bit reduction. The original pair is exactly three packets / 96 bytes /
twelve 64-bit words. No remainder path is needed or claimed.

The C harness includes the supplied C file unchanged. Its SHA-256 is
`fb316726f8d95ab83ad4721058586abb2e98ede635d48a2fd7ec5d6b15543fca`;
the header's is
`beb74b4ed072fee63a38164c43bba053fe5da38631abf0a3627b9dd601de3fe7`.
The reproduction script verifies both before running the vectors.

The four keys are zero, `{1,2,3,4}`, the published collision key, and that key
with its low bit flipped. Each is tested on A, B, and bytes `00..1f`; the last
case exercises Lean's byte decoder. Rows are ordered by key, then message.
Within a row: `v0[0..3]`, `v1[0..3]`, `mul0[0..3]`, `mul1[0..3]`, followed by
64-, 128- and 256-bit output words in C API order.

- [Lean inputs and evaluator](lean/ProvenHashes/Highway/Vectors.lean)
- [C harness](lean/tests/vectors.c)
- [C vectors](lean/tests/c-vectors.txt)
- [Lean vectors](lean/tests/lean-vectors.txt)
- [published C verification log](lean/tests/published-verification.txt)

The published key gives identical absorption states and `f5eba26391be727f`
for both 64-bit outputs. The C verification log also records `F3246108`,
`232D434E`, `0D50D328` and the upstream 33-byte vector. That last remainder-path
check was performed in C only. All executable comparisons are evidence, not
Lean proofs of C equivalence.

### M2 — complete

The principal theorem is about the actual keys, original messages and complete
1024-bit state:

```lean
ProvenHashes.Highway.trail_state_collision :
  ∀ k : Key, Class k → E2 k → StateCollision k
```

`Class k` is `hi (k 0) = 0xdbe6d5d5`. `E2 k` is
`lo (v1[0] + mul0[0] + packet2[0]) - hi v0[0] = 256`
in the actual state after packet 1, before packet 2's first multiply.
`StateCollision k` is `absorb k messageA = absorb k messageB`.

The proof uses additive-group identities, bit-index rewriting and integer carry
reasoning. Under E2, the two 32-bit operands swap, so their widened 64-bit
products agree. Packet 1 establishes the two word differences; packet 2's zipper
terms cancel the v0 difference and leave only `v1[0] - 256`; packet 3 cancels
that last difference. The carry bounds are derived from the key class. There
is no enumeration of keys or of 64-bit operands.

`trail_outputs` proves equality of the actual 64-, 128- and 256-bit finalizers
under `Trail k := Class k ∧ E2 k`. `common_suffix` also proves preservation of
state equality under arbitrary common packet suffixes.

### M3 — complete: exact count on full keys

The decisive unconditional statements in namespace `ProvenHashes.Highway` are:

```lean
full_count : (Finset.univ.filter Trail).card = 56165 * 2^184
exact_trail_count : ExactTrailCount
```

The original requested value is correct. The bridge preserves every coordinate:

1. `fullKeyCoordinates : Key ≃ Half × ((Half × Word) × (Word × Word))`
   retains the high half of key word 0, its XOR-adjusted low half, the bijective
   lane-1 coordinate `W`, and the unrestricted key words 2 and 3.
2. `byteCoordinates : (Half × Word) ≃ (FreeCoordinates × RelevantCoordinates)`
   splits the active 96 bits into 56 free bits and 40 relevant bits, with explicit
   assembly and disassembly inverses. The free bits are the low 16 reset bits,
   W bytes 1–3, and W bytes 4 and 5. The relevant bits are W byte 7, the high
   16 reset bits, W byte 6, and W byte 0.
3. `assembled_e2_iff` identifies the actual E₂ event with
   `ArithmeticBytes ... ∧ padCondition ...`. `assembled_pad_byte` proves the
   lane-1 output byte's pad form. `output_carry_bound` and the `Fin 4` type of
   `padTarget` put both target values in `[50,53]`. The low byte affects the
   target only through `padCarry`, including its exceptional boundary.
4. `assembled_count` gives exactly 56,165 satisfying relevant coordinates for
   every assignment of the 56 free bits. `two_word_count` gives `56165 * 2^56`.
   `full_key_trail` fixes the 32 class bits and shows the two remaining words
   are unrestricted. Finite-set cardinality transport gives
   `56165 * 2^56 * 2^128 = 56165 * 2^184`.

The bridge sources are [BridgeWords](lean/ProvenHashes/Highway/BridgeWords.lean),
[BridgeCoordinates](lean/ProvenHashes/Highway/BridgeCoordinates.lean),
[BridgePredicate](lean/ProvenHashes/Highway/BridgePredicate.lean), and
[BridgeCount](lean/ProvenHashes/Highway/BridgeCount.lean).
The composed coordinate equivalences establish the complete key-to-byte bijection.

The inherited reduced count is `235 * 239 = 56165`. The pad fibre has one solution
for every allowed pair of targets, so this argument does not assume independence
of the intermediate arithmetic tests. Only the following residue checks enumerate
cases, with ordinary kernel `decide`:

- carry window: at most `2 * 257 * 256 = 131584` cases;
- top byte: 256 cases;
- pad boundary: `4 * 4 * 256 = 4096` cases, plus its four boundary solutions.

These remain below `2^20`; the bridge adds no exhaustive key or residue search.
The full key count is proved by bijections and cardinality identities.

### M4 — complete: unconditional bounds and checked score

[ExactBound](lean/ProvenHashes/Highway/ExactBound.lean) applies the proved count
without leaving any hypothesis:

```lean
trail_probability : uniformProb Trail = 56165 / (2 : ℚ≥0)^72

state_collision_bound :
  56165 / (2 : ℚ≥0)^72 ≤ uniformProb StateCollision

output_collision64_bound :
  56165 / (2 : ℚ≥0)^72 ≤ uniformProb OutputCollision64
output_collision128_bound :
  56165 / (2 : ℚ≥0)^72 ≤ uniformProb OutputCollision128
output_collision256_bound :
  56165 / (2 : ℚ≥0)^72 ≤ uniformProb OutputCollision256
```

`output_collision_bounds` packages all three output results. The existing
`state_collision_bound_from_count` and `output_collision_bounds_from_count`
are retained as reusable implications and are discharged with `exact_trail_count`.
Probabilities are exact nonnegative rational cardinality ratios. The collision
probabilities are lower bounds; the exact equality counts the specified trail.

For the twelve-word pair, [Score](lean/ProvenHashes/Highway/Score.lean) defines

```lean
trailScore := Real.logb 2 (12 * (2 : ℝ)^72 / 56165)

trail_score_from_probability :
  Real.logb 2 (12 / (uniformProb Trail : ℝ)) = trailScore

trail_score_bounds :
  (59807 / 1000 : ℝ) < trailScore ∧ trailScore < (59808 / 1000 : ℝ)
```

Thus the score rounds to **59.81 bits** at two decimal places. The enclosure uses
logarithm monotonicity and the exact power inequalities
`12^1000 * 2^12192 < 56165^1000 < 12^1000 * 2^12193`, proved by `norm_num` and
checked by the kernel. It uses no floating-point oracle.

## Corrections

None. The full-key proof confirms the requested count and probability. The prior
qualification on the actual-key count has been removed by proof.

## Axioms and proof scope

Every explicit Highway theorem, the seven proof-bearing coordinate equivalences,
and the generated state-structure extensionality theorems have signatures and
`#print axioms` output in `HighwayAudit.txt`. Their axiom sets are subsets of
`{propext, Classical.choice, Quot.sound}`. For example:

```text
'ProvenHashes.Highway.trail_state_collision' depends on axioms:
[propext, Classical.choice, Quot.sound]
```

There are no placeholders, custom axioms or `native_decide` in the audited
sources, and no `Lean.ofReduceBool` in their axiom closures. Experimental files
were removed from the source mirror. One constructor-name style linter is
locally disabled in `ArithmeticCount.lean` because the linter itself exceeded
its recursion limit; kernel proof checking remains enabled throughout.

## Commits

Commits are in the new remote repository, leaving the original `lean-hash`
repository untouched:

- `9133bf9a2124461a80c3aabe45d68a53d734e307`: model, reference vectors, key-class
  density and the initial residue counts (29 audited declarations).
- `6e22941c4882bba1e6c6e6d920e7a1b81240e3c5`: complete algebraic trail and supporting
  byte/count lemmas (89 audited declarations).
- `59bb4295bad0c2d435f3017c66abf64861e281d0`: the 125-declaration build,
  actual E2 byte characterization, reduced 40-bit count, conditional numerical
  bounds, reproducible checks and explicit remaining key-count obligation.

Round-2 completion commits on the same branch:

- `697cf50`: recovered and checked the coordinate bijections, reset arithmetic,
  E₂ predicate bridge, and the 96-bit count.
- `36a9173`: proved `full_count`, discharged `ExactTrailCount`, and added the
  unconditional state and three output bounds.
- `bd15afcdd8a8d56fef506dc417691f29ddbc6ae6`: checked the score interval, updated the
  reproduction script, and recorded the successful 166-declaration axiom audit
  and complete reproduction log.

## Reproduction

On the Xeon, run `cd ~/agents/lean-highway/lean && ./build-highway.sh`.
The script verifies the pinned Mathlib commit and frozen reference hashes, builds
with CPUs 80–87 and eight Lean threads, regenerates the signature/axiom audit,
and compares the twelve complete C/Lean vector rows. The retained Mathlib cache
is used throughout. The final log is `lean/BridgeFinalBuild.txt`; the incremental
build history is `lean/BridgeBuildHistory.txt`.
