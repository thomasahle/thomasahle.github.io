import ProvenHashes.Highway.CountNormal
namespace ProvenHashes.Highway
set_option maxRecDepth 10000

theorem fin_filter_card (n : Nat) (f : Nat → Bool) :
    (Finset.univ.filter fun i : Fin n => f i.val).card = (List.range n).countP f := by
  have hc : (Finset.univ.filter fun i : Fin n => f i.val).card =
      ((Finset.range n).filter fun j => f j).card := by
    apply Finset.card_bij (fun i _ => i.val)
    · intro i hi
      simpa only [Finset.mem_filter, Finset.mem_range] using
        And.intro i.isLt (Finset.mem_filter.mp hi).2
    · intro a _ b _ hab
      exact Fin.ext hab
    · intro j hj
      obtain ⟨hj, hp⟩ := Finset.mem_filter.mp hj
      refine ⟨⟨j, Finset.mem_range.mp hj⟩, ?_, rfl⟩
      simpa only [Finset.mem_filter, Finset.mem_univ, true_and] using hp
  rw [hc, ← List.toFinset_range, ← List.toFinset_filter,
    List.toFinset_card_of_nodup ((List.nodup_range (n := n)).filter f)]
  exact (List.countP_eq_length_filter).symm

theorem window_card (sigma : Fin 2) (q : Fin 257)
    (h0 : sigma.val = 0 → 203 ≤ q.val) (h1 : sigma.val = 1 → q.val ≤ 203) :
    (Finset.univ.filter fun j : Fin 256 => windowGood sigma.val q.val j.val).card = 239 := by
  rw [fin_filter_card]
  exact window_count sigma q h0 h1

theorem top_byte_card : (Finset.univ.filter fun w : Fin 256 => 21 ≤ w.val).card = 235 := by
  have h := fin_filter_card 256 (fun w => decide (21 ≤ w))
  simp only [decide_eq_true_eq] at h
  exact h.trans top_byte_count

end ProvenHashes.Highway
