import Mathlib

namespace ProvenHashes.Classic
set_option maxRecDepth 8192
set_option maxHeartbeats 2000000

theorem prime_2 : Nat.Prime 2 := by norm_num
theorem prime_23 : Nat.Prime 23 := by norm_num
theorem prime_5 : Nat.Prime 5 := by norm_num
theorem prime_17 : Nat.Prime 17 := by norm_num
theorem prime_89 : Nat.Prime 89 := by norm_num
theorem prime_109 : Nat.Prime 109 := by norm_num
theorem prime_19403 : Nat.Prime 19403 := by
  apply lucas_primality 19403 (2 : ZMod 19403)
  · norm_num; reduce_mod_char
  · intro q hq hd
    have hf : 19403 - 1 = 2 * 89 * 109 := by norm_num
    rw [hf] at hd
    simp only [hq.dvd_mul] at hd
    rcases hd with ((h | h) | h)
    · have he := (Nat.dvd_prime prime_2).mp h
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
    · have he := (Nat.dvd_prime prime_89).mp h
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
    · have he := (Nat.dvd_prime prime_109).mp h
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
theorem prime_32985101 : Nat.Prime 32985101 := by
  apply lucas_primality 32985101 (2 : ZMod 32985101)
  · norm_num; reduce_mod_char
  · intro q hq hd
    have hf : 32985101 - 1 = 2 ^ 2 * 5 ^ 2 * 17 * 19403 := by norm_num
    rw [hf] at hd
    simp only [hq.dvd_mul] at hd
    rcases hd with (((h | h) | h) | h)
    · have he := (Nat.dvd_prime prime_2).mp (hq.dvd_of_dvd_pow h)
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
    · have he := (Nat.dvd_prime prime_5).mp (hq.dvd_of_dvd_pow h)
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
    · have he := (Nat.dvd_prime prime_17).mp h
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
    · have he := (Nat.dvd_prime prime_19403).mp h
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
theorem prime_73 : Nat.Prime 73 := by norm_num
theorem prime_487 : Nat.Prime 487 := by norm_num
theorem prime_461 : Nat.Prime 461 := by norm_num
theorem prime_3134801 : Nat.Prime 3134801 := by
  apply lucas_primality 3134801 (3 : ZMod 3134801)
  · norm_num; reduce_mod_char
  · intro q hq hd
    have hf : 3134801 - 1 = 2 ^ 4 * 5 ^ 2 * 17 * 461 := by norm_num
    rw [hf] at hd
    simp only [hq.dvd_mul] at hd
    rcases hd with (((h | h) | h) | h)
    · have he := (Nat.dvd_prime prime_2).mp (hq.dvd_of_dvd_pow h)
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
    · have he := (Nat.dvd_prime prime_5).mp (hq.dvd_of_dvd_pow h)
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
    · have he := (Nat.dvd_prime prime_17).mp h
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
    · have he := (Nat.dvd_prime prime_461).mp h
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
theorem prime_3 : Nat.Prime 3 := by norm_num
theorem prime_13 : Nat.Prime 13 := by norm_num
theorem prime_43 : Nat.Prime 43 := by norm_num
theorem prime_4889 : Nat.Prime 4889 := by norm_num
theorem prime_7 : Nat.Prime 7 := by norm_num
theorem prime_881 : Nat.Prime 881 := by norm_num
theorem prime_37003 : Nat.Prime 37003 := by
  apply lucas_primality 37003 (2 : ZMod 37003)
  · norm_num; reduce_mod_char
  · intro q hq hd
    have hf : 37003 - 1 = 2 * 3 * 7 * 881 := by norm_num
    rw [hf] at hd
    simp only [hq.dvd_mul] at hd
    rcases hd with (((h | h) | h) | h)
    · have he := (Nat.dvd_prime prime_2).mp h
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
    · have he := (Nat.dvd_prime prime_3).mp h
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
    · have he := (Nat.dvd_prime prime_7).mp h
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
    · have he := (Nat.dvd_prime prime_881).mp h
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
theorem prime_11 : Nat.Prime 11 := by norm_num
theorem prime_67 : Nat.Prime 67 := by norm_num
theorem prime_221101 : Nat.Prime 221101 := by
  apply lucas_primality 221101 (22 : ZMod 221101)
  · norm_num; reduce_mod_char
  · intro q hq hd
    have hf : 221101 - 1 = 2 ^ 2 * 3 * 5 ^ 2 * 11 * 67 := by norm_num
    rw [hf] at hd
    simp only [hq.dvd_mul] at hd
    rcases hd with ((((h | h) | h) | h) | h)
    · have he := (Nat.dvd_prime prime_2).mp (hq.dvd_of_dvd_pow h)
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
    · have he := (Nat.dvd_prime prime_3).mp h
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
    · have he := (Nat.dvd_prime prime_5).mp (hq.dvd_of_dvd_pow h)
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
    · have he := (Nat.dvd_prime prime_11).mp h
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
    · have he := (Nat.dvd_prime prime_67).mp h
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
theorem prime_4024685905107147541 : Nat.Prime 4024685905107147541 := by
  apply lucas_primality 4024685905107147541 (2 : ZMod 4024685905107147541)
  · norm_num; reduce_mod_char
  · intro q hq hd
    have hf : 4024685905107147541 - 1 = 2 ^ 2 * 3 ^ 2 * 5 * 13 * 43 * 4889 * 37003 * 221101 := by norm_num
    rw [hf] at hd
    simp only [hq.dvd_mul] at hd
    rcases hd with (((((((h | h) | h) | h) | h) | h) | h) | h)
    · have he := (Nat.dvd_prime prime_2).mp (hq.dvd_of_dvd_pow h)
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
    · have he := (Nat.dvd_prime prime_3).mp (hq.dvd_of_dvd_pow h)
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
    · have he := (Nat.dvd_prime prime_5).mp h
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
    · have he := (Nat.dvd_prime prime_13).mp h
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
    · have he := (Nat.dvd_prime prime_43).mp h
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
    · have he := (Nat.dvd_prime prime_4889).mp h
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
    · have he := (Nat.dvd_prime prime_37003).mp h
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
    · have he := (Nat.dvd_prime prime_221101).mp h
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
theorem prime_897064739519922787230182993783 : Nat.Prime 897064739519922787230182993783 := by
  apply lucas_primality 897064739519922787230182993783 (5 : ZMod 897064739519922787230182993783)
  · norm_num; reduce_mod_char
  · intro q hq hd
    have hf : 897064739519922787230182993783 - 1 = 2 * 73 * 487 * 3134801 * 4024685905107147541 := by norm_num
    rw [hf] at hd
    simp only [hq.dvd_mul] at hd
    rcases hd with ((((h | h) | h) | h) | h)
    · have he := (Nat.dvd_prime prime_2).mp h
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
    · have he := (Nat.dvd_prime prime_73).mp h
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
    · have he := (Nat.dvd_prime prime_487).mp h
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
    · have he := (Nat.dvd_prime prime_3134801).mp h
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
    · have he := (Nat.dvd_prime prime_4024685905107147541).mp h
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
theorem prime_1361129467683753853853498429727072845819 : Nat.Prime 1361129467683753853853498429727072845819 := by
  apply lucas_primality 1361129467683753853853498429727072845819 (2 : ZMod 1361129467683753853853498429727072845819)
  · norm_num; reduce_mod_char
  · intro q hq hd
    have hf : 1361129467683753853853498429727072845819 - 1 = 2 * 23 * 32985101 * 897064739519922787230182993783 := by norm_num
    rw [hf] at hd
    simp only [hq.dvd_mul] at hd
    rcases hd with (((h | h) | h) | h)
    · have he := (Nat.dvd_prime prime_2).mp h
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
    · have he := (Nat.dvd_prime prime_23).mp h
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
    · have he := (Nat.dvd_prime prime_32985101).mp h
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide
    · have he := (Nat.dvd_prime prime_897064739519922787230182993783).mp h
      rcases he with he | he
      · exact False.elim (hq.ne_one he)
      · subst q; norm_num; reduce_mod_char; decide

theorem poly1305_prime : Nat.Prime (2 ^ 130 - 5) := by
  exact prime_1361129467683753853853498429727072845819

#print axioms poly1305_prime
end ProvenHashes.Classic
