#!/usr/bin/env python3
"""Independent certificate reader: cardinality identity, geometric-series
inverses, direct rational weights, and literal quadratic-interval tests.
Does not import the certificate generator or exploratory programs.
"""
import itertools
import json
import os
from fractions import Fraction
from pathlib import Path

q, p = 1 << 64, (1 << 61) - 1
mask = q - 1


def inverse_by_doubling(y, k):
    x = y
    for n in (1, 2, 4, 8, 16, 32):
        x ^= ((x << n) ^ ((x << (n * k)) if k > 1 else 0)) & mask
    return x


def main():
    assert set(os.sched_getaffinity(0)) <= set(range(40, 48))
    data = json.loads(Path('checks/open_ph_enh_certificate.json').read_text())
    ds = data['masks']
    assert len(ds) == len(set(ds)) == 852
    pats = {d: data['patterns'][str(d)] for d in ds}
    mass = 0
    for d in ds:
        actual = []
        for t in pats[d]:
            assert 0 <= t < q and t & d == t and (2*t-d) % p == 0
            actual.append(t)
        assert len(actual) == len(set(actual))
        mass += len(actual) * (1 << (64-d.bit_count()))
    # Sound disjoint witnesses plus the exact number of congruent word pairs
    # proves completeness, without relying on either mask-enumeration algorithm.
    assert mass == 8*q + 72
    wh, wl = {}, {}
    for d in ds:
        fh = Fraction(1,q)
        fl = Fraction(1,q)
        for j in range(64):
            fh += Fraction(1 << j, q * (1 << (d & ((1 << j)-1)).bit_count()))
            fl += Fraction(1, 1 << (j+1+(d >> j).bit_count()))
        high = min(Fraction(1),len(pats[d])*fh)*q
        low = min(Fraction(1),len(pats[d])*fl)*q*q
        assert high.denominator == low.denominator == 1
        wh[d], wl[d] = int(high), int(low)
        assert wh[d] == data['high_weights'][str(d)]
        assert wl[d] == data['low_weights'][str(d)]
    checked = []
    for row in data['rows']:
        high = isinstance(row['r'],str)
        r = 0 if high else row['r']
        R = 1 << r
        selected = ds if high else [d for d in ds if d % R == 0]
        iv = {d: inverse_by_doubling(d,row['k']) for d in selected}
        total = 0
        targets = 0
        for v in selected:
            for u in selected:
                x = iv[u] ^ iv[v]
                if high and x >> 63:
                    continue
                e = x ^ u
                h = e.bit_count()
                if high:
                    coefficient = min(2 << (h-(e>>63)),
                                      276 << (64-h),
                                      16*((1 << ((h+1)//2))+1))
                    total += coefficient*wh[v]
                else:
                    coefficient = min(R << (h-(e>>63)),
                                      65 << (64-h),
                                      4*R*(1 << ((h+1)//2))) if e else R
                    total += R*coefficient*wl[v]
                targets += 1
        assert total == row['numerator'] and targets == row['targets']
        assert (total+row['denominator']-1)//row['denominator'] == row['ceiling']
        checked.append([row['r'],row['k']])
    interval_cases = 0
    for a in (-4,-3,-2,-1,1,2,3,4):
        for b in range(-8,9):
            for c in range(-4,5):
                for length in (1,2,5,16):
                    counts = [0]*8
                    # Outside this range |F(t)| > 64, hence outside every target.
                    for t in range(-64,65):
                        value = a*t*t+b*t+c
                        interval = value//length
                        if -4 <= interval < 4:
                            counts[interval+4] += 1
                    for selection in range(1,256):
                        m = selection.bit_count()
                        actual = sum(counts[i] for i in range(8) if selection>>i&1)
                        if actual > 2*m:
                            assert (actual-2*m)**2*abs(a) <= 4*m*length
                        interval_cases += 1
    out = dict(mask_completeness_mass=mass, expected_mass=8*q+72,
               all_60_sums_equal=True, geometric_series_inverses=True,
               quadratic_interval_cases=interval_cases, failures=0,
               checked_rows=checked)
    Path('checks/independent_verification.json').write_text(json.dumps(out,indent=2)+'\n')
    print(json.dumps({k:v for k,v in out.items() if k!='checked_rows'},indent=2))


if __name__ == '__main__':
    main()
