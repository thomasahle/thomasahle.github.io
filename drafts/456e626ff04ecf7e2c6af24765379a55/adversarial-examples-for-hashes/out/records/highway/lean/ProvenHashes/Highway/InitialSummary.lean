import ProvenHashes.Highway.ByteEquations
import ProvenHashes.Highway.InitialBound
namespace ProvenHashes.Highway
set_option maxRecDepth 10000
set_option maxHeartbeats 0

def firstGamma (n b1 w4 : Nat) : Nat :=
  (n + 0xcb0ef593 + 16 + 256*w4 + 65536*232 + 16777216*b1) / 4294967296

/-- Exact high half and carry of U₀; all variables are ordinary integers. -/
theorem nat_initial_summary (n w4 b1 w6 w7 : Nat)
    (hn : n < 4294967296) (h4 : w4 < 256) (hb : b1 < 256)
    (h6 : w6 < 256) (h7 : w7 < 256) :
    let u := ((n+0x3bd39e10cb0ef593)%18446744073709551616 +
      packN 16 w4 232 b1 w6 32 w7 70)%18446744073709551616
    firstGamma n b1 w4 ≤ 2 ∧
    u/4294967296 = 0x81d3be10+65536*w7+w6+firstGamma n b1 w4 ∧
    u/1099511627776%256 = 190+(16+w6+firstGamma n b1 w4)/256 := by
  dsimp only
  unfold firstGamma packN
  omega

theorem nat_pack_low (x0 x1 x2 x3 x4 x5 x6 x7 : Nat)
    (h0 : x0 < 256) (h1 : x1 < 256) (h2 : x2 < 256) (h3 : x3 < 256) :
    packN x0 x1 x2 x3 x4 x5 x6 x7 % 4294967296 =
      x0+256*x1+65536*x2+16777216*x3 := by
  unfold packN
  omega

theorem lo_zip0_toNat (a b : Word) :
    (lo (zip0 a b)).toNat = (byte a 3).toNat + 256*(byte b 4).toNat +
      65536*(byte a 2).toNat + 16777216*(byte a 5).toNat := by
  rw [lo_toNat, zip0_bytes, pack8_toNat]
  exact nat_pack_low _ _ _ _ _ _ _ _ (byte a 3).isLt (byte b 4).isLt
    (byte a 2).isLt (byte a 5).isLt

theorem lo_add (a b : Word) : lo (a+b) = lo a + lo b := by
  apply BitVec.eq_of_toNat_eq
  simp only [lo_toNat, BitVec.toNat_add]
  omega

theorem initial_summary (x c a b : Word) (hx : hi x = 0)
    (hc : c.toNat = 0x3bd39e10cb0ef593) (ha : lo a = 0x10e82046) :
    let g := firstGamma x.toNat (byte a 5).toNat (byte b 4).toNat
    g ≤ 2 ∧
    (hi (x+c+zip0 a b)).toNat = 0x81d3be10+65536*(byte b 7).toNat+(byte b 6).toNat+g ∧
    (byte (x+c+zip0 a b) 5).toNat = 190+(16+(byte b 6).toNat+g)/256 := by
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
  have hU : (x+c+zip0 a b).toNat =
      ((x.toNat+0x3bd39e10cb0ef593)%18446744073709551616 +
        packN 16 (byte b 4).toNat 232 (byte a 5).toNat (byte b 6).toNat 32 (byte b 7).toNat 70)%
        18446744073709551616 := by
    simp only [BitVec.toNat_add, hc, hzip]
  dsimp only
  rw [hi_toNat, byte_toNat (x+c+zip0 a b) 5, hU]
  exact nat_initial_summary x.toNat (byte b 4).toNat (byte a 5).toNat
    (byte b 6).toNat (byte b 7).toNat hxsmall (byte b 4).isLt (byte a 5).isLt
    (byte b 6).isLt (byte b 7).isLt

end ProvenHashes.Highway
