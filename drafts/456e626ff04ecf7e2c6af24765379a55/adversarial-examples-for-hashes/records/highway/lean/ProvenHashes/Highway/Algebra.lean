import ProvenHashes.Highway.Model
namespace ProvenHashes.Highway

/-- The lane-0 product collision is an operand swap. This proof is algebraic
and covers modular wrap of either 32-bit operand. -/
theorem product_swap (x h d : Half) (e : x - h = d) :
    widen (x - d) * widen (h + d) = widen x * widen h := by
  have hx : x - d = h := by rw [← e]; ring
  have hh : h + d = x := by rw [← e]; ring
  rw [hx, hh, mul_comm]

theorem packet_differences :
    (0x24192a2a01b332d1 : Word) = 0x24192a2a01b331d1 + 256 ∧
    (0x24192a2ab3b330d1 : Word) = 0x24192a2ab4b332d1 - 512 - 0x1000000 := by
  decide

theorem first_injection :
    (0xdbe6d5d5fe4cce2f : Word) + 0x24192a2a01b331d1 = 0 ∧
    (0xdbe6d5d5fe4cce2f : Word) + 0x24192a2ab4b332d1 = 0xb3000100 := by
  decide

theorem third_packet_cancellation (s : PairState) :
    pairStep 256 0 {s with a := {s.a with v1 := s.a.v1 - 256}} = pairStep 0 0 s := by
  unfold pairStep laneStep
  have h : s.a.v1 - 256 + s.a.mul0 + 256 = s.a.v1 + s.a.mul0 + 0 := by ring
  rw [h]

theorem packet2_adjust (a m p d : Word) :
    a + 256 + d + m + (p - 512 - d) = a + m + p - 256 := by ring

theorem add_cancel_shift (a z d : Word) : a + d + (z - d) = a + z := by abel

end ProvenHashes.Highway
