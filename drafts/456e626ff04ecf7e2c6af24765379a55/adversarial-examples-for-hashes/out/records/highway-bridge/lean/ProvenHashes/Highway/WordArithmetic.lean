import ProvenHashes.Highway.Difference
namespace ProvenHashes.Highway
set_option maxRecDepth 100000
set_option maxHeartbeats 0

theorem halves_add40_general (x d : Word) (hd : d.toNat = 1099511627776) :
    lo (x+d) = lo x ∧ hi (x+d) = hi x + 256 := by
  have H := nat_halves_add40 x.toNat x.isLt
  constructor <;> apply BitVec.eq_of_toNat_eq
  · simpa only [lo_toNat, BitVec.toNat_add, hd] using H.1
  · simpa only [hi_toNat, BitVec.toNat_add, hd, BitVec.ofNat_eq_ofNat,
      BitVec.toNat_ofNat] using H.2

theorem halves_add40 (x : Word) :
    lo (x + 0x10000000000#64) = lo x ∧
    hi (x + 0x10000000000#64) = hi x + 256 := halves_add40_general x _ rfl

theorem sub8_facts (x : Word) (h : byte x 1 ≠ 0) :
    byte (x-256) 1 ≠ 255 ∧ hi (x-256) = hi x := by
  have hn : (byte x 1).toNat ≠ 0 := fun he => h (BitVec.eq_of_toNat_eq he)
  rw [byte_toNat] at hn
  have H := nat_sub8 x.toNat x.isLt hn
  constructor
  · intro he
    have heN := congrArg BitVec.toNat he
    apply H.1
    simpa only [byte_toNat, BitVec.toNat_sub, BitVec.ofNat_eq_ofNat,
      BitVec.toNat_ofNat] using heN
  · apply BitVec.eq_of_toNat_eq
    simpa only [hi_toNat, BitVec.toNat_sub, BitVec.ofNat_eq_ofNat,
      BitVec.toNat_ofNat] using H.2

theorem lo_sub8 (x : Word) : lo (x-256) = lo x - 256 := by
  apply BitVec.eq_of_toNat_eq
  simpa only [lo_toNat, BitVec.toNat_sub, BitVec.ofNat_eq_ofNat,
    BitVec.toNat_ofNat] using nat_lo_sub8 x.toNat x.isLt

theorem zip_sub_one_general (a b d : Word) (hd : d.toNat = 1099511627776)
    (h : byte a 1 ≠ 0) :
    zip0 (a-256) b = zip0 a b - d ∧
    zip1 (a-256) b = zip1 a b ∧ hi (a-256) = hi a := by
  have H := zip_add_one_general (a-256) b d hd (sub8_facts a h).1
  have ha : a-256+256 = a := by ring
  rw [ha] at H
  refine ⟨?_, H.2.1.symm, (sub8_facts a h).2⟩
  exact eq_sub_of_add_eq H.1.symm

theorem zip_sub_one (a b : Word) (h : byte a 1 ≠ 0) :
    zip0 (a-256) b = zip0 a b - 0x10000000000#64 ∧
    zip1 (a-256) b = zip1 a b ∧ hi (a-256) = hi a :=
  zip_sub_one_general a b _ rfl h

theorem e2_safe_byte (u v : Word) (hb : byte u 5 ≠ 255) (he : lo v - hi u = 256) :
    byte v 1 ≠ 0 := by
  have hn : (byte u 5).toNat ≠ 255 := fun h => hb (BitVec.eq_of_toNat_eq h)
  rw [byte_toNat] at hn
  have he' : lo v = hi u + 256 := by rw [← he]; ring
  have heN := congrArg BitVec.toNat he'
  simp only [lo_toNat, hi_toNat, BitVec.toNat_add, BitVec.ofNat_eq_ofNat,
    BitVec.toNat_ofNat] at heN
  have H := nat_e2_byte u.toNat v.toNat u.isLt v.isLt hn heN
  intro h
  have hh := congrArg BitVec.toNat h
  apply H
  simpa only [byte_toNat, BitVec.toNat_ofNat, BitVec.ofNat_eq_ofNat] using hh


end ProvenHashes.Highway
