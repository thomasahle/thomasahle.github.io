import ProvenHashes.BinaryRabin

noncomputable section
namespace ProvenHashes.CLHash
open Polynomial ChainHash

/-- Algorithm 4's degree-127 binary modulus. -/
def modulus127 : BitsPolynomial := X ^ 127 + (X + 1)

theorem modulus127_spec : modulus127.Monic ∧ modulus127.natDegree = 127 := by
  have ht : (X + 1 : BitsPolynomial).degree < 127 := by
    apply lt_of_le_of_lt (degree_add_le _ _)
    norm_num
  constructor
  · exact monic_X_pow_add ht
  · unfold modulus127
    rw [natDegree_eq_of_degree_eq (degree_add_eq_left_of_degree_lt (by simpa using ht))]
    simp

end ProvenHashes.CLHash
