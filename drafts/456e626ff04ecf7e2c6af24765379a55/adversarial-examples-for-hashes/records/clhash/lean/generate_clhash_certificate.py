"""Generate a GF(2)[X] certificate for x^127+x+1; run on the Xeon only.

The generated theorem checks every arithmetic identity with kernel-checked
ring proofs. This program and its assertions are not trusted by Lean.
"""
from pathlib import Path

P = (1 << 127) | 3

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
    while a and a.bit_length() >= b.bit_length():
        d = a.bit_length() - b.bit_length()
        q ^= 1 << d
        a ^= b << d
    return q, a

def poly(a):
    return 'sparse [' + ', '.join(str(i) for i in range(a.bit_length()) if a >> i & 1) + ']'

res, qs = [2], []
for _ in range(127):
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

g, a, b = xgcd(P, 6)
assert g == 1
out = '''import ProvenHashes.CLHashModulus
import ProvenHashes.ModulusResidues

noncomputable section
namespace ProvenHashes.CLHash
open Polynomial ChainHash
set_option maxHeartbeats 0
set_option maxRecDepth 10000
set_option linter.unusedSimpArgs false
set_option linter.unusedTactic false
set_option linter.unreachableTactic false

'''
for i, r in enumerate(res):
    out += f'def r{i} : BitsPolynomial := {poly(r)}\n'
out += '''
/-- A checked Frobenius chain and a checked Bezout identity. -/
theorem modulus127_certificate :
    modulus127 ∣ X ^ (2 ^ 127) - X ∧ IsCoprime modulus127 (X ^ 2 - X) := by
  have step (i : ℕ) (r s q : BitsPolynomial)
      (hs : r ^ 2 = s + modulus127 * q)
      (hr : AdjoinRoot.root modulus127 ^ (2 ^ i) = AdjoinRoot.mk modulus127 r) :
      AdjoinRoot.root modulus127 ^ (2 ^ (i + 1)) = AdjoinRoot.mk modulus127 s := by
    rw [pow_succ, pow_mul, hr, ← map_pow, hs, map_add, map_mul,
      AdjoinRoot.mk_self, zero_mul, add_zero]
  have h0 : AdjoinRoot.root modulus127 ^ (2 ^ 0) = AdjoinRoot.mk modulus127 r0 := by
    simp [r0, sparse, AdjoinRoot.mk_X]
'''
normalizer = '''    simp only [sparse, List.map_cons, List.map_nil, List.sum_cons,
      List.sum_nil, pow_zero, pow_one]
    try simp only [CharTwo.add_sq, ← pow_mul]
    ring_nf
    all_goals simp only [''' + ', '.join(f'poly_num_{i}' for i in range(2, 65)) + ''',
      mul_zero, mul_one, zero_add, add_zero]
'''
for i, q in enumerate(qs):
    out += f'  have s{i} : r{i} ^ 2 = r{i+1} + modulus127 * ({poly(q)}) := by\n'
    out += f'    unfold r{i} r{i+1} modulus127\n' + normalizer
    out += f'  have h{i+1} := step {i} _ _ _ s{i} h{i}\n'
out += '''  constructor
  · apply AdjoinRoot.mk_eq_zero.mp
    rw [map_sub, map_pow, AdjoinRoot.mk_X, h127]
    simp [r127, sparse, AdjoinRoot.mk_X]
'''
out += f'  · refine ⟨{poly(a)}, {poly(b)}, ?_⟩\n'
out += '    rw [CharTwo.sub_eq_add]\n    unfold modulus127\n' + normalizer.replace('    ', '    ')
out += '\nend ProvenHashes.CLHash\n'
Path('ProvenHashes/CLHashCertificate.lean').write_text(out)
