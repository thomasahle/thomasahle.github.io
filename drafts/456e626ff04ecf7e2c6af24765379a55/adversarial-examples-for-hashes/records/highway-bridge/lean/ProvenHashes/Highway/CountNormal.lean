import ProvenHashes.Highway.KeySpace

/-! Small finite residue calculation in step 5 of the supplied byte proof.
This module counts the reduced window, not yet the original key predicate. -/
namespace ProvenHashes.Highway
set_option maxRecDepth 100000
set_option maxHeartbeats 0

def windowGood (sigma q j : Nat) : Bool :=
  let u := q + j
  let gamma := sigma + u / 256
  decide (((u % 256 + 512 - 202 - gamma) % 256) + gamma ≤ 239)

theorem window_count : ∀ (sigma : Fin 2) (q : Fin 257),
    (sigma.val = 0 → 203 ≤ q.val) → (sigma.val = 1 → q.val ≤ 203) →
    ((List.range 256).countP (windowGood sigma.val q.val)) = 239 := by
  decide

theorem top_byte_count : ((List.range 256).countP (fun w => decide (21 ≤ w))) = 235 := by
  decide

theorem reduced_product : (235 : Nat) * 239 = 56165 := by decide

end ProvenHashes.Highway
