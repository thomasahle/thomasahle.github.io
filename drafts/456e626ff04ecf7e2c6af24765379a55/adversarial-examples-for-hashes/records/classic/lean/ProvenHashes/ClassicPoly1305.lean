import ProvenHashes.ClassicBytes
import ProvenHashes.ClassicPrimes

namespace ProvenHashes.Classic.Poly1305
noncomputable section
open Polynomial

def P : ℕ := 2 ^ 130 - 5
def Q : ℕ := 2 ^ 128
instance : Fact P.Prime := ⟨poly1305_prime⟩
instance : NeZero Q := ⟨by norm_num [Q]⟩
abbrev F := ZMod P

/-- Four clamped 32-bit limbs: 28 low bits, then three groups of 26 free
bits starting at offsets 34, 66, and 98. Exactly the RFC 8439 mask. -/
abbrev Key := Fin (2 ^ 28) × Fin (2 ^ 26) × Fin (2 ^ 26) × Fin (2 ^ 26)

def keyNat (k : Key) : ℕ :=
  k.1.val + 2 ^ 34 * k.2.1.val + 2 ^ 66 * k.2.2.1.val + 2 ^ 98 * k.2.2.2.val

theorem keyNat_lt (k : Key) : keyNat k < P := by
  have h0 := k.1.isLt
  have h1 := k.2.1.isLt
  have h2 := k.2.2.1.isLt
  have h3 := k.2.2.2.isLt
  norm_num [keyNat, P] at *
  omega

theorem keyNat_injective : Function.Injective keyNat := by
  intro a b h
  have ha0 := a.1.isLt
  have ha1 := a.2.1.isLt
  have ha2 := a.2.2.1.isLt
  have ha3 := a.2.2.2.isLt
  have hb0 := b.1.isLt
  have hb1 := b.2.1.isLt
  have hb2 := b.2.2.1.isLt
  have hb3 := b.2.2.2.isLt
  norm_num [keyNat] at h ha0 ha1 ha2 ha3 hb0 hb1 hb2 hb3
  have h0 : a.1.val = b.1.val := by omega
  have h1 : a.2.1.val = b.2.1.val := by omega
  have h2 : a.2.2.1.val = b.2.2.1.val := by omega
  have h3 : a.2.2.2.val = b.2.2.2.val := by omega
  exact Prod.ext (Fin.ext h0) (Prod.ext (Fin.ext h1) (Prod.ext (Fin.ext h2) (Fin.ext h3)))

def key (k : Key) : F := keyNat k

theorem key_injective : Function.Injective key := by
  intro a b h
  apply keyNat_injective
  have hv := congrArg ZMod.val h
  simpa only [key, ZMod.val_natCast_of_lt (keyNat_lt a),
    ZMod.val_natCast_of_lt (keyNat_lt b)] using hv

theorem key_card : Fintype.card Key = 2 ^ 106 := by
  norm_num [Key, Fintype.card_prod, Fintype.card_fin]

def clampedSet : Finset F := Finset.univ.image key

theorem clampedSet_card : clampedSet.card = 2 ^ 106 := by
  classical
  rw [clampedSet, Finset.card_image_of_injective _ key_injective, Finset.card_univ, key_card]

abbrev Clamped := Set.range key
instance : Fintype Clamped := Fintype.ofFinite _

theorem mem_clamped_iff (r : F) : r ∈ clampedSet ↔ r ∈ Set.range key := by
  classical
  simp [clampedSet]

def clampedEquiv : Key ≃ Clamped := Equiv.ofInjective key key_injective

theorem clamped_card : Fintype.card Clamped = 2 ^ 106 := by
  rw [← Fintype.card_congr clampedEquiv, key_card]

def marked (b : List Byte) : F := markedNat b

theorem marked_val (b : List Byte) (hb : b.length ≤ 16) : (marked b).val = markedNat b := by
  apply ZMod.val_natCast_of_lt
  exact (markedNat_lt b hb).trans (by norm_num [P])

theorem marked_nonzero (b : List Byte) (hb : b.length ≤ 16) : marked b ≠ 0 := by
  intro h
  have hv := congrArg ZMod.val h
  rw [marked_val b hb] at hv
  have hz : markedNat b = 0 := by simpa using hv
  exact (Nat.ne_of_gt (markedNat_pos b)) hz

def poly (m : List Byte) : F[X] := positive ((chunks m).map marked).reverse

theorem poly_injective : Function.Injective poly := by
  intro m m' h
  have he := coeffs_inj_nonzero (as := ((chunks m).map marked).reverse)
    (bs := ((chunks m').map marked).reverse)
    (by
      intro a ha
      simp only [List.mem_reverse, List.mem_map] at ha
      obtain ⟨b, hb, rfl⟩ := ha
      exact marked_nonzero b (chunks_size m b hb))
    (by
      intro a ha
      simp only [List.mem_reverse, List.mem_map] at ha
      obtain ⟨b, hb, rfl⟩ := ha
      exact marked_nonzero b (chunks_size m' b hb))
    (positive_injective h)
  have hc : (chunks m).map markedNat = (chunks m').map markedNat := by
    have hv := congrArg (fun l : List F => l.reverse.map ZMod.val) he
    have hm (v : List Byte) : (chunks v).map (ZMod.val ∘ marked) =
        (chunks v).map markedNat := by
      apply List.map_congr_left
      intro b hb
      exact marked_val b (chunks_size v b hb)
    simpa only [List.reverse_reverse, List.map_map, hm] using hv
  have hc' : chunks m = chunks m' := List.map_injective_iff.mpr markedNat_injective hc
  have hf := congrArg List.flatten hc'
  simpa only [chunks_flatten] using hf

theorem degree_bound (m : List Byte) : (poly m).natDegree ≤ (m.length + 15) / 16 := by
  simpa [poly, chunks_length] using positive_degree ((chunks m).map marked).reverse

/-- Output reduction is on the canonical representative, not a field homomorphism. -/
def project (a : F) : ZMod Q := a.val

/-- A residue modulo Q has at most four canonical representatives below P. -/
theorem projection_fibre_cover (a : F) (g : ZMod Q) (h : project a = g) :
    ∃ i : Fin 4, a.val = g.val + i.val * Q := by
  have ha := ZMod.val_lt a
  have hm : a.val % Q = g.val := by simpa [project, ZMod.val_natCast] using congrArg ZMod.val h
  have hd := Nat.mod_add_div a.val Q
  have hi : a.val / Q < 4 := by norm_num [P, Q] at *; omega
  exact ⟨⟨a.val / Q, hi⟩, by simpa only [hm, Nat.mul_comm] using hd.symm⟩

def target (g : ZMod Q) (i : Fin 8) : F :=
  ((g.val : ℤ) + ((i.val : ℤ) - 4) * (Q : ℤ) : ℤ)

/-- At most eight explicit field targets cover every projected difference.
The quotient is in [-4,3]; no independence of the unreduced evaluations is used. -/
theorem projection_cover (a b : F) (g : ZMod Q)
    (h : project a - project b = g) :
    ∃ i : Fin 8, a - b = target g i := by
  let z : ℤ := (a.val : ℤ) - b.val
  have hz : (z : ZMod Q) = (g.val : ℤ) := by
    simpa [z, project] using h
  have hm : z % (Q : ℤ) = (g.val : ℤ) := by
    have he := (ZMod.intCast_eq_intCast_iff' z g.val Q).mp hz
    have hg : (g.val : ℤ) < Q := by exact_mod_cast ZMod.val_lt g
    rwa [Int.emod_eq_of_lt (by positivity) hg] at he
  have ha := ZMod.val_lt a
  have hb := ZMod.val_lt b
  have hg := ZMod.val_lt g
  have hd := Int.emod_add_mul_ediv z (Q : ℤ)
  have hi : 0 ≤ z / (Q : ℤ) + 4 ∧ z / (Q : ℤ) + 4 < 8 := by
    norm_num [P, Q] at ha hb hg hm hd ⊢
    dsimp [z] at *
    omega
  let i : Fin 8 := ⟨(z / (Q : ℤ) + 4).toNat, by omega⟩
  refine ⟨i, ?_⟩
  have hid : (i.val : ℤ) = z / (Q : ℤ) + 4 := by dsimp [i]; omega
  have he : z = (g.val : ℤ) + ((i.val : ℤ) - 4) * (Q : ℤ) := by
    rw [hid]; rw [hm] at hd; linarith
  have hf := congrArg (fun x : ℤ => (x : F)) he
  simpa [z, target] using hf

def targets (g : ZMod Q) : Finset F := Finset.univ.image (target g)

theorem targets_card (g : ZMod Q) : (targets g).card ≤ 8 := by
  exact (Finset.card_image_le).trans_eq (by simp)

def hash (r : Key) (m : List Byte) : ZMod Q := project ((poly m).eval (key r))
def tag (r : Key) (s : ZMod Q) (m : List Byte) : ZMod Q := hash r m + s

theorem pad_cancels (r : Key) (s : ZMod Q) (m m' : List Byte) :
    tag r s m = tag r s m' ↔ hash r m = hash r m' := by simp [tag]

/-- Bernstein Theorem 3.3 on the RFC clamped key set, with the published 8. -/
theorem differential_bound (m m' : List Byte) (hne : m ≠ m') (n : ℕ)
    (hn : (m.length + 15) / 16 ≤ n) (hn' : (m'.length + 15) / 16 ≤ n)
    (g : ZMod Q) :
    uniformProb (fun r : Key => hash r m - hash r m' = g) ≤
      (8 * n : ℕ) / (2 ^ 106 : ℚ≥0) := by
  classical
  have h := target_bound key key_injective (poly m) (poly m')
    (positive_zero _) (positive_zero _) (fun h => hne (poly_injective h))
    (targets g) (fun r => hash r m - hash r m' = g) (by
      intro r hr
      obtain ⟨i, hi⟩ := projection_cover _ _ g hr
      exact Finset.mem_image.mpr ⟨i, Finset.mem_univ _, hi.symm⟩)
  rw [key_card] at h
  simp only [Nat.cast_pow, Nat.cast_ofNat] at h
  apply h.trans
  apply div_le_div_of_nonneg_right _ (by positivity)
  exact_mod_cast Nat.mul_le_mul (targets_card g)
    (max_le ((degree_bound m).trans hn) ((degree_bound m').trans hn'))

theorem collision_bound (m m' : List Byte) (hne : m ≠ m') (n : ℕ)
    (hn : (m.length + 15) / 16 ≤ n) (hn' : (m'.length + 15) / 16 ≤ n) :
    uniformProb (fun r : Key => hash r m = hash r m') ≤
      (8 * n : ℕ) / (2 ^ 106 : ℚ≥0) := by
  simpa only [sub_eq_zero] using differential_bound m m' hne n hn hn' 0

theorem clamped_differential_bound (m m' : List Byte) (hne : m ≠ m') (n : ℕ)
    (hn : (m.length + 15) / 16 ≤ n) (hn' : (m'.length + 15) / 16 ≤ n)
    (g : ZMod Q) :
    uniformProb (fun r : Clamped => project ((poly m).eval r.val) -
      project ((poly m').eval r.val) = g) ≤ (8 * n : ℕ) / (2 ^ 106 : ℚ≥0) := by
  rw [← uniformProb_equiv clampedEquiv]
  exact differential_bound m m' hne n hn hn' g

def collisionTarget (i : Fin 7) : F := (((i.val : ℤ) - 3) * (Q : ℤ) : ℤ)

/-- Equality has only seven lifts: -3Q, -2Q, -Q, 0, Q, 2Q, 3Q. -/
theorem collision_projection_cover (a b : F) (h : project a = project b) :
    ∃ i : Fin 7, a - b = collisionTarget i := by
  let z : ℤ := (a.val : ℤ) - b.val
  have hz : (z : ZMod Q) = (0 : ℤ) := by simpa [z, project, sub_eq_zero] using h
  have hm : z % (Q : ℤ) = 0 := by
    simpa using (ZMod.intCast_eq_intCast_iff' z 0 Q).mp hz
  have ha := ZMod.val_lt a
  have hb := ZMod.val_lt b
  have hd := Int.emod_add_mul_ediv z (Q : ℤ)
  have hi : 0 ≤ z / (Q : ℤ) + 3 ∧ z / (Q : ℤ) + 3 < 7 := by
    norm_num [P, Q] at ha hb hm hd ⊢
    dsimp [z] at *
    omega
  let i : Fin 7 := ⟨(z / (Q : ℤ) + 3).toNat, by omega⟩
  refine ⟨i, ?_⟩
  have hid : (i.val : ℤ) = z / (Q : ℤ) + 3 := by dsimp [i]; omega
  have he : z = ((i.val : ℤ) - 3) * (Q : ℤ) := by
    rw [hid]; rw [hm] at hd; linarith
  have hf := congrArg (fun x : ℤ => (x : F)) he
  simpa [z, collisionTarget] using hf

theorem collision_bound_seven (m m' : List Byte) (hne : m ≠ m') (n : ℕ)
    (hn : (m.length + 15) / 16 ≤ n) (hn' : (m'.length + 15) / 16 ≤ n) :
    uniformProb (fun r : Key => hash r m = hash r m') ≤
      (7 * n : ℕ) / (2 ^ 106 : ℚ≥0) := by
  classical
  let ts := Finset.univ.image collisionTarget
  have h := target_bound key key_injective (poly m) (poly m')
    (positive_zero _) (positive_zero _) (fun h => hne (poly_injective h))
    ts (fun r => hash r m = hash r m') (by
      intro r hr
      obtain ⟨i, hi⟩ := collision_projection_cover _ _ hr
      exact Finset.mem_image.mpr ⟨i, Finset.mem_univ _, hi.symm⟩)
  have ht : ts.card ≤ 7 := Finset.card_image_le.trans_eq (by simp)
  rw [key_card] at h
  simp only [Nat.cast_pow, Nat.cast_ofNat] at h
  apply h.trans
  apply div_le_div_of_nonneg_right _ (by positivity)
  exact_mod_cast Nat.mul_le_mul ht
    (max_le ((degree_bound m).trans hn) ((degree_bound m').trans hn'))

theorem tag_collision_bound (m m' : List Byte) (hne : m ≠ m') (n : ℕ)
    (hn : (m.length + 15) / 16 ≤ n) (hn' : (m'.length + 15) / 16 ≤ n) :
    uniformProb (fun k : Key × ZMod Q => tag k.1 k.2 m = tag k.1 k.2 m') ≤
      (7 * n : ℕ) / (2 ^ 106 : ℚ≥0) := by
  simp only [pad_cancels]
  rw [uniformProb_ignore_right (A := Key) (B := ZMod Q)
    (fun r => hash r m = hash r m')]
  exact collision_bound_seven m m' hne n hn hn'

theorem zero_key (r : Key) (hr : key r = 0) (s : ZMod Q) (m : List Byte) :
    tag r s m = s := by simp [tag, hash, hr, poly, positive, project]

theorem word_bound (L : ℕ) (m m' : List Byte) (hne : m ≠ m')
    (hm : m.length ≤ 8 * L) (hm' : m'.length ≤ 8 * L) :
    uniformProb (fun k : Key × ZMod Q => tag k.1 k.2 m = tag k.1 k.2 m') ≤
      (7 * ((L + 1) / 2) : ℕ) / (2 ^ 106 : ℚ≥0) :=
  tag_collision_bound m m' hne _ (by omega) (by omega)

#print axioms collision_bound
#print axioms clampedSet_card
#print axioms projection_cover
#print axioms poly_injective
#print axioms tag_collision_bound
end
end ProvenHashes.Classic.Poly1305
