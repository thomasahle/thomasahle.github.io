#!/usr/bin/env python3
"""Empirical re-ranking: run ./measure at 2^26 seeds on every DP candidate with predicted bits <= LIMIT.
Prints measured rate for `hash` (standard 64-bit) and the resulting bits = log2(L) - log2(rate)."""
import json, subprocess, math, sys, re
from mkpairs import build
LIMIT = float(sys.argv[1]) if len(sys.argv) > 1 else 21.7
LG = int(sys.argv[2]) if len(sys.argv) > 2 else 26
cands = json.load(open('dp_merged.json'))
seen = set(); rows = []
for r in sorted(cands, key=lambda r: r['bits']):
    key = (r['word'], r['m'], r['k'], r['len'])
    if key in seen or r['bits'] > LIMIT: continue
    seen.add(key)
    M, M2 = build(r['word'], r['m'], r['k'], r['len'], r['D_lo'], r['D_hi'])
    out = subprocess.run(['nice', '-n', '10', 'taskset', '-c', '24-31', './measure', M, M2, str(LG), '0x5eed', '8'], capture_output=True, text=True).stdout
    mm = re.search(r'hash\s+(\d+) / (\d+)', out)
    c, n = int(mm.group(1)), int(mm.group(2))
    rate = c / n if c else float('nan')
    bits = math.log2(r['L']) - math.log2(rate) if c else float('nan')
    rows.append((bits, c, r, M, M2))
    print(f"{r['word']} m={r['m']:5d} k={r['k']:2d} len={r['len']:2d} L={r['L']} pred={r['bits']:.3f}  hits={c:6d}/2^{LG}  meas_bits={bits:.3f}", flush=True)
rows.sort(key=lambda t: t[0])
print("\n# top 10 measured")
for bits, c, r, M, M2 in rows[:10]:
    print(f"{bits:.3f}  {r['word']} m={r['m']} k={r['k']} len={r['len']} L={r['L']} hits={c}\n   M ={M}\n   M2={M2}")
