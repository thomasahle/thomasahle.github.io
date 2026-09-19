#!/usr/bin/env python3
"""Finite sums using new quadratic-pattern bounds; proof coverage separately."""
import json
from pathlib import Path
from ph_enh_candidates import W, Q, P, MASK, support, patterns, inv_s


def main():
    ds = sorted(set().union(*(support(j * P, W) for j in range(9))))
    ps = {d: len(patterns(d)) for d in ds}
    wh = {d: min(Q, ps[d] * (1 + sum(1 << (k - (d & ((1 << k) - 1)).bit_count())
                                   for k in range(W)))) for d in ds}
    wl = {d: min(Q * Q, ps[d] * (Q + sum(1 << (127 - k - (d >> k).bit_count())
                                         for k in range(W)))) for d in ds}
    rows = []
    for r in (1, 2, 3, 4):
        dr = [d for d in ds if r == 4 or d % (1 << r) == 0]
        den = Q if r == 4 else Q * Q
        for k in range(1, 16):
            inv = {d: inv_s(d, k) for d in dr}
            total = 0
            totals = [0, 0, 0]
            for u in dr:
                for v in dr:
                    x = inv[u] ^ inv[v]
                    if r == 4 and x >= Q // 2:
                        continue
                    e = u ^ x
                    h = e.bit_count()
                    if r == 4:
                        bounds = [2 * (1 << (h - (e >> 63))),
                                  276 * (1 << (64 - h)),
                                  16 * ((1 << ((h + 1) // 2)) + 1)]
                        term = min(bounds) * wh[v]
                    else:
                        R = 1 << r
                        bounds = [R * (1 << (h - (e >> 63))),
                                  65 * (1 << (64 - h)),
                                  4 * R * (1 << ((h + 1) // 2))]
                        if not e:
                            bounds = [R, R, R]
                        term = R * min(bounds) * wl[v]
                    totals[bounds.index(min(bounds))] += 1
                    total += term
            row = dict(r=r, k=k, numerator=total, denominator=den,
                       ceiling=(total + den - 1) // den,
                       below_2_41=total < (1 << 41) * den,
                       winning_bounds=totals)
            rows.append(row)
            print(json.dumps(row), flush=True)
    Path('checks/quadratic_candidates.json').write_text(json.dumps(rows, indent=2) + '\n')


if __name__ == '__main__':
    main()
