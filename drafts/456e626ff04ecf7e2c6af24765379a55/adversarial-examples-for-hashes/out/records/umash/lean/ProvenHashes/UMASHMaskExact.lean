import ProvenHashes.UMASHMasks

namespace ProvenHashes.UMASH

/-- Every enumerated support is realized by two bounded words with the required
signed difference. Together with `xor_mem_signedSupports`, this proves exactness. -/
theorem signedSupports_sound (w : ℕ) (n : ℤ) (d : ℕ)
    (hd : d ∈ signedSupports w n) :
    ∃ x y : ℕ, x < 2^w ∧ y < 2^w ∧ (x : ℤ)-y = n ∧ x ^^^ y = d := by
  induction w generalizing n d with
  | zero =>
    by_cases hn : n = 0
    · subst n
      simp [signedSupports] at hd
      subst d
      exact ⟨0, 0, by decide, by decide, rfl, rfl⟩
    · simp [signedSupports, hn] at hd
  | succ w ih =>
    simp only [signedSupports] at hd
    split_ifs at hd with he
    · obtain ⟨s, hs, rfl⟩ := Finset.mem_image.mp hd
      obtain ⟨x, y, hx, hy, hdiff, hxor⟩ := ih _ _ hs
      refine ⟨2*x, 2*y, ?_, ?_, ?_, ?_⟩
      · rw [pow_succ]; omega
      · rw [pow_succ]; omega
      · push_cast; omega
      · simpa [Nat.bit, hxor] using Nat.xor_bit false x false y
    · obtain ⟨s, hs, rfl⟩ := Finset.mem_image.mp hd
      rcases Finset.mem_union.mp hs with hs | hs
      · obtain ⟨x, y, hx, hy, hdiff, hxor⟩ := ih _ _ hs
        refine ⟨2*x+1, 2*y, ?_, ?_, ?_, ?_⟩
        · rw [pow_succ]; omega
        · rw [pow_succ]; omega
        · push_cast; omega
        · simpa [Nat.bit, hxor] using Nat.xor_bit true x false y
      · obtain ⟨x, y, hx, hy, hdiff, hxor⟩ := ih _ _ hs
        refine ⟨2*x, 2*y+1, ?_, ?_, ?_, ?_⟩
        · rw [pow_succ]; omega
        · rw [pow_succ]; omega
        · push_cast; omega
        · simpa [Nat.bit, hxor] using Nat.xor_bit false x true y
-- CHECKPOINT

end ProvenHashes.UMASH
