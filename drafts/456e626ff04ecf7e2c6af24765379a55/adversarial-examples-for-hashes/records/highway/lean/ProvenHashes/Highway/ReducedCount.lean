import ProvenHashes.Highway.CarryParameters
import ProvenHashes.Highway.PadFibre
namespace ProvenHashes.Highway
set_option maxRecDepth 10000
set_option maxHeartbeats 500000

abbrev ArithmeticCoordinates := Fin 256 × (Fin 65536 × Fin 256)
abbrev RelevantCoordinates := ArithmeticCoordinates × Byte

def padCarry (q : Fin (2^24)) (w : Byte) : Bool :=
  decide (q.val*256+w.toNat < 0x299f31d0)
def padCondition (q : Fin (2^24)) (t : Bool → Fin 4) (w : Byte) : Prop :=
  padPhi w + BitVec.ofNat 8 (q.val%256) = BitVec.ofNat 8 (50+(t (padCarry q w)).val)
instance (q : Fin (2^24)) (t : Bool → Fin 4) : DecidablePred (padCondition q t) :=
  fun _ => inferInstanceAs (Decidable (_ = _))

/-- Uniform pad fibre, including the exceptional carry triple. -/
theorem pad_condition_card (q : Fin (2^24)) (t : Bool → Fin 4) :
    (Finset.univ.filter (padCondition q t)).card = 1 := by
  by_cases hq : q.val < 0x299f31
  · have hc (w : Byte) : padCarry q w = true := by
      have hw := w.isLt
      unfold padCarry
      simp only [decide_eq_true_eq]
      omega
    unfold padCondition
    simp only [hc]
    exact pad_fibre _ _
  by_cases he : q.val = 0x299f31
  · have hc (w : Byte) : padCarry q w = decide (w.toNat < 208) := by
      by_cases hw : w.toNat < 208
      · have hn : q.val*256+w.toNat < 0x299f31d0 := by omega
        simp only [padCarry, hn, hw, decide_true]
      · have hn : ¬ q.val*256+w.toNat < 0x299f31d0 := by omega
        simp only [padCarry, hn, hw, decide_false]
    have hp : (Finset.univ.filter (padCondition q t)) =
        {padInv (BitVec.ofNat 8 (50+(t true).val)-49)} := by
      ext w
      simp only [Finset.mem_filter, Finset.mem_univ, true_and, Finset.mem_singleton]
      unfold padCondition
      rw [hc, he]
      by_cases hw : w.toNat < 208
      · simpa only [hw, decide_true, ite_true] using pad_boundary_word (t false) (t true) w
      · simpa only [hw, decide_false, ite_false] using pad_boundary_word (t false) (t true) w
    rw [hp]
    exact Finset.card_singleton _
  · have hc (w : Byte) : padCarry q w = false := by
      unfold padCarry
      simp only [decide_eq_false_iff_not]
      omega
    unfold padCondition
    simp only [hc]
    exact pad_fibre _ _

theorem relevant_coordinates_card : Fintype.card RelevantCoordinates = 2^40 := by
  simp [RelevantCoordinates, ArithmeticCoordinates, card_word]

/-- Exact count of the fully specified 40-bit byte model, for every choice of its
free parameters and every allowed pair of pad targets. `BridgePredicate` and
`BridgeCount` transport this count to the actual full key space. -/
theorem reduced_count (n b w4 : Nat) (q : Fin (2^24))
    (t : ArithmeticCoordinates → Bool → Fin 4)
    (hn : n < 65536) (hb : b < 256) (hw : w4 < 256) :
    (Finset.univ.filter fun p : RelevantCoordinates =>
      ArithmeticBytes n b w4 p.1 ∧ padCondition q (t p.1) p.2).card = 56165 := by
  rw [unique_pad_card (ArithmeticBytes n b w4) (fun a => padCondition q (t a))
    (fun a => pad_condition_card q (t a))]
  exact arithmetic_bytes_card n b w4 hn hb hw

end ProvenHashes.Highway
