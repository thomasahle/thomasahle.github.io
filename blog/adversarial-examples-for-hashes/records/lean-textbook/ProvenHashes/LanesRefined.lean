import ProvenHashes.EightLanes

/-! Refined bound for the eight-lane recurrence row: lanes that are empty for the
common length hold the same value z in both messages, so the combining polynomial
in the fourth key has degree at most J-1, J = min 8 ceil(L/2) = number of nonempty
lanes. Result: epsilon(L) <= (rounds L + J - 1)/2^64 <= L/2^64, i.e. score >= 64
(the published envelope (rounds L + 7)/2^64 gives 61). -/
namespace ProvenHashes.LanesRefined
open Polynomial ProvenHashes
open scoped BigOperators
noncomputable section
variable {F : Type*} [Field F]

/-- Vectors that agree at every index >= J: the difference polynomial has degree <= J-1. -/
theorem prefix_collision_bound [Fintype F] (J : ℕ) (v v' : Fin 8 → F) (hne : v ≠ v')
    (heq : ∀ j : Fin 8, J ≤ j.val → v j = v' j) :
    uniformProb (fun x : F => polynomialHash v x = polynomialHash v' x) ≤
      ((J - 1 : ℕ) : ℚ≥0) / Fintype.card F := by
  classical
  let p := Polynomial.ofFn 8 v - Polynomial.ofFn 8 v'
  have hp : p ≠ 0 := by
    intro h
    exact hne (Polynomial.injective_ofFn 8 (sub_eq_zero.mp h))
  have hd : p.natDegree ≤ J - 1 := by
    apply Polynomial.natDegree_le_iff_coeff_eq_zero.mpr
    intro i hi
    have hi' : J ≤ i := by omega
    by_cases h8 : i < 8
    · simp only [p, Polynomial.coeff_sub, Polynomial.ofFn_coeff_eq_val_of_lt _ h8]
      rw [heq ⟨i, h8⟩ hi', sub_self]
    · push_neg at h8
      simp [p, Polynomial.ofFn_coeff_eq_zero_of_ge _ h8]
  have hc := (polynomial_zero_count p hp).trans hd
  have he : (fun x : F => polynomialHash v x = polynomialHash v' x) =
      (fun x : F => p.eval x = 0) := by
    funext x
    simp [p, polynomialHash_eq_eval, sub_eq_zero]
  rw [he, uniformProb]
  exact div_le_div_of_nonneg_right (by exact_mod_cast hc) (by positivity)

/-- Number of nonempty lanes for L words. -/
def nonempty (L : ℕ) : ℕ := min 8 ((L + 1) / 2)

theorem lane_empty {L : ℕ} (m : Fin L → F) (j : Fin 8) (hj : nonempty L ≤ j.val) :
    EightLanes.lanes m j = [] := by
  have h0 : EightLanes.laneLength L j = 0 := by
    unfold EightLanes.laneLength
    unfold nonempty at hj
    have := j.isLt
    omega
  apply List.eq_nil_of_length_eq_zero
  simp [h0]

/-- The refined chart bound for the eight-lane construction (same model as
`EightLanes.word_collision_bound_gf64`). -/
theorem word_collision_bound_refined_gf64 {L : ℕ}
    (m m' : Fin L → GaloisField 2 64) (hne : m ≠ m') :
    uniformProb (fun k => EightLanes.wordHash m k = EightLanes.wordHash m' k) ≤
      ((EightLanes.rounds L + (nonempty L - 1) : ℕ) : ℚ≥0) / 2^64 := by
  classical
  have hne' : EightLanes.lanes m ≠ EightLanes.lanes m' :=
    fun h => hne (EightLanes.lanes_injective L h)
  obtain ⟨j, hj⟩ : ∃ j, EightLanes.lanes m j ≠ EightLanes.lanes m' j := Function.ne_iff.mp hne'
  have hi : uniformProb (fun k : Fin 3 → GaloisField 2 64 =>
      (fun j => Recurrence.hash (EightLanes.lanes m j) k) =
        (fun j => Recurrence.hash (EightLanes.lanes m' j) k)) ≤
      (EightLanes.rounds L : ℚ≥0) / Fintype.card (GaloisField 2 64) := by
    calc
      _ ≤ uniformProb (fun k => Recurrence.hash (EightLanes.lanes m j) k =
            Recurrence.hash (EightLanes.lanes m' j) k) :=
        uniformProb_mono (fun k h => congrFun h j)
      _ ≤ ((EightLanes.lanes m j).length : ℚ≥0) / Fintype.card (GaloisField 2 64) :=
        Recurrence.collision_bound _ _ (by simp) hj
      _ ≤ _ := by
        apply div_le_div_of_nonneg_right _ (by positivity)
        simp only [EightLanes.lanes_length]
        exact_mod_cast (by unfold EightLanes.laneLength EightLanes.rounds; omega)
  have hg : ∀ k : Fin 3 → GaloisField 2 64,
      (fun j => Recurrence.hash (EightLanes.lanes m j) k) ≠
        (fun j => Recurrence.hash (EightLanes.lanes m' j) k) →
      uniformProb (fun x : GaloisField 2 64 =>
        polynomialHash (fun j => Recurrence.hash (EightLanes.lanes m j) k) x =
        polynomialHash (fun j => Recurrence.hash (EightLanes.lanes m' j) k) x) ≤
      ((nonempty L - 1 : ℕ) : ℚ≥0) / Fintype.card (GaloisField 2 64) := by
    intro k hk
    apply prefix_collision_bound (nonempty L) _ _ hk
    intro j hj
    simp only [lane_empty m j hj, lane_empty m' j hj]
  have h := compose_collision_bound
    (fun k j => Recurrence.hash (EightLanes.lanes m j) k)
    (fun k j => Recurrence.hash (EightLanes.lanes m' j) k)
    (fun x v => polynomialHash v x) _ _ hi hg
  have hw : (fun k => EightLanes.wordHash m k = EightLanes.wordHash m' k) =
      (fun k : (Fin 3 → GaloisField 2 64) × GaloisField 2 64 =>
        polynomialHash (fun j => Recurrence.hash (EightLanes.lanes m j) k.1) k.2 =
        polynomialHash (fun j => Recurrence.hash (EightLanes.lanes m' j) k.1) k.2) := by
    funext k; rfl
  rw [hw]
  refine h.trans (le_of_eq ?_)
  rw [gf64_card]
  push_cast
  ring

/-- The refined numerator never exceeds L, so the score min_L log2(L/eps) is at least 64. -/
theorem refined_numerator_le (L : ℕ) (hL : 1 ≤ L) :
    EightLanes.rounds L + (nonempty L - 1) ≤ L := by
  unfold EightLanes.rounds nonempty
  omega

theorem refined_ratio (L : ℕ) (hL : 1 ≤ L) :
    ((EightLanes.rounds L + (nonempty L - 1) : ℕ) : ℚ≥0) / 2^64 ≤ (L : ℚ≥0) / 2^64 :=
  div_le_div_of_nonneg_right (by exact_mod_cast refined_numerator_le L hL) (by positivity)

end
end ProvenHashes.LanesRefined
