import ProvenHashes.Highway.Bytes
import ProvenHashes.Highway.KeySpace
namespace ProvenHashes.Highway
set_option maxRecDepth 100000
set_option maxHeartbeats 0

def padPhi (w : Byte) : Byte := (w - 208) ^^^ 174
def padInv (y : Byte) : Byte := (y ^^^ 174) + 208

theorem pad_left_inverse (w : Byte) : padInv (padPhi w) = w := by
  simp [padInv, padPhi, BitVec.xor_assoc]

theorem pad_right_inverse (w : Byte) : padPhi (padInv w) = w := by
  simp [padInv, padPhi, BitVec.xor_assoc]

def padEquiv : Byte ≃ Byte where
  toFun := padPhi
  invFun := padInv
  left_inv := pad_left_inverse
  right_inv := pad_right_inverse

theorem pad_solve (w b t : Byte) : padPhi w + b = t ↔ w = padInv (t-b) := by
  constructor
  · intro h
    have hh := congrArg padInv (eq_sub_of_add_eq h)
    simpa only [pad_left_inverse] using hh
  · intro h
    rw [h, pad_right_inverse]
    exact sub_add_cancel _ _

/-- The four possible pad targets at the carry boundary all select its lower side. -/
theorem pad_boundary_low : ∀ t : Fin 4,
    (padInv (BitVec.ofNat 8 (50+t.val) - 49)).toNat < 208 := by decide

/-- Only the branch with carry one can satisfy the pad equation at the exceptional triple.
The calculation checks 4 × 4 × 256 = 4096 byte cases. -/
theorem pad_boundary : ∀ (t0 t1 : Fin 4) (w : Fin 256),
    (padPhi (BitVec.ofNat 8 w.val) + 49 =
      BitVec.ofNat 8 (50 + if w.val < 208 then t1.val else t0.val)) ↔
    BitVec.ofNat 8 w.val = padInv (BitVec.ofNat 8 (50+t1.val) - 49) := by decide

theorem pad_fibre (b t : Byte) :
    (Finset.univ.filter fun w : Byte => padPhi w + b = t).card = 1 := by
  have he : (Finset.univ.filter fun w : Byte => padPhi w + b = t) =
      {padInv (t-b)} := by
    ext w
    simp [pad_solve]
  rw [he]
  exact Finset.card_singleton _

theorem pad_boundary_word (t0 t1 : Fin 4) (w : Byte) :
    (padPhi w + 49 = BitVec.ofNat 8 (50 + if w.toNat < 208 then t1.val else t0.val)) ↔
    w = padInv (BitVec.ofNat 8 (50+t1.val)-49) := by
  simpa only [BitVec.ofNat_toNat] using pad_boundary t0 t1 ⟨w.toNat, w.isLt⟩

/-- The boundary pad has exactly one solution, in the carry-one branch, even
when the two branches ask for different targets or different side conditions. -/
theorem pad_boundary_fibre (t0 t1 : Fin 4) (P : Bool → Prop) [DecidablePred P] :
    (Finset.univ.filter fun w : Byte => P (decide (w.toNat < 208)) ∧
      padPhi w + 49 = BitVec.ofNat 8 (50 + if w.toNat < 208 then t1.val else t0.val)).card =
      if P true then 1 else 0 := by
  let v := padInv (BitVec.ofNat 8 (50+t1.val)-49)
  have hv : v.toNat < 208 := pad_boundary_low t1
  have he : (Finset.univ.filter fun w : Byte => P (decide (w.toNat < 208)) ∧
      padPhi w + 49 = BitVec.ofNat 8 (50 + if w.toNat < 208 then t1.val else t0.val)) =
      if P true then {v} else ∅ := by
    ext w
    simp only [Finset.mem_filter, Finset.mem_univ, true_and, pad_boundary_word]
    by_cases hw : w = v
    · subst w
      change P (decide (v.toNat < 208)) ∧ v = v ↔ v ∈ if P true then {v} else ∅
      simp only [hv, decide_true, and_true]
      by_cases hp : P true <;> simp [hp]
    · change P (decide (w.toNat < 208)) ∧ w = v ↔ w ∈ if P true then {v} else ∅
      by_cases hp : P true <;> simp [hp, hw]
  rw [he]
  split_ifs <;> simp

end ProvenHashes.Highway
