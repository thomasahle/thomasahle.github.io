# PolyXOR128: audit record

PolyXOR128 is Orson Peters's 128-bit universal hash
([github.com/orlp/polyxor](https://github.com/orlp/polyxor), crate 0.1.0, commit
[3123eb6](https://github.com/orlp/polyxor/tree/3123eb6c0ac95b879a46b490ecda1de0184b14bd)).
The construction, the Lean development and the collision theorem are Peters's; this
directory records our independent checks of them for the post.

**Result.** The Lean proof rebuilds with the standard axioms only. The shipped code
computes the function the theorem is about on every backend tested. The bound is attained
to within 0.001 bits. Score: **≥ 126.41 bits (proved)** from 4160 uniformly random key bytes.

| File | What it is |
|---|---|
| [THEOREM.md](THEOREM.md) | The exact Lean statement, its model of the hash, and how it maps onto the post's score |
| [lean/AUDIT.txt](lean/AUDIT.txt) | Rebuild transcript, source hashes, `#print axioms` |
| [lean/AuditSanity.lean](lean/AuditSanity.lean) | Our non-vacuity lemmas: key-space and block-space sizes, and a field of size 2^128 exists |
| [difftest/DIFFTEST.txt](difftest/DIFFTEST.txt) | Differential test of the crate against an independent model of the Lean definitions: 1,658 cases, 11 builds, two hosts |
| [difftest/model.py](difftest/model.py) | Executable transcription of `PolyXORCompress.lean` and `PolyXORFull.lean` |
| [difftest/tightness.py](difftest/tightness.py), [difftest/witness.log](difftest/witness.log) | Exact collision probability of the pair 00/01, and three keys on which the shipped crate collides |
| [speeds_polyxor.json](speeds_polyxor.json), [Xeon](evidence/Xeon/provenance.json) and [M2](evidence/M2/provenance.json) build provenance, [registration](bench/polyxor.cpp), [Rust shim](bench/polyxor_ffi/src/lib.rs), [Xeon sanity log](evidence/Xeon/polyxor-128.sanity.txt) | SMHasher3 `--test=Speed` timings in the post's harness, with same-binary controls; raw run logs as `evidence/{Xeon,M2}/NAME.runN.txt` |

## Key model

The theorem draws the 4096-byte block key and three field words z, u, y uniformly and
independently. `PolyXor128::from_entropy` takes exactly these bytes: z, u, y from bytes
0–47, an avalanche multiplier from 48–63 and the block key from 64–4159. So the proved
family is the one the public API constructs. `from_key(u128)` instead expands a 128-bit key
with AES-128 in counter mode. That expansion is outside the theorem: it inherits the bound
only up to AES's distinguishing advantage. The timing registration seeds through
SplitMix64 and is likewise outside the theorem.

## Checks

1. **Proof.** `lake build` at the pinned toolchain (Lean 4.34.0, Mathlib v4.34.0) succeeds.
   Both theorems depend only on `propext`, `Classical.choice` and `Quot.sound`. The sources
   contain no `sorry`, extra axioms, `implemented_by`, `native_decide` or kernel options.
2. **Non-vacuity.** Probabilities are ratios of `Nat.card`, and `Nat.card` of an infinite
   type is 0, which would make a bound trivially true. `AuditSanity.lean` proves the
   denominators are the real key-space sizes (2^33152 keys, 2^512 compression keys), and
   that a field of cardinality 2^128 exists.
3. **Implementation = model.** `model.py` is written from the Lean definitions, not from the
   Rust reference. The theorem holds for every field of size 2^128, every F4-module
   structure and every bit embedding ι. The model fixes the ones the code uses:
   - the field is POLYVAL's Montgomery product a·b·x⁻¹²⁸ modulo x¹²⁸+x¹²⁷+x¹²⁶+x¹²¹+1. This is
     a field isomorphic to GF(2¹²⁸) via a ↦ a·x⁻¹²⁸, checked against the RFC 8452 vector;
   - ι is the bit layout;
   - w acts on 32-bit subwords as (lo, hi) ↦ (hi, lo+hi).

   The model reproduces all nine of the crate's test vectors. It then agrees with the crate on
   1,658 generated cases:
   - every length 0–640, the first four 4 KiB chunk boundaries (±1, ±7–9, ±127–129, ±257 bytes), random lengths up to 19,954 bytes;
   - random, all-zero and all-0xFF keys;
   - random streaming splits, `finalize_raw` between updates, and reuse after `reset()`.

   The same holds for the portable, AVX, AVX2, AVX-512 and NEON backends. The outputs of all
   11 builds are byte-identical.
4. **Tightness.** For the one-byte pair 00/01 the collision events are disjoint key
   conditions: y = 1; key word 12 = 0 with a zero parity word; and z+u equal to a quotient
   fixed by the other key words. Their exact probability is
   2⁻¹²⁸·(3 − 2⁻⁶⁴ − 2⁻¹²⁷ + 2⁻¹⁹²), or 126.4150 bits, against the proved 126.4141. The shipped
   crate collides on a key from each event, in both `finalize_raw` and `finalize_avalanche`,
   and not on a random control key.
5. **Other API surfaces.**
   - `finalize_avalanche` is a bijection of `finalize_raw`: 64-bit xorshift and odd-multiply
     steps, then multiplication by a nonzero field element. It therefore has the same
     collisions.
   - `finalize_mac` is a MAC with a nonce and is not scored here. Its AES input drops the top
     two bits of `raw ^ nonce`, so for one nonce two tags agree when the raw hashes agree on
     126 bits (at most 4·ε).

## Timing

SMHasher3 `--test=Speed`, with the crate (Rust 1.98.1, `-C target-cpu=native`, default features)
linked into the post's SMHasher3 build next to the control hashes. The Xeon binary reuses
the post's `build-release-20260917` objects and runs pinned to CPUs 16–23. The post's
original M2 base binary is no longer available, so the M2 binary links the public
reproduction's arm64 build (arm64 `family.cmake` fix included). Read the M2 figures against
the same-binary controls below.

- Xeon selection: the higher of two bulk passes and, independently, the lower small
  average.
- M2 selection: medians of three passes. No run is more than 15% from its median.
- Units: Xeon B/TSC tick; M2 B per calibrated cycle.
- Dispatch: AVX-512 VPCLMULQDQ on the Xeon, NEON PMULL on the M2.
- Verification value: `0xA9574CA8` (`polyxor-128`) on both hosts.

| Registration | Xeon bulk | Xeon 1–31 B | M2 bulk | M2 1–31 B |
|---|---:|---:|---:|---:|
| **polyxor-128** (`finalize_avalanche`) | **15.82** | **173.01** | **17.93** | **132.86** |
| polyxor-128.raw (`finalize_raw`) | 15.85 | 130.30 | 17.82 | 91.51 |
| XXH3-128 | 20.03 | 34.91 | 13.40 | 29.04 |
| UMASH-128 | 6.02 | 38.08 | 8.05 | 40.75 |
| rapidhash | 10.70 | 27.59 | 16.03 | 19.88 |
| komihash | 7.35 | 27.67 | 8.53 | 23.53 |

Every input, however short, is padded to one 128-byte block and goes through the streaming
hasher. Short inputs therefore cost 130–175 cycles; the avalanche step accounts for about
40 of them.

## Attribution

- Construction, Lean proofs and theorem: Orson Peters. The README says the proofs are
  AI-generated, and that he checked the definitions and statements against the code, not the
  proof.
- Components, as the design write-up credits them:
  - carry-less NH (as in CLHASH);
  - Nandi's encode–hash–combine, as used by HalftimeHash;
  - key-before-encoding, inspired by Multimixer-128;
  - the chunk recurrence p ← (p+u)(h₁+y)+h₀, credited to Ahle and Knudsen, *Fast Evaluation
    of Polynomials with Rational Preprocessing*;
  - PolymurHash's length finalization.
- REPRODUCTION: the rebuild and axiom audit.
- NEW here: the non-vacuity lemmas, the independent Lean-definition model and cross-backend
  differential test, the exact L = 1 tightness computation with witnesses, and the timings.
