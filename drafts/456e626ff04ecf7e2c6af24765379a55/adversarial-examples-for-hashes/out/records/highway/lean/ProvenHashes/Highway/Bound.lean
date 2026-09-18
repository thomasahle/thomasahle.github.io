import ProvenHashes.Highway.Trail
import ProvenHashes.Highway.KeySpace
import ProvenHashes.Composition
namespace ProvenHashes.Highway

def OutputCollision64 (k : Key) : Prop :=
  finalize64 (absorb k messageA) = finalize64 (absorb k messageB)
def OutputCollision128 (k : Key) : Prop :=
  finalize128 (absorb k messageA) = finalize128 (absorb k messageB)
def OutputCollision256 (k : Key) : Prop :=
  finalize256 (absorb k messageA) = finalize256 (absorb k messageB)

theorem state_probability_ge_trail :
    uniformProb Trail ≤ uniformProb StateCollision :=
  uniformProb_mono (fun k h => trail_state_collision k h.1 h.2)

theorem output_probabilities_ge_trail :
    uniformProb Trail ≤ uniformProb OutputCollision64 ∧
    uniformProb Trail ≤ uniformProb OutputCollision128 ∧
    uniformProb Trail ≤ uniformProb OutputCollision256 :=
  ⟨uniformProb_mono (fun k h => (trail_outputs k h).1),
   uniformProb_mono (fun k h => (trail_outputs k h).2.1),
   uniformProb_mono (fun k h => (trail_outputs k h).2.2)⟩

/-- Full-key count proposition, proved by `exact_trail_count` in `BridgeCount`. -/
noncomputable def ExactTrailCount : Prop := by
  classical
  exact (Finset.univ.filter Trail).card = 56165 * 2^184

theorem trail_probability_from_count (hcount : ExactTrailCount) :
    uniformProb Trail = 56165 / (2 : ℚ≥0)^72 := by
  classical
  unfold uniformProb
  change (Finset.univ.filter Trail).card = 56165 * 2^184 at hcount
  rw [hcount, card_key]
  norm_num

/-- The count-to-bound implication; `ExactBound` supplies its unconditional specialization. -/
theorem state_collision_bound_from_count (hcount : ExactTrailCount) :
    56165 / (2 : ℚ≥0)^72 ≤ uniformProb StateCollision := by
  rw [← trail_probability_from_count hcount]
  exact state_probability_ge_trail

theorem output_collision_bounds_from_count (hcount : ExactTrailCount) :
    56165 / (2 : ℚ≥0)^72 ≤ uniformProb OutputCollision64 ∧
    56165 / (2 : ℚ≥0)^72 ≤ uniformProb OutputCollision128 ∧
    56165 / (2 : ℚ≥0)^72 ≤ uniformProb OutputCollision256 := by
  rw [← trail_probability_from_count hcount]
  exact output_probabilities_ge_trail

end ProvenHashes.Highway
