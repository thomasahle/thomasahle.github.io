import ProvenHashes.Highway.InitialSummary
namespace ProvenHashes.Highway
set_option maxRecDepth 10000
set_option maxHeartbeats 1000000

/-- The machine-word E₂ test is exactly the four byte conditions from the argument. -/
theorem word_e2_equations (a c u v : Word) (w6 w7 : Byte) (g : Nat)
    (ha : (lo a).toNat = 0x10e82046) (hc : (lo c).toNat = 0xb3000100)
    (hg : g ≤ 2)
    (hh : (hi u).toNat = 0x81d3be10+65536*w7.toNat+w6.toNat+g)
    (hb : (byte u 5).toNat = 190+(16+w6.toNat+g)/256) :
    lo (a+zip0 u v+c)-hi u = 256 ↔
      w6.toNat+g ≤ 239 ∧ 21 ≤ w7.toNat ∧
      (byte u 3).toNat = (202+w6.toNat+g)%256 ∧
      (byte u 2).toNat = (235+w7.toNat)%256 ∧
      (byte v 4).toNat = 157 + if 54 ≤ w6.toNat+g then 1 else 0 := by
  rw [sub_eq_iff_eq_add, add_comm (256 : Half), lo_add, lo_add, ← BitVec.toNat_inj]
  simp only [BitVec.toNat_add, ha, hc, hh, lo_zip0_toNat, hb,
    BitVec.ofNat_eq_ofNat, BitVec.toNat_ofNat]
  have H := nat_e2_equations (byte u 3).toNat (byte v 4).toNat (byte u 2).toNat
    w6.toNat w7.toNat g (byte u 3).isLt (byte v 4).isLt (byte u 2).isLt
    w6.isLt w7.isLt hg
  have h6 := w6.isLt
  have h7 := w7.isLt
  have heq :
      ((283648070 + ((byte u 3).toNat + 256*(byte v 4).toNat +
        65536*(byte u 2).toNat + 16777216*(190+(16+w6.toNat+g)/256))) % 2^32 +
        3003121920) % 2^32 =
        (0x81d3be10+65536*w7.toNat+w6.toNat+g+256%2^32)%2^32 ↔
      ((byte u 3).toNat + 256*(byte v 4).toNat + 65536*(byte u 2).toNat +
        16777216*(190+(16+w6.toNat+g)/256) + 0xc3e82146)%4294967296 =
        0x81d3be10+65536*w7.toNat+w6.toNat+g+256 := by omega
  exact heq.trans H

end ProvenHashes.Highway
