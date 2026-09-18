STATUS: COMPLETE

## Remaining obligations

None. The original `K0 ≤ keyCard` statement is proved for the existing exact
`Key` definition, and the numeric core/idealHash and score theorems have no
cardinality or character-sum hypothesis.

## Corrections

None. `K0 = 189729088763903999` and `Key` are unchanged. The original conditional
transport lemmas are retained alongside their unconditional corollaries.

## Milestones

1. Preserved the compiled dyadic interval-character bound from round 1:
   `C = 43 * 1518500250 = 65295510750`.
2. Completed and audited the recovered subgroup-character indicators, Möbius
   inversion, exact-order discrepancy, and arithmetic constants
   `φ((p-1)/7) = 67744512000000000`, `2^ω((p-1)/7) = 2048`.
3. Specialized the count to the exact interval `1..H`, `H = 2^60-2^56-1`.
4. Constructed an injection `keyTargets × Fin 6 → Key`. For `m = (p-1)/7`,
   `r = 188232082384791343`, and `ζ = 37^m`, the six values
   `x^r * ζ^(i+1)` have order `p-1`, seventh power `x`, and are distinct.
5. Proved `6Hφ(m)/(p-1) - 6*2048*C ≤ keyCard` and then `K0 ≤ keyCard`.
   The exact real lower-bound expression exceeds `K0` by
   `460694710548221 / 465826870548221`.
6. Instantiated unconditional core and idealHash bounds with denominator `K0`,
   and the exact rational score lower bound `54.2267`.
7. Completed the Xeon build, full axiom audits, source mirror, reproduction
   scripts, and updated `LEAN_POLYMUR_STATUS.md`.

## Signatures

Namespace: `ProvenHashes.Polymur`.

```lean
theorem cardinalityCertificate : CardinalityCertificate
-- CardinalityCertificate is definitionally K0 ≤ keyCard.

theorem core_collision_bound_K0_unconditional {n : ℕ}
    (a b : Bytes) (ha : a.length ≤ n) (hb : b.length ≤ n) (hne : a ≠ b) :
    uniformProb (fun k : Key => core k.val a = core k.val b) ≤ (D n : ℚ≥0)/K0

theorem idealHash_collision_bound_K0_unconditional {n : ℕ}
    (a b : Bytes) (ha : a.length ≤ n) (hb : b.length ≤ n) (hne : a ≠ b)
    (s : Key → Word) (tweak : Word) :
    uniformProb (fun k : Key => idealHash k.val (s k) tweak a =
      idealHash k.val (s k) tweak b) ≤ (D n : ℚ≥0)/K0

theorem score_lower_unconditional : (54.2267 : ℝ) ≤ score
```

Sources: [KeyCardinality.lean](lean/ProvenHashes/Polymur/KeyCardinality.lean),
[Certified.lean](lean/ProvenHashes/Polymur/Certified.lean).

## Axioms

The four displayed theorems depend only on `propext`, `Classical.choice`,
and `Quot.sound`. The generated audit covers all **114 Polymur proof declarations**
in 15 modules; the inherited audit covers **57 proof declarations**. All pass.
No reported module contains proof placeholders, custom axioms, unsafe declarations,
`native_decide`, or `Lean.ofReduceBool`.

- [Every Polymur signature and axiom list](lean/PolymurAxioms.txt)
- [Generated audit source](lean/PolymurAudit.lean)
- [Inherited axiom lists](lean/FullAudit.txt)

## Commits

Authoritative repository: `thomas-ahle@hardware.normalcomputing.net:~/agents/lean-polymur`.

| Milestone | Commit |
|---|---|
| Dyadic character bound, retained from round 1 | `22e3aa4ed332a7857320669e5f785506f0efa1ed` |
| Subgroup/Möbius counts and interval specialization | `6804f6e9e345da52e51922ca1088b6efe5dd76af` |
| Unconditional cardinality certificate | `03bb6225501ca271c6459e0da6f55e753cc22538` |
| Unconditional corollaries, build and full audits | `80ff535c76f55257bc16ca5cf0113b52547414dc` |

## Build logs and reproduction

Full `lake build` succeeded (7375 jobs, using the existing dependency cache).
Every newly added lemma was followed by `lake build`. All compilation ran on
the Xeon with `nice -n 10`, `taskset -c 72-79`, and `LEAN_NUM_THREADS=8`.

- [Final build log](lean/CardinalityBuild.log)
- [Toolchain, Mathlib revision, build and Polymur audit](lean/CardinalityVerification.txt)
- [Inherited build and audit](lean/Verification.txt)
- Remote logs: `/home/thomas-ahle/agents/lean-polymur/lean/CardinalityBuild.log`,
  `CardinalityVerification.txt`, and `cardinality-logs/`.
- Lean 4.24.0 (`797c613eb9b6d4ec95db23e3e00af9ac6657f24b`).
- Mathlib v4.24.0 (`f897ebcf72cd16f89ab4577d0c826cd14afaafc7`).

Reproduce on the Xeon without rebuilding Mathlib:

```bash
cd ~/agents/lean-polymur
bash lean/build_polymur.sh
bash lean/build.sh
```

[Reproduction script](lean/build_polymur.sh). The complete Lean source mirror
is in `./lean/`; the updated lane report is [LEAN_POLYMUR_STATUS.md](LEAN_POLYMUR_STATUS.md).
