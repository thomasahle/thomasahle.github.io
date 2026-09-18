import ProvenHashes.UMASHModel

namespace ProvenHashes.UMASH

theorem encodeBlock_valid (m : Message) (i : ℕ) (hi : i < (m.length+255)/256) :
    (encodeBlock m i).Valid := by
  simp only [encodeBlock, Block.Valid, List.length_map, List.length_range]
  constructor
  · omega
  constructor
  · exact min_le_left _ _
  · trivial
-- CHECKPOINT

theorem encoded_blocks_valid (m : Message) : ∀ b ∈ encode m, b.Valid := by
  intro b hb
  obtain ⟨i, hi, rfl⟩ := List.mem_map.mp hb
  exact encodeBlock_valid m i (List.mem_range.mp hi)
-- CHECKPOINT

theorem encode_length (m : Message) : (encode m).length = (m.length+255)/256 := by
  simp only [encode, List.length_map, List.length_range]
-- CHECKPOINT

theorem compress_length (secondary : Bool) (k : OHKey) (seed : Word) (m : Message) :
    (compress secondary k seed m).length = (m.length+255)/256 := by
  simp only [compress, List.length_map, encode_length]
-- CHECKPOINT

theorem compress_length_le (L : ℕ) (secondary : Bool) (k : OHKey) (seed : Word)
    (m : Message) (hm : m.length ≤ 8*L) :
    (compress secondary k seed m).length ≤ (L+31)/32 := by
  rw [compress_length]
  omega
-- CHECKPOINT

end ProvenHashes.UMASH
