import ProvenHashes.Highway.PacketOne
import ProvenHashes.Highway.KeySpace
namespace ProvenHashes.Highway
set_option maxRecDepth 10000
set_option maxHeartbeats 500000

theorem hi_rot32 (x : Word) : hi (rot32 x) = lo x := by
  apply BitVec.eq_of_getLsbD_eq
  intro i h
  unfold lo hi rot32
  simp only [BitVec.getLsbD_extractLsb', BitVec.getLsbD_or,
    BitVec.getLsbD_shiftLeft, BitVec.getLsbD_ushiftRight]
  interval_cases i <;> simp

theorem rot32_involutive (x : Word) : rot32 (rot32 x) = x := by
  apply wordHalves.injective
  change (hi (rot32 (rot32 x)), lo (rot32 (rot32 x))) = (hi x, lo x)
  rw [hi_rot32, lo_rot32, lo_rot32, hi_rot32]

def xorCoordinates (n : Nat) (c : BitVec n) : BitVec n ≃ BitVec n where
  toFun x := c ^^^ x
  invFun x := c ^^^ x
  left_inv x := by simp [← BitVec.xor_assoc]
  right_inv x := by simp [← BitVec.xor_assoc]

/-- Key lane 1 to its pre-zipper packet-1 v₁ word is a bijection. -/
def key1Coordinates : Word ≃ Word where
  toFun k := (0xc0acf169b5f18a8c ^^^ rot32 k) + 0xa4093822299f31d0
  invFun w := rot32 (0xc0acf169b5f18a8c ^^^ (w-0xa4093822299f31d0))
  left_inv k := by simp [rot32_involutive, ← BitVec.xor_assoc]
  right_inv w := by simp [rot32_involutive, ← BitVec.xor_assoc]

theorem firstW_coordinates (k : Key) : firstW k = key1Coordinates (k 1) := rfl

def reducedKey (h : Half) (k1 : Word) : Key := ![(0xdbe6d5d5 : Half) ++ h, k1, 0, 0]
def ReducedE2 (r : Half × Word) : Prop := E2 (reducedKey r.1 r.2)

theorem reducedKey_class (h : Half) (k1 : Word) : Class (reducedKey h k1) := by
  change hi ((0xdbe6d5d5 : Half) ++ h) = _
  exact congrArg Prod.fst (wordHalves.apply_symm_apply (0xdbe6d5d5,h))

def pairE2 (s : PairState) : Prop :=
  let t := pairStep 0x24192a2a01b331d1 0 s
  lo (t.a.v1+t.a.mul0+0x24192a2ab4b332d1)-hi t.a.v0=256

theorem e2_pair (k : Key) : E2 k ↔ pairE2 (reset k).ab := Iff.rfl

theorem e2_depends_on_two_words (k j : Key) (h0 : k 0 = j 0) (h1 : k 1 = j 1) :
    E2 k ↔ E2 j := by
  rw [e2_pair, e2_pair]
  have hs : (reset k).ab = (reset j).ab := by
    change PairState.mk (resetLane _ _ (k 0)) (resetLane _ _ (k 1)) =
      PairState.mk (resetLane _ _ (j 0)) (resetLane _ _ (j 1))
    rw [h0,h1]
  rw [hs]

theorem class_reduced_e2 (k : Key) (hk : Class k) :
    E2 k ↔ ReducedE2 (lo (k 0), k 1) := by
  apply e2_depends_on_two_words
  · change k 0 = (0xdbe6d5d5 : Half) ++ lo (k 0)
    have h := wordHalves.symm_apply_apply (k 0)
    change hi (k 0) ++ lo (k 0) = k 0 at h
    rw [hk] at h
    exact h.symm
  · rfl

end ProvenHashes.Highway
