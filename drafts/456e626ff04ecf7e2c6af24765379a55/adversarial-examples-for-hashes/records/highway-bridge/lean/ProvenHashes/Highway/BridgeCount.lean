import ProvenHashes.Highway.BridgePredicate
import ProvenHashes.Highway.Bound
namespace ProvenHashes.Highway
set_option maxRecDepth 10000
set_option maxHeartbeats 1000000

open scoped Classical

/-- For every assignment of the 56 free bits, exactly 56165 relevant points satisfy E₂. -/
theorem assembled_count (f : FreeCoordinates) :
    (Finset.univ.filter fun p : RelevantCoordinates => E2 (assembledKey f p.1 p.2)).card = 56165 := by
  classical
  simpa only [assembled_e2_iff] using
    reduced_count f.1.val (freeA5 f).toNat f.2.2.1.val f.2.1 (padTarget f)
      f.1.isLt (freeA5 f).isLt f.2.2.1.isLt

theorem filter_card_equiv {A B : Type*} [Fintype A] [Fintype B]
    (e : A ≃ B) (P : B → Prop) [DecidablePred P] :
    (Finset.univ.filter fun a : A => P (e a)).card = (Finset.univ.filter P).card := by
  apply Finset.card_bij (fun a _ => e a)
  · intro a ha
    simpa only [Finset.mem_filter, Finset.mem_univ, true_and] using ha
  · intro a ha b hb hab
    exact e.injective hab
  · intro b hb
    refine ⟨e.symm b, ?_, e.apply_symm_apply b⟩
    simpa only [Finset.mem_filter, Finset.mem_univ, true_and, e.apply_symm_apply] using hb

theorem two_word_count :
    (Finset.univ.filter fun p : Half × Word => E2 (coordinateKey p.1 p.2)).card =
      56165 * 2^56 := by
  classical
  rw [← filter_card_equiv byteCoordinates.symm (fun p => E2 (coordinateKey p.1 p.2))]
  change (Finset.univ.filter fun p : FreeCoordinates × RelevantCoordinates =>
    E2 (assembledKey p.1 p.2.1 p.2.2)).card = _
  rw [product_filter_card]
  simp_rw [assembled_count]
  rw [Finset.sum_const, Finset.card_univ, nsmul_eq_mul]
  simp only [FreeCoordinates, Fintype.card_prod, Fintype.card_fin]
  norm_num

abbrev FullCoordinates := Half × ((Half × Word) × (Word × Word))

/-- All four key words, with the class halfword and the two unused words retained. -/
def fullKeyCoordinates : Key ≃ FullCoordinates where
  toFun k := (hi (k 0), ((0xfe4cce2f ^^^ lo (k 0), key1Coordinates (k 1)), (k 2,k 3)))
  invFun p := ![p.1 ++ (0xfe4cce2f ^^^ p.2.1.1), key1Coordinates.symm p.2.1.2,
    p.2.2.1, p.2.2.2]
  left_inv k := by
    funext i
    fin_cases i
    · change hi (k 0) ++ (0xfe4cce2f ^^^ (0xfe4cce2f ^^^ lo (k 0))) = k 0
      simp only [← BitVec.xor_assoc, BitVec.xor_self, BitVec.zero_xor]
      exact wordHalves.symm_apply_apply (k 0)
    · exact key1Coordinates.symm_apply_apply (k 1)
    · rfl
    · rfl
  right_inv p := by
    rcases p with ⟨h,⟨⟨n,w⟩,⟨v2,v3⟩⟩⟩
    have hh := wordHalves.apply_symm_apply (h,0xfe4cce2f ^^^ n)
    have hhi : hi (h ++ (0xfe4cce2f ^^^ n)) = h := congrArg Prod.fst hh
    have hlo : lo (h ++ (0xfe4cce2f ^^^ n)) = 0xfe4cce2f ^^^ n := congrArg Prod.snd hh
    change (hi (h ++ (0xfe4cce2f ^^^ n)),
      ((0xfe4cce2f ^^^ lo (h ++ (0xfe4cce2f ^^^ n)),
        key1Coordinates (key1Coordinates.symm w)), (v2,v3))) = _
    rw [hhi,hlo,key1Coordinates.apply_symm_apply]
    simp only [← BitVec.xor_assoc, BitVec.xor_self, BitVec.zero_xor]

theorem full_key_trail (h : Half) (p : Half × Word) (r : Word × Word) :
    Trail (fullKeyCoordinates.symm (h,p,r)) ↔
      h = 0xdbe6d5d5 ∧ E2 (coordinateKey p.1 p.2) := by
  have hc : Class (fullKeyCoordinates.symm (h,p,r)) ↔ h = 0xdbe6d5d5 :=
    Iff.of_eq (congrArg (fun q : FullCoordinates => q.1 = (0xdbe6d5d5 : Half))
      (fullKeyCoordinates.apply_symm_apply (h,p,r)))
  unfold Trail
  rw [hc]
  apply and_congr_right
  intro hh
  subst h
  exact e2_depends_on_two_words
    (fullKeyCoordinates.symm (0xdbe6d5d5,p,r)) (coordinateKey p.1 p.2) rfl rfl

theorem full_trail_finset :
    (Finset.univ.filter fun p : FullCoordinates => Trail (fullKeyCoordinates.symm p)) =
      ({(0xdbe6d5d5 : Half)} : Finset Half) ×ˢ
        ((Finset.univ.filter fun p : Half × Word => E2 (coordinateKey p.1 p.2)) ×ˢ
          (Finset.univ : Finset (Word × Word))) := by
  classical
  apply Finset.ext
  intro p
  rcases p with ⟨h,p,r⟩
  simp only [Finset.mem_filter, Finset.mem_univ, true_and,
    Finset.mem_product, Finset.mem_singleton, and_true, full_key_trail]

theorem singleton_product_card {A B C : Type*} [DecidableEq A] [Fintype C]
    (a : A) (s : Finset B) :
    (({a} : Finset A) ×ˢ (s ×ˢ (Finset.univ : Finset C))).card =
      s.card * Fintype.card C := by
  rw [Finset.card_product, Finset.card_singleton, one_mul,
    Finset.card_product, Finset.card_univ]

/-- Exact count over the entire uniform 256-bit key space. -/
theorem full_count : (Finset.univ.filter Trail).card = 56165 * 2^184 := by
  classical
  apply Eq.trans (filter_card_equiv fullKeyCoordinates.symm Trail).symm
  apply Eq.trans (congrArg Finset.card full_trail_finset)
  apply Eq.trans (singleton_product_card _ _)
  simp only [two_word_count, Fintype.card_prod, card_word]
  norm_num

/-- The previously outstanding full-key counting proposition is now discharged. -/
theorem exact_trail_count : ExactTrailCount := full_count

end ProvenHashes.Highway
