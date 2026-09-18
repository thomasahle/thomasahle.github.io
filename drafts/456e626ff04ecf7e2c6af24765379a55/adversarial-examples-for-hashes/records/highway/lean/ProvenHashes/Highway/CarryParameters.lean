import ProvenHashes.Highway.CountProduct
import ProvenHashes.Highway.InitialSummary
namespace ProvenHashes.Highway
set_option maxRecDepth 10000
set_option maxHeartbeats 1000000

def carrySigma (b : Nat) : Nat := (203+b)/256
def carryOffset (n b w4 : Nat) : Nat :=
  ((203+b)%256)*256 + (0xf6f5a3+256*w4+n)/65536
def lowTotal (n b w4 x : Nat) : Nat :=
  n+65536*x+0xcb0ef593+16+256*w4+65536*232+16777216*b

theorem carry_parameters_bounds (n b w4 : Nat)
    (hn : n < 65536) (hb : b < 256) (hw : w4 < 256) :
    carrySigma b < 2 ∧ carryOffset n b w4 < 65536 := by
  unfold carrySigma carryOffset
  omega

theorem carry_window_bounds (n b w4 r : Nat)
    (hn : n < 65536) (hb : b < 256) (hw : w4 < 256) (_hr : r < 256) :
    (carrySigma b = 0 → 203 ≤ windowStart (carryOffset n b w4) r) ∧
    (carrySigma b = 1 → windowStart (carryOffset n b w4) r ≤ 203) := by
  unfold carrySigma windowStart carryOffset lowSolution
  omega

theorem carry_coordinates (n b w4 x : Nat) :
    firstGamma (n+65536*x) b w4 = carrySigma b+(x+carryOffset n b w4)/65536 ∧
    lowTotal n b w4 x / 65536 % 256 = (x+carryOffset n b w4)%256 ∧
    lowTotal n b w4 x / 16777216 % 256 = (x+carryOffset n b w4)/256%256 := by
  unfold firstGamma carrySigma carryOffset lowTotal
  omega

def ArithmeticBytes (n b w4 : Nat) (p : Fin 256 × (Fin 65536 × Fin 256)) : Prop :=
  let g := firstGamma (n+65536*p.2.1.val) b w4
  21 ≤ p.1.val ∧ p.2.2.val+g ≤ 239 ∧
    lowTotal n b w4 p.2.1.val / 16777216 % 256 = (202+p.2.2.val+g)%256 ∧
    lowTotal n b w4 p.2.1.val / 65536 % 256 = (235+p.1.val)%256
instance (n b w4 : Nat) : DecidablePred (ArithmeticBytes n b w4) :=
  fun _ => inferInstanceAs (Decidable (_ ∧ _ ∧ _ ∧ _))

/-- The two free bytes have 56165 choices after the two forced byte equations.
The low-key bytes, zipper byte, and their XOR-derived byte may be arbitrary. -/
theorem arithmetic_bytes_card (n b w4 : Nat)
    (hn : n < 65536) (hb : b < 256) (hw : w4 < 256) :
    (Finset.univ.filter (ArithmeticBytes n b w4)).card = 56165 := by
  let sigma : Fin 2 := ⟨carrySigma b, (carry_parameters_bounds n b w4 hn hb hw).1⟩
  let s : Fin 65536 := ⟨carryOffset n b w4, (carry_parameters_bounds n b w4 hn hb hw).2⟩
  have he : (Finset.univ.filter (ArithmeticBytes n b w4)) =
      (Finset.univ.filter fun p : Fin 256 × (Fin 65536 × Fin 256) =>
        21 ≤ p.1.val ∧ arithmeticFibre sigma.val s.val ((235+p.1.val)%256)
          p.2.1.val p.2.2.val) := by
    ext p
    simp only [Finset.mem_filter, Finset.mem_univ, true_and]
    dsimp [ArithmeticBytes, arithmeticFibre, sigma, s]
    obtain ⟨hg, hl, hh⟩ := carry_coordinates n b w4 p.2.1.val
    rw [hg,hl,hh]
    simp only [add_assoc]
    tauto
  rw [he]
  apply top_arithmetic_card (fun _ => sigma) (fun _ => s)
  · intro w
    exact (carry_window_bounds n b w4 ((235+w.val)%256) hn hb hw
      (Nat.mod_lt _ (by decide))).1
  · intro w
    exact (carry_window_bounds n b w4 ((235+w.val)%256) hn hb hw
      (Nat.mod_lt _ (by decide))).2

end ProvenHashes.Highway
