STATUS: COMPLETE

## Completion

All requested results are proved, with the output-width scope correction below
matching the write-up exactly. There are no remaining proof obligations. The
full project build and all 35 theorem axiom audits pass.

## Milestones

- The original `ProvenHashes.MultiplyShiftBound` is proved by
  `ProvenHashes.multiplyShift_bound`, without changing its definition or quantifiers.
- Scalar and vector multiply-add-shift are strongly universal for `ℓ ≤ w`:
  every output pair has exact probability `1 / 2^(2ℓ)`, and distinct inputs
  collide with exact probability `1 / 2^ℓ`.
- The row theorem is `ProvenHashes.vectorMultiplyAddShift64`, with the direct
  arithmetic form `ProvenHashes.vectorMultiplyAddShift64_formula_bound`.
- `ProvenHashes.vectorMultiplyAddShift64_score` proves that the minimum over
  positive integer lengths of `log₂(L / (1 / 2^64))` is 64, attained at `L = 1`.

## Corrections and scope

The add-shift statements need an output-width restriction. We prove `ℓ ≤ w`,
including exactly the write-up's `w = ℓ = 64` case. They must not inherit the
odd-multiplier theorem's wider `ℓ ≤ 2*w` range. Without a restriction the claim
is false: `w = 2`, `ℓ = 4`, inputs 0 and 2 have collision probability `1/8`,
which exceeds `1/16`; this exact count is proved in
`multiplyAddShift_width_counterexample`. The corrected statement is precisely what the chart row
needs. The odd-multiplier proposition retains its original `ℓ ≤ 2*w` condition.

Keys for the vector theorem are
`(Fin L → ZMod (2^(2*w))) × ZMod (2^(2*w))`: all coefficients and the additive
key are independently uniform over the full word space. No oddness condition
is imposed on multiply-add-shift keys. The key cardinality is
`(2^(2*w))^(L+1)`. Inputs are distinct vectors in `Fin L → Fin (2^w)`, with the
same fixed length. No variable-length or short-seed claim is made.

`multiplyAddShift_val` and `vectorMultiplyAddShift_val` prove the exact
connection to `((a*x+b) mod 2^(2*w)) / 2^(2*w-ℓ)` and its vector sum variant.
Natural division here is logical right shift. The row uses 128-bit coefficients
and additive key, and the high 64 output bits.

Reference: [Thorup, High Speed Hashing for Integers and Strings, §§2.3, 3.3–3.4](https://arxiv.org/html/1504.06804).
The supplied local appendix mentions the multiply-shift baseline; the precise
full-key formula in the task is the formula formalized here.

## Signatures and axioms

`lean/MultishiftAudit.lean` prints every theorem signature and its transitive
axioms in the four multiply-shift modules. Its output is
`multishift-axioms.log`. Every one of the 35 audited theorems depends only on
`propext`, `Classical.choice`, and `Quot.sound`. No proof uses `sorry`, `admit`,
`native_decide`, custom axioms, or unsafe declarations. The reproduction script
checks both the source scan and the axiom allowlist. Probabilities use the
parent's exact `ℚ≥0` finite-cardinality ratios.

Principal signatures (namespace `ProvenHashes`):

```lean
theorem multiplyShift_bound : MultiplyShiftBound

theorem multiplyAddShift_strong (w ℓ : ℕ) (hℓ : ℓ ≤ w)
    (x y : Fin (2 ^ w)) (hne : x ≠ y) (u v : ZMod (2 ^ ℓ)) :
    uniformProb (fun k : ScalarMASKey w =>
      multiplyAddShift w ℓ k x = u ∧ multiplyAddShift w ℓ k y = v) =
      1 / (2 : ℚ≥0) ^ (2 * ℓ)

theorem multiplyAddShift_collision_exact (w ℓ : ℕ) (hℓ : ℓ ≤ w)
    (x y : Fin (2 ^ w)) (hne : x ≠ y) :
    uniformProb (fun k : ScalarMASKey w => multiplyAddShift w ℓ k x = multiplyAddShift w ℓ k y) =
      1 / (2 : ℚ≥0) ^ ℓ

theorem vectorMultiplyAddShift_strong (w ℓ L : ℕ) (hℓ : ℓ ≤ w)
    (x y : Fin L → Fin (2 ^ w)) (hne : x ≠ y) (u v : ZMod (2 ^ ℓ)) :
    uniformProb (fun k : VectorMASKey w L =>
      vectorMultiplyAddShift w ℓ L k x = u ∧ vectorMultiplyAddShift w ℓ L k y = v) =
      1 / (2 : ℚ≥0) ^ (2 * ℓ)

theorem vectorMultiplyAddShift_collision_exact (w ℓ L : ℕ) (hℓ : ℓ ≤ w)
    (x y : Fin L → Fin (2 ^ w)) (hne : x ≠ y) :
    uniformProb (fun k : VectorMASKey w L =>
      vectorMultiplyAddShift w ℓ L k x = vectorMultiplyAddShift w ℓ L k y) =
      1 / (2 : ℚ≥0) ^ ℓ

theorem vectorMultiplyAddShift64 (L : ℕ)
    (x y : Fin L → Fin (2 ^ 64)) (hne : x ≠ y) :
    uniformProb (fun k : VectorMASKey 64 L =>
      vectorMultiplyAddShift 64 64 L k x = vectorMultiplyAddShift 64 64 L k y) ≤
      1 / (2 : ℚ≥0) ^ 64

theorem vectorMultiplyAddShift64_score :
    IsLeast {s : ℝ | ∃ L : ℕ, 1 ≤ L ∧
      s = Real.logb 2 ((L : ℝ) / (1 / (2 : ℝ) ^ 64))} 64
```

## Reproduction and provenance

Remote workspace: `thomas-ahle@hardware.normalcomputing.net:~/agents/lean-multishift`.
It was created with `cp -a ~/agents/lean-hash ~/agents/lean-multishift`, including
the `.lake` cache. The parent records its status in `STATUS.md`; the named
`LEAN_STATUS.md` was not present there.

Run on the Xeon:

```sh
cd ~/agents/lean-multishift
./reproduce.sh
```

The script uses `nice -n 10 taskset -c 64-71`, `LEAN_NUM_THREADS=8`, and the
copied Lean 4.24.0 / Mathlib cache. It builds the complete project, checks for
forbidden proof escapes, and runs the axiom audit. It refuses to build on macOS.
No Lean compilation or mathematical computation was run on the Mac.

Build log: `~/agents/lean-multishift/multishift-build.log`, mirrored locally as
`./multishift-build.log`. Audit log: the corresponding `multishift-axioms.log`.
Lean sources and project metadata are mirrored in `./lean/` without `.lake`.

- Parent commit: `e70213feed3dac6280fe8d7ecdc35dcfd0cf1cc8`.
- Mathlib commit: `f897ebcf72cd16f89ab4577d0c826cd14afaafc7` (`v4.24.0`).
- Proof/audit commit: `ce6b48f7b4fb8af47ca09f2a7471d054ef120380`.
- Final reproduction: `Build completed successfully (7363 jobs)`;
  `AUDIT PASSED: 35 theorems; only the three standard axioms.`
