#!/usr/bin/env python3
"""Exact arithmetic for PROOF5; exact arithmetic certificate generator.

Earlier probability lemmas are imported, not inferred from numerical tables.
This file independently recounts the mask and subcase-b tables, checks the
imported numerical ceilings, and certifies every new envelope/score comparison.
"""
from collections import Counter
from decimal import Decimal, localcontext
from fractions import Fraction as F
from functools import lru_cache
from pathlib import Path
import hashlib
import json
import os
import platform
import resource

ROOT = Path(__file__).resolve().parents[1]
Q, P = 1 << 64, (1 << 61) - 1
M = P - 2
J = 345763417 * 4096
C1, C2 = 3125, 2 * 604**2
A = F(C1 + C2, Q - 561)
B = F(J, Q * (Q - 561))


def frac(x):
    x = F(x)
    return [x.numerator, x.denominator]


def read(path):
    return json.loads((ROOT / path).read_text())


def ceil(x):
    return -(-x.numerator // x.denominator)


@lru_cache(None)
def signed(n, w):
    if abs(n) >= 1 << w:
        return frozenset()
    if w == 0:
        return frozenset({0}) if n == 0 else frozenset()
    if n % 2 == 0:
        return frozenset(2 * d for d in signed(n // 2, w - 1))
    return frozenset(2 * d + 1 for t in ((n - 1) // 2, (n + 1) // 2)
                     for d in signed(t, w - 1))


def masks_and_patterns():
    ds = sorted(set().union(*(signed(j * P, 64) for j in range(9))))
    patterns = {}
    for d in ds:
        patterns[d] = sorted({(d + j * P) // 2 for j in range(-8, 9)
                              if d + j * P >= 0 and (d + j * P) % 2 == 0
                              and ((d + j * P) // 2) & d == (d + j * P) // 2})
    assert len(ds) == 852
    assert all(patterns.values())
    assert sum(len(patterns[d]) * (1 << (64 - d.bit_count())) for d in ds) == 8 * Q + 72
    assert Counter(d & 1 for d in ds) == {0: 248, 1: 604}
    assert Counter(d >> 63 for d in ds) == {0: 248, 1: 604}
    return ds, patterns


def subcase_b(ds):
    rows = []
    prior = read('materials/umash-subcase-b/verification/results/table.json')
    for h in range(1, 16):
        n, s = [0] * (1 << h), [0] * (1 << h)
        mask = (1 << h) - 1
        run = (1 << h) - 8
        for u in ds:
            shifted = (u << 1) & mask
            for v in ds:
                c = (v & mask) ^ shifted
                n[c] += 1
                if h < 5 or v & run != run:
                    s[c] += 1
        nh, sh = max(n), max(s)
        rho = min(F(1), F(h + 1) * F(2) ** (4 - h))
        bound = F(min(nh * nh, sh * sh + rho * nh * nh), 1 << (128 - h))
        old = prior['rows'][h - 1]
        assert nh == old['review_N'] and sh == old['review_S']
        assert bound == F(int(old['bound']['numerator']), int(old['bound']['denominator']))
        rows.append(dict(h=h, N=nh, S=sh, rho=frac(rho), bound=frac(bound)))
    assert max(F(*r['bound']) for r in rows) == F(J, Q * Q)
    assert rows[-1]['N'] == 68948 and rows[-1]['S'] == 2466
    return rows


def imported_checks():
    primary = read('materials/checks3/primary_certificate.json')
    assert primary['ph_constant'] == 1123
    assert primary['block_constant'] == primary['enh_constant'] == C1
    assert max(r['constant'] for r in primary['lifting_groups']) == C1
    ph_enh = read('materials/checks2/open_ph_enh_certificate.json')
    assert ph_enh['final_numerator'] == 170906186782
    assert ph_enh['final_denominator'] == Q * Q
    # The imported certificate contains every shuffler/valuation sum.
    ceilings = []
    for row in ph_enh['rows']:
        value = F(row['numerator'], row['denominator'])
        ceilings.append(ceil(value))
    assert max(ceilings) == 170906186782
    phi = read('materials/umash-enh-verify/certificates/phi.json')
    assert len(phi) == 15
    assert max(ceil(F(int(r['numerator']), int(r['denominator']))) for r in phi) == 111924178297
    assert 111924178297 < 1 << 37
    enh = read('materials/checks/constants.json')['enh_only_joint']
    assert F(int(enh['numerator']), int(enh['denominator'])) == F(4721784, Q * Q)
    return {'PH_ENH_rows': len(ceilings), 'PH_one_word_rows': len(phi),
            'PH_ENH_maximum_ceiling': max(ceilings), 'primary': C1}


def rho(L):
    return min(F(1), F(2 * ((L + 31) // 32), M))


def envelope(L):
    if L == 1:
        return F(1, Q * (Q - 561))
    r = rho(L)
    return B * (1 - r)**2 + A * r * (1 - r) + r * r


def primary4(L):
    if L == 1:
        return F(1, Q - 561)
    a4 = F(205, Q - 561)
    s4 = F(435, Q - 561) + F(2, M)
    return min(F(1), max(a4 + (1 - a4) * rho(L), s4))


def interval(value, lower, upper):
    """Certify lower/100 < log2(value) < upper/100 with integers."""
    assert value.numerator**100 > (1 << lower) * value.denominator**100
    assert value.numerator**100 < (1 << upper) * value.denominator**100
    with localcontext() as ctx:
        ctx.prec = 80
        score = (Decimal(value.numerator) / Decimal(value.denominator)).ln() / Decimal(2).ln()
    return {'ratio': frac(value), 'interval_hundredths': [lower, upper],
            'decimal_display_only': str(score)}


def main():
    assert platform.system() == 'Linux'
    assert set(os.sched_getaffinity(0)) <= set(range(40, 48))
    assert os.getpriority(os.PRIO_PROCESS, 0) >= 10
    assert resource.getrlimit(resource.RLIMIT_AS)[0] <= 32_000_000_000
    ds, patterns = masks_and_patterns()
    rows = subcase_b(ds)
    imports = imported_checks()

    # Exhaustive decision-list rows. Joint numerators have denominator q^2.
    cases = [
        ('different_chunk_counts', 82, F(604**2) + F(1, Q), 1 + 82 * 604**2),
        ('different_checksums_PH', 1123, 604**2, 1123 * 604**2),
        ('equal_checksum_two_or_more_PH', 1123, C2, J),
        ('equal_checksum_one_PH_one_ENH_word', 1123, C2, 1 << 37),
        ('equal_checksum_one_PH_two_ENH_words_r0', 1123, C2, 852**2),
        ('equal_checksum_one_PH_two_ENH_words_positive_r', 1123, C2, 170906186782),
        ('ENH_only', 3125, 604**2, 4721784),
        ('tag_only', F(1, 8), 195840, 11946240 * 4096),
        ('different_block_counts', 82, 46, 82 * 46),
        ('short_long', 9, 9, 81),
    ]
    ledger = []
    for name, c1, c2, joint in cases:
        assert c1 <= C1 and c2 <= C2 and joint <= J
        ledger.append(dict(case=name, primary_numerator=frac(c1),
                           secondary_numerator=frac(c2), joint_numerator=joint))
    assert J == 1416246956032 and C1 + C2 == 732757
    assert B < F(1, 1 << 87)
    assert 0 < B < A < 1 and 1 - A + B > 0 and A - 2 * B > 0
    assert B < F(205, Q - 561)  # pointwise dominance over primary4

    r0 = F(1 << 19, M)
    k = (B + A * r0 + r0*r0) * (1 << 83)
    assert F(5, 8) < k < F(81, 128) < 1
    assert F(1, Q * (Q - 561)) < k / (1 << 83)

    # Convex-plateau score proof: G(t)=F(t)/(32t-31).
    u, v, w = (1-A+B)*F(4, M*M), (A-2*B)*F(2, M), B
    remainder = w + F(31, 32)*v + F(31, 32)**2*u
    assert remainder > 0
    assert envelope(33) / 33 < envelope(2) / 2
    small_last = (1 << 23) - 31
    full_last = (1 << 61) - 31
    saturation = (1 << 65) - 63
    assert envelope(small_last) / small_last < envelope(2) / 2
    assert envelope(full_last) / full_last > envelope(2) / 2
    assert rho(saturation) == 1 and rho(saturation - 32) < 1
    assert envelope(saturation - 32) / (saturation - 32) > F(1, saturation)
    assert envelope(2) / 2 < F(1, saturation)
    scores = {
        'through_2pow23_words': dict(length=2, **interval(F(2) / envelope(2), 8863, 8864)),
        'through_2pow61_minus1_words': dict(length=full_last, **interval(F(full_last) / envelope(full_last), 6899, 6900)),
        'all_positive_lengths': dict(length=saturation-32,
            **interval(F(saturation-32)/envelope(saturation-32), 6499, 6500)),
        'published_full_byte_domain': dict(length=(1 << 61)-(1 << 23)+1,
            **interval(F(128*((1 << 61)-(1 << 23)+1)), 6799, 6800)),
    }
    other_scores = {
        'rounded_quadratic_short_domain': interval(F((1 << 83)*128,81), 8366,8367),
        'rounded_quadratic_full_domain': interval(F(128*((1 << 61)-(1 << 23)+1))*F(128,81),6866,6867),
        'inherited_linear_coarse': interval(F(1 << 61,58),5514,5515),
        'inherited_primary4_precise': interval(F(2)/(F(435,Q-561)+F(2,M)),5618,5619),
    }
    lengths = set(range(1, 4097))
    for e in range(0, 70):
        for step in (32, 512, 1 << 23):
            for d in (-33, -32, -31, -1, 0, 1, 31, 32, 33):
                lengths.add(max(1, step*(1 << e)+d))
    for L in lengths:
        e = envelope(L)
        h = (L + (1 << 23) - 1) // (1 << 23)
        assert 0 < e <= 1
        assert e < F(81*h*h, 128*(1 << 83))
        assert e <= primary4(L)
        assert primary4(L) <= min(F(1), F(58*((L+511)//512), 1 << 61))
    # Exact shared-multiplier bad-fibre count, with distinct OH words.
    distinct_keys = 1
    for i in range(34):
        distinct_keys *= Q-i
    bad_fibre = F(distinct_keys, M*distinct_keys)
    assert bad_fibre == F(1, M) and bad_fibre > F(1, 1 << 83)

    result = dict(status='PASS', model='independent polynomial multipliers; ideal full keys',
                  q=Q, p=P, multiplier_denominator=M, C1=C1, C2=C2, J=J,
                  A=frac(A), B=frac(B), K=frac(k), K_decimal_display=float(k),
                  mask_rows=[dict(mask=d, patterns=patterns[d]) for d in ds],
                  subcase_b=rows, imported_checks=imports, ledger=ledger,
                  convex_remainder=frac(remainder), scores=scores, other_scores=other_scores,
                  sampled_boundary_assertions=len(lengths),
                  shared_multiplier_fibre=dict(probability=frac(bad_fibre),
                      factor_over_published=frac(bad_fibre*(1 << 83)),
                      bad_key_count=str(distinct_keys), total_key_count=str(M*distinct_keys)),
                  environment=dict(host=platform.node(), python=platform.python_version(),
                      affinity=sorted(os.sched_getaffinity(0)), nice=os.getpriority(os.PRIO_PROCESS,0),
                      address_space_limit=resource.getrlimit(resource.RLIMIT_AS)[0]))
    (ROOT/'checks/fingerprint_certificate.json').write_text(json.dumps(result, indent=2)+'\n')
    print(json.dumps({k:result[k] for k in ('status','C1','C2','J','K_decimal_display','scores')}, indent=2))


if __name__ == '__main__':
    main()
