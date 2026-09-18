import ProvenHashes.Polymur.Bytes

namespace ProvenHashes.Polymur
abbrev Word := BitVec 64

def xorShift (r : ℕ) (x : Word) : Word := x ^^^ (x >>> r)

/-- Exactly polymur_mix, including unsigned 64-bit multiplication. -/
def mix (x : Word) : Word :=
  xorShift 28 ((xorShift 32 ((xorShift 32 x)*0x0e9846af9b1a615d))*0x0e9846af9b1a615d)

/-- The additive secret is applied after mixing, modulo 2^64. -/
def finish (s : Word) (x : Word) : Word := mix x + s

lemma xorShift32_inverse (x : Word) : xorShift 32 (xorShift 32 x) = x := by
  simp [xorShift, BitVec.ushiftRight_xor_distrib, ← BitVec.shiftRight_add,
    BitVec.ushiftRight_eq_zero (by decide : 64 ≤ 32+32), BitVec.xor_assoc]

lemma xorShift28_inverse (x : Word) :
    (xorShift 28 (xorShift 28 x)) ^^^ ((xorShift 28 x) >>> 56) = x := by
  simp [xorShift, BitVec.ushiftRight_xor_distrib, ← BitVec.shiftRight_add,
    BitVec.ushiftRight_eq_zero (by decide : 64 ≤ 28+56), BitVec.xor_assoc]

lemma xorShift32_injective : Function.Injective (xorShift 32) := by
  intro a b h
  have := congrArg (xorShift 32) h
  simpa only [xorShift32_inverse] using this

lemma xorShift28_injective : Function.Injective (xorShift 28) := by
  intro a b h
  have := congrArg (fun x : Word => xorShift 28 x ^^^ (x >>> 56)) h
  simpa only [xorShift28_inverse] using this

lemma mix_mul_injective : Function.Injective (fun x : Word => x*0x0e9846af9b1a615d) := by
  intro a b h
  have hi : (0x0e9846af9b1a615d : Word) * 0x153ed04bd89cfaf5 = 1 := by decide
  have := congrArg (fun x : Word => x*0x153ed04bd89cfaf5) h
  have hone (x : Word) : x*(1 : Word) = x := BitVec.mul_one x
  simpa only [BitVec.mul_assoc, hi, hone] using this

theorem mix_injective : Function.Injective mix :=
  xorShift28_injective.comp (mix_mul_injective.comp
    (xorShift32_injective.comp (mix_mul_injective.comp xorShift32_injective)))

lemma word_add_injective (s : Word) : Function.Injective (fun x : Word => x+s) := by
  intro a b h
  have := congrArg (fun x : Word => x-s) h
  simpa only [BitVec.add_sub_cancel] using this

theorem finish_injective (s : Word) : Function.Injective (finish s) :=
  (word_add_injective s).comp mix_injective

theorem finish_bijective (s : Word) : Function.Bijective (finish s) := by
  letI : Finite Word := Finite.of_injective (fun x : Word => x.toFin) (by
    intro a b h; cases a; cases b; cases h; rfl)
  exact (Finite.injective_iff_bijective).mp (finish_injective s)

/-- The source adds the common tweak before the mixer. It too preserves equality. -/
theorem finish_tweak_eq_iff (s tweak x y : Word) :
    finish s (x+tweak) = finish s (y+tweak) ↔ x = y :=
  ((finish_injective s).comp (word_add_injective tweak)).eq_iff

end ProvenHashes.Polymur
