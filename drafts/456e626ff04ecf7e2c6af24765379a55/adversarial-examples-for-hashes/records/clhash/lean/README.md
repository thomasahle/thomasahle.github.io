# Concrete ChainHash collision proof

Lean 4.24.0; Mathlib v4.24.0, commit
`f897ebcf72cd16f89ab4577d0c826cd14afaafc7`.

The authoritative build workspace is
`<xeon-host>:<xeon-work>/lean-chainhash`.
It was copied with its toolchain and cache from `<xeon-work>/lean-hash`.
The local `lean/` directory is a source and audit mirror. All compilation and
certificate generation ran on the Xeon; no Lean build ran on the Mac.

```bash
cd <xeon-work>/lean-chainhash
source env.sh
cd lean
./build.sh
```

The script uses `nice -n 10`, `taskset -c 0-31`, and
`LEAN_NUM_THREADS=32`. It builds the umbrella and produces `Audit.txt`,
`FullAudit.txt`, and `ChainHashAudit.txt`, then rejects proof placeholders,
custom axioms, unsafe declarations, and nonstandard audit axioms.

The main result is
`ProvenHashes.ChainHash.collision_bound_bytes` in
`ProvenHashes/ByteInterface.lean` (native `List UInt8` inputs and full `UInt64` outputs). For two distinct byte strings of at most
`8*L` bytes, with `8*L+255 < 2^64`, the full-output collision probability under
the uniform 41-word ideal key is at most

```
(max 1 ((L + 31) / 32) + 2) / 2^64.
```

The additional 255 in the length condition prevents overflow in the C++
reference's block-count rounding. The mathematical family is also proved under
the weaker `8*L < 2^64` condition (`chainHash_collision_bound`). The constant is
the paper's `(p+2)/2^64`, with `p` the maximum 256-byte block count.

The key layout, strided pairs, 32-byte group rounding, empty-message block,
little-endian padding, length in both halves, exact reduction polynomial,
three-key recurrence, integer-add twist, and three-multiplication quintic are
explicit in the definitions. `referenceHash_matches` proves that the direct
word-operation transcription equals the function used by the collision proof.
This is a proof of that mathematical reference function, not verification of a
C++ compiler or the optimized SIMD implementation. The seeded SplitMix64
subfamily is outside the ideal-key theorem.

Source map:

- `Carryless*.lean`: unreduced polynomial CLNH, including unequal pair counts
  with nonzero target differences.
- `ByteEncoding.lean`, `WordRepresentation.lean`: byte/word decoders, strided
  permutation, length injection, integer interpretation.
- `Stream.lean`, `FieldStream.lean`: concrete stream collision bounds and exact
  low/high-word splitting.
- `BinaryRabin.lean`, `Modulus*.lean`: irreducibility of
  `X^64 + X^4 + X^3 + X + 1`, using explicit squaring and Bézout certificates.
- `ConcreteWords.lean`: canonical polynomial-basis field representation and
  multiplication/remainder correspondence.
- `Finalizer.lean`, `FinalizerIndependence.lean`: explicit parameter decoder,
  exact collision probability, five-output uniformity, and twist preservation.
- `ChainHashModel.lean`, `KeyLayout.lean`, `ConcreteChainHash.lean`: composition
  with all stage hypotheses discharged and a bijection for the actual 41 words.
- `ReferenceOperations.lean`, `ReferenceChainHash.lean`: direct reference
  transcription and final theorem.

`generate_modulus_certificate.py` can regenerate the arithmetic certificates on
the Xeon. The generator is not trusted: Lean checks the resulting identities
using ring arithmetic. No key/message enumeration or native evaluation oracle
is used in the proofs.

The original polynomial/NH/tabulation/recurrence modules are retained. Their
unrelated multiply-shift proposition remains a proposition definition, as in the
baseline. See `../LEAN_CHAINHASH_STATUS.md` for the final verdict, theorem
signatures, source correspondence, audit output, and remote commit hashes.
