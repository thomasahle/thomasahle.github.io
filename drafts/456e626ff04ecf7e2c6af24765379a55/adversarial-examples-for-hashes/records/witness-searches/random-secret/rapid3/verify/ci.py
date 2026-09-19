#!/usr/bin/env python3
"""Exact central 95% Poisson (Garwood) interval on a count c over T = 2^t trials.
Usage: ci.py COUNT LOG2TRIALS L      -> rate 2^x [lo, hi] and bits = log2(L) - log2(rate) [lo, hi]
The interval is found by bisection on the Poisson CDF (no scipy needed)."""
import math, sys
def pcdf(k, mu):
    if mu <= 0: return 1.0
    lp = -mu; s = math.exp(lp)
    for i in range(1, k + 1):
        lp += math.log(mu) - math.log(i); s += math.exp(lp)
    return min(1.0, s)
def solve(f, lo, hi):
    for _ in range(200):
        mid = (lo + hi) / 2
        if f(mid) > 0: lo = mid
        else: hi = mid
    return (lo + hi) / 2
def garwood(c, alpha=0.05):
    lo = 0.0 if c == 0 else solve(lambda m: alpha / 2 - (1 - pcdf(c - 1, m)), 0.0, 10.0 * c + 50)
    hi = solve(lambda m: pcdf(c, m) - alpha / 2, 0.0, 10.0 * c + 50)
    return lo, hi
if __name__ == "__main__":
    c = int(sys.argv[1]); t = float(sys.argv[2]); L = float(sys.argv[3]); T = 2.0 ** t
    lo, hi = garwood(c)
    if c:
        print(f"count {c} / 2^{t:g}: rate 2^{math.log2(c/T):.3f} [2^{math.log2(lo/T):.3f}, 2^{math.log2(hi/T):.3f}]; "
              f"cap (L={L:g}) {math.log2(L)-math.log2(c/T):.2f} bits [{math.log2(L)-math.log2(hi/T):.2f}, {math.log2(L)-math.log2(lo/T):.2f}]")
    else:
        print(f"count 0 / 2^{t:g}: rate < 2^{math.log2(hi/T):.3f} (95%); cap (L={L:g}) > {math.log2(L)-math.log2(hi/T):.2f} bits")
