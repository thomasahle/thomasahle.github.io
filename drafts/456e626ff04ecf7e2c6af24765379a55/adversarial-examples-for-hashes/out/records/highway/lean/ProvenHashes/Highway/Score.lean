import ProvenHashes.Highway.ExactBound
import Mathlib.Analysis.SpecialFunctions.Log.Base

namespace ProvenHashes.Highway

set_option maxRecDepth 100000
set_option maxHeartbeats 2000000
set_option exponentiation.threshold 20000

/-- The twelve-word pair's score from its exact trail probability. -/
noncomputable def trailScore : ℝ := Real.logb 2 (12 * (2 : ℝ)^72 / 56165)

/-- Exact integer-power certificates for the decimal logarithm enclosure. -/
theorem score_power_bounds :
    (12 : ℝ)^1000 * 2^12192 < 56165^1000 ∧
    (56165 : ℝ)^1000 < 12^1000 * 2^12193 := by
  constructor <;> norm_num

/-- A checked interval that rounds to 59.81 bits at two decimal places. -/
theorem trail_score_bounds :
    (59807 / 1000 : ℝ) < trailScore ∧ trailScore < (59808 / 1000 : ℝ) := by
  have htwo : Real.logb 2 2 = 1 := Real.logb_self_eq_one (by norm_num)
  have hlog (n : Nat) : Real.logb 2 ((12 : ℝ)^1000 * 2^n) =
      1000 * Real.logb 2 12 + n := by
    rw [Real.logb_mul (by positivity) (by positivity)]
    simp only [Real.logb_pow, htwo, mul_one, Nat.cast_ofNat]
  have hlo := Real.logb_lt_logb (by norm_num : (1 : ℝ) < 2)
    (by positivity) score_power_bounds.2
  have hhi := Real.logb_lt_logb (by norm_num : (1 : ℝ) < 2)
    (by positivity) score_power_bounds.1
  rw [hlog, Real.logb_pow] at hlo hhi
  norm_num only [Nat.cast_ofNat] at hlo hhi
  have hf : trailScore = Real.logb 2 12 + 72 - Real.logb 2 56165 := by
    unfold trailScore
    rw [Real.logb_div (by positivity) (by norm_num),
      Real.logb_mul (by norm_num) (by positivity), Real.logb_pow, htwo]
    norm_num
  rw [hf]
  constructor <;> linarith

/-- The displayed score is computed from the proved actual-key trail probability. -/
theorem trail_score_from_probability :
    Real.logb 2 (12 / (uniformProb Trail : ℝ)) = trailScore := by
  rw [trail_probability]
  norm_num [trailScore]

end ProvenHashes.Highway
