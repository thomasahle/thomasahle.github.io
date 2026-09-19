# Concrete ChainHash collision theorem

**Verdict: PROVED.** Verified on 2026-09-18 in
`<xeon-host>:<xeon-work>/lean-chainhash`.
There is no remaining stage-bound, encoding, finalizer, or field-irreducibility
hypothesis in the concrete theorem.

For the shipped **256-byte blocks, S = 1, degree-5 finalizer, 41 independent
uniform 64-bit key words**, any two distinct byte strings of at most `8*L`
bytes satisfy

\[
\Pr_k[H_k(m)=H_k(m')]\le\varepsilon(L)
=\frac{\max(1,\lceil L/32\rceil)+2}{2^{64}}.
\]

Here `L : ℕ` counts 8-byte words; the strings may have arbitrary byte lengths,
including partial words and the empty string. The output is the full `UInt64`.
The implementation-facing theorem assumes `8*L + 255 < 2^64`, equivalently
`L ≤ 2^61 - 32`, to prevent overflow in the reference's block-count expression
`(len + 255) / 256`. The mathematical family is also proved under the weaker
`8*L < 2^64` condition.

**No correction to the paper's collision constant was needed.** This is its
`(p+2)/2^64` bound, with `p = max 1 ((L+31)/32)` and natural-number division.
The maximum with one implements the reference's empty-message block. The
additional implementation length restriction is stated explicitly instead of
silently treating machine-size addition as unbounded arithmetic.

## Main theorem and exact scope

The main result is in [ByteInterface.lean](lean/ProvenHashes/ByteInterface.lean),
namespace `ProvenHashes.ChainHash`:

```lean
theorem collision_bound_bytes (L : ℕ) (hL : 8 * L + 255 < 2 ^ 64)
    (m m' : List UInt8)
    (hm : m.length ≤ 8 * L) (hm' : m'.length ≤ 8 * L) (hne : m ≠ m') :
    uniformProb (fun k : Key41 => hashBytes k m = hashBytes k m') ≤
      ((max 1 ((L + 31) / 32) + 2 : ℕ) : ℚ≥0) / (2 : ℚ≥0) ^ 64
```

`Key41 = Fin 41 → Word 64`, where `Word w = Fin w → ZMod 2`.
`uniformProb` is the cardinality of the event divided by the cardinality of the
finite key space. Thus the probability is exactly over all 41-word ideal keys,
with all bits independent and uniform. The key space has `(2^64)^41` elements.
`hashBytes : Key41 → List UInt8 → UInt64` preserves the bit representation on
input and output. The native-byte conversion is proved injective.

The definitions are a mathematical transcription of
`chainhash_ref::hash<32,5,1>`. The theorem is about that concrete function,
including its actual word operations and key layout. It is not a verification
of C++ compilation, pointer/memory behavior, or the optimized SIMD routines.
It does not claim an ideal-key bound for the 64-bit-seeded SplitMix64 subfamily,
nor cover the separate 1 KB configuration. These are scope boundaries, not
undischarged assumptions of the displayed theorem.

The central correspondence theorem, in
[ReferenceChainHash.lean](lean/ProvenHashes/ReferenceChainHash.lean), is:

```lean
theorem referenceHash_matches (k : Key41) (m : Message) :
    referenceHash k m = chainHash k m
```

`referenceHash` directly performs the word-level stream construction,
recurrence, integer-add twist, and quintic circuit. `chainHash` is the concrete
polynomial-basis field function used by the collision proof. The native-byte
theorem follows through this equality, with no abstract stage probability
hypotheses. The weaker-cap mathematical theorem is:

```lean
theorem chainHash_collision_bound (L : ℕ) (hL : 8 * L < 2 ^ 64)
    (m m' : Message)
    (hm : m.length ≤ 8 * L) (hm' : m'.length ≤ 8 * L) (hne : m ≠ m') :
    uniformProb (fun k : Key41 => chainHash k m = chainHash k m') ≤ epsilon L
```

## Discharged proof pieces

| Piece | Checked result and source |
| --- | --- |
| Unreduced carry-less NH | [Carryless.lean](lean/ProvenHashes/Carryless.lean) represents words as polynomials over `ZMod 2`, multiplies without reduction, and XORs the products. `clnh_difference_bound` bounds every target XOR difference by `2^-w` when the selected message data differ. `clnh_natDegree_le` proves degree at most 126 for 64-bit inputs. |
| Unequal pair counts | [CarrylessVariable.lean](lean/ProvenHashes/CarrylessVariable.lean) proves `clnh_nested_nonzero_bound` for nested selected pair sets and a nonzero target. Its fresh-pair argument explicitly exposes the two key coordinates and handles a zero multiplicand using the existing atom bound. This is the statement needed for different final-block lengths. |
| Byte and length encoding | [ByteEncoding.lean](lean/ProvenHashes/ByteEncoding.lean) gives explicit byte/word and strided-position inverses. `block_encoding_injective` recovers equal-length byte strings from their active block data. `lengthWord_injective` handles lengths below `2^64`. [WordRepresentation.lean](lean/ProvenHashes/WordRepresentation.lean) connects the same bits to `BitVec` and integer residues. |
| Actual stream bound | [Stream.lean](lean/ProvenHashes/Stream.lean) and [FieldStream.lean](lean/ProvenHashes/FieldStream.lean) prove the stream collision bound `2^-64` for all distinct messages of valid lengths. This includes zero padding, different last-block pair counts, the empty message, length injection into both halves, and lossless low/high splitting below degree 128. |
| Actual binary field | [BinaryRabin.lean](lean/ProvenHashes/BinaryRabin.lean) proves the degree-64 Rabin criterion. The `Modulus*.lean` modules prove irreducibility of `X^64 + X^4 + X^3 + X + 1` using 64 explicit squaring identities and a Bézout certificate. [ConcreteWords.lean](lean/ProvenHashes/ConcreteWords.lean) constructs the polynomial-basis representation and proves that field multiplication is carry-less multiplication followed by remainder modulo this exact polynomial. |
| Degree-5 circuit | [Finalizer.lean](lean/ProvenHashes/Finalizer.lean) expands the actual three-multiplication circuit and supplies an explicit inverse to its five-coefficient map. Both inverse identities are proved. For distinct inputs, `chain5_collision_exact` gives collision probability exactly `1 / card F`. |
| Five-output independence | [FinalizerIndependence.lean](lean/ProvenHashes/FinalizerIndependence.lean) uses an explicit interpolation inverse to prove `chain5_fivewise_exact`: any prescribed output tuple at five distinct inputs has probability `1 / (card F)^5`. `chain5_twisted_fivewise_exact` preserves this for an injective input twist. |
| Integer-add twist | `integerTwist_bijective` in [Finalizer.lean](lean/ProvenHashes/Finalizer.lean) proves that adding a fixed word modulo `2^64`, including carries, is bijective. [ReferenceOperations.lean](lean/ProvenHashes/ReferenceOperations.lean) connects that twist to the word definition. |
| Composition and ideal-key layout | [ChainHashModel.lean](lean/ProvenHashes/ChainHashModel.lean) discharges all hypotheses of the existing equal-stream-length and different-stream-length composition theorems. [KeyLayout.lean](lean/ProvenHashes/KeyLayout.lean) explicitly encodes and decodes the actual 41 positions. [ConcreteChainHash.lean](lean/ProvenHashes/ConcreteChainHash.lean) instantiates the field and bit representation; the final native-byte theorem is in [ByteInterface.lean](lean/ProvenHashes/ByteInterface.lean). |

The important unreduced-CLNH signatures, with namespace qualifications omitted,
are:

```lean
theorem clnh_difference_bound {I : Type*} [Fintype I] [DecidableEq I] {w : ℕ}
    (s : Finset I) (m m' : I × Bool → Word w)
    (hne : ∃ i ∈ s, ∃ b, m (i, b) ≠ m' (i, b)) (C : BitsPolynomial) :
    uniformProb (fun k => clnh s m k - clnh s m' k = C) ≤ 1 / 2 ^ w

theorem clnh_nested_nonzero_bound {I : Type*} [Fintype I] [DecidableEq I] {w : ℕ}
    (s t : Finset I) (hst : s ⊆ t) (m m' : I × Bool → Word w)
    (C : BitsPolynomial) (hC : C ≠ 0) :
    uniformProb (fun k => clnh s m k + clnh t m' k = C) ≤ 1 / 2 ^ w
```

In characteristic two subtraction and addition here are both XOR. The nonzero
target restriction on the unequal-count lemma is intentional; the stream
proof supplies it using distinct length masks. These are bounds in the
unreduced polynomial ring, not a substitution of the pre-existing field-NH
theorem.

The composition splits according to block counts. For equal block counts the
three contributions are at most `1/2^64`, `p/2^64`, and `1/2^64`. For unequal
block counts the streams cannot be equal, the recurrence contributes at most
`(p+1)/2^64`, and the finalizer contributes at most `1/2^64`. Both give
`(p+2)/2^64`.

## Match to the reference implementation

The inspected files were
`tools/bench/chainhash/chainhash_ref.h` and `tools/bench/chainhash/chainhash.h`
in `<repos>/fast-polynomials`. Their snapshots are in
[reference/](reference/). The detailed paper statement is
`thm:ph:collision` in [appendix_chainhash.tex](reference/appendix_chainhash.tex),
lines 231–236. The implementation and paper use:

- 32 little-endian 64-bit words per 256-byte block, with one sub-block (`S=1`).
- Strided pairs `(4g, 4g+2)` and `(4g+1, 4g+3)`; the message and key words use
  the same position permutation.
- Two pairs for every 32-byte group intersecting the input; partial groups
  receive zero padding. The empty message has one block with no active pairs.
- An unreduced polynomial accumulator, split into low and high 64-bit words.
  The byte length is XORed into **both** halves of the last pair.
- `P₀=z` and `Pᵢ=aᵢ XOR ((bᵢ XOR y) · (Pᵢ₋₁ XOR u))`, where multiplication
  reduces modulo `X^64 + X^4 + X^3 + X + 1` (reduction constant 27).
- Integer addition modulo `2^64` for the input twist, followed by
  `y=v*v; z=(y XOR c0)*(v XOR y XOR c1); out=(v XOR c2)*(z XOR c3) XOR c4`.
- Key positions `0..31` for CLNH, `32=u`, `33=y`, `34=z`, `35..39=c0..c4`,
  and `40=twist`.

All of these choices are explicit in the Lean definitions. The snapshots were
clean tracked files at fast-polynomials commit
`04c73a79a47f6107ed20ae1d4792e3e24ca6bc6f`. Their SHA-256 values are recorded in
[ReferenceSHA256.txt](lean/ReferenceSHA256.txt).

## Axiom audit and build evidence

The final build and audit checked **271 theorem/lemma declarations**: the 57
baseline declarations plus 214 added declarations. Each has both its
elaborated signature and its actual `#print axioms` output recorded in
[FullAudit.txt](lean/FullAudit.txt), generated by
[FullAudit.lean](lean/FullAudit.lean). The shorter
[ChainHashAudit.txt](lean/ChainHashAudit.txt) records the principal ChainHash
statements and their axioms.

Every reported axiom list is a subset of
`[propext, Classical.choice, Quot.sound]`. In particular:

```text
'ProvenHashes.ChainHash.collision_bound_bytes' depends on axioms: [propext, Classical.choice, Quot.sound]
'ProvenHashes.ChainHash.referenceHash_matches' depends on axioms: [propext, Classical.choice, Quot.sound]
'ProvenHashes.ChainHash.modulus_irreducible' depends on axioms: [propext, Classical.choice, Quot.sound]
'ProvenHashes.ChainHash.integerTwist_bijective' depends on axioms: [propext, Quot.sound]
```

There are no `sorry`, `admit`, `native_decide`, custom axioms, or unsafe
declarations in the proof modules. The modulus generator produces arithmetic
identities; ordinary Lean proofs check them using ring arithmetic. The
generator's correctness is not assumed, and it enumerates neither keys nor
messages. No native computation oracle appears in the audited axioms.

Toolchain: **Lean 4.24.0**, **Mathlib v4.24.0**, Mathlib commit
`f897ebcf72cd16f89ab4577d0c826cd14afaafc7`.
All Lean builds and certificate generation ran on the Xeon. The Mac performed
only source editing, transfer, and lightweight inspection; no builds or
benchmarks ran there. The fresh remote copy retains the existing toolchain
and dependency cache, and the original `<xeon-work>/lean-hash` was left intact.

Reproduce on the Xeon:

```bash
cd <xeon-work>/lean-chainhash
source env.sh
lean/build.sh
```

[build.sh](lean/build.sh) runs `lake build` and the audit files under
`nice -n 10 taskset -c 0-31`, with `LEAN_NUM_THREADS=32`, then checks the
placeholder and axiom policy. The final successful build log is preserved as
[BuildVerification.txt](lean/BuildVerification.txt), copied from remote
`logs/41-final-audit.log`:

```text
✔ [7388/7389] Built ProvenHashes (5.6s)
Build completed successfully (7389 jobs).
Build and axiom audit passed (271 proof declarations with axiom lists).
```

This was an incremental build using the retained cache. Earlier module-build
and audit logs remain in remote `logs/00-baseline.log` through
`logs/40-byte-interface.log`, including development failures followed by
successful repairs. The final audited source has no outstanding build errors.
The inherited `lean/Verification.txt` records the old baseline; the current
result is `lean/BuildVerification.txt`.

## Remote commits and deliverables

Remote repository: `<xeon-host>:<xeon-work>/lean-chainhash`.

| Commit | Result |
| --- | --- |
| `e70213feed3dac6280fe8d7ecdc35dcfd0cf1cc8` | Copied baseline: 57 audited declarations and conditional ChainHash composition. |
| `ee7d81c94e084a9a4d44a08d76611da2f48d7305` | Unreduced CLNH, variable pair counts, byte/length decoding, finalizer coefficient decoder, and first successful 85-declaration audit. |
| `917798c2bad07cd0d8c5d8c84d3106614932fe97` | Complete concrete field, stream, key layout, reference correspondence, native-byte collision theorem, five-output finalizer result, and successful 271-declaration build/axiom audit. |

The complete source and audit mirror is [lean/](lean/), excluding the remote
toolchain, dependency checkouts, and binary cache. Build instructions and a
module map are in [lean/README.md](lean/README.md). The previously open,
unrelated multiply-shift proposition remains outside this task; it is not
used by the ChainHash result.
