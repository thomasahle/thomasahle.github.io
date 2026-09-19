#!/usr/bin/env python3
"""Pool wyrs result lines (hits=k over N=2^lg) and print rate + exact 95% Poisson (Garwood) CI in bits.
usage: pool.py L 'label' file:tag [file:tag ...]   (file:tag[:mode]; tag is a prefix, or exact when written =tag)"""
import math, re, sys
from scipy.stats import chi2
def garwood(k):
    lo = 0.0 if k == 0 else chi2.ppf(0.025, 2 * k) / 2
    return lo, chi2.ppf(0.975, 2 * k + 2) / 2
L = int(sys.argv[1]); label = sys.argv[2]; K = 0; N = 0; runs = []
for spec in sys.argv[3:]:
    parts = spec.split(":"); f, tag = parts[0], parts[1]; mode = parts[2] if len(parts) > 2 else None
    for line in open(f):
        m = re.match(r"(\S+) len=\S+ L=\d+ mode=(\d) N=2\^(\d+) hits=(\d+)", line)
        if not m or (mode and m.group(2) != mode): continue
        if (m.group(1) == tag[1:]) if tag.startswith("=") else m.group(1).startswith(tag):
            k, lg = int(m.group(4)), int(m.group(3)); K += k; N += 2 ** lg; runs.append(f"{m.group(1)}:{k}/2^{lg}")
lo, hi = garwood(K)
print(f"{label}: pooled {K} / 2^{math.log2(N):.2f} = 2^{math.log2(K/N) if K else float('-inf'):.3f}", end=" ")
if K: print(f"[2^{math.log2(lo/N):.3f}, 2^{math.log2(hi/N):.3f}]  bits(L={L}) = {math.log2(L*N/K):.3f} [{math.log2(L*N/hi):.3f}, {math.log2(L*N/lo):.3f}]")
else: print(f"rate <= 2^{math.log2(hi/N):.3f} (97.5%)  bits(L={L}) >= {math.log2(L*N/hi):.3f}")
print("   runs:", ", ".join(runs))
