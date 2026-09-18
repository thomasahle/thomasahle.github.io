# UMASH lane and inherited classic proofs

This workspace is the **partial UMASH formalization**. Start with
[README_UMASH.md](README_UMASH.md) and the supplied `STATUS.md` obligation ledger.
Use `bash reproduce.sh` on the Xeon (CPU set 56–63). The material below documents
the inherited classic proof modules and is not a statement about UMASH completion.

## Inherited classic polynomial hash proofs

Lean **4.24.0**, Mathlib **v4.24.0**, commit
`f897ebcf72cd16f89ab4577d0c826cd14afaafc7`.

The project proves the bounds for ideal-key byte-string families, including
unequal lengths and partial final blocks. See [the audit](../AUDIT_CLASSIC.md)
and [formal status](../LEAN_CLASSIC_STATUS.md) for the exact domain and the
distinction from the 64-bit-seeded OpenSSL wrappers.

| Module | Result |
|---|---|
| `ClassicCore.lean` | Positive-power polynomials, Horner evaluation, root counts on restricted keys, union over explicit field targets, averaging out a pad |
| `ClassicBytes.lean` | Actual `List (Fin 256)` inputs, 16-byte chunk parser and reconstruction, ceiling block count, byte and marked-byte encoding injectivity |
| `ClassicGHASH.lean` | Big-endian right-zero padding and standard length block; encoding injectivity; `(n+1)/2^128` AXU and collision bounds over `GaloisField 2 128` |
| `ClassicPrimes.lean` | Kernel-checked Lucas certificate for `2^130−5` |
| `ClassicPoly1305.lean` | Clamped key space of exactly `2^106` values; marked encoding; four representatives per projected residue; eight differential targets; seven collision targets; `(r,s)` tag bound |
| `Probability.lean`, `Polynomial.lean` | Reused ProvenHashes finite probability and root-count core |

GHASH takes a `Wire : Fin (2^128) ≃ GaloisField 2 128` parameter. The bounds
hold for **every** such bijection, so they do not depend on selecting one
polynomial-basis byte convention. Integer bytes and the length block are fully
encoded in the source, with injectivity proved. `Wire` is not an assumption
that the message encoding is injective. This project does not verify the
concrete GCM reduction routine or OpenSSL machine code.

On the designated Xeon:

```sh
cd ~/agents/lean-classic
bash build.sh
```

The script uses `taskset -c 72-79`, `nice -n 10`, and `LEAN_NUM_THREADS=8`.
It builds the library, regenerates `AuditAll.lean`, checks every local theorem
and lemma with `#print axioms`, rejects any extra axiom or admitted proof,
and writes the verification records. All successful reports permit only
`propext`, `Classical.choice`, and `Quot.sound`.

The remote workspace reuses the already downloaded Mathlib packages through
`.lake/packages -> ~/agents/lean-hash/lean/.lake/packages`. The local source
mirror intentionally omits `.lake`, compiled binaries, and that machine-specific
symlink. For a fresh Linux checkout, install the pinned Lean toolchain, fetch
dependencies with `lake update`, and obtain Mathlib's matching cache with
`lake exe cache get` before building. Retain the supplied dependency revision;
adapt CPU affinity only if using a different authorized machine.

Verification artifacts:

- [Build log](Build.txt)
- [All theorem types and axiom reports](AuditAll.txt)
- [Machine-readable verification result](Verification.json)
- [Toolchain identity](Toolchain.txt)
- [Source SHA-256 manifest](SourceHashes.json)

`MakePrimes.py` can regenerate the explicit certificate using SymPy, but is
not needed for builds. Python discovers witnesses; Lean's kernel checks the
generated proof. No Python result or native evaluation axiom is trusted.

`CheckSeededGHASH.py` is a separate, small executable check. It constructs a
fixed pair colliding at seed zero and compares its independently computed
GHASH relation with OpenSSL GMAC. [The recorded result](SeededGHASH.json)
supports the wrapper audit; it is not a Lean proof or a timing benchmark.
