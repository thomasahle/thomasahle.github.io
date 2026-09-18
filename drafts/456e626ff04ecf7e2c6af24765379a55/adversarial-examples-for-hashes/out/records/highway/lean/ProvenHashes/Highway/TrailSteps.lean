import ProvenHashes.Highway.WordArithmetic
import ProvenHashes.Highway.Algebra
namespace ProvenHashes.Highway
set_option maxRecDepth 10000
set_option maxHeartbeats 2000000

def bump1 (s : PairState) : PairState :=
  {s with a := {s.a with v0 := s.a.v0 + (0x10000000000#64), v1 := s.a.v1 + 256 + (0x1000000#64)}}
def bump2 (s : PairState) : PairState :=
  {s with a := {s.a with v1 := s.a.v1 - 256}}

theorem lane_first (s : Lane) (p : Word) (hz : hi s.v0 = 0)
    (hp : s.mul0 + p = 0) (hb : byte s.v1 1 ≠ 255) :
    laneStep (p+256) s = {laneStep p s with v1 := (laneStep p s).v1 + 256} := by
  have hA : s.v1 + s.mul0 + p = s.v1 := by rw [add_assoc, hp, add_zero]
  have hB : s.v1 + s.mul0 + (p+256) = s.v1 + 256 := by rw [← add_assoc, hA]
  have hh := (zip_add_one s.v1 0 hb).2.2
  simp only [laneStep, hA, hB, hz, hh]
  congr 2

theorem zipper_first (s : PairState) (hb : byte s.a.v1 1 ≠ 255)
    (hc : byte (s.a.v0 + zip0 s.a.v1 s.b.v1) 5 ≠ 255) :
    zipper {s with a := {s.a with v1 := s.a.v1 + 256}} = bump1 (zipper s) := by
  have hz := zip_add_one s.a.v1 s.b.v1 hb
  have hc' := zip_add_five (s.a.v0 + zip0 s.a.v1 s.b.v1)
    (s.b.v0 + zip1 s.a.v1 s.b.v1) hc
  have ha : s.a.v0 + (zip0 s.a.v1 s.b.v1 + (0x10000000000#64)) =
    s.a.v0 + zip0 s.a.v1 s.b.v1 + (0x10000000000#64) := by ac_rfl
  unfold zipper bump1
  dsimp only
  rw [hz.1, hz.2.1, ha, hc'.1, hc'.2]
  refine PairState.ext (Lane.ext rfl ?_ rfl rfl) (Lane.ext rfl rfl rfl rfl)
  ac_rfl

theorem pair_first (s : PairState) (p : Word) (hz : hi s.a.v0 = 0)
    (hp : s.a.mul0 + p = 0) (hb : byte s.a.v1 1 ≠ 255)
    (hc : byte ((laneStep p s.a).v0 +
      zip0 (laneStep p s.a).v1 (laneStep 0 s.b).v1) 5 ≠ 255) :
    pairStep (p+256) 0 s = bump1 (pairStep p 0 s) := by
  have hA : (laneStep p s.a).v1 = s.a.v1 := by
    change s.a.v1 + s.a.mul0 + p = s.a.v1
    rw [add_assoc, hp, add_zero]
  unfold pairStep
  rw [lane_first s.a p hz hp hb]
  apply zipper_first ⟨laneStep p s.a, laneStep 0 s.b⟩
  · simpa only [hA] using hb
  · exact hc

theorem lane_second_general (s : PairState) (p d e : Word)
    (hd : d.toNat = 1099511627776)
    (he : lo (s.a.v1+s.a.mul0+p) - hi s.a.v0 = 256)
    (hc : byte s.a.v0 5 ≠ 255) :
    laneStep (p-512-e) {s.a with v0 := s.a.v0+d, v1 := s.a.v1+256+e} =
      {laneStep p s.a with v0 := (laneStep p s.a).v0 + d, v1 := (laneStep p s.a).v1 - 256} := by
  let v := s.a.v1+s.a.mul0+p
  have hb := e2_safe_byte s.a.v0 v hc he
  have hhi := (sub8_facts v hb).2
  have hv : s.a.v1 + 256 + e + s.a.mul0 + (p-512-e) =
      v - 256 := packet2_adjust _ _ _ _
  have hzero : lo (s.a.v0 + d + s.a.mul1) =
      lo (s.a.v0+s.a.mul1) := by
    rw [show s.a.v0 + d + s.a.mul1 =
      (s.a.v0+s.a.mul1) + d by ac_rfl]
    exact (halves_add40_general _ d hd).1
  have hprod : widen (lo (v-256)) * widen (hi (s.a.v0+d)) =
      widen (lo v) * widen (hi s.a.v0) := by
    rw [lo_sub8, (halves_add40_general _ d hd).2]
    exact product_swap (lo v) (hi s.a.v0) 256 he
  unfold laneStep
  dsimp only
  rw [hv, hprod, hzero, hhi]
  refine Lane.ext ?_ rfl rfl rfl
  ac_rfl

theorem lane_second (s : PairState) (p : Word)
    (he : lo (s.a.v1+s.a.mul0+p) - hi s.a.v0 = 256)
    (hc : byte s.a.v0 5 ≠ 255) :
    laneStep (p-512-(0x1000000#64)) (bump1 s).a =
      {laneStep p s.a with v0 := (laneStep p s.a).v0 + (0x10000000000#64), v1 := (laneStep p s.a).v1 - 256} :=
  lane_second_general s p _ _ rfl he hc

theorem zipper_second_general (s : PairState) (d : Word)
    (hd : d.toNat = 1099511627776) (hb : byte s.a.v1 1 ≠ 0) :
    zipper {s with a := {s.a with v0 := s.a.v0+d, v1 := s.a.v1-256}} = bump2 (zipper s) := by
  have hz := zip_sub_one_general s.a.v1 s.b.v1 d hd hb
  have ha : s.a.v0 + d + (zip0 s.a.v1 s.b.v1 - d) =
      s.a.v0 + zip0 s.a.v1 s.b.v1 := add_cancel_shift _ _ _
  unfold zipper bump2
  dsimp only
  rw [hz.1, hz.2.1, ha]
  refine PairState.ext (Lane.ext rfl ?_ rfl rfl) (Lane.ext rfl rfl rfl rfl)
  exact sub_add_eq_add_sub _ _ _

theorem zipper_second (s : PairState) (hb : byte s.a.v1 1 ≠ 0) :
    zipper {s with a := {s.a with v0 := s.a.v0+(0x10000000000#64), v1 := s.a.v1-256}} = bump2 (zipper s) :=
  zipper_second_general s _ rfl hb

theorem pair_second_general (s : PairState) (p d e : Word)
    (hd : d.toNat = 1099511627776)
    (he : lo (s.a.v1+s.a.mul0+p) - hi s.a.v0 = 256)
    (hc : byte s.a.v0 5 ≠ 255) :
    pairStep (p-512-e) 0 {s with a := {s.a with v0 := s.a.v0+d, v1 := s.a.v1+256+e}} = bump2 (pairStep p 0 s) := by
  unfold pairStep
  rw [lane_second_general s p d e hd he hc]
  apply zipper_second_general ⟨laneStep p s.a, laneStep 0 s.b⟩ d hd
  exact e2_safe_byte s.a.v0 _ hc he

theorem pair_second (s : PairState) (p : Word)
    (he : lo (s.a.v1+s.a.mul0+p) - hi s.a.v0 = 256)
    (hc : byte s.a.v0 5 ≠ 255) :
    pairStep (p-512-(0x1000000#64)) 0 (bump1 s) = bump2 (pairStep p 0 s) :=
  pair_second_general s p _ _ rfl he hc

end ProvenHashes.Highway
