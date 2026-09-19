#!/usr/bin/env python3
"""Exact (Garwood) 95% Poisson CI for k events in n trials, printed as log2 rates.
Usage: poisson_ci.py k log2n [k log2n ...].  Uses the regularized lower incomplete gamma P(a,x)
(series/continued fraction, Numerical Recipes), since P[X<=k] = 1 - P(k+1, mu)."""
import sys, math
def gammainc_P(a, x):
    if x <= 0: return 0.0
    if x < a + 1:
        ap, s, d = a, 1.0 / a, 1.0 / a
        for _ in range(100000):
            ap += 1; d *= x / ap; s += d
            if abs(d) < abs(s) * 1e-15: break
        return s * math.exp(-x + a * math.log(x) - math.lgamma(a))
    b, c, d = x + 1 - a, 1e300, 1.0 / (x + 1 - a); h = d
    for i in range(1, 100000):
        an = -i * (i - a); b += 2; d = an * d + b; d = 1e-300 if abs(d) < 1e-300 else d
        c = b + an / c; c = 1e-300 if abs(c) < 1e-300 else c
        d = 1.0 / d; de = d * c; h *= de
        if abs(de - 1) < 1e-15: break
    return 1 - math.exp(-x + a * math.log(x) - math.lgamma(a)) * h
def pcdf(k, mu): return 1 - gammainc_P(k + 1, mu)          # P[X<=k]
def bisect(f, target, lo, hi):                             # f decreasing in mu
    for _ in range(200):
        mid = (lo + hi) / 2
        if f(mid) > target: lo = mid
        else: hi = mid
    return (lo + hi) / 2
def ci(k, alpha=0.05):
    hi = bisect(lambda mu: pcdf(k, mu), alpha / 2, 0, 10 * k + 50)
    lo = 0.0 if k == 0 else bisect(lambda mu: pcdf(k - 1, mu), 1 - alpha / 2, 0, 10 * k + 50)
    return lo, hi
if __name__ == "__main__":
    a = sys.argv[1:]
    for k, lg in zip(a[::2], a[1::2]):
        k = int(k); n = 2 ** float(lg); lo, hi = ci(k)
        print(f"k={k} n=2^{float(lg):.2f}: rate 2^{math.log2(k/n) if k else float('nan'):.3f}  exact 95% CI [2^{math.log2(lo/n) if lo>0 else float('-inf'):.3f}, 2^{math.log2(hi/n):.3f}]  (mu CI [{lo:.2f}, {hi:.2f}])")
