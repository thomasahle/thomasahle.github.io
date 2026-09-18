import ProvenHashes.Highway.Model
namespace ProvenHashes.Highway
set_option maxRecDepth 100000
set_option maxHeartbeats 0
abbrev Byte := BitVec 8
def byte (x : Word) (i : Nat) : Byte := x.extractLsb' (8 * i) 8
def pack8 (x0 x1 x2 x3 x4 x5 x6 x7 : Byte) : Word :=
  x7 ++ x6 ++ x5 ++ x4 ++ x3 ++ x2 ++ x1 ++ x0

theorem append_toNat {m n : Nat} (h : BitVec m) (l : BitVec n) :
    (h ++ l).toNat = h.toNat * 2^n + l.toNat := by
  rw [BitVec.toNat_append, ← Nat.shiftLeft_add_eq_or_of_lt l.isLt, Nat.shiftLeft_eq]

theorem zip0_bytes (a b : Word) : zip0 a b =
    pack8 (byte a 3) (byte b 4) (byte a 2) (byte a 5)
      (byte b 6) (byte a 1) (byte b 7) (byte a 0) := by
  apply BitVec.eq_of_getLsbD_eq
  intro i hi
  unfold zip0 pack8 byte
  simp only [BitVec.getLsbD_append, BitVec.getLsbD_or, BitVec.getLsbD_and,
    BitVec.getLsbD_extractLsb', BitVec.getLsbD_shiftLeft, BitVec.getLsbD_ushiftRight]
  interval_cases i <;>
    simp only [BitVec.getLsbD, BitVec.ofNat_eq_ofNat, BitVec.toNat_ofNat] <;>
    norm_num [Nat.testBit, Nat.shiftRight_eq_div_pow]
  all_goals
    simp only [BitVec.ofNat_eq_ofNat, BitVec.toNat_ofNat]
    norm_num [Nat.testBit, Nat.shiftRight_eq_div_pow] <;> simp [bne]

theorem zip1_bytes (a b : Word) : zip1 a b =
    pack8 (byte b 3) (byte a 4) (byte b 2) (byte b 5)
      (byte b 1) (byte a 6) (byte b 0) (byte a 7) := by
  apply BitVec.eq_of_getLsbD_eq
  intro i hi
  unfold zip1 pack8 byte
  simp only [BitVec.getLsbD_append, BitVec.getLsbD_or, BitVec.getLsbD_and,
    BitVec.getLsbD_extractLsb', BitVec.getLsbD_shiftLeft, BitVec.getLsbD_ushiftRight]
  interval_cases i <;>
    simp only [BitVec.getLsbD, BitVec.ofNat_eq_ofNat, BitVec.toNat_ofNat] <;>
    norm_num [Nat.testBit, Nat.shiftRight_eq_div_pow]
  all_goals
    simp only [BitVec.ofNat_eq_ofNat, BitVec.toNat_ofNat]
    norm_num [Nat.testBit, Nat.shiftRight_eq_div_pow] <;> simp [bne]

theorem byte_toNat (x : Word) (i : Nat) :
    (byte x i).toNat = x.toNat / 2^(8*i) % 256 := by
  change (x.toNat >>> (8*i)) % 256 = _
  rw [Nat.shiftRight_eq_div_pow]

theorem byte_add_one (x : Word) (h : byte x 1 ≠ 255) (i : Fin 8) :
    (byte (x + 256) i).toNat = (byte x i).toNat + if i.val = 1 then 1 else 0 := by
  have hx := x.isLt
  have hn : (byte x 1).toNat ≠ 255 := by
    intro he
    apply h
    apply BitVec.eq_of_toNat_eq
    exact he
  simp only [byte_toNat] at hn ⊢
  simp only [BitVec.toNat_add, BitVec.ofNat_eq_ofNat, BitVec.toNat_ofNat] at *
  fin_cases i <;> norm_num only at * <;>
    simp only [ite_true, ite_false, Nat.div_one, Nat.add_zero] at * <;> omega

theorem pack8_toNat (x0 x1 x2 x3 x4 x5 x6 x7 : Byte) :
    (pack8 x0 x1 x2 x3 x4 x5 x6 x7).toNat =
    x0.toNat + 256*x1.toNat + 65536*x2.toNat + 16777216*x3.toNat +
    4294967296*x4.toNat + 1099511627776*x5.toNat +
    281474976710656*x6.toNat + 72057594037927936*x7.toNat := by
  simp only [pack8, append_toNat]
  norm_num only
  ring

theorem lo_toNat (x : Word) : (lo x).toNat = x.toNat % 4294967296 := rfl

theorem hi_toNat (x : Word) : (hi x).toNat = x.toNat / 4294967296 := by
  have hx := x.isLt
  change (x.toNat >>> 32) % 4294967296 = _
  rw [Nat.shiftRight_eq_div_pow]
  norm_num only at *
  omega


end ProvenHashes.Highway
