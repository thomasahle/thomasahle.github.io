#!/usr/bin/env python3
"""CityHash64 / FarmHash64-NA 16-byte multicollision solver (unseeded Hash64 == H for every member, hence every
seed of Hash64WithSeed(s) collides).  len 9..16 path: a = w0 + k2, b = w1, mul = k2 + 32,
c = ror(b,37)*mul + a, d = (ror(a,25) + b)*mul, out = HashLen16(c, d, mul).
HashLen16 is inverted in closed form: with Q = mul^-1 * xs47(mul^-1 * H), every z gives the preimage
v = Q ^ z, u = v ^ (mul^-1 * xs47(z)) (xs47(x) = x ^ x>>47 is an involution).  For each random z, z3 solves the
remaining single 64-bit equation ror(D - ror(a,25), 37)*mul + a = u with D = v*mul^-1, then w1 = D - ror(a,25).
usage: city16_solve.py <H hex> <rng seed> <count> <outfile>"""
import sys, random, z3
M = (1 << 64) - 1; k2 = 0x9ae16a3b2f90404f; L = 16
mul = (k2 + 2 * L) & M; muli = pow(mul, -1, 1 << 64)
xs = lambda x: x ^ (x >> 47)
ror = lambda x, r: ((x >> r) | (x << (64 - r))) & M
def hashlen16(u, v):
    a = (u ^ v) * mul & M; a = xs(a); b = (v ^ a) * mul & M; b = xs(b); return b * mul & M
H = int(sys.argv[1], 16); random.seed(int(sys.argv[2])); cnt = int(sys.argv[3]); out = open(sys.argv[4], 'w')
Q = xs(H * muli & M) * muli & M
A = z3.BitVec('a', 64); n = 0
while n < cnt:
    z = random.getrandbits(64); v = Q ^ z; u = v ^ (xs(z) * muli & M); D = v * muli & M
    s = z3.SolverFor('QF_BV'); s.add(z3.RotateRight(D - z3.RotateRight(A, 25), 37) * mul + A == u)
    while s.check() == z3.sat:
        a = s.model()[A].as_long(); s.add(A != a)
        w0 = (a - k2) & M; w1 = (D - ror(a, 25)) & M
        c = (ror(w1, 37) * mul + a) & M; d = (ror(a, 25) + w1) * mul & M
        assert (c, d) == (u, v) and hashlen16(c, d) == H
        out.write((w0.to_bytes(8, 'little') + w1.to_bytes(8, 'little')).hex() + '\n'); n += 1
out.close()
