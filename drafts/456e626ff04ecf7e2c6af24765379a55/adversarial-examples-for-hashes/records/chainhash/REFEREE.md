# Adversarial referee report: Lean 4 collision theorem for ChainHash-256

Referee run: 2026-09-18. Claim under review:
`scratchpad/codex/lean-chainhash/LEAN_CHAINHASH_STATUS.md` (Codex lane).

## Verdict

**HOLDS WITH CAVEATS.**

The theorem is real, unconditional inside Lean, non-vacuous, and about the
right function. Every caveat below is a *scope* boundary that the status file
and the paper already state; none of them is a modelling shortcut that changes
the mathematics, and none of them is an undischarged hypothesis of the
displayed theorem. I found **no divergence** from the paper's
`\Cref{def:ph:hash}` or from `chainhash_ref::hash<32,5,1>`.

Caveats (all pre-declared by the claimant, repeated here because a reader of
the headline sentence could over-read it):

- **C1 (the one that matters).** The Lean object is a *transcription* of
  `chainhash_ref.h` written by hand. The Lean definitions are `noncomputable`
  (`Word w = Fin w → ZMod 2`, `%ₘ`, `AdjoinRoot`), so there is no in-Lean
  executable differential test against the C++. The Lean↔C++ link is
  human-audited. I reduced this risk independently: I transcribed the *Lean*
  definitions literally into Python and compared against the compiled
  `chainhash_ref::hash<32,5,1>` on 100 messages (all lengths 0–79, plus
  127/128/129, 255/256/257, 300, 511/512/513, 767/768/769, 1024/1025, 2000,
  and structured all-zero/all-ff cases) with a random 41-word key:
  **100 compared, 0 mismatches.** Scripts:
  `scratchpad/codex/lean-chainhash-referee/{lean_model.py,drive.cpp,cmp.py}`.
- **C2.** Scope is the *bit-serial reference*, not the shipped SIMD code.
  `tools/bench/chainhash/chainhash.h` and `smhasher3/hashes/chainhash.cpp`
  are tested (T1/T8) against the reference, not proved equal to it. The paper
  says the same (`appendix_chainhash.tex:1112`).
- **C3.** Ideal key only: 41 independent uniform 64-bit words. The
  SplitMix64-from-64-bit-seed subfamily (`Key::from_seed`) is *not* covered.
  Paper says the same (`appendix_chainhash.tex:1099`).
- **C4.** Only the `(W,S) = (32,1)` = ChainHash-256 configuration. The 1 KB
  `(128,5,2)` configuration is not covered. The claim did not assert it.
- **C5.** The implementation-facing theorem carries `8*L + 255 < 2^64`
  (i.e. `L ≤ 2^61 − 32`) rather than the mathematically sufficient
  `8*L < 2^64`. This is *stronger than needed for the mathematics* and exists
  to rule out `size_t` overflow in the C++ `(len + BB - 1)/BB`. The weaker-cap
  version is separately proved (`chainHash_collision_bound`). It is not a
  practical restriction and it never weakens the result.
- **C6.** The two `set_option maxHeartbeats` raises and three `linter.*`
  disables in the modulus-certificate modules are elaboration-budget /
  style-lint knobs only. No `debug.skipKernelTC`, no `trustCompiler`.

## 1. Clean rebuild and axiom audit

Remote: `thomas-ahle@hardware.normalcomputing.net:~/agents/verify-chainhash-full`
(fresh `cp -a` of `~/agents/lean-chainhash`, `.lake/packages` cache kept,
`.lake/build` **deleted**, so every project module was re-elaborated from
source). Built with `nice -n 10 taskset -c 32-63`, `LEAN_NUM_THREADS=32`.

- `lake build`: `Build completed successfully (7389 jobs)`, `EXIT=0`.
  **Zero** `error`, **zero** `warning`, **zero** `declaration uses 'sorry'`
  in the whole log (`~/agents/verify-chainhash-full/clean-build.log`).
- Toolchain as claimed: `leanprover/lean4:v4.24.0`; mathlib at
  `f897ebcf72cd16f89ab4577d0c826cd14afaafc7`.
- **Dependency checkouts are pristine.** `git status --porcelain` is empty for
  all nine packages (mathlib, batteries, aesop, Qq, Cli, LeanSearchClient,
  importGraph, plausible, proofwidgets). No patched Mathlib. (And a patched
  Mathlib would in any case surface as `sorryAx` in `#print axioms` of the
  main theorem, which it does not.)

### Token scan

`grep -rnE '\b(sorry|admit|native_decide|unsafe|implemented_by|opaque|partial def)\b|^\s*axiom |ofReduceBool|skipKernelTC|trustCompiler'`
over all 39 `ProvenHashes/*.lean` plus the four top-level `.lean` files, on
**both** the local mirror and the remote build tree:

| token | occurrences |
| --- | --- |
| `sorry` | 0 |
| `admit` | 0 |
| `axiom ` (declaration) | 0 |
| `native_decide` | 0 |
| `unsafe` | 0 |
| `implemented_by` | 0 |
| `opaque` | 0 |
| `partial def` | 0 |
| `ofReduceBool` / `skipKernelTC` / `trustCompiler` | 0 |

Only `set_option` uses are `maxHeartbeats` (1.2M / 4M / 8M) and three
`linter.unused*/unreachable*` disables — see C6.

### `#print axioms`

I re-ran the project's own `FullAudit.lean` on the clean tree and
independently parsed it: **271 declarations**, the printed set is exactly the
271 the audit file asks for, and **every** axiom list is a subset of
`{propext, Classical.choice, Quot.sound}`. No `sorryAx`, no
`Lean.ofReduceBool`, no `Lean.ofReduceNat`, no `error` in the output.

I then wrote my own audit (`RefereeAudit.lean`, uploaded to the remote tree)
covering the main theorem and every lemma the status file names as a
dependency. All report only the three standard axioms:

```
'ProvenHashes.ChainHash.collision_bound_bytes'                depends on axioms: [propext, Classical.choice, Quot.sound]
'ProvenHashes.ChainHash.reference_collision_bound'            depends on axioms: [propext, Classical.choice, Quot.sound]
'ProvenHashes.ChainHash.referenceHash_matches'                depends on axioms: [propext, Classical.choice, Quot.sound]
'ProvenHashes.ChainHash.chainHash_collision_bound'            depends on axioms: [propext, Classical.choice, Quot.sound]
'ProvenHashes.ChainHash.chainHash_collision_bound_no_overflow' depends on axioms: [propext, Classical.choice, Quot.sound]
'ProvenHashes.ChainHash.collision_bound_model'                depends on axioms: [propext, Classical.choice, Quot.sound]
'ProvenHashes.ChainHash.collision_bound_words_model'          depends on axioms: [propext, Classical.choice, Quot.sound]
'ProvenHashes.ChainHash.modulus_irreducible'                  depends on axioms: [propext, Classical.choice, Quot.sound]
'ProvenHashes.ChainHash.integerTwist_bijective'               depends on axioms: [propext, Quot.sound]
'ProvenHashes.ChainHash.chain5_collision_exact'               depends on axioms: [propext, Classical.choice, Quot.sound]
'ProvenHashes.ChainHash.stream_collision_bound'               depends on axioms: [propext, Classical.choice, Quot.sound]
'ProvenHashes.ChainHash.rawStream_collision_bound'            depends on axioms: [propext, Classical.choice, Quot.sound]
'ProvenHashes.ChainHash.clnh_difference_bound'                depends on axioms: [propext, Classical.choice, Quot.sound]
'ProvenHashes.ChainHash.clnh_nested_nonzero_bound'            depends on axioms: [propext, Classical.choice, Quot.sound]
'ProvenHashes.ChainHash.block_encoding_injective'             depends on axioms: [propext, Classical.choice, Quot.sound]
'ProvenHashes.ChainHash.fieldRepr_mul'                        depends on axioms: [propext, Classical.choice, Quot.sound]
'ProvenHashes.Recurrence.collision_bound'                     depends on axioms: [propext, Classical.choice, Quot.sound]
'ProvenHashes.Recurrence.collision_bound_any_length'          depends on axioms: [propext, Classical.choice, Quot.sound]
'ProvenHashes.chainhash_equal_length_from_stages'             depends on axioms: [propext, Classical.choice, Quot.sound]
'ProvenHashes.chainhash_different_lengths_from_stages'        depends on axioms: [propext, Classical.choice, Quot.sound]
'ProvenHashes.ChainHash.bytesToMessage_injective'             depends on axioms: [propext, Classical.choice, Quot.sound]
'ProvenHashes.ChainHash.idealKey_card'                        depends on axioms: [propext, Classical.choice, Quot.sound]
```

(The only non-zero exit from my audit file was my own typo,
`ProvenHashes.ChainHash.uniformProb` — the constant lives in the
`ProvenHashes` namespace. Everything else elaborated.)

**Provenance:** all 41 source files (`ProvenHashes/*.lean`, `ProvenHashes.lean`,
`lakefile.toml`, `lean-toolchain`, `lake-manifest.json`) are byte-identical
between the local mirror I read and the remote tree I built. So the code
reviewed below is the code that was checked.

### Independent re-derivation of the irreducibility certificate

The field's correctness rests on `modulus_irreducible` for
`X^64 + X^4 + X^3 + X + 1`, proved from 65 `residue_i` constants plus a Bézout
certificate, all checked by `ring`. I recomputed all of it outside Lean:

- all 65 `residue_i` equal `X^(2^i) mod Π` — **0 mismatches**;
- `residue_0 = X` and `residue_64 = X`, i.e. `Π | X^(2^64) − X`;
- `Π·a + (residue_32 − X)·b = 1` for the two literal sparse cofactors — **true**;
- independently, `gcd(Π, X^(2^32) + X) = 1` — **true**.

`binary_rabin64` is Rabin's criterion correctly specialised (64 = 2^6, the
only prime divisor of 64 is 2, so only the `n/2 = 32` gcd is needed), and the
proof structure (monic irreducible factor `g`, `deg g | 64`, `deg g = 2^k`,
`k ≤ 5` contradicts coprimality, so `k = 6` and `p = g`) is sound.

## 2. Transcription fidelity checklist

Compared three ways: Lean source ↔ paper `sections/appendix_chainhash.tex`
(`def:ph:hash`, lines 100–236) ↔ `tools/bench/chainhash/chainhash_ref.h`
(SHA-256 of the snapshots in `reference/` matches the repo files exactly:
`18bc8491…` for `chainhash_ref.h`, `f3c4e96d…` for `chainhash.h`).

| # | Item | Paper | `chainhash_ref.h` | Lean | Verdict |
| --- | --- | --- | --- | --- | --- |
| 1 | Field | `𝔽₂[X]/(X^64+X^4+X^3+X+1)`, bit i = coeff of X^i | `P = (1<<64)|27`, bit-serial `reduce` | `modulus = X^64+(X^4+X^3+X+1)`; `BinaryQuotient = AdjoinRoot modulus`; `Fact (Irreducible modulus)` proved | ✅ |
| 2 | Field mul = clmul then reduce | `a·b = (ab) mod Π` | `gfmul = reduce(clmul(a,b))` | `wordMul a b = lowWord ((pack a * pack b) %ₘ modulus)`; `fieldRepr_mul` proves it *is* field mul | ✅ |
| 3 | Word layout / byte order | little-endian 64-bit words, bit i = 2^i | `word_at`: `m[8i+t] << (8t)` | `wordAt m j i = byteAt m (8j + i/8) (i%8)` | ✅ little-endian, identical |
| 4 | Zero padding, no OOB read | "no byte beyond ℓ is ever read" | `if (byte < len)` | `byteAt m j = m[j]?.getD 0` | ✅ |
| 5 | Block count | `n = max{1, ⌈ℓ/B⌉}` | `(len==0)?1:(len+255)/256` | `blockCount = max 1 ((len+255)/256)` | ✅ (equal for all `len`) |
| 6 | Last-block byte count | `r_t = min{B_s, max{0, ℓ−(t−1)B_s}}` | `r = (j+1==n)? len−j*BB : BB` | `blockBytes m t = min 256 (len − 256t)` (ℕ-subtraction = `max 0`) | ✅ |
| 7 | Pair count / group rounding | `w_t = 2G_t`, `G_t = ⌈r_t/32⌉` | `W = 2*((r+31)/32)` | `activePairs = {s : Fin 16 | s < 2*((blockBytes+31)/32)}` | ✅ |
| 8 | Strided pairing | `(ω_{4j},ω_{4j+2})`, `(ω_{4j+1},ω_{4j+3})`; π swaps 4j+1↔4j+2 | `pair_alpha(s)=4*(s/2)+s%2`, `pair_beta=α+2` | `pairPosition (s,b) = 4*(s/2)+s%2+(2 if b)`; proved a bijection `Fin 16 × Bool ≃ Fin 32` | ✅ exact |
| 9 | Key uses the same permutation | `κ^(i) = π(k…)` — key word of its own position | `key.k[wa_i]`, `key.k[wb_i]` (position within the block) | `referenceBlockKey k j = k ⟨(pairPosition j).val⟩` | ✅ |
| 10 | CLNH = unreduced 128-bit sum | `Σ (g_{2s}+k'_{2s})(g_{2s+1}+k'_{2s+1}) ∈ R_{<128}`, XOR of carry-less products | `u128 acc`, `acc ^= clmul(wa,wb)` | `clnh s m k = Σ_{i∈s} pack(m+k)·pack(m+k)` in `Polynomial (ZMod 2)`, **no reduction**; `clnh_natDegree_le ≤ 126` | ✅ genuinely unreduced |
| 11 | lo/hi split | `w = lo(w) + X^64 hi(w)` | `(uint64_t)acc`, `acc>>64` | `lowWord p i = p.coeff i`, `highWord p i = p.coeff (i+64)`; `splitWords_injective` for `natDegree < 128` | ✅ lossless |
| 12 | Length into **both** halves of the **last** pair | `a_t = lo+[t=p]ℓ`, `b_t = hi+[t=p]ℓ` | `if (j+1==n && i+1==S) { a ^= len; b ^= len; }` | `len := if t+1 = blockCount then lengthWord m.length else 0`; added to **both** | ✅ |
| 13 | Short messages / `(0,0)` pair | "1–16 bytes ⇒ w₁=2 with ω₂=ω₃=0; ℓ≤8 ⇒ second pair is (0,0)" | same by `word_at` padding | `activePairs = {0,1}`, `blockData` reads zero-padded words ⇒ `(0,0)` | ✅ |
| 14 | Empty message | one block, no active pairs, last pair `(0,0)` | `n=1, r=0, W=0`, `a=b=0^0` | `blockCount = 1`, `activePairs = ∅`, `clnh = 0`, `len = 0` | ✅ |
| 15 | Recurrence | `P₀=z`, `P_t=a_t+(b_t+y)(P_{t−1}+u)` | `P=key.z`; `P = a ^ gfmul(b^key.y, P^key.u)` | `wordFold u y s p = foldl (fun p ab => ab.1 + wordMul (ab.2+y) (p+u)) p`, init `k 34` | ✅ |
| 16 | Key indices | `κ=k₀…k₃₁, θ=(u,y,z), c₀…c₄, τ` (W+9 = 41) | derivation order `k[0..32), u, y, z, c[0..5), t_in` | `Key41 = Fin 41 → Word 64`; 0–31 CLNH, 32=u, 33=y, 34=z, 35–39=c, 40=τ; `keyEquiv` is a proved bijection | ✅ |
| 17 | Finalizer circuit | `G₁=X·X; G₂=(G₁+c₀)(X+G₁+c₁); f=(X+c₂)(G₂+c₃)+c₄` | `y=gfmul(v,v); z=gfmul(y^c0, v^y^c1); t=gfmul(v^c2, z^c3); return t^c4` | `wordChain5 c v` — same three multiplications, same gate order | ✅ |
| 18 | Monicity of the finalizer | monic of degree 5 (`lem:ph:chain`) | — | `chain5_expansion : chain5 c v = v^5 + Σ_{i<5} coefficients c i · v^i`, proved by `ring`; `coefficientEquiv` is a proved bijection `(Fin 5→F) ≃ (Fin 5→F)` | ✅ monic, coefficient map bijective |
| 19 | Twist = integer add mod 2^64 | `v ⊞ τ`: add the 64-bit words as integers, carries and all; bit i = coeff of X^i = digit 2^i | `chain<K>(key.c, P + key.t_in)` — plain `uint64_t` add | `wordAdd a b = (wordIntegerEquiv 64).symm (… a + … b)` in `ZMod (2^64)`; `wordIntegerEquiv` goes `Word 64 → BitVec 64 → Fin (2^64) → ZMod (2^64)` with bit i ↦ 2^i (LSB-first `ofBoolListLE`) | ✅ **genuine mod-2^64 carry add, not an XOR stand-in** |
| 20 | Twist bijectivity (paper's `cor:ph:twist`) | `ψ_τ(v)=v⊞τ` bijective, inverse `v⊟τ` | — | `integerTwist_bijective` **proved** (both directions), and it is the form actually consumed by `chain5_twisted_collision_exact` | ✅ required lemma is proved, not assumed |
| 21 | Output | `H = f_c(V ⊞ τ) ∈ 𝔽` | full `uint64_t` | `hashBytes k m : UInt64 = ⟨bitVecOfBits (referenceHash k …)⟩`; `hashBytes_eq_iff` via a proved `Word 64 ≃ BitVec 64` | ✅ full 64-bit output, no truncation |
| 22 | Composition | `E₁∪E₂∪E₃`; equal counts `1+p+1`, unequal `0+(p+1)+1` | — | `chainhash_equal_length_from_stages` / `chainhash_different_lengths_from_stages`, both discharged in `collision_bound_model` | ✅ same case split |
| 23 | Byte↔word encoding injectivity | "(m₁,…,m_p,ℓ) determines m" | — | `block_encoding_injective` + `byte_words_injective` + `lengthWord_injective` (for `n < 2^64`) | ✅ |
| 24 | `referenceHash = chainHash` | — | — | `referenceHash_matches` **proved**; the word-level transcription and the field-level function used by the probability proof are literally equal | ✅ |

**End-to-end executable cross-check (C1):** Python transcription of the Lean
definitions vs compiled `chainhash_ref::hash<32,5,1>`, 100 messages,
**0 mismatches**.

## 3. Statement fidelity

Elaborated signature from the clean build:

```
collision_bound_bytes : ∀ (L : ℕ),
  8 * L + 255 < 2 ^ 64 →
    ∀ (m m' : List UInt8),
      m.length ≤ 8 * L → m'.length ≤ 8 * L → m ≠ m' →
        (uniformProb fun k => hashBytes k m = hashBytes k m')
          ≤ ↑(max 1 ((L + 31) / 32) + 2) / 2 ^ 64
```

- **Exact uniform probability, full key space.** `uniformProb E =
  (Finset.univ.filter E).card / Fintype.card K` — a counting probability, not
  a conditional one, not an expectation over a sub-family. `Key41 = Fin 41 →
  Word 64` with `Word 64 = Fin 64 → ZMod 2`; I verified in Lean that
  `Fintype.card Key41 = (2^64)^41`. All 41 words independent and uniform.
- **No hidden hypotheses.** Exactly four: the length cap `8L+255 < 2^64`, the
  two `≤ 8L` length bounds, and `m ≠ m'`. **No** equal-length assumption,
  **no** non-emptiness, **no** alignment, **no** whole-word requirement, **no**
  block-count hypothesis. Partial words and the empty string are in scope.
- **Distinctness is as byte strings.** `m m' : List UInt8`, `hne : m ≠ m'`.
  The proof lifts it through `bytesToMessage_injective`, so two byte strings
  that pad to the same word sequence are still handled correctly — indeed the
  equal-length case recovers *every* in-range byte
  (`byte_words_injective`), and different lengths are separated by the length
  word.
- **Constant matches the paper for all L.** Paper: `(Sn+2)/2^64`, `S=1`,
  `n ≥ max{1,⌈ℓ/256⌉}`. Lean: `p = max 1 ⌈L/32⌉` with `ℓ ≤ 8L`, and
  `max 1 ((8L+255)/256) = max 1 ((L+31)/32)` in ℕ-division. Checked in Lean:
  `L=0 → 3`, `L=1 → 3`, `L=32 (256 B) → 3`, `L=33 → 4`, `L=131072 (1 MiB) →
  4098` — all identical to the paper's stated numbers (`3/2^64` for ≤256 B,
  `4098/2^64 ≈ 2^-52` for ≤1 MB). `L < 32` correctly floors at 1 via the
  `max 1`, matching the reference's empty-message block.
- **Bound is never trivial.** The cap forces `L < 2^61`, so
  `ε ≤ (2^56+2)/2^64 < 2^-8 < 1` for every admissible `L`.

## 4. Non-vacuity

Hypothesis types are all inhabited, and I proved it rather than argued it. Two
concrete instantiations elaborated in the clean tree, each with the standard
axiom list:

```lean
theorem referee_tiny_instance :                  -- L = 1, equal length
    uniformProb (fun k : Key41 => hashBytes k [0] = hashBytes k [1])
      ≤ ((3 : ℕ) : ℚ≥0) / (2 : ℚ≥0) ^ 64 := …
theorem referee_tiny_instance2 :                 -- L = 1, empty vs 1 byte
    uniformProb (fun k : Key41 => hashBytes k [] = hashBytes k [7])
      ≤ ((3 : ℕ) : ℚ≥0) / (2 : ℚ≥0) ^ 64 := …
'referee_tiny_instance'  depends on axioms: [propext, Classical.choice, Quot.sound]
'referee_tiny_instance2' depends on axioms: [propext, Classical.choice, Quot.sound]
```

Both discharge `8*1+255 < 2^64`, the two length bounds and `m ≠ m'` by
`norm_num`/`decide`, so the hypothesis set is satisfiable, the conclusion is a
number strictly below 1, and the theorem has real content at the smallest
scale (including the empty message and unequal lengths).

Vacuity of the internal lemmas was also checked by inspection: the two CLNH
lemmas take `∃ i ∈ s, ∃ b, m ≠ m'` and `C ≠ 0` respectively, both of which the
stream proof *constructs* (from `block_encoding_injective` and from
`lengthMask_injective`), not assumes; `chain5_collision_exact` is an equality
(`= 1/card F`), not an inequality, so it cannot be vacuously true; and
`Recurrence.collision_bound` derives `0 < m.length` internally rather than
assuming it.

## 5. What I could not rule out

- The Lean↔C++ correspondence is not machine-checked (C1). My 100-message
  external cross-check is strong evidence but not a proof. A future hardening
  would be a computable `Vector Bool 64` model with `decide`-based unit tests
  inside Lean, or extraction to a runnable Lean function checked against the
  C++ vectors.
- I did not audit the *fast* SIMD path or the SMHasher3 integration (C2); the
  claim does not cover them.
- `#print axioms` protects against a doctored Mathlib (a `sorry` anywhere in
  the transitive closure would surface as `sorryAx`), and all nine package
  checkouts were clean at their pinned revisions, so I consider this closed.

## Plain summary

The claim survives adversarial review. From a clean rebuild — `.lake/build`
deleted, every project module re-elaborated, 7389 jobs, zero errors, zero
warnings, zero `sorry` — the 271 audited declarations all depend on nothing
beyond `propext`, `Classical.choice` and `Quot.sound`, the nine dependency
checkouts are pristine at their pinned revisions, and no `sorry`, `admit`,
custom `axiom`, `native_decide`, `unsafe`, `implemented_by`, `opaque` or
`partial def` appears anywhere in the sources. The statement is an exact
uniform counting probability over the full `(2^64)^41` ideal key space with
exactly four hypotheses — a length cap that exists only to mirror the C++'s
`size_t` rounding, two length bounds, and distinctness of the two *byte
strings* — and no hidden equal-length, non-emptiness or alignment condition;
its constant `(max(1,⌈L/32⌉)+2)/2^64` reproduces the paper's `(Sn+2)/2^64`
for every `L`, including `L < 32` where the `max 1` matches the reference's
empty-message block. Operation by operation the Lean definition agrees with
both `def:ph:hash` and `chainhash_ref::hash<32,5,1>`: little-endian word
assembly with zero padding and no out-of-range read, the strided
`(ω_{4j},ω_{4j+2})`/`(ω_{4j+1},ω_{4j+3})` pairing applied identically to
message and key words, a genuinely *unreduced* 128-bit carry-less NH sum split
losslessly into low and high halves, the byte length XORed into both halves of
the last pair only, the `P₀=z, P_t=a_t+(b_t+y)(P_{t−1}+u)` recurrence, the
three-multiplication quintic (proved monic with a bijective coefficient map),
and a twist that really is integer addition modulo `2^64` with carries —
modelled through a proved `𝔽 ≃ ZMod (2^64)` bit-identification and proved
bijective, exactly as the paper's `cor:ph:twist` requires, not silently
replaced by an XOR. The `X^64+X^4+X^3+X+1` irreducibility is a real Rabin
certificate whose 65 residues and Bézout cofactors I recomputed independently
and confirmed; and a literal Python transcription of the Lean definitions
matches the compiled C++ reference on 100 messages spanning every
partial-group, block-boundary and empty-input corner. The theorem is
non-vacuous — I instantiated it inside Lean at `L = 1` for `[0]` vs `[1]` and
for `[]` vs `[7]`. The only reservations are scope boundaries the claimant
already declared: it is the bit-serial reference, not the SIMD code or the
SplitMix64-seeded subfamily, only the 256-byte `S=1` configuration, and the
C++↔Lean transcription itself is human-audited rather than machine-checked.

## Reproduction

```bash
ssh thomas-ahle@hardware.normalcomputing.net
cp -a ~/agents/lean-chainhash ~/agents/verify-chainhash-full
cd ~/agents/verify-chainhash-full && sed -i 's|lean-chainhash|verify-chainhash-full|g' env.sh
source env.sh && cd lean && rm -rf .lake/build
LEAN_NUM_THREADS=32 nice -n 10 taskset -c 32-63 lake build
LEAN_NUM_THREADS=32 nice -n 10 taskset -c 32-63 lake env lean FullAudit.lean
LEAN_NUM_THREADS=32 nice -n 10 taskset -c 32-63 lake env lean RefereeAudit.lean
```

Referee artefacts (this directory):
`RefereeAudit.lean` (also at `~/agents/verify-chainhash-full/lean/`),
`lean_model.py`, `drive.cpp`, `cmp.py`.
Remote logs: `~/agents/verify-chainhash-full/{clean-build.log,referee-audit.txt,FullAudit.out,Audit.out,ChainHashAudit.out}`.
