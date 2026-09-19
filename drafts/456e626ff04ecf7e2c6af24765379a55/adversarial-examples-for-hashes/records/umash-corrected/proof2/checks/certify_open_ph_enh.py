#!/usr/bin/env python3
"""Exact certificate for PROOF2.md. Standard-library Python 3.10 or newer.

All counts are integers or exact rational pairs. No numerical integral,
floating-point square root, random sampling, or optimization oracle is used.
"""
import hashlib
import json
import os
import platform
from fractions import Fraction
from functools import lru_cache
from pathlib import Path

W = 64
Q = 1 << W
P = (1 << 61) - 1
MASK = Q - 1


@lru_cache(None)
def signed_supports(n, w):
    if w == 0:
        return frozenset([0]) if n == 0 else frozenset()
    if abs(n) >= 1 << w:
        return frozenset()
    if n % 2 == 0:
        return frozenset(2 * a for a in signed_supports(n // 2, w - 1))
    return frozenset(2 * a + 1 for b in ((n - 1) // 2, (n + 1) // 2)
                     for a in signed_supports(b, w - 1))


def addition_masks(j, w, p):
    states = {(0, 0)}
    for i in range(w):
        new = set()
        bit = (j * p >> i) & 1
        for carry, mask in states:
            for x in (0, 1):
                z = x + bit + carry
                new.add((z >> 1, mask | ((x ^ (z & 1)) << i)))
        states = new
    return {m for c, m in states if c == 0}


def make_masks(w, p):
    n = ((1 << w) - 1) // p
    a = set().union(*(signed_supports(j * p, w) for j in range(n + 1)))
    b = set().union(*(addition_masks(j, w, p) for j in range(n + 1)))
    assert a == b
    return sorted(a)


def patterns(d, w=W, p=P):
    n = ((1 << w) - 1) // p
    return [(d + j * p) // 2 for j in range(-n, n + 1)
            if d + j * p >= 0 and (d + j * p) % 2 == 0
            and ((d + j * p) // 2) & d == (d + j * p) // 2]


def inverse(y, k, w=W):
    x = 0
    for i in range(w):
        b = (y >> i) & 1
        if i:
            b ^= (x >> (i - 1)) & 1
        if k > 1 and i >= k:
            b ^= (x >> (i - k)) & 1
        x |= b << i
    sh = (x << 1) ^ ((x << k) if k > 1 else 0)
    assert (x ^ sh) & ((1 << w) - 1) == y
    return x


def high_weight_numerator(d, pattern_count):
    f = 1 + sum(1 << (k - (d & ((1 << k) - 1)).bit_count())
                for k in range(W))
    return min(Q, pattern_count * f)


def low_weight_numerator(d, pattern_count):
    f = Q + sum(1 << (127 - k - (d >> k).bit_count()) for k in range(W))
    return min(Q * Q, pattern_count * f)


def high_bound(e):
    h = e.bit_count()
    return min(2 * (1 << (h - (e >> 63))),
               276 * (1 << (64 - h)),
               16 * ((1 << ((h + 1) // 2)) + 1))


def low_bound(e, r):
    R = 1 << r
    if e == 0:
        return R
    h = e.bit_count()
    return min(R * (1 << (h - (e >> 63))),
               65 * (1 << (64 - h)),
               4 * R * (1 << ((h + 1) // 2)))


def certify():
    assert platform.system() == 'Linux', 'Run the certificate on the Xeon'
    assert set(os.sched_getaffinity(0)) <= set(range(40, 48))
    assert os.getpriority(os.PRIO_PROCESS, 0) >= 10
    ds = make_masks(W, P)
    assert len(ds) == 852
    pats = {d: patterns(d) for d in ds}
    assert all(pats.values()) and max(map(len, pats.values())) == 8
    assert [sum(d % (1 << r) == 0 for d in ds) for r in range(5)] == [852,248,64,2,1]
    assert [sum(d % 2 == bit for d in ds) for bit in (0,1)] == [248,604]
    assert [sum(d >> 63 == bit for d in ds) for bit in (0,1)] == [248,604]
    wh = {d: high_weight_numerator(d, len(pats[d])) for d in ds}
    wl = {d: low_weight_numerator(d, len(pats[d])) for d in ds}
    scaled_masks = []
    for w in range(5, 11):
        q, p = 1 << w, (1 << (w - 3)) - 1
        d = make_masks(w, p)
        literal = {a ^ b for a in range(q) for b in range(a % p, q, p)}
        assert set(d) == literal
        for m in d:
            pp = set(patterns(m, w, p))
            brute = {a & m for a in range(q) if (a - (a ^ m)) % p == 0}
            assert pp == brute
        scaled_masks.append(dict(w=w, p=p, count=len(d)))
    rows = []
    for r in (1, 2, 3, 4):
        selected = [d for d in ds if r == 4 or d % (1 << r) == 0]
        den = Q if r == 4 else Q * Q
        for k in range(1, 16):
            inv = {d: inverse(d, k) for d in selected}
            total = 0
            targets = 0
            for u in selected:
                for v in selected:
                    x = inv[u] ^ inv[v]
                    if r == 4 and x >= Q // 2:
                        continue
                    e = u ^ x
                    if r == 4:
                        total += high_bound(e) * wh[v]
                    else:
                        assert e % (1 << r) == 0
                        total += (1 << r) * low_bound(e, r) * wl[v]
                    targets += 1
            ceiling = (total + den - 1) // den
            assert total < (1 << 41) * den
            assert Fraction(total, den) / (1 - Fraction(561, Q)) < 1 << 38
            rows.append(dict(r='all_r_ge_4' if r == 4 else r, k=k,
                             targets=targets, numerator=total,
                             denominator=den, ceiling=ceiling))
    maxima = []
    for r in (1, 2, 3, 'all_r_ge_4'):
        maxima.append(max((x for x in rows if x['r'] == r),
                          key=lambda x: Fraction(x['numerator'], x['denominator'])))
    assert max(x['ceiling'] for x in rows) == 170906186782
    primary_small_r = []
    for r in range(4):
        total = Fraction(0)
        for d in ds:
            if d % (1 << r):
                continue
            raw = 1 if r == 0 else low_bound(d, r)
            total += min(Fraction((1 << r) * len(pats[d])),
                         Fraction(raw * wl[d], Q * Q))
        ceiling = (total.numerator + total.denominator - 1) // total.denominator
        assert ceiling <= 162
        primary_small_r.append(dict(r=r, numerator=total.numerator,
                                    denominator=total.denominator, ceiling=ceiling))
    assert [x['ceiling'] for x in primary_small_r] == [14,144,56,9]
    assert 604**2 < 162 * (1 << 12)
    # Divisor lemma used in the additional tag-only primary result.
    divisor_product = Fraction(1)
    divisor_rows = []
    for prime in (2, 3, 5, 7, 11, 13):
        maximum, exponent = max((Fraction((a + 1)**4, prime**a), a)
                                for a in range(128))
        divisor_product *= maximum
        divisor_rows.append(dict(prime=prime, exponent=exponent,
                                 maximum=[maximum.numerator, maximum.denominator]))
    assert divisor_product < 16**4
    tag_products = 16 * 255 * 8 * (1 << (64 - 54))
    tag_pairs_bound = tag_products * (1 << 36)
    assert tag_pairs_bound < Q // 8
    result = dict(status='exact upper-bound certificate; not an attained collision count',
                  word_bits=W, q=Q, p=P, masks=ds,
                  patterns={str(d): pats[d] for d in ds},
                  high_weights={str(d): wh[d] for d in ds},
                  low_weights={str(d): wl[d] for d in ds},
                  rows=rows, maxima=maxima, scaled_mask_checks=scaled_masks,
                  final_numerator=170906186782, final_denominator=Q*Q,
                  primary_enh_common_ph_small_r=primary_small_r,
                  primary_ph_rank_threshold=76,
                  strict_bits_iid=90, strict_bits_distinct=90,
                  divisor_rows=divisor_rows,
                  divisor_product=[divisor_product.numerator, divisor_product.denominator],
                  tag_product_targets=tag_products,
                  tag_pair_upper_bound=tag_pairs_bound,
                  tag_denominator=Q*Q,
                  environment=dict(host=platform.node(), python=platform.python_version(),
                                   affinity=sorted(os.sched_getaffinity(0)),
                                   nice=os.getpriority(os.PRIO_PROCESS, 0)))
    out = Path('checks/open_ph_enh_certificate.json')
    out.write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(dict(maxima=maxima, primary_small_r=primary_small_r,
                         tag_pair_upper_bound=tag_pairs_bound,
                         divisor_product=result['divisor_product'],
                         certificate_sha256=hashlib.sha256(out.read_bytes()).hexdigest()), indent=2))


if __name__ == '__main__':
    certify()
