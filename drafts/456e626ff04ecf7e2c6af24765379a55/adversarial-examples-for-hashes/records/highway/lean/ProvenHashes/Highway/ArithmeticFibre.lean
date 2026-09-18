import ProvenHashes.Highway.CountNormal
namespace ProvenHashes.Highway
set_option maxRecDepth 10000
set_option maxHeartbeats 1000000

def lowSolution (s r : Nat) : Nat := (r+256-s%256)%256
def windowStart (s r : Nat) : Nat := (s+lowSolution s r)/256
def middleSolution (sigma q j : Nat) : Nat :=
  ((q+j)%256+512-202-(sigma+(q+j)/256))%256

def arithmeticFibre (sigma s r x w : Nat) : Prop :=
  (x+s)%256 = r ∧
  (x+s)/256%256 = (202+w+sigma+(x+s)/65536)%256 ∧
  w+sigma+(x+s)/65536 ≤ 239
instance (sigma s r x w : Nat) : Decidable (arithmeticFibre sigma s r x w) :=
  inferInstanceAs (Decidable (_ ∧ _ ∧ _))

/-- Solving the low and middle byte equations leaves exactly the carry window. -/
theorem arithmetic_fibre_iff (sigma s r x w : Nat)
    (hsigma : sigma ≤ 1) (hs : s < 65536) (hr : r < 256)
    (hx : x < 65536) (hw : w < 256) :
    arithmeticFibre sigma s r x w ↔
      x%256 = lowSolution s r ∧
      w = middleSolution sigma (windowStart s r) (x/256) ∧
      windowGood sigma (windowStart s r) (x/256) = true := by
  have hl : (x+s)%256 = r ↔ x%256 = lowSolution s r := by
    unfold lowSolution
    omega
  unfold arithmeticFibre
  rw [hl]
  apply and_congr_right
  intro hlx
  have hsum : (x+s)/256 = windowStart s r + x/256 := by
    unfold windowStart
    omega
  have hcarry : (x+s)/65536 = (windowStart s r+x/256)/256 := by omega
  have hq : windowStart s r ≤ 256 := by
    unfold windowStart lowSolution
    omega
  rw [hsum, hcarry]
  unfold middleSolution windowGood
  simp only [decide_eq_true_eq]
  omega

end ProvenHashes.Highway
