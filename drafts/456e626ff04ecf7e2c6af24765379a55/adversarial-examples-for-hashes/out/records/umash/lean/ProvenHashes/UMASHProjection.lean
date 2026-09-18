import ProvenHashes.UMASHMaskCertificate
import ProvenHashes.UMASHProbability

namespace ProvenHashes.UMASH

set_option maxRecDepth 8192
set_option maxHeartbeats 2000000
attribute [local irreducible] maskSet

/-- Every actual equality after projection supplies a pair of admissible masks.
Membership alone is not asserted to imply equality for a given pair. -/
theorem project_eq_mask_cover (x y : Chunk) (h : project x = project y) :
    (x.1.toNat ^^^ y.1.toNat, x.2.toNat ^^^ y.2.toNat) ∈ maskSet ×ˢ maskSet := by
  have h1 := congrArg (fun v : Field × Field => v.1.val) h
  have h2 := congrArg (fun v : Field × Field => v.2.val) h
  simp only [project, ZMod.val_natCast] at h1 h2
  rw [Finset.mem_product]
  constructor
  · exact congruent_xor_mem_maskSet _ _ x.1.isLt y.1.isLt h1
  · exact congruent_xor_mem_maskSet _ _ x.2.isLt y.2.isLt h2
-- CHECKPOINT

/-- The exact 852-squared projection transfer on an injective key slice.
The injectivity premise must be proved separately for the PH slice being used. -/
theorem projected_collision_mask_bound {K : Type*} [Fintype K]
    (x y : K → Chunk)
    (hinj : Function.Injective (fun k =>
      ( (x k).1.toNat ^^^ (y k).1.toNat, (x k).2.toNat ^^^ (y k).2.toNat))) :
    uniformProb (fun k => project (x k) = project (y k)) ≤
      (852 : ℚ≥0)^2 / (Fintype.card K : ℚ≥0) := by
  have h := injective_target_probability _ hinj (maskSet ×ˢ maskSet)
    (fun k => project (x k) = project (y k))
    (fun k hk => project_eq_mask_cover (x k) (y k) hk)
  rw [Finset.card_product, maskSet_card] at h
  simpa only [Nat.cast_mul, Nat.cast_ofNat, pow_two] using h
-- CHECKPOINT

end ProvenHashes.UMASH
