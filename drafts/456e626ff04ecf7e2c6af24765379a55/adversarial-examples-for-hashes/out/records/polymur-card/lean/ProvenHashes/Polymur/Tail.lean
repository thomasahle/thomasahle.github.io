import ProvenHashes.Polymur.Algebra

noncomputable section

namespace ProvenHashes.Polymur
open Polynomial
attribute [local simp] Polynomial.coeff_monomial

/-- The source's overlapping seven-byte reads, in source index order. -/
def tailWords (m : Bytes) : Words :=
  ![window m 0 7, window m ((m.length-7)/2) 7, window m (m.length-7) 7,
    window m 7 7, window m 14 7, window m (m.length-21) 7, window m (m.length-14) 7]

def tailPoly (m : Bytes) : F[X] :=
  if m.length ≤ 7 then shortPoly m.length (tailWords m)
  else if m.length ≤ 21 then midPoly m.length (tailWords m)
  else longPoly m.length (tailWords m)

lemma tail_length_coeff (m : Bytes) : (tailPoly m).coeff 1 = (m.length : F) := by
  unfold tailPoly
  split_ifs <;> simp [shortPoly, midPoly, longPoly]

lemma tail_degree_le (m : Bytes) : (tailPoly m).natDegree ≤ 13 := by
  unfold tailPoly
  split_ifs
  · exact (short_degree_le _ _).trans (by omega)
  · exact (mid_degree_le _ _).trans (by omega)
  · exact long_degree_le _ _

lemma tail_small_degree (m : Bytes) (h : m.length ≤ 21) : (tailPoly m).natDegree ≤ 9 := by
  unfold tailPoly
  split_ifs
  · exact (short_degree_le _ _).trans (by omega)
  · exact mid_degree_le _ _

lemma tail_inj {a b : Bytes} (ha : a.length ≤ 49) (hb : b.length ≤ 49)
    (h : tailPoly a = tailPoly b) : a = b := by
  have hl : a.length = b.length := by
    apply cast_inj (lt_of_le_of_lt ha (by norm_num [p])) (lt_of_le_of_lt hb (by norm_num [p]))
    simpa only [tail_length_coeff] using congrArg (fun q : F[X] => q.coeff 1) h
  apply List.ext_getElem hl
  intro i hi hi'
  by_cases hs : a.length ≤ 7
  · have hc := congrArg (fun q : F[X] => q.coeff 2) h
    simp [tailPoly, hs, ← hl, shortPoly, tailWords] at hc
    exact window_byte hl (by omega) (by omega) hi (by omega) hc
  · by_cases hm : a.length ≤ 21
    · have hc0 := congrArg (fun q : F[X] => q.coeff 7) h
      have hc1 := congrArg (fun q : F[X] => q.coeff 2) h
      have hc2 := congrArg (fun q : F[X] => q.coeff 3) h
      simp [tailPoly, hs, hm, ← hl, midPoly, tailWords] at hc0 hc1 hc2
      have cover : i < 7 ∨ ((a.length-7)/2 ≤ i ∧ i < (a.length-7)/2+7) ∨ a.length-7 ≤ i := by omega
      rcases cover with h0 | ⟨h1,h2⟩ | h2
      · exact window_byte hl (by omega) (by omega) hi (by omega) hc0
      · exact window_byte hl (by omega) h1 hi h2 hc1
      · exact window_byte hl (by omega) h2 hi (by omega) hc2
    · have hw : tailWords a = tailWords b := by
        apply long_decode
        simpa [tailPoly, hs, hm, ← hl] using h
      have hw' (j : Fin 7) := congrFun hw j
      have cover : i < 7 ∨ (7 ≤ i ∧ i < 14) ∨ (14 ≤ i ∧ i < 21) ∨
          ((a.length-7)/2 ≤ i ∧ i < (a.length-7)/2+7) ∨
          (a.length-21 ≤ i ∧ i < a.length-14) ∨
          (a.length-14 ≤ i ∧ i < a.length-7) ∨ a.length-7 ≤ i := by omega
      rcases cover with h0 | ⟨h1,h2⟩ | ⟨h1,h2⟩ | ⟨h1,h2⟩ | ⟨h1,h2⟩ | ⟨h1,h2⟩ | h2
      · exact window_byte (start := 0) (width := 7) hl (by omega) (by omega) hi (by omega) (by simpa [tailWords] using hw' 0)
      · exact window_byte (start := 7) (width := 7) hl (by omega) h1 hi (by omega) (by simpa [tailWords] using hw' 3)
      · exact window_byte (start := 14) (width := 7) hl (by omega) h1 hi (by omega) (by simpa [tailWords] using hw' 4)
      · exact window_byte (start := (a.length-7)/2) (width := 7) hl (by omega) h1 hi h2 (by simpa [tailWords, ← hl] using hw' 1)
      · exact window_byte (start := a.length-21) (width := 7) hl (by omega) h1 hi (by omega) (by simpa [tailWords, ← hl] using hw' 5)
      · exact window_byte (start := a.length-14) (width := 7) hl (by omega) h1 hi (by omega) (by simpa [tailWords, ← hl] using hw' 6)
      · exact window_byte (start := a.length-7) (width := 7) hl (by omega) h2 hi (by omega) (by simpa [tailWords, ← hl] using hw' 2)

end ProvenHashes.Polymur
