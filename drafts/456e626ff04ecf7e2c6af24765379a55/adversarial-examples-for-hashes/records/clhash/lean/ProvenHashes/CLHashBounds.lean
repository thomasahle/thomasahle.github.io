import ProvenHashes.CLHashPolynomial
import ProvenHashes.CLHashFinalizer

noncomputable section
namespace ProvenHashes.CLHash
open ChainHash Polynomial

/-- The independent keys for content, with the length key exposed separately. -/
abbrev ContentKey := (BlockKey × Word 126) × (Word 64 × Word 64)

def keyEquiv : (ContentKey × Word 64) ≃ IdealKey where
  toFun k := (k.1.1.1, (k.1.1.2, (k.1.2, k.2)))
  invFun k := (((k.1, k.2.1), k.2.2.1), k.2.2.2)
  left_inv _ := rfl
  right_inv _ := rfl

def content (k : ContentKey) (m : Message) : BitsPolynomial :=
  if m.length ≤ 1024 then rawBlock m k.1.1 0
  else finalProduct (aggregate (fun i : Fin (blockCount m) => rawBlock m k.1.1 i.val) k.1.2) k.2

/-- Literal Algorithm 4 equals the field expression, including its length term. -/
theorem hash_keyEquiv (k : ContentKey × Word 64) (m : Message) :
    hash (keyEquiv k) m = fieldRepr.symm
      (AdjoinRoot.mk modulus (content k.1 m) + fieldRepr k.2 * fieldRepr (lengthWord m.length)) := by
  change reduce64 (content k.1 m + pack 64 k.2 * pack 64 (lengthWord m.length)) = _
  rw [reduce64_eq, map_add, map_mul]
  rfl

/-- Distinct byte lengths are separated by the independent length key,
including when the two messages take different branches of Algorithm 4. -/
theorem unequal_lengths_bound : UnequalLengthsBound := by
  intro m m' hm hm' hlen
  rw [← uniformProb_equiv keyEquiv (fun k : IdealKey => hash k m = hash k m')]
  simp_rw [hash_keyEquiv, fieldRepr.symm.injective.eq_iff]
  apply uniformProb_prod_le
  intro k
  have hd : fieldRepr (lengthWord m.length) - fieldRepr (lengthWord m'.length) ≠ 0 := by
    intro h
    exact hlen (lengthWord_injective hm hm' (fieldRepr.injective (sub_eq_zero.mp h)))
  let f : Word 64 → BinaryQuotient := fun l =>
    fieldRepr l * (fieldRepr (lengthWord m.length) - fieldRepr (lengthWord m'.length)) +
      (AdjoinRoot.mk modulus (content k m) - AdjoinRoot.mk modulus (content k m'))
  have he : (fun l : Word 64 =>
      AdjoinRoot.mk modulus (content k m) + fieldRepr l * fieldRepr (lengthWord m.length) =
      AdjoinRoot.mk modulus (content k m') + fieldRepr l * fieldRepr (lengthWord m'.length)) =
      (fun l => f l = 0) := by
    funext l
    apply propext
    rw [← sub_eq_zero]
    have hf : f l =
        (AdjoinRoot.mk modulus (content k m) + fieldRepr l * fieldRepr (lengthWord m.length)) -
        (AdjoinRoot.mk modulus (content k m') + fieldRepr l * fieldRepr (lengthWord m'.length)) := by
      dsimp [f]
      ring
    rw [hf]
  rw [he]
  have hinj : Function.Injective f := by
    intro a b h
    exact fieldRepr.injective (mul_right_cancel₀ hd (add_right_cancel h))
  simpa only [word_card, Nat.cast_pow, Nat.cast_ofNat] using uniformProb_injective_le f hinj 0

/-- Equal byte lengths cancel the length product exactly. -/
theorem equal_length_content_prob (m m' : Message) (hlen : m.length = m'.length) :
    uniformProb (fun k : IdealKey => hash k m = hash k m') =
      uniformProb (fun k : ContentKey =>
        AdjoinRoot.mk modulus (content k m) = AdjoinRoot.mk modulus (content k m')) := by
  rw [← uniformProb_equiv keyEquiv (fun k : IdealKey => hash k m = hash k m')]
  simp_rw [hash_keyEquiv, fieldRepr.symm.injective.eq_iff, hlen, add_right_cancel_iff]
  exact uniformProb_prod_fst (J := Word 64) (fun k : ContentKey =>
    AdjoinRoot.mk modulus (content k m) = AdjoinRoot.mk modulus (content k m'))

/-- One CLNH block, including a partial last word, is enough for the short regime. -/
theorem short_equal_length_bound (m m' : Message)
    (hlen : m.length = m'.length) (hne : m ≠ m') (hshort : m.length ≤ 1024) :
    uniformProb (fun k : IdealKey => hash k m = hash k m') ≤ 1 / (2 : ℚ≥0) ^ 64 := by
  classical
  have hshort' : m'.length ≤ 1024 := by omega
  have hd : ∃ s ∈ activePairs m 0, ∃ b, blockData m 0 (s, b) ≠ blockData m' 0 (s, b) := by
    by_contra! h
    apply hne
    apply block_encoding_injective m m' hlen
    intro t ht
    have ht0 : t = 0 := by unfold blockCount at ht; omega
    subst t
    exact h
  have ha : activePairs m 0 = activePairs m' 0 := by simp [activePairs, blockBytes, hlen]
  rw [equal_length_content_prob m m' hlen]
  simp only [content, if_pos hshort, if_pos hshort']
  rw [uniformProb_prod_fst (J := Word 64 × Word 64) (fun k : BlockKey × Word 126 =>
      AdjoinRoot.mk modulus (rawBlock m k.1 0) = AdjoinRoot.mk modulus (rawBlock m' k.1 0)),
    uniformProb_prod_fst (J := Word 126) (fun k : BlockKey =>
      AdjoinRoot.mk modulus (rawBlock m k 0) = AdjoinRoot.mk modulus (rawBlock m' k 0))]
  simpa only [rawBlock, ← ha, sub_eq_zero] using
    reduced_clnh_difference_bound (activePairs m 0) (blockData m 0) (blockData m' 0) hd 0

/-- Reused block keys and the independent polynomial key compose additively. -/
theorem raw_aggregate_collision_bound (m m' : Message)
    (hlen : m.length = m'.length) (hne : m ≠ m') :
    uniformProb (fun k : BlockKey × Word 126 =>
      aggregate (fun i : Fin (blockCount m) => rawBlock m k.1 i.val) k.2 =
      aggregate (fun i : Fin (blockCount m) => rawBlock m' k.1 i.val) k.2) ≤
      1 / (2 : ℚ≥0) ^ 64 +
        ((blockCount m - 1 : ℕ) : ℚ≥0) / (2 : ℚ≥0) ^ 126 := by
  let a := fun k : BlockKey => fun i : Fin (blockCount m) => rawBlock m k i.val
  let b := fun k : BlockKey => fun i : Fin (blockCount m) => rawBlock m' k i.val
  have hr : uniformProb (fun k : BlockKey => a k = b k) ≤ 1 / (2 : ℚ≥0) ^ 64 := by
    apply (uniformProb_mono (D := fun k : BlockKey =>
      ∀ t, t < blockCount m → rawBlock m k t = rawBlock m' k t) ?_).trans
        (rawBlocks_collision_bound m m' hlen hne)
    intro k hk t ht
    exact congrFun hk ⟨t, ht⟩
  exact compose_collision_bound a b (fun k v => aggregate v k) _ _ hr (by
    intro k hk
    exact aggregate_collision_bound (blockCount m) (a k) (b k)
      (fun i => rawBlock_degree m k i.val) (fun i => rawBlock_degree m' k i.val) hk)

/-- The two 1/2^64 terms are the block hash and the final reduction. -/
theorem long_equal_length_bound (m m' : Message)
    (hlen : m.length = m'.length) (hne : m ≠ m') (hlong : 1024 < m.length) :
    uniformProb (fun k : IdealKey => hash k m = hash k m') ≤
      2 / (2 : ℚ≥0) ^ 64 +
        ((blockCount m - 1 : ℕ) : ℚ≥0) / (2 : ℚ≥0) ^ 126 := by
  have hlong' : ¬ m'.length ≤ 1024 := by omega
  have hbc : blockCount m = blockCount m' := by simp [blockCount, hlen]
  let s := fun k : BlockKey × Word 126 =>
    aggregate (fun i : Fin (blockCount m) => rawBlock m k.1 i.val) k.2
  let s' := fun k : BlockKey × Word 126 =>
    aggregate (fun i : Fin (blockCount m) => rawBlock m' k.1 i.val) k.2
  rw [equal_length_content_prob m m' hlen]
  simp only [content, if_neg (by omega : ¬ m.length ≤ 1024), if_neg hlong']
  have ha (n n' : ℕ) (h : n = n') (f : ℕ → BitsPolynomial) (k : Word 126) :
      aggregate (fun i : Fin n => f i.val) k = aggregate (fun i : Fin n' => f i.val) k := by
    subst n'
    rfl
  simp_rw [ha _ _ hbc.symm]
  have hc := compose_collision_bound s s' (fun k p => AdjoinRoot.mk modulus (finalProduct p k))
    (1 / (2 : ℚ≥0) ^ 64 + ((blockCount m - 1 : ℕ) : ℚ≥0) / (2 : ℚ≥0) ^ 126)
    (1 / (2 : ℚ≥0) ^ 64) (raw_aggregate_collision_bound m m' hlen hne) (by
      intro k hk
      simpa only [sub_eq_zero] using finalProduct_difference_bound (s k) (s' k)
        (aggregate_degree _ _) (aggregate_degree _ _) hk 0)
  have he : (1 / (2 : ℚ≥0) ^ 64 +
      ((blockCount m - 1 : ℕ) : ℚ≥0) / (2 : ℚ≥0) ^ 126) + 1 / (2 : ℚ≥0) ^ 64 =
      2 / (2 : ℚ≥0) ^ 64 + ((blockCount m - 1 : ℕ) : ℚ≥0) / (2 : ℚ≥0) ^ 126 := by ring
  rw [he] at hc
  exact hc

/-- Ideal Algorithm 4 on all distinct byte strings shorter than 2^64 bytes.
The cap L counts 64-bit words and permits partial words and unequal lengths. -/
theorem clhash_collision_bound (L : ℕ) (m m' : Message)
    (hm : m.length < 2 ^ 64) (hm' : m'.length < 2 ^ 64)
    (hmL : m.length ≤ 8 * L) (hmL' : m'.length ≤ 8 * L) (hne : m ≠ m') :
    uniformProb (fun k : IdealKey => hash k m = hash k m') ≤ epsilon L := by
  have heps : 1 / (2 : ℚ≥0) ^ 64 ≤ epsilon L := by
    unfold epsilon
    split_ifs with h
    · exact le_rfl
    · calc
        _ ≤ 2 / (2 : ℚ≥0) ^ 64 := div_le_div_of_nonneg_right (by norm_num) (by positivity)
        _ ≤ _ := le_add_of_nonneg_right (by positivity)
  by_cases hlen : m.length = m'.length
  · by_cases hshort : m.length ≤ 1024
    · exact (short_equal_length_bound m m' hlen hne hshort).trans heps
    · have hL : ¬ L ≤ 128 := by omega
      have hc : blockCount m ≤ (L + 127) / 128 := by unfold blockCount; omega
      rw [epsilon, if_neg hL]
      exact (long_equal_length_bound m m' hlen hne (by omega)).trans
        (add_le_add_left (div_le_div_of_nonneg_right
          (by exact_mod_cast Nat.sub_le_sub_right hc 1) (by positivity)) _)
  · exact (unequal_lengths_bound m m' hm hm' hlen).trans heps

/-- The original specification is fully discharged. -/
theorem collision_bound : CollisionBound := clhash_collision_bound

end ProvenHashes.CLHash
