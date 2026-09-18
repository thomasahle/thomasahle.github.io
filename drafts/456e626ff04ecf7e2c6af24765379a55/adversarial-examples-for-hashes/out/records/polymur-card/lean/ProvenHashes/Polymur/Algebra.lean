import ProvenHashes.Polymur.Bytes

noncomputable section

namespace ProvenHashes.Polymur
open Polynomial
attribute [local simp] Polynomial.coeff_monomial

abbrev Words := Fin 7 → F

/-- Expanded nonfinal block; degree zero is deliberately retained. -/
def blockPoly (w : Words) : F[X] :=
  monomial 7 (w 6 + 3) + monomial 6 (w 0) + monomial 5 (w 2) +
  monomial 4 (w 4) + monomial 3 (w 5) + monomial 2 (w 3) + monomial 1 (w 1) +
  C (w 0*w 1 + w 2*w 3 + w 4*w 5)

def shortPoly (l : F) (w : Words) : F[X] :=
  X^3 + monomial 2 (w 0) + monomial 1 l + C (l*w 0)

def midPoly (l : F) (w : Words) : F[X] :=
  X^9 + monomial 7 (w 0) + X^4 + monomial 3 (w 2) + monomial 2 (w 1) +
  monomial 1 l + C (l*w 2 + w 0*w 1)

def longPoly (l : F) (w : Words) : F[X] :=
  X^13 + monomial 11 (w 0) + monomial 9 (w 6+1) +
  monomial 7 (w 0*w 6+w 3) + monomial 6 (w 1) +
  monomial 4 (w 0*w 1+w 5+1) + monomial 3 (w 2) +
  monomial 2 (w 1*w 6+w 4) + monomial 1 l +
  C (l*w 2+w 0*w 1*w 6+w 3*w 4+w 5*w 6)

/-- Four multiplications in the 49-byte loop. -/
theorem block_source (w : Words) : blockPoly w =
    (X + C (w 0)) * (X^6 + C (w 1)) +
    (X^2 + C (w 2)) * (X^5 + C (w 3)) +
    (X^3 + C (w 4)) * (X^4 + C (w 5)) + C (w 6)*X^7 := by
  simp only [blockPoly, ← C_mul_X_pow_eq_monomial, map_add, map_mul, map_ofNat]
  ring

theorem short_source (l : F) (w : Words) :
    shortPoly l w = (X + C (w 0)) * (X^2 + C l) := by
  simp only [shortPoly, ← C_mul_X_pow_eq_monomial, map_mul]
  ring

theorem mid_source (l : F) (w : Words) : midPoly l w =
    (X^2 + C (w 0)) * (X^7 + C (w 1)) +
    (X + C (w 2)) * (X^3 + C l) := by
  simp only [midPoly, ← C_mul_X_pow_eq_monomial, map_add, map_mul]
  ring

theorem long_source (l : F) (w : Words) : longPoly l w =
    (X + C (w 2)) * (X^3 + C l) +
    (X^2 + C (w 3)) * (X^7 + C (w 4)) +
    ((X^2 + C (w 0)) * (X^7 + C (w 1)) + C (w 5)) * (X^4 + C (w 6)) := by
  simp only [longPoly, ← C_mul_X_pow_eq_monomial, map_add, map_mul, map_one]
  ring

lemma block_degree_le (w : Words) : (blockPoly w).natDegree ≤ 7 := by
  apply natDegree_le_iff_coeff_eq_zero.mpr
  intro n hn
  have h0 : n ≠ 0 := by omega
  have h1 : n ≠ 1 := by omega
  have h2 : n ≠ 2 := by omega
  have h3 : n ≠ 3 := by omega
  have h4 : n ≠ 4 := by omega
  have h5 : n ≠ 5 := by omega
  have h6 : n ≠ 6 := by omega
  have h7 : n ≠ 7 := by omega
  simp [blockPoly, coeff_monomial, coeff_C, h0, Ne.symm h1, Ne.symm h2, Ne.symm h3, Ne.symm h4, Ne.symm h5, Ne.symm h6, Ne.symm h7]

lemma short_degree_le (l : F) (w : Words) : (shortPoly l w).natDegree ≤ 3 := by
  apply natDegree_le_iff_coeff_eq_zero.mpr
  intro n hn
  have h0 : n ≠ 0 := by omega
  have h1 : n ≠ 1 := by omega
  have h2 : n ≠ 2 := by omega
  have h3 : n ≠ 3 := by omega
  simp [shortPoly, coeff_monomial, coeff_C, coeff_X_pow, h0, Ne.symm h1, Ne.symm h2, h3]

lemma mid_degree_le (l : F) (w : Words) : (midPoly l w).natDegree ≤ 9 := by
  apply natDegree_le_iff_coeff_eq_zero.mpr
  intro n hn
  have h0 : n ≠ 0 := by omega
  have h1 : n ≠ 1 := by omega
  have h2 : n ≠ 2 := by omega
  have h3 : n ≠ 3 := by omega
  have h4 : n ≠ 4 := by omega
  have h7 : n ≠ 7 := by omega
  have h9 : n ≠ 9 := by omega
  simp [midPoly, coeff_monomial, coeff_C, coeff_X_pow, h0, Ne.symm h1, Ne.symm h2, Ne.symm h3, h4, Ne.symm h7, h9]

lemma long_degree_le (l : F) (w : Words) : (longPoly l w).natDegree ≤ 13 := by
  apply natDegree_le_iff_coeff_eq_zero.mpr
  intro n hn
  have h0 : n ≠ 0 := by omega
  have h1 : n ≠ 1 := by omega
  have h2 : n ≠ 2 := by omega
  have h3 : n ≠ 3 := by omega
  have h4 : n ≠ 4 := by omega
  have h6 : n ≠ 6 := by omega
  have h7 : n ≠ 7 := by omega
  have h9 : n ≠ 9 := by omega
  have h11 : n ≠ 11 := by omega
  have h13 : n ≠ 13 := by omega
  simp [longPoly, coeff_monomial, coeff_C, coeff_X_pow, h0, Ne.symm h1, Ne.symm h2, Ne.symm h3, Ne.symm h4, Ne.symm h6, Ne.symm h7, Ne.symm h9, Ne.symm h11, h13]

lemma short_difference_degree (l l' : F) (w v : Words) :
    (shortPoly l w - shortPoly l' v).natDegree ≤ 2 := by
  apply natDegree_le_iff_coeff_eq_zero.mpr
  intro n hn
  have h0 : n ≠ 0 := by omega
  have h1 : n ≠ 1 := by omega
  have h2 : n ≠ 2 := by omega
  simp [shortPoly, coeff_monomial, coeff_C, coeff_X_pow, h0, Ne.symm h1, Ne.symm h2]

lemma block_decode {w v : Words}
    (h : ∀ i, 1 ≤ i → i ≤ 7 → (blockPoly w).coeff i = (blockPoly v).coeff i) : w = v := by
  have h1 := h 1 (by omega) (by omega)
  have h2 := h 2 (by omega) (by omega)
  have h3 := h 3 (by omega) (by omega)
  have h4 := h 4 (by omega) (by omega)
  have h5 := h 5 (by omega) (by omega)
  have h6 := h 6 (by omega) (by omega)
  have h7 := h 7 (by omega) (by omega)
  simp [blockPoly] at h1 h2 h3 h4 h5 h6 h7
  funext i
  fin_cases i <;> assumption

lemma long_decode {l l' : F} {w v : Words} (h : longPoly l w = longPoly l' v) : w = v := by
  have hc (i : ℕ) := congrArg (fun q : F[X] => q.coeff i) h
  have h0 := hc 11
  have h6 := hc 9
  have h1 := hc 6
  have h2 := hc 3
  simp [longPoly] at h0 h6 h1 h2
  have h3 := hc 7
  have h5 := hc 4
  have h4 := hc 2
  simp [longPoly, h0, h6, h1, h2] at h3 h5 h4
  funext i
  fin_cases i <;> assumption

end ProvenHashes.Polymur
