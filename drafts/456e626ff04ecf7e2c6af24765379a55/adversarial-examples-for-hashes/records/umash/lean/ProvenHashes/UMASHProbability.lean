import ProvenHashes.UMASHModel

namespace ProvenHashes.UMASH

noncomputable instance wordFintype : Fintype Word :=
  Fintype.ofEquiv (Fin q) BitVec.equivFin.symm.toEquiv
noncomputable instance distinctOHFintype : Fintype DistinctOHKey := Fintype.ofFinite _
noncomputable instance polyKeyFintype : Fintype PolyKey := Fintype.ofFinite _
instance polyKeyNonempty : Nonempty PolyKey :=
  ⟨⟨⟨2, by norm_num [p]⟩, by decide⟩⟩

theorem probability_mono {K : Type*} [Fintype K] {E F : K → Prop}
    (h : ∀ k, E k → F k) : uniformProb E ≤ uniformProb F := by
  classical
  apply div_le_div_of_nonneg_right _ (by positivity)
  exact_mod_cast Finset.card_le_card (show Finset.univ.filter E ⊆ Finset.univ.filter F
    from fun k hk => Finset.mem_filter.mpr
      ⟨Finset.mem_univ k, h k (Finset.mem_filter.mp hk).2⟩)
-- CHECKPOINT

/-- Projection to the first fingerprint component preserves any certified
primary bound; the two polynomial multipliers are independently uniform. -/
theorem fingerprint_collision_le_primary (seed : Word) (x y : Message) :
    uniformProb (fun k : Key128 => hash128 k seed x = hash128 k seed y) ≤
    uniformProb (fun k : Key64 => hash64 k seed x = hash64 k seed y) := by
  classical
  let E : Key64 → Prop := fun k => hash64 k seed x = hash64 k seed y
  calc
    _ ≤ uniformProb (fun k : Key128 => E (k.1, k.2.1)) :=
      probability_mono (fun k h => congrArg Prod.fst h)
    _ = uniformProb (fun k : Key64 × PolyKey => E k.1) := by
      exact (uniformProb_equiv (Equiv.prodAssoc DistinctOHKey PolyKey PolyKey)
        (fun k : Key128 => E (k.1, k.2.1))).symm
    _ = _ := Classic.uniformProb_ignore_right E
-- CHECKPOINT

theorem injective_target_probability {K V : Type*} [Fintype K]
    (f : K → V) (hf : Function.Injective f) (targets : Finset V)
    (event : K → Prop) (cover : ∀ k, event k → f k ∈ targets) :
    uniformProb event ≤ targets.card / (Fintype.card K : ℚ≥0) := by
  classical
  have hc : (Finset.univ.filter event).card ≤ targets.card := by
    apply Finset.card_le_card_of_injOn f
    · intro k hk
      exact cover k (Finset.mem_filter.mp hk).2
    · exact hf.injOn
  unfold uniformProb
  exact div_le_div_of_nonneg_right (by exact_mod_cast hc) (by positivity)
-- CHECKPOINT

end ProvenHashes.UMASH
