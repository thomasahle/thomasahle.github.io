#!/usr/bin/env python3
"""Exact 95% Poisson (Garwood) interval for count c in N trials, via the chi-square/gamma relation
   lower = 0.5*chi2_{0.025}(2c), upper = 0.5*chi2_{0.975}(2c+2), computed by bisection on the Poisson CDF.
   usage: ci_check.py <count> <N> [L_words]"""
import sys, math
def pcdf(k, lam):
    t = math.exp(-lam); s = t
    for i in range(1, k + 1): t *= lam / i; s += t
    return s
def bisect(f, lo, hi, target, up):
    for _ in range(300):
        mid = (lo + hi) / 2
        if (f(mid) > target) == up: lo = mid
        else: hi = mid
    return (lo + hi) / 2
c = int(sys.argv[1]); N = float(sys.argv[2]); L = float(sys.argv[3]) if len(sys.argv) > 3 else 4.0
up = bisect(lambda l: pcdf(c, l), 0.0, 8 * c + 40, 0.025, True)
lo = 0.0 if c == 0 else bisect(lambda l: 1 - pcdf(c - 1, l), 0.0, 8 * c + 40, 0.025, False)
l2 = lambda x: math.log2(x) if x > 0 else float("-inf")
print(f"count={c} N=2^{math.log2(N):.3f} rate=2^{l2(c/N):.4f} 95%CI [2^{l2(lo/N):.4f}, 2^{l2(up/N):.4f}]  "
      f"cap bits (L={L:g}) = {math.log2(L)-l2(c/N):.3f} 95%CI [{math.log2(L)-l2(up/N):.3f}, {math.log2(L)-l2(lo/N):.3f}]")
if c == 0: print(f"zero count: rate < {up/N:.3e} = 2^{l2(up/N):.2f} at 97.5%; bits > {math.log2(L)-l2(up/N):.2f}")
