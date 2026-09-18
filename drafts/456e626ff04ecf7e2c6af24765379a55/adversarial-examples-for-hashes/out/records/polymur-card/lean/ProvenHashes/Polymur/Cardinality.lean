import ProvenHashes.Polymur.Collision

noncomputable section
namespace ProvenHashes.Polymur

/-- Review certificate's integer lower bound. See POLYMUR_REVIEW_SECTION.md §1.2
and review-support/checks.py, add_checks.py, results.json in the delivery. -/
def K0 : ℕ := 189729088763903999

/-- The target cardinality statement, proved by `cardinalityCertificate` in
`KeyCardinality.lean` from the interval character sum and exact-order count. -/
def CardinalityCertificate : Prop := K0 ≤ keyCard

theorem core_collision_bound_K0 (hK : CardinalityCertificate) {n : ℕ}
    (a b : Bytes) (ha : a.length ≤ n) (hb : b.length ≤ n) (hne : a ≠ b) :
    uniformProb (fun k : Key => core k.val a = core k.val b) ≤ (D n : ℚ≥0)/K0 := by
  apply (core_collision_bound a b ha hb hne).trans
  exact div_le_div_of_nonneg_left (by positivity) (by norm_num [K0]) (by exact_mod_cast hK)

theorem idealHash_collision_bound_K0 (hK : CardinalityCertificate) {n : ℕ}
    (a b : Bytes) (ha : a.length ≤ n) (hb : b.length ≤ n) (hne : a ≠ b)
    (s : Key → Word) (tweak : Word) :
    uniformProb (fun k : Key => idealHash k.val (s k) tweak a = idealHash k.val (s k) tweak b) ≤
      (D n : ℚ≥0)/K0 := by
  apply (idealHash_collision_bound a b ha hb hne s tweak).trans
  exact div_le_div_of_nonneg_left (by positivity) (by norm_num [K0]) (by exact_mod_cast hK)

end ProvenHashes.Polymur
