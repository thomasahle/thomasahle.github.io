"""Tightness of the PolyXOR128 bound at L = 1 word (NEW, this audit).

Pair: m = [0x00], m' = [0x01] (one byte each, equal length).  Only data word 0
differs, by delta = 1.  With x_i = key word i of block 0 (data words 1..15 are
zero padding), the even compression sees
    d(ha) = clmul(1, x12) = x12,   d(he) = clmul(1, x12^x14^x4^x6) =: s,
and hb, hc, hd and the odd half are unchanged, so d(h0, h1) = (x12, s).
The chaining step gives d(acc) = (z+u)*s + x12 and the output difference is
d(acc) * (len + y) with len = 1.  The pair collides exactly on
    A: y = 1                       (Pr 2^-128),
    B: x12 = 0 and s = 0           (Pr 2^-128),
    C: s != 0 and z+u = x12 / s    (Pr (1 - 2^-64) 2^-128),
with B, C disjoint and A independent of B, C, so
    Pr[collision] = 2^-128 (3 - 2^-64 - 2^-127 + 2^-192).
This script builds one key for each event (all other key bytes random), writes
them for the Rust witness (which calls the shipped crate's public API), and
cross-checks each with the Lean-derived model.
"""
import random, sys
from fractions import Fraction
from math import log2
from model import key_from_entropy, polyxor128, fmul, pinv, pmod

rng = random.Random(20260928)
m0, m1 = b"\x00", b"\x01"

def put_u128(e, off, v): e[off:off + 16] = v.to_bytes(16, "little")
def put_u64(e, word, v): e[64 + 8 * word:64 + 8 * word + 8] = v.to_bytes(8, "little")
def get_u64(e, word): return int.from_bytes(e[64 + 8 * word:64 + 8 * word + 8], "little")

def finv(a):
    """Inverse in the Montgomery field (identity x^128): x^256 * a^-1 mod P."""
    from model import pmul
    return pmod(pmul(pinv(a), pmod(1 << 256)))

keys = {}
e = bytearray(rng.randbytes(4160)); put_u128(e, 32, 1); keys["A_y_eq_len"] = bytes(e)
e = bytearray(rng.randbytes(4160)); put_u64(e, 12, 0); put_u64(e, 14, get_u64(e, 4) ^ get_u64(e, 6)); keys["B_x12_zero_s_zero"] = bytes(e)
e = bytearray(rng.randbytes(4160))
x12 = get_u64(e, 12); s = x12 ^ get_u64(e, 14) ^ get_u64(e, 4) ^ get_u64(e, 6)
t = fmul(x12, finv(s))                       # z + u = x12 / s in the Montgomery field
assert fmul(t, s) == x12
u = int.from_bytes(e[16:32], "little"); put_u128(e, 0, t ^ u); keys["C_zu_eq_x12_over_s"] = bytes(e)
keys["control_random"] = bytes(rng.randbytes(4160))

with open(sys.argv[1] if len(sys.argv) > 1 else "tightness_keys.txt", "w") as f:
    for name, ent in keys.items():
        k, _ = key_from_entropy(ent)
        a, b = polyxor128(k, m0), polyxor128(k, m1)
        print(f"{name:22s} model: H(00)={a:032x} H(01)={b:032x} collide={a == b}")
        f.write(f"{name} {ent.hex()}\n")

p = Fraction(1, 2**128) * (3 - Fraction(1, 2**64) - Fraction(1, 2**127) + Fraction(1, 2**192))
bound = Fraction(8, 4096) + 3
print("exact Pr[collision] for this pair = 2^-128 * (3 - 2^-64 - 2^-127 + 2^-192)")
print(f"pair score  log2(1/eps) = {128 - log2(3 - 2**-64):.6f}  (L = 1 word)")
print(f"proven lower bound 128 - log2(3 + 8/4096) = {128 - log2(float(bound)):.6f}")
print(f"gap = {log2(float(bound)) - log2(3 - 2**-64):.6f} bits")
