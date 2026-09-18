import Mathlib

namespace ProvenHashes.UMASH
set_option maxRecDepth 8192
set_option maxHeartbeats 2000000

theorem prime_2 : Nat.Prime 2 := by norm_num
theorem prime_3 : Nat.Prime 3 := by norm_num
theorem prime_5 : Nat.Prime 5 := by norm_num
theorem prime_7 : Nat.Prime 7 := by norm_num
theorem prime_11 : Nat.Prime 11 := by norm_num
theorem prime_13 : Nat.Prime 13 := by norm_num
theorem prime_31 : Nat.Prime 31 := by norm_num
theorem prime_41 : Nat.Prime 41 := by norm_num
theorem prime_61 : Nat.Prime 61 := by norm_num
theorem prime_151 : Nat.Prime 151 := by norm_num
theorem prime_331 : Nat.Prime 331 := by norm_num
theorem prime_1321 : Nat.Prime 1321 := by norm_num
theorem prime_2305843009213693951 : Nat.Prime 2305843009213693951 := by
  apply lucas_primality 2305843009213693951 (37 : ZMod 2305843009213693951)
  · norm_num; reduce_mod_char
  · intro q hq hd
    have hf : 2305843009213693951 - 1 = 2 * 3 ^ 2 * 5 ^ 2 * 7 * 11 * 13 * 31 * 41 * 61 * 151 * 331 * 1321 := by norm_num
    rw [hf] at hd
    simp only [hq.dvd_mul] at hd
    rcases hd with (((((((((((h | h) | h) | h) | h) | h) | h) | h) | h) | h) | h) | h)
    · have he := (Nat.dvd_prime prime_2).mp h
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
    · have he := (Nat.dvd_prime prime_3).mp (hq.dvd_of_dvd_pow h)
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
    · have he := (Nat.dvd_prime prime_5).mp (hq.dvd_of_dvd_pow h)
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
    · have he := (Nat.dvd_prime prime_7).mp h
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
    · have he := (Nat.dvd_prime prime_11).mp h
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
    · have he := (Nat.dvd_prime prime_13).mp h
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
    · have he := (Nat.dvd_prime prime_31).mp h
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
    · have he := (Nat.dvd_prime prime_41).mp h
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
    · have he := (Nat.dvd_prime prime_61).mp h
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
    · have he := (Nat.dvd_prime prime_151).mp h
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
    · have he := (Nat.dvd_prime prime_331).mp h
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
    · have he := (Nat.dvd_prime prime_1321).mp h
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide

theorem mersenne61_prime : Nat.Prime (2 ^ 61 - 1) := by
  exact prime_2305843009213693951

#print axioms mersenne61_prime
def p : ℕ := 2 ^ 61 - 1
instance : Fact p.Prime := ⟨mersenne61_prime⟩
end ProvenHashes.UMASH
