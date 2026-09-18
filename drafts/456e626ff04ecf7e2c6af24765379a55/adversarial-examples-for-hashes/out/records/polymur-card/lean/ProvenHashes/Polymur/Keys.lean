import ProvenHashes.Polymur.Bytes

noncomputable section
namespace ProvenHashes.Polymur

/-- A kernel-checked certificate for the generator used in polymur_init_params. -/
theorem generator37_order : orderOf (37 : F) = p-1 := by
  apply orderOf_eq_of_pow_and_pow_div_prime (by norm_num [p])
  · change (37 : ZMod 2305843009213693951) ^ _ = 1
    norm_num only [p, Nat.reduceSub]
    reduce_mod_char
  · intro r hr hd
    have hf : p-1 = 2*3*3*5*5*7*11*13*31*41*61*151*331*1321 := by norm_num [p]
    rw [hf] at hd
    simp only [hr.dvd_mul] at hd
    simp only [Nat.prime_dvd_prime_iff_eq hr (by norm_num : Nat.Prime 2),
      Nat.prime_dvd_prime_iff_eq hr (by norm_num : Nat.Prime 3),
      Nat.prime_dvd_prime_iff_eq hr (by norm_num : Nat.Prime 5),
      Nat.prime_dvd_prime_iff_eq hr (by norm_num : Nat.Prime 7),
      Nat.prime_dvd_prime_iff_eq hr (by norm_num : Nat.Prime 11),
      Nat.prime_dvd_prime_iff_eq hr (by norm_num : Nat.Prime 13),
      Nat.prime_dvd_prime_iff_eq hr (by norm_num : Nat.Prime 31),
      Nat.prime_dvd_prime_iff_eq hr (by norm_num : Nat.Prime 41),
      Nat.prime_dvd_prime_iff_eq hr (by norm_num : Nat.Prime 61),
      Nat.prime_dvd_prime_iff_eq hr (by norm_num : Nat.Prime 151),
      Nat.prime_dvd_prime_iff_eq hr (by norm_num : Nat.Prime 331),
      Nat.prime_dvd_prime_iff_eq hr (by norm_num : Nat.Prime 1321)] at hd
    simp only [or_assoc] at hd
    rcases hd with rfl | rfl | rfl | rfl | rfl | rfl | rfl | rfl | rfl | rfl | rfl | rfl | rfl | rfl
    all_goals
      change (37 : ZMod 2305843009213693951) ^ _ ≠ 1
      norm_num only [p, Nat.reduceSub, Nat.reduceDiv]
      reduce_mod_char
      decide

/-- Exact ideal multiplier acceptance set: a primitive field element whose
canonical seventh power is below the source's reduction threshold. -/
def Admissible (k : F) : Prop :=
  k ≠ 0 ∧ orderOf k = p-1 ∧ (k^7).val < 2^60-2^56

abbrev Key := {k : F // Admissible k}
instance : Fintype Key := Fintype.ofFinite Key

def keyCard : ℕ := Fintype.card Key

lemma admissible37 : Admissible (37 : F) := by
  exact ⟨by decide, generator37_order, by decide⟩

instance keyNonempty : Nonempty Key := ⟨⟨37, admissible37⟩⟩

lemma keyCard_pos : 0 < keyCard := Fintype.card_pos

end ProvenHashes.Polymur
