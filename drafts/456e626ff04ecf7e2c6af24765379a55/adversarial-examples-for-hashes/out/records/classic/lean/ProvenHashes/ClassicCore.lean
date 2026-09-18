import ProvenHashes.Polynomial

namespace ProvenHashes.Classic
noncomputable section
open Polynomial
open scoped BigOperators

variable {F : Type*} [Field F]

theorem uniformProb_ignore_right {A B : Type*} [Fintype A] [Fintype B]
    [Nonempty B] (event : A → Prop) :
    uniformProb (fun k : A × B => event k.1) = uniformProb event := by
  classical
  have hs : (Finset.univ.filter fun k : A × B => event k.1) =
      (Finset.univ.filter event) ×ˢ (Finset.univ : Finset B) := by ext k; simp
  have hb : (Fintype.card B : ℚ≥0) ≠ 0 := by exact_mod_cast Fintype.card_ne_zero
  simp only [uniformProb, hs, Finset.card_product, Finset.card_univ,
    Fintype.card_prod, Nat.cast_mul]
  exact mul_div_mul_right _ _ hb

/-- Low coefficient first. Reverse the block list to obtain Horner order. -/
def coeffs : List F → F[X]
  | [] => 0
  | a :: as => C a + X * coeffs as

@[simp] theorem coeffs_zero (as : List F) :
    (coeffs as).coeff 0 = as.headD 0 := by
  cases as <;> simp [coeffs]

theorem coeffs_cons_inj {a b : F} {as bs : List F}
    (h : coeffs (a :: as) = coeffs (b :: bs)) :
    a = b ∧ coeffs as = coeffs bs := by
  have hab : a = b := by simpa using congrArg (fun p : F[X] => p.coeff 0) h
  subst b
  simp only [coeffs, add_right_inj] at h
  exact ⟨rfl, mul_left_cancel₀ X_ne_zero h⟩

theorem coeffs_inj_length {as bs : List F} (hl : as.length = bs.length)
    (h : coeffs as = coeffs bs) : as = bs := by
  induction as generalizing bs with
  | nil => simpa using hl.symm
  | cons a as ih =>
    cases bs with
    | nil => simp at hl
    | cons b bs =>
      obtain ⟨rfl, ht⟩ := coeffs_cons_inj h
      exact congrArg (List.cons a) (ih (by simpa using hl) ht)

theorem coeffs_inj_nonzero {as bs : List F}
    (ha : ∀ a ∈ as, a ≠ 0) (hb : ∀ b ∈ bs, b ≠ 0)
    (h : coeffs as = coeffs bs) : as = bs := by
  induction as generalizing bs with
  | nil =>
    cases bs with
    | nil => rfl
    | cons b bs =>
      have hz := congrArg (fun p : F[X] => p.coeff 0) h
      exact False.elim (hb b (by simp) (by simpa [coeffs] using hz.symm))
  | cons a as ih =>
    cases bs with
    | nil =>
      have hz := congrArg (fun p : F[X] => p.coeff 0) h
      exact False.elim (ha a (by simp) (by simpa [coeffs] using hz))
    | cons b bs =>
      obtain ⟨rfl, ht⟩ := coeffs_cons_inj h
      exact congrArg (List.cons a) (ih (fun x hx => ha x (by simp [hx]))
        (fun x hx => hb x (by simp [hx])) ht)

theorem coeffs_degree (as : List F) : (coeffs as).natDegree ≤ as.length - 1 := by
  induction as with
  | nil => simp [coeffs]
  | cons a as ih =>
    cases as with
    | nil => simp [coeffs]
    | cons b bs =>
      apply (natDegree_add_le _ _).trans
      apply max_le
      · simp
      · exact (natDegree_mul_le).trans (by simp only [natDegree_X, List.length_cons] at *; omega)

def positive (as : List F) : F[X] := X * coeffs as

@[simp] theorem positive_zero (as : List F) : (positive as).coeff 0 = 0 := by
  simp [positive]

theorem positive_degree (as : List F) : (positive as).natDegree ≤ as.length := by
  cases as with
  | nil => simp [positive, coeffs]
  | cons a as =>
    have h := coeffs_degree (a :: as)
    exact (natDegree_mul_le).trans (by simp only [natDegree_X, List.length_cons] at *; omega)

theorem positive_injective {as bs : List F} (h : positive as = positive bs) :
    coeffs as = coeffs bs := mul_left_cancel₀ X_ne_zero h

theorem eval_positive_reverse (as : List F) (x : F) :
    (positive as.reverse).eval x = as.foldl (fun acc a => (acc + a) * x) 0 := by
  induction as using List.reverseRecOn with
  | nil => simp [positive, coeffs]
  | append_singleton a as ih =>
    simp only [List.reverse_append, List.reverse_cons, List.reverse_nil,
      List.nil_append, List.singleton_append, positive, coeffs, eval_mul,
      eval_X, eval_add, eval_C, List.foldl_append, List.foldl_cons, List.foldl_nil] at *
    rw [← ih]
    ring

theorem difference_nonzero {p q : F[X]} (hp : p.coeff 0 = 0)
    (hq : q.coeff 0 = 0) (hne : p ≠ q) (t : F) : p - q - C t ≠ 0 := by
  intro h
  have ht : t = 0 := by
    have hc := congrArg (fun v : F[X] => v.coeff 0) h
    simpa [hp, hq] using hc
  exact hne (sub_eq_zero.mp (by simpa [ht] using h))

theorem difference_degree (p q : F[X]) (t : F) :
    (p - q - C t).natDegree ≤ max p.natDegree q.natDegree := by
  exact (natDegree_sub_le _ _).trans (max_le (natDegree_sub_le _ _) (by simp))

/-- Root counting on an injectively represented restricted key space. -/
theorem restricted_roots {K : Type*} [Fintype K] [Fintype F] [DecidableEq F]
    (key : K → F) (hk : Function.Injective key) (p : F[X]) (hp : p ≠ 0) :
    (Finset.univ.filter fun k => p.eval (key k) = 0).card ≤ p.natDegree := by
  classical
  calc
    _ ≤ (Finset.univ.filter fun x : F => p.eval x = 0).card := by
      apply Finset.card_le_card_of_injOn key
      · intro k h; simpa using h
      · exact hk.injOn
    _ ≤ p.natDegree := polynomial_zero_count p hp

/-- Union of explicit field targets, each giving a nonzero polynomial. -/
theorem target_bound {K : Type*} [Fintype K] [Fintype F]
    (key : K → F) (hk : Function.Injective key) (p q : F[X])
    (hp : p.coeff 0 = 0) (hq : q.coeff 0 = 0) (hne : p ≠ q)
    (targets : Finset F) (event : K → Prop)
    (cover : ∀ k, event k → p.eval (key k) - q.eval (key k) ∈ targets) :
    uniformProb event ≤
      (targets.card * max p.natDegree q.natDegree : ℕ) / (Fintype.card K : ℚ≥0) := by
  classical
  have hc : (Finset.univ.filter event).card ≤
      targets.card * max p.natDegree q.natDegree := by
    calc
      _ ≤ (targets.biUnion fun t => Finset.univ.filter fun k =>
          (p - q - C t).eval (key k) = 0).card := by
        apply Finset.card_le_card
        intro k hk'
        have he := (Finset.mem_filter.mp hk').2
        apply Finset.mem_biUnion.mpr
        refine ⟨_, cover k he, ?_⟩
        simp
      _ ≤ ∑ t ∈ targets, (Finset.univ.filter fun k =>
          (p - q - C t).eval (key k) = 0).card := Finset.card_biUnion_le
      _ ≤ ∑ _t ∈ targets, max p.natDegree q.natDegree := by
        apply Finset.sum_le_sum
        intro t _
        exact (restricted_roots key hk _ (difference_nonzero hp hq hne t)).trans
          (difference_degree p q t)
      _ = _ := by simp
  unfold uniformProb
  exact div_le_div_of_nonneg_right (by exact_mod_cast hc) (by positivity)

end
end ProvenHashes.Classic
