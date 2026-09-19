STATUS: COMPLETE

# ChainHash v3 / ChainHash-Horner

## Remaining obligations

None. All requested statements are proved, the complete build and combined axiom audit passed, and all reference-versus-Lean vectors matched.

## Result and scope

The model follows the supplied `chainhash_v3.h`: little-endian zero-padded words, the comb layout, presence of a pair determined by its first word, the partner key retained for a padded partner, byte length as the leading Horner coefficient, integer addition modulo 2^64 for the twist, and the shipped structured quintic finalizer.

Write `q = 2^64`, `p(L) = blocks (8*L)`. For distinct byte messages of at most `8*L` bytes:

- Paper model: exact uniform-key collision probability is at most `(p(L)+1)/q`, over 39 independent words (`kappa[0..31], y, c0..c4, tau`).
- Model A: probability is at most `(degreeBudget L+p(L))/q`, over exactly 64 independent key bytes (`s, y, c0..c4, tau`), with `kappa[m]=s^(m+1)`.
- The same model A envelope covers fixed-length and unequal-length pairs. There are no assumed universality, injectivity, independence, or collision-stage lemmas in these endpoint theorems.
- Serial Horner, the physical round-robin exact-count schedule, and bounded 128-bit lazy evaluation give equal functions. The stride result covers every positive natural `k`, including strides exceeding the block count, and all multiplier values including zero.
- Both certified envelope scores have exact minimum **63.0**, attained at `L=1`. Here score means `log₂(L/ε(L))` for the proved envelope, not a claim that the upper bound is always an attained collision probability.

The byte bound signatures use the memo's 64-bit length domain, expressed as `8*L < 2^64`; model A also takes `0 < L`. “All k” means `0 < k`, since a zero-lane schedule is not defined by the implementation. Empty messages and final partial bytes are included.

## Milestones

| Module | Checked result |
|---|---|
| `ChainHashV3Comb` | Comb decoder, byte injectivity, active-pair coverage, `partnerExponent_injective`, block-count monotonicity, general Frobenius root count |
| `ChainHashV3PH` | Seeded difference polynomial with isolated partner coefficients; exact reduced-CLNH difference universality over the field |
| `ChainHashV3Horner` | Horner polynomial expansion, difference degree, leading coefficients, root count and stage composition |
| `ChainHashV3Model` | Complete hash, 39-word and 8-word key spaces, length injection, envelope definitions and arithmetic |
| `ChainHashV3Bytes` | Unconditional paper collision theorem; unequal byte lengths separated for every PH key |
| `ChainHashV3Seeded` | Short-message Frobenius improvement, memo degree budget, complete model A collision theorem |
| `ChainHashV3Keys` | Key encoding bijections, little-endian byte-key bijection, native `List UInt8` messages and `UInt64` digest theorems |
| `ChainHashV3Evaluation` | Serial Horner equals the physical `k`-lane schedule as a function |
| `ChainHashV3Lazy` | Lossless low/high polynomial split below degree 128, `X^64 = 0x1b`, lazy step reduction, complete hash-function equality |
| `ChainHashV3Scores` | Exact real-logarithmic score minima and the memo's numerical envelope tables |

All modules are in namespace `ProvenHashes.ChainHash.V3` and imported by the main `ProvenHashes` target. The v3 collision arguments do not invoke the recurrence decoder, nested-pair bound, or three-variable Schwartz–Zippel proof. The parent files remain available in the aggregate project. `clnh_natDegree_le` is reused for the bounded raw-state bridge, rather than for a collision argument.

## Main signatures

Full elaborated signatures and the `#print axioms` result for every new theorem are in [ChainHashV3Audit.txt](lean/ChainHashV3Audit.txt). The public endpoints are:

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

`uniformProb` is the exact `ℚ≥0` ratio of event cardinality to finite key-space cardinality. `idealKey_card` proves `(2^64)^39`; `modelAKey_card` proves `(2^64)^8`. The internal byte-message endpoints are `chainHashHorner_collision_bound` and `chainHashHorner_modelA_collision_bound`.

The load-bearing Frobenius theorem is general: for nonzero `h : K[X]` over a finite field of characteristic two, the number of distinct roots of `h.comp (X^2)` is at most `h.natDegree`. This discharges the short-message obligation required for the 63.0 model A score.

## Envelope values

`envelope_table` proves the model A numerators for `L=1..20`:

```
2, 3, 4, 4, 5, 5, 6, 6, 8, 8, 8, 8, 8, 8, 8, 8, 10, 12, 12, 12
```

`envelope_large_examples` proves:

| L (words) | 32 | 121 | 128 | 129 | 1024 | 131072 |
|---|---:|---:|---:|---:|---:|---:|
| Model A numerator | 12 | 36 | 36 | 37 | 64 | 4128 |

## Axioms and builds

Final audit result: **PASS**. All 89 new theorems use only `propext`, `Classical.choice`, and `Quot.sound`. The combined inherited audit also passed. No `sorry`, `admit`, `native_decide`, custom axiom, or unsafe declaration occurs in the v3 proof modules. The final `lake build` completed successfully (7402 jobs).

Each of the 89 new theorems has a successful `lake build` checkpoint with a SHA-256 of the exact source prefix. [audit_v3.py](scripts/audit_v3.py) checks these hashes, rebuilds the complete target, prints every theorem's signature and axioms, and rejects dependencies outside `propext`, `Classical.choice`, and `Quot.sound`. It also reruns the inherited `FullAudit.lean` and `ModelAAudit.lean` in [CombinedV3Audit.lean](lean/CombinedV3Audit.lean).

Remote build logs: `~/agents/lean-chainhash-v3-64/logs/v3/`.
Final build: [final-build.log](logs/v3/final-build.log).
Per-theorem logs: `logs/v3/ChainHashV3*-THEOREM.log`, with adjacent `.sha256` stamps.
Audit summary: [audit-summary.txt](logs/v3/audit-summary.txt).

All builds, audits, and C/Lean vector executions use `nice -n 10 taskset -c 48-55` and `LEAN_NUM_THREADS=8` on the Xeon. The parent `.lake` cache was copied; Mathlib was not rebuilt. The Mac was used only for editing, inspection, and transfers.

## Reference versus Lean evidence

Final reproduction result: **464/464 PASS**. Portable, XMM, YMM, and ZMM backends all passed strides 1–8 in both eager and lazy modes, plus dispatched one-shot evaluation. NEON was not available on this Xeon.

C and Lean output SHA-256: `732b082f59e860e8171016c6919be1d863320cfbafe358ab3f52900de6017103`. See [vectors.log](logs/v3/vectors.log), [vectors-c.log](logs/v3/vectors-c.log), and the two `.out` files in the same directory.

The deterministic corpus has 464 inputs: both key models, zero keys, `y=0`, `y=1`, and random keys; empty inputs; every byte length 0–33; word, pair, block, and region boundaries; and inputs up to 4096 bytes. [vectors.c](reference/vectors.c) calls the supplied header and additionally compares supported backends, strides 1–8, eager/lazy evaluation, and dispatched one-shot evaluation. [V3.lean](lean/vectors/V3.lean) is an independent executable bit-serial reference using polynomial `x^64+x^4+x^3+x+1`.

These vectors provide cross-language consistency evidence. The executable test reference and C intrinsics are not claimed to have a formal compiler-level refinement proof; the mathematical byte model and evaluation equalities are kernel checked.

Input header SHA-256: `402e3c31638aec9154bd896e2269736119d7be2c530a934f3625abd80ccf1825`.
Memo SHA-256: `ea97a4f373547d78cb6c511f6b70f9055f8e3b4f7c4e4d898000d0730f52484e`.
Both input copies were checked identical to the user-specified files.

## Reproduction and commits

From the Mac mirror: `./scripts/reproduce-remote.sh`.
On the Xeon, in the lane root: `./scripts/reproduce-v3.sh`.
See [scripts/README.md](scripts/README.md) for cache-preserving setup and generated evidence paths.

- Remote lane: `thomas-ahle@hardware.normalcomputing.net:~/agents/lean-chainhash-v3-64`.
- Parent lane: `~/agents/lean-chainhash-modelA`.
- Parent commit: `a3939c01b87d962ae170776b656a154300d5f3ca`.
- Proof/evidence commit: `54410e433a1d9f79cd4104dc9fc2cdea72164f07`.
- Lean: `leanprover/lean4:v4.24.0`.
- Mathlib: `f897ebcf72cd16f89ab4577d0c826cd14afaafc7`.
- Local sources and audit files: `./lean/`.

## Corrections

None to the memo's requested bounds or score values. The memo's byte-length and positive-stride domains are made explicit above.
