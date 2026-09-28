#!/usr/bin/env python3
"""Poly1305 (RFC 8439 section 2.5): the exact L = 1 collision probability of one short pair.

M' = d664915c5fd5 (6 bytes) and M = M' || 01 (7 bytes).  Both are single partial blocks, so each is
padded with one 0x01 byte: c(M') = m' + 2^48 and c(M) = m' + 2^48 + 2^56, a difference of exactly 2^56.
With h = c*r mod p (p = 2^130 - 5) reduced to [0, p) and tag = (h + s) mod 2^128, the tags agree iff
h(M) - h(M') = j * 2^128 as integers with |j| <= 3 (because 0 <= h < p < 4 * 2^128).  Modulo p this
forces 2^56 r = j 2^128, i.e. r = j * 2^72 mod p.  For j = -1, -2, -3 that residue is >= 2^124 and is
never a clamped r; for j = 0, 1, 2, 3 it is 0, 2^72, 2^73, 3 * 2^72, all clamp-valid.  So at most 4 of
the 2^106 clamped r values collide, whatever s is; this program checks that all 4 do.
epsilon(L = 1) = 4 / 2^106 = 2^-104, so this pair caps the normalized score at exactly 104 bits.
For L >= 2 the same counting (7 values of j per root, degree ceil(L/2)) gives 7*ceil(L/2)/2^106,
whose minimum of log2(L/eps) over L >= 2 is 103.78 at L = 3.  Hence the true score is in [103.78, 104].

Run: python3 poly1305_witness.py      (standard library only; uses 'cryptography' for a cross-check if present)
"""
import random
import sys
from math import ceil, log2

P = 2**130 - 5
CLAMP = 0x0FFFFFFC0FFFFFFC0FFFFFFC0FFFFFFF


def poly1305(key: bytes, msg: bytes) -> bytes:
    r = int.from_bytes(key[:16], "little") & CLAMP
    s = int.from_bytes(key[16:32], "little")
    acc = 0
    for i in range(0, len(msg), 16):
        acc = (acc + int.from_bytes(msg[i:i + 16] + b"\x01", "little")) * r % P
    return ((acc + s) % 2**128).to_bytes(16, "little")


# RFC 8439 section 2.5.2 test vector
k = bytes.fromhex("85d6be7857556d337f4452fe42d506a80103808afb0db2fd4abff6af4149f51b")
tag = poly1305(k, b"Cryptographic Forum Research Group")
ok = tag.hex() == "a8061dc1305136c6c22b8baf0c0127a9"
print("RFC 8439 2.5.2 vector:", tag.hex(), "OK" if ok else "MISMATCH")
if not ok:
    sys.exit(1)

rng = random.Random(20260928)
try:
    from cryptography.hazmat.primitives.poly1305 import Poly1305
    for _ in range(2000):
        kk = rng.randbytes(32)
        mm = rng.randbytes(rng.randrange(0, 80))
        assert Poly1305.generate_tag(kk, mm) == poly1305(kk, mm)
    print("cross-check against the 'cryptography' package: 2000/2000 equal")
except ImportError:
    print("'cryptography' package not installed: RFC vector only")

M_short = bytes.fromhex("d664915c5fd5")
M_long = M_short + b"\x01"
print("pair: M' =", M_short.hex(), "(6 bytes), M =", M_long.hex(), "(7 bytes); L = 1 word")

cands = {j: (j * 2**72) % P for j in range(-3, 4)}
valid = {j: r for j, r in cands.items() if r & CLAMP == r}
print("r = j*2^72 mod p, clamp-valid for j in", sorted(valid))
n_coll = 0
for j, r in sorted(valid.items()):
    hits = 0
    for _ in range(1000):
        key = r.to_bytes(16, "little") + rng.randbytes(16)   # same key (r, s) for both messages
        hits += poly1305(key, M_long) == poly1305(key, M_short)
    print(f"  j = {j}: r = {r:#x}: {hits}/1000 random s collide")
    n_coll += hits == 1000
key = (2**72).to_bytes(16, "little") + bytes(16)
print("explicit key r = 2^72, s = 0:", key.hex(), "->", poly1305(key, M_long).hex(), poly1305(key, M_short).hex())

ctrl = sum(poly1305(kk, M_long) == poly1305(kk, M_short) for kk in (rng.randbytes(32) for _ in range(200000)))
print(f"uniform random keys: {ctrl}/200000 collide")

print(f"epsilon(L=1) = {n_coll}/2^106 = 2^{log2(n_coll) - 106:.0f}; score at L=1 = {106 - log2(n_coll):.4f}")
env = min((106 + log2(L / (7 * ceil(L / 2))), L) for L in range(2, 100000))
print("envelope 7*ceil(L/2)/2^106 for L >= 2: minimum %.4f at L = %d" % env)
print("Bernstein's 8*ceil(bytes/16)/2^106 at L = 1: %.4f" % (106 - 3))
