import ProvenHashes.Highway.ArithmeticCount
namespace ProvenHashes.Highway
open scoped BigOperators
set_option maxRecDepth 10000
set_option maxHeartbeats 500000

theorem product_filter_card {A B : Type*} [Fintype A] [Fintype B]
    (P : A × B → Prop) [DecidablePred P] :
    (Finset.univ.filter P).card =
      ∑ a : A, (Finset.univ.filter fun b : B => P (a,b)).card := by
  simp only [Finset.card_eq_sum_ones, Finset.sum_filter, Fintype.sum_prod_type]

/-- Counting the top byte and the 24-bit arithmetic fibre gives 235 × 239. -/
theorem top_arithmetic_card (sigma : Fin 256 → Fin 2) (s : Fin 256 → Fin 65536)
    (h0 : ∀ w : Fin 256, (sigma w).val = 0 → 203 ≤ windowStart (s w).val ((235+w.val)%256))
    (h1 : ∀ w : Fin 256, (sigma w).val = 1 → windowStart (s w).val ((235+w.val)%256) ≤ 203) :
    (Finset.univ.filter fun p : Fin 256 × (Fin 65536 × Fin 256) =>
      21 ≤ p.1.val ∧ arithmeticFibre (sigma p.1).val (s p.1).val ((235+p.1.val)%256)
        p.2.1.val p.2.2.val).card = 56165 := by
  rw [product_filter_card]
  have hs (w : Fin 256) :
      (Finset.univ.filter fun p : Fin 65536 × Fin 256 =>
        21 ≤ w.val ∧ arithmeticFibre (sigma w).val (s w).val ((235+w.val)%256)
          p.1.val p.2.val).card = if 21 ≤ w.val then 239 else 0 := by
    by_cases hw : 21 ≤ w.val
    · simp only [hw, true_and, ite_true]
      exact arithmetic_fibre_card (sigma w) (s w)
        ⟨(235+w.val)%256, Nat.mod_lt _ (by decide)⟩ (h0 w) (h1 w)
    · simp only [hw, false_and, Finset.filter_false, Finset.card_empty, ite_false]
  simp_rw [hs]
  rw [← Finset.sum_filter]
  simp only [Finset.sum_const, nsmul_eq_mul, top_byte_card]
  decide

theorem unique_pad_card {A B : Type*} [Fintype A] [Fintype B]
    (P : A → Prop) [DecidablePred P] (Q : A → B → Prop) [∀ a, DecidablePred (Q a)]
    (hQ : ∀ a, (Finset.univ.filter (Q a)).card = 1) :
    (Finset.univ.filter fun p : A × B => P p.1 ∧ Q p.1 p.2).card =
      (Finset.univ.filter P).card := by
  rw [product_filter_card]
  have hs (a : A) : (Finset.univ.filter fun b : B => P a ∧ Q a b).card =
      if P a then 1 else 0 := by
    by_cases hp : P a
    · simp only [hp, true_and, ite_true, hQ]
    · simp only [hp, false_and, Finset.filter_false, Finset.card_empty, ite_false]
  simp_rw [hs]
  rw [← Finset.sum_filter]
  simp

end ProvenHashes.Highway
