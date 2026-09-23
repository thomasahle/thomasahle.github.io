# Lean proofs for the textbook rows

Machine-checked collision bounds for the post's textbook constructions (Lean 4.24.0,
Mathlib v4.24.0; `lake build`, then `lake env lean Audit.lean`). [AUDIT.txt](AUDIT.txt)
is the build and `#print axioms` transcript: every theorem below depends only on
`propext`, `Classical.choice` and `Quot.sound`.

| Row | Theorem | Statement |
|---|---|---|
| Horner / polynomial | `ProvenHashes.polynomial_collision_gf64` | degree-(L-1) polynomial over GF(2^64): collision probability at most (L-1)/2^64 |
| BRW | `ProvenHashes.BRW.collision_bound_gf64`, `ProvenHashes.ChartScores.brw_whole_bits` | BRW polynomial of degree at most 2L-1; whole-bit score floor 63 |
| Injective recurrence, one chain | `ProvenHashes.Recurrence.collision_bound_gf64` | ceil(L/2)/2^64 with three independent keys |
| Injective recurrence, eight lanes | `ProvenHashes.LanesRefined.word_collision_bound_refined_gf64`, `ProvenHashes.LanesRefined.refined_ratio` | (rounds(L) + J - 1)/2^64 with J = min(8, ceil(L/2)) nonempty lanes, at most L/2^64: score at least 64. Supersedes the envelope `EightLanes.word_collision_bound_gf64`, (rounds(L)+7)/2^64, which gave 61 |
| NH over GF(2^64) | `ProvenHashes.nh_collision_bound` | 2^-64 |
| Simple tabulation | `ProvenHashes.tabulation_collision_exact` | exactly 2^-64 on single words |
| Vector multiply-shift | `ProvenHashes.vectorMultiplyAddShift_collision_exact`, `ProvenHashes.vectorMultiplyAddShift64_formula_bound` | fixed-length vectors, independent uniform 128-bit coefficients and additive key, high 64 bits: exactly 2^-64 |

`MultiplyShift.lean` states the scalar multiply-shift bound as a proposition only; it is
imported for definitions and is not claimed as a theorem. The vector theorem above is the
one the post's row uses.
