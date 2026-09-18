import ProvenHashes.Highway.BridgeCoordinates
namespace ProvenHashes.Highway
set_option maxRecDepth 100000
set_option maxHeartbeats 2000000

def assembledKey (f : FreeCoordinates) (a : ArithmeticCoordinates) (z : Byte) : Key :=
  coordinateKey (assembleH f a) (assembleW f a z)
def arithmeticGamma (f : FreeCoordinates) (a : ArithmeticCoordinates) : Nat :=
  firstGamma (f.1.val+65536*a.2.1.val) (freeA5 f).toNat f.2.2.1.val

def padLowHalf (f : FreeCoordinates) (a : ArithmeticCoordinates) (c : Bool) : Half :=
  0xe933c0b9 ^^^ (BitVec.ofNat 32 (highValue f a) - 0xa4093822 - if c then 1 else 0)
def padLowZip (f : FreeCoordinates) : Nat :=
  f.2.1.val/65536 + 256*(freeA4 f).toNat +
    65536*(f.2.1.val/256%256) + 16777216*f.2.2.2.val
/-- Carry into byte 4 of the three-word sum; only the subtraction branch is variable. -/
def outputCarry (f : FreeCoordinates) (a : ArithmeticCoordinates) (c : Bool) : Nat :=
  ((padLowHalf f a c).toNat + 0xb5f18a8c + padLowZip f)/4294967296

theorem output_carry_bound (f : FreeCoordinates) (a : ArithmeticCoordinates) (c : Bool) :
    outputCarry f a c ≤ 2 := by
  have hh := (padLowHalf f a c).isLt
  have hq := f.2.1.isLt
  have hn := (freeA4 f).isLt
  have h5 := f.2.2.2.isLt
  unfold outputCarry padLowZip
  norm_num only at *
  omega

def padTarget (f : FreeCoordinates) (a : ArithmeticCoordinates) (c : Bool) : Fin 4 :=
  ⟨2 + (if 54 ≤ a.2.2.val+arithmeticGamma f a then 1 else 0) - outputCarry f a c, by
    have hc := output_carry_bound f a c
    split_ifs <;> omega⟩

theorem assembled_firstW (f : FreeCoordinates) (a : ArithmeticCoordinates) (z : Byte) :
    firstW (assembledKey f a z) = assembleW f a z := by
  unfold firstW assembledKey
  rw [coordinate_reset]
  exact sub_add_cancel _ _

theorem assembled_U0 (f : FreeCoordinates) (a : ArithmeticCoordinates) (z : Byte) :
    firstU0 (reset (assembledKey f a z)).ab =
      ((0 : Half) ++ assembleH f a) + 0x3bd39e10cb0ef593 +
        zip0 (coordinateA (assembleH f a)) (assembleW f a z) := by
  unfold assembledKey
  rw [coordinate_reset]
  simp only [firstU0, sub_add_cancel]

theorem assembled_gamma (f : FreeCoordinates) (a : ArithmeticCoordinates) (z : Byte) :
    keyGamma (assembledKey f a z) = arithmeticGamma f a := by
  unfold keyGamma
  rw [assembled_firstW]
  unfold assembledKey
  rw [coordinate_reset]
  dsimp only
  rw [(assembled_a_bytes f a).2.2.2.2.2, (assembled_bytes f a z).2.2.2.2.2.2.1,
    append_toNat, (assembled_values f a z).1]
  simp only [BitVec.ofNat_eq_ofNat, BitVec.toNat_ofNat, Nat.zero_mod, Nat.zero_mul, Nat.zero_add]
  rfl

theorem assembled_U0_low (f : FreeCoordinates) (a : ArithmeticCoordinates) (z : Byte) :
    (lo (firstU0 (reset (assembledKey f a z)).ab)).toNat =
      lowTotal f.1.val (freeA5 f).toNat f.2.2.1.val a.2.1.val % 4294967296 := by
  obtain ⟨a0,a1,a2,a3,a4,a5⟩ := assembled_a_bytes f a
  rw [assembled_U0, u0_low_generic _ _ _ a2 a3, (assembled_values f a z).1,
    (assembled_bytes f a z).2.2.2.2.2.2.1, a5]
  rfl

theorem assembled_U0_bytes (f : FreeCoordinates) (a : ArithmeticCoordinates) (z : Byte) :
    (byte (firstU0 (reset (assembledKey f a z)).ab) 2).toNat =
      lowTotal f.1.val (freeA5 f).toNat f.2.2.1.val a.2.1.val / 65536 % 256 ∧
    (byte (firstU0 (reset (assembledKey f a z)).ab) 3).toNat =
      lowTotal f.1.val (freeA5 f).toNat f.2.2.1.val a.2.1.val / 16777216 % 256 := by
  constructor
  · rw [byte_low_half _ (⟨2, by decide⟩ : Fin 4), assembled_U0_low]
    generalize lowTotal f.1.val (freeA5 f).toNat f.2.2.1.val a.2.1.val = t
    norm_num only
    omega
  · rw [byte_low_half _ (⟨3, by decide⟩ : Fin 4), assembled_U0_low]
    generalize lowTotal f.1.val (freeA5 f).toNat f.2.2.1.val a.2.1.val = t
    norm_num only
    omega

theorem assembled_pad_byte (f : FreeCoordinates) (a : ArithmeticCoordinates) (z : Byte) :
    (byte (firstU1 (reset (assembledKey f a z)).ab) 4).toNat =
      ((padPhi z).toNat+105+f.2.1.val%256+
        outputCarry f a (padCarry f.2.1 z))%256 := by
  obtain ⟨hl,hh,w0,w1,w2,w3,w4,w5,w6,w7⟩ := assembled_bytes f a z
  have a4 := (assembled_a_bytes f a).2.2.2.2.1
  have hB : lo (coordinateB (assembleW f a z)) =
      padLowHalf f a (padCarry f.2.1 z) := by
    rw [(coordinateB_parts _).1, hh, hl]
    simp only [padLowHalf, padCarry, decide_eq_true_eq]
  have hP := (coordinateB_parts (assembleW f a z)).2
  rw [w0] at hP
  have hZ : (lo (zip1 (coordinateA (assembleH f a)) (assembleW f a z))).toNat =
      padLowZip f := by
    rw [(zip1_parts _ _).1, a4, w2, w3, w5]
    rfl
  have hc4 : (byte (0xc0acf169b5f18a8c : Word) 4).toNat = 105 := by decide
  have hcL : (lo (0xc0acf169b5f18a8c : Word)).toNat = 0xb5f18a8c := by decide
  unfold assembledKey
  rw [coordinate_reset]
  simp only [firstU1, sub_add_cancel]
  rw [byte4_add_three, hB, hP, hc4, hcL, (zip1_parts _ _).2, w1, hZ]
  rfl

theorem assembled_pad_iff (f : FreeCoordinates) (a : ArithmeticCoordinates) (z : Byte) :
    (byte (firstU1 (reset (assembledKey f a z)).ab) 4).toNat =
      157 + (if 54 ≤ a.2.2.val+arithmeticGamma f a then 1 else 0) ↔
    padCondition f.2.1 (padTarget f a) z := by
  rw [assembled_pad_byte]
  unfold padCondition
  rw [← BitVec.toNat_inj]
  simp only [BitVec.toNat_add, BitVec.toNat_ofNat, padTarget]
  have hc := output_carry_bound f a (padCarry f.2.1 z)
  split_ifs <;> omega

/-- The actual E₂ event is exactly the reduced predicate, with its allowed pad targets. -/
theorem assembled_e2_iff (f : FreeCoordinates) (a : ArithmeticCoordinates) (z : Byte) :
    E2 (assembledKey f a z) ↔
      ArithmeticBytes f.1.val (freeA5 f).toNat f.2.2.1.val a ∧
      padCondition f.2.1 (padTarget f a) z := by
  have hclass : Class (assembledKey f a z) := reducedKey_class _ _
  rw [e2_byte_conditions _ hclass]
  unfold ByteConditions
  dsimp only
  rw [assembled_firstW, assembled_gamma]
  obtain ⟨hl,hh,w0,w1,w2,w3,w4,w5,w6,w7⟩ := assembled_bytes f a z
  rw [w6, w7, (assembled_U0_bytes f a z).1, (assembled_U0_bytes f a z).2,
    assembled_pad_iff]
  unfold ArithmeticBytes arithmeticGamma
  tauto

end ProvenHashes.Highway
