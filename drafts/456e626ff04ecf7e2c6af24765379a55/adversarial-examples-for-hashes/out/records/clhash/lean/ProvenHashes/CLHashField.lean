import ProvenHashes.CLHashCertificate
import ProvenHashes.ConcreteChainHash

noncomputable section
namespace ProvenHashes.CLHash
open Polynomial ChainHash

theorem modulus127_irreducible : Irreducible modulus127 := by
  obtain ⟨hpow, hcop⟩ := modulus127_certificate
  obtain ⟨hp, hdeg⟩ := modulus127_spec
  have hunit : ¬ IsUnit modulus127 := not_isUnit_of_natDegree_pos _ (by omega)
  obtain ⟨g, hgmon, hg, hgp⟩ := exists_monic_irreducible_factor modulus127 hunit
  have hd : g.natDegree ∣ 127 :=
    (irreducible_dvd_frobenius_iff g hg 127).mp (hgp.trans hpow)
  rcases (by norm_num : Nat.Prime 127).eq_one_or_self_of_dvd _ hd with hd | hd
  · have hd1 : g.natDegree ∣ 1 := by rw [hd]
    have hx := (irreducible_dvd_frobenius_iff g hg 1).mpr hd1
    exact (hg.not_isUnit (hcop.isUnit_of_dvd' hgp (by simpa using hx))).elim
  · have he : modulus127 = g := eq_of_monic_of_dvd_of_natDegree_le hgmon hp hgp (by omega)
    exact he.symm ▸ hg

instance : Fact (Irreducible modulus127) := ⟨modulus127_irreducible⟩
abbrev F127 := AdjoinRoot modulus127
abbrev F64 := BinaryQuotient

theorem remainder127_degree (a : F127) :
    (AdjoinRoot.modByMonicHom modulus127_spec.1 a).natDegree < 127 := by
  have hone : modulus127 ≠ 1 := by
    intro h
    have he := congrArg Polynomial.natDegree h
    rw [modulus127_spec.2, Polynomial.natDegree_one] at he
    omega
  induction a using AdjoinRoot.induction_on with
  | ih p =>
    rw [AdjoinRoot.modByMonicHom_mk]
    simpa only [modulus127_spec.2] using
      Polynomial.natDegree_modByMonic_lt p modulus127_spec.1 hone

/-- The literal polynomial-basis representation of GF(2^127). -/
def repr127 : Word 127 ≃+ F127 where
  toFun v := AdjoinRoot.mk modulus127 (pack 127 v)
  invFun a := fun i => (AdjoinRoot.modByMonicHom modulus127_spec.1 a).coeff i.val
  left_inv v := by
    have hd : (pack 127 v).degree < modulus127.degree := by
      rw [degree_eq_natDegree modulus127_spec.1.ne_zero, modulus127_spec.2]
      exact Polynomial.ofFn_degree_lt v
    have hr : (pack 127 v) %ₘ modulus127 = pack 127 v :=
      (Polynomial.modByMonic_eq_self_iff modulus127_spec.1).mpr hd
    funext i
    simp only [AdjoinRoot.modByMonicHom_mk, hr]
    exact Polynomial.ofFn_coeff_eq_val_of_lt v i.isLt
  right_inv a := by
    have hp : pack 127 (fun i => (AdjoinRoot.modByMonicHom modulus127_spec.1 a).coeff i.val) =
        AdjoinRoot.modByMonicHom modulus127_spec.1 a :=
      Polynomial.ofFn_comp_toFn_eq_id_of_natDegree_lt (remainder127_degree a)
    dsimp only
    rw [hp]
    exact AdjoinRoot.mk_leftInverse modulus127_spec.1 a
  map_add' v w := by simp only [map_add]

instance : Fintype F127 := Fintype.ofEquiv (Word 127) repr127.toEquiv

theorem field_cards : Fintype.card F127 = 2 ^ 127 ∧ Fintype.card F64 = 2 ^ 64 := by
  constructor
  · rw [← Fintype.card_congr repr127.toEquiv, word_card]
  · rw [← Fintype.card_congr fieldRepr.toEquiv, word_card]

theorem mk127_injective_below {p q : BitsPolynomial}
    (hp : p.natDegree < 127) (hq : q.natDegree < 127)
    (he : AdjoinRoot.mk modulus127 p = AdjoinRoot.mk modulus127 q) : p = q := by
  have hmod := congrArg (AdjoinRoot.modByMonicHom modulus127_spec.1) he
  simp only [AdjoinRoot.modByMonicHom_mk] at hmod
  have hd (r : BitsPolynomial) (hr : r.natDegree < 127) :
      r %ₘ modulus127 = r := by
    apply (Polynomial.modByMonic_eq_self_iff modulus127_spec.1).mpr
    rw [degree_eq_natDegree modulus127_spec.1.ne_zero, modulus127_spec.2]
    exact (Polynomial.degree_le_natDegree).trans_lt (by exact_mod_cast hr)
  rwa [hd p hp, hd q hq] at hmod

/-- The 126-bit key is embedded without reduction, exactly as Algorithm 4. -/
def polyKey (k : Word 126) : F127 := AdjoinRoot.mk modulus127 (pack 126 k)

theorem polyKey_injective : Function.Injective polyKey := by
  intro k k' h
  apply pack_injective 126
  apply mk127_injective_below _ _ h
  · exact (Polynomial.ofFn_natDegree_lt (by omega : 1 ≤ 126) k).trans (by omega)
  · exact (Polynomial.ofFn_natDegree_lt (by omega : 1 ≤ 126) k').trans (by omega)

end ProvenHashes.CLHash
