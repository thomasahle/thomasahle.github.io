#!/usr/bin/env python3
"""Pick phase-B candidates from phase-A outputs: per (variant,W) the union of
top-K by |z| (popcount-mean bias), every difference with >=1 full collision, top-3 by low16, top-3 by low32,
plus bit63 (the paper's most-biased single-bit difference) and complement0 as fixed references."""
import sys, glob, re, collections, os
d, out, K = sys.argv[1], sys.argv[2], int(sys.argv[3]) if len(sys.argv) > 3 else 12
os.makedirs(out, exist_ok=True)
groups = collections.defaultdict(list)
for f in sorted(glob.glob(f"{d}/*.out")):
    m = re.search(r"v(\d+)_W(\d)", f); key = (m.group(1), m.group(2))
    for line in open(f):
        if line.startswith("#"): continue
        p = line.split(); groups[key].append((p[0], p[1], p[2], int(p[3]), int(p[4]), int(p[5]), float(p[7])))
for (v, W), rows in sorted(groups.items()):
    pick = {}
    for r in sorted(rows, key=lambda r: -abs(r[6]))[:K]: pick[(r[0], r[1])] = r
    for r in rows:
        if r[3] > 0: pick[(r[0], r[1])] = r
    for r in sorted(rows, key=lambda r: -r[5])[:3]: pick[(r[0], r[1])] = r
    for r in sorted(rows, key=lambda r: -r[4])[:3]: pick[(r[0], r[1])] = r
    for r in rows:
        if r[2] in ("bit63", "complement0", "bit0"): pick[(r[0], r[1])] = r
    with open(f"{out}/v{v}_W{W}.txt", "w") as fh:
        for r in pick.values(): fh.write(f"{r[0]} {r[1]} {r[2]}\n")
    print(v, W, len(pick), "candidates")
