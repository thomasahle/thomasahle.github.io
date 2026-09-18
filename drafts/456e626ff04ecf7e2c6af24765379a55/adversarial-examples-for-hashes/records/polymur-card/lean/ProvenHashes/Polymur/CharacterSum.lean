import ProvenHashes.Polymur.Keys
import Mathlib.NumberTheory.JacobiSum.Basic
import Mathlib.NumberTheory.DirichletCharacter.Bounds
import Mathlib.Analysis.Fourier.ZMod

noncomputable section
open Finset
open scoped ComplexConjugate
namespace ProvenHashes.Polymur

variable {q : ℕ} [Fact q.Prime]
local notation "Q" => ZMod q

/-- The squared absolute value of every nontrivial Gauss sum is the field size. -/
theorem gaussSum_norm_sq (χ : MulChar Q ℂ) (hχ : χ ≠ 1) :
    ‖gaussSum χ ZMod.stdAddChar‖ ^ 2 = (q : ℝ) := by
  have hs : star (gaussSum χ ZMod.stdAddChar) =
      gaussSum χ⁻¹ (ZMod.stdAddChar : AddChar Q ℂ)⁻¹ := by
    simp only [gaussSum, star_sum, star_mul, MulChar.star_apply']
    apply Finset.sum_congr rfl
    intro x _
    have h := AddChar.starComp_apply (R := Q)
      (φ := (ZMod.stdAddChar : AddChar Q ℂ))
      (by simpa only [ZMod.ringChar_zmod_n] using (Fact.out : q.Prime).pos) x
    change star (ZMod.stdAddChar x) = _ at h
    rw [h]
    ring
  have hg := gaussSum_mul_gaussSum_eq_card hχ (ZMod.isPrimitive_stdAddChar q)
  rw [← hs] at hg
  have hr := congrArg Complex.re hg
  simpa [Complex.mul_conj, Complex.normSq_eq_norm_sq, ← Complex.ofReal_pow, ZMod.card] using hr

/-- Adjacent translates of a nontrivial multiplicative character have correlation -1. -/
theorem char_correlation_one (χ : MulChar Q ℂ) (hχ : χ ≠ 1) :
    ∑ x : Q, χ x * star (χ (x + 1)) = -1 := by
  have hs : (∑ x : Q, χ x * star (χ (x + 1))) =
      χ (-1) * jacobiSum χ χ⁻¹ := by
    rw [← Equiv.sum_comp (Equiv.neg Q) (fun x => χ x * star (χ (x + 1)))]
    simp only [Equiv.neg_apply, MulChar.star_apply', jacobiSum, Finset.mul_sum]
    apply Finset.sum_congr rfl
    intro x _
    rw [show -x + 1 = 1 - x by ring, show -x = -1 * x by ring, map_mul]
    ring
  rw [hs, jacobiSum_nontrivial_inv hχ]
  have hv : χ (-1) ^ 2 = 1 := by rw [← map_pow]; norm_num
  calc
    χ (-1) * -χ (-1) = -(χ (-1) ^ 2) := by ring
    _ = -1 := by rw [hv]

/-- Exact correlation of any two additive translates. -/
theorem char_correlation (χ : MulChar Q ℂ) (hχ : χ ≠ 1) (a b : Q) :
    (∑ x : Q, χ (x + a) * star (χ (x + b))) =
      if a = b then (q : ℂ) - 1 else -1 := by
  by_cases hab : a = b
  · subst b
    rw [if_pos rfl]
    have he := Equiv.sum_comp (Equiv.addRight a)
      (fun x : Q => χ x * star (χ x))
    simp only [Equiv.coe_addRight] at he
    rw [he]
    simp only [MulChar.star_apply', ← MulChar.mul_apply, mul_inv_cancel]
    rw [MulChar.sum_one_eq_card_units]
    have hc := Fintype.card_eq_card_units_add_one (α := Q)
    rw [ZMod.card] at hc
    have hu : Fintype.card Qˣ = q - 1 := by omega
    rw [hu]
    rw [Nat.cast_sub (Fact.out : q.Prime).one_le]
    norm_num
  · rw [if_neg hab]
    have hd : b - a ≠ 0 := sub_ne_zero.mpr (Ne.symm hab)
    have hp : χ (b - a) * star (χ (b - a)) = 1 := by
      rw [MulChar.star_apply', ← MulChar.mul_apply, mul_inv_cancel,
        MulChar.one_apply (isUnit_iff_ne_zero.mpr hd)]
    calc
      (∑ x : Q, χ (x + a) * star (χ (x + b))) =
          ∑ x : Q, χ ((b-a)*x-a+a) * star (χ ((b-a)*x-a+b)) := by
        exact (Fintype.sum_bijective (fun x : Q => (b-a)*x-a)
          ((Equiv.subRight a).bijective.comp (mulLeft_bijective₀ (b-a) hd))
          _ _ (fun _ => rfl)).symm
      _ = ∑ x : Q, χ x * star (χ (x+1)) := by
        apply Finset.sum_congr rfl
        intro x _
        rw [show (b-a)*x-a+a = (b-a)*x by ring,
          show (b-a)*x-a+b = (b-a)*(x+1) by ring, map_mul, map_mul, star_mul]
        calc
          _ = (χ (b-a) * star (χ (b-a))) * (χ x * star (χ (x+1))) := by ring
          _ = _ := by rw [hp, one_mul]
      _ = -1 := char_correlation_one χ hχ

/-- The exact energy identity behind the interval completion estimate. -/
theorem char_weighted_energy (χ : MulChar Q ℂ) (hχ : χ ≠ 1) (g : Q → ℂ) :
    (∑ x : Q, ‖∑ y : Q, g y * χ (x+y)‖ ^ 2) =
      (q : ℝ) * (∑ y : Q, ‖g y‖ ^ 2) - ‖∑ y : Q, g y‖ ^ 2 := by
  apply Complex.ofReal_injective
  simp only [Complex.ofReal_sum, Complex.ofReal_sub, Complex.ofReal_mul,
    Complex.ofReal_natCast, ← Complex.normSq_eq_norm_sq, ← Complex.mul_conj]
  change (∑ x : Q, (∑ y : Q, g y * χ (x+y)) * star (∑ y : Q, g y * χ (x+y))) =
    (q : ℂ) * (∑ y : Q, g y * star (g y)) - (∑ y : Q, g y) * star (∑ y : Q, g y)
  have he : (∑ x : Q, (∑ y : Q, g y * χ (x+y)) * star (∑ y : Q, g y * χ (x+y))) =
      ∑ y : Q, ∑ z : Q, (g z * star (g y)) *
        (∑ x : Q, χ (x+z) * star (χ (x+y))) := by
    simp only [star_sum, star_mul, Finset.sum_mul, Finset.mul_sum]
    rw [Finset.sum_comm]
    apply Finset.sum_congr rfl
    intro y _
    rw [Finset.sum_comm]
    apply Finset.sum_congr rfl
    intro z _
    apply Finset.sum_congr rfl
    intro x _
    ring
  rw [he]
  have hi (a b : Q) : (if a = b then (q : ℂ) - 1 else -1) =
      (if a = b then (q : ℂ) else 0) - 1 := by split_ifs <;> ring
  simp_rw [char_correlation χ hχ, hi, mul_sub, mul_ite, mul_zero, mul_one,
    Finset.sum_sub_distrib]
  simp only [Finset.sum_ite_eq', Finset.mem_univ, if_true]
  simp only [star_sum, Finset.sum_mul, Finset.mul_sum]
  congr 1
  apply Finset.sum_congr rfl
  intro y _
  ring

/-- Cauchy--Schwarz and the exact character energy, with the mean term retained. -/
theorem char_bilinear_sq (χ : MulChar Q ℂ) (hχ : χ ≠ 1) (f g : Q → ℂ) :
    ‖∑ x : Q, f x * (∑ y : Q, g y * χ (x+y))‖ ^ 2 ≤
      (∑ x : Q, ‖f x‖ ^ 2) *
        ((q : ℝ) * (∑ y : Q, ‖g y‖ ^ 2) - ‖∑ y : Q, g y‖ ^ 2) := by
  calc
    _ ≤ (∑ x : Q, ‖f x‖ * ‖∑ y : Q, g y * χ (x+y)‖) ^ 2 := by
      apply pow_le_pow_left₀ (norm_nonneg _)
      simpa only [norm_mul] using
        norm_sum_le (s := (Finset.univ : Finset Q))
          (fun x => f x * (∑ y : Q, g y * χ (x+y)))
    _ ≤ (∑ x : Q, ‖f x‖ ^ 2) *
        (∑ x : Q, ‖∑ y : Q, g y * χ (x+y)‖ ^ 2) :=
      Finset.sum_mul_sq_le_sq_mul_sq _ _ _
    _ = _ := by rw [char_weighted_energy χ hχ]

/-- An initial interval, interpreted as residues. -/
def residueInterval (q n : ℕ) : Finset (ZMod q) :=
  (Finset.range n).image (fun i : ℕ => (i : ZMod q))

lemma sum_residueInterval (n : ℕ) (hn : n ≤ q) (f : Q → ℂ) :
    (∑ x ∈ residueInterval q n, f x) = ∑ i ∈ Finset.range n, f (i : Q) := by
  unfold residueInterval
  apply Finset.sum_image
  intro a ha b hb hab
  have := congrArg ZMod.val hab
  simpa only [ZMod.val_natCast,
    Nat.mod_eq_of_lt ((Finset.mem_range.mp ha).trans_le hn),
    Nat.mod_eq_of_lt ((Finset.mem_range.mp hb).trans_le hn)] using this

/-- A signed difference of two sets has energy at most their combined size. -/
theorem char_set_difference_sq (χ : MulChar Q ℂ) (hχ : χ ≠ 1)
    (A B C : Finset Q) :
    ‖(∑ x ∈ A, ∑ y ∈ C, χ (x+y)) - (∑ x ∈ B, ∑ y ∈ C, χ (x+y))‖ ^ 2 ≤
      ((A.card : ℝ) + B.card) * ((q : ℝ) * C.card) := by
  classical
  let f : Q → ℂ := fun x => (if x ∈ A then 1 else 0) - (if x ∈ B then 1 else 0)
  let g : Q → ℂ := fun y => if y ∈ C then 1 else 0
  have hf : (∑ x : Q, ‖f x‖ ^ 2) ≤ (A.card : ℝ) + B.card := by
    calc
      _ ≤ ∑ x : Q, ((if x ∈ A then 1 else 0 : ℝ) + (if x ∈ B then 1 else 0)) := by
        apply Finset.sum_le_sum
        intro x _
        dsimp [f]
        split_ifs <;> norm_num
      _ = _ := by simp [Finset.sum_add_distrib]
  have hg := char_bilinear_sq χ hχ f g
  have hh : (∑ x : Q, f x * (∑ y : Q, g y * χ (x+y))) =
      (∑ x ∈ A, ∑ y ∈ C, χ (x+y)) - (∑ x ∈ B, ∑ y ∈ C, χ (x+y)) := by
    simp [f, g, sub_mul, Finset.sum_sub_distrib]
  rw [hh] at hg
  have hgg : (∑ y : Q, ‖g y‖ ^ 2) = (C.card : ℝ) := by simp [g, apply_ite]
  rw [hgg] at hg
  refine hg.trans ?_
  calc
    _ ≤ (∑ x : Q, ‖f x‖ ^ 2) * ((q : ℝ) * C.card) := by
      exact mul_le_mul_of_nonneg_left (sub_le_self _ (sq_nonneg _)) (by positivity)
    _ ≤ _ := mul_le_mul_of_nonneg_right hf (by positivity)

lemma card_residueInterval (n : ℕ) (hn : n ≤ q) :
    (residueInterval q n).card = n := by
  have h := sum_residueInterval n hn (fun _ => (1 : ℂ))
  simp only [Finset.sum_const, Finset.card_range, nsmul_eq_mul, mul_one] at h
  exact_mod_cast h

/-- Double interval sums, including a freely chosen starting residue. -/
def intervalDouble (χ : MulChar Q ℂ) (a b : ℕ) (t : Q) : ℂ :=
  ∑ i ∈ Finset.range a, ∑ j ∈ Finset.range b, χ (t + (i : Q) + (j : Q))

lemma intervalDouble_difference_sq (χ : MulChar Q ℂ) (hχ : χ ≠ 1)
    (s : ℕ) (hs : s ≤ q) (u v : Q) :
    ‖intervalDouble χ s s u - intervalDouble χ s s v‖ ^ 2 ≤
      2 * (q : ℝ) * (s : ℝ) ^ 2 := by
  let I := residueInterval q s
  have h := char_set_difference_sq χ hχ
    (I.map (Equiv.addLeft u).toEmbedding) (I.map (Equiv.addLeft v).toEmbedding) I
  simp only [Finset.card_map, I, card_residueInterval s hs] at h
  simp only [Finset.sum_map, Equiv.toEmbedding_apply, Equiv.coe_addLeft] at h
  simp_rw [sum_residueInterval s hs] at h
  change ‖intervalDouble χ s s u - intervalDouble χ s s v‖ ^ 2 ≤ _ at h
  convert h using 1
  ring

/-- Centering the first set at one half gives a useful bound for long smoothing intervals. -/
theorem char_set_half_sq (χ : MulChar Q ℂ) (hχ : χ ≠ 1) (A C : Finset Q)
    (hC : q ≤ 2 * C.card) :
    ‖∑ x ∈ A, ∑ y ∈ C, χ (x+y)‖ ^ 2 ≤ (q : ℝ) * (C.card : ℝ) ^ 2 / 4 := by
  classical
  let f : Q → ℂ := fun x => (if x ∈ A then 1 else 0) - 1/2
  let g : Q → ℂ := fun y => if y ∈ C then 1 else 0
  have hf : (∑ x : Q, ‖f x‖ ^ 2) = (q : ℝ)/4 := by
    have hx (x : Q) : ‖f x‖ ^ 2 = (1/4 : ℝ) := by
      dsimp [f]
      split_ifs <;> norm_num
    simp only [hx, Finset.sum_const, Finset.card_univ, ZMod.card, nsmul_eq_mul]
    ring
  have hs : (∑ x : Q, ∑ y ∈ C, χ (x+y)) = 0 := by
    rw [Finset.sum_comm]
    apply Finset.sum_eq_zero
    intro y _
    exact (Equiv.sum_comp (Equiv.addRight y) χ).trans (χ.sum_eq_zero_of_ne_one hχ)
  have hh : (∑ x : Q, f x * (∑ y : Q, g y * χ (x+y))) =
      ∑ x ∈ A, ∑ y ∈ C, χ (x+y) := by
    simp [f, g, sub_mul, Finset.sum_sub_distrib, ← Finset.mul_sum, hs]
  have hg := char_bilinear_sq χ hχ f g
  rw [hh, hf] at hg
  have hgg : (∑ y : Q, ‖g y‖ ^ 2) = (C.card : ℝ) := by simp [g, apply_ite]
  have hgs : (∑ y : Q, g y) = (C.card : ℂ) := by simp [g]
  rw [hgg, hgs, Complex.norm_natCast] at hg
  have hc : (q : ℝ) ≤ 2 * (C.card : ℝ) := by exact_mod_cast hC
  have hq : (0 : ℝ) ≤ q := by positivity
  have hn : (0 : ℝ) ≤ C.card := by positivity
  nlinarith [mul_nonneg hq (mul_nonneg hn (sub_nonneg.mpr hc))]

lemma sum_range_shift_sub (f : ℕ → ℂ) (a b : ℕ) :
    ((∑ i ∈ Finset.range a, f (b+i)) - ∑ i ∈ Finset.range a, f i) =
      (∑ i ∈ Finset.range b, f (a+i)) - ∑ i ∈ Finset.range b, f i := by
  have h : (∑ i ∈ Finset.range a, f i) + (∑ i ∈ Finset.range b, f (a+i)) =
      (∑ i ∈ Finset.range b, f i) + (∑ i ∈ Finset.range a, f (b+i)) := by
    rw [← Finset.sum_range_add, ← Finset.sum_range_add, Nat.add_comm a b]
  linear_combination -h

lemma intervalDouble_shift_sub (χ : MulChar Q ℂ) (H s : ℕ) (t : Q) :
    intervalDouble χ H s (t + (s : Q)) - intervalDouble χ H s t =
      intervalDouble χ s s (t + (H : Q)) - intervalDouble χ s s t := by
  unfold intervalDouble
  conv_lhs => lhs; rw [Finset.sum_comm]
  conv_lhs => rhs; rw [Finset.sum_comm]
  conv_rhs => lhs; rw [Finset.sum_comm]
  conv_rhs => rhs; rw [Finset.sum_comm]
  rw [← Finset.sum_sub_distrib, ← Finset.sum_sub_distrib]
  apply Finset.sum_congr rfl
  intro j _
  simpa only [Nat.cast_add, add_assoc, add_left_comm, add_comm] using
    sum_range_shift_sub (fun i => χ (t + (j : Q) + (i : Q))) H s

lemma intervalDouble_two_mul (χ : MulChar Q ℂ) (H s : ℕ) (t : Q) :
    intervalDouble χ H (2*s) t =
      intervalDouble χ H s t + intervalDouble χ H s (t + (s : Q)) := by
  unfold intervalDouble
  simp only [two_mul, Finset.sum_range_add, Nat.cast_add, Finset.sum_add_distrib]
  congr 1
  apply Finset.sum_congr rfl
  intro i _
  apply Finset.sum_congr rfl
  intro j _
  congr 1
  ring

/-- The interval sum averaged over the first `s` additive shifts. -/
def intervalAverage (χ : MulChar Q ℂ) (H s : ℕ) (t : Q) : ℂ :=
  intervalDouble χ H s t / (s : ℂ)

lemma intervalAverage_step (χ : MulChar Q ℂ) (hχ : χ ≠ 1)
    (H s : ℕ) (hs0 : 0 < s) (hs : s ≤ q) (t : Q)
    (B : ℝ) (hB : 0 ≤ B) (hq : (q : ℝ) ≤ B^2) :
    ‖intervalAverage χ H (2*s) t - intervalAverage χ H s t‖ ≤ 17*B/24 := by
  have hsn : (s : ℂ) ≠ 0 := by exact_mod_cast hs0.ne'
  have he : intervalAverage χ H (2*s) t - intervalAverage χ H s t =
      (intervalDouble χ s s (t+(H:Q)) - intervalDouble χ s s t) / (2*(s:ℂ)) := by
    unfold intervalAverage
    rw [intervalDouble_two_mul]
    have hh := intervalDouble_shift_sub χ H s t
    push_cast
    field_simp
    linear_combination hh
  rw [he, norm_div, norm_mul, Complex.norm_natCast]
  norm_num only [Complex.norm_ofNat]
  apply (div_le_iff₀ (by positivity : (0:ℝ) < 2*(s:ℝ))).mpr
  apply (sq_le_sq₀ (norm_nonneg _) (by positivity)).mp
  have hd := intervalDouble_difference_sq χ hχ s hs (t+(H:Q)) t
  have hq' := mul_le_mul_of_nonneg_right hq (sq_nonneg (s:ℝ))
  nlinarith [sq_nonneg (B*(s:ℝ))]

lemma intervalAverage_half (χ : MulChar Q ℂ) (hχ : χ ≠ 1)
    (H s : ℕ) (hH : H ≤ q) (hs0 : 0 < s) (hs : s ≤ q)
    (hs2 : q ≤ 2*s) (t : Q) (B : ℝ) (hB : 0 ≤ B) (hq : (q:ℝ) ≤ B^2) :
    ‖intervalAverage χ H s t‖ ≤ B/2 := by
  have h := char_set_half_sq χ hχ
    ((residueInterval q H).map (Equiv.addLeft t).toEmbedding)
    (residueInterval q s) (by rwa [card_residueInterval s hs])
  simp only [card_residueInterval s hs, Finset.sum_map,
    Equiv.toEmbedding_apply, Equiv.coe_addLeft] at h
  simp_rw [sum_residueInterval s hs, sum_residueInterval H hH] at h
  change ‖intervalDouble χ H s t‖ ^ 2 ≤ (q:ℝ)*(s:ℝ)^2/4 at h
  rw [intervalAverage, norm_div, Complex.norm_natCast]
  apply (div_le_iff₀ (by exact_mod_cast hs0 : (0:ℝ) < s)).mpr
  apply (sq_le_sq₀ (norm_nonneg _) (by positivity)).mp
  have hq' := mul_le_mul_of_nonneg_right hq (sq_nonneg (s:ℝ))
  nlinarith

lemma intervalAverage_dyadic (χ : MulChar Q ℂ) (hχ : χ ≠ 1)
    (H j : ℕ) (hj : 2^j ≤ q) (t : Q) (B : ℝ) (hB : 0 ≤ B)
    (hq : (q:ℝ) ≤ B^2) :
    ‖intervalAverage χ H (2^j) t - intervalAverage χ H 1 t‖ ≤ (j:ℝ)*17*B/24 := by
  induction j with
  | zero => simp
  | succ j ih =>
    have hj' : 2^j ≤ q := (Nat.pow_le_pow_right (by decide) (Nat.le_succ j)).trans hj
    have hs := intervalAverage_step χ hχ H (2^j) (by positivity) hj' t B hB hq
    rw [← pow_succ'] at hs
    calc
      _ ≤ ‖intervalAverage χ H (2^(j+1)) t - intervalAverage χ H (2^j) t‖ +
          ‖intervalAverage χ H (2^j) t - intervalAverage χ H 1 t‖ := by
            simpa only [dist_eq_norm] using dist_triangle
              (intervalAverage χ H (2^(j+1)) t) (intervalAverage χ H (2^j) t)
              (intervalAverage χ H 1 t)
      _ ≤ 17*B/24 + (j:ℝ)*17*B/24 := add_le_add hs (ih hj')
      _ = _ := by push_cast; ring

/-- A fully kernel-checked completion bound, sufficient for the review certificate.
It holds for every interval of at most `p` consecutive residues. -/
theorem polymur_character_sum_bound (χ : MulChar F ℂ) (hχ : χ ≠ 1)
    (H : ℕ) (hH : H ≤ p) (t : F) :
    ‖∑ i ∈ Finset.range H, χ (t + (i : F))‖ ≤ (65295510750 : ℝ) := by
  have h₁ := intervalAverage_dyadic χ hχ H 60 (by norm_num [p]) t
    1518500250 (by norm_num) (by norm_num [p])
  have h₂ := intervalAverage_half χ hχ H (2^60) hH (by norm_num) (by norm_num [p])
    (by norm_num [p]) t 1518500250 (by norm_num) (by norm_num [p])
  have hn : ‖intervalAverage χ H 1 t‖ ≤
      ‖intervalAverage χ H (2^60) t - intervalAverage χ H 1 t‖ +
        ‖intervalAverage χ H (2^60) t‖ := by
    rw [norm_sub_rev]
    exact norm_le_norm_sub_add _ _
  have h : ‖intervalAverage χ H 1 t‖ ≤ (65295510750 : ℝ) := by
    norm_num only [Nat.cast_ofNat] at h₁ h₂ hn
    linarith
  simpa only [intervalAverage, intervalDouble, Nat.cast_one, div_one,
    Finset.sum_range_one, Nat.cast_zero, add_zero] using h

end ProvenHashes.Polymur
