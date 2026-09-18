import ProvenHashes.UMASHModel

namespace ProvenHashes.UMASH

set_option maxRecDepth 8192
set_option maxHeartbeats 2000000

/-- Supports of width-w signed binary expansions of the integer n.  This is
the recurrence in GAP.md, with no unverified external enumeration. -/
def signedSupports : ℕ → ℤ → Finset ℕ
  | 0, n => if n = 0 then {0} else ∅
  | w+1, n =>
    if n % 2 = 0 then (signedSupports w (n/2)).image (2*·)
    else ((signedSupports w ((n-1)/2)) ∪
      (signedSupports w ((n+1)/2))).image (fun s => 2*s+1)

/-- The modulus is an argument so symbolic proofs never normalize the large
concrete enumeration when matching finite-set expressions. -/
def masksFor (w modulus : ℕ) : Finset ℕ :=
  (Finset.range ((2^w-1)/modulus+1)).biUnion fun a =>
    signedSupports w ((a : ℤ)*(modulus : ℤ))

def maskSet : Finset ℕ := masksFor 64 p

theorem xor_mem_signedSupports (w x y : ℕ) (hx : x < 2^w) (hy : y < 2^w) :
    x ^^^ y ∈ signedSupports w ((x : ℤ) - y) := by
  induction w generalizing x y with
  | zero =>
    have hx0 : x = 0 := by simpa using hx
    have hy0 : y = 0 := by simpa using hy
    subst x; subst y
    simp [signedSupports]
  | succ w ih =>
    have hxd : x/2 < 2^w := by rw [pow_succ] at hx; omega
    have hyd : y/2 < 2^w := by rw [pow_succ] at hy; omega
    have h := ih (x/2) (y/2) hxd hyd
    have hxm : x%2 < 2 := Nat.mod_lt _ (by decide)
    have hym : y%2 < 2 := Nat.mod_lt _ (by decide)
    have hxor : x ^^^ y = 2 * ((x/2) ^^^ (y/2)) + (x+y)%2 := by
      have hd := Nat.mod_add_div (x ^^^ y) 2
      rw [Nat.xor_div_two, Nat.xor_mod_two_eq] at hd
      omega
    simp only [signedSupports]
    split_ifs with he
    · have hdiv : ((x : ℤ) - y)/2 = (x/2 : ℕ) - (y/2 : ℕ) := by omega
      have hm : (x+y)%2 = 0 := by omega
      rw [hdiv, hxor, hm, Nat.add_zero]
      exact Finset.mem_image.mpr ⟨_, h, rfl⟩
    · have hm : (x+y)%2 = 1 := by omega
      rw [hxor, hm]
      apply Finset.mem_image.mpr
      refine ⟨_, ?_, rfl⟩
      by_cases hxy : x%2 = 1
      · have hdiv : ((x : ℤ) - y - 1)/2 = (x/2 : ℕ) - (y/2 : ℕ) := by omega
        exact Finset.mem_union_left _ (hdiv ▸ h)
      · have hdiv : ((x : ℤ) - y + 1)/2 = (x/2 : ℕ) - (y/2 : ℕ) := by omega
        exact Finset.mem_union_right _ (hdiv ▸ h)

theorem congruent_xor_mem_masksFor (w modulus x y : ℕ)
    (hx : x < 2^w) (hy : y < 2^w)
    (hmod : x % modulus = y % modulus) : x ^^^ y ∈ masksFor w modulus := by
  wlog hxy : y ≤ x generalizing x y
  · rw [Nat.xor_comm]
    exact this y x hy hx hmod.symm (by omega)
  let a := x / modulus - y / modulus
  have hd := Nat.div_le_div_right hxy (c := modulus)
  have ha : a < (2^w-1)/modulus+1 := by
    have htop := Nat.div_le_div_right (show x ≤ 2^w-1 by omega) (c := modulus)
    exact (Nat.sub_le _ _).trans_lt (htop.trans_lt (Nat.lt_succ_self _))
  have he : (x : ℤ) - y = (a : ℤ)*modulus := by
    have hxq : (x : ℤ) = (x % modulus : ℕ) + (modulus : ℤ)*(x / modulus : ℕ) := by
      exact_mod_cast (Nat.mod_add_div x modulus).symm
    have hyq : (y : ℤ) = (y % modulus : ℕ) + (modulus : ℤ)*(y / modulus : ℕ) := by
      exact_mod_cast (Nat.mod_add_div y modulus).symm
    dsimp [a]
    rw [Nat.cast_sub hd]
    rw [hxq, hyq, hmod]
    ring
  have hm : x ^^^ y ∈ signedSupports w ((a : ℤ)*modulus) := by
    rw [← he]
    exact xor_mem_signedSupports w x y hx hy
  rw [masksFor]
  exact (Finset.mem_biUnion.mpr ⟨a, Finset.mem_range.mpr ha, hm⟩ :
    x ^^^ y ∈ (Finset.range ((2^w-1)/modulus+1)).biUnion
      (fun a => signedSupports w ((a : ℤ)*modulus)))

theorem congruent_xor_mem_maskSet (x y : ℕ) (hx : x < q) (hy : y < q)
    (hmod : x % p = y % p) : x ^^^ y ∈ maskSet := by
  exact congruent_xor_mem_masksFor 64 p x y hx hy hmod

end ProvenHashes.UMASH
