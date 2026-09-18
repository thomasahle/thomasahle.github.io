import ProvenHashes.OddMultiplyShift

namespace ProvenHashes

/-- The output-width restriction cannot be dropped: full four-bit output on
two-bit inputs 0 and 2 collides with probability 1/8, not 1/16. The two allowed
multipliers are obtained from the modular equation, without enumerating keys. -/
theorem multiplyAddShift_width_counterexample :
    uniformProb (fun k : ScalarMASKey 2 =>
      multiplyAddShift 2 4 k 0 = multiplyAddShift 2 4 k 2) = 1 / 8 := by
  classical
  have he (k : ScalarMASKey 2) :
      multiplyAddShift 2 4 k 0 = multiplyAddShift 2 4 k 2 ↔ k.1 * 2 = 0 := by
    simp [multiplyAddShift, highWord, eq_comm]
  simp_rw [he]
  change uniformProb (fun k : ZMod 16 × ZMod 16 => k.1 * 2 = 0) = 1 / 8
  rw [uniformProb_prod_fst (J := ZMod 16) (fun a : ZMod 16 => a * 2 = 0)]
  have hf : (Finset.univ.filter (fun a : ZMod 16 => a * 2 = 0)) = {0, 8} := by
    ext a
    simp only [Finset.mem_filter, Finset.mem_univ, true_and,
      Finset.mem_insert, Finset.mem_singleton]
    constructor
    · intro h
      have hv := congrArg ZMod.val h
      rw [ZMod.val_mul] at hv
      change (a.val * 2) % 16 = 0 at hv
      have ha := ZMod.val_lt a
      have hcases : a.val = 0 ∨ a.val = 8 := by omega
      rcases hcases with h0 | h8
      · left; rw [← ZMod.natCast_zmod_val a, h0]; rfl
      · right; rw [← ZMod.natCast_zmod_val a, h8]; rfl
    · rintro (rfl | rfl) <;> rfl
  have hcard : (Finset.univ.filter (fun a : ZMod 16 => a * 2 = 0)).card = 2 := by
    rw [hf]
    have h08 : (0 : ZMod 16) ≠ 8 := by
      intro h
      have hv := congrArg ZMod.val h
      change (0 : ℕ) = 8 at hv
      omega
    simp [h08]
  unfold uniformProb
  convert (show (2 : ℚ≥0) / 16 = 1 / 8 by norm_num) using 1
  congr 1
  · apply congrArg Nat.cast
    convert hcard using 1
    congr 1
    ext a
    simp

end ProvenHashes
