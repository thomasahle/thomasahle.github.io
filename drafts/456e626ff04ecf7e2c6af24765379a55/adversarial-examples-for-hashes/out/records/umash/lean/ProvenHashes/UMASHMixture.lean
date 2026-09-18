import ProvenHashes.UMASHProbability

namespace ProvenHashes.UMASH
open scoped BigOperators

theorem probability_le_one {K : Type*} [Fintype K] [Nonempty K] (E : K → Prop) :
    uniformProb E ≤ 1 := by
  classical
  unfold uniformProb
  have h : (0 : ℚ≥0) < Fintype.card K := by exact_mod_cast Fintype.card_pos
  apply (div_le_one h).mpr
  exact_mod_cast Finset.card_le_card (Finset.filter_subset E Finset.univ)
-- CHECKPOINT

theorem probability_complement {K : Type*} [Fintype K] [Nonempty K] (E : K → Prop) :
    uniformProb E + uniformProb (fun k => ¬E k) = 1 := by
  classical
  have h : (Fintype.card K : ℚ≥0) ≠ 0 := by exact_mod_cast Fintype.card_ne_zero
  unfold uniformProb
  rw [← add_div, ← Nat.cast_add, Finset.filter_card_add_filter_neg_card_eq_card,
    Finset.card_univ, div_self h]
-- CHECKPOINT

theorem probability_prod {A B : Type*} [Fintype A] [Fintype B] (E : A × B → Prop) :
    uniformProb E = (∑ a, uniformProb (fun b => E (a,b))) / Fintype.card A := by
  classical
  have hc : (Finset.univ.filter E).card =
      ∑ a, (Finset.univ.filter (fun b => E (a,b))).card := by
    simp only [Finset.card_eq_sum_ones, Finset.sum_filter, Fintype.sum_prod_type]
  simp only [uniformProb, hc, Fintype.card_prod, Nat.cast_mul, Nat.cast_sum]
  rw [← Finset.sum_div, div_div, mul_comm]
-- CHECKPOINT

theorem probability_mixture {A B : Type*} [Fintype A] [Fintype B]
    [Nonempty A] [Nonempty B] (E : A × B → Prop) (bad : A → Prop)
    (r bound : ℚ≥0) (hr : r ≤ 1) (hb : uniformProb bad ≤ bound)
    (hslice : ∀ a, ¬bad a → uniformProb (fun b => E (a,b)) ≤ r) :
    uniformProb E ≤ bound + (1-bound)*r := by
  classical
  have hs : uniformProb E ≤ uniformProb bad + uniformProb (fun a => ¬bad a)*r := by
    rw [probability_prod]
    calc
      _ ≤ (∑ a, if bad a then (1 : ℚ≥0) else r) / Fintype.card A := by
        apply div_le_div_of_nonneg_right _ (by positivity)
        apply Finset.sum_le_sum
        intro a _
        split_ifs with h
        · exact probability_le_one _
        · exact hslice a h
      _ = _ := by
        rw [Finset.sum_ite]
        simp only [Finset.sum_const, nsmul_eq_mul, mul_one, uniformProb]
        rw [add_div, mul_div_right_comm]
        congr 2 <;> congr 1
        norm_cast
        apply congrArg Finset.card
        ext a
        simp
  have hc := probability_complement bad
  apply NNRat.coe_le_coe.mp
  have hsQ := NNRat.coe_le_coe.mpr hs
  have hbQ := NNRat.coe_le_coe.mpr hb
  have hrQ := NNRat.coe_le_coe.mpr hr
  have hcQ := congrArg (fun a : ℚ≥0 => (a : ℚ)) hc
  by_cases hbound : bound ≤ 1
  · have ht := NNRat.coe_sub hbound
    norm_num only [NNRat.coe_add, NNRat.coe_mul, NNRat.coe_one] at hsQ hcQ ⊢
    rw [ht]
    norm_num only [NNRat.coe_one] at hrQ ⊢
    nlinarith
  · have hb1 : (1 : ℚ≥0) ≤ bound := le_of_not_ge hbound
    exact NNRat.coe_le_coe.mpr ((probability_le_one E).trans
      (hb1.trans (le_add_of_nonneg_right (by positivity))))
-- CHECKPOINT

end ProvenHashes.UMASH
