import ProvenHashes.Polymur.KeyCardinality
import ProvenHashes.Polymur.Metric

noncomputable section
namespace ProvenHashes.Polymur

/-- The numeric core collision bound with the key-cardinality hypothesis discharged. -/
theorem core_collision_bound_K0_unconditional {n : ℕ}
    (a b : Bytes) (ha : a.length ≤ n) (hb : b.length ≤ n) (hne : a ≠ b) :
    uniformProb (fun k : Key => core k.val a = core k.val b) ≤ (D n : ℚ≥0)/K0 :=
  core_collision_bound_K0 cardinalityCertificate a b ha hb hne

/-- The numeric idealHash collision bound for any shared additive-secret function. -/
theorem idealHash_collision_bound_K0_unconditional {n : ℕ}
    (a b : Bytes) (ha : a.length ≤ n) (hb : b.length ≤ n) (hne : a ≠ b)
    (s : Key → Word) (tweak : Word) :
    uniformProb (fun k : Key => idealHash k.val (s k) tweak a =
      idealHash k.val (s k) tweak b) ≤ (D n : ℚ≥0)/K0 :=
  idealHash_collision_bound_K0 cardinalityCertificate a b ha hb hne s tweak

/-- The certified word-normalized score, with no remaining cardinality hypothesis. -/
theorem score_lower_unconditional : (54.2267 : ℝ) ≤ score :=
  score_lower cardinalityCertificate

end ProvenHashes.Polymur
