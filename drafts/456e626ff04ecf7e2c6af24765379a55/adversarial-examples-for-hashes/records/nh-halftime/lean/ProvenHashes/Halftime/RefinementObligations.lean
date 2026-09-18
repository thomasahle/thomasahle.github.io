import ProvenHashes.Halftime.StyleSchedule
import ProvenHashes.Halftime.ExecutableBridge

/- These are precise remaining propositions, not assumptions or declarations
   of proofs. Nothing below supplies a proof of these definitions. -/
namespace ProvenHashes.Halftime

abbrev FixedMessage (b : ℕ) :=
  (bytes : Fin (168 * b * 19173961)) × (Fin bytes.val → Byte)

def messageArray {n : ℕ} (x : Fin n → Byte) : Array UInt8 :=
  Array.ofFn (fun i => UInt8.ofNat (x i).val)

def wordKeyArray {n : ℕ} (k : Fin n → Word64) : Array UInt64 :=
  Array.ofFn (fun i => UInt64.ofNat (k i).val)

noncomputable def xorKeyArray (k : Fin 6144 → XorWord 64) : Array UInt64 :=
  wordKeyArray (fun i => xorWordEquiv 64 (k i))

def fixedExec {b : ℕ} (x : FixedMessage b)
    (k : Fin (217 + 215*b) → Word64) : Fin 3 → Word64 :=
  fun i => ((Exec.core b 3 (wordKeyArray k) (messageArray x.2))[i.val]!).toNat

/-- Universal correspondence to some admissible public C++ leaf ordering. -/
def StyleRefinement : Prop :=
  ∀ (b : ℕ) (hb0 : 0 < b) (hb : b ≤ 8), b ∈ [1,2,4,8] →
    ∃ plans : StyleScheduleFamily b, ∀ (x : StyleMessage b) (k : Fin 6144 → XorWord 64),
      Exec.style b (xorKeyArray k) (messageArray x.2) =
        UInt64.ofNat ((xorWordEquiv 64) (byteStyleHash hb0 hb plans x k)).val

/-- The concrete executable distance-three statement, without an encoder hypothesis. -/
def Encode3Distance : Prop :=
  ∀ x y : Fin 21 → UInt64, x ≠ y →
    ∃ j : Fin 3 ↪ Fin 9, ∀ i,
      (fun c : Fin 3 => (Exec.encode 3 (Array.ofFn x))[3*(j i).val+c.val]!) ≠
      (fun c : Fin 3 => (Exec.encode 3 (Array.ofFn y))[3*(j i).val+c.val]!)

/-- The fixed 24-byte header's all-length ideal-key target bound. -/
def FixedHeaderBound : Prop :=
  ∀ (b : ℕ), b ∈ [1,2,4,8] → ∀ (x y : FixedMessage b), x ≠ y →
    ∀ target : Fin 3 → Word64,
      uniformProb (fun k => fixedExec x k - fixedExec y k = target) ≤
        6804 / (2 : ℚ≥0) ^ 96

/-- A separate probability theorem is required if the fixed benchmark adapter's
64-bit seed, rather than an ideal key array, is the uniform experiment. -/
def SeededStyleBound : Prop :=
  ∀ (b : ℕ), b ∈ [1,2,4,8] → ∀ (L : ℕ), 1 ≤ L →
    ∀ (x y : StyleMessage b), x ≠ y → x.1.val ≤ 8*L → y.1.val ≤ 8*L →
      uniformProb (fun seed : Word64 =>
        Exec.style b (Exec.seedExpand (UInt64.ofNat seed.val)) (messageArray x.2) =
        Exec.style b (Exec.seedExpand (UInt64.ofNat seed.val)) (messageArray y.2)) ≤
          (L : ℚ≥0) * (1 / 2^63 - 1 / 2^128)

end ProvenHashes.Halftime
