STATUS: COMPLETE

## Remaining obligations

None. All five requested items are complete with the standard axioms only: (1) the byte-interface paper-model theorem, (2) the model A theorem with SPEC's envelope, (3) complete evaluation independence for every positive stride, (4) the score minima (127, 127, and `128 − log2 33`), and (5) reference-versus-Lean vector agreement including the header's hard-coded test vectors.

## Milestones

- 116 new theorems across 10 modules in `ProvenHashes.ChainHash.V3_128`, each with a
  successful complete `lake build` checkpoint and source-prefix hash.
- Paper certificate `min 1 ((p_B(L)+1)/2^128)` over 39 independent 128-bit key words, plus
  SPEC's block form `min 1 ((p+1)/2^128)`; 624-byte key layout and 16-byte output variants.
- Model A: SPEC's refined at-most-length envelope `min 1 ((p_B(L)+d_B(L))/2^128)` over exactly
  128 independent uniform key bytes (`kappa[a] = s^(a+1)`), the coarse envelope
  `min 1 ((p+32)/2^128)` in both `L`-form and SPEC's block form; short messages (≤ 128 bytes)
  through the general Frobenius root count (`d_B = 1`).
- Serial Horner = physical `k`-lane exact-count schedule = 256-bit lazy raw state
  (`X^128 = 0x87`), as equal hash functions, for every positive stride.
- Score minima, exact: 127 (paper), 127 (refined model A), `128 − log2 33 = 122.955606…`
  (coarse model A envelope), all attained at `L = 1`; SPEC's numerator tables proved.
- Reference-versus-Lean vectors: 625/625 PASS against `chainhash128_v3.h` (portable/XMM/YMM/ZMM × strides 1–8 × eager/lazy × schoolbook/Karatsuba, `with_backend`, dispatch, selftest) and the header's three hard-coded self-test vectors; C and Lean output SHA-256 `d9fc195c93bd358b4b26293d8a3fa2a2cec85f7cda185c9f68d068846ee029b6`.
- Sources, audits, build/vector logs mirrored under `./lean/` and `./logs/v3-128/`.

## Signatures

Namespace `ProvenHashes.ChainHash.V3_128`; `p_B(L) = blocks (8*L)`; `L` counts 64-bit words;
`Key39 = Fin 39 → Word 128`; `Byte = Word 8`; digests are `BitVec 128`.

```lean
theorem paper_collision_bound_bytes (L : ℕ) (hL : 8*L < 2^128)
    (m m' : List UInt8) (hm : m.length ≤ 8*L) (hm' : m'.length ≤ 8*L) (hne : m ≠ m') :
    uniformProb (fun k : Key39 => hashBytes k m = hashBytes k m') ≤ paperEpsilon L

theorem modelA_collision_bound_bytes (L : ℕ) (hL : 0 < L) (hcap : 8*L < 2^128)
    (m m' : List UInt8) (hm : m.length ≤ 8*L) (hm' : m'.length ≤ 8*L) (hne : m ≠ m') :
    uniformProb (fun k : Fin 128 → Byte => modelAHashBytes k m = modelAHashBytes k m') ≤
      modelAEpsilon L

theorem complete_evaluation_independence (k : ℕ) (hk : 0 < k) :
    scheduledHash k = hash ∧ lazyHash = hash

theorem paper_score_minimum :
    IsLeast (Set.range (fun L : {L : ℕ // 0 < L} => score paperEpsilon L.val)) 127

theorem modelA_score_minimum :
    IsLeast (Set.range (fun L : {L : ℕ // 0 < L} => score modelAEpsilon L.val)) 127

theorem modelA_coarse_score_minimum :
    IsLeast (Set.range (fun L : {L : ℕ // 0 < L} => score modelACoarseEpsilon L.val))
      (128 - Real.logb 2 33)
```

`paperEpsilon L = min 1 ((blocks (8*L)+1)/2^128)`, `modelAEpsilon L = min 1 ((blocks (8*L) +
degreeBudget L)/2^128)`, `modelACoarseEpsilon L = min 1 ((blocks (8*L)+32)/2^128)`, with
`degreeBudget L = if L ≤ 16 then 1 else min 32 (2*((L+31)/32))`. Every elaborated signature is
in [lean/ChainHash128V3Audit.txt](lean/ChainHash128V3Audit.txt); definitions, scope, and
provenance are in [LEAN_CHAINHASH_V3_128_STATUS.md](LEAN_CHAINHASH_V3_128_STATUS.md).

## Axioms

Only `propext`, `Classical.choice`, and `Quot.sound` (four of the 116 theorems use none); no unresolved probabilistic stage hypotheses; no `sorry`, `admit`, `native_decide`, custom axiom, or unsafe declaration in the v3 modules. Both [lean/ChainHash128V3Audit.txt](lean/ChainHash128V3Audit.txt) and [lean/CombinedV3128Audit.txt](lean/CombinedV3128Audit.txt) (495 entries) passed the axiom allowlist check.

## Commits, builds, and reproduction

- Proof/evidence commit: `3698caae7c03cc359056a9cea08e9b2d6483f6a7` on branch `chainhash-v3-128`.
- Base lane: `<xeon-work>/lean-chainhash128` at `0ff7ac3` (copied with `cp -a` including
  `lean/.lake`; Mathlib `f897ebcf72cd16f89ab4577d0c826cd14afaafc7`, never rebuilt).
- Lean 4.24.0; all computation `nice -n 10 taskset -c 48-55`, `LEAN_NUM_THREADS=8`.
- Remote lane: `<xeon-host>:<xeon-work>/lean-chainhash-v3-128`.
- Final build log: [logs/v3-128/final-build.log](logs/v3-128/final-build.log); audit summary:
  [logs/v3-128/audit-summary.txt](logs/v3-128/audit-summary.txt); vectors:
  [logs/v3-128/vectors.log](logs/v3-128/vectors.log).
- Reproduce from the Mac: `./scripts/reproduce-remote.sh`; on the Xeon: `./scripts/reproduce-v3-128.sh`.

## Corrections

None to SPEC's bounds or scores. The task summary's `p = ⌈length/512⌉` and "four lanes of 32
words in a 1 KB region" paraphrases differ from SPEC's B = 512 definition (`p(ell) = 8Q + min 8
⌈r/16⌉`; eight logical blocks interleaved per 4096-byte region, eight comb lanes); SPEC, which the
task designates as authoritative, is what the header implements and what is formalized. The length
and positive-stride domains appear explicitly in the signatures.
