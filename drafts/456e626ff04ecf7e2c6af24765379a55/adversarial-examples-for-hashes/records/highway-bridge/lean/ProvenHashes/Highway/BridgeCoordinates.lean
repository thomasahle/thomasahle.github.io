import ProvenHashes.Highway.BridgeWords
namespace ProvenHashes.Highway
set_option maxRecDepth 10000
set_option maxHeartbeats 2000000

/-- The 56 free bits: low 16 reset bits, W bytes 1--3, byte 4, byte 5. -/
abbrev FreeCoordinates := Fin 65536 × Fin (2^24) × Fin 256 × Fin 256

def assembleH (f : FreeCoordinates) (a : ArithmeticCoordinates) : Half :=
  BitVec.ofNat 32 (f.1.val + 65536*a.2.1.val)
def highValue (f : FreeCoordinates) (a : ArithmeticCoordinates) : Nat :=
  f.2.2.1.val + 256*f.2.2.2.val + 65536*a.2.2.val + 16777216*a.1.val
def assembleW (f : FreeCoordinates) (a : ArithmeticCoordinates) (z : Byte) : Word :=
  BitVec.ofNat 64 (f.2.1.val*256+z.toNat+4294967296*highValue f a)

theorem assembled_values (f : FreeCoordinates) (a : ArithmeticCoordinates) (z : Byte) :
    (assembleH f a).toNat = f.1.val + 65536*a.2.1.val ∧
    (assembleW f a z).toNat = f.2.1.val*256+z.toNat+4294967296*highValue f a := by
  have hn := f.1.isLt
  have hq := f.2.1.isLt
  have h4 := f.2.2.1.isLt
  have h5 := f.2.2.2.isLt
  have h7 := a.1.isLt
  have hx := a.2.1.isLt
  have h6 := a.2.2.isLt
  have hz := z.isLt
  simp only [assembleH, assembleW, highValue, BitVec.toNat_ofNat]
  norm_num only at *
  constructor <;> omega

def disassemble (p : Half × Word) : FreeCoordinates × RelevantCoordinates :=
  ((⟨p.1.toNat%65536, Nat.mod_lt _ (by decide)⟩,
    ⟨p.2.toNat/256%16777216, Nat.mod_lt _ (by decide)⟩,
    ⟨p.2.toNat/4294967296%256, Nat.mod_lt _ (by decide)⟩,
    ⟨p.2.toNat/1099511627776%256, Nat.mod_lt _ (by decide)⟩),
   ((⟨p.2.toNat/72057594037927936, by have h := p.2.isLt; omega⟩,
     ⟨p.1.toNat/65536, by have h := p.1.isLt; omega⟩,
     ⟨p.2.toNat/281474976710656%256, Nat.mod_lt _ (by decide)⟩),
    BitVec.ofNat 8 p.2.toNat))

/-- Complete 96-bit reindexing, retaining every free coordinate. -/
def byteCoordinates : (Half × Word) ≃ (FreeCoordinates × RelevantCoordinates) where
  toFun := disassemble
  invFun p := (assembleH p.1 p.2.1, assembleW p.1 p.2.1 p.2.2)
  left_inv p := by
    have hh := p.1.isLt
    have hw := p.2.isLt
    apply Prod.ext <;> apply BitVec.eq_of_toNat_eq
    · rw [(assembled_values (disassemble p).1 (disassemble p).2.1 (disassemble p).2.2).1]
      dsimp [disassemble]
      omega
    · rw [(assembled_values (disassemble p).1 (disassemble p).2.1 (disassemble p).2.2).2]
      dsimp [disassemble, highValue]
      simp only [BitVec.toNat_ofNat]
      norm_num only at *
      omega
  right_inv p := by
    rcases p with ⟨⟨n,q,w4,w5⟩,⟨⟨w7,x,w6⟩,z⟩⟩
    have hn := n.isLt
    have hq := q.isLt
    have h4 := w4.isLt
    have h5 := w5.isLt
    have h7 := w7.isLt
    have hx := x.isLt
    have h6 := w6.isLt
    have hz := z.isLt
    obtain ⟨hH,hW⟩ := assembled_values (n,q,w4,w5) (w7,x,w6) z
    refine Prod.ext (Prod.ext ?_ (Prod.ext ?_ (Prod.ext ?_ ?_)))
      (Prod.ext (Prod.ext ?_ (Prod.ext ?_ ?_)) ?_)
    all_goals first | apply Fin.ext | apply BitVec.eq_of_toNat_eq
    all_goals
      dsimp [disassemble]
      simp only [hH, hW, highValue, BitVec.toNat_ofNat]
      norm_num only at *
      omega

theorem assembled_bytes (f : FreeCoordinates) (a : ArithmeticCoordinates) (z : Byte) :
    (lo (assembleW f a z)).toNat = f.2.1.val*256+z.toNat ∧
    hi (assembleW f a z) = BitVec.ofNat 32 (highValue f a) ∧
    byte (assembleW f a z) 0 = z ∧
    (byte (assembleW f a z) 1).toNat = f.2.1.val%256 ∧
    (byte (assembleW f a z) 2).toNat = f.2.1.val/256%256 ∧
    (byte (assembleW f a z) 3).toNat = f.2.1.val/65536 ∧
    (byte (assembleW f a z) 4).toNat = f.2.2.1.val ∧
    (byte (assembleW f a z) 5).toNat = f.2.2.2.val ∧
    (byte (assembleW f a z) 6).toNat = a.2.2.val ∧
    (byte (assembleW f a z) 7).toNat = a.1.val := by
  have hn := f.1.isLt
  have hq := f.2.1.isLt
  have h4 := f.2.2.1.isLt
  have h5 := f.2.2.2.isLt
  have h7 := a.1.isLt
  have hx := a.2.1.isLt
  have h6 := a.2.2.isLt
  have hz := z.isLt
  have hw := (assembled_values f a z).2
  repeat' constructor
  all_goals try apply BitVec.eq_of_toNat_eq
  all_goals
    simp only [byte_toNat, lo_toNat, hi_toNat, hw, highValue, BitVec.toNat_ofNat]
    norm_num only at *
    omega

def freeA4 (f : FreeCoordinates) : Byte := 63 ^^^ BitVec.ofNat 8 f.1.val
def freeA5 (f : FreeCoordinates) : Byte := 80 ^^^ BitVec.ofNat 8 (f.1.val/256)

theorem assembled_a_bytes (f : FreeCoordinates) (a : ArithmeticCoordinates) :
    byte (coordinateA (assembleH f a)) 0 = 70 ∧
    byte (coordinateA (assembleH f a)) 1 = 32 ∧
    byte (coordinateA (assembleH f a)) 2 = 232 ∧
    byte (coordinateA (assembleH f a)) 3 = 16 ∧
    byte (coordinateA (assembleH f a)) 4 = freeA4 f ∧
    byte (coordinateA (assembleH f a)) 5 = freeA5 f := by
  refine ⟨?_,?_,?_,?_,?_,?_⟩
  all_goals unfold byte coordinateA
  all_goals first
    | rw [BitVec.extractLsb'_append_eq_of_add_le (by decide)]; rfl
    | rw [BitVec.extractLsb'_append_eq_of_le (by decide), BitVec.extractLsb'_xor]
  · change (63 : Byte) ^^^ (assembleH f a).extractLsb' 0 8 =
      63 ^^^ BitVec.ofNat 8 f.1.val
    congr 1
    apply BitVec.eq_of_toNat_eq
    change ((assembleH f a).toNat >>> 0)%256 = f.1.val%256
    rw [Nat.shiftRight_eq_div_pow, (assembled_values f a 0).1]
    omega
  · change (80 : Byte) ^^^ (assembleH f a).extractLsb' 8 8 =
      80 ^^^ BitVec.ofNat 8 (f.1.val/256)
    congr 1
    apply BitVec.eq_of_toNat_eq
    change ((assembleH f a).toNat >>> 8)%256 = (f.1.val/256)%256
    rw [Nat.shiftRight_eq_div_pow, (assembled_values f a 0).1]
    norm_num only
    omega

end ProvenHashes.Highway
