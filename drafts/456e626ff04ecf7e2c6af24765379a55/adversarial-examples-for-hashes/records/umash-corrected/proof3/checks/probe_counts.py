#!/usr/bin/env python3
"""Exploratory exact constants, run on the Xeon."""
from functools import lru_cache
from fractions import Fraction
from collections import Counter
import json

W = 64
Q = 1 << W
P = Q // 8 - 1

@lru_cache(None)
def supports(n, w):
    if abs(n) >= 1 << w:
        return frozenset()
    if not w:
        return frozenset([0]) if not n else frozenset()
    if not n & 1:
        return frozenset(2*x for x in supports(n//2, w-1))
    return frozenset(2*x+1 for z in ((n-1)//2, (n+1)//2)
                     for x in supports(z, w-1))

def patterns(d):
    return sorted({(d+j*P)//2 for j in range(-8, 9)
                   if d+j*P >= 0 and not (d+j*P) & 1
                   and ((d+j*P)//2) & d == (d+j*P)//2})

def main():
    ds = sorted(set().union(*(supports(j*P, W) for j in range(9))))
    rows = []
    for s in range(1, 65):
        hist = Counter(d % (1 << s) for d in ds)
        rows.append(dict(s=s, signatures=len(hist), maximum=max(hist.values()),
                         low_bound=(1 << s)*max(hist.values())))
    print(json.dumps({'prefix_rows':rows}, indent=2))
    sums = [Fraction() for _ in range(4)]
    for d in ds:
        pats = patterns(d)
        h = d.bit_count()
        f = Fraction(1, Q) + sum(Fraction(1 << j, Q << (d & ((1 << j)-1)).bit_count())
                                  for j in range(W))
        weight = min(1, len(pats)*f)
        kh = min(1 << (1+h-(d >> 63)), 276*(1 << (64-h)),
                 16*((1 << ((h+1)//2))+1))
        sums[0] += min(2*len(pats), kh*weight)
        sums[1] += min(2*len(pats), kh*weight, Fraction(1 << 8, 1))
        # One-word sparse/dense bound from earlier ledger is separate.
        sums[2] += min(2*len(pats), (1 << (1+h-(d >> 63)))*weight)
        sums[3] += len(pats)*weight
    print(json.dumps({'enh_sums':[[x.numerator,x.denominator,
                                 -(-x.numerator//x.denominator)] for x in sums]}, indent=2))
    groups = []
    for r in range(4, 64):
        mask = (1 << (r-1))-1
        keys = {(d & mask, t & (mask >> 1), (d-2*t) % (1 << r))
                for d in ds for t in patterns(d)}
        groups.append(dict(r=r, function_count=len(keys), bound=2*len(keys)))
    print(json.dumps({'lift_function_groups':groups}, indent=2))
    small = []
    for s in range(1, 4):
        m = 1 << s
        hist = Counter(d % m for d in ds)
        maximum = (0, None)
        for delta in range(m):
            for epsilon in range(m):
                weighted = sum(hist[(a*b % m) ^ (((a+delta)*(b+epsilon)) % m)]
                               for a in range(m) for b in range(m))
                if weighted > maximum[0]:
                    maximum = (weighted, [delta, epsilon])
        small.append(dict(s=s, numerator=maximum[0], denominator=m,
                          witness=maximum[1]))
    print(json.dumps({'small_convolution':small}, indent=2))

if __name__ == '__main__':
    main()
