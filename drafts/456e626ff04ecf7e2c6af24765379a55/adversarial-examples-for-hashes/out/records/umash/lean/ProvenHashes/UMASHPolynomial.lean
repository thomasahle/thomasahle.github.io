import ProvenHashes.UMASHModel
import ProvenHashes.UMASHFinalizer
import ProvenHashes.UMASHProbability

namespace ProvenHashes.UMASH

set_option maxRecDepth 8192

theorem cast_mod_q_sub_eight (n : ℕ) :
    ((n % (q-8) : ℕ) : Field) = (n : Field) := by
  apply (ZMod.natCast_eq_natCast_iff' _ _ _).mpr
  exact Nat.mod_mod_of_dvd n (by norm_num [p, q])

theorem cast_polyStep (f acc : ℕ) (x : Chunk) :
    ((polyStep f acc x : ℕ) : Field) =
      (f : Field) * ((f : Field) * ((acc : Field) + (project x).1) + (project x).2) := by
  rw [polyStep, cast_mod_q_sub_eight]
  simp only [Nat.cast_add, Nat.cast_mul, ZMod.natCast_mod, project]
  ring

theorem cast_polyReduce (f : ℕ) (xs : List Chunk) (acc : ℕ) :
    ((polyReduce f xs acc : ℕ) : Field) = xs.foldl
      (fun a x => (f : Field) * ((f : Field) * (a+(project x).1) + (project x).2))
      (acc : Field) := by
  induction xs generalizing acc with
  | nil => rfl
  | cons x xs ih =>
    change ((polyReduce f xs (polyStep f acc x) : ℕ) : Field) = _
    rw [ih, cast_polyStep]
    rfl
-- CHECKPOINT

theorem fold_chunk_coefficients (f acc : Field) (xs : List Chunk) :
    (xs.flatMap (fun x => [(project x).1, (project x).2])).foldl
      (fun a v => (a+v)*f) acc =
    xs.foldl (fun a x => f*(f*(a+(project x).1)+(project x).2)) acc := by
  induction xs generalizing acc with
  | nil => rfl
  | cons x xs ih =>
    simp only [List.flatMap_cons, List.foldl_append, List.foldl_cons, List.foldl_nil]
    rw [ih]
    congr 1
    ring
-- CHECKPOINT

theorem eval_blockPolynomial (f : ℕ) (xs : List Chunk) :
    (blockPolynomial xs).eval (f : Field) = (polyReduce f xs : ℕ) := by
  rw [blockPolynomial, Classic.eval_positive_reverse, fold_chunk_coefficients]
  exact (cast_polyReduce f xs 0).symm
-- CHECKPOINT

theorem blockPolynomial_degree (xs : List Chunk) :
    (blockPolynomial xs).natDegree ≤ 2*xs.length := by
  have hl : (xs.flatMap fun x => [(project x).1, (project x).2]).length = 2*xs.length := by
    induction xs with
    | nil => rfl
    | cons x xs ih => simp only [List.flatMap_cons, List.length_append,
        List.length_cons, List.length_nil] at *; omega
  simpa only [blockPolynomial, List.length_reverse, hl] using
    (Classic.positive_degree ((xs.flatMap fun x => [(project x).1, (project x).2]).reverse))
-- CHECKPOINT

theorem blockPolynomial_constant (xs : List Chunk) : (blockPolynomial xs).coeff 0 = 0 :=
  Classic.positive_zero _
-- CHECKPOINT

def polyKeyEquiv : Fin (p-2) ≃ PolyKey where
  toFun f := ⟨⟨f.val+2, by have := f.isLt; omega⟩, by change 1 < f.val+2; omega⟩
  invFun f := ⟨f.val.val-2, by have := f.val.isLt; have := f.property; omega⟩
  left_inv f := by apply Fin.ext; simp
  right_inv f := by
    apply Subtype.ext; apply Fin.ext
    change f.val.val-2+2 = f.val.val
    have := f.property
    omega

theorem polyKey_card : Fintype.card PolyKey = p-2 := by
  simpa only [Fintype.card_fin] using (Fintype.card_congr polyKeyEquiv).symm
-- CHECKPOINT

theorem polyKey_field_injective : Function.Injective (fun f : PolyKey => (f.val.val : Field)) := by
  intro a b h
  have hv := congrArg ZMod.val h
  simp only [ZMod.val_natCast, Nat.mod_eq_of_lt a.val.isLt,
    Nat.mod_eq_of_lt b.val.isLt] at hv
  exact Subtype.ext (Fin.ext hv)
-- CHECKPOINT

theorem polynomial_key_collision_bound (xs ys : List Chunk)
    (hne : blockPolynomial xs ≠ blockPolynomial ys) :
    uniformProb (fun f : PolyKey =>
      (blockPolynomial xs).eval (f.val.val : Field) =
      (blockPolynomial ys).eval (f.val.val : Field)) ≤
    ((2 * max xs.length ys.length : ℕ) : ℚ≥0) / (p-2 : ℕ) := by
  classical
  have hc := Classic.restricted_roots (fun f : PolyKey => (f.val.val : Field))
    polyKey_field_injective (blockPolynomial xs - blockPolynomial ys)
    (sub_ne_zero.mpr hne)
  have hd : (blockPolynomial xs - blockPolynomial ys).natDegree ≤
      2 * max xs.length ys.length := by
    refine (Polynomial.natDegree_sub_le _ _).trans (max_le ?_ ?_)
    · exact (blockPolynomial_degree xs).trans (Nat.mul_le_mul_left _ (le_max_left _ _))
    · exact (blockPolynomial_degree ys).trans (Nat.mul_le_mul_left _ (le_max_right _ _))
  have hn := hc.trans hd
  have he : (fun f : PolyKey =>
      (blockPolynomial xs).eval (f.val.val : Field) =
      (blockPolynomial ys).eval (f.val.val : Field)) =
    (fun f : PolyKey => (blockPolynomial xs - blockPolynomial ys).eval (f.val.val : Field) = 0) := by
    funext f
    simp only [Polynomial.eval_sub, sub_eq_zero]
  rw [he]
  unfold uniformProb
  rw [polyKey_card]
  apply div_le_div_of_nonneg_right _ (by positivity)
  apply Nat.cast_le.mpr
  convert hn using 2 <;> ext f <;> simp
-- CHECKPOINT

theorem polyReduce_lt (f : ℕ) (xs : List Chunk) (acc : ℕ) (ha : acc < q) :
    polyReduce f xs acc < q := by
  induction xs generalizing acc with
  | nil => exact ha
  | cons x xs ih =>
    apply ih
    exact (Nat.mod_lt _ (by norm_num [q] : 0 < q-8)).trans_le (Nat.sub_le q 8)
-- CHECKPOINT

theorem finished_polynomial_collision_bound (xs ys : List Chunk)
    (hne : blockPolynomial xs ≠ blockPolynomial ys) :
    uniformProb (fun f : PolyKey =>
      finalize (BitVec.ofNat 64 (polyReduce f.val.val xs)) =
      finalize (BitVec.ofNat 64 (polyReduce f.val.val ys))) ≤
    ((2 * max xs.length ys.length : ℕ) : ℚ≥0) / (p-2 : ℕ) := by
  apply le_trans (probability_mono (fun f hf => ?_))
    (polynomial_key_collision_bound xs ys hne)
  have hraw := congrArg BitVec.toNat (finalize_injective hf)
  have hx := polyReduce_lt f.val.val xs 0 (by norm_num [q])
  have hy := polyReduce_lt f.val.val ys 0 (by norm_num [q])
  dsimp only [q] at hx hy
  simp only [BitVec.toNat_ofNat, Nat.mod_eq_of_lt hx, Nat.mod_eq_of_lt hy] at hraw
  rw [eval_blockPolynomial, eval_blockPolynomial]
  exact congrArg (fun n : ℕ => (n : Field)) hraw
-- CHECKPOINT

end ProvenHashes.UMASH
