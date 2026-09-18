import ProvenHashes.Highway.WordArithmetic
namespace ProvenHashes.Highway
set_option maxRecDepth 10000
set_option maxHeartbeats 0
theorem first_byte5_bound (x c a b : Word) (hx : hi x = 0)
    (hc : c.toNat = 0x3bd39e10cb0ef593) (ha : lo a = 0x10e82046) :
    byte (x+c+zip0 a b) 5 ≠ 255 := by
  have hxN := congrArg BitVec.toNat hx
  simp only [hi_toNat, BitVec.ofNat_eq_ofNat, BitVec.toNat_ofNat] at hxN
  have hxsmall : x.toNat < 4294967296 := by omega
  have haN := congrArg BitVec.toNat ha
  simp only [lo_toNat, BitVec.ofNat_eq_ofNat, BitVec.toNat_ofNat] at haN
  obtain ⟨h0,h1,h2,h3⟩ := nat_lowbytes a.toNat haN
  have hb0 : (byte a 0).toNat = 70 := by
    simpa only [byte_toNat, Nat.mul_zero, Nat.pow_zero, Nat.div_one] using h0
  have hb1 : (byte a 1).toNat = 32 := by rw [byte_toNat]; exact h1
  have hb2 : (byte a 2).toNat = 232 := by rw [byte_toNat]; exact h2
  have hb3 : (byte a 3).toNat = 16 := by rw [byte_toNat]; exact h3
  have hzip : (zip0 a b).toNat = packN 16 (byte b 4).toNat 232
      (byte a 5).toNat (byte b 6).toNat 32 (byte b 7).toNat 70 := by
    rw [zip0_bytes, pack8_toNat, hb0,hb1,hb2,hb3]
    rfl
  have H := nat_first_byte5 x.toNat c.toNat (byte b 4).toNat (byte a 5).toNat
    (byte b 6).toNat (byte b 7).toNat hc hxsmall
    (byte b 4).isLt (byte a 5).isLt (byte b 6).isLt (byte b 7).isLt
  intro he
  have heN := congrArg BitVec.toNat he
  simp only [byte_toNat, BitVec.toNat_add, hzip,
    BitVec.ofNat_eq_ofNat, BitVec.toNat_ofNat] at heN
  apply H
  simp only [byte_toNat]
  norm_num only [Nat.reduceMul, Nat.reducePow, Nat.reduceMod] at heN
  exact heN

end ProvenHashes.Highway
