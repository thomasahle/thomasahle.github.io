The corrected bound is now unconditional in the reference model, and both previously open joint cases are closed on paper. The remaining proof gap is the sharp primary block constant. The published 55-bit UMASH-64 and quadratic 83-bit fingerprint claims remain neither proved nor refuted.

Put q = 2^64 and p = 2^61−1, and measure L in eight-byte words. For every fixed seed and every two distinct inputs of at most 8L bytes, with 34 OH words sampled uniformly without replacement (IID sampling is also covered) and an independent multiplier uniform on {2,…,p−1}, the certified UMASH-64 collision envelope is

```
ε(1)     = 1/(2^64−561)
ε(L ≥ 2) = A + (1−A) min(1, 2 ceil(L/32)/(2^61−3))
A        = 3125/(2^64−561).
```

Equal-length distinct short inputs never collide. A simpler, coarser form is

```
Pr[UMASH-64 collision] ≤ 423 ceil(L/512) / 2^61.
```

The tighter envelope gives a score of **53.38 bits**, attained at L = 2 under both the fixed-length and at-most-length conventions. At 1 KB / 1 MB / 1 GB (2^10 / 2^20 / 2^30 bytes), −log₂ ε is **52.36 / 47.93 / 37.9999**, versus the published 55 / 47 / 37. The corrected envelope beats the published one from **L = 5633 words (about 44 KB)** upward, because its polynomial slope is half the published per-word slope. The coefficient 423 is the least integer coefficient for this coarser assembly; the 53.38-bit score is computed from the tighter envelope above.

The primary block ledger, in units of 1/q under IID OH words, is a maximum over disjoint cases:

| Block case | Proved numerator |
| --- | ---: |
| Unequal chunk counts | 82 |
| At least one differing PH chunk, allowing ENH changes | 1123 |
| PH data agree, final ENH data differ | 3125 |
| Same data, valid tags differ | < 1/8 |

Thus C = 3125. The PH improvement counts the low-lane mask event by the least valuation of the PH differences and ENH increments, rather than multiplying two separate lane-mask counts. The ENH improvement groups mask/pattern pairs that give the same lifting equation: F₄ = 36 and Fᵣ = 26r−75 for 5 ≤ r ≤ 63. For r ≥ 4, the numerator is min(2^r, 2Fᵣ−1), at most 3125. The lower-valuation ENH numerators are 2771, 1230, 508 and 24. These are upper bounds, not collision rates shown to be attained.

The tag-only row uses **PROOF2 Theorem 9.2**, whose review is now complete. It proves a primary, single-compressor bound below 1/(8q), not the separate joint tag-only statement. This discharges the dependency recorded in the PROOF3 review. Without that import, the independently established fallback has C = 269280 and a 46.96-bit score.

Both joint cases are now closed by reviewed paper proofs:

- **ENH-only (`OpenENHOnly`):** joint probability at most 4721784/2^128 under IID OH words, still below 2^-105 after conditioning on distinct words. This covers the formerly open two-final-word changes at high valuation.
- **PH+ENH (`OpenPHENH`):** for equal chunk counts and data XOR checksums, exactly one differing nonfinal PH chunk, both final ENH words differing, and minimum valuation r = 1,…,63, joint probability at most 170906186782/2^128 under IID words, still below 2^-90 after distinct-word conditioning. The proof handles the differing PH offsets directly and does not assume independence between the completed compressors.

**What remains:** the published primary envelope follows from any **C ≤ 255**, while the proved C is 3125: a factor of 12.25, or 3.6 bits, in the block constant. The Lean target 162 is a stricter sufficient target. Closing this gap requires sharper counts for both ENH-only and PH-differing blocks. For ENH, retaining the full high-word equation or averaging over the first operand could reduce the grouped-candidate count. For PH, tightening the low-word estimate alone still leaves a constant above 255, so the high projection must also be used. The scaled-model evidence suggests slack, but does not prove the required constants.

UMASH-128 inherits the full primary envelope by event inclusion, hence the same **53.38-bit score** and an envelope growing linearly with length. Closing the joint cases does not by itself prove its stronger quadratic 83-bit claim.

The proofs, reviews and reproducibility records are linked from the [public post’s UMASH appendix](index.html#appendix-umash):

- ENH-only joint closure and original assembly: [proof](records/umash-corrected/PROOF.md), [review](records/umash-corrected/VERDICT.md), [checks](records/umash-corrected/checks/README.md).
- PH+ENH joint closure and tag-only primary bound: [PROOF2](records/umash-corrected/proof2/PROOF2.md), [review](records/umash-corrected/proof2/VERDICT.md), [checks](records/umash-corrected/proof2/checks/README.md).
- Sharper primary constant and envelope: [PROOF3](records/umash-corrected/proof3/PROOF3.md), [review](records/umash-corrected/proof3/VERDICT.md), [checks](records/umash-corrected/proof3/checks/README.md).

These are natural-language proofs with integer certificates, three independent adversarial reviews, exhaustive scaled checks and production-width checks of the counting lemmas; they are not yet formalized in Lean. The model is `umash_reference.py` and its reference/Lean formulation. Production `umash.c` was not re-audited beyond the ENH fold. No production collision violating the published rate is established here.
