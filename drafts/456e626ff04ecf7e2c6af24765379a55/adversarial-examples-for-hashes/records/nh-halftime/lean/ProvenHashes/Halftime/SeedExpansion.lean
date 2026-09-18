import ProvenHashes.Halftime.Probability

namespace ProvenHashes.Halftime
set_option maxRecDepth 4000

/-- A deterministic seed expansion cannot have uniform full-key atoms when
its codomain has more elements than its seed space. -/
theorem no_uniform_expansion {S K : Type*} [Fintype S] [Fintype K] [Nonempty K]
    (hcard : Fintype.card S < Fintype.card K) (expand : S → K) :
    ¬ (∀ k, uniformProb (fun s => expand s = k) = 1 / (Fintype.card K : ℚ≥0)) := by
  intro h
  have hs : Function.Surjective expand := by
    intro k
    by_contra hn
    have hz : uniformProb (fun s => expand s = k) = 0 := by
      have he : (fun s => expand s = k) = (fun _ => False) := by
        funext s
        apply propext
        simp only [iff_false]
        exact fun hsk => hn ⟨s, hsk⟩
      rw [he]
      simp [uniformProb]
    have hk := h k
    rw [hz] at hk
    have hc : (0 : ℚ≥0) < Fintype.card K := by exact_mod_cast Fintype.card_pos
    have := one_div_pos.mpr hc
    exact (ne_of_gt this) hk.symm
  exact (not_le_of_gt hcard) (Fintype.card_le_of_surjective expand hs)

/-- Already two independent 64-bit words cannot be supplied uniformly by one
64-bit seed. This rules out a uniform-key refinement for any larger array. -/
theorem no_uniform_seed_expansion_two_words
    (expand : ZMod (2 ^ 64) → (Fin 2 → ZMod (2 ^ 64))) :
    ¬ (∀ k, uniformProb (fun s => expand s = k) = 1 / (2 : ℚ≥0) ^ 128) := by
  have H := no_uniform_expansion (S := ZMod (2 ^ 64))
    (K := Fin 2 → ZMod (2 ^ 64)) (by norm_num [ZMod.card, Fintype.card_fun]) expand
  simpa [ZMod.card, Fintype.card_fun] using H

end ProvenHashes.Halftime
