import ProvenHashes.Composition
import ProvenHashes.Polymur.Encoding
import ProvenHashes.Polymur.Mix
import ProvenHashes.Polymur.Keys

noncomputable section
namespace ProvenHashes.Polymur
open Polynomial

lemma key_root_count (q : F[X]) (hq : q ≠ 0) :
    (Finset.univ.filter (fun k : Key => q.eval k.val = 0)).card ≤ q.natDegree := by
  classical
  apply le_trans _ (ProvenHashes.polynomial_zero_count q hq)
  apply Finset.card_le_card_of_injOn (fun k : Key => k.val)
  · intro k hk
    simpa only [Finset.mem_coe, Finset.mem_filter, Finset.mem_univ, true_and] using hk
  · intro a _ b _ he
    exact Subtype.ext he

/-- Uniform sampling is over Key itself, not over the shipped seed space. -/
theorem core_collision_bound {n : ℕ} (a b : Bytes) (ha : a.length ≤ n) (hb : b.length ≤ n)
    (hne : a ≠ b) :
    uniformProb (fun k : Key => core k.val a = core k.val b) ≤ (D n : ℚ≥0)/keyCard := by
  classical
  let q := encode a - encode b
  have hq : q ≠ 0 := fun h => hne (encode_injective (sub_eq_zero.mp h))
  have hc := (key_root_count q hq).trans (difference_degree a b ha hb)
  have he : (fun k : Key => core k.val a = core k.val b) =
      (fun k : Key => q.eval k.val = 0) := by
    funext k
    simp [q, core, sub_eq_zero]
  rw [he, uniformProb]
  exact div_le_div_of_nonneg_right (by exact_mod_cast hc) (by positivity)

/-- A canonical residue embedded into a 64-bit word, used for the ideal full hash. -/
def coreWord (k : F) (m : Bytes) : Word := BitVec.ofNat 64 (core k m).val

def idealHash (k : F) (s tweak : Word) (m : Bytes) : Word := finish s (coreWord k m+tweak)

lemma coreWord_eq_implies (k : F) (a b : Bytes) (h : coreWord k a = coreWord k b) :
    core k a = core k b := by
  apply ZMod.val_injective
  have hv := congrArg BitVec.toNat h
  have ha : (core k a).val < 2^64 := (ZMod.val_lt _).trans (by norm_num [p])
  have hb : (core k b).val < 2^64 := (ZMod.val_lt _).trans (by norm_num [p])
  change (core k a).val % 2^64 = (core k b).val % 2^64 at hv
  simpa only [Nat.mod_eq_of_lt ha, Nat.mod_eq_of_lt hb] using hv

/-- Exact source mixer, secret addition and common tweak; s may even depend on k. -/
theorem idealHash_collision_bound {n : ℕ} (a b : Bytes) (ha : a.length ≤ n) (hb : b.length ≤ n)
    (hne : a ≠ b) (s : Key → Word) (tweak : Word) :
    uniformProb (fun k : Key => idealHash k.val (s k) tweak a = idealHash k.val (s k) tweak b) ≤
      (D n : ℚ≥0)/keyCard := by
  apply le_trans (uniformProb_mono ?_) (core_collision_bound a b ha hb hne)
  intro k h
  exact coreWord_eq_implies k.val a b ((finish_tweak_eq_iff _ _ _ _).mp h)

/-- Transfer to lazy representatives needs only congruence, not canonicality.
The C arithmetic/overflow refinement must supply this hypothesis. -/
theorem representative_collision_bound {n : ℕ} (a b : Bytes)
    (ha : a.length ≤ n) (hb : b.length ≤ n) (hne : a ≠ b)
    (rep : Key → Bytes → Word)
    (hrep : ∀ k m, ((rep k m).toNat : F) = core k.val m)
    (s : Key → Word) (tweak : Word) :
    uniformProb (fun k : Key => finish (s k) (rep k a+tweak) = finish (s k) (rep k b+tweak)) ≤
      (D n : ℚ≥0)/keyCard := by
  apply le_trans (uniformProb_mono ?_) (core_collision_bound a b ha hb hne)
  intro k h
  have he := (finish_tweak_eq_iff _ _ _ _).mp h
  rw [← hrep k a, ← hrep k b, he]

end ProvenHashes.Polymur
