import ProvenHashes.Highway.ArithmeticFibre
import ProvenHashes.Highway.FiniteCounts
namespace ProvenHashes.Highway
set_option maxRecDepth 10000
set_option maxHeartbeats 1000000
-- The constructor-name style linter exceeds its recursion limit on this finite-set proof.
set_option linter.constructorNameAsVariable false

/-- The 24-bit arithmetic fibre is counted by a bijection, not by enumeration. -/
theorem arithmetic_fibre_card (sigma : Fin 2) (s : Fin 65536) (r : Fin 256)
    (h0 : sigma.val = 0 → 203 ≤ windowStart s.val r.val)
    (h1 : sigma.val = 1 → windowStart s.val r.val ≤ 203) :
    (Finset.univ.filter fun p : Fin 65536 × Fin 256 =>
      arithmeticFibre sigma.val s.val r.val p.1.val p.2.val).card = 239 := by
  have hsig : sigma.val ≤ 1 := by omega
  have hlow : lowSolution s.val r.val < 256 := Nat.mod_lt _ (by decide)
  have hq : windowStart s.val r.val < 257 := by
    unfold windowStart
    have hs := s.isLt
    omega
  let q : Fin 257 := ⟨windowStart s.val r.val, hq⟩
  have hiff (p : Fin 65536 × Fin 256) :=
    arithmetic_fibre_iff sigma.val s.val r.val p.1.val p.2.val
      hsig s.isLt r.isLt p.1.isLt p.2.isLt
  let f (p : Fin 65536 × Fin 256) : Fin 256 :=
    ⟨p.1.val/256, by have h := p.1.isLt; omega⟩
  have hc : (Finset.univ.filter fun p : Fin 65536 × Fin 256 =>
      arithmeticFibre sigma.val s.val r.val p.1.val p.2.val).card =
      (Finset.univ.filter fun j : Fin 256 => windowGood sigma.val q.val j.val).card := by
    apply Finset.card_bij (fun p _ => f p)
    · intro p hp
      simp only [Finset.mem_filter, Finset.mem_univ, true_and] at hp ⊢
      exact ((hiff p).mp hp).2.2
    · intro a ha b hb hab
      simp only [Finset.mem_filter, Finset.mem_univ, true_and] at ha hb
      have hA := (hiff a).mp ha
      have hB := (hiff b).mp hb
      have hdiv : a.1.val/256 = b.1.val/256 := congrArg Fin.val hab
      apply Prod.ext
      · apply Fin.ext
        omega
      · apply Fin.ext
        rw [hA.2.1, hB.2.1, hdiv]
    · intro j hj
      simp only [Finset.mem_filter, Finset.mem_univ, true_and] at hj
      have hgood := hj
      let x : Fin 65536 := ⟨lowSolution s.val r.val+256*j.val, by
        have hj := j.isLt
        omega⟩
      let w : Fin 256 := ⟨middleSolution sigma.val q.val j.val, Nat.mod_lt _ (by decide)⟩
      have hx0 : x.val%256 = lowSolution s.val r.val := by dsimp [x]; omega
      have hx1 : x.val/256 = j.val := by dsimp [x]; omega
      refine ⟨(x,w), ?_, ?_⟩
      · simp only [Finset.mem_filter, Finset.mem_univ, true_and]
        apply (hiff (x,w)).mpr
        dsimp only
        rw [hx0, hx1]
        exact ⟨rfl, rfl, hgood⟩
      · apply Fin.ext
        exact hx1
  exact hc.trans (window_card sigma q h0 h1)

end ProvenHashes.Highway
