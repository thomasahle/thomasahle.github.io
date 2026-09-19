STATUS: PARTIAL

## Remaining obligations

The propositions below are in namespace `ProvenHashes.UMASH`. The exact
original theorem `open_phenh_sharp : OpenPHENH` is now proved, with no extra
premise. The stronger PROOF2 constant, the closed 3125 all-pairs theorems,
`Published64`, and `Published128` remain unproved. Their original definitions
are preserved in [UMASHObligations.lean](lean/ProvenHashes/UMASHObligations.lean),
[UMASHSharpObligations.lean](lean/ProvenHashes/UMASHSharpObligations.lean), and
the continuation-obligations modules. No conditional interface below is
reported as a closed collision theorem.

1. **The stronger PROOF2 result `JointPHENHSharpBound` and its 90-bit
   distinct-key transfer.** The remaining sharp proposition is exactly:

   ```lean
   ∀ (seed : Word) (x y : Block),
     x.Valid → y.Valid → sameCount x y → dataChecksum x = dataChecksum y →
     phDiffCount x y = 1 → enhChanges x y = 2 →
     1 ≤ enhValuation x y → enhValuation x y ≤ 63 →
     uniformProb (jointEvent seed x y) ≤ (170906186782 : ℚ≥0)/q^2
   ```

   **The first remaining obligation from round 5 is closed:**
   `phenh_joint_ledger_reduction : PHENHJointLedgerReduction`. The literal
   single-PH decomposition, checksum relation, wrapped valuation transfer,
   independent PH/ENH coordinate counts, preservation of zero low bits,
   and conditional twisting-weight summation are all proved. Equation (8)
   is proved with exactly `r ≥ 4`. No analytic ENH bound, checksum-weight
   lemma, or block-model reduction remains assumed.

   **All 45 low-ledger cases are now closed.**
   `phenh_low_ledger_certificate` covers `1 ≤ r ≤ 3` and `1 ≤ s ≤ 15`.
   The new `r=1,2` proofs work for every shuffler `s`, with respective
   ceilings `169030451200` and `68719476736`. The fifteen previously
   certified `r=3` cases are preserved.

   The **first remaining obligation** is now exactly 15 integer inequalities:

   ```lean
   ∀ s : Fin 15,
     phenhHighLedgerNumerator (s.val+1) ≤ 170906186782*q
   ```

   The numerator, defined in
   [UMASHLedgerInteger.lean](lean/ProvenHashes/UMASHLedgerInteger.lean), is:

   ```lean
   ∑ u ∈ maskSet, ∑ v ∈ maskSet,
     if (phenhPHMask s u v).toNat < 2^63 then
       highTargetKNat (phenhENHMask s u v)*twistHighWeightNumerator v
     else 0
   ```

   `phenhHighLedger_eq_numerator` proves the exact equality to the original
   rational ledger divided by `q`. `phenh_high_ledger_bound_iff` proves that
   the displayed integer inequalities are equivalent to
   `∀ s, 1 ≤ s → s ≤ 15 → phenhHighLedger s ≤ 170906186782`.
   `joint_phenh_sharp_of_high_integer_certificate` supplies all remaining
   mathematical assembly with precisely this certificate as its premise.

   The blocker is the kernel-checked evaluation of these finite sums.
   Direct rational and initial integer-form computations for `s=1`
   exhausted an imposed 12 GiB virtual-memory cap; no failed certificate
   is retained in the reported modules. The inverse's XOR linearity and
   a one-pass prefix-count algorithm are proved for reuse in a partitioned
   certificate. The reference Python computation is not a Lean proof.
   See [GOAL_ROUND6_NOTES.md](lean/GOAL_ROUND6_NOTES.md) for the exact
   computational interfaces and next steps.

   **The original `OpenPHENH` proposition is nevertheless fully closed.**
   [UMASHOpenPHENH.lean](lean/ProvenHashes/UMASHOpenPHENH.lean) proves
   `uniformProb (jointEvent seed x y) ≤ 2058363321984/q^2 < 1/2^87`
   for every positive ENH valuation through 63, using a certified total
   high twisting weight at most 18. This proves exactly
   `open_phenh_sharp : OpenPHENH`. `phenh_open_distinct` proves the same
   strict 87-bit threshold after distinct-key conditioning. The requested
   sharper 90-bit result remains separately open above.

2. `PrimaryPHBound1123`, exactly:

   ```lean
   ∀ (seed : Word) (x y : Block),
     x.Valid → y.Valid → sameCount x y → 0 < phDiffCount x y →
     uniformProb (primaryEvent seed x y) ≤ (1123 : ℚ≥0)/q
   ```

   Missing work: PROOF3 Lemma 3.1(b)–(e) at arbitrary width, convolution identity (7), and its PH-valuation/block-key assembly. Lemma 3.1(a), all 84 small-width convolution cases, the mask census extras, and the displayed arithmetic ledger are proved. The existing 364816 PH theorem remains unchanged.

3. `PrimaryTagOnlyBoundSharp`, exactly:

   ```lean
   ∀ (seed : Word) (x y : Block),
     x.Valid → y.Valid → x.chunks = y.chunks →
     blockTag seed x ≠ blockTag seed y →
     uniformProb (primaryEvent seed x y) < (1 : ℚ≥0)/(8*q)
   ```

   Missing work: the 54-bit mask restriction, product-target count, and transfer to the actual tag-only event in PROOF2 Theorem 9.2. The required divisor bound is proved; it is not by itself a tag-only collision theorem.

4. `PrimaryBlockBound3125`, `IIDLongPrimaryIdentityBound3125`, `LongPrimaryIdentityBound3125`, `CertifiedAllPairs64_3125`, `CertifiedAllPairs128_3125`, `CorrectedLinear64_423`, and `CorrectedLinear128_423`. These remain unproved as closed propositions because items 2 and 3 remain open. Their assembly is proved with those premises explicit. In particular, neither requested closed declaration `certified_all_pairs64_3125` nor its 128-bit counterpart exists. The exact 64-bit target is:

   ```lean
   ∀ (L : ℕ) (seed : Word) (x y : Message),
     1 ≤ L → x.length ≤ 8*L → y.length ≤ 8*L → x ≠ y →
     uniformProb (fun k : Key64 => hash64 k seed x = hash64 k seed y) ≤
       certifiedEnvelope3125 L
   ```

   The 128-bit target replaces `Key64`/`hash64` by `Key128`/`hash128`. The linear targets replace the right-hand side by `423 * (((L+511)/512 : ℕ) : ℚ≥0) / 2^61`. The envelope and its exact integer-power score certificates are proved as arithmetic statements; no unconditional collision or new logarithm/infimum theorem is claimed from them.

5. **PROOF5: `Published128`, its strict strengthening, and sampler variants.** The original exact target remains a proposition definition in [UMASHObligations.lean](lean/ProvenHashes/UMASHObligations.lean), not a theorem:

   ```lean
   ∀ (L : ℕ) (seed : Word) (x y : Message),
     1 ≤ L → x.length ≤ 8*L → y.length ≤ 8*L → x ≠ y →
     uniformProb (fun k : Key128 => hash128 k seed x = hash128 k seed y) ≤
       (((L+2^23-1)/2^23 : ℕ) : ℚ≥0)^2 / (2:ℚ≥0)^83
   ```

   `Published128Strong` substitutes the strict bound `(81/128)*ceil(L/2^23)^2/2^83`. The exact checked definitions `Published128StrongIID`, `Published128StrongNonzero`, and `Published128StrongIIDNonzero` specify the IID-OH and `{1,...,p-1}` variants in [UMASHContinuationObligations.lean](lean/ProvenHashes/UMASHContinuationObligations.lean). These are missing propositions, not assumptions or certificates.

   Missing work: the imported joint rows, starting with `SubcaseBBound`; the shuffler kernel proof; secondary marginal bounds; the exhaustive block ledger; both comparison-polynomial message identities, including actual short/short collisions; averaging over OH identity events; and the all-length envelope/rounding assembly. The same module defines `SecondaryBlockBound729632` and `JointBlockBound1416246956032` precisely: for every valid block pair with unequal expanded data or tags, the secondary projected probability must be at most `729632/q` and the joint probability at most `1416246956032/q^2`.

   The pointwise product of root bounds over two independent `PolyKey` coordinates is now proved for arbitrary fixed comparison polynomials, including zero polynomials. The exact rational `K < 81/128` certificate is also proved. Neither result establishes the missing OH marginal or joint bounds. `hash128` continues to use two independent multipliers and all 34 shared OH words. PROOF5 does not require PROOF4.

6. **PROOF4: `Published64`, its envelope, strict coefficient 58, and inheritance.** The original exact target remains:

   ```lean
   ∀ (L : ℕ) (seed : Word) (x y : Message),
     1 ≤ L → x.length ≤ 8*L → y.length ≤ 8*L → x ≠ y →
     uniformProb (fun k : Key64 => hash64 k seed x = hash64 k seed y) ≤
       (((L+511)/512 : ℕ) : ℚ≥0) / (2:ℚ≥0)^55
   ```

   `Published64Envelope`, `Published64Strong`, `Published64StrongIID`, and `Published128Inherited58` in the continuation-obligations module specify the remaining strengthened targets. The envelope is exactly `headlineEnvelope`, with `headlineA = 205/(q-561)`, `headlineS = 435/(q-561)+2/(p-2)`, value `1/(q-561)` at `L=1`, and `max (headlineA+(1-headlineA)*rootRate L) headlineS` otherwise.

   **Lemma 7.1 is now proved on the literal `polyStep`, including its finalized common-prefix form.** The model retains modulus `q-8=8p` and the reduced square `f*f % p`; no field-only replacement was made. Missing work: Lemma 7.2's grouping and finite certificates, Corollary 7.3, the sharper PH and ENH block bounds (Theorems 4.5/6.2), and the Section 8 message decision list and collision-envelope assembly. The numerical envelope-to-58 implication is proved for every positive length, but the collision envelope is not.

7. **PROOF5 Proposition 11.1 for the excluded shared-multiplier variant.** `ReferenceSwapCollision` and `ReferenceSwapLowerBound` are explicit remaining propositions. `referenceSwapX` is 256 zero bytes followed by 256 bytes of value 1; `referenceSwapY` reverses the blocks. The pointwise target asserts both `hashWith` components collide at multiplier `p-1` for every OH key and seed. The probability target is:

   ```lean
   ∀ (seed : Word), (1:ℚ≥0)/(p-2:ℕ) ≤
     uniformProb (fun k : Key64 =>
       referenceFingerprint k seed referenceSwapX =
       referenceFingerprint k seed referenceSwapY)
   ```

   Missing work: literal encoding of the block-swap messages, commuting updates at `p-1`, and the exact multiplier-fibre count. This is a lower bound for `referenceFingerprint`, not for `hash128`.

## Milestones and signatures

This round adds **69 audited theorem/lemma declarations**, for **830 total**:
454 parent declarations and 376 part-two declarations. All compiled sources
are mirrored in `lean/ProvenHashes/`.

| Completed result | Declaration / source |
|---|---|
| Original OpenPHENH target, with no extra premise | `open_phenh_sharp : OpenPHENH` in [UMASHOpenPHENH.lean](lean/ProvenHashes/UMASHOpenPHENH.lean) |
| Strict 87-bit distinct-key transfer | `phenh_open_distinct` in [UMASHOpenPHENHDistinct.lean](lean/ProvenHashes/UMASHOpenPHENHDistinct.lean) |
| Closed first round-five obligation | `phenh_joint_ledger_reduction : PHENHJointLedgerReduction` in [UMASHPHENHProbability.lean](lean/ProvenHashes/UMASHPHENHProbability.lean) |
| Literal single-PH compressor identities and checksum relation | [UMASHSinglePH.lean](lean/ProvenHashes/UMASHSinglePH.lean) |
| Wrapped-additive/XOR valuation equality and ENH target transfer | [UMASHWordValuation.lean](lean/ProvenHashes/UMASHWordValuation.lean), [UMASHENHTargetWords.lean](lean/ProvenHashes/UMASHENHTargetWords.lean) |
| PH/ENH coordinate counts and conditional weighted partition | [UMASHPHENHTargets.lean](lean/ProvenHashes/UMASHPHENHTargets.lean) |
| Width-uniform zero-prefix preservation and literal equation (8) | [UMASHShufflerSupport.lean](lean/ProvenHashes/UMASHShufflerSupport.lean), [UMASHPHENHAlgebra.lean](lean/ProvenHashes/UMASHPHENHAlgebra.lean) |
| Popcount twisting envelope and two certified weight sums | [UMASHTwistPopcount.lean](lean/ProvenHashes/UMASHTwistPopcount.lean), [UMASHLowWeightCertificate.lean](lean/ProvenHashes/UMASHLowWeightCertificate.lean) |
| All 45 low-ledger cases | `phenh_low_ledger_certificate` in [UMASHJointLedgerLowFinal.lean](lean/ProvenHashes/UMASHJointLedgerLowFinal.lean) |
| Exact integer high-ledger reduction and inverse XOR linearity | [UMASHLedgerInteger.lean](lean/ProvenHashes/UMASHLedgerInteger.lean), [UMASHLedgerIntegerAssembly.lean](lean/ProvenHashes/UMASHLedgerIntegerAssembly.lean) |
| Preserved | All prior proofs, including the 364816 all-pairs theorems, modulo-8p accumulator, two-multiplier model, and conditional 3125 assembly |

Principal new closed signatures:

```lean
open_phenh_sharp : OpenPHENH
phenh_joint_ledger_reduction : PHENHJointLedgerReduction

phenh_low_ledger_certificate (r s : ℕ)
  (hr1 : 1 ≤ r) (hr3 : r ≤ 3) (hs1 : 1 ≤ s) (hs15 : s ≤ 15) :
  phenhLowLedger r s ≤ 170906186782

joint_phenh_open_bound (seed : Word) (x y : Block)
  (hx : x.Valid) (hy : y.Valid) (hc : sameCount x y)
  (hsum : dataChecksum x = dataChecksum y)
  (hp : phDiffCount x y = 1) (he : enhChanges x y = 2)
  (hr : 1 ≤ enhValuation x y) (hr' : enhValuation x y ≤ 63) :
  uniformProb (jointEvent seed x y) ≤ (2058363321984 : ℚ≥0)/q^2

phenh_high_ledger_bound_iff (s C : ℕ) :
  phenhHighLedger s ≤ (C:ℚ≥0) ↔ phenhHighLedgerNumerator s ≤ C*q

phShuffleInverse_xor {n : ℕ} (s : ℕ) (x y : BitVec n) :
  phShuffleInverse s (x ^^^ y) = phShuffleInverse s x ^^^ phShuffleInverse s y
```

The stronger PROOF2 assembly is explicitly conditional:

```lean
joint_phenh_sharp_of_high_integer_certificate
  (hhigh : ∀ s : Fin 15,
    phenhHighLedgerNumerator (s.val+1) ≤ 170906186782*q) :
  JointPHENHSharpBound
```

The preserved all-message signatures still include:

```lean
primary_block3125_of_ph_and_tag :
  PrimaryPHBound1123 → PrimaryTagOnlyBoundSharp → PrimaryBlockBound3125
certified64_3125_of_ph_and_tag :
  PrimaryPHBound1123 → PrimaryTagOnlyBoundSharp → CertifiedAllPairs64_3125
certified128_3125_of_ph_and_tag :
  PrimaryPHBound1123 → PrimaryTagOnlyBoundSharp → CertifiedAllPairs128_3125
```

Full signatures and `#print axioms` reports are in
[AuditAll.txt](lean/AuditAll.txt), generated by [AuditAll.lean](lean/AuditAll.lean).

## Corrections

No requested statement was found false or changed. `open_phenh_sharp` has
exactly the original `OpenPHENH` type. Its proof uses a closed 87-bit
estimate; it does not establish the additional 90-bit constant requested
in PROOF2. That stronger proposition remains explicitly listed above.

The PROOF4/PROOF5 addenda remain tracked. The literal model still has
`polyStep f acc x = ((f*f % p)*(acc+x.1.toNat)+f*x.2.toNat) % (q-8)`;
`hash128` still uses two independent multipliers and all 34 shared OH words.
No conditional interface or numerical envelope is reported as a closed
published collision theorem.

## Axioms and validation

- Full `lake build`: **PASS**, 7555 jobs, using the copied Mathlib cache.
- Axiom audit: **830/830** declarations use only a subset of
  `{propext, Classical.choice, Quot.sound}`. No prohibited placeholders,
  evaluation axioms, custom axioms, or unsafe declarations occur in reported
  modules. See [Verification.json](lean/Verification.json) and [AuditAll.txt](lean/AuditAll.txt).
- Parent preservation and per-declaration coverage: **PASS**; 454 parent
  declarations plus 376 part-two declarations, with 385 successful
  build records. See [Part2Coverage.json](lean/Part2Coverage.json).
- Exact continuation preservation: **PASS**; all 187 source modules at
  `6585d40` are byte-for-byte unchanged, and every original root import is
  preserved. See [GoalRound6Preservation.json](lean/GoalRound6Preservation.json),
  checked by [check-goal-round6.py](lean/check-goal-round6.py). All earlier
  continuation baselines remain checked as well.
- Every retained checkpoint log matches its recorded SHA-256. Every one
  of this round's 69 new declarations has a successful per-lemma `lake build`
  checkpoint. The 14 earlier superseded records remain separately recorded
  in `Part2SupersededCheckpoints.json`.
- No full 64-bit key space is enumerated. Finite certificates concern
  mask sets, 65 popcounts, and the small valuation/shuffler tables.

## Commits, workspace and reproduction

- This round's final proof-source commit: **517811f8f72cebaa4b536c3508e5b7fa45a44139**.
- Literal joint-ledger reduction milestone:
  `8efe3abd2458bdf74a14b925f4fcc1ad7034c1f2`.
- Literal single-PH decomposition and ENH target transfer:
  `7d8e68c0b6a052d028ab31eee777d62fca7e5a11`.
- Resumed commit: `6585d4013fa56818233b2422bdac59e09e240528`;
  its proof-source commit: `15982d3643e3ddc338e2d0b17c04be0058c627ea`.
- Earlier proof-source commits include `20a047d`, `5089c91`, `0aa58dc`,
  `f61ef42c`, and `87509b4`.
- Parent lane commit: `a4786432293aacd4399431a05e732e7ec0c793b2`.
- Parent proof-source commit: `8e20764ffd2ae37313505813383a2ba1e068ebc7`.
- Lean: `leanprover/lean4:v4.24.0`.
- Mathlib: `f897ebcf72cd16f89ab4577d0c826cd14afaafc7`.
- Xeon workspace: `/home/thomas-ahle/agents/lean-umash-corrected-2`, copied
  from the named parent with `.lake` included.
- Build log: [BuildUMASHCorrected2.txt](lean/BuildUMASHCorrected2.txt);
  remote path `/home/thomas-ahle/agents/lean-umash-corrected-2/BuildUMASHCorrected2.txt`.
- Per-lemma logs: `lean/part2-build-logs.tar.gz`, containing all 385
  validated successful logs; originals remain in the Xeon workspace's `logs/`.
- Reproduce on the Xeon: `cd <xeon-work>/lean-umash-corrected-2 && bash reproduce-part2.sh`.
  The script checks the pinned toolchain/cache, builds the entire project,
  reruns every axiom report, and validates preservation and checkpoint logs.
  The complete run is [ReproductionGoalRound6.txt](lean/ReproductionGoalRound6.txt).
  It uses `LEAN_NUM_THREADS=8`, `nice -n 10 taskset -c 56-63`.
- Source hashes: [SourceHashes.json](lean/SourceHashes.json). The Mac was used
  only for editing, reading, and transfers.
