# Fifth UMASH review: independent verification

**All requested constants and lemmas are confirmed.** No claim was refuted. [VERDICT.md](VERDICT.md) contains the proofs, a claim-by-claim status table, all 15 shuffler results, and the primary-hash case ledger.

- D has 852 masks, divisibility counts **852, 248, 64, 2, 1**, and every nonzero mask has top bit at least 60.
- Lemma 1 is proved and exhaustively checked for every δ,A,B at widths 8, 10, and 12: **69,809,995,776 triples**, zero failures.
- Lemma 2 is re-derived; **Σκ=32042**. All δ and high XOR targets were checked for ten specified width-12 tag pairs. An additional 120 selected 64-bit instances were counted exactly.
- The checksum pattern bound gives **ΣW=101/2+909712/2^64<51**.
- Source-derived shufflers give **max ceil(Φ)=111924178297**, at the one-bit shuffler. The x<2^63 restriction and inverse equations are verified.
- The tag-only constants are **195840/q**, **61·2^-52**, and **11946240·2^-116**, with q=2^64.

The independent analysis also strengthens the two-word ENH-only cases:

| Minimum valuation r | Primary reduced bound | Joint reduced bound |
|---:|---:|---:|
| 1 | 1568/q | 777728/q² |
| 2 | 1020/q | 261120/q² |
| 3 | 24/q | 384/q² |

These follow from quadratic congruence and product-counting arguments, checked against **2,111,832,064** additional key pairs. They imply the review's larger constants. All enumerations ran on the Xeon at nice level 10, with at most 32 task compute threads. The Mac ran no enumeration.

The **sharp primary all-pairs bound remains open**. General PH differences have the prior 725904/q bound, with sharper odd-word and single-word cases. One-word ENH-only changes have at most 32042/q, with smaller valuation-dependent bounds. Two-word ENH-only changes with r≥4 have 2^r/q. Tag-only primary collisions have the bound above. These are valid reduced bounds, not a complete proof of the advertised primary envelope; the polynomial root term must also be included.

The joint <2^-87 threshold remains unproved for ENH-only two-word changes with r≥32, and one PH plus two-word ENH with equal checksums when r∈{1,2,3} or r≥36. No counterexample is established.

Deliverables:

- [VERDICT.md](VERDICT.md): derivations, status table, source correspondence, primary and joint coverage.
- [certificates/manifest.json](certificates/manifest.json): environment and source/certificate hashes.
- [certificates/arithmetic.json](certificates/arithmetic.json): exact constants and strict comparisons, including distinct-key conditioning.
- [certificates/masks.json](certificates/masks.json), [certificates/phi.json](certificates/phi.json): complete finite certificates.
- [code/README.md](code/README.md), [code/run_xeon.sh](code/run_xeon.sh): independent code and reproduction command.

The review's full numbered text was unavailable; the omitted weight definition and bijection were reconstructed and their constants matched exactly. The prior <65/q single-PH-word theorem and GAP's exact PH families are identified as background results, not newly rerun certificates. This is computer-assisted verification, not proof-assistant formalization.
