import ProvenHashes.CLHashAlgorithm
import ProvenHashes.CLHashField

noncomputable section
namespace ProvenHashes.CLHash
open ChainHash Polynomial
open scoped BigOperators

/-- Root counting on any injectively embedded finite key set. -/
theorem restricted_polynomial_collision_bound {F K : Type*}
    [Field F] [Fintype F] [Fintype K]
    (embed : K → F) (hinj : Function.Injective embed)
    {n : ℕ} (a b : Fin n → F) (hne : a ≠ b) :
    uniformProb (fun k : K => polynomialHash a (embed k) = polynomialHash b (embed k)) ≤
      ((n - 1 : ℕ) : ℚ≥0) / Fintype.card K := by
  classical
  have hn : 1 ≤ n := by
    by_contra h
    have h0 : n = 0 := by omega
    subst n
    exact hne (Subsingleton.elim _ _)
  let p := Polynomial.ofFn n a - Polynomial.ofFn n b
  have hp : p ≠ 0 := fun h => hne (Polynomial.injective_ofFn n (sub_eq_zero.mp h))
  have hd : p.natDegree ≤ n - 1 := by
    apply (Polynomial.natDegree_sub_le _ _).trans
    apply max_le
    · exact Nat.le_pred_of_lt (Polynomial.ofFn_natDegree_lt hn a)
    · exact Nat.le_pred_of_lt (Polynomial.ofFn_natDegree_lt hn b)
  have hc : (Finset.univ.filter fun k : K => p.eval (embed k) = 0).card ≤
      (Finset.univ.filter fun x : F => p.eval x = 0).card := by
    apply Finset.card_le_card_of_injOn embed
    · intro k hk
      simpa using hk
    · exact hinj.injOn
  have hcount := hc.trans ((polynomial_zero_count p hp).trans hd)
  have he : (fun k : K => polynomialHash a (embed k) = polynomialHash b (embed k)) =
      (fun k => p.eval (embed k) = 0) := by
    funext k
    simp [p, polynomialHash_eq_eval, sub_eq_zero]
  rw [he, uniformProb]
  exact div_le_div_of_nonneg_right (by exact_mod_cast hcount) (by positivity)

/-- The lazy remainder has exactly 128 bits. -/
theorem lazyModulus_spec : lazyModulus.Monic ∧ lazyModulus.natDegree = 128 := by
  constructor
  · exact modulus127_spec.1.mul monic_X
  · rw [lazyModulus, natDegree_mul modulus127_spec.1.ne_zero X_ne_zero,
      modulus127_spec.2, natDegree_X]

/-- Lazy reduction still represents the same element of GF(2^127). -/
theorem mk127_lazy_remainder (p : BitsPolynomial) :
    AdjoinRoot.mk modulus127 (p %ₘ lazyModulus) = AdjoinRoot.mk modulus127 p := by
  rw [modByMonic_eq_sub_mul_div p lazyModulus_spec.1]
  simp [lazyModulus, map_sub, map_mul, AdjoinRoot.mk_self]

/-- The literal descending Horner polynomial, projected to the field. -/
theorem aggregate_projection {n : ℕ} (a : Fin n → BitsPolynomial) (k : Word 126) :
    AdjoinRoot.mk modulus127 (aggregate a k) =
      polynomialHash (fun i => AdjoinRoot.mk modulus127 (a i.rev)) (polyKey k) := by
  rw [aggregate, mk127_lazy_remainder, map_sum]
  simp only [map_mul, map_pow, polynomialHash]
  rw [← Equiv.sum_comp Fin.revPerm]
  apply Finset.sum_congr rfl
  intro i hi
  simp [Fin.revPerm, Fin.rev, polyKey]
  left
  congr 1
  omega

/-- The polynomial collision bound uses 126 key bits, including lazy reduction. -/
theorem aggregate_collision_bound : AggregateCollisionBound := by
  intro n a b ha hb hab
  have hne : (fun i : Fin n => AdjoinRoot.mk modulus127 (a i.rev)) ≠
      (fun i : Fin n => AdjoinRoot.mk modulus127 (b i.rev)) := by
    intro he
    apply hab
    funext i
    apply mk127_injective_below (ha i) (hb i)
    simpa only [Fin.rev_rev] using congrFun he i.rev
  have h := restricted_polynomial_collision_bound polyKey polyKey_injective _ _ hne
  have hm := uniformProb_mono (E := fun k : Word 126 => aggregate a k = aggregate b k)
    (D := fun k => polynomialHash (fun i => AdjoinRoot.mk modulus127 (a i.rev)) (polyKey k) =
      polynomialHash (fun i => AdjoinRoot.mk modulus127 (b i.rev)) (polyKey k)) (by
      intro k hk
      simpa only [aggregate_projection] using congrArg (AdjoinRoot.mk modulus127) hk)
  exact hm.trans (by simpa only [word_card, Nat.cast_pow, Nat.cast_ofNat] using h)

theorem aggregate_degree {n : ℕ} (a : Fin n → BitsPolynomial) (k : Word 126) :
    (aggregate a k).natDegree < 128 := by
  have hone : lazyModulus ≠ 1 := by
    intro h
    have he := congrArg Polynomial.natDegree h
    rw [lazyModulus_spec.2, natDegree_one] at he
    omega
  exact (Polynomial.natDegree_modByMonic_lt _ lazyModulus_spec.1 hone).trans_le
    (by rw [lazyModulus_spec.2])

end ProvenHashes.CLHash
