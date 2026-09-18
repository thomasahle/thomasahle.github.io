import ProvenHashes.Halftime.RefinementObligations

namespace ProvenHashes.Halftime
open scoped BigOperators
set_option maxHeartbeats 1000000
set_option maxRecDepth 4000

/-- Prefix invariant for the literal eight-iteration byte loader. -/
def loadPrefix (x : Array UInt8) (start : ℕ) : ℕ → UInt64
  | 0 => 0
  | n + 1 => loadPrefix x start n ||| (x[start+n]!.toUInt64 <<< UInt64.ofNat (8*n))

theorem load_eq_prefix (x : Array UInt8) (start : ℕ) :
    Exec.load x start = loadPrefix x start 8 := by
  simp [Exec.load, loadPrefix, Std.Range.forIn_eq_forIn_range', List.range',
    Id.run]
  rfl

/-- The loader's OR is addition of disjoint byte fields, with no overflow. -/
theorem loadPrefix_spec (x : Array UInt8) (start n : ℕ) (hn : n ≤ 8) :
    (loadPrefix x start n).toNat < 2 ^ (8*n) ∧
      (loadPrefix x start n).toNat =
        ∑ j ∈ Finset.range n, x[start+j]!.toNat * 2 ^ (8*j) := by
  induction n with
  | zero => simp [loadPrefix]
  | succ n ih =>
    obtain ⟨hbound, hsum⟩ := ih (by omega)
    have hshift : 8*n < 64 := by omega
    have hshift64 : 8*n < 2^64 := by omega
    have hp : 0 < 2^(8*n) := by positivity
    have hbyte := (x[start+n]!).toNat_lt
    have hpow : 2^(8*(n+1)) = 256 * 2^(8*n) := by
      rw [Nat.mul_add, pow_add]
      norm_num
      omega
    have hterm : x[start+n]!.toNat * 2^(8*n) < 2^64 := by
      have hle : 2^(8*(n+1)) ≤ 2^64 := Nat.pow_le_pow_right (by decide) (by omega)
      have hlt : x[start+n]!.toNat * 2^(8*n) < 2^(8*(n+1)) := by
        rw [hpow]
        exact Nat.mul_lt_mul_of_pos_right hbyte hp
      exact hlt.trans_le hle
    have hstep : (loadPrefix x start (n+1)).toNat =
        (loadPrefix x start n).toNat + x[start+n]!.toNat * 2^(8*n) := by
      simp only [loadPrefix, UInt64.toNat_or, UInt64.toNat_shiftLeft,
        UInt8.toNat_toUInt64, UInt64.toNat_ofNat', Nat.mod_eq_of_lt hshift64,
        Nat.mod_eq_of_lt hshift, Nat.shiftLeft_eq, Nat.mod_eq_of_lt hterm]
      rw [Nat.or_comm, Nat.mul_comm x[start+n]!.toNat,
        ← Nat.two_pow_add_eq_or_of_lt hbound]
      omega
    rw [hstep]
    constructor
    · rw [hpow]
      have hb : x[start+n]!.toNat + 1 ≤ 256 := hbyte
      nlinarith
    · rw [Finset.sum_range_succ, hsum]

/-- M5's equivalence uses the same little-endian byte weights. -/
theorem byteWordEquiv_val (x : Fin 8 → Byte) :
    (byteWordEquiv x).val = ∑ j : Fin 8, (x j).val * 256 ^ j.val := by
  rfl

/-- Universal agreement, including the executable's zero default beyond the array. -/
theorem load_agrees_byteWord (x : Array UInt8) (start : ℕ) :
    ((Exec.load x start).toNat : Word64) =
      byteWordEquiv (fun j => x[start+j.val]!.toFin) := by
  apply ZMod.val_injective
  rw [ZMod.val_natCast, Nat.mod_eq_of_lt (Exec.load x start).toNat_lt,
    byteWordEquiv_val, load_eq_prefix, (loadPrefix_spec x start 8 (by omega)).2]
  simp only [UInt8.toFin_val, ← Fin.sum_univ_eq_sum_range]
  apply Finset.sum_congr rfl
  intro j _
  rw [show (256 : ℕ) = 2^8 from rfl, ← pow_mul]

/-- Array conversion and out-of-range zero agree with M5's byte padding. -/
theorem messageArray_get_byte {n capacity : ℕ} (x : Fin n → Byte) (i : Fin capacity) :
    ((messageArray x)[i.val]!).toFin = zeroPad x i := by
  apply Fin.ext
  by_cases hi : i.val < n
  · simp [messageArray, zeroPad, hi]
  · simp [messageArray, zeroPad, hi]
    rfl

/-- Every scalar executable input word is exactly the corresponding M5 word. -/
theorem load_messageArray_agrees {n words : ℕ} (x : Fin n → Byte) (i : Fin words) :
    ((Exec.load (messageArray x) (8*i.val)).toNat : Word64) = paddedWords x i := by
  rw [load_agrees_byteWord]
  change byteWordEquiv _ = byteWordEquiv (fun j => zeroPad x (finProdFinEquiv (i,j)))
  congr 1
  funext j
  simpa [Nat.add_comm] using messageArray_get_byte x (finProdFinEquiv (i,j))

end ProvenHashes.Halftime
