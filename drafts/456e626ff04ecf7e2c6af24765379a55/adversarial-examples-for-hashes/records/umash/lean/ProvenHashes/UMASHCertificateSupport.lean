import ProvenHashes.UMASHMasks

namespace ProvenHashes.UMASH

theorem list_toFinset_map (l : List ℕ) (f : ℕ → ℕ) :
    (l.map f).toFinset = l.toFinset.image f := by
  ext x
  simp
-- CHECKPOINT

theorem biUnion_range_step (n : ℕ) (f : ℕ → Finset ℕ) :
    (Finset.range (n+1)).biUnion f = f n ∪ (Finset.range n).biUnion f := by
  ext x
  simp only [Finset.mem_biUnion, Finset.mem_range, Finset.mem_union]
  constructor
  · rintro ⟨a, ha, hx⟩
    by_cases he : a = n
    · exact Or.inl (he ▸ hx)
    · exact Or.inr ⟨a, by omega, hx⟩
  · rintro (hx | ⟨a, ha, hx⟩)
    · exact ⟨n, by omega, hx⟩
    · exact ⟨a, by omega, hx⟩
-- CHECKPOINT

end ProvenHashes.UMASH
