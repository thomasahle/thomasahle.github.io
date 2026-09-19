#!/usr/bin/env python3
"""Exact central 95% Poisson (Garwood) intervals for the RESULT lines in the given logs.
bits = log2(L / rate), L = ceil(max(lenA, lenB) / 8). Zero hits: 97.5% one-sided upper bound.
usage: python3 ci.py log [log ...]   (a log given as a:b pools count a over count b? no: pass files; use --pool to add them)"""
import math, re, sys

def poisson_cdf(k, mu):
    # P[X <= k], summed in log space (exp(-mu) underflows for mu > 745)
    if mu <= 0: return 1.0
    logs = [-mu + i * math.log(mu) - math.lgamma(i + 1) for i in range(k + 1)]
    m = max(logs)
    return math.exp(m) * sum(math.exp(x - m) for x in logs)

def garwood(k, alpha=0.05):
    def solve(f, lo, hi):
        for _ in range(200):
            mid = (lo + hi) / 2
            if f(mid): lo = mid
            else: hi = mid
        return (lo + hi) / 2
    lo = 0.0 if k == 0 else solve(lambda m: 1 - poisson_cdf(k - 1, m) < alpha / 2, 0.0, max(10.0, 3 * k))
    hi = solve(lambda m: poisson_cdf(k, m) > alpha / 2, 0.0, max(10.0, 3 * k + 30))
    return lo, hi

def report(label, hits, trials, L):
    lo, hi = garwood(hits)
    if hits:
        rate = hits / trials
        print(f"{label}: {hits}/2^{math.log2(trials):.2f} rate=2^{math.log2(rate):.3f} [2^{math.log2(lo/trials):.3f}, 2^{math.log2(hi/trials):.3f}]  L={L} bits={math.log2(L/rate):.2f} [{math.log2(L*trials/hi):.2f}, {math.log2(L*trials/lo):.2f}]")
    else:
        print(f"{label}: 0/2^{math.log2(trials):.2f} rate<=2^{math.log2(hi/trials):.3f} (97.5%)  L={L} bits>={math.log2(L*trials/hi):.2f}")

pool = {}
for fn in sys.argv[1:]:
    for line in open(fn):
        m = re.search(r"RESULT lenA=(\d+) lenB=(\d+) model=(\w+) rng=(\w+) salt=(\d+) threads=\d+ trials=(\d+) hits=(\d+)", line)
        if not m: continue
        la, lb, model, rng, salt, trials, hits = m.groups()
        L = -(-max(int(la), int(lb)) // 8)
        report(f"{fn} ({model}, {rng}, salt {salt})", int(hits), int(trials), L)
        key = (la, lb, model)
        h, t = pool.get(key, (0, 0)); pool[key] = (h + int(hits), t + int(trials))
for (la, lb, model), (h, t) in pool.items():
    if t and len([1 for fn in sys.argv[1:] if True]) > 1:
        report(f"POOLED len {la}/{lb} {model}", h, t, -(-max(int(la), int(lb)) // 8))
