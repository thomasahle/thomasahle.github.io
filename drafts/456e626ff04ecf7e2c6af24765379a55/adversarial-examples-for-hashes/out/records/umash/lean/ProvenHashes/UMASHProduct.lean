import ProvenHashes.UMASHProbability

namespace ProvenHashes.UMASH

set_option maxHeartbeats 2000000

/-- Integer product point masses, including the zero-product exceptional fibre. -/
theorem integer_product_count (Q t : ℕ) (hQ : 0 < Q) :
    (Finset.univ.filter (fun ab : Fin Q × Fin Q => ab.1.val*ab.2.val = t)).card ≤ 2*Q-1 := by
  classical
  let E := Finset.univ.filter (fun ab : Fin Q × Fin Q => ab.1.val*ab.2.val = t)
  have h0 : (E.filter fun ab => ab.1.val = 0).card ≤ Q := by
    calc
      _ ≤ (Finset.univ : Finset (Fin Q)).card := by
        apply Finset.card_le_card_of_injOn Prod.snd
        · intro ab hab; exact Finset.mem_univ _
        · intro a ha b hb hab
          apply Prod.ext
          · apply Fin.ext
            exact (Finset.mem_filter.mp ha).2.trans (Finset.mem_filter.mp hb).2.symm
          · exact hab
      _ = Q := by simp
  have h1 : (E.filter fun ab => ¬ab.1.val = 0).card ≤ Q-1 := by
    calc
      _ ≤ ((Finset.range Q).erase 0).card := by
        apply Finset.card_le_card_of_injOn (fun ab : Fin Q × Fin Q => ab.1.val)
        · intro ab hab
          exact Finset.mem_erase.mpr ⟨(Finset.mem_filter.mp hab).2,
            Finset.mem_range.mpr ab.1.isLt⟩
        · intro a ha b hb hab
          change a.1.val = b.1.val at hab
          have hpa := (Finset.mem_filter.mp (Finset.mem_filter.mp ha).1).2
          have hpb := (Finset.mem_filter.mp (Finset.mem_filter.mp hb).1).2
          apply Prod.ext (Fin.ext hab)
          apply Fin.ext
          apply mul_left_cancel₀ (Finset.mem_filter.mp ha).2
          exact hpa.trans (by simpa only [hab] using hpb.symm)
      _ = Q-1 := by simp [Finset.card_erase_of_mem (Finset.mem_range.mpr hQ)]
  have hs := Finset.filter_card_add_filter_neg_card_eq_card (s := E) (fun ab => ab.1.val = 0)
  change E.card ≤ 2*Q-1
  omega
-- CHECKPOINT

theorem integer_product_probability (Q t : ℕ) (hQ : 0 < Q) :
    uniformProb (fun ab : Fin Q × Fin Q => ab.1.val*ab.2.val = t) ≤
      ((2*Q-1 : ℕ) : ℚ≥0) / (Q:ℚ≥0)^2 := by
  unfold uniformProb
  simp only [Fintype.card_prod, Fintype.card_fin, Nat.cast_mul]
  rw [pow_two]
  apply div_le_div_of_nonneg_right _ (by positivity)
  apply Nat.cast_le.mpr
  convert integer_product_count Q t hQ using 2 <;> ext ab <;> simp
-- CHECKPOINT

theorem word_residue_fibre (r : ℕ) :
    (Finset.univ.filter (fun x : Word => x.toNat % p = r)).card ≤ 9 := by
  classical
  calc
    _ ≤ (Finset.range 9).card := by
      apply Finset.card_le_card_of_injOn (fun x : Word => x.toNat / p)
      · intro x hx
        apply Finset.mem_range.mpr
        have h := x.isLt
        norm_num [p] at *
        omega
      · intro x hx y hy hxy
        change x.toNat / p = y.toNat / p at hxy
        have hm : x.toNat % p = y.toNat % p :=
          (Finset.mem_filter.mp hx).2.trans (Finset.mem_filter.mp hy).2.symm
        have hxq := Nat.mod_add_div x.toNat p
        have hyq := Nat.mod_add_div y.toNat p
        apply BitVec.eq_of_toNat_eq
        rw [hm, hxy] at hxq
        exact hxq.symm.trans hyq
    _ = 9 := Finset.card_range _
-- CHECKPOINT

theorem field_word_fibre (r : Field) :
    (Finset.univ.filter (fun x : Word => (x.toNat : Field) = r)).card ≤ 9 := by
  classical
  have he : (Finset.univ.filter (fun x : Word => (x.toNat : Field) = r)) =
      Finset.univ.filter (fun x : Word => x.toNat % p = r.val) := by
    ext x
    simp only [Finset.mem_filter, Finset.mem_univ, true_and]
    constructor
    · intro h; simpa only [ZMod.val_natCast] using congrArg ZMod.val h
    · intro h; apply ZMod.val_injective; simpa only [ZMod.val_natCast] using h
  rw [he]
  convert word_residue_fibre r.val using 2 <;> ext x <;> simp
-- CHECKPOINT

theorem project_fibre (r : Field × Field) :
    (Finset.univ.filter (fun x : Chunk => project x = r)).card ≤ 81 := by
  classical
  have he : (Finset.univ.filter (fun x : Chunk => project x = r)) =
      (Finset.univ.filter (fun x : Word => (x.toNat : Field) = r.1)) ×ˢ
      (Finset.univ.filter (fun x : Word => (x.toNat : Field) = r.2)) := by
    ext x
    simp only [Finset.mem_filter, Finset.mem_univ, true_and, Finset.mem_product, project]
    exact Prod.mk.inj_iff
  rw [he, Finset.card_product]
  exact (Nat.mul_le_mul (field_word_fibre r.1) (field_word_fibre r.2)).trans (by norm_num)
-- CHECKPOINT

def tagFold (tag : Word) (x : Wide) : Chunk :=
  let v := split (x + (tag.zeroExtend 128 <<< 64))
  (v.1, v.2 ^^^ v.1)

def untagFold (tag : Word) (x : Chunk) : Wide :=
  join (x.1, x.2 ^^^ x.1) - (tag.zeroExtend 128 <<< 64)

theorem join_split (x : Wide) : join (split x) = x := by
  apply BitVec.eq_of_getLsbD_eq
  intro i hi
  interval_cases i <;> simp [join, split]
-- CHECKPOINT

theorem untagFold_tagFold (tag : Word) (x : Wide) : untagFold tag (tagFold tag x) = x := by
  simp only [untagFold, tagFold, BitVec.xor_assoc, BitVec.xor_self, BitVec.xor_zero]
  rw [join_split]
  exact BitVec.add_sub_cancel _ _
-- CHECKPOINT

theorem enh_eq_tagFold (tag : Word) (key data : Chunk) :
    enh key data tag = tagFold tag
      ((data.1 + key.1).zeroExtend 128 * (data.2 + key.2).zeroExtend 128) := rfl
-- CHECKPOINT

end ProvenHashes.UMASH
