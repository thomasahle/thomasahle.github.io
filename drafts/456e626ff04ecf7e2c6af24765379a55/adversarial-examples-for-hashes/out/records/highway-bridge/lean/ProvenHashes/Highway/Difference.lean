import ProvenHashes.Highway.Bytes
import ProvenHashes.Highway.Carry
namespace ProvenHashes.Highway
set_option maxRecDepth 100000
set_option maxHeartbeats 0
theorem byte_add_five_general (x d : Word) (hd : d.toNat = 1099511627776)
    (h : byte x 5 ≠ 255) (i : Fin 8) :
    (byte (x + d) i).toNat = (byte x i).toNat + if i.val = 5 then 1 else 0 := by
  have hn : (byte x 5).toNat ≠ 255 := fun he => h (BitVec.eq_of_toNat_eq he)
  rw [byte_toNat] at hn
  simp only [byte_toNat, BitVec.toNat_add, hd]
  exact nat_byte_add40 x.toNat x.isLt hn i

theorem byte_add_five (x : Word) (h : byte x 5 ≠ 255) (i : Fin 8) :
    (byte (x + 0x10000000000#64) i).toNat =
      (byte x i).toNat + if i.val = 5 then 1 else 0 :=
  byte_add_five_general x _ rfl h i

theorem zip_add_one_general (a b d : Word) (hd : d.toNat = 1099511627776) (h : byte a 1 ≠ 255) :
    zip0 (a + 256) b = zip0 a b + d ∧
      zip1 (a + 256) b = zip1 a b ∧ hi (a + 256) = hi a := by
  have hb (i : Fin 8) := byte_add_one a h i
  have ha := a.isLt
  have hn : (byte a 1).toNat ≠ 255 := fun he => h (BitVec.eq_of_toNat_eq he)
  constructor
  · apply BitVec.eq_of_toNat_eq
    simp only [zip0_bytes, pack8_toNat, BitVec.toNat_add,
      BitVec.ofNat_eq_ofNat, hd]
    have h0 := hb 0; have h1 := hb 1; have h2 := hb 2
    have h3 := hb 3; have h5 := hb 5
    dsimp at h0 h1 h2 h3 h5
    rw [h0, h1, h2, h3, h5]
    clear hb h0 h1 h2 h3 h5
    have b0 := (byte a 0).isLt
    have b1 := (byte a 1).isLt
    have b2 := (byte a 2).isLt
    have b3 := (byte a 3).isLt
    have b4 := (byte b 4).isLt
    have b5 := (byte a 5).isLt
    have b6 := (byte b 6).isLt
    have b7 := (byte b 7).isLt
    have hsmall : (byte a 1).toNat < 255 := by omega
    simpa only [packN, Nat.add_zero] using
      packN_inc5 _ _ _ _ _ _ _ _ b3 b4 b2 b5 b6 hsmall b7 b0
  constructor
  · apply BitVec.eq_of_toNat_eq
    simp only [zip1_bytes, pack8_toNat, BitVec.ofNat_eq_ofNat]
    have h4 := hb 4; have h6 := hb 6; have h7 := hb 7
    dsimp at h4 h6 h7
    rw [h4, h6, h7]
  · apply BitVec.eq_of_toNat_eq
    simp only [hi_toNat, BitVec.toNat_add, BitVec.ofNat_eq_ofNat, BitVec.toNat_ofNat]
    rw [byte_toNat] at hn
    norm_num only at *
    omega

theorem zip_add_five_general (a b d e : Word)
    (hd : d.toNat = 1099511627776) (he : e.toNat = 16777216) (h : byte a 5 ≠ 255) :
    zip0 (a + d) b = zip0 a b + e ∧
      zip1 (a + d) b = zip1 a b := by
  have hb (i : Fin 8) := byte_add_five_general a d hd h i
  have hn : (byte a 5).toNat ≠ 255 := fun he => h (BitVec.eq_of_toNat_eq he)
  constructor
  · apply BitVec.eq_of_toNat_eq
    simp only [zip0_bytes, pack8_toNat, BitVec.toNat_add, he]
    have h0 := hb 0; have h1 := hb 1; have h2 := hb 2
    have h3 := hb 3; have h5 := hb 5
    dsimp at h0 h1 h2 h3 h5
    rw [h0, h1, h2, h3, h5]
    clear hb h0 h1 h2 h3 h5
    have b0 := (byte a 0).isLt; have b1 := (byte a 1).isLt
    have b2 := (byte a 2).isLt; have b3 := (byte a 3).isLt
    have b4 := (byte b 4).isLt; have b5 := (byte a 5).isLt
    have b6 := (byte b 6).isLt; have b7 := (byte b 7).isLt
    have hsmall : (byte a 5).toNat < 255 := by omega
    simpa only [packN, Nat.add_zero] using
      packN_inc3 _ _ _ _ _ _ _ _ b3 b4 b2 hsmall b6 b1 b7 b0
  · apply BitVec.eq_of_toNat_eq
    simp only [zip1_bytes, pack8_toNat]
    have h4 := hb 4; have h6 := hb 6; have h7 := hb 7
    dsimp at h4 h6 h7
    rw [h4, h6, h7]

theorem zip_add_one (a b : Word) (h : byte a 1 ≠ 255) :
    zip0 (a + 256) b = zip0 a b + 0x10000000000#64 ∧
      zip1 (a + 256) b = zip1 a b ∧ hi (a + 256) = hi a :=
  zip_add_one_general a b _ rfl h

theorem zip_add_five (a b : Word) (h : byte a 5 ≠ 255) :
    zip0 (a + 0x10000000000#64) b = zip0 a b + 0x1000000#64 ∧
      zip1 (a + 0x10000000000#64) b = zip1 a b :=
  zip_add_five_general a b _ _ rfl rfl h

end ProvenHashes.Highway
