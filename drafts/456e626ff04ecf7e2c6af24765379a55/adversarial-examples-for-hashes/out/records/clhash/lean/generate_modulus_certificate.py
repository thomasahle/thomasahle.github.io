"""Generate explicit GF(2)[X] identities; Lean checks every identity with ring.

This script is a certificate producer, not part of the trusted proof checker.
Run only on the Xeon. No message/key enumeration is involved.
"""
from pathlib import Path

P = (1 << 64) | 27

def mul(a, b):
    c = 0
    while b:
        if b & 1:
            c ^= a
        a <<= 1
        b >>= 1
    return c

def divmod_poly(a, b):
    q = 0
    while a.bit_length() >= b.bit_length() and a:
        d = a.bit_length() - b.bit_length()
        q ^= 1 << d
        a ^= b << d
    return q, a

def poly(a):
    return "sparse [" + ", ".join(str(i) for i in range(a.bit_length()) if (a >> i) & 1) + "]"

res = [2]
qs = []
for _ in range(64):
    q, r = divmod_poly(mul(res[-1], res[-1]), P)
    qs.append(q)
    res.append(r)
assert res[-1] == 2

def xgcd(a, b):
    a0, a1, b0, b1 = 1, 0, 0, 1
    while b:
        q, r = divmod_poly(a, b)
        a, b = b, r
        a0, a1 = a1, a0 ^ mul(q, a1)
        b0, b1 = b1, b0 ^ mul(q, b1)
    return a, a0, b0

g, a, b = xgcd(P, res[32] ^ 2)
assert g == 1

header = '''import ProvenHashes.Modulus

noncomputable section
namespace ProvenHashes.ChainHash
open Polynomial
set_option maxHeartbeats 4000000
set_option linter.unusedSimpArgs false
set_option linter.unusedTactic false
set_option linter.unreachableTactic false

def sparse (l : List ℕ) : BitsPolynomial := (l.map fun i => X ^ i).sum

'''
normalizer = """  simp only [sparse, List.map_cons, List.map_nil, List.sum_cons, List.sum_nil, pow_zero, pow_one]
  try simp only [CharTwo.add_sq, ← pow_mul]
  ring_nf
  all_goals simp only [""" + ", ".join(f"poly_num_{n}" for n in range(2, 65)) + """, mul_zero,
    mul_one, zero_add, add_zero]
"""
for n in range(2, 65):
    header += f"theorem poly_num_{n} : ({n} : BitsPolynomial) = {n % 2} := by\n"
    header += f"  simpa using CharP.natCast_eq_natCast_mod BitsPolynomial 2 {n}\n"


base = header
for i, r in enumerate(res):
    base += f'def residue_{i} : BitsPolynomial := {poly(r)}\n'
base += '\nend ProvenHashes.ChainHash\n'
Path('ProvenHashes/ModulusResidues.lean').write_text(base)
for chunk in range(8):
    out = "import ProvenHashes.ModulusResidues\n\nnoncomputable section\nnamespace ProvenHashes.ChainHash\nopen Polynomial\nset_option maxHeartbeats 4000000\nset_option linter.unusedSimpArgs false\nset_option linter.unusedTactic false\nset_option linter.unreachableTactic false\n"
    for i in range(8 * chunk, 8 * chunk + 8):
        out += f'\ntheorem residue_step_{i} : residue_{i} ^ 2 = residue_{i+1} + modulus * ({poly(qs[i])}) := by\n'
        out += f'  unfold residue_{i} residue_{i+1} modulus\n' + normalizer
    out += '\nend ProvenHashes.ChainHash\n'
    Path(f'ProvenHashes/ModulusSteps{chunk}.lean').write_text(out)
out = "".join(f'import ProvenHashes.ModulusSteps{i}\n' for i in range(8))
out += "\nnoncomputable section\nnamespace ProvenHashes.ChainHash\nopen Polynomial\nset_option maxHeartbeats 8000000\nset_option linter.unusedSimpArgs false\nset_option linter.unusedTactic false\nset_option linter.unreachableTactic false\n"
out += f'\ntheorem residue_bezout : modulus * ({poly(a)}) + (residue_32 - X) * ({poly(b)}) = 1 := by\n'
out += '  unfold modulus residue_32\n  rw [CharTwo.sub_eq_add]\n' + normalizer
out += '\nend ProvenHashes.ChainHash\n'
Path('ProvenHashes/ModulusCertificate.lean').write_text(out)

out = '''import ProvenHashes.ModulusCertificate

noncomputable section
namespace ProvenHashes.ChainHash
open Polynomial

 theorem residue_power_step (i : ℕ) (r s q : BitsPolynomial)
    (hs : r ^ 2 = s + modulus * q)
    (hr : AdjoinRoot.root modulus ^ (2 ^ i) = AdjoinRoot.mk modulus r) :
    AdjoinRoot.root modulus ^ (2 ^ (i + 1)) = AdjoinRoot.mk modulus s := by
  have hp : (2 ^ (i + 1) : ℕ) = 2 ^ i * 2 := pow_succ _ _
  rw [hp, pow_mul, hr, ← map_pow, hs, map_add, map_mul,
    AdjoinRoot.mk_self, zero_mul, add_zero]

theorem modulus_frobenius_certificates :
    modulus ∣ X ^ (2 ^ 64) - X ∧ modulus ∣ X ^ (2 ^ 32) - residue_32 := by
  have h0 : AdjoinRoot.root modulus ^ (2 ^ 0) = AdjoinRoot.mk modulus residue_0 := by
    simp [residue_0, sparse, AdjoinRoot.mk_X]
'''
for i in range(64):
    out += f'  have h{i+1} := residue_power_step {i} _ _ _ residue_step_{i} h{i}\n'
out += '''  constructor
  · apply AdjoinRoot.mk_eq_zero.mp
    rw [map_sub, map_pow, AdjoinRoot.mk_X, h64]
    simp [residue_64, sparse, AdjoinRoot.mk_X]
  · apply AdjoinRoot.mk_eq_zero.mp
    rw [map_sub, map_pow, AdjoinRoot.mk_X, h32, sub_self]

theorem modulus_irreducible : Irreducible modulus := by
  obtain ⟨h64, h32⟩ := modulus_frobenius_certificates
  apply binary_rabin64 modulus modulus_monic modulus_degree h64
  obtain ⟨q, hq⟩ := h32
'''
out += f'  let a : BitsPolynomial := {poly(a)}\n'
out += f'  let b : BitsPolynomial := {poly(b)}\n'
out += '''  refine ⟨a - b * q, b, ?_⟩
  have hx : X ^ (2 ^ 32) = modulus * q + residue_32 := sub_eq_iff_eq_add.mp hq
  rw [hx]
  calc
    _ = modulus * a + (residue_32 - X) * b := by ring
    _ = 1 := residue_bezout

end ProvenHashes.ChainHash
'''
Path('ProvenHashes/ModulusIrreducible.lean').write_text(out)
