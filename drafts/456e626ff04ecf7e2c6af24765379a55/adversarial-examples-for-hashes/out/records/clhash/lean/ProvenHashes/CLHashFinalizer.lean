import ProvenHashes.CLHashAlgorithm
import ProvenHashes.ConcreteChainHash

noncomputable section
namespace ProvenHashes.CLHash
open ChainHash Polynomial

/-- The final binary remainder, decoded in the polynomial basis. -/
def reduce64 (p : BitsPolynomial) : Word 64 := lowWord (p %ₘ modulus)

theorem reduce64_eq (p : BitsPolynomial) :
    reduce64 p = fieldRepr.symm (AdjoinRoot.mk modulus p) := by
  change lowWord (p %ₘ modulus) =
    lowWord (AdjoinRoot.modByMonicHom modulus_monic (AdjoinRoot.mk modulus p))
  rw [AdjoinRoot.modByMonicHom_mk]

/-- Reduction modulo the irreducible 64-bit polynomial preserves the CLNH bound. -/
theorem reduced_clnh_difference_bound {I : Type*} [Fintype I] [DecidableEq I]
    (s : Finset I) (m m' : I × Bool → Word 64)
    (hne : ∃ i ∈ s, ∃ b, m (i, b) ≠ m' (i, b)) (C : BinaryQuotient) :
    uniformProb (fun k : I × Bool → Word 64 =>
      AdjoinRoot.mk modulus (clnh s m k) - AdjoinRoot.mk modulus (clnh s m' k) = C) ≤
      1 / (2 : ℚ≥0) ^ 64 := by
  classical
  obtain ⟨i, hi, b, hb⟩ := hne
  have hδ : AdjoinRoot.mk modulus (pack 64 (m (i, b))) -
      AdjoinRoot.mk modulus (pack 64 (m' (i, b))) ≠ 0 :=
    sub_ne_zero.mpr (fun h => hb (fieldRepr.injective h))
  have hinj (k : I × Bool → Word 64) : Function.Injective
      (fun v => AdjoinRoot.mk modulus (clnh s m (Function.update k (i, !b) v)) -
        AdjoinRoot.mk modulus (clnh s m' (Function.update k (i, !b) v))) := by
    have he (v : Word 64) :
        AdjoinRoot.mk modulus (clnh s m (Function.update k (i, !b) v)) -
          AdjoinRoot.mk modulus (clnh s m' (Function.update k (i, !b) v)) =
        (AdjoinRoot.mk modulus (pack 64 (m (i, b))) -
          AdjoinRoot.mk modulus (pack 64 (m' (i, b)))) *
            AdjoinRoot.mk modulus (pack 64 v) +
          (AdjoinRoot.mk modulus
            ((pack 64 (m (i, b)) + pack 64 (k (i, b))) * pack 64 (m (i, !b))) -
           AdjoinRoot.mk modulus
            ((pack 64 (m' (i, b)) + pack 64 (k (i, b))) * pack 64 (m' (i, !b))) +
           AdjoinRoot.mk modulus (clnh (s.erase i) m k) -
           AdjoinRoot.mk modulus (clnh (s.erase i) m' k)) := by
      rw [clnh_update _ _ _ _ hi, clnh_update _ _ _ _ hi]
      simp only [Bool.not_not, map_add, map_mul]
      ring
    intro v v' hv
    dsimp only at hv
    rw [he, he] at hv
    exact fieldRepr.injective (mul_left_cancel₀ hδ (add_right_cancel hv))
  have h := uniformProb_of_injective_update
    (fun k : I × Bool → Word 64 =>
      AdjoinRoot.mk modulus (clnh s m k) - AdjoinRoot.mk modulus (clnh s m' k))
    (i, !b) hinj C
  simpa only [word_card, Nat.cast_pow, Nat.cast_ofNat] using h

/-- The final two-word product contributes one further 1/2^64 term. -/
theorem finalProduct_difference_bound (p q : BitsPolynomial)
    (hp : p.natDegree < 128) (hq : q.natDegree < 128) (hne : p ≠ q)
    (C : BinaryQuotient) :
    uniformProb (fun k : Word 64 × Word 64 =>
      AdjoinRoot.mk modulus (finalProduct p k) -
        AdjoinRoot.mk modulus (finalProduct q k) = C) ≤ 1 / (2 : ℚ≥0) ^ 64 := by
  classical
  let e : (Word 64 × Word 64) ≃ (Fin 1 × Bool → Word 64) :=
    { toFun := fun k j => if j.2 then k.2 else k.1
      invFun := fun f => (f (0, false), f (0, true))
      left_inv := by intro k; rfl
      right_inv := by
        intro f
        funext ⟨i, b⟩
        have hi : i = 0 := Subsingleton.elim _ _
        subst i
        cases b <;> rfl }
  let a : Fin 1 × Bool → Word 64 := fun j => if j.2 then highWord p else lowWord p
  let b : Fin 1 × Bool → Word 64 := fun j => if j.2 then highWord q else lowWord q
  have hd : ∃ i ∈ (Finset.univ : Finset (Fin 1)), ∃ t, a (i, t) ≠ b (i, t) := by
    by_contra! h
    exact hne (splitWords_injective hp hq (h 0 (by simp) false) (h 0 (by simp) true))
  have he := uniformProb_equiv e (fun k : Fin 1 × Bool → Word 64 =>
    AdjoinRoot.mk modulus (clnh Finset.univ a k) -
      AdjoinRoot.mk modulus (clnh Finset.univ b k) = C)
  have hr := reduced_clnh_difference_bound Finset.univ a b hd C
  rw [← he] at hr
  simpa [clnh, a, b, e, finalProduct] using hr

/-- Difference universality stated for the literal final 64-bit remainder. -/
theorem final_reduction_difference_bound (p q : BitsPolynomial)
    (hp : p.natDegree < 128) (hq : q.natDegree < 128) (hne : p ≠ q)
    (c : Word 64) :
    uniformProb (fun k : Word 64 × Word 64 =>
      reduce64 (finalProduct p k) - reduce64 (finalProduct q k) = c) ≤
        1 / (2 : ℚ≥0) ^ 64 := by
  have he : (fun k : Word 64 × Word 64 =>
      reduce64 (finalProduct p k) - reduce64 (finalProduct q k) = c) =
      (fun k => AdjoinRoot.mk modulus (finalProduct p k) -
        AdjoinRoot.mk modulus (finalProduct q k) = fieldRepr c) := by
    funext k
    apply propext
    rw [← fieldRepr.injective.eq_iff, map_sub, reduce64_eq, reduce64_eq,
      fieldRepr.apply_symm_apply, fieldRepr.apply_symm_apply]
  rw [he]
  exact finalProduct_difference_bound p q hp hq hne (fieldRepr c)

end ProvenHashes.CLHash
