import ProvenHashes.Highway.Carry
namespace ProvenHashes.Highway
set_option maxRecDepth 10000
set_option maxHeartbeats 0

/-- The four subtraction bytes, with all carries retained. -/
theorem nat_e2_equations (t z r w6 w7 g : Nat)
    (ht : t < 256) (hz : z < 256) (hr : r < 256)
    (h6 : w6 < 256) (h7 : w7 < 256) (hg : g ≤ 2) :
    (t + 256*z + 65536*r + 16777216*(190+(16+w6+g)/256) + 0xc3e82146) %
      4294967296 = 0x81d3be10 + 65536*w7 + w6 + g + 256 ↔
    w6+g ≤ 239 ∧ 21 ≤ w7 ∧ t = (202+w6+g)%256 ∧
      r = (235+w7)%256 ∧ z = 157 + if 54 ≤ w6+g then 1 else 0 := by
  have hc : (16+w6+g)/256 = 0 ∨ (16+w6+g)/256 = 1 := by omega
  rcases hc with hc | hc <;> rw [hc]
  all_goals split_ifs <;> omega

end ProvenHashes.Highway
