import ProvenHashes.Polynomial

namespace ProvenHashes.Polymur

abbrev p : ℕ := 2305843009213693951
abbrev F := ZMod p
abbrev Byte := Fin 256
abbrev Bytes := List Byte

instance prime : Fact (Nat.Prime p) := ⟨by
  exact lucas_lehmer_sufficiency 61 (by norm_num) (by norm_num [LucasLehmerTest])⟩

/-- The little-endian integer read by the source, with at most seven bytes. -/
def pack (m : Bytes) : ℕ := Nat.ofDigits 256 (m.map Fin.val)

def window (m : Bytes) (start width : ℕ) : F :=
  (pack ((m.drop start).take width) : F)

lemma pack_lt (m : Bytes) (h : m.length ≤ 7) : pack m < 2^56 := by
  have hd : ∀ x ∈ m.map Fin.val, x < 256 := by
    intro x hx
    obtain ⟨b, _, rfl⟩ := List.mem_map.mp hx
    exact b.isLt
  have ht := Nat.ofDigits_lt_base_pow_length (by decide : 1 < 256) hd
  have hp : 256 ^ (m.map Fin.val).length ≤ 256 ^ 7 := by
    apply Nat.pow_le_pow_right (by decide)
    simpa using h
  exact lt_of_lt_of_le ht (by norm_num at hp ⊢; exact hp)

lemma cast_inj {a b : ℕ} (ha : a < p) (hb : b < p)
    (h : (a : F) = (b : F)) : a = b := by
  have := congrArg ZMod.val h
  simpa [ZMod.val_natCast, Nat.mod_eq_of_lt ha, Nat.mod_eq_of_lt hb] using this

lemma pack_inj {a b : Bytes} (hlen : a.length = b.length)
    (h : pack a = pack b) : a = b := by
  apply Fin.val_injective.list_map
  apply Nat.ofDigits_inj_of_len_eq (by decide : 1 < 256) (by simpa using hlen) _ _ h
  · intro x hx
    obtain ⟨v, _, rfl⟩ := List.mem_map.mp hx
    exact v.isLt
  · intro x hx
    obtain ⟨v, _, rfl⟩ := List.mem_map.mp hx
    exact v.isLt

lemma window_inj {a b : Bytes} (hlen : a.length = b.length)
    (start width : ℕ) (hw : width ≤ 7) (h : window a start width = window b start width) :
    (a.drop start).take width = (b.drop start).take width := by
  apply pack_inj (by simp [hlen])
  apply cast_inj
  · exact (pack_lt _ (by simpa using (min_le_left width (a.length - start)).trans hw)).trans (by norm_num [p])
  · exact (pack_lt _ (by simpa using (min_le_left width (b.length - start)).trans hw)).trans (by norm_num [p])
  · exact h

lemma window_byte {a b : Bytes} (hlen : a.length = b.length)
    {start width i : ℕ} (hw : width ≤ 7) (hs : start ≤ i) (hi : i < a.length)
    (he : i < start + width) (h : window a start width = window b start width) :
    a[i] = b[i]'(by omega) := by
  have hh := window_inj hlen start width hw h
  have ha : i - start < ((a.drop start).take width).length := by simp; omega
  have hb : i - start < ((b.drop start).take width).length := by simp; omega
  have hh' := congrArg (fun l : Bytes => l[i-start]?) hh
  simpa [List.getElem?_take, List.getElem?_drop, show start + (i-start) = i by omega,
    show i-start < width by omega, List.getElem?_eq_getElem hi,
    List.getElem?_eq_getElem (show i < b.length by omega)] using hh'

end ProvenHashes.Polymur
