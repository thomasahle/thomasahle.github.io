import ProvenHashes.Highway.Model
import ProvenHashes.Probability

namespace ProvenHashes.Highway

instance bitVecFintype (n : Nat) : Fintype (BitVec n) :=
  Fintype.ofEquiv (Fin (2 ^ n)) (BitVec.equivFin (m := n)).toEquiv.symm

theorem card_word (n : Nat) : Fintype.card (BitVec n) = 2 ^ n := by
  simpa using Fintype.card_congr (BitVec.equivFin (m := n)).toEquiv

theorem card_key : Fintype.card Key = 2 ^ 256 := by
  simp [Key, Lanes, card_word]

def wordHalves : Word ≃ Half × Half where
  toFun x := (hi x, lo x)
  invFun p := p.1 ++ p.2
  left_inv x := by
    change (x.extractLsb' 32 32 ++ x.extractLsb' 0 32) = x
    have h := BitVec.extractLsb'_append_extractLsb'_eq_extractLsb'
      (x := x) (start₁ := 0) (len₁ := 32) (start₂ := 32) (len₂ := 32) (by decide)
    simpa using h
  right_inv p := by
    rcases p with ⟨h, l⟩
    apply Prod.ext
    · change (h ++ l).extractLsb' 32 32 = h
      rw [BitVec.extractLsb'_append_eq_of_le (by decide)]
      simp
    · change (h ++ l).extractLsb' 0 32 = l
      rw [BitVec.extractLsb'_append_eq_of_add_le (by decide)]
      simp

abbrev OtherKeys := { i : Fin 4 // i ≠ 0 } → Word

def keyCoordinates : Key ≃ Half × (Half × OtherKeys) :=
  (Equiv.funSplitAt (0 : Fin 4) Word).trans
    ((Equiv.prodCongr wordHalves (Equiv.refl OtherKeys)).trans
      (Equiv.prodAssoc Half Half OtherKeys))

theorem keyCoordinates_class (k : Key) : (keyCoordinates k).1 = hi (k 0) := rfl

theorem class_probability : ProvenHashes.uniformProb Class = 1 / (2 : ℚ≥0)^32 := by
  have h := ProvenHashes.uniformProb_equiv keyCoordinates
    (fun p => p.1 = (0xdbe6d5d5 : Half))
  change ProvenHashes.uniformProb Class = _ at h
  rw [h, ProvenHashes.uniformProb_fst, card_word]
  norm_cast

end ProvenHashes.Highway
