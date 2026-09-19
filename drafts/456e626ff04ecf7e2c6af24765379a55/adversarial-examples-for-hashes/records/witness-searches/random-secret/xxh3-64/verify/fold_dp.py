#!/usr/bin/env python3
"""Bit-serial carry DP for the XXH3 fold differential under the independence heuristic.

fold(x,y) = lo(xy) ^ hi(xy).  Heuristic: treat lo, hi and the inputs as independent uniform
64-bit words (they are not exactly; the true rates are sampled by xxh3_verify).  Exact algebra:
  (M,0): x -> ~x:        x'y = 2^64 y - y - xy          => lo' = -(lo+y) mod 2^64,
                                                            hi' = y - hi - cs - [ (lo+y) mod 2^64 != 0 ]
  (M,M): x,y -> ~x,~y:   x'y' = xy + (x+y+1) - 2^64 (x+y+2) (mod 2^128)
                                                         => lo' = lo + t + 1, hi' = hi + cu + ct - t - 2, t = x+y
where cs, cu, ct are the carries out of the 64-bit additions.  Collision iff lo'^hi' == lo^hi bitwise.
"""
from fractions import Fraction
from collections import defaultdict
import math

def m0():
    total = Fraction(0)
    for cs_g in (0, 1):
        for t_g in (0, 1):
            st = defaultdict(Fraction); st[(0, cs_g + t_g, 0)] = Fraction(1)   # (carry of s, borrow of hi', z)
            for _ in range(64):
                nx = defaultdict(Fraction)
                for (c, b, z), p in st.items():
                    for lo in (0, 1):
                        for hi in (0, 1):
                            for y in (0, 1):
                                s = lo ^ y ^ c; c2 = (lo + y + c) >> 1
                                lo2 = s ^ z; z2 = z | s
                                v = y - hi - b; hb = v & 1; b2 = (hb - v) // 2
                                if (lo2 ^ hb) == (lo ^ hi):
                                    nx[(c2, b2, z2)] += p / 8
                st = nx
            total += sum(p for (c, b, z), p in st.items() if c == cs_g and z == t_g)
    return total

def mm():
    total = Fraction(0)
    for g in (0, 1, 2):
        st = defaultdict(Fraction); st[(0, 1, g - 2)] = Fraction(1)   # (carry of t=x+y, carry of u=lo+t+1, carry of hi')
        for _ in range(64):
            nx = defaultdict(Fraction)
            for (a, c, k), p in st.items():
                for lo in (0, 1):
                    for hi in (0, 1):
                        for x in (0, 1):
                            for y in (0, 1):
                                t = x ^ y ^ a; a2 = (x + y + a) >> 1
                                u = lo ^ t ^ c; c2 = (lo + t + c) >> 1
                                v = hi - t + k; hb = v & 1; k2 = (v - hb) // 2
                                if (u ^ hb) == (lo ^ hi):
                                    nx[(a2, c2, k2)] += p / 16
            st = nx
        total += sum(p for (a, c, k), p in st.items() if a + c == g)
    return total

for name, f in (("(M,0)", m0), ("(M,M)", mm)):
    p = f(); print(f"{name}: heuristic P = 2^{math.log2(p):.4f}   ((3/4)^64 = 2^{64*math.log2(0.75):.4f})")
