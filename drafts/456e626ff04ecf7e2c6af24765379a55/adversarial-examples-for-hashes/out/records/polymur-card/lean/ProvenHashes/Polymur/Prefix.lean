import ProvenHashes.Polymur.Algebra

noncomputable section

namespace ProvenHashes.Polymur
open Polynomial
attribute [local simp] Polynomial.coeff_monomial

/-- The first block occupies the highest seven nonconstant coefficients. -/
def prefixPoly : List Words → F[X]
  | [] => 0
  | w :: ws => blockPoly w * X^(7*ws.length) + prefixPoly ws

lemma prefix_degree_le (ws : List Words) : (prefixPoly ws).natDegree ≤ 7*ws.length := by
  induction ws with
  | nil => simp [prefixPoly]
  | cons w ws ih =>
    apply (natDegree_add_le _ _).trans
    apply max_le
    · exact (natDegree_mul_le).trans (by
        have := block_degree_le w
        simp only [natDegree_X_pow, List.length_cons]
        omega)
    · simpa only [List.length_cons] using ih.trans (by omega)

lemma prefix_high (w : Words) (ws : List Words) (i : ℕ) (hi : 1 ≤ i) :
    (prefixPoly (w::ws)).coeff (i+7*ws.length) = (blockPoly w).coeff i := by
  rw [prefixPoly, coeff_add, coeff_mul_X_pow]
  rw [coeff_eq_zero_of_natDegree_lt ((prefix_degree_le ws).trans_lt (by omega)), add_zero]

lemma prefix_degree (ws : List Words) (hg : ∀ w ∈ ws, w 6+3 ≠ 0) :
    (prefixPoly ws).natDegree = 7*ws.length := by
  cases ws with
  | nil => simp [prefixPoly]
  | cons w ws =>
    apply natDegree_eq_of_le_of_coeff_ne_zero (prefix_degree_le _)
    have hh := prefix_high w ws 7 (by omega)
    have he : 7*(w::ws).length = 7+7*ws.length := by simp; omega
    rw [he, hh]
    simpa [blockPoly] using hg w (by simp)

lemma prefix_inj_same_length {a b : List Words} (hl : a.length = b.length)
    (h : prefixPoly a = prefixPoly b) : a = b := by
  induction a generalizing b with
  | nil => simpa using hl.symm
  | cons w ws ih =>
    cases b with
    | nil => simp at hl
    | cons v vs =>
      have hl' : ws.length = vs.length := by simpa using hl
      have hw : w = v := by
        apply block_decode
        intro i hi _
        have hc := congrArg (fun q : F[X] => q.coeff (i+7*ws.length)) h
        dsimp only at hc
        rw [prefix_high w ws i hi, hl', prefix_high v vs i hi] at hc
        exact hc
      subst v
      have he : prefixPoly ws = prefixPoly vs := by
        simpa only [prefixPoly, hl', add_right_inj] using h
      rw [ih hl' he]

lemma prefix_inj {a b : List Words}
    (ha : ∀ w ∈ a, w 6+3 ≠ 0) (hb : ∀ w ∈ b, w 6+3 ≠ 0)
    (h : prefixPoly a = prefixPoly b) : a = b := by
  have hd := congrArg natDegree h
  rw [prefix_degree a ha, prefix_degree b hb] at hd
  exact prefix_inj_same_length (by omega) h

/-- Agreement with the source's block-Horner loop. -/
theorem prefix_fold (ws : List Words) (acc : F[X]) :
    ws.foldl (fun h w => h*X^7 + blockPoly w) acc = acc*X^(7*ws.length) + prefixPoly ws := by
  induction ws generalizing acc with
  | nil => simp [prefixPoly]
  | cons w ws ih =>
    rw [List.foldl_cons, ih]
    simp only [List.length_cons, prefixPoly, Nat.mul_add, Nat.mul_one, pow_add]
    ring

lemma separate_prefix_tail {a b t u : F[X]} (ht : t.natDegree ≤ 13) (hu : u.natDegree ≤ 13)
    (h : a*X^14+t = b*X^14+u) : a = b ∧ t = u := by
  have hab : a = b := by
    ext i
    have hc := congrArg (fun q : F[X] => q.coeff (i+14)) h
    simp only [coeff_add, coeff_mul_X_pow] at hc
    rw [coeff_eq_zero_of_natDegree_lt (ht.trans_lt (by omega)),
      coeff_eq_zero_of_natDegree_lt (hu.trans_lt (by omega))] at hc
    simpa using hc
  exact ⟨hab, by simpa only [hab, add_right_inj] using h⟩

end ProvenHashes.Polymur
