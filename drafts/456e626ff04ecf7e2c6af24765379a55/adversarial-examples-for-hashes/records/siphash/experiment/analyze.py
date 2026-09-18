#!/usr/bin/env python3
"""Summarise sipdiff outputs. usage: analyze.py <dir> [top]
For each (variant, W): number of differences, keys per difference, max collision count with 95% CI
(Clopper-Pearson), largest |z| of popcount-mean, low16/low32 excess, and the top rows.
95% one-sided upper bound on p when 0 of N collisions: p <= -ln(0.05)/N ~ 3/N."""
import sys, glob, math, re, collections, json
d = sys.argv[1]; top = int(sys.argv[2]) if len(sys.argv) > 2 else 8
def cp_upper(k, n, a=0.05):
    # Clopper-Pearson upper bound via bisection on the binomial cdf (regularised incomplete beta not in stdlib)
    from math import lgamma, exp, log
    def cdf(p):
        if p <= 0: return 1.0
        if p >= 1: return 0.0
        s = 0.0
        for i in range(k+1):
            s += exp(lgamma(n+1)-lgamma(i+1)-lgamma(n-i+1) + i*log(p) + (n-i)*log(1-p))
        return s
    lo, hi = 0.0, 1.0
    for _ in range(200):
        mid = (lo+hi)/2
        if cdf(mid) > a: lo = mid
        else: hi = mid
    return hi
def cp_lower(k, n, a=0.05):
    if k == 0: return 0.0
    return 1 - cp_upper(n-k, n, a)
groups = collections.defaultdict(list); meta = {}
for f in sorted(glob.glob(f"{d}/*.out")):
    m = re.search(r"v(\d+)_W(\d)(?:_(R|F0|FF))?", f); key = (m.group(1), m.group(2), m.group(3) or "")
    for line in open(f):
        if line.startswith("# variant"):
            meta[key] = dict(kv.split("=") for kv in line[2:].split()); continue
        if line.startswith("#"): continue
        p = line.split(); groups[key].append(dict(d0=p[0], d1=p[1], name=p[2], coll=int(p[3]), low32=int(p[4]), low16=int(p[5]), mean=float(p[6]), z=float(p[7]), hist=[int(x) for x in p[8:]]))
summary = {}
for key in sorted(groups):
    rows = groups[key]; N = int(meta[key]["N"]); nd = len(rows)
    tot = sum(r["coll"] for r in rows); mx = max(rows, key=lambda r: r["coll"]); mz = max(rows, key=lambda r: abs(r["z"]))
    exp16 = N / 65536; sd16 = math.sqrt(exp16); m16 = max(rows, key=lambda r: r["low16"])
    exp32 = N / 2**32; m32 = max(rows, key=lambda r: r["low32"])
    print(f"=== SipHash-{key[0][0]}-{key[0][1]}  W={key[1]} ({8*int(key[1])}-byte msgs) {key[2]}  {nd} diffs x N=2^{int(math.log2(N))} keys, mode={meta[key]['mode']}")
    print(f"  full 64-bit collisions: total {tot} over {nd*N:.3g} pair-evaluations; max per diff {mx['coll']} ({mx['name']})")
    up = cp_upper(mx['coll'], N); lo = cp_lower(mx['coll'], N)
    print(f"    best diff rate {mx['coll']/N:.3g}  95% CI [{lo:.3g}, {up:.3g}]  -> bits >= {-math.log2(up):.2f} (upper CI), floor for 0 hits: 2^{math.log2(3/N):.1f}")
    print(f"  popcount-mean distinguisher: max |z| = {abs(mz['z']):.2f} ({mz['name']}, mean {mz['mean']:.4f}); Bonferroni 5% threshold over {nd} diffs: {math.sqrt(2*math.log(nd/0.05*2)):.2f}")
    print(f"  low-16 collisions: expected {exp16:.1f}+-{sd16:.1f}; max {m16['low16']} ({m16['name']}, z={(m16['low16']-exp16)/sd16:.2f})")
    print(f"  low-32 collisions: expected {exp32:.3f}; max {m32['low32']} ({m32['name']})")
    print("  top by |z|:"); 
    for r in sorted(rows, key=lambda r: -abs(r['z']))[:top]: print(f"    {r['d0']} {r['d1']} {r['name']:14s} coll={r['coll']} low16={r['low16']} mean={r['mean']:.4f} z={r['z']:+.2f} hist0..8={r['hist']}")
    summary[f"{key[0]}_W{key[1]}{'_'+key[2] if key[2] else ''}"] = dict(N=N, ndiffs=nd, total_coll=tot, max_coll=mx['coll'], max_coll_diff=[mx['d0'], mx['d1'], mx['name']], rate_ci=[lo, up], max_abs_z=abs(mz['z']), max_z_diff=[mz['d0'], mz['d1'], mz['name']], low16_max=m16['low16'], low16_expected=exp16)
json.dump(summary, open(f"{d}/summary.json", "w"), indent=1)
