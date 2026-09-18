STATUS: PARTIAL

## Remaining obligations

This is a partial formalization, not a certification of the published bound or
of the unconditional 25.6-bit bound. In addition to the two cases open on paper,
several paper-proved probability arguments have not yet been translated into
Lean. No open case has been closed.

The following are precise, closed Lean propositions in namespace
`ProvenHashes.UMASH`. They are **definitions of propositions**, not axioms or
theorems. Their full expansions, including the literal hash, finite key space,
case predicates and quantifiers, are in
[UMASHObligations.lean](lean/ProvenHashes/UMASHObligations.lean) and
[UMASHAssembly.lean](lean/ProvenHashes/UMASHAssembly.lean).

| Missing Lean proposition | What blocks it |
| --- | --- |
| `PHXorSliceInjective` | Connect the literal carry-less multiplication loop to polynomial multiplication over GF(2), and prove the nonzero difference slice injective. |
| `RankLemma64` | Prove the shifted carry-less-product fibre count; the rank argument has not been formalized. |
| `PrimaryPHBound` | Apply PH slice injectivity to the actual OH expression. The 852-mask set and the general 852² projection transfer are proved, but this application is missing. |
| `DifferentChunkCountsBound` | Expose the independent last-pair keys and apply the ENH product point-mass bound to the two different chunk counts. |
| `OddPHBound` | Formalize the odd-difference refinement giving 17/q. |
| `SubcaseBBound` | Formalize the rank lemma, shuffler constraints, weighted checksum mask enumeration and their probability assembly. Only the final numerical inequality is checked. |
| `DifferentChecksumsBound` | Formalize the independent twist-key conditioning and combine it with the primary PH estimate. |
| `OneWordENHBound` | Formalize the signed-mask/folded-product certificate and show it covers every one-word ENH case. Only the numerical constant is checked. |
| `PHOneWordENHBound` | Formalize the checksum relation and PH+ENH certificate coverage. Only the numerical constant is checked. |
| `TagOnlyBound` | Formalize the tag/shuffler/checksum-mask count. Only the numerical constant is checked. |
| `ClosedTwoWordENHBound` | Formalize the supplied two-word certificate for r ≤ 31. |
| `ClosedPHTwoWordENHBound` | Formalize the supplied PH+ENH certificate for r = 0 or 4 ≤ r ≤ 35. |
| `DifferentBlockCountsBound` | Connect unequal polynomial lengths to the leading ENH coefficient point mass. |
| `EncodingInjectiveLong` | Prove that the overlapping byte/chunk encoding preserves distinct long messages. Validity and lengths of encoded blocks are proved. |
| `WeakENHBlockBound` | Formalize the weak folded-product/LP estimate with numerator 718333281557. Its rounding arithmetic alone is proved. |
| `WeakPrimaryProjection` | Assemble all block cases, encoding injectivity, short/long comparisons, and conditioning on distinct OH keys. This is the missing primary marginal premise for the certified envelope. |
| `ShortOneWordBound` | Prove the ≤8-byte packing/mixing argument and the exact short-message key bound. |
| `CertifiedAllPairs64` | Discharge `WeakPrimaryProjection` and `ShortOneWordBound`. Their implication to this proposition is proved. |
| `CertifiedAllPairs128` | Discharge `CertifiedAllPairs64`; its inheritance by the independent-multiplier fingerprint is proved. |
| `OpenENHOnly` | Open on paper: two ENH words change, no PH chunks change, 32 ≤ r ≤ 63. |
| `OpenPHENH` | Open on paper: exactly one PH chunk and both ENH words change, equal checksums, r ∈ {1,2,3} ∪ [36,63]. |
| `SharpPrimaryProjection` | The sharper primary marginal projection estimate is also open in the supplied ledger. Joint fingerprint estimates do not establish it. |
| `RequestedConditional55` | The requested implication from only the two open joint ENH propositions lacks the primary marginal premise. The corrected implication `SharpPrimaryProjection → Published64` is proved. |
| `Published64` | Discharge `SharpPrimaryProjection`, or supply a different unconditional primary argument. |

The published 83-bit fingerprint proposition is also defined as `Published128`;
no proof of that additional published claim is asserted.

The two paper-open propositions expand as follows. `uniformProb` is the exact
cardinality ratio on all `OHKey = Fin 34 → BitVec 64`; `jointEvent` means equality
of both OH components after both 64-bit halves are reduced modulo p.

```lean
def OpenENHOnly : Prop := ∀ (seed : Word) (x y : Block),
  x.Valid → y.Valid → sameCount x y → phDiffCount x y = 0 → enhChanges x y = 2 →
  32 ≤ enhValuation x y → enhValuation x y ≤ 63 →
  uniformProb (jointEvent seed x y) < (1 : ℚ≥0) / 2^87

def OpenPHENH : Prop := ∀ (seed : Word) (x y : Block),
  x.Valid → y.Valid → sameCount x y → dataChecksum x = dataChecksum y →
  phDiffCount x y = 1 → enhChanges x y = 2 →
  (enhValuation x y = 1 ∨ enhValuation x y = 2 ∨ enhValuation x y = 3 ∨
    (36 ≤ enhValuation x y ∧ enhValuation x y ≤ 63)) →
  uniformProb (jointEvent seed x y) < (1 : ℚ≥0) / 2^87
```

## Corrections

1. **Primary versus joint projection.** The source ledger does not justify the
   requested 55-bit implication from just the two open ENH cases. Those are
   joint fingerprint propositions. The corrected, proved conditional theorem
   explicitly assumes `SharpPrimaryProjection`. This is a dependency correction,
   not a proved counterexample to the requested logical implication. The original
   proposition remains in the obligation list rather than being silently replaced.
   The premise is an all-message comparison-polynomial identity bound; deriving
   it from block projection estimates, encoding and short-message lemmas is also
   part of the outstanding work.
2. **Fingerprint multipliers.** `hash128` has two independent polynomial
   multipliers, matching the C architecture. The supplied Python API can reuse
   one multiplier; that variant is modeled separately as `referenceFingerprint`.
   The supplied review records a shared-multiplier block-swap issue, so no 83-bit
   theorem is transferred to that variant.
3. **Mask membership.** An actual congruent pair yields an XOR mask in S.
   Membership is not asserted to be sufficient for an arbitrary pair; the signed
   digit pattern also matters. The model uses actual modular equalities.
4. **Source availability.** No fourth-review file was present, and the specified
   `sections/appendix_adversarial.tex` had no UMASH match in the current checkout.
   The available GAP/SUMMARY/second/third/fifth reviews, subcase-b and ENH ledgers,
   filed issue and Python reference were used. Copies of the main mathematical
   ledgers are retained under `lean/sources/`.

## Milestones

| Deliverable | Status |
| --- | --- |
| Literal block/hash model | Implemented: 64-bit PH, wrapped ENH inputs, high-word tag, triangular fold, OH checksum/twist/shufflers, 852-mask definition, projection modulo p, overlapping encoding, short branch, double-pumped recurrence, finalizer and both fingerprint key variants. |
| Mathematical model checks | Proved: p prime, signed-support recurrence soundness and completeness, S.card = 852, actual projection implies S×S coverage, injective-slice 852² transfer, integer product point masses, projection fibres, encoding validity/lengths, finalizer inverse, literal recurrence's field polynomial interpretation and root bound. |
| Key sampling | Proved: each of the 561 word pairs repeats with probability 1/q, their union has probability at most 561/q, and distinct-key acceptance has probability at least (q−561)/q. |
| Paper-proved block cases | Incomplete as listed above. Arithmetic checks of their constants are not labeled as case probability proofs. |
| All-pairs 25.6-bit envelope | Exact envelope arithmetic and conditional end-to-end reduction proved; unconditional primary premises remain missing. |
| Conditional published 55 bits | Proved with the explicit sharp primary marginal premise; not proved from only the two joint ENH propositions. |
| Fingerprint inheritance | Proved for every pair, seed and length, using the independent two-multiplier key distribution. The numerical certification remains conditional on its primary premises. |

The finite key model is ideal uniform sampling: 34 distinct OH words at the full
hash level, and multiplier(s) independently uniform on `{2,…,p−1}`. No theorem
about Salsa20 key expansion or the production sampler's distribution is claimed.
No upper cap on L is inserted in the all-pairs propositions. L is in 64-bit words;
messages are byte lists of length at most 8L, and L ≥ 1.

## Signatures

All names below are in `ProvenHashes.UMASH`; see
[AuditAll.lean](lean/AuditAll.lean) and [AuditAll.txt](lean/AuditAll.txt) for the
complete signatures and axiom reports.

```lean
PrimaryIdentityBound (B : ℚ≥0) : Prop := ∀ (seed : Word) (x y : Message),
  x ≠ y → uniformProb (fun k : DistinctOHKey =>
    comparisonPolynomial k.val seed x = comparisonPolynomial k.val seed y) ≤ B
maskSet_card : maskSet.card = 852
unfinalize_finalize (x : Word) : unfinalize (finalize x) = x
finalize_injective : Function.Injective finalize
distinct_key_acceptance : DistinctKeyAcceptance
eval_blockPolynomial (f : ℕ) (xs : List Chunk) :
  (blockPolynomial xs).eval (f : Field) = (polyReduce f xs : ℕ)
conditional55_with_primary : SharpPrimaryProjection → Published64
certified64_of_primary_and_short :
  WeakPrimaryProjection → ShortOneWordBound → CertifiedAllPairs64
certified128_of_primary_and_short :
  WeakPrimaryProjection → ShortOneWordBound → CertifiedAllPairs128
certifiedAllPairs128_of_64 : CertifiedAllPairs64 → CertifiedAllPairs128
certifiedSlope_better_than_25_6 : certifiedSlope^5 < 1 / (2 : ℚ≥0)^128
```

For every seed and pair of messages, `fingerprint_collision_le_primary` proves
the unconditional inequality between their collision probabilities. The
polynomial collision theorems explicitly require unequal coefficient
polynomials; they do not assume that distinct messages automatically give them.

The exact paper envelope is represented with q = 2^64, p = 2^61−1,
A = 718333281557/(q−561), r(L) = min(1, 2⌈L/32⌉/(p−2)), and
ε(1) = 1/(q−561), ε(L≥2) = A + (1−A)r(L).
The proved per-word slope is

```
1656363775600634047357796737589 /
85070591730234613168007331077920132390
```

`certifiedEnvelope_per_word` proves ε(L) ≤ L·slope, with equality at L=2.
The fifth-power inequality is an exact rational certificate of a score greater
than 25.6 bits, without floating-point logarithms. Applying it to actual hash
probabilities still requires the missing primary and short-message proofs.

## Axioms, builds, reproduction and commits

The combined `lake build` passed. The complete audit checked **187 local
theorems and lemmas**, including the inherited classic modules. Every axiom set
is a subset of `{propext, Classical.choice, Quot.sound}`. No reported module
contains `sorry`, `admit`, `native_decide`, a custom axiom or an unsafe declaration.
The finalizer and mask certificate are checked by the kernel; no compiler-trust
axioms are used.

- Build log: [lean/BuildUMASH.txt](lean/BuildUMASH.txt), also
  `~/agents/lean-umash/BuildUMASH.txt` on the Xeon.
- Complete signatures/axioms: [lean/AuditAll.txt](lean/AuditAll.txt).
- Machine-readable audit: [lean/Verification.json](lean/Verification.json).
- Source/input SHA-256 manifest: [lean/SourceHashes.json](lean/SourceHashes.json).
- Checkpoint builds: [lean/CheckpointBuild.txt](lean/CheckpointBuild.txt) and
  per-module logs under `lean/logs/`.
- Reference-model check: [lean/ModelVerification.json](lean/ModelVerification.json),
  PASS for 120 outputs (20 lengths × 3 key/message profiles × 2 compressors).
  Profiles exercise wrapped additions and multipliers near p. This is a smoke
  check, not a formal equivalence theorem or a collision-rate experiment.
- Verified proof-source commit: `c85c49bef9ac5b71f17980874adddc66c9146376`.
  The subsequent documentation commit is recorded in `lean/ReportCommit.txt`;
  its Lean sources are unchanged from the verified proof commit.

The workspace is `thomas-ahle@hardware.normalcomputing.net:~/agents/lean-umash`,
copied with its `.lake` cache from `~/agents/lean-classic`. Every Lean build uses
`nice -n 10 taskset -c 56-63` and `LEAN_NUM_THREADS=8`. Mathlib is never rebuilt;
the Mac is used only for edits and transfers. Reproduce on the Xeon with
`bash reproduce.sh`. The local source and audit mirror is [lean/](lean/).

Lean: 4.24.0, commit `797c613eb9b6d4ec95db23e3e00af9ac6657f24b`.
Mathlib: `f897ebcf72cd16f89ab4577d0c826cd14afaafc7`.
The copied classic lane was not a Git repository; its underlying hash lane
reported `e70213feed3dac6280fe8d7ecdc35dcfd0cf1cc8`.
