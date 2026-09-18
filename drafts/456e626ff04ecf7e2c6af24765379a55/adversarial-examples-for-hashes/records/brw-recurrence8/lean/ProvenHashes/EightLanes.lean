import ProvenHashes.Composition

namespace ProvenHashes.EightLanes
open scoped BigOperators
noncomputable section
variable {F : Type*} [Field F]

/-- Eight recurrence streams share u,y,z. Only the combining key is separate. -/
def hash (m : Fin 8 → List (F × F)) (k : (Fin 3 → F) × F) : F :=
  polynomialHash (fun j => Recurrence.hash (m j) k.1) k.2

/-- No independence among the eight lane values is used: select one differing lane. -/
theorem collision_bound [Fintype F] (n : ℕ) (m m' : Fin 8 → List (F × F))
    (hlen : ∀ j, (m j).length = (m' j).length)
    (hmax : ∀ j, (m j).length ≤ n) (hne : m ≠ m') :
    uniformProb (fun k => hash m k = hash m' k) ≤
      ((n + 7 : ℕ) : ℚ≥0) / Fintype.card F := by
  classical
  obtain ⟨j, hj⟩ : ∃ j, m j ≠ m' j := Function.ne_iff.mp hne
  have hi : uniformProb (fun k : Fin 3 → F =>
      (fun j => Recurrence.hash (m j) k) = (fun j => Recurrence.hash (m' j) k)) ≤
      (n : ℚ≥0) / Fintype.card F := by
    calc
      _ ≤ uniformProb (fun k => Recurrence.hash (m j) k = Recurrence.hash (m' j) k) :=
        uniformProb_mono (fun k h => congrFun h j)
      _ ≤ ((m j).length : ℚ≥0) / Fintype.card F := Recurrence.collision_bound _ _ (hlen j) hj
      _ ≤ _ := div_le_div_of_nonneg_right (by exact_mod_cast hmax j) (by positivity)
  calc
    _ ≤ (n : ℚ≥0) / Fintype.card F + (7 : ℚ≥0) / Fintype.card F :=
      compose_collision_bound
        (fun k j => Recurrence.hash (m j) k) (fun k j => Recurrence.hash (m' j) k)
        (fun k v => polynomialHash v k) _ _ hi
        (fun k hk => polynomial_collision_bound (L := 8) (by omega) _ _ hk)
    _ = _ := by push_cast; ring

/-- Zero extension at a fixed original length. -/
def word {L : ℕ} (m : Fin L → F) (i : ℕ) : F := if h : i < L then m ⟨i, h⟩ else 0

/-- ceil(ceil(L/2)/8), the maximum lane length. Only the final odd word is padded. -/
def rounds (L : ℕ) : ℕ := ((L + 1) / 2 + 7) / 8

/-- Number of actual message pairs dealt to lane j. -/
def laneLength (L : ℕ) (j : Fin 8) : ℕ := ((L + 1) / 2 + 7 - j.val) / 8

def lanes {L : ℕ} (m : Fin L → F) : Fin 8 → List (F × F) :=
  fun j => List.ofFn (fun r : Fin (laneLength L j) =>
    (word m (16 * r.val + 2 * j.val), word m (16 * r.val + 2 * j.val + 1)))

@[simp] theorem lanes_length {L : ℕ} (m : Fin L → F) (j : Fin 8) :
    (lanes m j).length = laneLength L j := by simp [lanes]

/-- Fixed-length zero padding and round-robin dealing preserve every input word. -/
theorem lanes_injective (L : ℕ) : Function.Injective (lanes (F := F) (L := L)) := by
  intro m m' h
  funext i
  have hi := i.isLt
  let j : Fin 8 := ⟨i.val % 16 / 2, by omega⟩
  let r : Fin (laneLength L j) := ⟨i.val / 16, by dsimp [laneLength, j]; omega⟩
  have hh := congrFun (List.ofFn_injective (congrFun h j)) r
  change (word m (16*r.val+2*j.val), word m (16*r.val+2*j.val+1)) =
    (word m' (16*r.val+2*j.val), word m' (16*r.val+2*j.val+1)) at hh
  by_cases he : i.val % 2 = 0
  · have idx : 16*r.val+2*j.val = i.val := by dsimp [r,j]; omega
    have hh' := congrArg Prod.fst hh
    simpa [idx, word, hi] using hh'
  · have idx : 16*r.val+2*j.val+1 = i.val := by dsimp [r,j]; omega
    have hh' := congrArg Prod.snd hh
    simpa [idx, word, hi] using hh'

/-- The concrete four-field-key, eight-lane hash of L field words. -/
def wordHash {L : ℕ} (m : Fin L → F) (k : (Fin 3 → F) × F) : F := hash (lanes m) k

/-- The chart row, including injective fixed-length padding and shared lane keys. -/
theorem word_collision_bound_gf64 {L : ℕ}
    (m m' : Fin L → GaloisField 2 64) (hne : m ≠ m') :
    uniformProb (fun k => wordHash m k = wordHash m' k) ≤
      ((rounds L + 7 : ℕ) : ℚ≥0) / 2^64 := by
  have hh := collision_bound (rounds L) (lanes m) (lanes m')
    (by intro j; simp) (by intro j; simp only [lanes_length]; unfold laneLength rounds; omega) (fun h => hne (lanes_injective L h))
  simpa only [wordHash, gf64_card, Nat.cast_pow, Nat.cast_ofNat] using hh

end
end ProvenHashes.EightLanes
