STATUS: COMPLETE

## Remaining obligations

None within the requested corrected UMASH proof. `CertifiedAllPairs64`,
`OpenENHOnly`, the sharp `JointENHOnlyBound`, the IID all-pairs variant, both
linear corollaries, and UMASH-128 inheritance are proved. The terminal theorems
have no extra mathematical premises and use only the three standard Lean axioms.

All declarations below are in namespace `ProvenHashes.UMASH`. Their input
conditions are the conditions of the requested propositions, not additional
assumed probability bounds.

## Certified statements

The exported terminal declarations are in
[UMASHCertified.lean](lean/ProvenHashes/UMASHCertified.lean) and
[UMASHJointClosure.lean](lean/ProvenHashes/UMASHJointClosure.lean):

```lean
certified_all_pairs64     : CertifiedAllPairs64
certified_all_pairs64_iid : CertifiedAllPairs64IID
certified_all_pairs128    : CertifiedAllPairs128
corrected_linear64       : CorrectedLinear64
corrected_linear128      : CorrectedLinear128
joint_enh_only_bound      : JointENHOnlyBound
open_enh_only_sharp       : OpenENHOnly
```

In particular, the primary theorem retains the literal hash and distinct-key
sampling semantics:

```lean
def CertifiedAllPairs64 : Prop := ∀ (L : ℕ) (seed : Word) (x y : Message),
  1 ≤ L → x.length ≤ 8*L → y.length ≤ 8*L → x ≠ y →
  uniformProb (fun k : Key64 => hash64 k seed x = hash64 k seed y) ≤ certifiedEnvelope L
```

With `q = 2^64` and `A = 364816/(q-561)`, the envelope is exactly

* `ε(1) = 1/(q-561)`;
* `ε(L≥2) = A + (1-A) · min(1, 2⌈L/32⌉/(2^61-3))`.

The certified linear bound is `45635 · ⌈L/512⌉ / 2^61`, for both the 64-bit
hash and its 128-bit fingerprint. Probabilities are exact finite uniform counts
in `ℚ≥0`.

The sharp joint theorem proves the actual `jointEvent` probability at most
`4721784/q^2` for valid equal-count blocks with equal nonfinal chunks and distinct
final chunks. It closes the original target unchanged:

```lean
def OpenENHOnly : Prop := ∀ (seed : Word) (x y : Block),
  x.Valid → y.Valid → sameCount x y → phDiffCount x y = 0 → enhChanges x y = 2 →
  32 ≤ enhValuation x y → enhValuation x y ≤ 63 →
  uniformProb (jointEvent seed x y) < (1 : ℚ≥0) / 2^87
```

## Corrections

No requested conclusion was weakened, and no counterexample to the corrected
proof was found.

* `weakA` has the corrected numerator **364816**. The complement, envelope, and
  slope arithmetic use that value.
* The assembly uses actual short/short collision probabilities. Its polynomial
  identity bound is restricted to comparisons involving at least one long
  message, as required by the verdict's Lean trap.
* The dyadic product count proves the stronger `65/q` point-mass bound, implying
  the requested strict `66/q` statement. A 4096-element boundary cover then gives
  `266240/q` for tag-only comparisons, implying the requested strict `269280/q`.
* The earlier coarse joint bound and its `open_enh_only` closure are preserved.
  The final result also proves the requested sharp numerator **4721784**; completion
  does not rely on replacing that statement by the coarse estimate.

The historical published `162/q` primary estimate and `OpenPHENH` are outside
this corrected proof's requested claims. Neither is asserted by this completion.

## Milestones

| Proof stage | Checked result |
| --- | --- |
| Mask certificate and Lemmas 3.1–3.2 | Exact 852-mask set; pattern weights, valuation counts, 248/604 bit splits, `D ∩ 16ℤ = {0}`, and the nonzero-mask lower bound. Finite certificate checks use kernel `decide`. |
| Lemmas 4.1–4.2 | Exact additive low-difference distribution; common-mask pattern cover and low-projection bound. |
| Lemmas 4.3–4.5 | ENH carry/fibre uniqueness with arbitrary common masks, wrapped-operand counting, and the actual all-valuation primary ENH-only bound `5542/q`. |
| Lemmas 5.1–5.3 | Full PH XOR point bound, odd-PH triangular decoding giving `17/q`, and actual even-PH fixed-bit projection giving `364816/q`. |
| Lemmas 6.1–6.4 | High-product point mass; tag-only bound; projected ENH point mass `(82q-81)/q²`; unequal chunk counts; complete primary block bound `364816/q`. |
| Lemmas 7.1–7.2 | Exact truncated carry-less product fibres, checksum cancellation, independent twist-key conditioning, sharp joint `4721784/q²`, and `OpenENHOnly`. |
| Lemma 8.1 | Explicit length/chunk/byte reconstruction for the overlapping long-message encoding. |
| Lemma 8.2 | Short mixer inversion, packing injectivity, exact short uniformity and different-length collisions. |
| Lemmas 8.3–8.4 | Polynomial coefficient recovery; unequal block-count identities below `82/q`; short/long identities at most `9/q`; complete IID long-message identity bound `364816/q`. |
| Lemmas 9.1–9.2 and assembly | Distinct-key conditioning, the corrected all-pairs envelope, IID variant, linear corollaries, and UMASH-128 inheritance. |

The joint proof multiplies a primary probability by a uniform conditional bound
over the independent twisting keys. It does not assume independence of the
completed compressors. Scaled-model experiments are not used as proofs.

## Obligation signatures

Every proposition formerly listed as remaining now has a closed proof:

```lean
low_additive_distribution                 : LowAdditiveDistribution
low_projection_bound                      : LowProjectionBound
primary_enh_only_bound                     : PrimaryENHOnlyBound
odd_ph_bound                              : OddPHBound
even_ph_bound                             : EvenPHBound
high_product_point_mass                   : HighProductPointMass
enh_projected_point_mass                  : ENHProjectedPointMass
primary_tag_only_bound                    : PrimaryTagOnlyBound
corrected_different_chunk_counts_bound    : CorrectedDifferentChunkCountsBound
corrected_primary_block_bound             : CorrectedPrimaryBlockBound
low_ph_distribution                       : LowPHDistribution
joint_enh_only_bound                       : JointENHOnlyBound
open_enh_only_sharp                        : OpenENHOnly
encoding_injective_long                   : EncodingInjectiveLong
short_message_uniform                     : ShortMessageUniform
short_equal_length_injective              : ShortEqualLengthInjective
short_different_length_collision          : ShortDifferentLengthCollision
short_one_word_bound                      : ShortOneWordBound
corrected_different_block_counts_bound    : CorrectedDifferentBlockCountsBound
iid_long_primary_identity_bound           : IIDLongPrimaryIdentityBound
long_primary_identity_bound               : LongPrimaryIdentityBound
certified_all_pairs64                     : CertifiedAllPairs64
certified_all_pairs64_iid                 : CertifiedAllPairs64IID
certified_all_pairs128                    : CertifiedAllPairs128
corrected_linear64                        : CorrectedLinear64
corrected_linear128                       : CorrectedLinear128
```

Full signatures and `#print axioms` output for every local theorem are in
[AuditAll.txt](lean/AuditAll.txt), generated by [AuditAll.lean](lean/AuditAll.lean).

## Axioms and verification

The final full `lake build` passed: **7428 jobs**, with the inherited Mathlib
cache reused. All **454 local theorems and lemmas** passed the axiom audit.
Only `propext`, `Classical.choice`, and `Quot.sound` occur. The source scan found
no `sorry`, `admit`, `native_decide`, custom axioms, or unsafe declarations in
reported modules.

Round 2 adds **137 theorems**, each covered by a successful individual `lake build`
checkpoint. There are **146 successful round-2 checkpoints**, including integration
builds. Every recorded log hash was checked; no theorem checkpoint is missing.

* Final verification: [Verification.json](lean/Verification.json).
* Source manifest: [SourceHashes.json](lean/SourceHashes.json).
* Round-2 checkpoints: [Round2Checkpoints.json](lean/Round2Checkpoints.json).
* Checkpoint coverage: [Round2Coverage.json](lean/Round2Coverage.json).
* Prior-round checkpoints remain in [CorrectedCheckpoints.json](lean/CorrectedCheckpoints.json).

## Commits, logs, and reproduction

* Final proof-source commit: **`8e20764ffd2ae37313505813383a2ba1e068ebc7`**.
* Sharp joint closure: `6853965f62b7fc1b1d6dddfb1b5742967a9fbd2c`.
* Complete primary block proof: `5e6b9af0cb52272a718eeabad36e04923f51db8d`.
* Encoding reconstruction: `062bfeb8a332c94ba4c86997f396e02defef2f65`.
* Resumed reporting commit: `c5e00b7304e0e3038f6b1d0efcb20326b8849aa6`;
  prior proof-source commit: `2e24330bc8fae104489b483303f62e808e2f7e31`.
* This completion report's commit is recorded in [ReportCommit.txt](lean/ReportCommit.txt).
* Lean 4.24.0: `797c613eb9b6d4ec95db23e3e00af9ac6657f24b`.
* Mathlib v4.24.0: `f897ebcf72cd16f89ab4577d0c826cd14afaafc7`.
* Final build log: [BuildUMASHCorrected.txt](lean/BuildUMASHCorrected.txt).
* Final reproduction log: [ReproductionRound2-Final.txt](lean/ReproductionRound2-Final.txt).
* Reproduction script: [reproduce-corrected.sh](lean/reproduce-corrected.sh).

Remote workspace: `/home/thomas-ahle/agents/lean-umash-corrected` on
`<xeon-host>`. Reproduce there with:

```sh
cd <xeon-work>/lean-umash-corrected
bash reproduce-corrected.sh
```

The script checks for the cached Mathlib artifact and runs builds/audits with
`LEAN_NUM_THREADS=8`, `nice -n 10`, and `taskset -c 56-63`. Lean sources, audit
artifacts, checkpoint logs, and reproduction scripts are mirrored in `./lean/`.
All Lean compilation was performed on the Xeon.
