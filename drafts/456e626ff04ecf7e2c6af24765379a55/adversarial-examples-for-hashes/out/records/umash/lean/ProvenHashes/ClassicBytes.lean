import ProvenHashes.ClassicCore

namespace ProvenHashes.Classic
abbrev Byte := Fin 256

def chunks (m : List Byte) : List (List Byte) :=
  if m.length = 0 then [] else m.take 16 :: chunks (m.drop 16)
termination_by m.length
decreasing_by simp only [List.length_drop]; omega

theorem chunks_flatten (m : List Byte) : (chunks m).flatten = m := by
  fun_induction chunks m with
  | case1 m h => simpa using (List.length_eq_zero_iff.mp h).symm
  | case2 m h ih => rw [List.flatten_cons, ih]; exact List.take_append_drop 16 m

theorem chunks_length (m : List Byte) : (chunks m).length = (m.length + 15) / 16 := by
  fun_induction chunks m with
  | case1 m h => simp [h]
  | case2 m h ih => simp only [List.length_cons, ih, List.length_drop]; omega

theorem chunks_size (m : List Byte) : ∀ b ∈ chunks m, b.length ≤ 16 := by
  fun_induction chunks m with
  | case1 m h => simp
  | case2 m h ih =>
    simp only [List.mem_cons]
    intro b hb
    rcases hb with rfl | hb
    · simp
    · exact ih b hb

def bytesNat (b : List Byte) : ℕ := Nat.ofDigits 256 (b.map Fin.val)

theorem bytesNat_lt (b : List Byte) : bytesNat b < 256 ^ b.length := by
  simpa [bytesNat] using Nat.ofDigits_lt_base_pow_length (by decide : 1 < 256)
    (l := b.map Fin.val) (by simp)

theorem bytesNat_inj_length {a b : List Byte} (hl : a.length = b.length)
    (h : bytesNat a = bytesNat b) : a = b := by
  induction a generalizing b with
  | nil => simpa using hl.symm
  | cons a as ih =>
    cases b with
    | nil => simp at hl
    | cons b bs =>
      have hv : a.val = b.val := by
        have hm := congrArg (fun x => x % 256) h
        simpa [bytesNat, Nat.ofDigits, Nat.mod_eq_of_lt a.isLt,
          Nat.mod_eq_of_lt b.isLt] using hm
      have hab : a = b := Fin.ext hv
      subst b
      have ht : bytesNat as = bytesNat bs := by
        change a.val + 256 * bytesNat as = a.val + 256 * bytesNat bs at h
        omega
      exact congrArg (List.cons a) (ih (by simpa using hl) ht)

def markedNat (b : List Byte) : ℕ := bytesNat b + 256 ^ b.length

theorem markedNat_eq_append_one (b : List Byte) :
    markedNat b = bytesNat (b ++ [1]) := by
  simp [markedNat, bytesNat, Nat.ofDigits_append, Nat.ofDigits]

theorem markedNat_pos (b : List Byte) : 0 < markedNat b := by
  dsimp [markedNat]; positivity

theorem markedNat_lt (b : List Byte) (hb : b.length ≤ 16) : markedNat b < 2 ^ 129 := by
  have h := bytesNat_lt b
  have hp : 256 ^ b.length ≤ 256 ^ 16 := Nat.pow_le_pow_right (by decide) hb
  norm_num at hp ⊢
  dsimp [markedNat]
  omega

theorem markedNat_injective : Function.Injective markedNat := by
  intro a b h
  have hl : a.length = b.length := by
    have ha := bytesNat_lt a
    have hb := bytesNat_lt b
    have order (x y : List Byte) (hxy : x.length < y.length) : markedNat x < markedNat y := by
      have he : 256 ^ (x.length + 1) ≤ 256 ^ y.length :=
        Nat.pow_le_pow_right (by decide) hxy
      have hx := bytesNat_lt x
      have hp : 0 < 256 ^ x.length := by positivity
      rw [pow_succ] at he
      dsimp [markedNat]
      omega
    rcases lt_trichotomy a.length b.length with hab | hab | hab
    · exact False.elim ((ne_of_lt (order a b hab)) h)
    · exact hab
    · exact False.elim ((ne_of_lt (order b a hab)) h.symm)
  apply bytesNat_inj_length hl
  simpa only [markedNat, hl, Nat.add_right_cancel_iff] using h

theorem chunk_codes_inj_with {R : Type*} (code : List Byte → R)
    (hinj : ∀ a b, a.length = b.length → code a = code b → a = b)
    {a b : List Byte} (hl : a.length = b.length)
    (h : (chunks a).map code = (chunks b).map code) : a = b := by
  induction a using chunks.induct generalizing b with
  | case1 a ha =>
    have hb : b.length = 0 := hl.symm.trans ha
    simp_all
  | case2 a ha ih =>
    have hb : b.length ≠ 0 := by omega
    rw [chunks.eq_1 a, if_neg ha, chunks.eq_1 b, if_neg hb] at h
    simp only [List.map_cons, List.cons.injEq] at h
    have ht : a.take 16 = b.take 16 := hinj _ _ (by simp [hl]) h.1
    have hd : a.drop 16 = b.drop 16 := ih (by simp [hl]) h.2
    rw [← List.take_append_drop 16 a, ← List.take_append_drop 16 b, ht, hd]

theorem chunk_codes_inj {a b : List Byte} (hl : a.length = b.length)
    (h : (chunks a).map bytesNat = (chunks b).map bytesNat) : a = b :=
  chunk_codes_inj_with bytesNat (fun _ _ => bytesNat_inj_length) hl h

end ProvenHashes.Classic
