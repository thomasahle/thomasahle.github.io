import ProvenHashes.Polynomial

namespace ProvenHashes.BRW
open Polynomial
noncomputable section
variable {F : Type*} [Field F]

/-- Bernstein §5.2, with an explicit recursion depth. The valid input range is
n < 2^(h+2); smaller blocks skip unused levels. Indices are zero-based. -/
def tree : ℕ → ℕ → (ℕ → F) → F[X]
  | 0, 0, _ => 0
  | 0, 1, m => C (m 0)
  | 0, 2, m => C (m 0) * X + C (m 1)
  | 0, _ + 3, m => (X + C (m 0)) * (X^2 + C (m 1)) + C (m 2)
  | h + 1, n, m =>
    let t := 2^(h+2)
    if n < t then tree h n m else
      tree h (t-1) m * (X^t + C (m (t-1))) + tree h (n-t) (fun i => m (t+i))

/-- A split is uniquely decodable when the right top coefficient is fixed. -/
theorem split_injective (t : ℕ) (ht : 0 < t) (p p' r r' : F[X]) (a a' : F)
    (hp : p.natDegree < t) (hp' : p'.natDegree < t)
    (hr : r.natDegree < t) (hr' : r'.natDegree < t)
    (hmonic : p.coeff (t-1) = 1) (htop : r.coeff (t-1) = r'.coeff (t-1))
    (he : p * (X^t + C a) + r = p' * (X^t + C a') + r') :
    p = p' ∧ a = a' ∧ r = r' := by
  have hleft : p = p' := by
    ext i
    have hh := congrArg (fun f : F[X] => f.coeff (i+t)) he
    simpa [mul_add, coeff_add, coeff_mul_X_pow, coeff_mul_C,
      coeff_eq_zero_of_natDegree_lt (show p.natDegree < i+t by omega),
      coeff_eq_zero_of_natDegree_lt (show p'.natDegree < i+t by omega),
      coeff_eq_zero_of_natDegree_lt (show r.natDegree < i+t by omega),
      coeff_eq_zero_of_natDegree_lt (show r'.natDegree < i+t by omega)] using hh
  subst p'
  have hmid : a = a' := by
    have hh := congrArg (fun f : F[X] => f.coeff (t-1)) he
    simpa [mul_add, coeff_add, coeff_mul_X_pow', coeff_mul_C, hmonic,
      show ¬ t ≤ t-1 by omega, htop] using hh
  subst a'
  exact ⟨rfl, rfl, add_left_cancel he⟩

/-- The top coefficient at every level depends only on length, not the message. -/
theorem tree_shape (h n : ℕ) (m : ℕ → F) (hn : n < 2^(h+2)) :
    (tree h n m).natDegree < 2^(h+2) ∧
    (tree h n m).coeff (2^(h+2)-1) =
      if 2^(h+2)/2 ≤ n ∧ 3 ≤ n then 1 else 0 := by
  induction h generalizing n m with
  | zero =>
    have hn' : n < 4 := by simpa using hn
    interval_cases n <;> constructor
    all_goals simp [tree, mul_add, add_mul, coeff_add, coeff_mul_X_pow']
    all_goals (compute_degree; norm_num)
  | succ h ih =>
    let t := 2^(h+2)
    have ht : 4 ≤ t := by
      dsimp [t]
      calc
        4 = 2^2 := by norm_num
        _ ≤ 2^(h+2) := Nat.pow_le_pow_right (by omega) (by omega)
    have hc : 2^(h+1+2) = 2*t := by dsimp [t]; rw [show h+1+2 = (h+2)+1 by omega, pow_succ]; omega
    change (tree (h+1) n m).natDegree < 2^(h+1+2) ∧ _
    rw [tree, hc]
    by_cases hs : n < t
    · rw [if_pos hs]
      obtain ⟨hd, _⟩ := ih n m hs
      constructor
      · omega
      · rw [coeff_eq_zero_of_natDegree_lt (by omega)]
        simp [show ¬ t ≤ n by omega]
    · rw [if_neg hs]
      obtain ⟨hp, htop⟩ := ih (t-1) m (by omega)
      obtain ⟨hr, _⟩ := ih (n-t) (fun i => m (t+i)) (by rw [hc] at hn; omega)
      have htop' : (tree h (t-1) m).coeff (t-1) = 1 := by
        change (tree h (t-1) m).coeff (t-1) = if t/2 ≤ t-1 ∧ 3 ≤ t-1 then 1 else 0 at htop
        rw [if_pos (show t/2 ≤ t-1 ∧ 3 ≤ t-1 by omega)] at htop
        exact htop
      constructor
      · have hm : (tree h (t-1) m * (X^t+C (m (t-1)))).natDegree ≤
            (tree h (t-1) m).natDegree + (X^t+C (m (t-1)) : F[X]).natDegree := natDegree_mul_le
        have ha := natDegree_add_le (X^t : F[X]) (C (m (t-1)))
        have hb := natDegree_add_le
          (tree h (t-1) m * (X^t+C (m (t-1)))) (tree h (n-t) (fun i => m (t+i)))
        simp only [natDegree_X_pow, natDegree_C] at ha
        dsimp [t] at *
        omega
      · change (tree h (t-1) m * (X^t+C (m (t-1))) + tree h (n-t) (fun i => m (t+i))).coeff (2*t-1) = if 2*t/2 ≤ n ∧ 3 ≤ n then 1 else 0
        have hz : 2*t-1 = (t-1)+t := by omega
        simp only [mul_add, coeff_add, hz, coeff_mul_X_pow, coeff_mul_C]
        rw [htop', coeff_eq_zero_of_natDegree_lt (show (tree h (t-1) m).natDegree < t-1+t by omega),
          coeff_eq_zero_of_natDegree_lt (show (tree h (n-t) (fun i => m (t+i))).natDegree < t-1+t by omega)]
        simp [show t ≤ n ∧ 3 ≤ n by omega]

/-- Bernstein's high-coefficient / middle-word / right-block decoder. -/
theorem tree_injective (h n : ℕ) (m m' : ℕ → F) (hn : n < 2^(h+2))
    (he : tree h n m = tree h n m') : ∀ i < n, m i = m' i := by
  induction h generalizing n m m' with
  | zero =>
    have hn' : n < 4 := by simpa using hn
    interval_cases n
    · omega
    · intro i hi
      have hh := congrArg (fun p : F[X] => p.coeff 0) he
      have hi' : i = 0 := by omega
      simpa [tree, hi'] using hh
    · have h0 := congrArg (fun p : F[X] => p.coeff 1) he
      have h1 := congrArg (fun p : F[X] => p.coeff 0) he
      simp [tree] at h0 h1
      intro i hi
      interval_cases i <;> assumption
    · have h0 := congrArg (fun p : F[X] => p.coeff 2) he
      have h1 := congrArg (fun p : F[X] => p.coeff 1) he
      have h2 := congrArg (fun p : F[X] => p.coeff 0) he
      simp [tree, mul_add, add_mul, coeff_mul_X_pow'] at h0 h1 h2
      rw [h0, h1] at h2
      have h2' : m 2 = m' 2 := add_left_cancel h2
      intro i hi
      interval_cases i <;> assumption
  | succ h ih =>
    let t := 2^(h+2)
    have ht : 4 ≤ t := by
      dsimp [t]
      calc
        4 = 2^2 := by norm_num
        _ ≤ 2^(h+2) := Nat.pow_le_pow_right (by omega) (by omega)
    have hc : 2^(h+1+2) = 2*t := by dsimp [t]; rw [show h+1+2 = (h+2)+1 by omega, pow_succ]; omega
    by_cases hs : n < t
    · simp only [tree, if_pos (show n < 2^(h+2) from hs)] at he
      exact ih n m m' hs he
    · simp only [tree, if_neg (show ¬ n < 2^(h+2) from hs)] at he
      have hl : t-1 < 2^(h+2) := by change t-1 < t; omega
      have hr : n-t < 2^(h+2) := by rw [hc] at hn; change n-t < t; omega
      obtain ⟨hdp, htp⟩ := tree_shape h (t-1) m hl
      have hm : (tree h (t-1) m).coeff (t-1) = 1 := by
        simpa [show 2^(h+2)/2 ≤ t-1 ∧ 3 ≤ t-1 by change t/2 ≤ t-1 ∧ 3 ≤ t-1; omega] using htp
      obtain ⟨hel, hea, her⟩ := split_injective t (by omega) _ _ _ _ _ _
        hdp (tree_shape h (t-1) m' hl).1
        (tree_shape h (n-t) (fun i => m (t+i)) hr).1
        (tree_shape h (n-t) (fun i => m' (t+i)) hr).1 hm
        ((tree_shape h (n-t) (fun i => m (t+i)) hr).2.trans
          (tree_shape h (n-t) (fun i => m' (t+i)) hr).2.symm) he
      intro i hi
      by_cases hil : i < t-1
      · exact ih (t-1) m m' hl hel i hil
      · by_cases hie : i = t-1
        · simpa [hie] using hea
        · have hir : i-t < n-t := by omega
          have hh := ih (n-t) _ _ hr her (i-t) hir
          simpa only [Nat.add_sub_of_le (show t ≤ i by omega)] using hh

/-- The simpler degree bound used by the chart, including lengths 0, 1, 2. -/
theorem tree_degree (h n : ℕ) (m : ℕ → F) (hn : n < 2^(h+2)) :
    (tree h n m).natDegree ≤ 2*n-1 := by
  induction h generalizing n m with
  | zero =>
    have hn' : n < 4 := by simpa using hn
    interval_cases n <;> simp [tree]
    all_goals (compute_degree; norm_num)
  | succ h ih =>
    by_cases hs : n < 2^(h+2)
    · simpa only [tree, if_pos hs] using ih n m hs
    · have hd := (tree_shape (h+1) n m hn).1
      have hc : 2^(h+1+2) = 2 * 2^(h+2) := by
        rw [show h+1+2 = (h+2)+1 by omega, pow_succ]; omega
      rw [hc] at hd
      omega

/-- Extra unused recursion levels do not affect the polynomial. -/
theorem tree_stable {h H n : ℕ} (hh : h ≤ H) (hn : n < 2^(h+2)) (m : ℕ → F) :
    tree H n m = tree h n m := by
  induction H, hh using Nat.le_induction with
  | base => rfl
  | succ H hh ih =>
    have hn' : n < 2^(H+2) := hn.trans_le (Nat.pow_le_pow_right (by omega) (by omega))
    simpa only [tree, if_pos hn'] using ih

/-- Canonical BRW polynomial; log depth only controls recursion, not semantics. -/
def polynomial (n : ℕ) (m : ℕ → F) : F[X] := tree (Nat.log 2 n) n m

/-- Every sufficient depth computes the same canonical polynomial. -/
theorem tree_eq_polynomial (h n : ℕ) (m : ℕ → F) (hn : n < 2^(h+2)) :
    tree h n m = polynomial n m := by
  have hc : n < 2^(Nat.log 2 n + 2) :=
    (Nat.lt_pow_succ_log_self (by omega : 1 < 2) n).trans_le
      (Nat.pow_le_pow_right (by omega) (by omega))
  unfold polynomial
  rcases le_total h (Nat.log 2 n) with hh | hh
  · exact (tree_stable hh hn m).symm
  · exact tree_stable hh hc m

/-- Exactly Bernstein §5.2's split, t=2^k, t≤n<2t, k≥2. -/
theorem polynomial_recursion (k n : ℕ) (hk : 2 ≤ k)
    (hlo : 2^k ≤ n) (hhi : n < 2^(k+1)) (m : ℕ → F) :
    polynomial n m = polynomial (2^k-1) m * (X^(2^k) + C (m (2^k-1))) +
      polynomial (n-2^k) (fun i => m (2^k+i)) := by
  obtain ⟨h, rfl⟩ : ∃ h, k = h+2 := ⟨k-2, by omega⟩
  have ht : 0 < 2^(h+2) := by positivity
  have hc : 2^(h+2+1) = 2 * 2^(h+2) := by rw [pow_succ]; omega
  have hn : n < 2^(h+1+2) := by convert hhi using 1
  rw [← tree_eq_polynomial (h+1) n m hn, tree, if_neg (by omega : ¬ n < 2^(h+2))]
  rw [tree_eq_polynomial h (2^(h+2)-1) m (by omega),
    tree_eq_polynomial h (n-2^(h+2)) (fun i => m (2^(h+2)+i)) (by rw [hc] at hhi; omega)]

/-- The four base cases from the write-up. -/
theorem polynomial_base (m : ℕ → F) :
    polynomial 0 m = 0 ∧ polynomial 1 m = C (m 0) ∧
    polynomial 2 m = C (m 0)*X+C (m 1) ∧
    polynomial 3 m = (X+C (m 0))*(X^2+C (m 1))+C (m 2) := by
  refine ⟨?_, ?_, ?_, ?_⟩
  all_goals rw [← tree_eq_polynomial 0 _ m (by norm_num)]
  all_goals rfl

/-- Fixed-length messages, with irrelevant out-of-range coordinates set to zero. -/
def encode {n : ℕ} (m : Fin n → F) : F[X] :=
  polynomial n (fun i => if hi : i < n then m ⟨i, hi⟩ else 0)

theorem encode_injective (n : ℕ) : Function.Injective (encode (F := F) (n := n)) := by
  intro m m' he
  have hn : n < 2^(Nat.log 2 n + 2) :=
    (Nat.lt_pow_succ_log_self (by omega : 1 < 2) n).trans_le
      (Nat.pow_le_pow_right (by omega) (by omega))
  have hh := tree_injective (Nat.log 2 n) n _ _ hn he
  funext i
  simpa only [dif_pos i.isLt] using hh i i.isLt

/-- Requested BRW degree bound, with no message restrictions. -/
theorem encode_degree {n : ℕ} (m : Fin n → F) : (encode m).natDegree ≤ 2*n-1 := by
  apply tree_degree
  exact (Nat.lt_pow_succ_log_self (by omega : 1 < 2) n).trans_le
    (Nat.pow_le_pow_right (by omega) (by omega))

/-- BRW evaluation with one uniform field key. -/
def hash {n : ℕ} (m : Fin n → F) (x : F) : F := (encode m).eval x

/-- Distinct fixed-length messages: injectivity followed by the univariate root bound. -/
theorem collision_bound [Fintype F] {n : ℕ} (m m' : Fin n → F) (hne : m ≠ m') :
    uniformProb (fun x : F => hash m x = hash m' x) ≤
      ((2*n-1 : ℕ) : ℚ≥0) / Fintype.card F := by
  classical
  let p := encode m - encode m'
  have hp : p ≠ 0 := fun h => hne (encode_injective n (sub_eq_zero.mp h))
  have hd : p.natDegree ≤ 2*n-1 :=
    (natDegree_sub_le _ _).trans (max_le (encode_degree m) (encode_degree m'))
  have hc := (polynomial_zero_count p hp).trans hd
  have he : (fun x : F => hash m x = hash m' x) = (fun x : F => p.eval x = 0) := by
    funext x
    simp [hash, p, sub_eq_zero]
  rw [he, uniformProb]
  exact div_le_div_of_nonneg_right (by exact_mod_cast hc) (by positivity)

/-- The BRW chart row over the full field of 2^64 elements. This holds for all
fixed lengths, hence in particular for 1≤L≤2^61-1. -/
theorem collision_bound_gf64 {L : ℕ} (m m' : Fin L → GaloisField 2 64) (hne : m ≠ m') :
    uniformProb (fun x : GaloisField 2 64 => hash m x = hash m' x) ≤
      ((2*L-1 : ℕ) : ℚ≥0) / 2^64 := by
  simpa only [gf64_card, Nat.cast_pow, Nat.cast_ofNat] using collision_bound m m' hne

end
end ProvenHashes.BRW
