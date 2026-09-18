import ProvenHashes.MultiplyAddShift
import ProvenHashes.Composition

namespace ProvenHashes

theorem odd_mod_of_even_modulus {a M : ℕ} (ha : Odd a) (hM : 2 ∣ M) :
    Odd (a % M) := by
  rw [Nat.odd_iff, Nat.mod_mod_of_dvd _ hM]
  exact Nat.odd_iff.mp ha

def oddKeyShift (w : ℕ) (hw : 0 < w) (c : ℕ) (hc : Even c)
    (a : OddMultiplier w) : OddMultiplier w :=
  ⟨a.1 + ⟨c % 2 ^ (2 * w), Nat.mod_lt _ (by positivity)⟩, by
    change Odd ((a.1.val + c % 2 ^ (2 * w)) % 2 ^ (2 * w))
    rw [Nat.add_mod_mod]
    exact odd_mod_of_even_modulus (a.2.add_even hc)
      (dvd_pow_self 2 (by omega : 2 * w ≠ 0))⟩

theorem oddKeyShift_injective (w : ℕ) (hw : 0 < w) (c : ℕ) (hc : Even c) :
    Function.Injective (oddKeyShift w hw c hc) := by
  intro a b h
  apply Subtype.ext
  exact add_right_cancel (congrArg Subtype.val h)

noncomputable def oddKeyShiftEquiv (w : ℕ) (hw : 0 < w) (c : ℕ) (hc : Even c) :
    OddMultiplier w ≃ OddMultiplier w :=
  Equiv.ofBijective _ ((Finite.injective_iff_bijective).mp (oddKeyShift_injective w hw c hc))

theorem oddKeyShift_cast (w : ℕ) (hw : 0 < w) (c : ℕ) (hc : Even c)
    (a : OddMultiplier w) :
    (((oddKeyShiftEquiv w hw c hc a).1.val : ℕ) : ZMod (2 ^ (2 * w))) =
      (a.1.val : ZMod (2 ^ (2 * w))) + c := by
  change ((((a.1.val + c % 2 ^ (2 * w)) % 2 ^ (2 * w) : ℕ)) :
    ZMod (2 ^ (2 * w))) = _
  simp

theorem even_bucket_solution (N b s z : ℕ) (hs : s < b) (hz : Odd z) :
    ∃ c : ℕ, Even c ∧
      (c : ZMod (2 ^ N)) * (2 ^ s * z : ℕ) = (2 ^ b : ℕ) := by
  let a : ZMod (2 ^ N) := (z : ZMod (2 ^ N))⁻¹ * 2 ^ (b - s - 1)
  have hi : (z : ZMod (2 ^ N))⁻¹ * z = 1 :=
    ZMod.inv_mul_of_unit _ ((ZMod.isUnit_iff_coprime _ _).mpr (hz.coprime_two_right.pow_right _))
  refine ⟨2 * a.val, even_two.mul_right _, ?_⟩
  push_cast
  rw [ZMod.natCast_zmod_val]
  change (2 * ((z : ZMod (2 ^ N))⁻¹ * 2 ^ (b - s - 1))) * (2 ^ s * z) = 2 ^ b
  calc
    _ = ((z : ZMod (2 ^ N))⁻¹ * z) * (2 ^ (b - s - 1) * 2 ^ s * 2) := by ring
    _ = _ := by rw [hi, one_mul, ← pow_add, ← pow_succ]; congr 1; omega

theorem odd_difference_high_uniform (w ℓ s z : ℕ) (hw : 0 < w)
    (hℓ : ℓ ≤ 2 * w) (hs : s < 2 * w - ℓ) (hz : Odd z) (v : ZMod (2 ^ ℓ)) :
    uniformProb (fun a : OddMultiplier w =>
      highWord w ℓ ((a.1.val : ZMod (2 ^ (2 * w))) * (2 ^ s * z : ℕ)) = v) =
      1 / (2 : ℚ≥0) ^ ℓ := by
  classical
  have hm : 1 < 2 ^ (2 * w) := by
    simpa using Nat.pow_lt_pow_right (by omega : 1 < 2) (by omega : 0 < 2 * w)
  letI : Nonempty (OddMultiplier w) := ⟨⟨⟨1, hm⟩, odd_one⟩⟩
  let f (a : OddMultiplier w) :=
    highWord w ℓ ((a.1.val : ZMod (2 ^ (2 * w))) * (2 ^ s * z : ℕ))
  suffices h : uniformProb (fun a => f a = v) = 1 / Fintype.card (ZMod (2 ^ ℓ)) by
    simpa [f, ZMod.card] using h
  apply uniformProb_of_equal_fibers
  intro u v
  obtain ⟨c, hc, he⟩ := even_bucket_solution (2 * w) (2 * w - ℓ) s z hs hz
  let t := v - u
  let e := oddKeyShiftEquiv w hw (c * t.val) (hc.mul_right _)
  have hf (a : OddMultiplier w) : f (e a) = f a + t := by
    dsimp only [f, e]
    rw [oddKeyShift_cast, add_mul]
    have he' : ((c * t.val : ℕ) : ZMod (2 ^ (2 * w))) * (2 ^ s * z : ℕ) =
        (2 ^ (2 * w - ℓ) * t.val : ℕ) := by
      push_cast at he ⊢
      calc
        _ = ((c : ZMod (2 ^ (2 * w))) * (2 ^ s * z)) * t.val := by ring
        _ = _ := by rw [he]
    rw [he']
    exact highWord_translate w ℓ hℓ _ t
  apply Finset.card_bij (fun a _ => e a)
  · intro a ha
    simp only [Finset.mem_filter, Finset.mem_univ, true_and] at ha ⊢
    rw [hf, ha]
    dsimp [t]
    abel
  · intro a _ b _ hab
    exact e.injective hab
  · intro a ha
    simp only [Finset.mem_filter, Finset.mem_univ, true_and] at ha
    refine ⟨e.symm a, ?_, e.apply_symm_apply a⟩
    simp only [Finset.mem_filter, Finset.mem_univ, true_and]
    have hh := hf (e.symm a)
    rw [e.apply_symm_apply, ha] at hh
    dsimp [t] at hh
    apply add_right_cancel (b := v - u)
    rw [← hh]
    abel

theorem same_bucket_difference_endpoint (M B Q : ℕ) [NeZero M]
    (hB : 0 < B) (hQ : 0 < Q) (hM : B * Q = M)
    (r s : ZMod M) (h : r.val / B = s.val / B) :
    (r - s).val / B = 0 ∨ (r - s).val / B = Q - 1 := by
  have hc := same_bucket_close hB h
  by_cases hs : s.val ≤ r.val
  · left
    rw [ZMod.val_sub hs]
    apply Nat.div_eq_of_lt
    omega
  · right
    have hs' : r.val < s.val := by omega
    have hn : s - r ≠ 0 := by
      intro he
      have he' := congrArg ZMod.val (sub_eq_zero.mp he)
      omega
    have he : (r - s).val = M - (s.val - r.val) := by
      rw [← neg_sub s r, ZMod.neg_val, if_neg hn, ZMod.val_sub (Nat.le_of_lt hs')]
    rw [he]
    have hQB : (Q - 1) * B + B = M := by
      have hQ' : Q - 1 + 1 = Q := by omega
      nlinarith
    apply Nat.div_eq_of_lt_le
    · omega
    · have hv := ZMod.val_lt s
      have hQ' : Q - 1 + 1 = Q := by omega
      rw [hQ', Nat.mul_comm Q, hM]
      omega

theorem multiplyShift_val_formula (w ℓ : ℕ) (a : OddMultiplier w) (x : Fin (2 ^ w)) :
    multiplyShift w ℓ a x =
      (((a.1.val : ZMod (2 ^ (2 * w))) * x.val).val / 2 ^ (2 * w - ℓ)) := by
  rw [← Nat.cast_mul, ZMod.val_natCast]
  rfl

theorem multiplyShift_no_collision_of_bucket_dvd (w ℓ : ℕ)
    (x y : Fin (2 ^ w)) (hxy : x.val < y.val)
    (hd : 2 ^ (2 * w - ℓ) ∣ y.val - x.val) (a : OddMultiplier w) :
    multiplyShift w ℓ a x ≠ multiplyShift w ℓ a y := by
  intro h
  have hBM : 2 ^ (2 * w - ℓ) ∣ 2 ^ (2 * w) :=
    Nat.pow_dvd_pow 2 (Nat.sub_le _ _)
  have hm := ((Nat.modEq_iff_dvd' (Nat.le_of_lt hxy)).mpr hd).mul_left a.1.val
  have he : a.1.val * x.val % 2 ^ (2 * w) = a.1.val * y.val % 2 ^ (2 * w) := by
    have hm' : (a.1.val * x.val % 2 ^ (2 * w)) % 2 ^ (2 * w - ℓ) =
        (a.1.val * y.val % 2 ^ (2 * w)) % 2 ^ (2 * w - ℓ) := by
      simpa only [Nat.mod_mod_of_dvd _ hBM] using hm
    have hx := Nat.mod_add_div (a.1.val * x.val % 2 ^ (2 * w)) (2 ^ (2 * w - ℓ))
    have hy := Nat.mod_add_div (a.1.val * y.val % 2 ^ (2 * w)) (2 ^ (2 * w - ℓ))
    change (a.1.val * x.val % 2 ^ (2 * w)) / 2 ^ (2 * w - ℓ) =
      (a.1.val * y.val % 2 ^ (2 * w)) / 2 ^ (2 * w - ℓ) at h
    rw [h, hm'] at hx
    omega
  have hm2 : 2 ^ w ≤ 2 ^ (2 * w) := Nat.pow_le_pow_right (by omega) (by omega)
  have he' := odd_mul_mod_injective (2 * w) a.1.val a.2
    (a₁ := ⟨x.val, lt_of_lt_of_le x.isLt hm2⟩) (a₂ := ⟨y.val, lt_of_lt_of_le y.isLt hm2⟩) he
  have heval : x.val = y.val := congrArg (fun q : Fin (2 ^ (2 * w)) => q.val) he'
  omega

theorem multiplyShift_bound_ordered (w ℓ : ℕ) (hw : 0 < w) (hℓ : ℓ ≤ 2 * w)
    (x y : Fin (2 ^ w)) (hxy : x.val < y.val) :
    uniformProb (fun a : OddMultiplier w => multiplyShift w ℓ a x = multiplyShift w ℓ a y) ≤
      2 / (2 : ℚ≥0) ^ ℓ := by
  obtain ⟨s, z, hz, hd⟩ := Nat.exists_eq_two_pow_mul_odd
    (by omega : y.val - x.val ≠ 0)
  by_cases hs : s < 2 * w - ℓ
  · let f (a : OddMultiplier w) :=
      highWord w ℓ ((a.1.val : ZMod (2 ^ (2 * w))) * (2 ^ s * z : ℕ))
    have h0 := odd_difference_high_uniform w ℓ s z hw hℓ hs hz 0
    have h1 := odd_difference_high_uniform w ℓ s z hw hℓ hs hz
      ((2 ^ ℓ - 1 : ℕ) : ZMod (2 ^ ℓ))
    have hsub (a : OddMultiplier w)
        (ha : multiplyShift w ℓ a x = multiplyShift w ℓ a y) :
        f a = 0 ∨ f a = ((2 ^ ℓ - 1 : ℕ) : ZMod (2 ^ ℓ)) := by
      have hm : 2 ^ (2 * w - ℓ) * 2 ^ ℓ = 2 ^ (2 * w) := by
        rw [← pow_add, Nat.sub_add_cancel hℓ]
      simp only [multiplyShift_val_formula] at ha
      have he := same_bucket_difference_endpoint (2 ^ (2 * w)) (2 ^ (2 * w - ℓ))
        (2 ^ ℓ) (by positivity) (by positivity) hm
        ((a.1.val : ZMod (2 ^ (2 * w))) * y.val)
        ((a.1.val : ZMod (2 ^ (2 * w))) * x.val) ha.symm
      have hf : f a = highWord w ℓ
          (((a.1.val : ZMod (2 ^ (2 * w))) * y.val) -
            ((a.1.val : ZMod (2 ^ (2 * w))) * x.val)) := by
        dsimp only [f]
        rw [← hd, Nat.cast_sub (Nat.le_of_lt hxy), mul_sub]
      rw [hf]
      unfold highWord
      rcases he with he | he
      · left; rw [he]; simp
      · right; rw [he]
    calc
      _ ≤ uniformProb (fun a => f a = 0 ∨ f a = ((2 ^ ℓ - 1 : ℕ) : ZMod (2 ^ ℓ))) :=
        uniformProb_mono hsub
      _ ≤ uniformProb (fun a => f a = 0) +
          uniformProb (fun a => f a = ((2 ^ ℓ - 1 : ℕ) : ZMod (2 ^ ℓ))) :=
        uniformProb_or_le _ _
      _ = _ := by rw [h0, h1]; ring
  · have hd' : 2 ^ (2 * w - ℓ) ∣ y.val - x.val := by
      rw [hd]
      exact dvd_mul_of_dvd_left (Nat.pow_dvd_pow 2 (by omega)) _
    calc
      _ ≤ uniformProb (fun _ : OddMultiplier w => False) :=
        uniformProb_mono (fun a ha => multiplyShift_no_collision_of_bucket_dvd w ℓ x y hxy hd' a ha)
      _ = 0 := by simp [uniformProb]
      _ ≤ _ := by positivity

/-- The parent lane's proposition, now proved with its original quantifiers. -/
theorem multiplyShift_bound : MultiplyShiftBound := by
  intro w ℓ hw hℓ x y hne
  have hne' : x.val ≠ y.val := fun h => hne (Fin.ext h)
  rcases lt_or_gt_of_ne hne' with h | h
  · exact multiplyShift_bound_ordered w ℓ hw hℓ x y h
  · simpa only [eq_comm] using multiplyShift_bound_ordered w ℓ hw hℓ y x h

end ProvenHashes
