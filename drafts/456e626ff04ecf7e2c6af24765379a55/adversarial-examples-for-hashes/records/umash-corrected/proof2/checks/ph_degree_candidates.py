#!/usr/bin/env python3
"""Exploratory PH two-key fibre bounds by polynomial degrees and pivots."""
import json
from collections import defaultdict
from pathlib import Path
from ph_enh_candidates import W, Q, P, MASK, support, patterns


def leadmask(t, h, d):
    result = 0
    for i in range(W - t):
        a = h + 2 * i
        b = d + i if d >= 0 else -1
        if a != b:
            result |= 1 << max(a, b)
    return result


def weights(ds, pats, piv):
    return [len({x & piv for x in pats[d]}) << (W - (d & piv).bit_count())
            for d in ds]


def main():
    ds = sorted(set().union(*(support(j * P, W) for j in range(9))))
    pats = {d: patterns(d) for d in ds}
    rows = []
    # First probe only equal nonzero coordinate differences (t=h=0).
    for t in (0, 1, 2, 3):
        for h in range(t, 2 * t + 1):
            tables = []
            for d in range(W + t):
                piv = leadmask(t, h, d)
                lo = weights(ds, pats, piv & MASK)
                hi = weights(ds, pats, piv >> W)
                maxlo, maxhi = [], []
                for b in range(W):
                    groups = defaultdict(int)
                    for z, v in zip(ds, lo):
                        groups[z >> (b + 1)] += v
                    maxlo.append(max(groups.values()))
                    groups = defaultdict(int)
                    for z, v in zip(ds, hi):
                        groups[z >> (b + 1)] += v
                    maxhi.append(max(groups.values()))
                tables.append((sum(lo), max(hi), maxlo, maxhi))
            for G in range(W - t):
                # c=0 contributes at most one difference target of mass 1.
                total = Q * Q
                for d, (sumlo, hi, maxlo, maxhi) in enumerate(tables):
                    cutoff = G + d
                    val = maxlo[cutoff] * hi if cutoff < W else sumlo * maxhi[cutoff - W]
                    total += min(val, (1 << d) * Q * Q)
                row = dict(t=t, h=h, G=G, numerator=total,
                           denominator=(1 << t) * Q * Q,
                           ceiling=(total + (1 << t) * Q * Q - 1) // ((1 << t) * Q * Q))
                rows.append(row)
            print(json.dumps(dict(t=t, h=h, maximum=max(x['ceiling'] for x in rows if x['t']==t and x['h']==h))), flush=True)
    Path('checks/ph_degree_candidates.json').write_text(json.dumps(rows, indent=2) + '\n')


if __name__ == '__main__':
    main()
