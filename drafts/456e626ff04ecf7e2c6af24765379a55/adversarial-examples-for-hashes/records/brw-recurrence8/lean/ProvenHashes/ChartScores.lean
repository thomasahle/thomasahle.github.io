import ProvenHashes.EightLanes

namespace ProvenHashes.ChartScores
noncomputable section

/-- Scores use the published upper bounds, with L measured in field words. -/
def brwRatio (L : ℕ) : ℝ := (L : ℝ) * 2^64 / (2*(L : ℝ)-1)
def laneRatio (L : ℕ) : ℝ := (L : ℝ) * 2^64 / (EightLanes.rounds L + 7)
def brwScore (L : ℕ) : ℝ := Real.logb 2 (brwRatio L)
def laneScore (L : ℕ) : ℝ := Real.logb 2 (laneRatio L)
def maxLength : ℕ := 2^61-1

/-- At finite positive length the BRW score is strictly above 63 bits. -/
theorem brw_ratio_gt (L : ℕ) (hL : 1 ≤ L) : (2 : ℝ)^63 < brwRatio L := by
  have hl : (1 : ℝ) ≤ L := by exact_mod_cast hL
  unfold brwRatio
  apply (lt_div_iff₀ (by linarith : (0 : ℝ) < 2*(L : ℝ)-1)).2
  norm_num
  linarith

/-- The finite-length BRW minimum is attained at the largest permitted length. -/
theorem brw_ratio_antitone {L M : ℕ} (hL : 1 ≤ L) (hLM : L ≤ M) :
    brwRatio M ≤ brwRatio L := by
  have hl : (1 : ℝ) ≤ L := by exact_mod_cast hL
  have hm : (L : ℝ) ≤ M := by exact_mod_cast hLM
  unfold brwRatio
  apply (div_le_div_iff₀ (by linarith : (0 : ℝ) < 2*(M : ℝ)-1)
    (by linarith : (0 : ℝ) < 2*(L : ℝ)-1)).2
  nlinarith

/-- The exact finite-range minimum; its integer chart label is checked below. -/
theorem brw_minimum (L : ℕ) (hL : 1 ≤ L) (hmax : L ≤ maxLength) :
    brwScore maxLength ≤ brwScore L := by
  apply Real.logb_le_logb_of_le (by norm_num : (1 : ℝ) < 2)
    (lt_trans (by positivity : (0 : ℝ) < 2^63) (brw_ratio_gt maxLength (by norm_num [maxLength])))
  exact brw_ratio_antitone hL hmax

/-- 63 is the integer chart score; the exact finite minimum lies in (63,64). -/
theorem brw_minimum_bracket : 63 < brwScore maxLength ∧ brwScore maxLength < 64 := by
  have hlo := Real.logb_lt_logb (by norm_num : (1 : ℝ) < 2)
    (by positivity : (0 : ℝ) < 2^63) (brw_ratio_gt maxLength (by norm_num [maxLength]))
  have hhi : brwRatio maxLength < (2 : ℝ)^64 := by norm_num [brwRatio, maxLength]
  have hp : (0 : ℝ) < brwRatio maxLength :=
    lt_trans (by positivity : (0 : ℝ) < 2^63) (brw_ratio_gt maxLength (by norm_num [maxLength]))
  have hh := Real.logb_lt_logb (by norm_num : (1 : ℝ) < 2) hp hhi
  rw [Real.logb_pow] at hlo hh
  norm_num [Real.logb_self_eq_one, brwScore] at hlo hh ⊢
  exact ⟨hlo, hh⟩

/-- The lane ratio is always at least 2^61, even for an incomplete padded round. -/
theorem lane_ratio_ge (L : ℕ) (hL : 1 ≤ L) : (2 : ℝ)^61 ≤ laneRatio L := by
  have hn : EightLanes.rounds L + 7 ≤ 8*L := by unfold EightLanes.rounds; omega
  have hr : (EightLanes.rounds L : ℝ) + 7 ≤ 8*(L : ℝ) := by exact_mod_cast hn
  unfold laneRatio
  apply (le_div_iff₀ (by positivity : (0 : ℝ) < EightLanes.rounds L + 7)).2
  norm_num
  linarith

/-- The eight-lane chart score has exact global minimum 61, attained at L=1. -/
theorem lane_minimum : laneScore 1 = 61 ∧ ∀ L : ℕ, 1 ≤ L → 61 ≤ laneScore L := by
  have hpow : Real.logb 2 ((2 : ℝ)^61) = 61 := by
    rw [Real.logb_pow]
    norm_num [Real.logb_self_eq_one]
  constructor
  · have he : laneRatio 1 = (2 : ℝ)^61 := by norm_num [laneRatio, EightLanes.rounds]
    rw [laneScore, he, hpow]
  · intro L hL
    rw [← hpow]
    exact Real.logb_le_logb_of_le (by norm_num) (by positivity) (lane_ratio_ge L hL)

/-- The displayed BRW chart integer, without falsely asserting exact equality. -/
theorem brw_whole_bits : ⌊brwScore maxLength⌋ = (63 : ℤ) := by
  apply Int.floor_eq_iff.mpr
  constructor
  · exact le_of_lt brw_minimum_bracket.1
  · convert brw_minimum_bracket.2 using 1; norm_num

/-- The ratios above are exactly L divided by the proved collision-bound formulas. -/
theorem score_formulas (L : ℕ) (hL : 1 ≤ L) :
    brwScore L = Real.logb 2 ((L : ℝ) / (((2*L-1 : ℕ) : ℝ) / 2^64)) ∧
    laneScore L = Real.logb 2 ((L : ℝ) / (((EightLanes.rounds L+7 : ℕ) : ℝ) / 2^64)) := by
  have he : ((2*L-1 : ℕ) : ℝ) = 2*(L : ℝ)-1 := by
    rw [Nat.cast_sub (by omega), Nat.cast_mul, Nat.cast_ofNat, Nat.cast_one]
  constructor <;> simp [brwScore, brwRatio, laneScore, laneRatio, he, div_div_eq_mul_div]

/-- Flooring the published bounds at the ideal-output probability changes neither. -/
theorem probability_floor (L : ℕ) (hL : 1 ≤ L) :
    max (1 / (2 : ℝ)^64) (((2*L-1 : ℕ) : ℝ) / 2^64) =
      ((2*L-1 : ℕ) : ℝ) / 2^64 ∧
    max (1 / (2 : ℝ)^64) (((EightLanes.rounds L+7 : ℕ) : ℝ) / 2^64) =
      ((EightLanes.rounds L+7 : ℕ) : ℝ) / 2^64 := by
  constructor <;> apply max_eq_right <;>
    apply div_le_div_of_nonneg_right _ (by positivity)
  · exact_mod_cast (show 1 ≤ 2*L-1 by omega)
  · exact_mod_cast (show 1 ≤ EightLanes.rounds L+7 by omega)

/-- Exact correction to the finite-range chart minimum, retaining its positive gap. -/
theorem brw_minimum_exact :
    brwScore maxLength =
      63 + Real.logb 2 (((2 : ℝ)^62 - 2) / ((2 : ℝ)^62 - 3)) := by
  have he : brwRatio maxLength =
      (2 : ℝ)^63 * (((2 : ℝ)^62 - 2) / ((2 : ℝ)^62 - 3)) := by
    norm_num [brwRatio, maxLength]
  rw [brwScore, he, Real.logb_mul (by norm_num) (by norm_num), Real.logb_pow]
  norm_num [Real.logb_self_eq_one]

end
end ProvenHashes.ChartScores
