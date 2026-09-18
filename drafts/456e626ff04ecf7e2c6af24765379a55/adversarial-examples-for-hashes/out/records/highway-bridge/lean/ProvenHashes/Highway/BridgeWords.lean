import ProvenHashes.Highway.Coordinates
import ProvenHashes.Highway.ReducedCount
namespace ProvenHashes.Highway
set_option maxRecDepth 10000
set_option maxHeartbeats 1000000

/-- Lane coordinates: the low reset word in lane 0, and the pre-zipper word in lane 1. -/
def coordinateKey (h : Half) (w : Word) : Key :=
  reducedKey (0xfe4cce2f ^^^ h) (key1Coordinates.symm w)

def coordinateA (h : Half) : Word := (0xc59f503f ^^^ h) ++ (0x10e82046 : Half)
def coordinateB (w : Word) : Word := rot32 (0xe933c0b911f8b2ae ^^^ (w-0xa4093822299f31d0))

theorem rot32_xor (a b : Word) : rot32 (a ^^^ b) = rot32 a ^^^ rot32 b := by
  apply wordHalves.injective
  apply Prod.ext
  · change hi (rot32 (a ^^^ b)) = hi (rot32 a ^^^ rot32 b)
    rw [hi_rot32, show hi (rot32 a ^^^ rot32 b) = hi (rot32 a) ^^^ hi (rot32 b)
      from BitVec.extractLsb'_xor, hi_rot32, hi_rot32]
    exact BitVec.extractLsb'_xor
  · change lo (rot32 (a ^^^ b)) = lo (rot32 a ^^^ rot32 b)
    rw [lo_rot32, show lo (rot32 a ^^^ rot32 b) = lo (rot32 a) ^^^ lo (rot32 b)
      from BitVec.extractLsb'_xor, lo_rot32, lo_rot32]
    exact BitVec.extractLsb'_xor

theorem rot32_append (a b : Half) : rot32 (a ++ b) = b ++ a := by
  apply wordHalves.injective
  change (hi (rot32 (a ++ b)), lo (rot32 (a ++ b))) = wordHalves (b ++ a)
  rw [hi_rot32, lo_rot32]
  have hab := wordHalves.apply_symm_apply (a,b)
  have hba := wordHalves.apply_symm_apply (b,a)
  change (hi (a ++ b), lo (a ++ b)) = (a,b) at hab
  change wordHalves (b ++ a) = (b,a) at hba
  rw [hba]
  exact congrArg Prod.swap hab

theorem coordinate_reset (h : Half) (w : Word) :
    (reset (coordinateKey h w)).ab =
      ⟨⟨(0 : Half) ++ h, coordinateA h, 0xdbe6d5d5fe4cce2f, 0x3bd39e10cb0ef593⟩,
       ⟨coordinateB w, w-0xa4093822299f31d0, 0xa4093822299f31d0, 0xc0acf169b5f18a8c⟩⟩ := by
  apply PairState.ext <;> apply Lane.ext
  all_goals simp only [reset, resetLane, coordinateKey, reducedKey, init0, init1,
    Matrix.cons_val_zero, Matrix.cons_val_one, key1Coordinates, Equiv.coe_fn_symm_mk]
  · change ((0xdbe6d5d5 : Half) ++ (0xfe4cce2f : Half)) ^^^
        ((0xdbe6d5d5 : Half) ++ (0xfe4cce2f ^^^ h)) = (0 : Half) ++ h
    rw [BitVec.xor_append, ← BitVec.xor_assoc]
    simp
  · rw [rot32_append]
    change ((0x3bd39e10 : Half) ++ (0xcb0ef593 : Half)) ^^^
        ((0xfe4cce2f ^^^ h) ++ (0xdbe6d5d5 : Half)) = coordinateA h
    rw [BitVec.xor_append, ← BitVec.xor_assoc]
    rfl
  · unfold coordinateB
    rw [rot32_xor, rot32_xor]
    have hc : (0xa4093822299f31d0 : Word) ^^^ rot32 0xc0acf169b5f18a8c =
        rot32 0xe933c0b911f8b2ae := by decide
    rw [← BitVec.xor_assoc, hc]
  · rw [rot32_involutive]
    simp [← BitVec.xor_assoc]

theorem coordinateB_parts (w : Word) :
    lo (coordinateB w) = 0xe933c0b9 ^^^
      (hi w - 0xa4093822 - if (lo w).toNat < 0x299f31d0 then 1 else 0) ∧
    byte (coordinateB w) 4 = padPhi (byte w 0) := by
  constructor
  · unfold coordinateB
    rw [lo_rot32]
    rw [show hi ((0xe933c0b911f8b2ae : Word) ^^^ (w-0xa4093822299f31d0)) =
      0xe933c0b9 ^^^ hi (w-0xa4093822299f31d0) from BitVec.extractLsb'_xor]
    congr 1
    apply BitVec.eq_of_toNat_eq
    simp only [hi_toNat, lo_toNat, BitVec.toNat_sub,
      BitVec.ofNat_eq_ofNat, BitVec.toNat_ofNat]
    have hw := w.isLt
    split_ifs <;> norm_num only [BitVec.toNat_ofNat] at * <;> omega
  · have hr (x : Word) : byte (rot32 x) 4 = byte x 0 := by
      apply BitVec.eq_of_getLsbD_eq
      intro i hi
      interval_cases i <;>
        simp [byte, rot32]
    unfold coordinateB
    rw [hr]
    change byte ((0xe933c0b911f8b2ae : Word) ^^^ (w-0xa4093822299f31d0)) 0 = _
    have hs : byte (w-0xa4093822299f31d0) 0 = byte w 0 - 208 := by
      apply BitVec.eq_of_toNat_eq
      simp only [byte_toNat, BitVec.toNat_sub, BitVec.ofNat_eq_ofNat,
        BitVec.toNat_ofNat]
      have hw := w.isLt
      norm_num only at *
      omega
    rw [show byte ((0xe933c0b911f8b2ae : Word) ^^^ (w-0xa4093822299f31d0)) 0 =
        174 ^^^ byte (w-0xa4093822299f31d0) 0 from BitVec.extractLsb'_xor, hs]
    exact BitVec.xor_comm _ _

theorem byte4_add_three (a b c : Word) :
    (byte (a+b+c) 4).toNat = ((byte a 4).toNat + (byte b 4).toNat +
      (byte c 4).toNat + ((lo a).toNat+(lo b).toNat+(lo c).toNat)/4294967296)%256 := by
  simp only [byte_toNat, lo_toNat, BitVec.toNat_add]
  have ha := a.isLt
  have hb := b.isLt
  have hc := c.isLt
  norm_num only at *
  omega

theorem zip1_parts (a w : Word) :
    (lo (zip1 a w)).toNat = (byte w 3).toNat + 256*(byte a 4).toNat +
      65536*(byte w 2).toNat + 16777216*(byte w 5).toNat ∧
    byte (zip1 a w) 4 = byte w 1 := by
  constructor
  · rw [lo_toNat, zip1_bytes, pack8_toNat]
    exact nat_pack_low _ _ _ _ _ _ _ _ (byte w 3).isLt (byte a 4).isLt
      (byte w 2).isLt (byte w 5).isLt
  · apply BitVec.eq_of_toNat_eq
    rw [byte_toNat, zip1_bytes, pack8_toNat]
    have h0 := (byte w 3).isLt
    have h1 := (byte a 4).isLt
    have h2 := (byte w 2).isLt
    have h3 := (byte w 5).isLt
    have h4 := (byte w 1).isLt
    norm_num only at *
    omega

theorem nat_coordinate_lowbytes (n x b w4 w6 w7 : Nat)
    (hn : n < 65536) (hx : x < 65536) (_hb : b < 256)
    (_h4 : w4 < 256) (_h6 : w6 < 256) (_h7 : w7 < 256) :
    let u := ((n+65536*x+0x3bd39e10cb0ef593)%18446744073709551616 +
      (16+256*w4+65536*232+16777216*b+4294967296*w6+
        1099511627776*32+281474976710656*w7+72057594037927936*70))%18446744073709551616
    u/65536%256 = lowTotal n b w4 x/65536%256 ∧
    u/16777216%256 = lowTotal n b w4 x/16777216%256 := by
  dsimp only
  unfold lowTotal
  constructor <;> omega

theorem byte_low_half (w : Word) (i : Fin 4) :
    (byte w i.val).toNat = (lo w).toNat / 2^(8*i.val) % 256 := by
  simp only [byte_toNat, lo_toNat]
  fin_cases i <;> norm_num only <;> omega

theorem lo_three_toNat (x y z : Word) :
    (lo (x+y+z)).toNat = ((lo x).toNat+(lo y).toNat+(lo z).toNat)%4294967296 := by
  simp only [lo_toNat, BitVec.toNat_add]
  omega

theorem u0_low_generic (h : Half) (a w : Word)
    (ha2 : byte a 2 = 232) (ha3 : byte a 3 = 16) :
    (lo (((0 : Half) ++ h) + 0x3bd39e10cb0ef593 + zip0 a w)).toNat =
      (h.toNat+0xcb0ef593+16+256*(byte w 4).toNat+65536*232+
        16777216*(byte a 5).toNat)%4294967296 := by
  have hl : (lo ((0 : Half) ++ h)).toNat = h.toNat :=
    congrArg BitVec.toNat (congrArg Prod.snd (wordHalves.apply_symm_apply (0,h)))
  have hc : (lo (0x3bd39e10cb0ef593 : Word)).toNat = 0xcb0ef593 := by decide
  have h2 : (byte a 2).toNat = 232 := congrArg BitVec.toNat ha2
  have h3 : (byte a 3).toNat = 16 := congrArg BitVec.toNat ha3
  rw [lo_three_toNat, hl, hc, lo_zip0_toNat, h2, h3]
  simp only [Nat.add_assoc]

end ProvenHashes.Highway
