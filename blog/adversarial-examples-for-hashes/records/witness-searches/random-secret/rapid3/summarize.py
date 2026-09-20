#!/usr/bin/env python3
"""Parse logs/*.txt from rs_rapid3 runs and print a results table with exact Poisson 95% CIs."""
import glob, math, os, re, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from ci import garwood
rows = []
for f in sorted(glob.glob(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'logs', '*.txt'))):
    txt = open(f).read()
    model = 'default secret' if 'DEFAULT secret' in txt else 'random secret'
    for m in re.finditer(r'^pair\s+len\s+(\d+)/\s*(\d+) L=(\d+) : (\d+) / (\d+)', txt, re.M):
        l1, l2, L, c, T = map(int, m.groups())
        lo, hi = garwood(c)
        if c:
            r = math.log2(c / T); bits = math.log2(L) - r
            rows.append((os.path.basename(f), model, f'{l1}/{l2}', L, f'{c} / 2^{math.log2(T):.0f}', f'2^{r:.3f} [2^{math.log2(lo/T):.3f}, 2^{math.log2(hi/T):.3f}]', f'{bits:.3f} [{math.log2(L)-math.log2(hi/T):.3f}, {math.log2(L)-math.log2(lo/T):.3f}]'))
        else:
            rows.append((os.path.basename(f), model, f'{l1}/{l2}', L, f'0 / 2^{math.log2(T):.0f}', f'<= 2^{math.log2(hi/T):.3f} (95%)', f'>= {math.log2(L)-math.log2(hi/T):.3f}'))
    for m in re.finditer(r'^fold n=64 dA=(\w+) dB=(\w+) : (\d+) / 2\^(\d+)', txt, re.M):
        dA, dB, c, l2 = m.group(1), m.group(2), int(m.group(3)), int(m.group(4)); T = 2 ** l2
        lo, hi = garwood(c)
        rows.append((os.path.basename(f), 'fold primitive', f'dA={dA} dB={dB}', '-', f'{c} / 2^{l2}', f'2^{math.log2(c/T):.3f} [2^{math.log2(lo/T):.3f}, 2^{math.log2(hi/T):.3f}]' if c else '0', '-'))
print('| log | model | len | L | count | rate (95% CI) | bits (95% CI) |')
print('|---|---|---|---|---|---|---|')
for r in rows: print('| ' + ' | '.join(str(x) for x in r) + ' |')
