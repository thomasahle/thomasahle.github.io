#!/usr/bin/env python3
"""Exact (Garwood) central 95% Poisson interval for a count c in N trials; prints log2 rate CI and bits cap = log2(L) - log2(rate).
usage: ci.py <count> <N> [L_words]"""
import sys, math
def pois_cdf(k, lam):
    if lam == 0: return 1.0
    s = 0.0; t = math.exp(-lam)
    for i in range(0, k + 1):
        s += t; t *= lam / (i + 1)
    return s
def upper(c, a=0.025):   # smallest lam with P(X<=c) = a
    lo, hi = 0.0, max(10.0, 4 * c + 20)
    for _ in range(200):
        mid = (lo + hi) / 2
        if pois_cdf(c, mid) > a: lo = mid
        else: hi = mid
    return (lo + hi) / 2
def lower(c, a=0.025):   # largest lam with P(X>=c) = a  <=> P(X<=c-1) = 1-a
    if c == 0: return 0.0
    lo, hi = 0.0, 4 * c + 20
    for _ in range(200):
        mid = (lo + hi) / 2
        if pois_cdf(c - 1, mid) > 1 - a: lo = mid
        else: hi = mid
    return (lo + hi) / 2
c = int(sys.argv[1]); N = float(sys.argv[2]); L = float(sys.argv[3]) if len(sys.argv) > 3 else None
lo, hi = lower(c), upper(c)
r = c / N
def l2(x): return math.log2(x) if x > 0 else float('-inf')
print(f"count={c} N={N:.6g} (2^{math.log2(N):.2f}) rate={r:.4e} log2rate={l2(r):.4f} 95%CI log2 [{l2(lo/N):.4f}, {l2(hi/N):.4f}]")
if L:
    print(f"L={L:g} words: cap bits = {math.log2(L) - l2(r):.3f}  95% [{math.log2(L) - l2(hi/N):.3f}, {math.log2(L) - l2(lo/N):.3f}]" if c else f"L={L:g}: no hits; rate < {hi/N:.3e} = 2^{l2(hi/N):.2f} at 97.5%, so bits > {math.log2(L) - l2(hi/N):.2f}")
