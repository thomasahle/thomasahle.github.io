import ProvenHashes.Highway.BridgeCount

namespace ProvenHashes.Highway

/-- Exact probability of the trail over uniformly sampled 256-bit keys. -/
theorem trail_probability : uniformProb Trail = 56165 / (2 : ℚ≥0)^72 :=
  trail_probability_from_count exact_trail_count

/-- Unconditional lower bound for the actual complete-state collision event. -/
theorem state_collision_bound :
    56165 / (2 : ℚ≥0)^72 ≤ uniformProb StateCollision :=
  state_collision_bound_from_count exact_trail_count

/-- Unconditional bounds for the three actual HighwayHash finalizers. -/
theorem output_collision_bounds :
    56165 / (2 : ℚ≥0)^72 ≤ uniformProb OutputCollision64 ∧
    56165 / (2 : ℚ≥0)^72 ≤ uniformProb OutputCollision128 ∧
    56165 / (2 : ℚ≥0)^72 ≤ uniformProb OutputCollision256 :=
  output_collision_bounds_from_count exact_trail_count

theorem output_collision64_bound :
    56165 / (2 : ℚ≥0)^72 ≤ uniformProb OutputCollision64 :=
  output_collision_bounds.1

theorem output_collision128_bound :
    56165 / (2 : ℚ≥0)^72 ≤ uniformProb OutputCollision128 :=
  output_collision_bounds.2.1

theorem output_collision256_bound :
    56165 / (2 : ℚ≥0)^72 ≤ uniformProb OutputCollision256 :=
  output_collision_bounds.2.2

end ProvenHashes.Highway
