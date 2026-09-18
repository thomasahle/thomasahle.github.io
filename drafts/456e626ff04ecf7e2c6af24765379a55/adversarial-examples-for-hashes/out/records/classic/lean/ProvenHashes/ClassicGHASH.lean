import ProvenHashes.ClassicBytes

namespace ProvenHashes.Classic.GHASH
noncomputable section
open Polynomial

abbrev F := GaloisField 2 128
instance : Fintype F := Fintype.ofFinite _
theorem field_card : Fintype.card F = 2 ^ 128 := by
  rw [Fintype.card_eq_nat_card]
  exact GaloisField.card 2 128 (by decide)

/-- A 128-bit block's field interpretation. The bound holds for every bijection,
including the standard GCM polynomial-basis convention. This is not a cast of
integers into a characteristic-two field. -/
abbrev Wire := Fin (2 ^ 128) ≃ F

/-- Big-endian data block with zero bytes appended on its right. -/
def paddedNat (b : List Byte) : ℕ := bytesNat b.reverse * 256 ^ (16 - b.length)

theorem paddedNat_lt (b : List Byte) (hb : b.length ≤ 16) : paddedNat b < 2 ^ 128 := by
  calc
    _ < 256 ^ b.length * 256 ^ (16 - b.length) :=
      Nat.mul_lt_mul_of_pos_right (by simpa using bytesNat_lt b.reverse) (by positivity)
    _ = 2 ^ 128 := by rw [← pow_add, Nat.add_sub_of_le hb]; norm_num

theorem paddedNat_inj_length {a b : List Byte} (hl : a.length = b.length)
    (h : paddedNat a = paddedNat b) : a = b := by
  have hv : bytesNat a.reverse = bytesNat b.reverse := by
    dsimp [paddedNat] at h
    rw [hl] at h
    exact Nat.eq_of_mul_eq_mul_right (by positivity) h
  have he := bytesNat_inj_length (by simpa using hl) hv
  simpa using congrArg List.reverse he

def block (b : List Byte) (hb : b.length ≤ 16) : Fin (2 ^ 128) :=
  ⟨paddedNat b, paddedNat_lt b hb⟩

/-- Standard [8 * byte-length]_64 || [0]_64 length block as a big-endian word. -/
def lengthValue (m : List Byte) (hm : 8 * m.length < 2 ^ 64) : Fin (2 ^ 128) :=
  ⟨8 * m.length * 2 ^ 64, by
    have h := Nat.mul_lt_mul_of_pos_right hm (by norm_num : 0 < 2 ^ 64)
    norm_num at h ⊢; exact h⟩

def data (wire : Wire) (m : List Byte) : List F :=
  (chunks m).attach.map fun b => wire (block b.val (chunks_size m b.val b.property))

@[simp] theorem data_length (wire : Wire) (m : List Byte) :
    (data wire m).length = (m.length + 15) / 16 := by simp [data, chunks_length]

theorem data_inj_length (wire : Wire) {m m' : List Byte}
    (hl : m.length = m'.length) (h : data wire m = data wire m') : m = m' := by
  apply chunk_codes_inj_with paddedNat (fun _ _ => paddedNat_inj_length) hl
  have he := congrArg (List.map fun x : F => (wire.symm x).val) h
  simpa [data, List.map_map, block] using he

def poly (wire : Wire) (m : List Byte) (hm : 8 * m.length < 2 ^ 64) : F[X] :=
  positive (wire (lengthValue m hm) :: (data wire m).reverse)

theorem poly_injective (wire : Wire)
    (m m' : List Byte) (hm : 8 * m.length < 2 ^ 64) (hm' : 8 * m'.length < 2 ^ 64)
    (h : poly wire m hm = poly wire m' hm') : m = m' := by
  obtain ⟨hlen, hdata⟩ := coeffs_cons_inj (positive_injective h)
  have hl : m.length = m'.length := by
    have hv := congrArg Fin.val (wire.injective hlen)
    dsimp [lengthValue] at hv
    omega
  apply data_inj_length wire hl
  have he := coeffs_inj_length (by simp [hl]) hdata
  simpa using congrArg List.reverse he

theorem degree_bound (wire : Wire) (m : List Byte) (hm : 8 * m.length < 2 ^ 64) :
    (poly wire m hm).natDegree ≤ (m.length + 15) / 16 + 1 := by
  simpa [poly] using positive_degree (wire (lengthValue m hm) :: (data wire m).reverse)

/-- Fixed-pair AXU, with unequal lengths, partial blocks, and the empty string. -/
theorem differential_bound (wire : Wire)
    (m m' : List Byte) (hm : 8 * m.length < 2 ^ 64) (hm' : 8 * m'.length < 2 ^ 64)
    (hne : m ≠ m') (n : ℕ)
    (hn : (m.length + 15) / 16 ≤ n) (hn' : (m'.length + 15) / 16 ≤ n) (t : F) :
    uniformProb (fun H : F => (poly wire m hm).eval H - (poly wire m' hm').eval H = t) ≤
      (n + 1 : ℕ) / (2 ^ 128 : ℚ≥0) := by
  classical
  have he : poly wire m hm ≠ poly wire m' hm' :=
    fun h => hne (poly_injective wire m m' hm hm' h)
  have bound := target_bound (fun H : F => H) Function.injective_id
    (poly wire m hm) (poly wire m' hm') (positive_zero _) (positive_zero _) he {t}
    (fun H => (poly wire m hm).eval H - (poly wire m' hm').eval H = t)
    (by intro H h; simpa using h)
  simp only [Finset.card_singleton, one_mul, field_card, Nat.cast_pow, Nat.cast_ofNat] at bound
  apply bound.trans
  apply div_le_div_of_nonneg_right _ (by positivity)
  exact_mod_cast max_le ((degree_bound wire m hm).trans (Nat.add_le_add_right hn 1))
    ((degree_bound wire m' hm').trans (Nat.add_le_add_right hn' 1))

theorem collision_bound (wire : Wire)
    (m m' : List Byte) (hm : 8 * m.length < 2 ^ 64) (hm' : 8 * m'.length < 2 ^ 64)
    (hne : m ≠ m') (n : ℕ)
    (hn : (m.length + 15) / 16 ≤ n) (hn' : (m'.length + 15) / 16 ≤ n) :
    uniformProb (fun H : F => (poly wire m hm).eval H = (poly wire m' hm').eval H) ≤
      (n + 1 : ℕ) / (2 ^ 128 : ℚ≥0) := by
  simpa only [sub_eq_zero] using differential_bound wire m m' hm hm' hne n hn hn' 0

theorem zero_key (wire : Wire) (m : List Byte) (hm : 8 * m.length < 2 ^ 64) :
    (poly wire m hm).eval 0 = 0 := by simp [poly, positive]

theorem fixed_pad_cancels (p q : F[X]) (H pad : F) :
    p.eval H + pad = q.eval H + pad ↔ p.eval H = q.eval H := add_left_inj _

theorem word_bound (wire : Wire) (L : ℕ)
    (m m' : List Byte) (hm : 8 * m.length < 2 ^ 64) (hm' : 8 * m'.length < 2 ^ 64)
    (hne : m ≠ m') (hL : m.length ≤ 8 * L) (hL' : m'.length ≤ 8 * L) :
    uniformProb (fun H : F => (poly wire m hm).eval H = (poly wire m' hm').eval H) ≤
      ((L + 1) / 2 + 1 : ℕ) / (2 ^ 128 : ℚ≥0) :=
  collision_bound wire m m' hm hm' hne _ (by omega) (by omega)

#print axioms collision_bound
#print axioms poly_injective
end
end ProvenHashes.Classic.GHASH
