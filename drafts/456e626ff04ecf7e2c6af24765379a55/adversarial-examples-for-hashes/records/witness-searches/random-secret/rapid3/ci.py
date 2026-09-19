#!/usr/bin/env python3
"""Exact (Garwood) central 95% Poisson interval for a count c over T trials, reported as
log2 rate and as bits = log2(L) - log2(rate).  Usage: ci.py COUNT LOG2TRIALS L  (trials may be a plain integer too)."""
import math, sys
def pois_cdf(k, mu):
    """P[X <= k] for X ~ Poisson(mu), summed in log space (stable for large mu)."""
    if mu == 0: return 1.0
    logs = [i * math.log(mu) - mu - math.lgamma(i + 1) for i in range(0, k + 1)]
    m = max(logs)
    return min(1.0, math.exp(m) * sum(math.exp(l - m) for l in logs))
def bisect(f, lo, hi, it=200):
    for _ in range(it):
        mid = (lo + hi) / 2
        if f(mid) > 0: lo = mid
        else: hi = mid
    return (lo + hi) / 2
def garwood(c, alpha=0.05):
    # lower: P[X >= c | mu] = alpha/2 ; upper: P[X <= c | mu] = alpha/2
    lo = 0.0 if c == 0 else bisect(lambda m: alpha / 2 - (1 - pois_cdf(c - 1, m)), 0, 10 * c + 50)
    hi = bisect(lambda m: pois_cdf(c, m) - alpha / 2, 0, 10 * c + 50)
    return lo, hi
if __name__ == "__main__":
    c = int(sys.argv[1]); t = sys.argv[2]; L = float(sys.argv[3])
    T = 2 ** float(t) if float(t) < 100 else float(t)
    lo, hi = garwood(c)
    if c:
        print(f"count {c} / 2^{math.log2(T):.0f}: rate 2^{math.log2(c/T):.3f}  95% CI [2^{math.log2(lo/T):.3f}, 2^{math.log2(hi/T):.3f}]")
        print(f"bits (L={L:g}) = {math.log2(L)-math.log2(c/T):.3f}  95% CI [{math.log2(L)-math.log2(hi/T):.3f}, {math.log2(L)-math.log2(lo/T):.3f}]")
    else:
        print(f"count 0 / 2^{math.log2(T):.0f}: rate <= 2^{math.log2(hi/T):.3f} (95% upper); bits (L={L:g}) >= {math.log2(L)-math.log2(hi/T):.3f}")
