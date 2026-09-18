# PolymurHash 2.0: machine-checked ideal collision bound

Verified 2026-09-18 on `thomas-ahle@hardware.normalcomputing.net`, in
`~/agents/lean-polymur/lean`. The Mac holds the source mirror and reports;
all Lean compilation and proof checking ran on the Xeon. The cardinality
completion uses `nice -n 10`, `taskset -c 72-79`, and `LEAN_NUM_THREADS=8`.
The existing dependency cache was retained.

The key-cardinality certificate is now proved in Lean:
`cardinalityCertificate : CardinalityCertificate`, meaning
`189729088763903999 ≤ keyCard`. The interval character bound, subgroup inversion,
and six primitive seventh-root lifts apply to the exact existing `Key` definition.
The numerical core/idealHash bounds and score ≥54.2267 are unconditional for
the ideal uniform key space. The existing initializer-distribution and C-refinement
scope is unchanged.

## Verification and milestone commits

| Milestone | Verified result | Remote commit |
|---|---|---|
| M1 | **COMPILED**: byte-list encoding, field-level injectivity, piecewise difference degree | `d9be4fdca8aab59b7b055c135d94756a07f64418` |
| M2 | **COMPILED**: ideal uniform-key AU, exact finalizer, representative transfer | `8c1a3858542de7f42bc4c0142df136d6a90bb36b` |
| M3 | **COMPILED, CONDITIONAL**: explicit cardinality hypothesis and `D(n)/K0` corollaries | `072a37a82d861ad42c0aea7d94cce2fcbb00c96a` |
| M4 | **COMPILED, CONDITIONAL**: exact minimum formula and score ≥54.2267 | `eda15ada3c0742cb6d10f66875a6d655813e35e8` |
| Character sums | **COMPILED**: dyadic completion with constant `65295510750` | `22e3aa4` |
| Exact-order counts | **COMPILED**: subgroup indicator, Möbius inversion, interval specialization | `6804f6e` |
| Cardinality completion | **COMPILED, UNCONDITIONAL**: `K0 ≤ keyCard` | `03bb622` |
| Numeric corollaries | **COMPILED, UNCONDITIONAL**: core, idealHash, score ≥54.2267; 114-declaration audit | `80ff535` |

The original M3/M4 conditional lemmas are retained; `Certified.lean` instantiates
them with the proved certificate. Their `_unconditional` corollaries require no
cardinality or character-sum hypothesis.

Each milestone has a successful full `lake build` and a checked signature/axiom
report: [M1 build](lean/M1Build.txt), [M1 axioms](lean/M1Axioms.txt),
[M2 build](lean/M2Build.txt), [M2 axioms](lean/M2Axioms.txt),
[M3 build](lean/M3Build.txt), [M3 axioms](lean/M3Axioms.txt),
[M4 build](lean/M4Build.txt), [M4 axioms](lean/M4Axioms.txt).

The final regenerated audit covers **114 Polymur proof declarations**, including every local
theorem and lemma and the named primality/nonemptiness instances. Their only
axioms are subsets of `propext`, `Classical.choice`, and `Quot.sound`; some
declarations use no axioms. No proof uses `sorry`, `admit`, a declared axiom,
`native_decide`, or `Lean.ofReduceBool`. The inherited 57 proof declarations
have a separate complete audit.

- [Final build and Polymur audit result](lean/PolymurVerification.txt)
- [Cardinality completion build and audit](lean/CardinalityVerification.txt)
- [All Polymur signatures and axioms](lean/PolymurAxioms.txt)
- [Generated Polymur audit source](lean/PolymurAudit.lean)
- [Inherited build and audit result](lean/Verification.txt)
- [Inherited complete signatures and axioms](lean/FullAudit.txt)
- [Reproduction instructions](lean/README.md)

Toolchain: Lean **4.24.0**, compiler commit
`797c613eb9b6d4ec95db23e3e00af9ac6657f24b`; Mathlib **v4.24.0**, commit
`f897ebcf72cd16f89ab4577d0c826cd14afaafc7`. The inherited ProvenHashes snapshot
is `e70213feed3dac6280fe8d7ecdc35dcfd0cf1cc8`.

## M1: the byte polynomial

Namespace: `ProvenHashes.Polymur`. `Byte = Fin 256`, `Bytes = List Byte`,
`p = 2305843009213693951`, and `F = ZMod p`. Primality is proved by a
kernel-checked Lucas–Lehmer certificate.

`pack` is little-endian base-256 encoding. `window` reads up to seven bytes.
`pack_lt`, `window_inj`, and `window_byte` prove the 56-bit bound and recover
every byte from overlapping windows. The source's loop count is
`blockCount m = (m.length - 1)/49`, with natural subtraction, so the empty
message is included. `tail` retains at most 49 bytes.

The function is

```lean
encode m = prefixPoly (blocks m) * Polynomial.X^14 + tailPoly (tail m)
coefficients m = (encode m).coeff
core k m = (encode m).eval k
```

`block_source`, `short_source`, `mid_source`, and `long_source` prove the
factored identities matching the C multiplications. `prefix_fold` proves
agreement with the block-Horner loop. The block constant is retained when
decoding. The short, middle, and long tail branches include the length
coefficient; overlapping reads cover all bytes. Injectivity is proved over
the field, including unequal message lengths and the empty message.

Main signatures, with the surrounding namespace omitted:

```lean
encode_injective : Function.Injective encode
coefficients_injective : Function.Injective coefficients
difference_degree {n : ℕ} (a b : Bytes)
  (ha : a.length ≤ n) (hb : b.length ≤ n) :
  (encode a - encode b).natDegree ≤ D n
```

The exact reviewed piecewise bound is

```text
D(n) = 2                         for 0 ≤ n ≤ 7
       9                         for 8 ≤ n ≤ 21
       13                        for 22 ≤ n ≤ 49
       7 * floor((n-1)/49) + 14   for n ≥ 50.
```

The `n=0` extension is harmless: no distinct byte lists satisfy that cap.
The common cubic cancels in the first branch.

Sources: [Bytes](lean/ProvenHashes/Polymur/Bytes.lean),
[Algebra](lean/ProvenHashes/Polymur/Algebra.lean),
[Prefix](lean/ProvenHashes/Polymur/Prefix.lean),
[Tail](lean/ProvenHashes/Polymur/Tail.lean),
[Encoding](lean/ProvenHashes/Polymur/Encoding.lean).

## M2: exact ideal multiplier set and AU

The ideal acceptance set is defined explicitly:

```lean
def Admissible (k : F) : Prop :=
  k ≠ 0 ∧ orderOf k = p-1 ∧ (k^7).val < 2^60-2^56
abbrev Key := {k : F // Admissible k}
def keyCard : ℕ := Fintype.card Key
```

This is the primitive-element/seventh-power condition from the generator,
with the canonical seventh-power residue and a strict threshold. It is not
the whole field or a guessed key density. `generator37_order` proves that
37, the source's base, has order `p-1`, by exact modular powers at every prime
factor of `p-1`. `admissible37` proves that 37 itself passes the bound, so
`keyCard_pos` is unconditional. A formal equivalence to the C seed loop and
its distribution is outside this theorem.

`uniformProb` is the exact nonnegative rational event count divided by the
finite key-space cardinality. The main bound is:

```lean
core_collision_bound {n : ℕ} (a b : Bytes)
  (ha : a.length ≤ n) (hb : b.length ≤ n) (hne : a ≠ b) :
  uniformProb (fun k : Key => core k.val a = core k.val b)
    ≤ (D n : ℚ≥0) / keyCard
```

`key_root_count` injects roots in `Key` into field roots and reuses
`ProvenHashes.polynomial_zero_count`. Message injectivity makes the difference
polynomial nonzero. Messages and the common tweak are fixed independently of
the sampled ideal multiplier.

`Word = BitVec 64`. `mix` uses exactly the source's xor shifts 32, 32, 28 and
both multiplications by `0x0e9846af9b1a615d`, with 64-bit wrapping arithmetic.
The multiplier inverse `0x153ed04bd89cfaf5` is kernel checked. The source's
secret addition is `finish s x = mix x + s`; `finish_bijective` proves that
this is a bijection. `finish_tweak_eq_iff` also includes the common addition
before the mixer.

```lean
finish_bijective (s : Word) : Function.Bijective (finish s)
finish_tweak_eq_iff (s tweak x y : Word) :
  finish s (x+tweak) = finish s (y+tweak) ↔ x = y
```

`idealHash` embeds the canonical field residue as a 64-bit word before this
exact finalizer. `idealHash_collision_bound` proves the same `D(n)/keyCard`
bound. The secret may be an arbitrary `s : Key → Word`, because a common
translation preserves equality. No independence or uniformity assumption
on the additive secret is needed for this collision argument.

The C routine uses lazy representatives, so `idealHash` is not asserted to be
bit-for-bit the C hash. The separate `representative_collision_bound` proves
the same bound for any `rep : Key → Bytes → Word`, provided

```lean
hrep : ∀ k m, ((rep k m).toNat : F) = core k.val m
```

The C arithmetic/overflow refinement must establish this congruence and
identify its return value with the stated finalizer. Equality of lazy
representatives then implies field equality; canonicality is unnecessary.
This obligation is explicit, not silently assumed as a theorem.

Sources: [Keys](lean/ProvenHashes/Polymur/Keys.lean),
[Mix](lean/ProvenHashes/Polymur/Mix.lean),
[Collision](lean/ProvenHashes/Polymur/Collision.lean).

## M3: certified denominator

```lean
def K0 : ℕ := 189729088763903999
def CardinalityCertificate : Prop := K0 ≤ keyCard
```

`CardinalityCertificate` is a proposition definition, proved without hypotheses
by `cardinalityCertificate`. The original `core_collision_bound_K0` and
`idealHash_collision_bound_K0` transport a cardinality certificate into a bound.
`core_collision_bound_K0_unconditional` and
`idealHash_collision_bound_K0_unconditional` supply the proved certificate and
have denominator `K0` without that parameter.

The mathematical certificate described in
[POLYMUR_REVIEW_SECTION.md §1.2](POLYMUR_REVIEW_SECTION.md), with exact arithmetic
in [checks.py](review-support/checks.py) and [results.json](review-support/results.json),
is now formalized. It uses

```text
N = p-1, m = N/7, H = 2^60-2^56-1,
φ(m) = 67744512000000000, 2^ω(m) = 2048,
C = 43 * 1518500250 = 65295510750,
|K| ≥ 6Hφ(m)/N - 6*2048*C ≥ K0.
```

The completion proof uses exact translate correlations and Cauchy–Schwarz.
Sixty dyadic smoothing steps cost at most `60 * 17 * B / 24`; the final average
costs at most `B/2`, where `B = 1518500250` and `p ≤ B²`. Their sum is `43*B`.
This avoids any need for a numerical approximation to `log p`.

A faithful character of the cyclic unit group gives subgroup indicators as
geometric sums. Möbius inversion then bounds the discrepancy for exact order
`m` by `2^ω(m)*C`. The totient and prime-factor constants are kernel checked.

For each interval element `x` of order `m`, put `r = 188232082384791343`
and `ζ = 37^m`. Lean verifies `7*r = 4*m+1` and `orderOf ζ = 7`.
The six elements `x^r * ζ^(i+1)`, `i : Fin 6`, have order `p-1`, seventh power
`x`, and are distinct. They therefore define an injection into the exact `Key`.

Finally, `keyCard_analytic_lower` proves the displayed real lower bound, and
`cardinalityCertificate` compares it with `K0` using exact rational arithmetic.
The lower-bound expression exceeds `K0` by exactly
`460694710548221 / 465826870548221`.

Sources: [Character sums](lean/ProvenHashes/Polymur/CharacterSum.lean),
[Exact-order counts](lean/ProvenHashes/Polymur/KeyCount.lean),
[Cardinality proof](lean/ProvenHashes/Polymur/KeyCardinality.lean),
[Unconditional corollaries](lean/ProvenHashes/Polymur/Certified.lean).

## M4: exact minimum and numerical lower bound

All real-valued quantities are defined in Lean:

```lean
epsilon L = (D (8*L) : ℝ) / keyCard
wordScore L = Real.logb 2 ((L : ℝ) / epsilon L)
score = sInf (wordScore '' {L : ℕ | 1 ≤ L})
```

`epsilon` is the proven AU upper-bound function, before optional probability
clipping. The score is for that bound; no claim of tight actual collision
probabilities is made. `core_collision_bound_words` connects `epsilon` to
the exact rational collision probability. `epsilon_pos` is unconditional.

```lean
D_eight : D 8 = 9
D_eight_mul_le (L : ℕ) (hL : 2 ≤ L) : D (8*L) ≤ 9*L
wordScore_lower (L : ℕ) (hL : 1 ≤ L) :
  Real.logb 2 ((keyCard : ℝ)/9) ≤ wordScore L
wordScore_one : wordScore 1 = Real.logb 2 ((keyCard : ℝ)/9)
score_eq : score = Real.logb 2 ((keyCard : ℝ)/9)
K0_log_lower : (54.2267 : ℝ) ≤ Real.logb 2 ((K0 : ℝ)/9)
score_lower_unconditional : (54.2267 : ℝ) ≤ score
```

Thus the infimum is attained at `L=1` and is a genuine minimum. The step for
`L≥2` uses the exact piecewise integer `D`, including its constant.
`K0_log_lower` is unconditional arithmetic/analysis: a five-term exponential
Taylor bound certifies `exp(0.15715) ≤ K0/(9*2^54)`, then Mathlib's proved
bound on `log 2` gives the exact rational target `542267/10000`. No floating
point calculation is trusted. The review's approximate value
`54.22679349755191` is consistent with the proved lower bound.

Source: [Metric](lean/ProvenHashes/Polymur/Metric.lean).

## What the shipped initializer still needs

For `polymur_init_params`, the seed update is a deterministic walk by the odd
constant `POLYMUR_ARBITRARY2`. It extracts `(k_seed >> 3) | 1`, rejects
noncoprime exponents, and accepts the first key passing the seventh-power
threshold. Uniform initial seeds do not by themselves prove uniform accepted
keys: each accepted state accumulates the seeds in its preceding rejection
run, and exponent extraction has its own multiplicities.

To obtain a bound for the shipped distribution, a separate development must:

1. Formalize the seed-to-key map, termination, word arithmetic, exponent tests,
   and the returned key's support in `Key`.
2. For the stated input-seed distribution, prove a maximum multiplier mass
   `μmax = max_k Pr[returned multiplier = k]`, accounting for all rejection
   runs and exponent preimages. Root counting then gives `D(n)*μmax`.
   The ideal denominator follows only from an appropriate uniformity result
   (or the corresponding mass bound).
3. Establish the C polynomial-arithmetic congruence required by `hrep`,
   including intermediate overflow and lazy reductions.

For uniform input to `polymur_init_params_from_seed`, the mixer permutation
preserves uniformity of its intermediate `k_seed`; this alone does not make
the eventual accepted multiplier uniform. Its two derived secrets need not
be independent. Dependence of the shared additive secret does not harm AU,
because the proved finalizer preserves equality; it does not establish ASU.

No ASU or additive-differential theorem, short-message numerical headline, or
guarantee for the shipped seed distribution is claimed here.

## Delivery and provenance

The source mirror is [lean/](lean/). It excludes the remote `.git`, toolchain,
dependency checkouts, and build caches. [SourceManifest.txt](lean/SourceManifest.txt)
records SHA-256 digests of the exact supplied C header, published proof, review,
and arithmetic certificates. The authoritative proof commits are listed above.
The supplied reference documents are preserved in the remote workspace as well.

The following appendix is the verbatim generated `#check` / `#print axioms`
output for every Polymur proof declaration. The inherited complete audit is
[FullAudit.txt](lean/FullAudit.txt).

## Complete Polymur signatures and axiom output

```text
ProvenHashes.Polymur.pack_lt : ∀ (m : ProvenHashes.Polymur.Bytes),
  List.length m ≤ 7 → ProvenHashes.Polymur.pack m < 2 ^ 56
'ProvenHashes.Polymur.pack_lt' depends on axioms: [propext, Classical.choice, Quot.sound]
@ProvenHashes.Polymur.cast_inj : ∀ {a b : ℕ}, a < ProvenHashes.Polymur.p → b < ProvenHashes.Polymur.p → ↑a = ↑b → a = b
'ProvenHashes.Polymur.cast_inj' depends on axioms: [propext, Classical.choice, Quot.sound]
@ProvenHashes.Polymur.pack_inj : ∀ {a b : ProvenHashes.Polymur.Bytes},
  List.length a = List.length b → ProvenHashes.Polymur.pack a = ProvenHashes.Polymur.pack b → a = b
'ProvenHashes.Polymur.pack_inj' depends on axioms: [propext, Classical.choice, Quot.sound]
@ProvenHashes.Polymur.window_inj : ∀ {a b : ProvenHashes.Polymur.Bytes},
  List.length a = List.length b →
    ∀ (start width : ℕ),
      width ≤ 7 →
        ProvenHashes.Polymur.window a start width = ProvenHashes.Polymur.window b start width →
          List.take width (List.drop start a) = List.take width (List.drop start b)
'ProvenHashes.Polymur.window_inj' depends on axioms: [propext, Classical.choice, Quot.sound]
@ProvenHashes.Polymur.window_byte : ∀ {a b : ProvenHashes.Polymur.Bytes} (hlen : List.length a = List.length b)
  {start width i : ℕ},
  width ≤ 7 →
    start ≤ i →
      ∀ (hi : i < List.length a),
        i < start + width →
          ProvenHashes.Polymur.window a start width = ProvenHashes.Polymur.window b start width → a[i] = b[i]
'ProvenHashes.Polymur.window_byte' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.prime : Fact (Nat.Prime ProvenHashes.Polymur.p)
'ProvenHashes.Polymur.prime' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.generator37_order : orderOf 37 = ProvenHashes.Polymur.p - 1
'ProvenHashes.Polymur.generator37_order' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.admissible37 : ProvenHashes.Polymur.Admissible 37
'ProvenHashes.Polymur.admissible37' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.keyCard_pos : 0 < ProvenHashes.Polymur.keyCard
'ProvenHashes.Polymur.keyCard_pos' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.keyNonempty : Nonempty ProvenHashes.Polymur.Key
'ProvenHashes.Polymur.keyNonempty' depends on axioms: [propext, Classical.choice, Quot.sound]
@ProvenHashes.Polymur.gaussSum_norm_sq : ∀ {q : ℕ} [inst : Fact (Nat.Prime q)] (χ : MulChar (ZMod q) ℂ),
  χ ≠ 1 → ‖gaussSum χ ZMod.stdAddChar‖ ^ 2 = ↑q
'ProvenHashes.Polymur.gaussSum_norm_sq' depends on axioms: [propext, Classical.choice, Quot.sound]
@ProvenHashes.Polymur.char_correlation_one : ∀ {q : ℕ} [inst : Fact (Nat.Prime q)] (χ : MulChar (ZMod q) ℂ),
  χ ≠ 1 → ∑ x, χ x * star (χ (x + 1)) = -1
'ProvenHashes.Polymur.char_correlation_one' depends on axioms: [propext, Classical.choice, Quot.sound]
@ProvenHashes.Polymur.char_correlation : ∀ {q : ℕ} [inst : Fact (Nat.Prime q)] (χ : MulChar (ZMod q) ℂ),
  χ ≠ 1 → ∀ (a b : ZMod q), ∑ x, χ (x + a) * star (χ (x + b)) = if a = b then ↑q - 1 else -1
'ProvenHashes.Polymur.char_correlation' depends on axioms: [propext, Classical.choice, Quot.sound]
@ProvenHashes.Polymur.char_weighted_energy : ∀ {q : ℕ} [inst : Fact (Nat.Prime q)] (χ : MulChar (ZMod q) ℂ),
  χ ≠ 1 → ∀ (g : ZMod q → ℂ), ∑ x, ‖∑ y, g y * χ (x + y)‖ ^ 2 = ↑q * ∑ y, ‖g y‖ ^ 2 - ‖∑ y, g y‖ ^ 2
'ProvenHashes.Polymur.char_weighted_energy' depends on axioms: [propext, Classical.choice, Quot.sound]
@ProvenHashes.Polymur.char_bilinear_sq : ∀ {q : ℕ} [inst : Fact (Nat.Prime q)] (χ : MulChar (ZMod q) ℂ),
  χ ≠ 1 →
    ∀ (f g : ZMod q → ℂ),
      ‖∑ x, f x * ∑ y, g y * χ (x + y)‖ ^ 2 ≤ (∑ x, ‖f x‖ ^ 2) * (↑q * ∑ y, ‖g y‖ ^ 2 - ‖∑ y, g y‖ ^ 2)
'ProvenHashes.Polymur.char_bilinear_sq' depends on axioms: [propext, Classical.choice, Quot.sound]
@ProvenHashes.Polymur.sum_residueInterval : ∀ {q : ℕ} [inst : Fact (Nat.Prime q)],
  ∀ n ≤ q, ∀ (f : ZMod q → ℂ), ∑ x ∈ ProvenHashes.Polymur.residueInterval q n, f x = ∑ i ∈ Finset.range n, f ↑i
'ProvenHashes.Polymur.sum_residueInterval' depends on axioms: [propext, Classical.choice, Quot.sound]
@ProvenHashes.Polymur.char_set_difference_sq : ∀ {q : ℕ} [Fact (Nat.Prime q)] (χ : MulChar (ZMod q) ℂ),
  χ ≠ 1 →
    ∀ (A B C : Finset (ZMod q)),
      ‖∑ x ∈ A, ∑ y ∈ C, χ (x + y) - ∑ x ∈ B, ∑ y ∈ C, χ (x + y)‖ ^ 2 ≤ (↑A.card + ↑B.card) * (↑q * ↑C.card)
'ProvenHashes.Polymur.char_set_difference_sq' depends on axioms: [propext, Classical.choice, Quot.sound]
@ProvenHashes.Polymur.card_residueInterval : ∀ {q : ℕ} [Fact (Nat.Prime q)],
  ∀ n ≤ q, (ProvenHashes.Polymur.residueInterval q n).card = n
'ProvenHashes.Polymur.card_residueInterval' depends on axioms: [propext, Classical.choice, Quot.sound]
@ProvenHashes.Polymur.intervalDouble_difference_sq : ∀ {q : ℕ} [inst : Fact (Nat.Prime q)] (χ : MulChar (ZMod q) ℂ),
  χ ≠ 1 →
    ∀ s ≤ q,
      ∀ (u v : ZMod q),
        ‖ProvenHashes.Polymur.intervalDouble χ s s u - ProvenHashes.Polymur.intervalDouble χ s s v‖ ^ 2 ≤
          2 * ↑q * ↑s ^ 2
'ProvenHashes.Polymur.intervalDouble_difference_sq' depends on axioms: [propext, Classical.choice, Quot.sound]
@ProvenHashes.Polymur.char_set_half_sq : ∀ {q : ℕ} [Fact (Nat.Prime q)] (χ : MulChar (ZMod q) ℂ),
  χ ≠ 1 → ∀ (A C : Finset (ZMod q)), q ≤ 2 * C.card → ‖∑ x ∈ A, ∑ y ∈ C, χ (x + y)‖ ^ 2 ≤ ↑q * ↑C.card ^ 2 / 4
'ProvenHashes.Polymur.char_set_half_sq' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.sum_range_shift_sub : ∀ (f : ℕ → ℂ) (a b : ℕ),
  ∑ i ∈ Finset.range a, f (b + i) - ∑ i ∈ Finset.range a, f i =
    ∑ i ∈ Finset.range b, f (a + i) - ∑ i ∈ Finset.range b, f i
'ProvenHashes.Polymur.sum_range_shift_sub' depends on axioms: [propext, Classical.choice, Quot.sound]
@ProvenHashes.Polymur.intervalDouble_shift_sub : ∀ {q : ℕ} [inst : Fact (Nat.Prime q)] (χ : MulChar (ZMod q) ℂ)
  (H s : ℕ) (t : ZMod q),
  ProvenHashes.Polymur.intervalDouble χ H s (t + ↑s) - ProvenHashes.Polymur.intervalDouble χ H s t =
    ProvenHashes.Polymur.intervalDouble χ s s (t + ↑H) - ProvenHashes.Polymur.intervalDouble χ s s t
'ProvenHashes.Polymur.intervalDouble_shift_sub' depends on axioms: [propext, Classical.choice, Quot.sound]
@ProvenHashes.Polymur.intervalDouble_two_mul : ∀ {q : ℕ} [inst : Fact (Nat.Prime q)] (χ : MulChar (ZMod q) ℂ) (H s : ℕ)
  (t : ZMod q),
  ProvenHashes.Polymur.intervalDouble χ H (2 * s) t =
    ProvenHashes.Polymur.intervalDouble χ H s t + ProvenHashes.Polymur.intervalDouble χ H s (t + ↑s)
'ProvenHashes.Polymur.intervalDouble_two_mul' depends on axioms: [propext, Classical.choice, Quot.sound]
@ProvenHashes.Polymur.intervalAverage_step : ∀ {q : ℕ} [inst : Fact (Nat.Prime q)] (χ : MulChar (ZMod q) ℂ),
  χ ≠ 1 →
    ∀ (H s : ℕ),
      0 < s →
        s ≤ q →
          ∀ (t : ZMod q) (B : ℝ),
            0 ≤ B →
              ↑q ≤ B ^ 2 →
                ‖ProvenHashes.Polymur.intervalAverage χ H (2 * s) t - ProvenHashes.Polymur.intervalAverage χ H s t‖ ≤
                  17 * B / 24
'ProvenHashes.Polymur.intervalAverage_step' depends on axioms: [propext, Classical.choice, Quot.sound]
@ProvenHashes.Polymur.intervalAverage_half : ∀ {q : ℕ} [inst : Fact (Nat.Prime q)] (χ : MulChar (ZMod q) ℂ),
  χ ≠ 1 →
    ∀ (H s : ℕ),
      H ≤ q →
        0 < s →
          s ≤ q →
            q ≤ 2 * s →
              ∀ (t : ZMod q) (B : ℝ), 0 ≤ B → ↑q ≤ B ^ 2 → ‖ProvenHashes.Polymur.intervalAverage χ H s t‖ ≤ B / 2
'ProvenHashes.Polymur.intervalAverage_half' depends on axioms: [propext, Classical.choice, Quot.sound]
@ProvenHashes.Polymur.intervalAverage_dyadic : ∀ {q : ℕ} [inst : Fact (Nat.Prime q)] (χ : MulChar (ZMod q) ℂ),
  χ ≠ 1 →
    ∀ (H j : ℕ),
      2 ^ j ≤ q →
        ∀ (t : ZMod q) (B : ℝ),
          0 ≤ B →
            ↑q ≤ B ^ 2 →
              ‖ProvenHashes.Polymur.intervalAverage χ H (2 ^ j) t - ProvenHashes.Polymur.intervalAverage χ H 1 t‖ ≤
                ↑j * 17 * B / 24
'ProvenHashes.Polymur.intervalAverage_dyadic' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.polymur_character_sum_bound : ∀ (χ : MulChar ProvenHashes.Polymur.F ℂ),
  χ ≠ 1 → ∀ H ≤ ProvenHashes.Polymur.p, ∀ (t : ProvenHashes.Polymur.F), ‖∑ i ∈ Finset.range H, χ (t + ↑i)‖ ≤ 65295510750
'ProvenHashes.Polymur.polymur_character_sum_bound' depends on axioms: [propext, Classical.choice, Quot.sound]
@ProvenHashes.Polymur.exists_faithful_character : ∀ {q : ℕ} [inst : Fact (Nat.Prime q)],
  ∃ χ, (Function.Injective fun u => χ ↑u) ∧ orderOf χ = q - 1
'ProvenHashes.Polymur.exists_faithful_character' depends on axioms: [propext, Classical.choice, Quot.sound]
@ProvenHashes.Polymur.faithful_character_pow_eq_one : ∀ {q : ℕ} [inst : Fact (Nat.Prime q)] (χ : MulChar (ZMod q) ℂ),
  (Function.Injective fun u => χ ↑u) → ∀ (x : ZMod q), x ≠ 0 → ∀ (e : ℕ), χ x ^ e = 1 ↔ x ^ e = 1
'ProvenHashes.Polymur.faithful_character_pow_eq_one' depends on axioms: [propext, Classical.choice, Quot.sound]
@ProvenHashes.Polymur.subgroup_character_indicator : ∀ {q : ℕ} [inst : Fact (Nat.Prime q)] (χ : MulChar (ZMod q) ℂ),
  (Function.Injective fun u => χ ↑u) →
    ∀ (e d : ℕ),
      e * d = q - 1 → ∀ (x : ZMod q), x ≠ 0 → ∑ j ∈ Finset.range d, (χ ^ (e * j)) x = if x ^ e = 1 then ↑d else 0
'ProvenHashes.Polymur.subgroup_character_indicator' depends on axioms: [propext, Classical.choice, Quot.sound]
@ProvenHashes.Polymur.subgroup_count_bound : ∀ {q : ℕ} [inst : Fact (Nat.Prime q)] (S : Finset (ZMod q)),
  (∀ x ∈ S, x ≠ 0) →
    ∀ (B : ℝ),
      0 ≤ B →
        (∀ (ψ : MulChar (ZMod q) ℂ), ψ ≠ 1 → ‖∑ x ∈ S, ψ x‖ ≤ B) →
          ∀ (e d : ℕ), 0 < e → 0 < d → e * d = q - 1 → |↑{x ∈ S | x ^ e = 1}.card - ↑S.card / ↑d| ≤ B
'ProvenHashes.Polymur.subgroup_count_bound' depends on axioms: [propext, Classical.choice, Quot.sound]
@ProvenHashes.Polymur.orderCount_sum_divisors : ∀ {q : ℕ} [inst : Fact (Nat.Prime q)] (S : Finset (ZMod q)) (n : ℕ),
  n ≠ 0 → ∑ d ∈ n.divisors, {x ∈ S | orderOf x = d}.card = {x ∈ S | x ^ n = 1}.card
'ProvenHashes.Polymur.orderCount_sum_divisors' depends on axioms: [propext, Classical.choice, Quot.sound]
@ProvenHashes.Polymur.orderCount_moebius : ∀ {q : ℕ} [inst : Fact (Nat.Prime q)] (S : Finset (ZMod q)) (m : ℕ),
  0 < m →
    ↑{x ∈ S | orderOf x = m}.card = ∑ d ∈ m.divisors, ↑(ArithmeticFunction.moebius d) * ↑{x ∈ S | x ^ (m / d) = 1}.card
'ProvenHashes.Polymur.orderCount_moebius' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.totient_moebius_real : ∀ (m : ℕ),
  0 < m → ↑m.totient = ∑ d ∈ m.divisors, ↑(ArithmeticFunction.moebius d) * ↑(m / d)
'ProvenHashes.Polymur.totient_moebius_real' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.sum_abs_moebius_real : ∀ (m : ℕ),
  m ≠ 0 → ∑ d ∈ m.divisors, |↑(ArithmeticFunction.moebius d)| = 2 ^ m.primeFactors.card
'ProvenHashes.Polymur.sum_abs_moebius_real' depends on axioms: [propext, Classical.choice, Quot.sound]
@ProvenHashes.Polymur.rootCount_error : ∀ {q : ℕ} [inst : Fact (Nat.Prime q)] (S : Finset (ZMod q)),
  (∀ x ∈ S, x ≠ 0) →
    ∀ (B : ℝ),
      0 ≤ B →
        (∀ (ψ : MulChar (ZMod q) ℂ), ψ ≠ 1 → ‖∑ x ∈ S, ψ x‖ ≤ B) →
          ∀ (e : ℕ), e ∣ q - 1 → |↑{x ∈ S | x ^ e = 1}.card - ↑S.card * ↑e / ↑(q - 1)| ≤ B
'ProvenHashes.Polymur.rootCount_error' depends on axioms: [propext, Classical.choice, Quot.sound]
@ProvenHashes.Polymur.orderCount_error : ∀ {q : ℕ} [inst : Fact (Nat.Prime q)] (S : Finset (ZMod q)),
  (∀ x ∈ S, x ≠ 0) →
    ∀ (B : ℝ),
      0 ≤ B →
        (∀ (ψ : MulChar (ZMod q) ℂ), ψ ≠ 1 → ‖∑ x ∈ S, ψ x‖ ≤ B) →
          ∀ (m : ℕ),
            m ∣ q - 1 → |↑{x ∈ S | orderOf x = m}.card - ↑S.card * ↑m.totient / ↑(q - 1)| ≤ 2 ^ m.primeFactors.card * B
'ProvenHashes.Polymur.orderCount_error' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.keyOrder_primeFactors : ProvenHashes.Polymur.keyOrder.primeFactors =
  {2, 3, 5, 11, 13, 31, 41, 61, 151, 331, 1321}
'ProvenHashes.Polymur.keyOrder_primeFactors' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.keyOrder_totient : ProvenHashes.Polymur.keyOrder.totient = 67744512000000000
'ProvenHashes.Polymur.keyOrder_totient' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.block_source : ∀ (w : ProvenHashes.Polymur.Words),
  ProvenHashes.Polymur.blockPoly w =
    (Polynomial.X + Polynomial.C (w 0)) * (Polynomial.X ^ 6 + Polynomial.C (w 1)) +
          (Polynomial.X ^ 2 + Polynomial.C (w 2)) * (Polynomial.X ^ 5 + Polynomial.C (w 3)) +
        (Polynomial.X ^ 3 + Polynomial.C (w 4)) * (Polynomial.X ^ 4 + Polynomial.C (w 5)) +
      Polynomial.C (w 6) * Polynomial.X ^ 7
'ProvenHashes.Polymur.block_source' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.short_source : ∀ (l : ProvenHashes.Polymur.F) (w : ProvenHashes.Polymur.Words),
  ProvenHashes.Polymur.shortPoly l w = (Polynomial.X + Polynomial.C (w 0)) * (Polynomial.X ^ 2 + Polynomial.C l)
'ProvenHashes.Polymur.short_source' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.mid_source : ∀ (l : ProvenHashes.Polymur.F) (w : ProvenHashes.Polymur.Words),
  ProvenHashes.Polymur.midPoly l w =
    (Polynomial.X ^ 2 + Polynomial.C (w 0)) * (Polynomial.X ^ 7 + Polynomial.C (w 1)) +
      (Polynomial.X + Polynomial.C (w 2)) * (Polynomial.X ^ 3 + Polynomial.C l)
'ProvenHashes.Polymur.mid_source' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.long_source : ∀ (l : ProvenHashes.Polymur.F) (w : ProvenHashes.Polymur.Words),
  ProvenHashes.Polymur.longPoly l w =
    (Polynomial.X + Polynomial.C (w 2)) * (Polynomial.X ^ 3 + Polynomial.C l) +
        (Polynomial.X ^ 2 + Polynomial.C (w 3)) * (Polynomial.X ^ 7 + Polynomial.C (w 4)) +
      ((Polynomial.X ^ 2 + Polynomial.C (w 0)) * (Polynomial.X ^ 7 + Polynomial.C (w 1)) + Polynomial.C (w 5)) *
        (Polynomial.X ^ 4 + Polynomial.C (w 6))
'ProvenHashes.Polymur.long_source' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.block_degree_le : ∀ (w : ProvenHashes.Polymur.Words),
  (ProvenHashes.Polymur.blockPoly w).natDegree ≤ 7
'ProvenHashes.Polymur.block_degree_le' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.short_degree_le : ∀ (l : ProvenHashes.Polymur.F) (w : ProvenHashes.Polymur.Words),
  (ProvenHashes.Polymur.shortPoly l w).natDegree ≤ 3
'ProvenHashes.Polymur.short_degree_le' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.mid_degree_le : ∀ (l : ProvenHashes.Polymur.F) (w : ProvenHashes.Polymur.Words),
  (ProvenHashes.Polymur.midPoly l w).natDegree ≤ 9
'ProvenHashes.Polymur.mid_degree_le' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.long_degree_le : ∀ (l : ProvenHashes.Polymur.F) (w : ProvenHashes.Polymur.Words),
  (ProvenHashes.Polymur.longPoly l w).natDegree ≤ 13
'ProvenHashes.Polymur.long_degree_le' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.short_difference_degree : ∀ (l l' : ProvenHashes.Polymur.F) (w v : ProvenHashes.Polymur.Words),
  (ProvenHashes.Polymur.shortPoly l w - ProvenHashes.Polymur.shortPoly l' v).natDegree ≤ 2
'ProvenHashes.Polymur.short_difference_degree' depends on axioms: [propext, Classical.choice, Quot.sound]
@ProvenHashes.Polymur.block_decode : ∀ {w v : ProvenHashes.Polymur.Words},
  (∀ (i : ℕ), 1 ≤ i → i ≤ 7 → (ProvenHashes.Polymur.blockPoly w).coeff i = (ProvenHashes.Polymur.blockPoly v).coeff i) →
    w = v
'ProvenHashes.Polymur.block_decode' depends on axioms: [propext, Classical.choice, Quot.sound]
@ProvenHashes.Polymur.long_decode : ∀ {l l' : ProvenHashes.Polymur.F} {w v : ProvenHashes.Polymur.Words},
  ProvenHashes.Polymur.longPoly l w = ProvenHashes.Polymur.longPoly l' v → w = v
'ProvenHashes.Polymur.long_decode' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.prefix_degree_le : ∀ (ws : List ProvenHashes.Polymur.Words),
  (ProvenHashes.Polymur.prefixPoly ws).natDegree ≤ 7 * ws.length
'ProvenHashes.Polymur.prefix_degree_le' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.prefix_high : ∀ (w : ProvenHashes.Polymur.Words) (ws : List ProvenHashes.Polymur.Words) (i : ℕ),
  1 ≤ i →
    (ProvenHashes.Polymur.prefixPoly (w :: ws)).coeff (i + 7 * ws.length) = (ProvenHashes.Polymur.blockPoly w).coeff i
'ProvenHashes.Polymur.prefix_high' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.prefix_degree : ∀ (ws : List ProvenHashes.Polymur.Words),
  (∀ w ∈ ws, w 6 + 3 ≠ 0) → (ProvenHashes.Polymur.prefixPoly ws).natDegree = 7 * ws.length
'ProvenHashes.Polymur.prefix_degree' depends on axioms: [propext, Classical.choice, Quot.sound]
@ProvenHashes.Polymur.prefix_inj_same_length : ∀ {a b : List ProvenHashes.Polymur.Words},
  a.length = b.length → ProvenHashes.Polymur.prefixPoly a = ProvenHashes.Polymur.prefixPoly b → a = b
'ProvenHashes.Polymur.prefix_inj_same_length' depends on axioms: [propext, Classical.choice, Quot.sound]
@ProvenHashes.Polymur.prefix_inj : ∀ {a b : List ProvenHashes.Polymur.Words},
  (∀ w ∈ a, w 6 + 3 ≠ 0) →
    (∀ w ∈ b, w 6 + 3 ≠ 0) → ProvenHashes.Polymur.prefixPoly a = ProvenHashes.Polymur.prefixPoly b → a = b
'ProvenHashes.Polymur.prefix_inj' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.prefix_fold : ∀ (ws : List ProvenHashes.Polymur.Words) (acc : Polynomial ProvenHashes.Polymur.F),
  List.foldl (fun h w => h * Polynomial.X ^ 7 + ProvenHashes.Polymur.blockPoly w) acc ws =
    acc * Polynomial.X ^ (7 * ws.length) + ProvenHashes.Polymur.prefixPoly ws
'ProvenHashes.Polymur.prefix_fold' depends on axioms: [propext, Classical.choice, Quot.sound]
@ProvenHashes.Polymur.separate_prefix_tail : ∀ {a b t u : Polynomial ProvenHashes.Polymur.F},
  t.natDegree ≤ 13 → u.natDegree ≤ 13 → a * Polynomial.X ^ 14 + t = b * Polynomial.X ^ 14 + u → a = b ∧ t = u
'ProvenHashes.Polymur.separate_prefix_tail' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.tail_length_coeff : ∀ (m : ProvenHashes.Polymur.Bytes),
  (ProvenHashes.Polymur.tailPoly m).coeff 1 = ↑(List.length m)
'ProvenHashes.Polymur.tail_length_coeff' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.tail_degree_le : ∀ (m : ProvenHashes.Polymur.Bytes),
  (ProvenHashes.Polymur.tailPoly m).natDegree ≤ 13
'ProvenHashes.Polymur.tail_degree_le' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.tail_small_degree : ∀ (m : ProvenHashes.Polymur.Bytes),
  List.length m ≤ 21 → (ProvenHashes.Polymur.tailPoly m).natDegree ≤ 9
'ProvenHashes.Polymur.tail_small_degree' depends on axioms: [propext, Classical.choice, Quot.sound]
@ProvenHashes.Polymur.tail_inj : ∀ {a b : ProvenHashes.Polymur.Bytes},
  List.length a ≤ 49 → List.length b ≤ 49 → ProvenHashes.Polymur.tailPoly a = ProvenHashes.Polymur.tailPoly b → a = b
'ProvenHashes.Polymur.tail_inj' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.tail_length_le : ∀ (m : ProvenHashes.Polymur.Bytes), List.length (ProvenHashes.Polymur.tail m) ≤ 49
'ProvenHashes.Polymur.tail_length_le' depends on axioms: [propext, Quot.sound]
ProvenHashes.Polymur.message_length : ∀ (m : ProvenHashes.Polymur.Bytes),
  List.length m = 49 * ProvenHashes.Polymur.blockCount m + List.length (ProvenHashes.Polymur.tail m)
'ProvenHashes.Polymur.message_length' depends on axioms: [propext, Quot.sound]
ProvenHashes.Polymur.blocks_good : ∀ (m : ProvenHashes.Polymur.Bytes), ∀ w ∈ ProvenHashes.Polymur.blocks m, w 6 + 3 ≠ 0
'ProvenHashes.Polymur.blocks_good' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.blocks_length : ∀ (m : ProvenHashes.Polymur.Bytes),
  (ProvenHashes.Polymur.blocks m).length = ProvenHashes.Polymur.blockCount m
'ProvenHashes.Polymur.blocks_length' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.encode_injective : Function.Injective ProvenHashes.Polymur.encode
'ProvenHashes.Polymur.encode_injective' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.coefficients_injective : Function.Injective ProvenHashes.Polymur.coefficients
'ProvenHashes.Polymur.coefficients_injective' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.encode_degree_le : ∀ (m : ProvenHashes.Polymur.Bytes),
  (ProvenHashes.Polymur.encode m).natDegree ≤ 7 * ProvenHashes.Polymur.blockCount m + 14
'ProvenHashes.Polymur.encode_degree_le' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.encode_short : ∀ (m : ProvenHashes.Polymur.Bytes),
  List.length m ≤ 49 → ProvenHashes.Polymur.encode m = ProvenHashes.Polymur.tailPoly m
'ProvenHashes.Polymur.encode_short' depends on axioms: [propext, Classical.choice, Quot.sound]
@ProvenHashes.Polymur.difference_degree : ∀ {n : ℕ} (a b : ProvenHashes.Polymur.Bytes),
  List.length a ≤ n →
    List.length b ≤ n →
      (ProvenHashes.Polymur.encode a - ProvenHashes.Polymur.encode b).natDegree ≤ ProvenHashes.Polymur.D n
'ProvenHashes.Polymur.difference_degree' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.xorShift32_inverse : ∀ (x : ProvenHashes.Polymur.Word),
  ProvenHashes.Polymur.xorShift 32 (ProvenHashes.Polymur.xorShift 32 x) = x
'ProvenHashes.Polymur.xorShift32_inverse' depends on axioms: [propext, Quot.sound]
ProvenHashes.Polymur.xorShift28_inverse : ∀ (x : ProvenHashes.Polymur.Word),
  ProvenHashes.Polymur.xorShift 28 (ProvenHashes.Polymur.xorShift 28 x) ^^^ ProvenHashes.Polymur.xorShift 28 x >>> 56 =
    x
'ProvenHashes.Polymur.xorShift28_inverse' depends on axioms: [propext, Quot.sound]
ProvenHashes.Polymur.xorShift32_injective : Function.Injective (ProvenHashes.Polymur.xorShift 32)
'ProvenHashes.Polymur.xorShift32_injective' depends on axioms: [propext, Quot.sound]
ProvenHashes.Polymur.xorShift28_injective : Function.Injective (ProvenHashes.Polymur.xorShift 28)
'ProvenHashes.Polymur.xorShift28_injective' depends on axioms: [propext, Quot.sound]
ProvenHashes.Polymur.mix_mul_injective : Function.Injective fun x => x * 1051668233026429277
'ProvenHashes.Polymur.mix_mul_injective' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.mix_injective : Function.Injective ProvenHashes.Polymur.mix
'ProvenHashes.Polymur.mix_injective' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.word_add_injective : ∀ (s : ProvenHashes.Polymur.Word), Function.Injective fun x => x + s
'ProvenHashes.Polymur.word_add_injective' depends on axioms: [propext]
ProvenHashes.Polymur.finish_injective : ∀ (s : ProvenHashes.Polymur.Word),
  Function.Injective (ProvenHashes.Polymur.finish s)
'ProvenHashes.Polymur.finish_injective' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.finish_bijective : ∀ (s : ProvenHashes.Polymur.Word),
  Function.Bijective (ProvenHashes.Polymur.finish s)
'ProvenHashes.Polymur.finish_bijective' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.finish_tweak_eq_iff : ∀ (s tweak x y : ProvenHashes.Polymur.Word),
  ProvenHashes.Polymur.finish s (x + tweak) = ProvenHashes.Polymur.finish s (y + tweak) ↔ x = y
'ProvenHashes.Polymur.finish_tweak_eq_iff' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.key_root_count : ∀ (q : Polynomial ProvenHashes.Polymur.F),
  q ≠ 0 → {k | Polynomial.eval (↑k) q = 0}.card ≤ q.natDegree
'ProvenHashes.Polymur.key_root_count' depends on axioms: [propext, Classical.choice, Quot.sound]
@ProvenHashes.Polymur.core_collision_bound : ∀ {n : ℕ} (a b : ProvenHashes.Polymur.Bytes),
  List.length a ≤ n →
    List.length b ≤ n →
      a ≠ b →
        (ProvenHashes.uniformProb fun k => ProvenHashes.Polymur.core (↑k) a = ProvenHashes.Polymur.core (↑k) b) ≤
          ↑(ProvenHashes.Polymur.D n) / ↑ProvenHashes.Polymur.keyCard
'ProvenHashes.Polymur.core_collision_bound' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.coreWord_eq_implies : ∀ (k : ProvenHashes.Polymur.F) (a b : ProvenHashes.Polymur.Bytes),
  ProvenHashes.Polymur.coreWord k a = ProvenHashes.Polymur.coreWord k b →
    ProvenHashes.Polymur.core k a = ProvenHashes.Polymur.core k b
'ProvenHashes.Polymur.coreWord_eq_implies' depends on axioms: [propext, Classical.choice, Quot.sound]
@ProvenHashes.Polymur.idealHash_collision_bound : ∀ {n : ℕ} (a b : ProvenHashes.Polymur.Bytes),
  List.length a ≤ n →
    List.length b ≤ n →
      a ≠ b →
        ∀ (s : ProvenHashes.Polymur.Key → ProvenHashes.Polymur.Word) (tweak : ProvenHashes.Polymur.Word),
          (ProvenHashes.uniformProb fun k =>
              ProvenHashes.Polymur.idealHash (↑k) (s k) tweak a = ProvenHashes.Polymur.idealHash (↑k) (s k) tweak b) ≤
            ↑(ProvenHashes.Polymur.D n) / ↑ProvenHashes.Polymur.keyCard
'ProvenHashes.Polymur.idealHash_collision_bound' depends on axioms: [propext, Classical.choice, Quot.sound]
@ProvenHashes.Polymur.representative_collision_bound : ∀ {n : ℕ} (a b : ProvenHashes.Polymur.Bytes),
  List.length a ≤ n →
    List.length b ≤ n →
      a ≠ b →
        ∀ (rep : ProvenHashes.Polymur.Key → ProvenHashes.Polymur.Bytes → ProvenHashes.Polymur.Word),
          (∀ (k : ProvenHashes.Polymur.Key) (m : ProvenHashes.Polymur.Bytes),
              ↑(BitVec.toNat (rep k m)) = ProvenHashes.Polymur.core (↑k) m) →
            ∀ (s : ProvenHashes.Polymur.Key → ProvenHashes.Polymur.Word) (tweak : ProvenHashes.Polymur.Word),
              (ProvenHashes.uniformProb fun k =>
                  ProvenHashes.Polymur.finish (s k) (rep k a + tweak) =
                    ProvenHashes.Polymur.finish (s k) (rep k b + tweak)) ≤
                ↑(ProvenHashes.Polymur.D n) / ↑ProvenHashes.Polymur.keyCard
'ProvenHashes.Polymur.representative_collision_bound' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.core_collision_bound_K0 : ProvenHashes.Polymur.CardinalityCertificate →
  ∀ {n : ℕ} (a b : ProvenHashes.Polymur.Bytes),
    List.length a ≤ n →
      List.length b ≤ n →
        a ≠ b →
          (ProvenHashes.uniformProb fun k => ProvenHashes.Polymur.core (↑k) a = ProvenHashes.Polymur.core (↑k) b) ≤
            ↑(ProvenHashes.Polymur.D n) / ↑ProvenHashes.Polymur.K0
'ProvenHashes.Polymur.core_collision_bound_K0' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.idealHash_collision_bound_K0 : ProvenHashes.Polymur.CardinalityCertificate →
  ∀ {n : ℕ} (a b : ProvenHashes.Polymur.Bytes),
    List.length a ≤ n →
      List.length b ≤ n →
        a ≠ b →
          ∀ (s : ProvenHashes.Polymur.Key → ProvenHashes.Polymur.Word) (tweak : ProvenHashes.Polymur.Word),
            (ProvenHashes.uniformProb fun k =>
                ProvenHashes.Polymur.idealHash (↑k) (s k) tweak a = ProvenHashes.Polymur.idealHash (↑k) (s k) tweak b) ≤
              ↑(ProvenHashes.Polymur.D n) / ↑ProvenHashes.Polymur.K0
'ProvenHashes.Polymur.idealHash_collision_bound_K0' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.keyOrder_factor_count : ProvenHashes.Polymur.keyOrder.primeFactors.card = 11
'ProvenHashes.Polymur.keyOrder_factor_count' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.card_keyInterval : ProvenHashes.Polymur.keyInterval.card = ProvenHashes.Polymur.keyIntervalLength
'ProvenHashes.Polymur.card_keyInterval' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.keyInterval_bounds : ∀ x ∈ ProvenHashes.Polymur.keyInterval, x ≠ 0 ∧ ZMod.val x < 2 ^ 60 - 2 ^ 56
'ProvenHashes.Polymur.keyInterval_bounds' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.keyTargets_count_error : |↑ProvenHashes.Polymur.keyTargets.card -
      ↑ProvenHashes.Polymur.keyIntervalLength * 67744512000000000 / ↑(ProvenHashes.Polymur.p - 1)| ≤
  2048 * 65295510750
'ProvenHashes.Polymur.keyTargets_count_error' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.seventhRoot_order : orderOf ProvenHashes.Polymur.seventhRoot = 7
'ProvenHashes.Polymur.seventhRoot_order' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.keyLiftValue_pow_seven : ∀ (x : ProvenHashes.Polymur.F),
  orderOf x = ProvenHashes.Polymur.keyOrder → ∀ (i : Fin 6), ProvenHashes.Polymur.keyLiftValue x i ^ 7 = x
'ProvenHashes.Polymur.keyLiftValue_pow_seven' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.keyLiftValue_order : ∀ (x : ProvenHashes.Polymur.F),
  orderOf x = ProvenHashes.Polymur.keyOrder →
    ∀ (i : Fin 6), orderOf (ProvenHashes.Polymur.keyLiftValue x i) = ProvenHashes.Polymur.p - 1
'ProvenHashes.Polymur.keyLiftValue_order' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.keyLiftValue_admissible : ∀ x ∈ ProvenHashes.Polymur.keyTargets,
  ∀ (i : Fin 6), ProvenHashes.Polymur.Admissible (ProvenHashes.Polymur.keyLiftValue x i)
'ProvenHashes.Polymur.keyLiftValue_admissible' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.keyLift_injective : Function.Injective ProvenHashes.Polymur.keyLift
'ProvenHashes.Polymur.keyLift_injective' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.six_mul_keyTargets_le_keyCard : 6 * ProvenHashes.Polymur.keyTargets.card ≤
  ProvenHashes.Polymur.keyCard
'ProvenHashes.Polymur.six_mul_keyTargets_le_keyCard' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.keyCard_analytic_lower : 6 * ↑ProvenHashes.Polymur.keyIntervalLength * 67744512000000000 /
      ↑(ProvenHashes.Polymur.p - 1) -
    6 * 2048 * 65295510750 ≤
  ↑ProvenHashes.Polymur.keyCard
'ProvenHashes.Polymur.keyCard_analytic_lower' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.cardinalityCertificate : ProvenHashes.Polymur.CardinalityCertificate
'ProvenHashes.Polymur.cardinalityCertificate' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.D_pos : ∀ (n : ℕ), 0 < ProvenHashes.Polymur.D n
'ProvenHashes.Polymur.D_pos' depends on axioms: [propext, Quot.sound]
ProvenHashes.Polymur.D_eight : ProvenHashes.Polymur.D 8 = 9
'ProvenHashes.Polymur.D_eight' does not depend on any axioms
ProvenHashes.Polymur.D_eight_mul_le : ∀ (L : ℕ), 2 ≤ L → ProvenHashes.Polymur.D (8 * L) ≤ 9 * L
'ProvenHashes.Polymur.D_eight_mul_le' depends on axioms: [propext, Quot.sound]
ProvenHashes.Polymur.D_word_cap : ∀ (L : ℕ), 1 ≤ L → ProvenHashes.Polymur.D (8 * L) ≤ 9 * L
'ProvenHashes.Polymur.D_word_cap' depends on axioms: [propext, Quot.sound]
ProvenHashes.Polymur.core_collision_bound_words : ∀ (L : ℕ) (a b : ProvenHashes.Polymur.Bytes),
  List.length a ≤ 8 * L →
    List.length b ≤ 8 * L →
      a ≠ b →
        ↑(ProvenHashes.uniformProb fun k => ProvenHashes.Polymur.core (↑k) a = ProvenHashes.Polymur.core (↑k) b) ≤
          ProvenHashes.Polymur.epsilon L
'ProvenHashes.Polymur.core_collision_bound_words' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.epsilon_pos : ∀ (L : ℕ), 0 < ProvenHashes.Polymur.epsilon L
'ProvenHashes.Polymur.epsilon_pos' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.wordScore_lower : ∀ (L : ℕ),
  1 ≤ L → Real.logb 2 (↑ProvenHashes.Polymur.keyCard / 9) ≤ ProvenHashes.Polymur.wordScore L
'ProvenHashes.Polymur.wordScore_lower' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.wordScore_one : ProvenHashes.Polymur.wordScore 1 = Real.logb 2 (↑ProvenHashes.Polymur.keyCard / 9)
'ProvenHashes.Polymur.wordScore_one' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.score_eq : ProvenHashes.Polymur.score = Real.logb 2 (↑ProvenHashes.Polymur.keyCard / 9)
'ProvenHashes.Polymur.score_eq' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.exp_fraction_bound : Real.exp (15715 / 100000) ≤ ↑ProvenHashes.Polymur.K0 / (9 * 2 ^ 54)
'ProvenHashes.Polymur.exp_fraction_bound' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.K0_log_lower : 54.2267 ≤ Real.logb 2 (↑ProvenHashes.Polymur.K0 / 9)
'ProvenHashes.Polymur.K0_log_lower' depends on axioms: [propext, Classical.choice, Quot.sound]
ProvenHashes.Polymur.score_lower : ProvenHashes.Polymur.CardinalityCertificate → 54.2267 ≤ ProvenHashes.Polymur.score
'ProvenHashes.Polymur.score_lower' depends on axioms: [propext, Classical.choice, Quot.sound]
@ProvenHashes.Polymur.core_collision_bound_K0_unconditional : ∀ {n : ℕ} (a b : ProvenHashes.Polymur.Bytes),
  List.length a ≤ n →
    List.length b ≤ n →
      a ≠ b →
        (ProvenHashes.uniformProb fun k => ProvenHashes.Polymur.core (↑k) a = ProvenHashes.Polymur.core (↑k) b) ≤
          ↑(ProvenHashes.Polymur.D n) / ↑ProvenHashes.Polymur.K0
'ProvenHashes.Polymur.core_collision_bound_K0_unconditional' depends on axioms: [propext, Classical.choice, Quot.sound]
@ProvenHashes.Polymur.idealHash_collision_bound_K0_unconditional : ∀ {n : ℕ} (a b : ProvenHashes.Polymur.Bytes),
  List.length a ≤ n →
    List.length b ≤ n →
      a ≠ b →
        ∀ (s : ProvenHashes.Polymur.Key → ProvenHashes.Polymur.Word) (tweak : ProvenHashes.Polymur.Word),
          (ProvenHashes.uniformProb fun k =>
              ProvenHashes.Polymur.idealHash (↑k) (s k) tweak a = ProvenHashes.Polymur.idealHash (↑k) (s k) tweak b) ≤
            ↑(ProvenHashes.Polymur.D n) / ↑ProvenHashes.Polymur.K0
'ProvenHashes.Polymur.idealHash_collision_bound_K0_unconditional' depends on axioms: [propext,
 Classical.choice,
 Quot.sound]
ProvenHashes.Polymur.score_lower_unconditional : 54.2267 ≤ ProvenHashes.Polymur.score
'ProvenHashes.Polymur.score_lower_unconditional' depends on axioms: [propext, Classical.choice, Quot.sound]
```
