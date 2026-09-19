#!/usr/bin/env python3
"""Independent reader: no import of the production certificate generator."""
from collections import defaultdict
from fractions import Fraction
from pathlib import Path
import json
import platform

assert platform.system() == 'Linux'
c = json.loads(Path('checks/primary_certificate.json').read_text())
q,p = 2**64,2**61-1
rows = c['mask_rows']
assert len({r['mask'] for r in rows}) == len(rows) == 852
total = 0
full_pairs = q+2*sum(q-j*p for j in range(1,9))
for row in rows:
    m = row['mask']
    assert 0 <= m < q
    wanted = []
    for j in range(-8,9):
        z = m+j*p
        if z >= 0 and z % 2 == 0 and (z//2) & m == z//2:
            wanted.append(z//2)
    assert sorted(set(wanted)) == row['patterns']
    total += len(wanted)*2**(64-m.bit_count())
assert total == full_pairs == 8*q+72
# Every certified pattern is valid and pair sets are disjoint. Matching the
# count of ALL congruent pairs proves completeness without either generator.
counts=[]
for r in range(4,64):
    R = 2**r
    signatures={}
    for row in rows:
        m = row['mask']
        for t in row['patterns']:
            # Literal function values at zero and the bit vectors recover all
            # integer affine coefficients in the binary digits of the input.
            const = (m-2*t) % R
            sig = (const,) + tuple((m-2*((m & 2**j)^t)-const)%R for j in range(r))
            signatures.setdefault(sig, []).append((m,t))
    recorded=c['lifting_groups'][r-4]
    assert len(signatures)==recorded['classes']
    assert recorded['constant']==2*len(signatures)-1
    assert signatures[(0,)*(r+1)]==[(0,0)]
    represented=set()
    for representative in recorded['representatives']:
        m,t=representative['mask'],representative['pattern']
        const=(m-2*t)%R
        sig=(const,)+tuple((m-2*((m & 2**j)^t)-const)%R for j in range(r))
        assert (m,t) in signatures[sig]
        assert representative['multiplicity']==len(signatures[sig])
        assert representative['label']==[m%(R//2),const]
        assert sig not in represented
        represented.add(sig)
    assert represented==set(signatures)
    counts.append(len(signatures))
assert max(counts)==1563

small_convolution_checks=0
for record in c['ph_small_convolution']:
    s=record['s']; M=2**s
    prefix_counts=[sum(row['mask']%M==i for row in rows) for i in range(M)]
    values=[]
    for entry in record['rows']:
        d,e=entry['delta'],entry['epsilon']
        exact=0
        for a in range(M):
            for b in range(M):
                first=(a*b)%M
                second=(((a+d)%M)*((b+e)%M))%M
                exact+=prefix_counts[first^second]
        assert exact==entry['weighted_count']
        values.append(exact); small_convolution_checks+=1
    assert len(values)==M*M and max(values)==record['maximum']==852*M
for row in c['ph_convolution']:
    s=row['s']
    assert row['by_enh_valuation']==[852,1060+s,512+s,24]
assert max(v for row in c['ph_convolution'] for v in row['by_enh_valuation'])==1123

scaled_functions=0
for w in range(5,10):
    Q=2**w; P=2**(w-3)-1
    pats=defaultdict(set)
    for x in range(Q):
        for y in range(x%P,Q,P): pats[x^y].add(x&(x^y))
    for r in range(4,w):
        R=2**r
        for common in (0,Q//3,Q-1):
            by_label={}; by_values={}
            for m,ps in pats.items():
                for t in ps:
                    label=(m%(R//2),(m-2*t)%R)
                    values=tuple((m-2*(((x^common)&m)^t))%R for x in range(R))
                    if label in by_label: assert by_label[label]==values
                    if values in by_values: assert by_values[values]==label
                    by_label[label]=values; by_values[values]=label
                    scaled_functions+=1

C=3125
A=Fraction(C,q-561)
unrounded=2**61*(A+Fraction(32,p-2))
eps2=A+(1-A)*Fraction(2,p-2)
score_ratio=2/eps2
assert c['block_constant']==C and c['ph_constant']==1123
assert c['envelope_unrounded']==[unrounded.numerator,unrounded.denominator]
assert 422<unrounded<423
assert c['epsilon_at_L2']==[eps2.numerator,eps2.denominator]
assert score_ratio**50>2**2669 and score_ratio**100<2**5339
assert c['sharp_primary_projection_proved'] is False
assert c['published_constant_refuted'] is False
print(json.dumps(dict(mask_completeness_pairs=full_pairs,
                     independently_verified_group_counts=counts,
                     scaled_literal_function_checks=scaled_functions,
                     small_convolution_checks=small_convolution_checks,
                     exact_score_interval=['53.38','53.39'],failures=0),indent=2))
