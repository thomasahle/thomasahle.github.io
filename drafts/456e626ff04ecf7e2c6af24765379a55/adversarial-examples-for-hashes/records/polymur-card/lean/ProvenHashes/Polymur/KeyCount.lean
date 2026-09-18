import ProvenHashes.Polymur.CharacterSum
import Mathlib.NumberTheory.ArithmeticFunction
import Mathlib.NumberTheory.MulChar.Duality

noncomputable section
open Finset
namespace ProvenHashes.Polymur

variable {q : ℕ} [Fact q.Prime]
local notation "Q" => ZMod q

/-- A faithful complex character of the cyclic unit group. -/
theorem exists_faithful_character : ∃ χ : MulChar Q ℂ,
    Function.Injective (fun u : Qˣ => χ u) ∧ orderOf χ = q-1 := by
  have hn : q-1 ≠ 0 := by have := (Fact.out : q.Prime).two_le; omega
  letI : NeZero (q-1) := ⟨hn⟩
  let e : Qˣ ≃* rootsOfUnity (q-1) ℂ := mulEquivOfCyclicCardEq (by
    simp only [Nat.card_eq_fintype_card, Fintype.card_units, ZMod.card,
      Complex.card_rootsOfUnity])
  let χ : MulChar Q ℂ := MulChar.ofUnitHom ((rootsOfUnity (q-1) ℂ).subtype.comp e.toMonoidHom)
  have hi : Function.Injective (fun u : Qˣ => χ u) := by
    intro u v h
    apply e.injective
    apply Subtype.ext
    apply Units.ext
    simpa only [χ, MulChar.ofUnitHom_coe, MonoidHom.coe_comp, Function.comp_apply,
      Subgroup.coe_subtype, MulEquiv.coe_toMonoidHom] using h
  refine ⟨χ, hi, Nat.dvd_antisymm ?_ ?_⟩
  · simpa only [ZMod.card] using χ.orderOf_dvd_card_sub_one
  · obtain ⟨g, hg⟩ := IsCyclic.exists_generator (α := Qˣ)
    have hp : g ^ orderOf χ = 1 := by
      apply hi
      change χ ((g ^ orderOf χ : Qˣ) : Q) = χ (1 : Qˣ)
      rw [Units.val_pow_eq_pow_val, map_pow, ← MulChar.pow_apply_coe,
        pow_orderOf_eq_one, MulChar.one_apply_coe]
      simp
    have hd := orderOf_dvd_of_pow_eq_one hp
    simpa only [orderOf_eq_card_of_forall_mem_zpowers hg, Nat.card_eq_fintype_card,
      Fintype.card_units, ZMod.card] using hd

lemma faithful_character_pow_eq_one (χ : MulChar Q ℂ)
    (hi : Function.Injective (fun u : Qˣ => χ u)) (x : Q) (hx : x ≠ 0) (e : ℕ) :
    χ x ^ e = 1 ↔ x ^ e = 1 := by
  let u : Qˣ := Units.mk0 x hx
  constructor
  · intro h
    have hu : u ^ e = 1 := by
      apply hi
      change χ (x ^ e) = χ 1
      rw [map_pow, h, map_one]
    exact congrArg (fun a : Qˣ => (a : Q)) hu
  · intro h
    rw [← map_pow, h, map_one]

/-- The subgroup indicator is a finite geometric sum of multiplicative characters. -/
theorem subgroup_character_indicator (χ : MulChar Q ℂ)
    (hi : Function.Injective (fun u : Qˣ => χ u))
    (e d : ℕ) (hed : e*d = q-1) (x : Q) (hx : x ≠ 0) :
    (∑ j ∈ Finset.range d, (χ^(e*j)) x) = if x^e = 1 then (d:ℂ) else 0 := by
  let u : Qˣ := Units.mk0 x hx
  have heval (j : ℕ) : (χ^(e*j)) x = (χ x ^ e)^j := by
    change (χ^(e*j)) (u : Q) = (χ (u : Q)^e)^j
    rw [MulChar.pow_apply_coe, pow_mul]
  simp_rw [heval]
  by_cases hxe : x^e = 1
  · rw [if_pos hxe, (faithful_character_pow_eq_one χ hi x hx e).mpr hxe]
    simp
  · rw [if_neg hxe]
    have hz : χ x ^ e ≠ 1 := fun h => hxe ((faithful_character_pow_eq_one χ hi x hx e).mp h)
    rw [geom_sum_eq hz, ← pow_mul, hed]
    have hp : χ x ^ (q-1) = 1 := by
      rw [← map_pow, ZMod.pow_card_sub_one_eq_one hx, map_one]
    rw [hp, sub_self, zero_div]

/-- The interval count in an arbitrary multiplicative subgroup inherits the character bound. -/
theorem subgroup_count_bound (S : Finset Q) (hS : ∀ x ∈ S, x ≠ 0)
    (B : ℝ) (hB : 0 ≤ B)
    (hbound : ∀ ψ : MulChar Q ℂ, ψ ≠ 1 → ‖∑ x ∈ S, ψ x‖ ≤ B)
    (e d : ℕ) (he : 0 < e) (hd : 0 < d) (hed : e*d = q-1) :
    |((S.filter (fun x => x^e = 1)).card : ℝ) - (S.card:ℝ)/(d:ℝ)| ≤ B := by
  classical
  obtain ⟨χ, hi, ho⟩ := exists_faithful_character (q := q)
  let P := (S.filter (fun x => x^e = 1)).card
  have hsum : (∑ j ∈ Finset.range d, ∑ x ∈ S, (χ^(e*j)) x) = (d:ℂ)*(P:ℂ) := by
    rw [Finset.sum_comm]
    have hh : (∑ x ∈ S, ∑ j ∈ Finset.range d, (χ^(e*j)) x) =
        ∑ x ∈ S, if x^e = 1 then (d:ℂ) else 0 := by
      apply Finset.sum_congr rfl
      intro x hx
      exact subgroup_character_indicator χ hi e d hed x (hS x hx)
    rw [hh, ← Finset.sum_filter]
    simp [P, mul_comm]
  have hzero : (∑ x ∈ S, (χ^(e*0)) x) = (S.card:ℂ) := by
    simp only [Nat.mul_zero, pow_zero]
    trans ∑ _ ∈ S, (1:ℂ)
    · apply Finset.sum_congr rfl
      intro x hx
      exact MulChar.one_apply (isUnit_iff_ne_zero.mpr (hS x hx))
    · simp
  have ht := Finset.sum_range_succ' (fun j => ∑ x ∈ S, (χ^(e*j)) x) (d-1)
  rw [Nat.sub_add_cancel (Nat.one_le_iff_ne_zero.mpr hd.ne'), hzero, hsum] at ht
  have hn : ‖(d:ℂ)*(P:ℂ)-(S.card:ℂ)‖ ≤ (d:ℝ)*B := by
    have hs : (d:ℂ)*(P:ℂ)-(S.card:ℂ) =
        ∑ j ∈ Finset.range (d-1), ∑ x ∈ S, (χ^(e*(j+1))) x := by
      linear_combination ht
    rw [hs]
    calc
      _ ≤ ∑ j ∈ Finset.range (d-1), ‖∑ x ∈ S, (χ^(e*(j+1))) x‖ := norm_sum_le _ _
      _ ≤ ∑ _ ∈ Finset.range (d-1), B := by
        apply Finset.sum_le_sum
        intro j hj
        apply hbound
        apply pow_ne_one_of_lt_orderOf (Nat.mul_ne_zero he.ne' (Nat.succ_ne_zero j))
        rw [ho, ← hed]
        exact Nat.mul_lt_mul_of_pos_left (by have := Finset.mem_range.mp hj; omega) he
      _ ≤ (d:ℝ)*B := by
        simp only [Finset.sum_const, Finset.card_range, nsmul_eq_mul]
        exact mul_le_mul_of_nonneg_right (by exact_mod_cast Nat.sub_le d 1) hB
  have hr : |(d:ℝ)*(P:ℝ)-(S.card:ℝ)| ≤ (d:ℝ)*B := by
    calc
      _ = |((d:ℂ)*(P:ℂ)-(S.card:ℂ)).re| := by simp
      _ ≤ ‖(d:ℂ)*(P:ℂ)-(S.card:ℂ)‖ := Complex.abs_re_le_norm _
      _ ≤ _ := hn
  have hdr : (0:ℝ) < d := by exact_mod_cast hd
  change |(P:ℝ)-(S.card:ℝ)/(d:ℝ)| ≤ B
  rw [show (P:ℝ)-(S.card:ℝ)/(d:ℝ) = ((d:ℝ)*(P:ℝ)-(S.card:ℝ))/(d:ℝ) by
    field_simp, abs_div, abs_of_pos hdr]
  exact (div_le_iff₀ hdr).mpr (by simpa only [mul_comm] using hr)

lemma orderCount_sum_divisors (S : Finset Q) (n : ℕ) (hn : n ≠ 0) :
    (∑ d ∈ n.divisors, (S.filter (fun x => orderOf x = d)).card) =
      (S.filter (fun x => x^n = 1)).card := by
  classical
  refine (Finset.card_biUnion ?_).symm.trans ?_
  · simp +contextual [Set.PairwiseDisjoint, Set.Pairwise, disjoint_iff, Finset.ext_iff]
  · congr
    ext x
    simp [hn, orderOf_dvd_iff_pow_eq_one, and_comm]

/-- Möbius inversion for the exact-order count inside any finite set. -/
lemma orderCount_moebius (S : Finset Q) (m : ℕ) (hm : 0 < m) :
    ((S.filter (fun x => orderOf x = m)).card : ℝ) =
      ∑ d ∈ m.divisors, (ArithmeticFunction.moebius d : ℝ) *
        ((S.filter (fun x => x^(m/d) = 1)).card : ℝ) := by
  have h := (ArithmeticFunction.sum_eq_iff_sum_mul_moebius_eq (R := ℝ)
    (f := fun d => ((S.filter (fun x => orderOf x = d)).card : ℝ))
    (g := fun d => ((S.filter (fun x => x^d = 1)).card : ℝ))).mp
      (fun n hn => by exact_mod_cast orderCount_sum_divisors S n hn.ne') m hm
  rw [Nat.sum_divisorsAntidiagonal (fun a b => (ArithmeticFunction.moebius a : ℝ) *
    ((S.filter (fun x => x^b = 1)).card : ℝ))] at h
  exact h.symm

lemma totient_moebius_real (m : ℕ) (hm : 0 < m) :
    (m.totient : ℝ) = ∑ d ∈ m.divisors,
      (ArithmeticFunction.moebius d : ℝ) * ((m/d : ℕ) : ℝ) := by
  have h := (ArithmeticFunction.sum_eq_iff_sum_mul_moebius_eq (R := ℝ)
    (f := fun d => (d.totient : ℝ)) (g := fun d => (d:ℝ))).mp
      (fun n _ => by exact_mod_cast Nat.sum_totient n) m hm
  rw [Nat.sum_divisorsAntidiagonal (fun a b => (ArithmeticFunction.moebius a : ℝ)*(b:ℝ))] at h
  exact h.symm

/-- Only squarefree divisors contribute to the Möbius error. -/
lemma sum_abs_moebius_real (m : ℕ) (hm : m ≠ 0) :
    (∑ d ∈ m.divisors, |(ArithmeticFunction.moebius d : ℝ)|) =
      (2:ℝ)^m.primeFactors.card := by
  classical
  simp_rw [← Int.cast_abs, ArithmeticFunction.abs_moebius, Int.cast_ite,
    Int.cast_one, Int.cast_zero]
  rw [← Finset.sum_filter, Nat.sum_divisors_filter_squarefree hm]
  simp [Nat.factors_eq]

lemma rootCount_error (S : Finset Q) (hS : ∀ x ∈ S, x ≠ 0)
    (B : ℝ) (hB : 0 ≤ B)
    (hbound : ∀ ψ : MulChar Q ℂ, ψ ≠ 1 → ‖∑ x ∈ S, ψ x‖ ≤ B)
    (e : ℕ) (he : e ∣ q-1) :
    |((S.filter (fun x => x^e = 1)).card : ℝ) -
      (S.card:ℝ)*(e:ℝ)/((q-1:ℕ):ℝ)| ≤ B := by
  have hN : 0 < q-1 := by have := (Fact.out : q.Prime).two_le; omega
  have he0 := Nat.pos_of_dvd_of_pos he hN
  have hd0 : 0 < (q-1)/e := Nat.div_pos (Nat.le_of_dvd hN he) he0
  have h := subgroup_count_bound S hS B hB hbound e ((q-1)/e)
    he0 hd0 (Nat.mul_div_cancel' he)
  simpa only [Nat.cast_div_charZero he, div_div_eq_mul_div] using h

/-- Exact-order discrepancy obtained by subgroup inversion. -/
theorem orderCount_error (S : Finset Q) (hS : ∀ x ∈ S, x ≠ 0)
    (B : ℝ) (hB : 0 ≤ B)
    (hbound : ∀ ψ : MulChar Q ℂ, ψ ≠ 1 → ‖∑ x ∈ S, ψ x‖ ≤ B)
    (m : ℕ) (hm : m ∣ q-1) :
    |((S.filter (fun x => orderOf x = m)).card : ℝ) -
      (S.card:ℝ)*(m.totient:ℝ)/((q-1:ℕ):ℝ)| ≤ (2:ℝ)^m.primeFactors.card * B := by
  have hN : 0 < q-1 := by have := (Fact.out : q.Prime).two_le; omega
  have hm0 := Nat.pos_of_dvd_of_pos hm hN
  have hmain : (S.card:ℝ)*(m.totient:ℝ)/((q-1:ℕ):ℝ) =
      ∑ d ∈ m.divisors, (ArithmeticFunction.moebius d : ℝ) *
        ((S.card:ℝ)*((m/d:ℕ):ℝ)/((q-1:ℕ):ℝ)) := by
    rw [totient_moebius_real m hm0, Finset.mul_sum, Finset.sum_div]
    apply Finset.sum_congr rfl
    intro d _
    ring
  rw [orderCount_moebius S m hm0, hmain, ← Finset.sum_sub_distrib]
  calc
    _ ≤ ∑ d ∈ m.divisors, |(ArithmeticFunction.moebius d : ℝ) *
        ((S.filter (fun x => x^(m/d) = 1)).card : ℝ) -
        (ArithmeticFunction.moebius d : ℝ) *
          ((S.card:ℝ)*((m/d:ℕ):ℝ)/((q-1:ℕ):ℝ))| := Finset.abs_sum_le_sum_abs _ _
    _ ≤ ∑ d ∈ m.divisors, |(ArithmeticFunction.moebius d : ℝ)| * B := by
      apply Finset.sum_le_sum
      intro d hd
      rw [← mul_sub, abs_mul]
      apply mul_le_mul_of_nonneg_left _ (abs_nonneg _)
      exact rootCount_error S hS B hB hbound (m/d)
        ((Nat.div_dvd_of_dvd (Nat.dvd_of_mem_divisors hd)).trans hm)
    _ = _ := by rw [← Finset.sum_mul, sum_abs_moebius_real m hm0.ne']

def keyOrder : ℕ := (p-1)/7

def keyIntervalLength : ℕ := 2^60-2^56-1

lemma keyOrder_primeFactors : keyOrder.primeFactors =
    {2, 3, 5, 11, 13, 31, 41, 61, 151, 331, 1321} := by
  have hf : keyOrder = 2*3*3*5*5*11*13*31*41*61*151*331*1321 := by norm_num [keyOrder, p]
  ext r
  constructor
  · intro h
    obtain ⟨hr, hd, _⟩ := Nat.mem_primeFactors.mp h
    rw [hf] at hd
    simp only [hr.dvd_mul] at hd
    simp only [
      Nat.prime_dvd_prime_iff_eq hr (by norm_num : Nat.Prime 2),
      Nat.prime_dvd_prime_iff_eq hr (by norm_num : Nat.Prime 3),
      Nat.prime_dvd_prime_iff_eq hr (by norm_num : Nat.Prime 5),
      Nat.prime_dvd_prime_iff_eq hr (by norm_num : Nat.Prime 11),
      Nat.prime_dvd_prime_iff_eq hr (by norm_num : Nat.Prime 13),
      Nat.prime_dvd_prime_iff_eq hr (by norm_num : Nat.Prime 31),
      Nat.prime_dvd_prime_iff_eq hr (by norm_num : Nat.Prime 41),
      Nat.prime_dvd_prime_iff_eq hr (by norm_num : Nat.Prime 61),
      Nat.prime_dvd_prime_iff_eq hr (by norm_num : Nat.Prime 151),
      Nat.prime_dvd_prime_iff_eq hr (by norm_num : Nat.Prime 331),
      Nat.prime_dvd_prime_iff_eq hr (by norm_num : Nat.Prime 1321)] at hd
    simpa only [Finset.mem_insert, Finset.mem_singleton, or_assoc, or_self] using hd
  · intro h
    simp only [Finset.mem_insert, Finset.mem_singleton] at h
    rcases h with rfl | rfl | rfl | rfl | rfl | rfl | rfl | rfl | rfl | rfl | rfl
    all_goals norm_num [Nat.mem_primeFactors, keyOrder, p]

lemma keyOrder_totient : keyOrder.totient = 67744512000000000 := by
  rw [Nat.totient_eq_div_primeFactors_mul, keyOrder_primeFactors]
  norm_num [keyOrder, p]

end ProvenHashes.Polymur
