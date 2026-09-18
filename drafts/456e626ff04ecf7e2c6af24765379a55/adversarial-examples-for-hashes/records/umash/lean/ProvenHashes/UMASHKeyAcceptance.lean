import ProvenHashes.UMASHAssembly

namespace ProvenHashes.UMASH
open scoped BigOperators
set_option maxRecDepth 8192
attribute [local irreducible] uniformProb

theorem word_card : Fintype.card Word = q := by
  exact Fintype.card_congr BitVec.equivFin.toEquiv |>.trans (Fintype.card_fin q)
-- CHECKPOINT

theorem independent_words_equal (i j : Fin 34) (hij : i ≠ j) :
    uniformProb (fun k : OHKey => k i = k j) = (1:ℚ≥0)/q := by
  have hbij : ∀ k : OHKey, Function.Bijective
      (fun v : Word => (Function.update k i v) i ^^^ (Function.update k i v) j) := by
    intro k
    simp only [Function.update_self, Function.update_of_ne hij.symm]
    refine ⟨?_, ?_⟩
    · intro a b h
      have := congrArg (fun v : Word => v ^^^ k j) h
      simpa only [BitVec.xor_assoc, BitVec.xor_self, BitVec.xor_zero] using this
    · intro a
      refine ⟨a ^^^ k j, ?_⟩
      simp only [BitVec.xor_assoc, BitVec.xor_self, BitVec.xor_zero]
  have h := uniformProb_of_bijective_update (fun k : OHKey => k i ^^^ k j) i hbij 0
  have he : (fun k : OHKey => k i ^^^ k j = 0) = (fun k => k i = k j) := by
    funext k
    exact propext BitVec.xor_eq_zero_iff
  rw [he, word_card] at h
  exact h
-- CHECKPOINT

theorem probability_union_bound {K I : Type*} [Fintype K] (s : Finset I)
    (E : I → K → Prop) :
    uniformProb (fun k => ∃ i ∈ s, E i k) ≤ ∑ i ∈ s, uniformProb (E i) := by
  classical
  unfold uniformProb
  rw [← Finset.sum_div]
  apply div_le_div_of_nonneg_right _ (by positivity)
  rw [← Nat.cast_sum]
  apply Nat.cast_le.mpr
  calc
    _ = (s.biUnion fun i => Finset.univ.filter (E i)).card := by
      apply congrArg Finset.card
      ext k
      simp
    _ ≤ _ := Finset.card_biUnion_le
-- CHECKPOINT

def keyPairs : Finset (Fin 34 × Fin 34) := Finset.univ.filter (fun ij => ij.1 < ij.2)

theorem keyPairs_card : keyPairs.card = 561 := by decide +kernel
-- CHECKPOINT

theorem noninjective_key_iff (k : OHKey) :
    ¬Function.Injective k ↔ ∃ ij ∈ keyPairs, k ij.1 = k ij.2 := by
  classical
  constructor
  · intro h
    obtain ⟨i,j,hk,hij⟩ := Function.not_injective_iff.mp h
    rcases lt_or_gt_of_ne hij with hij | hji
    · exact ⟨(i,j), Finset.mem_filter.mpr ⟨Finset.mem_univ _, hij⟩, hk⟩
    · exact ⟨(j,i), Finset.mem_filter.mpr ⟨Finset.mem_univ _, hji⟩, hk.symm⟩
  · rintro ⟨ij, hij, hk⟩ h
    exact (ne_of_lt (Finset.mem_filter.mp hij).2) (h hk)
-- CHECKPOINT

theorem noninjective_key_probability :
    uniformProb (fun k : OHKey => ¬Function.Injective k) ≤ (561:ℚ≥0)/q := by
  have he : (fun k : OHKey => ¬Function.Injective k) =
      (fun k => ∃ ij ∈ keyPairs, k ij.1 = k ij.2) := by
    funext k
    exact propext (noninjective_key_iff k)
  rw [he]
  apply (probability_union_bound keyPairs (fun ij (k : OHKey) => k ij.1 = k ij.2)).trans
  have hs : (∑ ij ∈ keyPairs, uniformProb (fun k : OHKey => k ij.1 = k ij.2)) =
      ∑ _ij ∈ keyPairs, (1:ℚ≥0)/q := by
    apply Finset.sum_congr rfl
    intro ij hij
    exact independent_words_equal ij.1 ij.2 (ne_of_lt (Finset.mem_filter.mp hij).2)
  rw [hs]
  simp only [Finset.sum_const, nsmul_eq_mul, keyPairs_card]
  norm_num [div_eq_mul_inv]
-- CHECKPOINT

theorem distinct_key_acceptance : DistinctKeyAcceptance := by
  have h := noninjective_key_probability
  have hc := probability_complement (fun k : OHKey => Function.Injective k)
  have arithmetic (a b : ℚ≥0) (ha : a+b=1) (hb : b ≤ (561:ℚ≥0)/q) :
      ((q-561 : ℕ) : ℚ≥0)/q ≤ a := by
    apply NNRat.coe_le_coe.mp
    have hQ := NNRat.coe_le_coe.mpr hb
    have hcQ := congrArg (fun a : ℚ≥0 => (a : ℚ)) ha
    norm_num [q] at hQ ⊢
    norm_num only [NNRat.coe_add, NNRat.coe_one] at hcQ
    linarith
  exact arithmetic _ _ hc h
-- CHECKPOINT

end ProvenHashes.UMASH
