STATUS: COMPLETE

## Remaining obligations

None for this integration. Both requested model-A reference bounds are
proved, all shipped paths match the Lean strided pairing, and the one final
`verify.sh` run passed. The repo is committed, clean, and not pushed.

## Milestones

- Final `verify.sh` passed once with model A included: 583 axiom audits,
  no disallowed declarations, and a successful 7424-job Lake build.
- Imported `SeededPH`, `ModelAStream`, and `ModelA` unchanged from the completed
  lane; 36 model-A declarations, of which 24 are new to this repo.
- All 24 newly imported theorem checkpoints passed separate Xeon builds with
  `LEAN_NUM_THREADS=8`, `nice -n 10 taskset -c 0-7`.
- The default library imports and generated axiom audit include model A:
  583 total theorem/lemma declarations across 69 modules.
- Updated the root and Lean READMEs, SEEDED_THEOREMS, source provenance,
  and exact theorem catalogue. Fixed catalogue truncation at embedded `:=`.
- Model B has written proofs but no complete Lean theorem in the source lane;
  its formalization remains outside this requested integration.
- All 324 C++ reference/header versus Lean encoding/evaluator vectors passed.
  These comprise 84 archived vectors and 240 model-A vectors.
- C/C++ tests passed: 21,479 differential cases and all 92 frozen vectors in
  both hardware and portable builds, plus key-schedule/C99 checks.

## Corrections

No statement or constant was weakened. The current shipped header uses
strided pairs `(w0,w2),(w1,w3)` on portable, NEON, baseline and pipelined XMM,
YMM VPCLMUL, and ZMM VPCLMUL paths. No disagreement was found. The adjacent
pairing in `chainhash-x86/SPEC.md` belongs to a separate 1024-byte hash family.
`ByteEncoding`, `KeyLayout`, and `referenceHash_matches` already match the
shipped header and remain unchanged. NEON was source-audited; runtime tests
ran on the Xeon, exercising both YMM and ZMM (detected width 2).

The vector evaluator directly uses the Lean proof's encoding definitions and
executes the arithmetic using Nat bit operations. This is differential test
evidence, not a new formal C/compiler/SIMD refinement theorem. The existing
formal word/field correspondence is `referenceHash_matches`.

## Signatures

Namespace `ProvenHashes.ChainHash.ModelA`:

```lean
theorem reference_collision_bound_fixed (L : ℕ) (hL : 0 < L)
    (m m' : Message) (hm : m.length = 8 * L) (hm' : m'.length = 8 * L) (hne : m ≠ m') :
    uniformProb (fun k : Key => referenceHash (expandedKey k) m = referenceHash (expandedKey k) m') ≤
      epsilonFixed L
```

```lean
theorem reference_collision_bound_atMost (L : ℕ) (hL : 0 < L) (hcap : 8 * L < 2 ^ 64)
    (m m' : Message) (hm : m.length ≤ 8 * L) (hm' : m'.length ≤ 8 * L) (hne : m ≠ m') :
    uniformProb (fun k : Key => referenceHash (expandedKey k) m = referenceHash (expandedKey k) m') ≤
      epsilonAtMost L
```

```lean
theorem expandedKey_power (k : Key) (j : Fin 32) :
    fieldRepr (expandedKey k ⟨j.val, by omega⟩) = k.1.1 ^ (j.val + 1)
```

```lean
theorem referenceHash_expanded (k : Key) (m : Message) :
    referenceHash (expandedKey k) m = fieldRepr.symm (hash k m)
```

`Key = (F × (Fin 3 → F)) × (F × (Fin 5 → F))` has cardinality `(2^64)^10`.
All ten sampled words are independent uniform, including zero. PH words are
`s^(i+1)`. The probability is an exact `ℚ≥0` finite-cardinality ratio.
`epsilonFixed` is `min 1 ((d(L)+n+1)/2^64)`, and `epsilonAtMost` is
`min 1 (envelopeNumerator L / 2^64)`, where `envelopeNumerator` is exactly
`E_A(L)` from the write-up. The length/distinctness premises shown above are
the theorem's domain; no stage, field, encoding, or finalizer hypothesis remains.
Full signatures are in `lean/THEOREM_STATEMENTS.md`.

## Axioms

The final `#print axioms` audit passed for all 583 theorem/lemma declarations.
Every axiom set is a subset of `propext`, `Classical.choice`, and `Quot.sound`;
both headline model-A reference bounds use exactly these three. The source
checks found no placeholders, native decision oracle, custom axioms, or unsafe
declarations. The complete output is [lean/VERIFICATION.txt](lean/VERIFICATION.txt).
All 76 source/configuration hashes recorded by that run match the committed files.

## Commits and environment

- Target repo: `<repos>/chainhash`.
- Inspected repo base: `8d18348ffc5a18ed284b45c0270089a5b5e63992`.
- Completed model-A source HEAD: `a3939c01b87d962ae170776b656a154300d5f3ca`.
- Source-lane documented proof milestone: `408d147c7f0aa7e123c6027b36a26821522e84a6`.
- Integration commit: `caf38345ea806e02153c54ba39234531fd7c402a`.
  Message: `Integrate complete model A proofs and audit shipped pairing`.
  Working tree is clean. No push was performed.
- Lean 4.24.0; Mathlib v4.24.0 at
  `f897ebcf72cd16f89ab4577d0c826cd14afaafc7`.
- Remote workspace: `<xeon-host>:<xeon-work>/chainhash-integrate`,
  copied with `cp -a` from `<xeon-work>/chainhash-repo-check`, including `.lake`.
  Mathlib was not rebuilt. All Lean and C/C++ execution was on the Xeon;
  the Mac performed edits, transfer, and inspection only.
- Shipped header SHA256:
  `963b6000faaf0a0bc5430bfdad791f14ab056e9271c2163d3a4301233d1345f6`.

## Build logs and reproduction

- Final log: `<xeon-work>/chainhash-integrate/logs/final-verify.log` on the Xeon;
  audit copy: [lean/VERIFICATION.txt](lean/VERIFICATION.txt).
  The audit records the pre-commit base HEAD plus exact source SHA256s; the
  integration commit above contains those audited sources and the log.
- Incremental logs: `<xeon-work>/chainhash-integrate/logs/modela-integration/`;
  summary: `lean/MODELA_INTEGRATION_BUILDS.txt`.
- Test evidence: `lean/VECTOR_AGREEMENT.txt`, `lean/HEADER_TESTS.txt`,
  and `lean/PAIRING_AUDIT.md`.
- Reproduction script: `lean/reproduce.sh`, run on the Xeon with Lean on PATH
  and the retained Mathlib cache. Incremental builds use CPUs 0–7/eight threads;
  `verify.sh` uses CPUs 0–31/32 threads and is run once at the end.
- Final sources and audit evidence are mirrored into this workspace's [lean/](lean/).
