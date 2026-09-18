import ProvenHashes.MultiplyShift

namespace ProvenHashes
open scoped BigOperators

/-- Equal-size fibers give the exact uniform counting distribution. -/
theorem uniformProb_of_equal_fibers {K V : Type*} [Fintype K] [Nonempty K]
    [Fintype V] [DecidableEq V] (f : K → V)
    (h : ∀ u v, (Finset.univ.filter (fun k => f k = u)).card =
      (Finset.univ.filter (fun k => f k = v)).card) (v : V) :
    uniformProb (fun k => f k = v) = 1 / Fintype.card V := by
  classical
  have hc : Fintype.card K = Fintype.card V *
      (Finset.univ.filter (fun k => f k = v)).card := by
    calc
      _ = ∑ u : V, (Finset.univ.filter (fun k => f k = u)).card :=
        Finset.card_eq_sum_card_fiberwise (fun _ _ => Finset.mem_univ _)
      _ = ∑ _u : V, (Finset.univ.filter (fun k => f k = v)).card :=
        Finset.sum_congr rfl (fun u _ => h u v)
      _ = _ := by simp
  have hk : (Fintype.card K : ℚ≥0) ≠ 0 := by exact_mod_cast Fintype.card_ne_zero
  have he : (Fintype.card K : ℚ≥0) = (Fintype.card V : ℚ≥0) *
      ((Finset.univ.filter (fun k => f k = v)).card : ℚ≥0) := by exact_mod_cast hc
  unfold uniformProb
  rw [he] at hk ⊢
  obtain ⟨hv, hf⟩ := mul_ne_zero_iff.mp hk
  field_simp
  congr 2
  ext k
  simp

/-- Translation witnesses for every output imply exact uniformity. -/
theorem uniformProb_of_translations {K V : Type*} [Fintype K] [AddGroup K]
    [Fintype V] [AddGroup V] (f : K → V)
    (h : ∀ t : V, ∃ d : K, ∀ k, f (k + d) = f k + t) (v : V) :
    uniformProb (fun k => f k = v) = 1 / Fintype.card V := by
  classical
  apply uniformProb_of_equal_fibers
  intro u v
  obtain ⟨d, hd⟩ := h (-u + v)
  apply Finset.card_bij (fun k _ => k + d)
  · intro k hk
    simp only [Finset.mem_filter, Finset.mem_univ, true_and] at hk ⊢
    rw [hd, hk]
    simp [← add_assoc]
  · intro a _ b _ hab
    exact add_right_cancel hab
  · intro k hk
    simp only [Finset.mem_filter, Finset.mem_univ, true_and] at hk
    refine ⟨k - d, ?_, sub_add_cancel _ _⟩
    simp only [Finset.mem_filter, Finset.mem_univ, true_and]
    have hh := hd (k - d)
    rw [sub_add_cancel, hk] at hh
    have := congrArg (fun z => z + (-v + u)) hh
    simpa [add_assoc] using this.symm

/-- The upper part of a modular word, with bucket width `B` and `Q` buckets. -/
def upperPart (B Q : ℕ) (z : ZMod (B * Q)) : ZMod Q := (z.val / B : ℕ)

theorem upperPart_translate (B Q : ℕ) [NeZero B] [NeZero Q]
    (z : ZMod (B * Q)) (t : ZMod Q) :
    upperPart B Q (z + (B * t.val : ℕ)) = upperPart B Q z + t := by
  have hB : 0 < B := Nat.pos_of_ne_zero (NeZero.ne B)
  unfold upperPart
  rw [ZMod.val_add, ZMod.val_natCast, Nat.add_mod_mod,
    Nat.mod_mul_right_div_self, Nat.add_mul_div_left _ _ hB]
  simp

/-- Explicit inverse of the odd part solves the bucket-sized difference equation. -/
theorem bucket_difference_solvable (w ℓ d : ℕ) (hℓ : ℓ ≤ w)
    (hd : 0 < d) (hdw : d < 2 ^ w) :
    ∃ a : ZMod (2 ^ (2 * w)), a * (d : ℕ) = (2 ^ (2 * w - ℓ) : ℕ) := by
  obtain ⟨s, z, hz, hdz⟩ := Nat.exists_eq_two_pow_mul_odd (Nat.ne_of_gt hd)
  have hzpos : 0 < z := by
    by_contra h
    have : z = 0 := by omega
    simp [this] at hdz
    omega
  have hsw : s < w := by
    have hp : 2 ^ s ≤ d := by rw [hdz]; exact Nat.le_mul_of_pos_right _ hzpos
    exact (Nat.pow_lt_pow_iff_right (by omega : 1 < 2)).mp (lt_of_le_of_lt hp hdw)
  have hs : s ≤ 2 * w - ℓ := by omega
  have hi : (z : ZMod (2 ^ (2 * w)))⁻¹ * z = 1 := by
    exact ZMod.inv_mul_of_unit _ ((ZMod.isUnit_iff_coprime _ _).mpr
      (hz.coprime_two_right.pow_right _))
  refine ⟨(z : ZMod (2 ^ (2 * w)))⁻¹ * 2 ^ (2 * w - ℓ - s), ?_⟩
  rw [hdz, Nat.cast_mul, Nat.cast_pow, Nat.cast_ofNat]
  calc
    _ = ((z : ZMod (2 ^ (2 * w)))⁻¹ * z) *
        (2 ^ (2 * w - ℓ - s) * 2 ^ s) := by ring
    _ = _ := by rw [hi, one_mul, ← pow_add, Nat.sub_add_cancel hs]; norm_cast

def highWord (w ℓ : ℕ) (z : ZMod (2 ^ (2 * w))) : ZMod (2 ^ ℓ) :=
  (z.val / 2 ^ (2 * w - ℓ) : ℕ)

theorem highWord_translate (w ℓ : ℕ) (hℓ : ℓ ≤ 2 * w)
    (z : ZMod (2 ^ (2 * w))) (t : ZMod (2 ^ ℓ)) :
    highWord w ℓ (z + (2 ^ (2 * w - ℓ) * t.val : ℕ)) = highWord w ℓ z + t := by
  have hm : 2 ^ (2 * w) = 2 ^ (2 * w - ℓ) * 2 ^ ℓ := by
    rw [← pow_add, Nat.sub_add_cancel hℓ]
  unfold highWord
  generalize hz : 2 ^ (2 * w) = M at z ⊢
  rw [hz] at hm
  subst M
  exact upperPart_translate _ _ z t

theorem word_difference_solvable (w ℓ : ℕ) (hℓ : ℓ ≤ w)
    (x y : Fin (2 ^ w)) (hne : x ≠ y) :
    ∃ a : ZMod (2 ^ (2 * w)),
      a * ((x.val : ZMod (2 ^ (2 * w))) - y.val) = (2 ^ (2 * w - ℓ) : ℕ) := by
  have hn : x.val ≠ y.val := fun h => hne (Fin.ext h)
  rcases lt_or_gt_of_ne hn with h | h
  · obtain ⟨a, ha⟩ := bucket_difference_solvable w ℓ (y.val - x.val) hℓ
      (by omega) (by have := y.isLt; omega)
    refine ⟨-a, ?_⟩
    rw [Nat.cast_sub (Nat.le_of_lt h)] at ha
    calc
      _ = a * ((y.val : ZMod (2 ^ (2 * w))) - x.val) := by ring
      _ = _ := ha
  · obtain ⟨a, ha⟩ := bucket_difference_solvable w ℓ (x.val - y.val) hℓ
      (by omega) (by have := x.isLt; omega)
    refine ⟨a, ?_⟩
    simpa only [Nat.cast_sub (Nat.le_of_lt h)] using ha

/-- Full independent coefficients, including the additive key. -/
abbrev VectorMASKey (w L : ℕ) :=
  (Fin L → ZMod (2 ^ (2 * w))) × ZMod (2 ^ (2 * w))

def vectorAffine (w L : ℕ) (k : VectorMASKey w L) (x : Fin L → Fin (2 ^ w)) :
    ZMod (2 ^ (2 * w)) := (∑ i, k.1 i * (x i).val) + k.2

def vectorMultiplyAddShift (w ℓ L : ℕ) (k : VectorMASKey w L)
    (x : Fin L → Fin (2 ^ w)) : ZMod (2 ^ ℓ) := highWord w ℓ (vectorAffine w L k x)

theorem vectorAffine_add (w L : ℕ) (k d : VectorMASKey w L)
    (x : Fin L → Fin (2 ^ w)) :
    vectorAffine w L (k + d) x = vectorAffine w L k x + vectorAffine w L d x := by
  simp only [vectorAffine, Prod.fst_add, Prod.snd_add, Pi.add_apply, add_mul,
    Finset.sum_add_distrib]
  ring

theorem vectorAffine_prescribed (w ℓ L : ℕ) (hℓ : ℓ ≤ w)
    (x y : Fin L → Fin (2 ^ w)) (hne : x ≠ y)
    (u v : ZMod (2 ^ ℓ)) :
    ∃ d : VectorMASKey w L,
      vectorAffine w L d x = (2 ^ (2 * w - ℓ) * u.val : ℕ) ∧
      vectorAffine w L d y = (2 ^ (2 * w - ℓ) * v.val : ℕ) := by
  classical
  obtain ⟨i, hi⟩ := Function.ne_iff.mp hne
  obtain ⟨c, hc⟩ := word_difference_solvable w ℓ hℓ (x i) (y i) hi
  let a : Fin L → ZMod (2 ^ (2 * w)) :=
    fun j => if j = i then c * ((u.val : ZMod (2 ^ (2 * w))) - v.val) else 0
  let d : VectorMASKey w L :=
    (a, (2 ^ (2 * w - ℓ) * v.val : ℕ) - ∑ j, a j * (y j).val)
  have hy : vectorAffine w L d y = (2 ^ (2 * w - ℓ) * v.val : ℕ) := by
    simp [vectorAffine, d]
  refine ⟨d, ?_, hy⟩
  have hdiff : vectorAffine w L d x - vectorAffine w L d y =
      (2 ^ (2 * w - ℓ) : ℕ) *
        ((u.val : ZMod (2 ^ (2 * w))) - v.val) := by
    simp only [vectorAffine, d, add_sub_add_right_eq_sub, ← Finset.sum_sub_distrib,
      ← mul_sub]
    simp only [a, ite_mul, zero_mul, Finset.sum_ite_eq', Finset.mem_univ, if_true]
    calc
      _ = (c * ((x i).val - (y i).val)) *
          ((u.val : ZMod (2 ^ (2 * w))) - v.val) := by ring
      _ = _ := by rw [hc]
  rw [hy] at hdiff
  push_cast at hdiff ⊢
  linear_combination hdiff

theorem vectorMAS_translations (w ℓ L : ℕ) (hℓ : ℓ ≤ w)
    (x y : Fin L → Fin (2 ^ w)) (hne : x ≠ y)
    (t : ZMod (2 ^ ℓ) × ZMod (2 ^ ℓ)) :
    ∃ d : VectorMASKey w L, ∀ k,
      (vectorMultiplyAddShift w ℓ L (k + d) x,
        vectorMultiplyAddShift w ℓ L (k + d) y) =
      (vectorMultiplyAddShift w ℓ L k x, vectorMultiplyAddShift w ℓ L k y) + t := by
  obtain ⟨d, hx, hy⟩ := vectorAffine_prescribed w ℓ L hℓ x y hne t.1 t.2
  refine ⟨d, fun k => ?_⟩
  apply Prod.ext
  · simp only [vectorMultiplyAddShift, vectorAffine_add, hx, Prod.fst_add]
    exact highWord_translate w ℓ (by omega) _ _
  · simp only [vectorMultiplyAddShift, vectorAffine_add, hy, Prod.snd_add]
    exact highWord_translate w ℓ (by omega) _ _

/-- Strong universality for the full-key, fixed-length vector family. -/
theorem vectorMultiplyAddShift_strong (w ℓ L : ℕ) (hℓ : ℓ ≤ w)
    (x y : Fin L → Fin (2 ^ w)) (hne : x ≠ y) (u v : ZMod (2 ^ ℓ)) :
    uniformProb (fun k : VectorMASKey w L =>
      vectorMultiplyAddShift w ℓ L k x = u ∧ vectorMultiplyAddShift w ℓ L k y = v) =
      1 / (2 : ℚ≥0) ^ (2 * ℓ) := by
  have h := uniformProb_of_translations
    (fun k : VectorMASKey w L =>
      (vectorMultiplyAddShift w ℓ L k x, vectorMultiplyAddShift w ℓ L k y))
    (vectorMAS_translations w ℓ L hℓ x y hne) (u, v)
  simpa [Prod.mk.injEq, Fintype.card_prod, ZMod.card, two_mul ℓ, pow_add] using h

/-- Exact collision probability; stronger than the row's requested upper bound. -/
theorem vectorMultiplyAddShift_collision_exact (w ℓ L : ℕ) (hℓ : ℓ ≤ w)
    (x y : Fin L → Fin (2 ^ w)) (hne : x ≠ y) :
    uniformProb (fun k : VectorMASKey w L =>
      vectorMultiplyAddShift w ℓ L k x = vectorMultiplyAddShift w ℓ L k y) =
      1 / (2 : ℚ≥0) ^ ℓ := by
  have h := uniformProb_of_translations
    (fun k : VectorMASKey w L =>
      vectorMultiplyAddShift w ℓ L k x - vectorMultiplyAddShift w ℓ L k y)
    (fun t => ?_) 0
  · simpa [sub_eq_zero, ZMod.card] using h
  · obtain ⟨d, hd⟩ := vectorMAS_translations w ℓ L hℓ x y hne (t, 0)
    refine ⟨d, fun k => ?_⟩
    have hx := congrArg Prod.fst (hd k)
    have hy := congrArg Prod.snd (hd k)
    simp only [Prod.fst_add, Prod.snd_add, add_zero] at hx hy
    dsimp only
    rw [hx, hy]
    abel

/-- This is the theorem certifying the vector multiply-shift chart row. -/
theorem vectorMultiplyAddShift64 (L : ℕ)
    (x y : Fin L → Fin (2 ^ 64)) (hne : x ≠ y) :
    uniformProb (fun k : VectorMASKey 64 L =>
      vectorMultiplyAddShift 64 64 L k x = vectorMultiplyAddShift 64 64 L k y) ≤
      1 / (2 : ℚ≥0) ^ 64 :=
  le_of_eq (vectorMultiplyAddShift_collision_exact 64 64 L (by omega) x y hne)

theorem highWord_val (w ℓ : ℕ) (hℓ : ℓ ≤ 2 * w) (z : ZMod (2 ^ (2 * w))) :
    (highWord w ℓ z).val = z.val / 2 ^ (2 * w - ℓ) := by
  have hm : 2 ^ (2 * w - ℓ) * 2 ^ ℓ = 2 ^ (2 * w) := by
    rw [← pow_add, Nat.sub_add_cancel hℓ]
  have hb : z.val / 2 ^ (2 * w - ℓ) < 2 ^ ℓ := by
    apply (Nat.div_lt_iff_lt_mul (by positivity)).mpr
    rw [Nat.mul_comm (2 ^ ℓ), hm]
    exact ZMod.val_lt z
  exact (ZMod.val_natCast _ _).trans (Nat.mod_eq_of_lt hb)

/-- The ring-valued definition computes exactly the stated unsigned shift. -/
theorem vectorMultiplyAddShift_val (w ℓ L : ℕ) (hℓ : ℓ ≤ 2 * w)
    (k : VectorMASKey w L) (x : Fin L → Fin (2 ^ w)) :
    (vectorMultiplyAddShift w ℓ L k x).val =
      ((∑ i, (k.1 i).val * (x i).val) + k.2.val) % 2 ^ (2 * w) /
        2 ^ (2 * w - ℓ) := by
  rw [vectorMultiplyAddShift, highWord_val _ _ hℓ]
  have h : vectorAffine w L k x =
      (((∑ i, (k.1 i).val * (x i).val) + k.2.val : ℕ) : ZMod (2 ^ (2 * w))) := by
    simp [vectorAffine, Nat.cast_sum]
  rw [h, ZMod.val_natCast]

abbrev ScalarMASKey (w : ℕ) := ZMod (2 ^ (2 * w)) × ZMod (2 ^ (2 * w))

def multiplyAddShift (w ℓ : ℕ) (k : ScalarMASKey w) (x : Fin (2 ^ w)) :
    ZMod (2 ^ ℓ) := highWord w ℓ (k.1 * x.val + k.2)

def scalarVectorKeyEquiv (w : ℕ) : ScalarMASKey w ≃ VectorMASKey w 1 where
  toFun k := (fun _ => k.1, k.2)
  invFun k := (k.1 0, k.2)
  left_inv _ := rfl
  right_inv k := by
    apply Prod.ext
    · funext i
      change k.1 0 = k.1 i
      exact congrArg k.1 (Subsingleton.elim _ _)
    · rfl

theorem scalar_vector_hash (w ℓ : ℕ) (k : ScalarMASKey w) (x : Fin (2 ^ w)) :
    vectorMultiplyAddShift w ℓ 1 (scalarVectorKeyEquiv w k) (fun _ => x) =
      multiplyAddShift w ℓ k x := by
  simp [vectorMultiplyAddShift, vectorAffine, scalarVectorKeyEquiv, multiplyAddShift]

theorem multiplyAddShift_strong (w ℓ : ℕ) (hℓ : ℓ ≤ w)
    (x y : Fin (2 ^ w)) (hne : x ≠ y) (u v : ZMod (2 ^ ℓ)) :
    uniformProb (fun k : ScalarMASKey w =>
      multiplyAddShift w ℓ k x = u ∧ multiplyAddShift w ℓ k y = v) =
      1 / (2 : ℚ≥0) ^ (2 * ℓ) := by
  have hne' : (fun _ : Fin 1 => x) ≠ (fun _ : Fin 1 => y) :=
    fun h => hne (congrFun h 0)
  have he := uniformProb_equiv (scalarVectorKeyEquiv w)
    (fun k => vectorMultiplyAddShift w ℓ 1 k (fun _ => x) = u ∧
      vectorMultiplyAddShift w ℓ 1 k (fun _ => y) = v)
  simp only [scalar_vector_hash] at he
  exact he.trans (vectorMultiplyAddShift_strong w ℓ 1 hℓ _ _ hne' u v)

theorem multiplyAddShift_collision_exact (w ℓ : ℕ) (hℓ : ℓ ≤ w)
    (x y : Fin (2 ^ w)) (hne : x ≠ y) :
    uniformProb (fun k : ScalarMASKey w => multiplyAddShift w ℓ k x = multiplyAddShift w ℓ k y) =
      1 / (2 : ℚ≥0) ^ ℓ := by
  have hne' : (fun _ : Fin 1 => x) ≠ (fun _ : Fin 1 => y) :=
    fun h => hne (congrFun h 0)
  have he := uniformProb_equiv (scalarVectorKeyEquiv w)
    (fun k => vectorMultiplyAddShift w ℓ 1 k (fun _ => x) =
      vectorMultiplyAddShift w ℓ 1 k (fun _ => y))
  simp only [scalar_vector_hash] at he
  exact he.trans (vectorMultiplyAddShift_collision_exact w ℓ 1 hℓ _ _ hne')

theorem multiplyAddShift_val (w ℓ : ℕ) (hℓ : ℓ ≤ 2 * w)
    (k : ScalarMASKey w) (x : Fin (2 ^ w)) :
    (multiplyAddShift w ℓ k x).val =
      (k.1.val * x.val + k.2.val) % 2 ^ (2 * w) / 2 ^ (2 * w - ℓ) := by
  have h := vectorMultiplyAddShift_val w ℓ 1 hℓ (scalarVectorKeyEquiv w k) (fun _ => x)
  rw [scalar_vector_hash] at h
  simpa [scalarVectorKeyEquiv] using h

theorem vectorMASKey_card (w L : ℕ) :
    Fintype.card (VectorMASKey w L) = (2 ^ (2 * w)) ^ (L + 1) := by
  simp [VectorMASKey, pow_succ]

/-- The row bound written directly as natural-number modulo and right shift. -/
theorem vectorMultiplyAddShift64_formula_bound (L : ℕ)
    (x y : Fin L → Fin (2 ^ 64)) (hne : x ≠ y) :
    uniformProb (fun k : VectorMASKey 64 L =>
      ((∑ i, (k.1 i).val * (x i).val) + k.2.val) % 2 ^ 128 / 2 ^ 64 =
      ((∑ i, (k.1 i).val * (y i).val) + k.2.val) % 2 ^ 128 / 2 ^ 64) ≤
      1 / (2 : ℚ≥0) ^ 64 := by
  have he : (fun k : VectorMASKey 64 L =>
      ((∑ i, (k.1 i).val * (x i).val) + k.2.val) % 2 ^ 128 / 2 ^ 64 =
      ((∑ i, (k.1 i).val * (y i).val) + k.2.val) % 2 ^ 128 / 2 ^ 64) =
      (fun k => vectorMultiplyAddShift 64 64 L k x = vectorMultiplyAddShift 64 64 L k y) := by
    funext k
    apply propext
    have hx := vectorMultiplyAddShift_val 64 64 L (by omega) k x
    have hy := vectorMultiplyAddShift_val 64 64 L (by omega) k y
    change (vectorMultiplyAddShift 64 64 L k x).val =
      ((∑ i, (k.1 i).val * (x i).val) + k.2.val) % 2 ^ 128 / 2 ^ 64 at hx
    change (vectorMultiplyAddShift 64 64 L k y).val =
      ((∑ i, (k.1 i).val * (y i).val) + k.2.val) % 2 ^ 128 / 2 ^ 64 at hy
    rw [← hx, ← hy]
    exact (ZMod.val_injective _).eq_iff
  rw [he]
  exact vectorMultiplyAddShift64 L x y hne

/-- The chart score, minimized over positive fixed lengths. -/
theorem vectorMultiplyAddShift64_score :
    IsLeast {s : ℝ | ∃ L : ℕ, 1 ≤ L ∧
      s = Real.logb 2 ((L : ℝ) / (1 / (2 : ℝ) ^ 64))} 64 := by
  have he : Real.logb 2 ((2 : ℝ) ^ 64) = 64 := by
    rw [Real.logb_pow]
    norm_num
  constructor
  · refine ⟨1, by omega, ?_⟩
    simpa using he.symm
  · rintro s ⟨L, hL, rfl⟩
    rw [← he]
    apply Real.logb_le_logb_of_le (by norm_num) (by positivity)
    have hL' : (1 : ℝ) ≤ L := by exact_mod_cast hL
    simp only [div_div_eq_mul_div, div_one]
    nlinarith [pow_pos (by norm_num : (0 : ℝ) < 2) 64]

end ProvenHashes
