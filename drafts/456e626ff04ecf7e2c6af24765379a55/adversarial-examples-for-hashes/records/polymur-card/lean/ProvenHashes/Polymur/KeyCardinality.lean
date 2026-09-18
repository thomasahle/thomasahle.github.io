import ProvenHashes.Polymur.KeyCount
import ProvenHashes.Polymur.Cardinality

noncomputable section
open Finset
namespace ProvenHashes.Polymur

lemma keyOrder_factor_count : keyOrder.primeFactors.card = 11 := by
  rw [keyOrder_primeFactors]
  decide

/-- The exact nonzero interval used for canonical seventh powers. -/
def keyInterval : Finset F :=
  (residueInterval p keyIntervalLength).map (Equiv.addLeft (1:F)).toEmbedding

lemma card_keyInterval : keyInterval.card = keyIntervalLength := by
  rw [keyInterval, Finset.card_map,
    card_residueInterval keyIntervalLength (by norm_num [keyIntervalLength, p])]

lemma keyInterval_bounds (x : F) (hx : x ∈ keyInterval) :
    x ≠ 0 ∧ x.val < 2^60-2^56 := by
  obtain ⟨y, hy, rfl⟩ := Finset.mem_map.mp hx
  obtain ⟨i, hi, rfl⟩ := Finset.mem_image.mp hy
  have hi' := Finset.mem_range.mp hi
  change (1 + (i:F)) ≠ 0 ∧ (1 + (i:F)).val < 2^60-2^56
  have hip : 1+i < p := by norm_num [keyIntervalLength, p] at hi' ⊢; omega
  have hv : (((1+i:ℕ):F)).val = 1+i := by
    rw [ZMod.val_natCast, Nat.mod_eq_of_lt hip]
  simp only [Nat.cast_add, Nat.cast_one] at hv
  constructor
  · intro hz
    have h := congrArg ZMod.val hz
    rw [hv, ZMod.val_zero] at h
    omega
  · rw [hv]
    norm_num [keyIntervalLength] at hi' ⊢
    omega

/-- Exact-order elements in the acceptance interval, before taking seventh roots. -/
def keyTargets : Finset F := keyInterval.filter (fun x => orderOf x = keyOrder)

theorem keyTargets_count_error :
    |(keyTargets.card : ℝ) - (keyIntervalLength : ℝ) * 67744512000000000 /
      ((p-1 : ℕ) : ℝ)| ≤ 2048 * 65295510750 := by
  have hb (χ : MulChar F ℂ) (hχ : χ ≠ 1) :
      ‖∑ x ∈ keyInterval, χ x‖ ≤ (65295510750 : ℝ) := by
    simp only [keyInterval, Finset.sum_map, Equiv.toEmbedding_apply, Equiv.coe_addLeft]
    rw [sum_residueInterval keyIntervalLength (by norm_num [keyIntervalLength, p])]
    exact polymur_character_sum_bound χ hχ keyIntervalLength
      (by norm_num [keyIntervalLength, p]) 1
  have h := orderCount_error keyInterval (fun x hx => (keyInterval_bounds x hx).1)
    65295510750 (by norm_num) hb keyOrder (by norm_num [keyOrder, p])
  simpa only [keyTargets, card_keyInterval, keyOrder_totient, keyOrder_factor_count,
    Nat.cast_ofNat, show (2:ℝ)^11 = 2048 by norm_num] using h

/-- A fixed primitive seventh root of unity. -/
def seventhRoot : F := (37:F)^keyOrder

lemma seventhRoot_order : orderOf seventhRoot = 7 := by
  rw [seventhRoot, orderOf_pow' _ (by norm_num [keyOrder, p]), generator37_order]
  norm_num [keyOrder, p]

/-- Invert the seventh power on the order-`keyOrder` subgroup, then choose one
of the six nontrivial seventh roots of unity. -/
def keyLiftValue (x : F) (i : Fin 6) : F :=
  x ^ 188232082384791343 * seventhRoot ^ (i.val + 1)

lemma keyLiftValue_pow_seven (x : F) (hx : orderOf x = keyOrder) (i : Fin 6) :
    keyLiftValue x i ^ 7 = x := by
  have hxpow : x ^ keyOrder = 1 := by rw [← hx, pow_orderOf_eq_one]
  have hzpow : seventhRoot ^ 7 = 1 := by rw [← seventhRoot_order, pow_orderOf_eq_one]
  rw [keyLiftValue, mul_pow, pow_right_comm seventhRoot, hzpow, one_pow,
    mul_one, ← pow_mul]
  rw [show 188232082384791343 * 7 = keyOrder * 4 + 1 by norm_num [keyOrder, p],
    pow_succ, pow_mul, hxpow, one_pow, one_mul]

lemma keyLiftValue_order (x : F) (hx : orderOf x = keyOrder) (i : Fin 6) :
    orderOf (keyLiftValue x i) = p-1 := by
  have hx' : orderOf (x ^ 188232082384791343) = keyOrder := by
    rw [orderOf_pow' _ (by decide), hx]
    norm_num [keyOrder, p]
  have hi' : orderOf (seventhRoot ^ (i.val + 1)) = 7 := by
    rw [orderOf_pow' _ (Nat.succ_ne_zero _), seventhRoot_order]
    fin_cases i <;> norm_num
  change orderOf (x ^ 188232082384791343 * seventhRoot ^ (i.val + 1)) = p-1
  rw [(Commute.all _ _).orderOf_mul_eq_mul_orderOf_of_coprime
    (by rw [hx', hi']; norm_num [keyOrder, p, Nat.Coprime]), hx', hi']
  norm_num [keyOrder, p]

lemma keyLiftValue_admissible (x : F) (hx : x ∈ keyTargets) (i : Fin 6) :
    Admissible (keyLiftValue x i) := by
  obtain ⟨hxI, hxo⟩ := Finset.mem_filter.mp hx
  obtain ⟨hx0, hxv⟩ := keyInterval_bounds x hxI
  refine ⟨?_, keyLiftValue_order x hxo i, ?_⟩
  · intro hz
    have h := keyLiftValue_pow_seven x hxo i
    rw [hz, zero_pow (by decide)] at h
    exact hx0 h.symm
  · rw [keyLiftValue_pow_seven x hxo i]
    exact hxv

def keyLift (xi : keyTargets × Fin 6) : Key :=
  ⟨keyLiftValue xi.1 xi.2, keyLiftValue_admissible xi.1 xi.1.property xi.2⟩

lemma keyLift_injective : Function.Injective keyLift := by
  rintro ⟨⟨x, hx⟩, i⟩ ⟨⟨y, hy⟩, j⟩ hij
  have hv := congrArg (fun k : Key => k.val) hij
  simp only [keyLift] at hv
  have hxy : x = y := by
    have h := congrArg (fun z : F => z^7) hv
    dsimp only at h
    rwa [keyLiftValue_pow_seven x (Finset.mem_filter.mp hx).2 i,
      keyLiftValue_pow_seven y (Finset.mem_filter.mp hy).2 j] at h
  subst y
  have hx0 := (keyInterval_bounds x (Finset.mem_filter.mp hx).1).1
  simp only [keyLiftValue] at hv
  have hp : seventhRoot ^ (i.val + 1) = seventhRoot ^ (j.val + 1) :=
    mul_left_cancel₀ (pow_ne_zero _ hx0) hv
  have hi : i.val + 1 < orderOf seventhRoot := by rw [seventhRoot_order]; omega
  have hj : j.val + 1 < orderOf seventhRoot := by rw [seventhRoot_order]; omega
  have he : i.val + 1 = j.val + 1 := pow_injOn_Iio_orderOf hi hj hp
  have hij' : i = j := Fin.ext (by omega)
  subst j
  rfl

/-- Every counted order-`keyOrder` element yields six distinct accepted keys. -/
theorem six_mul_keyTargets_le_keyCard : 6 * keyTargets.card ≤ keyCard := by
  have h := Fintype.card_le_of_injective keyLift keyLift_injective
  simpa only [Fintype.card_prod, Fintype.card_coe, Fintype.card_fin,
    Nat.mul_comm, keyCard] using h

/-- The review's real-valued lower bound, now for the exact Lean key set. -/
theorem keyCard_analytic_lower :
    (6:ℝ) * keyIntervalLength * 67744512000000000 / ((p-1 : ℕ) : ℝ) -
      6 * 2048 * 65295510750 ≤ (keyCard : ℝ) := by
  have herror := (abs_le.mp keyTargets_count_error).1
  have hcount : (6:ℝ) * keyTargets.card ≤ (keyCard : ℝ) := by
    exact_mod_cast six_mul_keyTargets_le_keyCard
  linarith

/-- The key-cardinality certificate, with no unproved character-sum hypothesis. -/
theorem cardinalityCertificate : CardinalityCertificate := by
  have hnum : (K0 : ℝ) ≤
      (6:ℝ) * keyIntervalLength * 67744512000000000 / ((p-1 : ℕ) : ℝ) -
        6 * 2048 * 65295510750 := by
    norm_num [K0, keyIntervalLength, p]
  have h := hnum.trans keyCard_analytic_lower
  change K0 ≤ keyCard
  exact_mod_cast h

end ProvenHashes.Polymur
