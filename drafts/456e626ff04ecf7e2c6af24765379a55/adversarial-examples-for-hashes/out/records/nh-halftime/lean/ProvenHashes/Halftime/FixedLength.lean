import ProvenHashes.Halftime.Toeplitz

namespace ProvenHashes.Halftime
open scoped BigOperators
set_option maxHeartbeats 1000000

/-- One whole NH pair at a chosen key word. -/
def terminalEmbedding {I : Type*} (p : I) : (Fin 1 × Bool) ↪ (I × Bool) :=
  ⟨fun z => (p, z.2), by
    intro a b h
    apply Prod.ext
    · exact Subsingleton.elim _ _
    · exact congrArg (fun z : I × Bool => z.2) h⟩

def terminalNH {I : Type*} (x : Bool → ZMod (2^32))
    (k : I × Bool → ZMod (2^32)) (p : I) : ZMod (2^64) :=
  nh32 (fun z : Fin 1 × Bool => x z.2) (fun z => k (terminalEmbedding p z))

theorem terminal_difference_injective {I : Type*} [DecidableEq I]
    (x y : Bool → ZMod (2^32)) (b : Bool) (hb : x b ≠ y b)
    (k : I × Bool → ZMod (2^32)) (p : I) :
    Function.Injective (fun v =>
      terminalNH x (Function.update k (p, !b) v) p -
      terminalNH y (Function.update k (p, !b) v) p) := by
  intro u v huv
  change terminalNH x (Function.update k (terminalEmbedding p (0, !b)) u) p -
      terminalNH y (Function.update k (terminalEmbedding p (0, !b)) u) p =
    terminalNH x (Function.update k (terminalEmbedding p (0, !b)) v) p -
      terminalNH y (Function.update k (terminalEmbedding p (0, !b)) v) p at huv
  simp only [terminalNH, update_comp_embedding, nh32, ← map_sub] at huv
  exact nh_difference_update_injective
    (fun z : Fin 1 × Bool => x z.2) (fun z : Fin 1 × Bool => y z.2)
    (fun z => k (terminalEmbedding p z)) 0 b hb (nh32Equiv.injective huv)

/-- A right-aligned terminal length pair gives 2^-96 AΔU, even when the
preceding data supports, byte lengths, and forests differ. `residual i` may
use earlier terminal keys, but ignores its own and every later terminal key.
No independence of the three derived outputs is assumed. -/
theorem terminal_length_three_bound {I : Type*} [Fintype I] [DecidableEq I]
    (pick : Fin 3 ↪ I) (x y : Bool → ZMod (2^32)) (hxy : x ≠ y)
    (residual : Fin 3 → (I × Bool → ZMod (2^32)) → ZMod (2^64))
    (hr : ∀ i j, i ≤ j → ∀ k b v,
      residual i (Function.update k (pick j, b) v) = residual i k)
    (target : Fin 3 → ZMod (2^64)) :
    uniformProb (fun k => ∀ j,
      residual j k + (terminalNH x k (pick j) - terminalNH y k (pick j)) = target j)
      ≤ 1 / (2 : ℚ≥0)^96 := by
  classical
  obtain ⟨b, hb⟩ : ∃ b, x b ≠ y b := by
    by_contra h
    push_neg at h
    exact hxy (funext h)
  let f := fun j k => residual j k +
    (terminalNH x k (pick j) - terminalNH y k (pick j))
  have hf : ∀ j k, Function.Injective
      (fun v => f j (Function.update k (pick j, !b) v)) := by
    intro j k u v huv
    simp only [f, hr j j le_rfl] at huv
    exact terminal_difference_injective x y b hb k (pick j) (add_left_cancel huv)
  have hp : ∀ i j : Fin 3, i < j → ∀ k v,
      f i (Function.update k (pick j, !b) v) = f i k := by
    intro i j hij k v
    have hne : pick i ≠ pick j := fun h => (ne_of_lt hij) (pick.injective h)
    have hk : (fun z : Fin 1 × Bool =>
        Function.update k (pick j, !b) v (terminalEmbedding (pick i) z)) =
        (fun z => k (terminalEmbedding (pick i) z)) := by
      funext z
      apply Function.update_of_ne
      intro h
      exact hne (congrArg Prod.fst h)
    simp only [f, hr i j (le_of_lt hij), terminalNH, hk]
  have H := triangular_adu 3 f (fun j => (pick j, !b)) hf hp target
  norm_num [ZMod.card] at H ⊢
  exact H

#print axioms terminal_difference_injective
#print axioms terminal_length_three_bound
end ProvenHashes.Halftime
