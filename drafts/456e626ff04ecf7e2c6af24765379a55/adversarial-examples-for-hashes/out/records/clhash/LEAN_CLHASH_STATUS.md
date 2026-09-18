STATUS: COMPLETE

# Ideal Algorithm-4 CLHASH collision bound

The requested two-regime bound is proved with no unproved stage, encoding,
field, or independence hypothesis. The final theorem applies to distinct
`List UInt8` messages, each shorter than `2^64` bytes, and returns a full
`UInt64`. Empty input, partial last words, partial last blocks, unequal lengths,
and short/long cross-branch comparisons are included.

## Remaining obligations

None for the requested ideal Algorithm-4 collision bound. All seven obligations
listed in the previous `STATUS.md` are discharged. In particular,
`CollisionBound`, `AggregateCollisionBound`, and `UnequalLengthsBound` now have
compiled theorem proofs; they are no longer just specification definitions.

## Main signature

In namespace `ProvenHashes.CLHash`,
[CLHashByteInterface.lean](lean/ProvenHashes/CLHashByteInterface.lean):

```lean
theorem collision_bound_bytes (L : ℕ) (m m' : List UInt8)
    (hm : m.length < 2 ^ 64) (hm' : m'.length < 2 ^ 64)
    (hmL : m.length ≤ 8 * L) (hmL' : m'.length ≤ 8 * L) (hne : m ≠ m') :
    uniformProb (fun k : IdealKey => hashBytes k m = hashBytes k m') ≤
      if L ≤ 128 then 1 / (2 : ℚ≥0) ^ 64
      else 2 / (2 : ℚ≥0) ^ 64 +
        (((L + 127) / 128 - 1 : ℕ) : ℚ≥0) / (2 : ℚ≥0) ^ 126
```

Here `(L + 127) / 128 = ⌈L/128⌉`. Thus the displayed conclusion is exactly
`1/q` for `L ≤ 128`, and `2/q + (⌈L/128⌉ − 1)/2^126` otherwise, with
`q = 2^64`. `L` is a common upper bound in 64-bit words; each message may have
any byte length at most `8*L`. No upper bound on `L` beyond the individual
message-length hypotheses is required.

The bit-level theorem `clhash_collision_bound` has the same hypotheses for
`Message = List (Fin 8 → ZMod 2)` and concludes `≤ epsilon L`.
`collision_bound : CollisionBound` proves the original specification verbatim.
All signatures are recorded by `#check` in [CLHashAudit.txt](lean/CLHashAudit.txt).

## Model and ideal key space

[CLHashAlgorithm.lean](lean/ProvenHashes/CLHashAlgorithm.lean) defines:

```lean
abbrev BlockKey := Fin 64 × Bool → Word 64
abbrev IdealKey :=
  BlockKey × (Word 126 × ((Word 64 × Word 64) × Word 64))
-- Word w = Fin w → ZMod 2
-- hash : IdealKey → Message → Word 64
-- hashBytes : IdealKey → List UInt8 → UInt64
```

`uniformProb` is the exact `ℚ≥0` ratio of the event's cardinality to the finite
key-space cardinality. The key contains 128 independent CLNH words, 126
independent polynomial-key bits, two independent finalizer words, and an
independent length word. The same CLNH key is reused across blocks, as in the
algorithm.

Each block contains at most 1024 bytes. Words are little endian; missing bytes
in the final word are zero, and an odd word count has a zero partner. The last
block uses precisely the pairs intersecting the message, matching the recorded
C++ tail routines. The empty string takes the short branch with no active pairs.
Lengths use a proved injective 64-bit encoding on `[0, 2^64)`.

The polynomial stage is the literal descending polynomial in the block
outputs, with remainder modulo `X^128 + X^2 + X`. The long branch splits its
128-bit result into two words, applies one final keyed carry-less product,
adds the independent length product, and reduces modulo
`X^64 + X^4 + X^3 + X + 1`. The short branch performs the latter reduction
directly on its CLNH output plus the length product.

## Milestones and discharged statements

| Obligation | Compiled result |
| --- | --- |
| Unreduced CLNH | The inherited `ChainHash.clnh_difference_bound` bounds every polynomial XOR-difference target by `1/2^64`, for arbitrary selected pairs. `rawBlock_degree` specializes the degree bound to `<127`. |
| Concrete GF(2^127) | `modulus127_irreducible : Irreducible modulus127`, where `modulus127 = X^127 + (X + 1)`. A generated 127-step Frobenius chain and Bézout identity are checked by Lean; the generator is not trusted. |
| Field cardinalities | `field_cards : Fintype.card F127 = 2^127 ∧ Fintype.card F64 = 2^64`. `repr127` constructs the explicit polynomial-basis equivalence. |
| Byte and last-block encoding | `block_encoding_injective` and `rawBlocks_collision_bound` recover equal-length messages and prove the `1/2^64` bound for equality of the entire block-output vector under the reused key. |
| Restricted polynomial key | `polyKey_injective` embeds `Word 126` into `F127`. `restricted_polynomial_collision_bound` counts roots on that subset. |
| Lazy reduction | `mk127_lazy_remainder` and `aggregate_projection` identify the field value; `aggregate_degree` proves the lazy result has degree `<128`. |
| Polynomial bound | `aggregate_collision_bound : AggregateCollisionBound` proves `(n−1)/2^126` for distinct vectors of degree-`<127` block outputs. |
| Final reduction | `reduced_clnh_difference_bound`, `finalProduct_difference_bound`, and `final_reduction_difference_bound` prove the additional `1/2^64` bound for every fixed XOR-difference target. |
| Unequal lengths | `unequal_lengths_bound : UnequalLengthsBound` proves the stronger `1/2^64` collision bound whenever byte lengths differ, including cross-branch pairs. |
| Equal-length regimes | `short_equal_length_bound` proves `1/2^64`; `long_equal_length_bound` proves `2/2^64 + (blockCount m−1)/2^126`. |
| End-to-end composition | `clhash_collision_bound`, `collision_bound`, and `collision_bound_bytes` combine the stages and prove the requested rounding to a common word cap. |

The two long-regime `1/q` terms have distinct sources: equality of the block
output vectors and a collision in the final two-word reduction. Root counting
contributes `(n−1)/2^126`. Equal lengths cancel the independent length product;
unequal lengths expose a nonzero field multiplier of that independent key.

## Corrections and scope

No correction or weakening of the requested ideal-family bound was necessary.
The ambient polynomial field has `2^127` elements, while Algorithm 4 samples
only `2^126` polynomial keys; the proof preserves that distinction.

The theorem concerns the full independent ideal key, with unbounded natural
arithmetic for length rounding. It is not a theorem about the deterministic
SMHasher seed-expansion subfamily, C++ pointer behavior, or compilation. The
C++ expression `(lengthbyte + 7)/8` can overflow near the top of the 64-bit
range; that implementation issue does not restrict the proved ideal model.
The optional bit mixer is outside this model. These are scope boundaries,
not missing hypotheses of the requested theorem.

The native model's tail convention is additionally checked against the
recorded SMHasher3 implementation. Those checks are executable evidence,
not a Lean-to-C++ correspondence proof.

Primary reference: Lemire–Kaser,
[Faster 64-bit universal hashing using carry-less multiplications, v8, §5 and Algorithm 4](https://arxiv.org/html/1503.03465v8).
The local appendix and `audit-classic/AUDIT_CLASSIC.md` snapshots inspected in
the previous round contained no CLHASH match; the formula explicitly supplied
in this task is the theorem proved here.

## Axioms and validation

The full reproduction run succeeded on the Xeon using Lean 4.24.0 and the
copied Mathlib v4.24.0 cache. No Mathlib rebuild or Mac proof computation was
performed. All proof builds used `nice -n 10 taskset -c 48-55` and
`LEAN_NUM_THREADS=8`.

- [CLHashAudit.lean](lean/CLHashAudit.lean) and
  [CLHashAudit.txt](lean/CLHashAudit.txt): `#check` and `#print axioms` for all
  **184 CLHASH declarations**, including every theorem and the generated
  certificate.
- [FullAudit.txt](lean/FullAudit.txt): **271 inherited declarations**, audited
  again in the completed environment.
- All 455 audited declarations use only subsets of
  `[propext, Classical.choice, Quot.sound]`. In particular, the native-byte
  theorem, end-to-end theorem, and modulus irreducibility theorem list exactly
  these three axioms.
- The reproduction script checks every reported source for proof placeholders,
  `native_decide`, custom axioms, and unsafe declarations, and rejects extra
  axioms or missing audit output.
- [Vector report](lean/vectors/REPORT.txt): **608/608** scalar-model comparisons
  passed against the recorded `clhash.cpp` routines with `clhash<false,false>`
  and directly supplied full keys. Cases include empty messages, partial words,
  both sides of block boundaries, polynomial keys zero/one/all ones, and
  randomized messages up to 32647 bytes. The report records source, harness,
  input-corpus, and result hashes.

## Commits, workspace, and build logs

- Parent ChainHash commit:
  `618f1b6abdeee57c83a6b1fa2f00b84e687b61ea`.
- Previous-round checkpoint: `8a6cfd8` (drafts blocked before Lean by the old
  invalid CPU allocation; no compiled result was discarded).
- Completed proof and audit commit:
  **`b2dd80c9394b9b1bb3c6be3f4979cc6f08853cb9`**.
- Mathlib commit:
  `f897ebcf72cd16f89ab4577d0c826cd14afaafc7`.
- Lean compiler commit:
  `797c613eb9b6d4ec95db23e3e00af9ac6657f24b` (4.24.0).
- Remote workspace:
  `thomas-ahle@hardware.normalcomputing.net:~/agents/lean-clhash`.
- Final full build, audits, and vector checks:
  [logs/clhash-32-reproduction-audit.log](logs/clhash-32-reproduction-audit.log).
- Native-byte theorem build:
  [logs/clhash-31-native-byte-bound.log](logs/clhash-31-native-byte-bound.log).
- Main bound first successful build:
  [logs/clhash-27-main-bound.log](logs/clhash-27-main-bound.log).
- Allocation/toolchain evidence:
  [logs/clhash-cpu-allocation-round2.txt](logs/clhash-cpu-allocation-round2.txt).

The Lean sources, generated certificate, audit inputs and outputs, vector
artifacts, and build logs are mirrored locally under `./lean/` and `./logs/`.
The final build has no errors; its only linter notice is a redundant word-cap
hypothesis in the symmetric statement.

## Reproduction

On the Xeon, using the preserved cache:

```bash
cd ~/agents/lean-clhash
CPUSET=48-55 bash lean/build.sh
```

This generates the certificate, runs `lake build`, generates and executes all
axiom audits, checks the allowed axiom set and source restrictions, then
compiles the recorded C++ routines and runs the vector checks. The script's
default CPU set is also `48-55`. The local `build-remote.sh` transfers source
edits and invokes the same remote reproduction script.
