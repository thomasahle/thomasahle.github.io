import ProvenHashes.Halftime.IntegerNH

namespace ProvenHashes.Halftime

/-- UMAC integer NH: 64-bit wrapped additions and full 128-bit output. -/
def nh64 {n : ℕ} (x k : Fin n × Bool → ZMod (2 ^ 64)) : ZMod (2 ^ 128) :=
  nh x k

theorem nh64_adu {n : ℕ} (x y : Fin n × Bool → ZMod (2 ^ 64))
    (hxy : x ≠ y) (t : ZMod (2 ^ 128)) :
    uniformProb (fun k => nh64 x k - nh64 y k = t) ≤ 1 / (2 : ℚ≥0) ^ 64 := by
  simpa [nh64] using nh_adu x y hxy t

/-- Pair consecutive words; append one zero precisely when the length is odd. -/
def pad64 {n : ℕ} (x : Fin n → ZMod (2 ^ 64)) :
    Fin ((n + 1) / 2) × Bool → ZMod (2 ^ 64) :=
  fun i => if h : 2 * i.1.val + (if i.2 then 1 else 0) < n then
    x ⟨2 * i.1.val + (if i.2 then 1 else 0), h⟩ else 0

theorem pad64_injective (n : ℕ) : Function.Injective (@pad64 n) := by
  intro x y h
  funext j
  let i : Fin ((n + 1) / 2) × Bool := (⟨j.val / 2, by omega⟩, decide (j.val % 2 = 1))
  have hi : 2 * i.1.val + (if i.2 then 1 else 0) = j.val := by
    dsimp [i]
    split_ifs <;> simp_all <;> omega
  have he := congrFun h i
  simpa only [pad64, hi, dif_pos j.isLt] using he

/-- Fixed original word length, including odd lengths with one zero word. -/
theorem nh64_padded_adu {n : ℕ} (x y : Fin n → ZMod (2 ^ 64))
    (hxy : x ≠ y) (t : ZMod (2 ^ 128)) :
    uniformProb (fun k => nh64 (pad64 x) k - nh64 (pad64 y) k = t) ≤
      1 / (2 : ℚ≥0) ^ 64 := by
  exact nh64_adu _ _ (fun h => hxy (pad64_injective n h)) t

theorem nh64_padded_collision {n : ℕ} (x y : Fin n → ZMod (2 ^ 64))
    (hxy : x ≠ y) :
    uniformProb (fun k => nh64 (pad64 x) k = nh64 (pad64 y) k) ≤
      1 / (2 : ℚ≥0) ^ 64 := by
  simpa only [sub_eq_zero] using nh64_padded_adu x y hxy 0

def witnessZero : Fin 1 × Bool → ZMod (2 ^ 64) := fun _ => 0
def witnessOne : Fin 1 × Bool → ZMod (2 ^ 64) := fun p => if p.2 then 0 else 1

/-- The unpadded two-word pair (0,0), (1,0) attains the NH upper bound. -/
theorem nh64_witness_probability :
    uniformProb (fun k => nh64 witnessZero k = nh64 witnessOne k) =
      1 / (2 : ℚ≥0) ^ 64 := by
  have hne : witnessZero ≠ witnessOne := by
    intro h
    have := congrFun h (0, false)
    norm_num [witnessZero, witnessOne] at this
    have hv := congrArg ZMod.val this
    norm_num [ZMod.val_one_eq_one_mod] at hv
  apply le_antisymm
  · simpa only [sub_eq_zero] using nh64_adu witnessZero witnessOne hne 0
  · have hp : uniformProb (fun k : Fin 1 × Bool → ZMod (2 ^ 64) => k (0,true) = 0) =
        1 / (2 : ℚ≥0) ^ 64 := by
      simpa only [ZMod.card, Nat.cast_pow, Nat.cast_ofNat] using
        uniformProb_of_bijective_update (fun k : Fin 1 × Bool → ZMod (2 ^ 64) => k (0,true))
          (0,true) (by intro k; simpa using Function.bijective_id) 0
    rw [← hp]
    apply uniformProb_mono
    intro k hk
    simp [nh64, nh, witnessZero, witnessOne, hk]

/-- Dividing by the unpadded length (at least two words) gives score 65. -/
theorem nh64_unpadded_normalized {n : ℕ} (hn : 1 ≤ n)
    (x y : Fin n × Bool → ZMod (2 ^ 64)) (hxy : x ≠ y) :
    uniformProb (fun k => nh64 x k = nh64 y k) / (2 * n : ℚ≥0) ≤
      1 / (2 : ℚ≥0) ^ 65 := by
  have H : uniformProb (fun k => nh64 x k = nh64 y k) ≤ 1 / (2 : ℚ≥0) ^ 64 := by
    simpa only [sub_eq_zero] using nh64_adu x y hxy 0
  have hn' : (2 : ℚ≥0) ≤ 2 * n := by exact_mod_cast (by omega : 2 ≤ 2 * n)
  calc
    _ ≤ (1 / (2 : ℚ≥0) ^ 64) / (2 * n) := div_le_div_of_nonneg_right H (by positivity)
    _ ≤ (1 / (2 : ℚ≥0) ^ 64) / 2 := div_le_div_of_nonneg_left (by positivity) (by norm_num) hn'
    _ = _ := by norm_num

/-- A positive word cap gives the advertised normalized score at least 64. -/
theorem nh64_padded_normalized {n L : ℕ} (hL : 1 ≤ L)
    (x y : Fin n → ZMod (2 ^ 64)) (hxy : x ≠ y) :
    uniformProb (fun k => nh64 (pad64 x) k = nh64 (pad64 y) k) / (L : ℚ≥0) ≤
      1 / (2 : ℚ≥0) ^ 64 := by
  have hL' : (1 : ℚ≥0) ≤ L := by exact_mod_cast hL
  calc
    _ ≤ (1 / (2 : ℚ≥0) ^ 64) / L :=
      div_le_div_of_nonneg_right (nh64_padded_collision x y hxy) (by positivity)
    _ ≤ (1 / (2 : ℚ≥0) ^ 64) / 1 :=
      div_le_div_of_nonneg_left (by positivity) (by norm_num) hL'
    _ = _ := div_one _

/-- One-word messages 0 and 1 attain the padded score-64 bound. -/
theorem nh64_padded_witness_probability :
    uniformProb (fun k => nh64 (pad64 (fun _ : Fin 1 => 0)) k =
      nh64 (pad64 (fun _ : Fin 1 => 1)) k) = 1 / (2 : ℚ≥0) ^ 64 := by
  have h0 : pad64 (fun _ : Fin 1 => 0) = witnessZero := by
    funext ⟨i,b⟩
    fin_cases i
    cases b <;> rfl
  have h1 : pad64 (fun _ : Fin 1 => 1) = witnessOne := by
    funext ⟨i,b⟩
    fin_cases i
    cases b <;> rfl
  rw [h0, h1]
  exact nh64_witness_probability

/-- The two-word witness attains the normalized unpadded value 2^-65. -/
theorem nh64_unpadded_score_sharp :
    uniformProb (fun k => nh64 witnessZero k = nh64 witnessOne k) / 2 =
      1 / (2 : ℚ≥0) ^ 65 := by
  rw [nh64_witness_probability]
  norm_num

end ProvenHashes.Halftime
