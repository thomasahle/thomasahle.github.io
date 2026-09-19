STATUS: COMPLETE

## Remaining obligations

None. All requested paper/model A bounds, byte interfaces, evaluation equalities, and score minima are proved and audited.

## Milestones

- 89 new theorems across 10 modules, with a successful `lake build` checkpoint after each theorem.
- Paper collision envelope `(p(L)+1)/2^64`, with 39 independent key words.
- Model A collision envelope `(degreeBudget L+p(L))/2^64`, with exactly 64 random key bytes; covers fixed-length and at-most inputs.
- General Frobenius root count and reduced-CLNH difference universality discharged.
- Serial Horner = physical exact-count schedule = bounded 128-bit lazy evaluation, as equal functions, for every positive stride.
- Both certified envelope scores have exact minimum 63.0 at `L=1`; memo tables proved.
- Final complete build, new and inherited axiom audits, and all 464 C/Lean vectors passed. C additionally passed portable/XMM/YMM/ZMM, strides 1–8, eager/lazy, and dispatched one-shot checks.
- Sources, audit output, and build/vector evidence are mirrored under `./lean/` and `./logs/v3/`.

## Signatures

Namespace: `ProvenHashes.ChainHash.V3`. Here `p(L)=blocks (8*L)`. Lengths follow the memo's 64-bit encoding domain; stride zero is outside the implementation's schedule domain.

```lean
theorem paper_collision_bound_bytes (L : ℕ) (hL : 8*L < 2^64)
    (m m' : List UInt8) (hm : m.length ≤ 8*L) (hm' : m'.length ≤ 8*L)
    (hne : m ≠ m') :
    uniformProb (fun k : Key39 => hashBytes k m = hashBytes k m') ≤ paperEpsilon L

theorem modelA_collision_bound_bytes (L : ℕ) (hL : 0 < L)
    (hcap : 8*L < 2^64) (m m' : List UInt8)
    (hm : m.length ≤ 8*L) (hm' : m'.length ≤ 8*L) (hne : m ≠ m') :
    uniformProb (fun k : Fin 64 → Byte =>
      modelAHashBytes k m = modelAHashBytes k m') ≤ modelAEpsilon L

theorem complete_evaluation_independence (k : ℕ) (hk : 0 < k) :
    scheduledHash k = hash ∧ lazyHash = hash

theorem paper_score_minimum :
    IsLeast (Set.range (fun L : {L : ℕ // 0 < L} => score paperEpsilon L.val)) 63

theorem modelA_score_minimum :
    IsLeast (Set.range (fun L : {L : ℕ // 0 < L} => score modelAEpsilon L.val)) 63
```

Every elaborated theorem signature is recorded in [lean/ChainHashV3Audit.txt](lean/ChainHashV3Audit.txt). Full definitions, proof milestones, scope, and input provenance are in [LEAN_CHAINHASH_V3_STATUS.md](LEAN_CHAINHASH_V3_STATUS.md).

## Axioms

Only `propext`, `Classical.choice`, and `Quot.sound`; no unresolved probabilistic stage hypotheses. No `sorry`, `admit`, `native_decide`, custom axiom, or unsafe declaration in the v3 proof modules. Both [ChainHashV3Audit.txt](lean/ChainHashV3Audit.txt) and [CombinedV3Audit.txt](lean/CombinedV3Audit.txt) passed the axiom allowlist check.

## Commits, builds, and reproduction

- Proof/evidence commit: `54410e433a1d9f79cd4104dc9fc2cdea72164f07`.
- Parent: `<xeon-work>/lean-chainhash-modelA`, commit `a3939c01b87d962ae170776b656a154300d5f3ca`.
- Lean 4.24.0; Mathlib `f897ebcf72cd16f89ab4577d0c826cd14afaafc7`.
- Remote lane: `<xeon-host>:<xeon-work>/lean-chainhash-v3-64`.
- Build log: `<xeon-work>/lean-chainhash-v3-64/logs/v3/final-build.log`, mirrored at [logs/v3/final-build.log](logs/v3/final-build.log).
- Per-theorem logs and exact source-prefix hashes: `logs/v3/ChainHashV3*-THEOREM.{log,sha256}`.
- Audit summary: [logs/v3/audit-summary.txt](logs/v3/audit-summary.txt).
- Vector results: [logs/v3/vectors.log](logs/v3/vectors.log); C and Lean output SHA-256 `732b082f59e860e8171016c6919be1d863320cfbafe358ab3f52900de6017103`.
- Reproduce from the Mac: `./scripts/reproduce-remote.sh`; from the Xeon lane: `./scripts/reproduce-v3.sh`.

All computation used `nice -n 10 taskset -c 48-55` and `LEAN_NUM_THREADS=8` on the Xeon with the copied `.lake` cache. Mathlib was not rebuilt; the Mac was used for edits, inspection, and transfers only.

## Corrections

None to the memo's requested bounds or score values. The length and positive-stride domains appear explicitly in the signatures. Scores refer to the certified envelopes; the executable vector reference is consistency evidence, not a formal C refinement proof.
