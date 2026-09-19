#!/usr/bin/env python3
"""Exploratory low-lane PH+ENH sums for minimum valuations 1,2,3."""
import json
from pathlib import Path
from ph_enh_candidates import W, Q, P, MASK, support, patterns, inv_s, pop


def main():
    ds = sorted(set().union(*(support(j * P, W) for j in range(9))))
    ps = {d: len(patterns(d)) for d in ds}
    # q^2 times the upper probability for a prescribed low-product pattern.
    f = {d: Q + sum(1 << (127 - k - pop(d >> k))
                     for k in range(W)) for d in ds}
    weight = {d: min(Q * Q, ps[d] * f[d]) for d in ds}
    rows = []
    for r in (1, 2, 3):
        R = 1 << r
        dr = [d for d in ds if d % R == 0]
        for k in range(1, 16):
            inv = {d: inv_s(d, k) for d in dr}
            total = 0
            largest = []
            for u in dr:
                for v in dr:
                    x = inv[u] ^ inv[v]
                    e = u ^ x
                    h = pop(e)
                    bounds = [R * (1 << (h - (e >> 63))),
                              65 * (1 << (64 - h))]
                    a = min(bounds)
                    term = R * a * weight[v]
                    total += term
                    if len(largest) < 12 or term > largest[-1][0]:
                        largest.append((term, u, v, e, bounds))
                        largest.sort(reverse=True)
                        del largest[12:]
            row = dict(r=r, k=k, numerator=total, denominator=Q * Q,
                       ceiling=(total + Q * Q - 1) // (Q * Q),
                       below_2_41=total < (1 << 41) * Q * Q,
                       largest_terms=largest)
            rows.append(row)
            print(json.dumps({a: b for a, b in row.items() if a != 'largest_terms'}), flush=True)
    Path('checks/small_r_candidates.json').write_text(json.dumps(rows, indent=2) + '\n')


if __name__ == '__main__':
    main()
