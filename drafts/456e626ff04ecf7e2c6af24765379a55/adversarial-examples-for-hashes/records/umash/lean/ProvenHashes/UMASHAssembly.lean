import ProvenHashes.UMASHObligations
import ProvenHashes.UMASHEncoding
import ProvenHashes.UMASHMixture

namespace ProvenHashes.UMASH

instance distinctOHNonempty : Nonempty DistinctOHKey := by
  refine ⟨⟨fun i => BitVec.ofNat 64 i.val, ?_⟩⟩
  intro i j h
  have hi : i.val < 2^64 := i.isLt.trans (by norm_num)
  have hj : j.val < 2^64 := j.isLt.trans (by norm_num)
  have hh := congrArg BitVec.toNat h
  simp only [BitVec.toNat_ofNat, Nat.mod_eq_of_lt hi, Nat.mod_eq_of_lt hj] at hh
  exact Fin.ext hh

theorem comparisonPolynomial_eval (k : OHKey) (seed : Word) (m : Message) (f : ℕ) :
    (comparisonPolynomial k seed m).eval (f : Field) =
      ((unfinalize (hashWith k f seed m)).toNat : Field) := by
  by_cases hm : m.length ≤ 8
  · simp [comparisonPolynomial, hashWith, hm]
  · have ht := polyReduce_lt f (compress false k seed m) 0 (by norm_num [q])
    norm_num only [q, Nat.reducePow] at ht
    simp [comparisonPolynomial, hashWith, hm, unfinalize_finalize,
      BitVec.toNat_ofNat, Nat.mod_eq_of_lt ht, eval_blockPolynomial]
-- CHECKPOINT

theorem comparisonPolynomial_degree (L : ℕ) (k : OHKey) (seed : Word) (m : Message)
    (hm : m.length ≤ 8*L) :
    (comparisonPolynomial k seed m).natDegree ≤ 2*((L+31)/32) := by
  unfold comparisonPolynomial
  split_ifs
  · simp
  · exact (blockPolynomial_degree _).trans
      (Nat.mul_le_mul_left 2 (compress_length_le L false k seed m hm))
-- CHECKPOINT

theorem comparison_slice_bound (L : ℕ) (k : OHKey) (seed : Word) (x y : Message)
    (hx : x.length ≤ 8*L) (hy : y.length ≤ 8*L)
    (hne : comparisonPolynomial k seed x ≠ comparisonPolynomial k seed y) :
    uniformProb (fun f : PolyKey =>
      hashWith k f.val.val seed x = hashWith k f.val.val seed y) ≤ rootRate L := by
  classical
  apply le_min (probability_le_one _) _
  let P := comparisonPolynomial k seed x
  let Q := comparisonPolynomial k seed y
  have hd : (P-Q).natDegree ≤ 2*((L+31)/32) :=
    (Polynomial.natDegree_sub_le _ _).trans (max_le
      (comparisonPolynomial_degree L k seed x hx)
      (comparisonPolynomial_degree L k seed y hy))
  have hc := (Classic.restricted_roots (fun f : PolyKey => (f.val.val : Field))
    polyKey_field_injective (P-Q) (sub_ne_zero.mpr hne)).trans hd
  have hm : uniformProb (fun f : PolyKey =>
      hashWith k f.val.val seed x = hashWith k f.val.val seed y) ≤
      uniformProb (fun f : PolyKey => (P-Q).eval (f.val.val : Field) = 0) := by
    apply probability_mono
    intro f h
    simp only [Polynomial.eval_sub, sub_eq_zero, P, Q, comparisonPolynomial_eval]
    exact congrArg (fun a => ((unfinalize a).toNat : Field)) h
  apply hm.trans
  unfold uniformProb
  rw [polyKey_card]
  apply div_le_div_of_nonneg_right _ (by positivity)
  norm_cast
-- CHECKPOINT

/-- Complete reduction for the literal hash, including short/long comparisons.
The only mathematical premise is the stated polynomial-identity probability. -/
theorem primary_identity_to_all_pairs (B : ℚ≥0) (hB : PrimaryIdentityBound B)
    (L : ℕ) (seed : Word) (x y : Message)
    (hx : x.length ≤ 8*L) (hy : y.length ≤ 8*L) (hne : x ≠ y) :
    uniformProb (fun k : Key64 => hash64 k seed x = hash64 k seed y) ≤
      B + (1-B)*rootRate L := by
  apply probability_mixture _
    (fun k : DistinctOHKey => comparisonPolynomial k.val seed x = comparisonPolynomial k.val seed y)
    (rootRate L) B (min_le_left _ _) (hB seed x y hne)
  intro k hk
  exact comparison_slice_bound L k.val seed x y hx hy hk
-- CHECKPOINT

theorem sharp_rate_le_published (L : ℕ) (hL : 1 ≤ L) :
    (162:ℚ≥0)/(q-561 : ℕ) + (1-(162:ℚ≥0)/(q-561 : ℕ))*rootRate L ≤
      ((L+511)/512 : ℕ)/(2:ℚ≥0)^55 := by
  have hceil : (L+31)/32 ≤ 16*((L+511)/512) := by omega
  have hceilQ : (((L+31)/32 : ℕ) : ℚ) ≤ 16*(((L+511)/512 : ℕ) : ℚ) := by
    exact_mod_cast hceil
  have hpos : (1 : ℚ) ≤ (((L+511)/512 : ℕ) : ℚ) := by
    exact_mod_cast (show 1 ≤ (L+511)/512 by omega)
  have hr : rootRate L ≤ 2*((((L+31)/32 : ℕ) : ℚ≥0))/(p-2 : ℕ) := min_le_right _ _
  have hrQ := NNRat.coe_le_coe.mpr hr
  have hB : (162:ℚ≥0)/(q-561 : ℕ) ≤ 1 := by
    apply NNRat.coe_le_coe.mp; norm_num [q]
  apply NNRat.coe_le_coe.mp
  simp only [NNRat.coe_add, NNRat.coe_mul, NNRat.coe_sub hB, NNRat.coe_one]
  norm_num [q, p] at hrQ ⊢
  nlinarith
-- CHECKPOINT

/-- The sharp primary marginal is an explicit extra premise; the two open
joint ENH propositions do not supply it. -/
theorem conditional55_with_primary : Conditional55WithPrimaryPremise := by
  intro h L seed x y hL hx hy hxy
  exact (primary_identity_to_all_pairs _ h L seed x y hx hy hxy).trans
    (sharp_rate_le_published L hL)
-- CHECKPOINT

/-- The one-word branch needed by the exact envelope is recorded independently. -/
def ShortOneWordBound : Prop := ∀ (seed : Word) (x y : Message),
  x.length ≤ 8 → y.length ≤ 8 → x ≠ y →
  uniformProb (fun k : Key64 => hash64 k seed x = hash64 k seed y) ≤
    (1:ℚ≥0)/(q-561 : ℕ)

theorem certified64_of_primary_and_short (h : WeakPrimaryProjection)
    (hs : ShortOneWordBound) : CertifiedAllPairs64 := by
  intro L seed x y _hL hx hy hxy
  by_cases hL : L = 1
  · subst L
    simpa only [certifiedEnvelope, if_pos, mul_one] using hs seed x y hx hy hxy
  · have hc : 1-weakA = weakComplement := by
      exact (tsub_eq_of_eq_add (weakA_add_complement.symm.trans (add_comm _ _)))
    simpa only [certifiedEnvelope, if_neg hL, hc] using
      primary_identity_to_all_pairs weakA h L seed x y hx hy hxy
-- CHECKPOINT

theorem certified128_of_primary_and_short (h : WeakPrimaryProjection)
    (hs : ShortOneWordBound) : CertifiedAllPairs128 :=
  certifiedAllPairs128_of_64 (certified64_of_primary_and_short h hs)
-- CHECKPOINT

end ProvenHashes.UMASH
