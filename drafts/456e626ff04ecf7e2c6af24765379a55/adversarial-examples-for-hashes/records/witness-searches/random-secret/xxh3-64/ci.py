#!/usr/bin/env python3
"""Clopper-Pearson 95% intervals for hit counts and the resulting cap bits = log2(L/eps).

usage: ci.py HITS LOG2N L [HITS LOG2N L ...]
"""
import math
import sys
from scipy.stats import beta


def cp(k, n, a=0.05):
    lo = 0.0 if k == 0 else beta.ppf(a / 2, k, n - k + 1)
    hi = 1.0 if k == n else beta.ppf(1 - a / 2, k + 1, n - k)
    return lo, hi


def main(argv):
    args = argv[1:]
    for i in range(0, len(args), 3):
        k, log2n, L = int(args[i]), int(args[i + 1]), int(args[i + 2])
        n = 1 << log2n
        lo, hi = cp(k, n)
        if k:
            p = k / n
            print(f"hits={k}/2^{log2n} L={L}: rate=2^{math.log2(p):.4f} CI95=[2^{math.log2(lo):.4f}, 2^{math.log2(hi):.4f}]"
                  f"  cap={math.log2(L / p):.3f} bits CI95=[{math.log2(L / hi):.3f}, {math.log2(L / lo):.3f}]")
        else:
            print(f"hits=0/2^{log2n} L={L}: rate < 2^{math.log2(hi):.2f} (95%), cap > {math.log2(L / hi):.2f} bits")


if __name__ == "__main__":
    main(sys.argv)
