STATUS: COMPLETE

# ChainHash-128 v3 (B = 512) — Lean 4 machine check

Port of the completed 64-bit ChainHash v3 formalization (`ProvenHashes.ChainHash.V3`) to
the GCM field `F = GF(2)[X]/(X^128 + X^7 + X^2 + X + 1)`, `q = 2^128`, following
`chainhash128_v3.h` and `SPEC.md` (512-byte comb blocks, eight interleaved logical blocks
per 4096-byte region, level-2 Horner in `y` with the byte length as leading coefficient,
integer twist modulo `2^128`, unchanged structured quintic). New namespace:
`ProvenHashes.ChainHash.V3_128`, ten modules `ChainHash128V3*.lean`.

## Remaining obligations

None. All five requested items are complete with the standard axioms only: (1) the byte-interface paper-model theorem, (2) the model A theorem with SPEC's envelope, (3) complete evaluation independence for every positive stride, (4) the score minima (127, 127, and `128 − log2 33`), and (5) reference-versus-Lean vector agreement including the header's hard-coded test vectors.

## Result and scope

The model follows the supplied header and SPEC exactly: little-endian zero-padded 16-byte
words; global word `i = 256R + 16C + 8h + j` belongs to logical block `t = 8R + j`
(zero-based), pair slot `C`, half `h`; the first word pairs with word `i+8`; a pair is present
exactly when its first word has an existing byte (`active`), an absent partner contributes
the partner key with a zero word, an absent first word omits the product; block count
`p(0) = 1`, `p(ell) = 8Q + min 8 ⌈r/16⌉` for `ell = 4096Q + r > 0` (`blocks`); the byte length
is the leading Horner coefficient (`lengthField`); the twist is integer addition modulo
`2^128` (`integerTwist fieldIntegerEquiv`); the finalizer is `chain5`.

Write `p_B(L) = blocks (8*L)` for `L` in 64-bit words (both messages have at most `8L` bytes),
`d_B(L) = 1` for `L ≤ 16` and `min 32 (2⌈L/32⌉)` otherwise (`degreeBudget`), and
`E_A(L) = p_B(L) + d_B(L)` (`modelANumerator`). For distinct byte messages:

- Paper model (39 independent uniform field words `kappa[0..31], y, c0..c4, tau`):
  `ε ≤ min 1 ((p_B(L)+1)/2^128)` (`paperEpsilon L`), and in SPEC's block form
  `ε ≤ min 1 ((p+1)/2^128)` for any `p` bounding both block counts.
- Model A (exactly 128 independent uniform key bytes `s, y, c0..c4, tau`, `kappa[a] = s^(a+1)`):
  `ε ≤ min 1 (E_A(L)/2^128)` (`modelAEpsilon L`, SPEC's refined at-most-length envelope), the
  coarse envelope `min 1 ((p_B(L)+32)/2^128)` (`modelACoarseEpsilon L`), and SPEC's block form
  `min 1 ((p+32)/2^128)`.
- Serial Horner, the physical `k`-lane exact-count schedule, and the 256-bit lazy raw state
  (`U = lo + X^128·hi`, update `clmul(lo,y) ⊕ clmul(hi,0x87·y) ⊕ C_t`, `X^128 = 0x87`) are equal
  hash functions for every positive stride `k`, including strides exceeding the block count and
  `y = 0`.
- Score minima (`score ε L = log2(L/ε(L))` over positive `L`): exactly **127** for the paper
  certificate and for the refined model A certificate, attained at `L = 1`; exactly
  **`128 − log2 33` (= 122.955606…)** for the coarse model A envelope, attained at `L = 1` — the
  three statements SPEC makes.

Length domain: `8*L < 2^128` (the field's length word; the C API's `< 2^64` domain is a
sub-case). Stride domain: `0 < k`. Empty messages and partial final words are included.
Schoolbook versus Karatsuba, backends, chain counts, and reduction placement do not appear in
any theorem; they are exercised only by the vector evidence.

## Milestones

| Module | Checked result |
|---|---|
| `ChainHash128V3Comb` | SPEC index maps (`wordIndex`, `blockOf`, `slotOf`), `blocks` with SPEC examples, comb byte injectivity, active-pair coverage, `partnerExponent_injective`, block-count monotonicity, general Frobenius root count |
| `ChainHash128V3PH` | Seeded difference polynomial with isolated partner coefficients; exact reduced-CLNH difference universality over the field |
| `ChainHash128V3Horner` | Horner polynomial expansion, difference degree, leading coefficients, root count and stage composition |
| `ChainHash128V3Model` | Complete hash, 39-word and 8-word key spaces (`(2^128)^39`, `(2^128)^8`), length injection, SPEC envelope definitions (`min 1 (·)`) and their arithmetic (`≤ 2L`, `≤ 33L`, values at `L = 1`) |
| `ChainHash128V3Bytes` | Unconditional paper collision theorem (`L`-form and SPEC block form); unequal byte lengths separated for every PH key |
| `ChainHash128V3Seeded` | Short-message Frobenius improvement (`d_B = 1` below 128 bytes), SPEC degree budget, complete model A theorem (refined, coarse, and SPEC block form) |
| `ChainHash128V3Keys` | Key encoding bijections (39 words / 624 bytes / 128 bytes), `kappa[a] = s^(a+1)` in the shipped layout, native `List UInt8` messages, `BitVec 128` digest and 16-byte serialization theorems |
| `ChainHash128V3Evaluation` | Serial Horner equals the physical `k`-lane schedule as a function |
| `ChainHash128V3Lazy` | Lossless low/high split below degree 256, `X^128 = X^7+X^2+X+1 = 0x87`, lazy step reduction, complete hash-function equality |
| `ChainHash128V3Scores` | Exact real-logarithmic score minima (127, 127, `128 − log2 33`) and SPEC's numerator tables |

## Main signatures

Full elaborated signatures and `#print axioms` for every new theorem are in
[lean/ChainHash128V3Audit.txt](lean/ChainHash128V3Audit.txt). Public endpoints
(namespace `ProvenHashes.ChainHash.V3_128`, `Key39 = Fin 39 → Word 128`, `Byte = Word 8`):

```lean
theorem paper_collision_bound_bytes (L : ℕ) (hL : 8*L < 2^128)
    (m m' : List UInt8) (hm : m.length ≤ 8*L) (hm' : m'.length ≤ 8*L) (hne : m ≠ m') :
    uniformProb (fun k : Key39 => hashBytes k m = hashBytes k m') ≤ paperEpsilon L
-- paperEpsilon L = min 1 ((blocks (8*L) + 1 : ℕ) / 2^128)

theorem paper_collision_bound_bytes_blocks (p : ℕ) (m m' : List UInt8)
    (hm : m.length < 2^128) (hm' : m'.length < 2^128) (hne : m ≠ m')
    (hp : blocks m.length ≤ p) (hp' : blocks m'.length ≤ p) :
    uniformProb (fun k : Key39 => hashBytes k m = hashBytes k m') ≤
      min 1 (((p+1 : ℕ) : ℚ≥0)/2^128)

theorem modelA_collision_bound_bytes (L : ℕ) (hL : 0 < L) (hcap : 8*L < 2^128)
    (m m' : List UInt8) (hm : m.length ≤ 8*L) (hm' : m'.length ≤ 8*L) (hne : m ≠ m') :
    uniformProb (fun k : Fin 128 → Byte => modelAHashBytes k m = modelAHashBytes k m') ≤
      modelAEpsilon L
-- modelAEpsilon L = min 1 ((blocks (8*L) + degreeBudget L : ℕ) / 2^128)

theorem modelA_coarse_bound_bytes_blocks (p : ℕ) (m m' : List UInt8)
    (hm : m.length < 2^128) (hm' : m'.length < 2^128) (hne : m ≠ m')
    (hp : blocks m.length ≤ p) (hp' : blocks m'.length ≤ p) :
    uniformProb (fun k : Fin 128 → Byte => modelAHashBytes k m = modelAHashBytes k m') ≤
      min 1 (((p+32 : ℕ) : ℚ≥0)/2^128)

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

Also proved: `paper_collision_bound_key_bytes` (the 624-byte `key_from_ideal_bytes` layout),
`paper_collision_bound_output` (the 16 serialized little-endian output bytes),
`modelA_coarse_bound_bytes` (`L`-form coarse envelope), `expandedWords_power`
(`kappa[a] = s^(a+1)` at word `a` of the 39-word layout), `alpha_eq_135` (`X^128` is the word
`0x87`), `blocks_examples`, `envelope_table` (`E_A(1..20) = 2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,10,10`),
and `envelope_large_examples` (1 KiB: 9/40/16; 1 MiB: `p = 2048`, 2049/2080/2080 — SPEC's
figures). `uniformProb` is the exact `ℚ≥0` ratio of event cardinality to finite key-space
cardinality; `idealKey_card` proves `(2^128)^39` and `modelAKey_card` proves `(2^128)^8`.

The proof structure is the 64-bit one: one changed comb block gives an exact `1/q`
reduced-CLNH difference (paper) or a degree-`d_B(L)` polynomial in `s` (model A; below 128
bytes the difference is `δ·s^2` and the general Frobenius root count gives one root);
Horner in independent `y` adds `p−1` (equal lengths) or the nonzero leading length coefficient
gives at most `p` (unequal lengths); the bijective twist and the exactly `1/q` quintic add one.

## Axioms and builds

Final audit result: **PASS**. All 116 new theorems use only `propext`, `Classical.choice`, and `Quot.sound` (four use no axioms). The combined inherited audit — the 128-bit lane's 379 declarations plus the 116 new theorems, 495 entries in [lean/CombinedV3128Audit.txt](lean/CombinedV3128Audit.txt) — also passed. No `sorry`, `admit`, `native_decide`, custom axiom, or unsafe declaration occurs in the v3 modules. The final complete `lake build` succeeded (7523 jobs, [logs/v3-128/final-build.log](logs/v3-128/final-build.log)). Per-theorem checkpoint builds: 116/116 PASS (110 in the first pass, [logs/v3-128/02-checkpoints.log](logs/v3-128/02-checkpoints.log); 6 for the SPEC block-form additions, followed by the audit, [logs/v3-128/03-audit.log](logs/v3-128/03-audit.log)). Summary: [logs/v3-128/audit-summary.txt](logs/v3-128/audit-summary.txt).

Each new theorem has a successful complete `lake build` checkpoint with a SHA-256 of the exact
source prefix (`logs/v3-128/ChainHash128V3*-THEOREM.{log,sha256}`), produced by
[scripts/checkpoint.py](scripts/checkpoint.py) through
[scripts/run_checkpoints.sh](scripts/run_checkpoints.sh). [scripts/audit_v3_128.py](scripts/audit_v3_128.py)
verifies the stamps, rebuilds the complete target, prints every theorem's signature and
axioms, and rejects dependencies outside `propext`, `Classical.choice`, `Quot.sound`; it also
reruns the inherited 128-bit lane audit in [lean/CombinedV3128Audit.lean](lean/CombinedV3128Audit.lean).

All builds, audits, and C/Lean vector runs used `nice -n 10 taskset -c 48-55` and
`LEAN_NUM_THREADS=8` on the Xeon. Lean 4.24.0; Mathlib v4.24.0
(`f897ebcf72cd16f89ab4577d0c826cd14afaafc7`); the `.lake` cache was copied with `cp -a`, and
Mathlib was not rebuilt. The Mac was used only for editing, inspection, and rsync.

## Reference versus Lean evidence

Final result: **625/625 PASS** ([logs/v3-128/vectors.log](logs/v3-128/vectors.log), [vectors-c.log](logs/v3-128/vectors-c.log)). On this Xeon the header's portable, XMM, YMM, and ZMM backends were available (NEON is not). For every input `chainhash128_v3_portable` agreed with `chainhash128_v3_evaluate` for all four backends × strides 1–8 × eager/lazy × schoolbook/Karatsuba, with `chainhash128_v3_with_backend` (both multiplication methods), and with the dispatched `chainhash128_v3`; `chainhash128_v3_selftest()` passed; the independent Lean reference produced byte-identical output (C and Lean output SHA-256 `d9fc195c93bd358b4b26293d8a3fa2a2cec85f7cda185c9f68d068846ee029b6`); and the header's three hard-coded self-test vectors (seed 123; lengths 0, 17, 2049) matched both C and Lean.

The corpus ([logs/v3-128/vectors.in](logs/v3-128/vectors.in)) has both key models; zero keys,
`y = 0`, `y = 1`, and random keys; every byte length 0–33; word, pair (112/113), short-kernel
(128/129), chunk (256), block (512), region (4096/4097) and multi-region boundaries up to 8193
bytes plus random lengths below 12288; and the header's own seed-123 self-test inputs. The C
harness [reference/vectors.c](reference/vectors.c) runs `chainhash128_v3_selftest`, then
compares `chainhash128_v3_portable` with `chainhash128_v3_evaluate` for every available backend
× strides 1–8 × eager/lazy × schoolbook/Karatsuba, with `chainhash128_v3_with_backend`, and
with the dispatched `chainhash128_v3`. [lean/vectors/V3_128.lean](lean/vectors/V3_128.lean) is an
independent executable bit-serial reference over the GCM polynomial following SPEC's
three-level definition. These vectors are cross-language consistency evidence; no formal
compiler-level refinement of the C intrinsics is claimed. The mathematical byte model and the
evaluation equalities are kernel checked.

Input SPEC.md (mirrored as reference/SPEC_v3.md) SHA-256: `df8a08b59bea78ca4908d60200d3d33873a6bf32c3fa8fdd7a8994aa6d9312b3`.
Input header SHA-256: `6c31b6f3638d545f3d95c95bd4edd7483a8964a429957fb1ab3111d9b107a455`.
Both are mirrored in [reference/](reference/) and were checked identical to the task's files.

## Reproduction and commits

From the Mac mirror: `./scripts/reproduce-remote.sh`. On the Xeon, in the lane root:
`./scripts/reproduce-v3-128.sh`. See [scripts/README.md](scripts/README.md).

- Remote lane: `<xeon-host>:<xeon-work>/lean-chainhash-v3-128`,
  branch `chainhash-v3-128`.
- Base lane (copied with `cp -a`, including `lean/.lake`, `.elan`, `.cache`):
  `<xeon-work>/lean-chainhash128` at `0ff7ac3` (its parent `a3939c0` is also the parent of
  `<xeon-work>/lean-chainhash-v3-64`, so the Mathlib cache is the same one the 64-bit v3 lane used).
- Porting source: `<xeon-work>/lean-chainhash-v3-64` at `c757ccc` (`ChainHashV3*.lean`).
- Proof/evidence commit: `3698caae7c03cc359056a9cea08e9b2d6483f6a7`.
- Local sources, audit files, and logs: `./lean/`, `./logs/v3-128/`.

## Corrections and deviations

- The task summary wrote the paper bound with `p = ⌈length/512⌉`; SPEC's `p(ell)` for B = 512 is
  `8Q + min 8 ⌈r/16⌉` (a 17-byte message occupies two logical blocks), and the header computes
  exactly that (`ch128v3_lanes`). The theorems use SPEC's `p` (`blocks`), which the task also
  designates as authoritative; the SPEC block form `(p+1)/2^128` is proved verbatim.
- The task summary described "a 1 KB region split into four lanes of 32 words"; SPEC's B = 512
  family interleaves eight logical blocks in a 4096-byte region with eight comb lanes
  (`j = i mod 8`), which is what the header does and what is formalized.
- The base workspace was copied from the completed 128-bit lane rather than from the 64-bit v3
  lane (see scripts/README.md): the 64-bit lane's base modules are `Word 64`-specific and its
  `.lake` holds no 128-bit `.olean`s, while both lanes share the same Mathlib cache lineage.
- SPEC's score statements were all proved: 127 (paper), 127 (refined model A), and
  `128 − log2 33` (coarse model A). No requested bound or score value needed correction.
