#!/usr/bin/env python3
"""Exploratory exact sums; the constants do not by themselves prove coverage."""
import json
import platform
from functools import lru_cache
from pathlib import Path

assert platform.system() == "Linux", "Run enumeration on the Xeon"
W = 64
Q = 1 << W
P = (1 << 61) - 1
MASK = Q - 1


def pop(x):
    return bin(x).count('1')


@lru_cache(None)
def support(n, w):
    if w == 0:
        return frozenset([0]) if n == 0 else frozenset()
    if abs(n) >= 1 << w:
        return frozenset()
    if n % 2 == 0:
        return frozenset(2 * z for z in support(n // 2, w - 1))
    return frozenset(2 * z + 1 for a in ((n - 1) // 2, (n + 1) // 2)
                     for z in support(a, w - 1))


def patterns(d):
    return [(d + j * P) // 2 for j in range(-8, 9)
            if d + j * P >= 0 and (d + j * P) % 2 == 0
            and ((d + j * P) // 2) & d == (d + j * P) // 2]


def inv_s(y, k):
    x = 0
    for i in range(W):
        bit = (y >> i) & 1
        if i >= 1:
            bit ^= (x >> (i - 1)) & 1
        if k > 1 and i >= k:
            bit ^= (x >> (i - k)) & 1
        x |= bit << i
    return x


def run():
    ds = sorted(set().union(*(support(j * P, W) for j in range(9))))
    ps = {d: len(patterns(d)) for d in ds}
    f = {d: 1 + sum(1 << (k - pop(d & ((1 << k) - 1)))
                    for k in range(W)) for d in ds}
    weight = {d: min(Q, ps[d] * f[d]) for d in ds}
    rows = []
    for k in range(1, 16):
        inv = {d: inv_s(d, k) for d in ds}
        total = 0
        counts = [0, 0, 0]
        largest = []
        for u in ds:
            for v in ds:
                x = inv[u] ^ inv[v]
                if x >= Q // 2:
                    continue
                e = u ^ x
                h = pop(e)
                bounds = [2 * (1 << (h - (e >> 63))),
                          276 * (1 << (64 - h)),
                          2 * ps[u] * (1 << pop(e & (MASK ^ u)))]
                a = min(bounds)
                counts[bounds.index(a)] += 1
                term = a * weight[v]
                total += term
                if len(largest) < 12 or term > largest[-1][0]:
                    largest.append((term, u, v, e, bounds))
                    largest.sort(reverse=True)
                    del largest[12:]
        row = dict(k=k, numerator=total, denominator=Q,
                   ceiling=(total + Q - 1) // Q,
                   below_2_41=total < (1 << 41) * Q,
                   winning_bounds=counts, largest_terms=largest)
        rows.append(row)
        print(json.dumps({a: b for a, b in row.items() if a != 'largest_terms'}), flush=True)
    result = dict(scope="exploratory candidate bounds, coverage needs proof",
                  masks=len(ds), weight_sum_numerator=sum(weight.values()),
                  weight_sum_denominator=Q, rows=rows)
    Path('checks/ph_enh_candidates.json').write_text(json.dumps(result, indent=2) + '\n')


if __name__ == '__main__':
    run()
