# HighwayHash verification in ProvenHashes

Lean 4.24.0; Mathlib `v4.24.0` (commit
`f897ebcf72cd16f89ab4577d0c826cd14afaafc7`).

The full-key counting bridge and unconditional numerical bounds are proved.
Exactly `56165 * 2^184` of the `2^256` keys satisfy the trail. Its probability is
`56165 / 2^72`, which is a lower bound for the complete-state collision event
and each of the 64-, 128-, and 256-bit output collision events. Lean also proves
`59.807 < log2(12 * 2^72 / 56165) < 59.808`, justifying 59.81 bits. See
[the milestone report](../LEAN_HIGHWAY_STATUS.md).

The authoritative repository is on the Xeon at
`<xeon-host>:<xeon-work>/lean-highway`.
Run there:

```bash
cd <xeon-work>/lean-highway/lean
./build-highway.sh
```

The script refuses to build on macOS. It retains the existing Mathlib cache,
loads the pinned toolchain, overrides `LEAN_NUM_THREADS=8`, and runs builds,
Lean evaluation and C execution under `taskset -c 80-87 nice -n 10`.
The frozen supplied C files must be in `../materials/`.

The script checks the reference hashes, runs `lake build`, generates signatures
and `#print axioms` for all explicit Highway theorems and proof-bearing
equivalences, rejects additional axioms, and compares twelve C/Lean vector rows.
Each row contains all sixteen state words and all seven output words.

Entry points:

- `ProvenHashes/Highway.lean`: the audited import closure.
- `HighwayAudit.txt`: complete signatures and axiom lists.
- `BridgeFinalBuild.txt`: the complete bridge build, audit and comparison result.
- `ProvenHashes/Highway/Model.lean`: frozen complete-packet algorithm and pair.
- `ProvenHashes/Highway/Trail.lean`: the algebraic full-state collision theorem.
- `ProvenHashes/Highway/PacketOne.lean`: actual `E2` to byte-condition equivalence.
- `ProvenHashes/Highway/ReducedCount.lean`: the reduced 40-bit count.
- `ProvenHashes/Highway/BridgeWords.lean`: actual reset words and pad-byte arithmetic.
- `ProvenHashes/Highway/BridgeCoordinates.lean`: bijection retaining all 96 active bits.
- `ProvenHashes/Highway/BridgePredicate.lean`: actual E₂ equals the counted byte predicate.
- `ProvenHashes/Highway/BridgeCount.lean`: full-key bijection and `exact_trail_count`.
- `ProvenHashes/Highway/Bound.lean`: event monotonicity and reusable count-to-bound implications.
- `ProvenHashes/Highway/ExactBound.lean`: unconditional state and output bounds.
- `ProvenHashes/Highway/Score.lean`: exact power certificates and logarithmic score interval.
- `tests/`: C/Lean vectors, the unchanged reference header and audit script.

The older ProvenHashes modules and their `Audit`/`FullAudit` reports are retained
from the original project. They are not the HighwayHash verification report.

The full count follows from 56 free byte-coordinate bits, 56,165 relevant points,
and two unrestricted 64-bit key words. The 32 class bits are fixed. The pad target
may depend on the subtraction carry branch; its uniqueness is proved for all
allowed targets, including the exceptional carry boundary. The proof enumerates
only the small residue checks documented in the milestone report. The score uses
exact integer-power inequalities checked by `norm_num`; all proofs are checked
by Lean's kernel.
