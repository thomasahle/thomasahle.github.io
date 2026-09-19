#!/usr/bin/env python3
"""Exact production-width certificate for PROOF3.md. Standard library only."""
from collections import Counter, defaultdict
from decimal import Decimal, localcontext
from fractions import Fraction
from functools import lru_cache
from pathlib import Path
import json
import platform

W = 64
Q = 1 << W
P = (1 << 61)-1

@lru_cache(None)
def supports(n, w):
    if abs(n) >= 1 << w:
        return frozenset()
    if not w:
        return frozenset([0]) if n == 0 else frozenset()
    if not n & 1:
        return frozenset(2*x for x in supports(n//2, w-1))
    return frozenset(2*x+1 for a in ((n-1)//2, (n+1)//2)
                     for x in supports(a, w-1))

def signed_masks(w, p):
    return sorted(set().union(*(supports(j*p, w)
                               for j in range(((1 << w)-1)//p+1))))

def carry_masks(w, p):
    result = {0}
    for j in range(1, ((1 << w)-1)//p+1):
        states = {(0, 0)}
        for i in range(w):
            new = set()
            bit = ((j*p) >> i) & 1
            for carry, d in states:
                for x in (0, 1):
                    z = x+bit+carry
                    new.add((z >> 1, d | ((x ^ (z & 1)) << i)))
            states = new
        result.update(d for carry, d in states if carry == 0)
    return sorted(result)

def patterns(d, w=W, p=P):
    limit = ((1 << w)-1)//p
    return sorted({(d+j*p)//2 for j in range(-limit, limit+1)
                   if d+j*p >= 0 and (d+j*p) % 2 == 0
                   and ((d+j*p)//2) & d == (d+j*p)//2})

def label(d, t, r):
    return d % (1 << (r-1)), (d-2*t) % (1 << r)

def function_signature(d, t, r, common=0):
    modulus = 1 << r
    f = lambda x: (d-2*(((x ^ common) & d) ^ t)) % modulus
    c = f(0)
    return (c,) + tuple((f(1 << j)-c) % modulus for j in range(r))

def frac(x):
    return [x.numerator, x.denominator]

def main():
    assert platform.system() == 'Linux', 'Run computations on the Xeon.'
    ds = signed_masks(W, P)
    assert ds == carry_masks(W, P)
    pats = {d: patterns(d) for d in ds}
    assert len(ds) == 852
    assert sum(len(pats[d])*(1 << (W-d.bit_count())) for d in ds) == 8*Q+72
    assert sum(len(pats[d]) for d in ds) == 2771
    prefixes = []
    for s in range(1, 65):
        hist = Counter(d % (1 << s) for d in ds)
        prefixes.append(dict(s=s, maximum=max(hist.values()),
                             histogram=sorted(hist.items())))
    assert [x['maximum'] for x in prefixes[:3]] == [604, 362, 240]
    assert Counter(d >> 63 for d in ds) == {0:248, 1:604}
    assert [sum(d % (1 << r) == 0 for d in ds) for r in range(5)] == [852,248,64,2,1]
    assert [sum(d != 0 and (d & -d) == 1 << r for d in ds)
            for r in range(4)] == [604,184,62,1]
    assert [d for d in ds if d != 0 and d % 8 == 0] == [Q-8]

    groups = []
    # Two independent representations: labels and affine coefficients of f.
    for r in range(4, 64):
        by_label = defaultdict(list)
        by_signature = defaultdict(list)
        for d in ds:
            for t in pats[d]:
                key = label(d, t, r)
                sig = function_signature(d, t, r)
                by_label[key].append((d,t))
                by_signature[sig].append((d,t))
        assert sorted(sorted(v) for v in by_label.values()) == sorted(sorted(v) for v in by_signature.values())
        assert by_label[(0,0)] == [(0,0)]
        count = len(by_label)
        groups.append(dict(r=r, classes=count, constant=2*count-1,
                           representatives=[{'label':list(k), 'mask':v[0][0],
                                             'pattern':v[0][1], 'multiplicity':len(v)}
                                            for k,v in sorted(by_label.items())]))
    assert max(x['classes'] for x in groups) == 1563
    assert groups[-1]['classes'] == 1563
    assert groups[0]['classes'] == 36
    assert all(x['classes'] == 26*x['r']-75 for x in groups[1:])
    small_enh = [(1 << r)*sum(len(pats[d]) for d in ds if d % (1 << r) == 0)
                 for r in range(4)]
    assert small_enh == [2771,1230,508,24]
    enh = max(*small_enh, *(x['constant'] for x in groups))
    assert enh == 3125

    convolution = []
    for s in range(4, 64):
        vals = [852, 2+184*4+62*5+s+12, 4+62*8+s+12, 8+16]
        convolution.append(dict(s=s, by_enh_valuation=vals))
    small_convolution = []
    ph_small = []
    for s in range(1,4):
        m = 1 << s
        hist = dict(prefixes[s-1]['histogram'])
        table = []
        for delta in range(m):
            for epsilon in range(m):
                weighted = sum(hist[(a*b % m) ^ (((a+delta)*(b+epsilon)) % m)]
                               for a in range(m) for b in range(m))
                table.append(dict(delta=delta,epsilon=epsilon,weighted_count=weighted))
        maximum = max(row['weighted_count'] for row in table)
        assert maximum == 852*m
        ph_small.append(maximum//m)
        small_convolution.append(dict(s=s,maximum=maximum,denominator=m,rows=table))
    ph = max(17,604,*ph_small,*(v for row in convolution for v in row['by_enh_valuation']))
    assert ph_small == [852,852,852]
    assert ph == 1123
    assert max(row['by_enh_valuation'][1] for row in convolution) == 1123
    C = max(enh, ph, 82, 1)  # Tag-only is strictly below 1/(8q).
    assert C == 3125
    A = Fraction(C,Q-561)
    raw_constant = (1 << 61)*(A+Fraction(32,P-2))
    assert 422 < raw_constant < 423
    eps2 = A+(1-A)*Fraction(2,P-2)
    ratio = 2/eps2
    assert ratio**50 > 2**2669       # score > 53.38
    assert ratio**100 < 2**5339      # score < 53.39
    assert ratio < 2**55
    assert Q-561 > ratio            # short/short L=1 has a better score
    lengths = set(range(1,4097))
    for k in [1,2,3,100,1 << 20,1 << 60,1 << 61,1 << 64]:
        lengths.update([512*k-1,512*k,512*k+1])
    for L in sorted(lengths):
        if L == 1:
            eps = Fraction(1,Q-561)
        else:
            eps = A+(1-A)*min(1,Fraction(2*((L+31)//32),P-2))
            assert eps/L <= eps2/2
        assert eps <= Fraction(423*((L+511)//512),1 << 61)
    with localcontext() as ctx:
        ctx.prec = 45
        score = (Decimal(ratio.numerator)/Decimal(ratio.denominator)).ln()/Decimal(2).ln()
    # Supply actual 64-bit mask prefix weights to the independent C++ check.
    header = '#pragma once\nstatic const unsigned prefix_weight[9][256] = {\n'
    for s in range(9):
        hist = Counter(d % (1 << s) for d in ds)
        header += '{'+','.join(str(hist.get(i,0)) for i in range(256))+'},\n'
    header += '};\n'
    Path('checks/prefix_weights.h').write_text(header)
    output = dict(word_bits=W,q=Q,p=P,mask_count=len(ds),
                  mask_rows=[dict(mask=d,patterns=pats[d]) for d in ds],
                  prefix_rows=prefixes,lifting_groups=groups,
                  small_valuation_enh=small_enh,enh_constant=enh,
                  ph_small_valuation=ph_small,ph_convolution=convolution,
                  ph_small_convolution=small_convolution,
                  ph_constant=ph,block_constant=C,identity_bound_at_least_one_long=frac(A),
                  envelope_constant=423,envelope_unrounded=frac(raw_constant),
                  epsilon_at_L2=frac(eps2),score_ratio=frac(ratio),
                  score_decimal_display=str(score),score_exact_interval=['53.38','53.39'],
                  tested_lengths=len(lengths),
                  sharp_primary_projection_proved=False,published_constant_refuted=False)
    Path('checks/primary_certificate.json').write_text(json.dumps(output,indent=2)+'\n')
    print(json.dumps({k:v for k,v in output.items() if k not in
                      ('mask_rows','prefix_rows','lifting_groups','ph_convolution','ph_small_convolution')},indent=2))

if __name__ == '__main__':
    main()
