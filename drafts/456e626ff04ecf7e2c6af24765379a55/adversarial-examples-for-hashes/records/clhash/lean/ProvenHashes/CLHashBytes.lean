import ProvenHashes.Carryless
import ProvenHashes.ByteEncoding

noncomputable section
namespace ProvenHashes.CLHash
open ChainHash

abbrev BlockKey := Fin 64 × Bool → Word 64

/-- No implicit extra block: the empty message takes the short branch. -/
def blockCount (m : Message) : ℕ := (m.length + 1023) / 1024
def blockBytes (m : Message) (t : ℕ) : ℕ := min 1024 (m.length - 1024 * t)

/-- The tail includes exactly the pairs intersecting the message. -/
def activePairs (m : Message) (t : ℕ) : Finset (Fin 64) :=
  Finset.univ.filter fun i => i.val < (blockBytes m t + 15) / 16

/-- Adjacent words, little endian, zero padding at both byte and word boundaries. -/
def blockData (m : Message) (t : ℕ) (j : Fin 64 × Bool) : Word 64 :=
  wordAt m (128 * t + 2 * j.1.val + if j.2 then 1 else 0)

def rawBlock (m : Message) (k : BlockKey) (t : ℕ) : BitsPolynomial :=
  clnh (activePairs m t) (blockData m t) k

theorem rawBlock_degree (m : Message) (k : BlockKey) (t : ℕ) :
    (rawBlock m k t).natDegree < 127 := by
  have h := clnh_natDegree_le (activePairs m t) (blockData m t) k
  exact Nat.lt_succ_of_le h

theorem block_encoding_injective (m m' : Message) (hlen : m.length = m'.length)
    (hblocks : ∀ t, t < blockCount m → ∀ s ∈ activePairs m t, ∀ b,
      blockData m t (s, b) = blockData m' t (s, b)) : m = m' := by
  apply byte_words_injective m m' hlen
  intro j hj
  let s : Fin 64 := ⟨j % 128 / 2, by omega⟩
  let b : Bool := decide (j % 2 = 1)
  have ht : j / 128 < blockCount m := by unfold blockCount; omega
  have hs : s ∈ activePairs m (j / 128) := by
    simp only [activePairs, Finset.mem_filter, Finset.mem_univ, true_and]
    dsimp [s, blockBytes]
    omega
  have he := hblocks (j / 128) ht s hs b
  have hidx : 128 * (j / 128) + 2 * s.val + (if b then 1 else 0) = j := by
    dsimp [s, b]
    simp only [decide_eq_true_eq]
    split_ifs <;> omega
  simpa only [blockData, hidx] using he

/-- Reusing the same 128 key words for all blocks costs only one CLNH bound. -/
theorem rawBlocks_collision_bound (m m' : Message)
    (hlen : m.length = m'.length) (hne : m ≠ m') :
    uniformProb (fun k : BlockKey =>
      ∀ t, t < blockCount m → rawBlock m k t = rawBlock m' k t) ≤
        1 / (2 : ℚ≥0) ^ 64 := by
  classical
  have hex : ∃ t, t < blockCount m ∧ ∃ s ∈ activePairs m t, ∃ b,
      blockData m t (s, b) ≠ blockData m' t (s, b) := by
    by_contra! h
    exact hne (block_encoding_injective m m' hlen h)
  obtain ⟨t, ht, s, hs, b, hb⟩ := hex
  have ha : activePairs m t = activePairs m' t := by
    simp [activePairs, blockBytes, hlen]
  refine (uniformProb_mono (D := fun k : BlockKey =>
    clnh (activePairs m t) (blockData m t) k -
      clnh (activePairs m t) (blockData m' t) k = 0) ?_).trans
        (clnh_difference_bound _ _ _ ⟨s, hs, b, hb⟩ 0)
  intro k hk
  have he := hk t ht
  simpa only [rawBlock, ← ha, sub_eq_zero] using he

end ProvenHashes.CLHash
