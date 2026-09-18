# Machine-checked Poly1305 and GHASH bounds

Verified 2026-09-18 in `thomas-ahle@hardware.normalcomputing.net:~/agents/lean-classic`.
All requested ideal-key bounds are proved for byte-string inputs. **No `sorry`,
`admit`, custom axiom, or `native_decide` is used.** The Mac was not used for
Lean compilation, arithmetic checks, or PDF rendering.

Toolchain: Lean **4.24.0**, commit
`797c613eb9b6d4ec95db23e3e00af9ac6657f24b`; Mathlib **v4.24.0**, commit
`f897ebcf72cd16f89ab4577d0c826cd14afaafc7`. All builds use
`taskset -c 72-79`, `nice -n 10`, `LEAN_NUM_THREADS=8`.
The workspace copies the existing `Probability.lean` and `Polynomial.lean`
from ProvenHashes and reuses its cached dependency directory. The original
ProvenHashes sources were not edited.

The deliverable source mirror is [lean/](lean/README.md). Exact finite
probabilities are nonnegative rationals: cardinality of the event divided
by cardinality of the sampled key type. Polynomial bounds are un-clipped
upper bounds; taking their minimum with one is always valid.

## 1. Shared field and byte-string infrastructure — COMPLETE

[ClassicCore.lean](lean/ProvenHashes/ClassicCore.lean) defines coefficient lists
and their positive-power polynomials. It proves agreement of polynomial
evaluation with the usual left-fold update `(acc + block) * key`.
The core proves:

- Equality of coefficient polynomials determines equal-length coefficient
  lists, and determines arbitrary-length lists whose coefficients are all
  nonzero.
- Distinct positive-power polynomials remain distinct after subtracting any
  constant target.
- A nonzero polynomial has at most its degree many roots even when keys
  are drawn from an injectively represented restricted space.
- A union over a finite set of explicit field targets has at most the
  number of targets times the maximum degree many bad keys.
- An independent finite pad coordinate can be averaged out exactly.

[ClassicBytes.lean](lean/ProvenHashes/ClassicBytes.lean) uses actual lists
of bytes, `Byte = Fin 256`. Its recursive parser takes 16 bytes and drops
16 bytes at each step. The proofs establish that flattening its chunks
recovers the original message, every chunk has at most 16 bytes, and its
chunk count is `(message.length + 15) / 16`.

The little-endian integer encoding is injective at a fixed byte length.
Adding `256^length` is proved equal to appending byte `0x01`, and makes the
integer encoding injective across lengths. These facts are proved from byte
digits and bounds; none is an assumed message-encoding hypothesis.

## 2. GHASH — COMPLETE for the ideal single-stream field family

[ClassicGHASH.lean](lean/ProvenHashes/ClassicGHASH.lean) uses
`F = GaloisField 2 128` and proves `Fintype.card F = 2^128`.
It encodes each data chunk as a big-endian integer with zero bytes appended
on the right. It encodes the final block exactly as
`(8 * message.length) * 2^64`, the big-endian block
`[bit_length]_64 || [0]_64`.

`poly_injective` proves that distinct byte strings give distinct formal
polynomials, including unequal lengths, empty strings, and partial blocks.
The only length restriction is the required
`8 * message.length < 2^64` for each input. Equal polynomial length
coefficients first recover equal byte lengths; injectivity of the padded
data encoding then recovers all bytes.

The theorem is parameterized by `wire : Fin (2^128) ≃ F`, a bijective
interpretation of a 128-bit block as a field element. It holds for every
such interpretation, including the standard GCM representation. This
avoids mistakenly casting ordinary integers into a characteristic-two
field, which would retain only their parity. The byte parser, zero padding,
length serialization, and polynomial injectivity are all discharged
proof obligations, rather than hypotheses attached to `wire`.

The principal statements are:

```lean
-- For fixed distinct m,m', each with at most n data blocks:
GHASH.differential_bound :
  uniformProb (fun H : GaloisField 2 128 =>
    (GHASH.poly wire m hm).eval H -
    (GHASH.poly wire m' hm').eval H = t)
  ≤ (n + 1 : ℕ) / (2 ^ 128 : ℚ≥0)

GHASH.collision_bound :
  uniformProb (fun H : GaloisField 2 128 =>
    (GHASH.poly wire m hm).eval H =
    (GHASH.poly wire m' hm').eval H)
  ≤ (n + 1 : ℕ) / (2 ^ 128 : ℚ≥0)
```

These are readable excerpts; [AuditAll.txt](lean/AuditAll.txt) contains the
complete Lean signatures, including every hypothesis. `GHASH.word_bound`
derives the block caps from `m.length ≤ 8*L` and proves the envelope
`(((L+1)/2)+1)/2^128`. `zero_key` proves the value at `H=0` is zero, and
`fixed_pad_cancels` proves that adding the same field pad preserves equality.
The root count samples the entire field, including zero.

There is no AES uniformity assumption smuggled into these theorems: they
sample the field key directly. The concrete GCM polynomial reduction,
the mapping of its bit basis into Mathlib's abstract field, AES, and
OpenSSL C/assembly are outside this formal verification. The theorem's
representation-independent formulation does not need a particular basis
to prove the collision bound. It does not certify the actual seeded
GMAC registration, two nonempty streams, or truncated outputs.

## 3. Poly1305 — COMPLETE with published 8 and stronger collision 7

[ClassicPrimes.lean](lean/ProvenHashes/ClassicPrimes.lean) proves the concrete
primality of `2^130−5` using explicit Lucas certificates and checked modular
exponentiation. The certificate's small factors are also proved prime.
Python/SymPy was used only to find the witnesses; the generated Lean file
is independently kernel checked and is sufficient to rebuild the proof.
Its axiom report contains only the three standard logical axioms.

[ClassicPoly1305.lean](lean/ProvenHashes/ClassicPoly1305.lean) then installs
that proved fact for `ZMod (2^130−5)`. It does not leave primality as an
external hypothesis. It uses the actual marked-byte chunks and the
positive-power polynomial with reversed coefficient order, hence the
standard Horner order.

The clamped multiplier is represented by four independent bounded limbs:

```text
Key = Fin (2^28) × Fin (2^26) × Fin (2^26) × Fin (2^26)
r   = a + 2^34 b + 2^66 c + 2^98 d
```

These bit intervals are exactly the 106 free positions of the RFC mask.
The implementation proves this arithmetic map injective and below the
prime, its image `clampedSet` has exactly `2^106` elements, and
`clampedEquiv` is a bijection from the limb space to the actual subtype
`Clamped = Set.range key`. `mem_clamped_iff` identifies this subtype's
membership with the finite set `clampedSet`. `clamped_card` establishes the
subtype's cardinality, and `clamped_differential_bound` states the
probability theorem directly on that uniform clamped subtype. The exact
denominator is therefore part of the formal result, not a numerical
annotation to a full-field theorem.

The output projection is the cast of the **canonical field representative**
to `ZMod (2^128)`. It is not defined as a field homomorphism.

- `projection_fibre_cover`: at most four canonical representatives for a
  fixed output residue, explicitly `g + iQ`, `i : Fin 4`.
- `projection_cover`: a projected difference `g` implies a field target
  `g + (i−4)Q`, `i : Fin 8`.
- `collision_projection_cover`: output equality implies one of the seven
  field targets `(i−3)Q`, `i : Fin 7`.
- `poly_injective`: distinct byte strings give distinct polynomials over
  the concrete prime field, not merely over the integers.

The roots for each target are counted separately; all target polynomials
are proved nonzero. The resulting statements are:

```lean
-- r uniform on the explicit clamped set, arbitrary fixed g:
Poly1305.clamped_differential_bound :
  uniformProb (fun r : Poly1305.Clamped =>
    Poly1305.project ((Poly1305.poly m).eval r.val) -
    Poly1305.project ((Poly1305.poly m').eval r.val) = g)
  ≤ (8 * n : ℕ) / (2 ^ 106 : ℚ≥0)

-- Independently uniform clamped r and full 128-bit pad s:
Poly1305.tag_collision_bound :
  uniformProb (fun k : Poly1305.Key × ZMod Poly1305.Q =>
    Poly1305.tag k.1 k.2 m = Poly1305.tag k.1 k.2 m')
  ≤ (7 * n : ℕ) / (2 ^ 106 : ℚ≥0)
```

Again, the complete hypotheses appear in `AuditAll.txt`: distinct messages
and at most `n` chunks each. `differential_bound` and `collision_bound` retain
Bernstein's constant **8**; `collision_bound_seven` and the full tag theorem
prove the optional collision-specific constant **7**. The source's theorem
does not need a constant correction. `word_bound` proves the corresponding
`7 ceil(L/2)/2^106` bound directly from the byte length caps.

`pad_cancels` is an equivalence for each fixed `(r,s)`, and `zero_key`
proves that `r=0` makes every tag equal to `s`. No weak-key exclusion is used.
The limb parametrization models the exact mathematical clamped set; it is
not a verification of a raw-key generator, a C bitwise masking routine,
SplitMix64, or OpenSSL.

## 4. Build and full axiom verification

**Final result: `lake build` succeeded without warnings, and all 96 local
theorems and lemmas passed the complete axiom audit.** The required checks
are automated by [build.sh](lean/build.sh):

1. `lake build` must succeed.
2. `MakeAudit.py` emits `#check` and `#print axioms` for every locally
   declared theorem and lemma, including reused core and primality helpers.
3. `lake env lean AuditAll.lean` must succeed.
4. `VerifyAudit.py` checks that every expected report exists, permits only
   `propext`, `Classical.choice`, `Quot.sound`, rejects admitted proofs and
   custom axioms, and records SHA-256 hashes of the checked source.

The final records are [Build.txt](lean/Build.txt),
[AuditAll.lean](lean/AuditAll.lean), [AuditAll.txt](lean/AuditAll.txt),
[Verification.json](lean/Verification.json),
[Toolchain.txt](lean/Toolchain.txt), and
[SourceHashes.json](lean/SourceHashes.json). The build accounting includes
cached Mathlib jobs; it does not mean every dependency was recompiled.

The `#print axioms` reports for the main byte-encoding, projection,
clamped-cardinality, GHASH, and Poly1305 tag theorems all contain only:

```text
[propext, Classical.choice, Quot.sound]
```

No probabilistic encoding hypothesis, primality axiom, root-count axiom,
`sorryAx`, or `Lean.ofReduceBool` is part of their trusted base. The exact
logarithmic metric minimization and the audit's explicit 64-bit-seed
counterargument are explained in [AUDIT_CLASSIC.md](AUDIT_CLASSIC.md);
the Lean deliverable proves the probability envelopes and word/block
conversion rather than a theorem about real logarithmic minima.

## 5. Separate implementation check

[CheckSeededGHASH.py](lean/CheckSeededGHASH.py) and
[SeededGHASH.json](lean/SeededGHASH.json) record a real seed-zero collision
between the empty message and a fixed 16-byte message, independently
derived by bit-serial GCM field arithmetic and verified against OpenSSL
3.5.5 GMAC. This supports the finding that the wrappers cannot inherit
the ideal-key bounds. It is deliberately not counted as a Lean theorem,
a re-run of the benchmark suite, or a verification of native code.
