import ProvenHashes.CLHashBounds
import ProvenHashes.ByteInterface

noncomputable section
namespace ProvenHashes.CLHash
open ChainHash

/-- Native bytes and a full 64-bit output for the ideal Algorithm-4 family. -/
def hashBytes (k : IdealKey) (m : List UInt8) : UInt64 :=
  ⟨bitVecOfBits (hash k (bytesToMessage m))⟩

theorem hashBytes_eq_iff (k : IdealKey) (m m' : List UInt8) :
    hashBytes k m = hashBytes k m' ↔ hash k (bytesToMessage m) = hash k (bytesToMessage m') := by
  rw [← UInt64.toBitVec_inj]
  exact (bitsEquivBitVec 64).injective.eq_iff

/-- The requested two-regime theorem for native byte strings and UInt64 outputs. -/
theorem collision_bound_bytes (L : ℕ) (m m' : List UInt8)
    (hm : m.length < 2 ^ 64) (hm' : m'.length < 2 ^ 64)
    (hmL : m.length ≤ 8 * L) (hmL' : m'.length ≤ 8 * L) (hne : m ≠ m') :
    uniformProb (fun k : IdealKey => hashBytes k m = hashBytes k m') ≤
      if L ≤ 128 then 1 / (2 : ℚ≥0) ^ 64
      else 2 / (2 : ℚ≥0) ^ 64 +
        (((L + 127) / 128 - 1 : ℕ) : ℚ≥0) / (2 : ℚ≥0) ^ 126 := by
  simp_rw [hashBytes_eq_iff]
  apply clhash_collision_bound L (bytesToMessage m) (bytesToMessage m')
  · simpa only [bytesToMessage, List.length_map] using hm
  · simpa only [bytesToMessage, List.length_map] using hm'
  · simpa only [bytesToMessage, List.length_map] using hmL
  · simpa only [bytesToMessage, List.length_map] using hmL'
  · exact fun h => hne (bytesToMessage_injective h)

end ProvenHashes.CLHash
