import ProvenHashes.Polymur.Cardinality
import Mathlib.Analysis.Complex.ExponentialBounds
import Mathlib.Analysis.SpecialFunctions.Log.Base

noncomputable section
namespace ProvenHashes.Polymur

lemma D_pos (n : ℕ) : 0 < D n := by
  unfold D
  split_ifs <;> omega

theorem D_eight : D 8 = 9 := by decide

/-- The exact piecewise integer step in the review, with no dropped constant. -/
theorem D_eight_mul_le (L : ℕ) (hL : 2 ≤ L) : D (8*L) ≤ 9*L := by
  unfold D
  split_ifs <;> omega

lemma D_word_cap (L : ℕ) (hL : 1 ≤ L) : D (8*L) ≤ 9*L := by
  by_cases h : L = 1
  · subst L; decide
  · exact D_eight_mul_le L (by omega)

/-- The proven upper bound, before optional clipping to one. L counts 8-byte words. -/
def epsilon (L : ℕ) : ℝ := (D (8*L) : ℝ) / keyCard

theorem core_collision_bound_words (L : ℕ) (a b : Bytes)
    (ha : a.length ≤ 8*L) (hb : b.length ≤ 8*L) (hne : a ≠ b) :
    ((uniformProb (fun k : Key => core k.val a = core k.val b)) : ℝ) ≤ epsilon L := by
  unfold epsilon
  simpa only [NNRat.cast_div, NNRat.cast_natCast] using
    (NNRat.cast_mono (K := ℝ) (core_collision_bound a b ha hb hne))

def wordScore (L : ℕ) : ℝ := Real.logb 2 ((L : ℝ) / epsilon L)

/-- Infimum over positive integer word caps; the next theorem proves attainment at one. -/
def score : ℝ := sInf (wordScore '' {L : ℕ | 1 ≤ L})

lemma epsilon_pos (L : ℕ) : 0 < epsilon L :=
  div_pos (by exact_mod_cast D_pos (8*L)) (by exact_mod_cast keyCard_pos)

theorem wordScore_lower (L : ℕ) (hL : 1 ≤ L) :
    Real.logb 2 ((keyCard : ℝ)/9) ≤ wordScore L := by
  apply Real.logb_le_logb_of_le (by norm_num) (by
    have := keyCard_pos
    positivity)
  have hk : 0 < (keyCard : ℝ) := by exact_mod_cast keyCard_pos
  have hd : 0 < (D (8*L) : ℝ) := by exact_mod_cast D_pos (8*L)
  have hb : (D (8*L) : ℝ) ≤ 9*(L : ℝ) := by exact_mod_cast D_word_cap L hL
  dsimp [epsilon]
  rw [div_div_eq_mul_div]
  apply (div_le_div_iff₀ (by norm_num) hd).mpr
  nlinarith

theorem wordScore_one : wordScore 1 = Real.logb 2 ((keyCard : ℝ)/9) := by
  simp only [wordScore, epsilon, mul_one, D_eight, Nat.cast_one, Nat.cast_ofNat,
    one_div, inv_div]

theorem score_eq : score = Real.logb 2 ((keyCard : ℝ)/9) := by
  apply IsLeast.csInf_eq
  constructor
  · exact ⟨1, by simp, wordScore_one⟩
  · rintro _ ⟨L, hL, rfl⟩
    exact wordScore_lower L hL

/-- A short rational Taylor certificate, checked in Lean's kernel. -/
lemma exp_fraction_bound : Real.exp (15715/100000 : ℝ) ≤
    (K0 : ℝ)/(9*2^54) := by
  have h := Real.exp_bound' (x := (15715/100000 : ℝ)) (n := 5)
    (by norm_num) (by norm_num) (by norm_num)
  apply h.trans
  norm_num [Finset.sum_range_succ, Nat.factorial, K0]

/-- The decimal target is an exact rational, not a floating-point assertion. -/
theorem K0_log_lower : (54.2267 : ℝ) ≤ Real.logb 2 ((K0 : ℝ)/9) := by
  have hl : (15715/100000 : ℝ) ≤ Real.log ((K0 : ℝ)/(9*2^54)) :=
    (Real.le_log_iff_exp_le (by norm_num [K0])).mpr exp_fraction_bound
  have h2 := Real.log_two_lt_d9
  have he : Real.log ((K0 : ℝ)/(9*2^54)) = Real.log ((K0 : ℝ)/9) - 54*Real.log 2 := by
    rw [show (K0 : ℝ)/(9*2^54) = ((K0 : ℝ)/9)/2^54 by ring]
    rw [Real.log_div (by norm_num [K0]) (by positivity), Real.log_pow]
    norm_num
  rw [he] at hl
  rw [Real.logb, le_div_iff₀ (Real.log_pos (by norm_num : (1 : ℝ) < 2))]
  linarith

theorem score_lower (hK : CardinalityCertificate) : (54.2267 : ℝ) ≤ score := by
  rw [score_eq]
  apply K0_log_lower.trans
  apply Real.logb_le_logb_of_le (by norm_num) (by norm_num [K0])
  exact div_le_div_of_nonneg_right (by exact_mod_cast hK) (by norm_num)

end ProvenHashes.Polymur
