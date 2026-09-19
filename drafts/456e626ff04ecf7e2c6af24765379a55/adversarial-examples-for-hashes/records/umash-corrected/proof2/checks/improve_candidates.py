#!/usr/bin/env python3
"""Additional independent upper counts for the finite PH+ENH target set."""
import json
from pathlib import Path
from ph_enh_candidates import W, Q, P, MASK, support, patterns, inv_s, pop


def fh(d):
    return 1 + sum(1 << (k - pop(d & ((1 << k) - 1))) for k in range(W))


def main():
    ds = sorted(set().union(*(support(j * P, W) for j in range(9))))
    ps = {d: len(patterns(d)) for d in ds}
    weight = {d: min(Q, ps[d] * fh(d)) for d in ds}
    common = {u & v for u in ds for v in ds}
    fhs = {d: fh(d) for d in common}
    rows = []
    for k in range(1, 16):
        inv = {d: inv_s(d, k) for d in ds}
        total = 0
        counts = [0] * 5
        largest = []
        for u in ds:
            for v in ds:
                x = inv[u] ^ inv[v]
                if x >= Q // 2:
                    continue
                e = u ^ x
                h = pop(e)
                bounds = [2 * (1 << (h - (e >> 63))) * weight[v],
                          276 * (1 << (64 - h)) * weight[v],
                          2 * ps[u] * (1 << pop(e & (MASK ^ u))) * weight[v],
                          2 * ps[v] * (1 << pop(e & (MASK ^ v))) * Q,
                          2 * ps[u] * ps[v] * (1 << pop(e & (MASK ^ (u | v)))) * fhs[u & v]]
                term = min(bounds)
                counts[bounds.index(term)] += 1
                total += term
                if len(largest) < 20 or term > largest[-1][0]:
                    largest.append((term, u, v, e, bounds))
                    largest.sort(reverse=True)
                    del largest[20:]
        row = dict(k=k, numerator=total, denominator=Q,
                   ceiling=(total + Q - 1) // Q,
                   below_2_41=total < (1 << 41) * Q,
                   winning_bounds=counts, largest_terms=largest)
        rows.append(row)
        print(json.dumps({a: b for a, b in row.items() if a != 'largest_terms'}), flush=True)
    Path('checks/improve_candidates.json').write_text(json.dumps(rows, indent=2) + '\n')


if __name__ == '__main__':
    main()
