import ProvenHashes.UMASHModel

namespace ProvenHashes.UMASH

set_option maxHeartbeats 8000000

theorem bool_xor_cancel (a b : Bool) : (a ^^ (a ^^ b)) = b := by
  cases a <;> cases b <;> rfl
-- CHECKPOINT

/-- Explicit inverse: factors of (1+R^8+R^33)^63 in characteristic two. -/
def finalize2 (x : Word) : Word := x ^^^ x.rotateLeft 16 ^^^ x.rotateLeft 2
def finalize4 (x : Word) : Word := x ^^^ x.rotateLeft 32 ^^^ x.rotateLeft 4

def unfinalize (x : Word) : Word :=
  (finalize4 (finalize2 (finalize x))).rotateLeft 56

theorem finalize_square (x : Word) : finalize (finalize x) = finalize2 x := by
  apply BitVec.eq_of_getLsbD_eq
  intro i hi
  interval_cases i <;>
    simp only [finalize, finalize2, BitVec.getLsbD_xor, BitVec.getLsbD_rotateLeft] <;>
    norm_num <;>
    simp only [Bool.xor_assoc, Bool.xor_left_comm, Bool.xor_comm,
      Bool.xor_self, Bool.xor_false, Bool.false_xor, bool_xor_cancel]
-- CHECKPOINT

theorem finalize2_square (x : Word) : finalize2 (finalize2 x) = finalize4 x := by
  apply BitVec.eq_of_getLsbD_eq
  intro i hi
  interval_cases i <;>
    simp only [finalize2, finalize4, BitVec.getLsbD_xor, BitVec.getLsbD_rotateLeft] <;>
    norm_num <;>
    simp only [Bool.xor_assoc, Bool.xor_left_comm, Bool.xor_comm,
      Bool.xor_self, Bool.xor_false, Bool.false_xor, bool_xor_cancel]
-- CHECKPOINT

theorem finalize4_square (x : Word) : finalize4 (finalize4 x) = x.rotateLeft 8 := by
  apply BitVec.eq_of_getLsbD_eq
  intro i hi
  interval_cases i <;>
    simp only [finalize4, BitVec.getLsbD_xor, BitVec.getLsbD_rotateLeft] <;>
    norm_num <;>
    simp only [Bool.xor_assoc, Bool.xor_left_comm, Bool.xor_comm,
      Bool.xor_self, Bool.xor_false, Bool.false_xor, bool_xor_cancel]
-- CHECKPOINT

theorem unfinalize_finalize (x : Word) : unfinalize (finalize x) = x := by
  rw [unfinalize, finalize_square, finalize2_square, finalize4_square]
  apply BitVec.eq_of_getLsbD_eq
  intro i hi
  interval_cases i <;> simp
-- CHECKPOINT

theorem finalize_injective : Function.Injective finalize :=
  Function.LeftInverse.injective unfinalize_finalize
-- CHECKPOINT

end ProvenHashes.UMASH
