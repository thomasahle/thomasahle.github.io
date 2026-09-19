#!/usr/bin/env python3
"""Separate reader: mask completeness, envelope coefficients, and exact scores.

Does not import the certificate generator. Integer comparisons below are proof
certificates; decimal strings in the generated JSON are never consulted.
"""
import json
import os
from collections import Counter
from fractions import Fraction
from pathlib import Path

root = Path(__file__).resolve().parents[1]
q, p = 2**64, 2**61-1
cert = json.loads((root/'checks/fingerprint_certificate.json').read_text())
assert set(os.sched_getaffinity(0)) <= set(range(40,48))
assert cert['C1'] == 3125 and cert['C2'] == 729632
assert cert['J'] == 1416246956032
words = cert['mask_rows']
assert len(words) == len({row['mask'] for row in words}) == 852
total = 0
for row in words:
    d, ts = row['mask'], row['patterns']
    assert 0 <= d < q and ts == sorted(set(ts)) and ts
    expected = []
    for j in range(-8,9):
        x = d+j*p
        if x >= 0 and x % 2 == 0 and (x//2) & d == x//2:
            expected.append(x//2)
    assert ts == sorted(set(expected))
    for t in ts:
        assert t ^ (d ^ t) == d
        assert (t-(d ^ t)) % p == 0
    total += len(ts)*2**(64-bin(d).count('1'))
# The sets represented by (d,t) are disjoint. This independently proves coverage.
assert total == q+2*sum(q-j*p for j in range(1,9)) == 8*q+72
assert max(Counter(row['mask'] & 1 for row in words).values()) == 604
assert max(Counter(row['mask'] >> 63 for row in words).values()) == 604

a = Fraction(732757,q-561)
b = Fraction(345763417,2**116) / Fraction(q-561,q)
assert list((a.numerator,a.denominator)) == cert['A']
assert list((b.numerator,b.denominator)) == cert['B']
degree = 2**19
den = p-2
# The expanded polynomial is the independent reader's representation.
c0, c1, c2 = b, (a-2*b)*2/den, (1-a+b)*4/den**2
def error(L):
    if L == 1:
        return Fraction(1,q*(q-561))
    n = (L-1)//32+1
    if 2*n >= den:
        return Fraction(1)
    return c0+c1*n+c2*n*n

k = Fraction(cert['K'][0],cert['K'][1])
assert k == 2**83*(b+a*degree/den+Fraction(degree**2,den**2))
assert 128*k.numerator < 81*k.denominator
assert c2 > 0 and c1 > 0 and c0 > 0
remainder = c0+c1*Fraction(31,32)+c2*Fraction(31,32)**2
assert remainder > 0
assert [remainder.numerator,remainder.denominator] == cert['convex_remainder']
for name, row in cert['scores'].items():
    L = row['length']
    if name == 'published_full_byte_domain':
        h = (L-1)//2**23+1
        ratio = Fraction(L*2**83,h*h)
    else:
        ratio = Fraction(L)/error(L)
    n,d = ratio.numerator,ratio.denominator
    assert [n,d] == row['ratio']
    lo,hi = row['interval_hundredths']
    assert n**100 > 2**lo*d**100
    assert n**100 < 2**hi*d**100
other_ratios = {
    'rounded_quadratic_short_domain': Fraction(2**83*128,81),
    'rounded_quadratic_full_domain': Fraction(128*(2**61-2**23+1)*128,81),
    'inherited_linear_coarse': Fraction(2**61,58),
    'inherited_primary4_precise': 2/(Fraction(435,q-561)+Fraction(2,p-2)),
}
for name,row in cert['other_scores'].items():
    ratio = other_ratios[name]
    n,d = ratio.numerator,ratio.denominator
    assert [n,d] == row['ratio']
    lo,hi = row['interval_hundredths']
    assert n**100 > 2**lo*d**100 and n**100 < 2**hi*d**100

# Verify endpoint comparisons for the convexity argument over each whole domain.
small, full, sat = 2**23-31, 2**61-31, 2**65-63
assert error(33)/33 < error(2)/2
assert error(small)/small < error(2)/2 < error(full)/full
assert error(sat-32)/(sat-32) > Fraction(1,sat)
assert error(2)/2 < Fraction(1,sat)
assert b < Fraction(205,q-561) and a < 1
assert error(1) < Fraction(1,q-561)

# Direct integer proof of the shared-multiplier fibre probability.
bad = int(cert['shared_multiplier_fibre']['bad_key_count'])
all_keys = int(cert['shared_multiplier_fibre']['total_key_count'])
product = 1
for j in range(34):
    product *= q-j
assert bad == product and all_keys == (p-2)*product
assert 2**83*bad > all_keys

out = dict(status='PASS', independent_mask_pair_total=str(total),
           score_intervals_checked=len(cert['scores'])+len(cert['other_scores']),
           exact_headline='K < 81/128 < 1',
           distinction='Imported probability lemmas remain dependencies; this is an arithmetic verifier.')
(root/'checks/independent_verification.json').write_text(json.dumps(out,indent=2)+'\n')
print(json.dumps(out,indent=2))
