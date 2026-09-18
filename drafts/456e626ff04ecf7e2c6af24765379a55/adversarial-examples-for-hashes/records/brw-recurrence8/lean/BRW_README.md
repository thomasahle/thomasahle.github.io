# BRW and eight-lane recurrence

Run all compilation on the Xeon, inside `~/agents/lean-brw/lean`:

```sh
./reproduce.sh
```

The lane was copied with `cp -a` from `~/agents/lean-hash`, including `.lake`.
No Mathlib rebuild or local Mac compilation is needed. `env.sh` selects Lean
4.24.0, and the scripts set `LEAN_NUM_THREADS=8`, `nice -n 10`, and CPU set 40-47.
Mathlib is v4.24.0, commit f897ebcf72cd16f89ab4577d0c826cd14afaafc7.

- `ProvenHashes/BRW.lean`: Bernstein's base cases and power-of-two recursion,
  degree bound, recursive injectivity proof, and GF(2^64) collision theorem.
- `ProvenHashes/EightLanes.lean`: shared three-key recurrence, one independent
  combining key, round-robin dealing, odd-word zero padding, and collision bound.
- `ProvenHashes/ChartScores.lean`: exact real score formulas, probability floors,
  minimizers, and the integer BRW chart label.
- `BRWAudit.txt`: signatures and axioms of every new theorem and lemma.
- `FullAudit.txt`: the parent proofs and every new theorem and lemma.
- `Reproduction.log`: full build and audit validation output.

`generate_audit.py` regenerates the audit commands before each reproduction.
`build.sh` rejects proof holes, custom axioms, unsafe declarations, and
nonstandard axioms, and checks the full audit inventory.

The bound in the BRW chart is (2L-1)/2^64. Its exact finite-range score minimum
is strictly greater than 63; the floor of that minimum is 63. This distinction
is proved, not numerically approximated. See the root STATUS.md for reporting
status and corrections. The eight-lane minimum is exactly 61 at L=1.
