#!/usr/bin/env python3
"""Independent mask enumeration, primality, and exact final inequalities."""
from collections import Counter
from fractions import Fraction
from math import gcd, isqrt, prod
from pathlib import Path
import json
import triangular


def carry_masks(w, p):
    result=set()
    counts=[]
    for k in range(((1 << w)-1)//p+1):
        n=k*p
        states={(0,0)}
        for i in range(w):
            states={(s//2, mask|((a^(s&1)) << i))
                    for carry,mask in states for a in (0,1)
                    for s in (a+((n >> i)&1)+carry,)}
        masks={m for carry,m in states if carry==0}
        counts.append(len(masks))
        result.update(masks)
    return sorted(result),counts


def prime_by_trial(n):
    return n>=2 and all(n%d for d in range(2,isqrt(n)+1))


def primality_certificate():
    p=(1 << 61)-1
    n=p-1
    factors={}
    d=2
    while d*d<=n:
        while n%d==0:
            factors[d]=factors.get(d,0)+1
            n//=d
        d+=1
    if n>1:
        factors[n]=factors.get(n,0)+1
    assert prod(ell**a for ell,a in factors.items())==p-1
    assert all(prime_by_trial(ell) for ell in factors)
    g=2
    while not (pow(g,p-1,p)==1 and all(gcd(pow(g,(p-1)//ell,p)-1,p)==1 for ell in factors)):
        g+=1
    rows=[{'prime':ell,'power':a,'residue':pow(g,(p-1)//ell,p),
           'gcd':gcd(pow(g,(p-1)//ell,p)-1,p)} for ell,a in factors.items()]
    return {'p':p,'witness':g,'fermat_residue':pow(g,p-1,p),'factors':rows}


def ratio(x):
    x=Fraction(x)
    return {'numerator':x.numerator,'denominator':x.denominator}


def main():
    root=Path(__file__).resolve().parent
    cert=json.loads((root/'triangular.json').read_text())
    q,p=cert['q'],cert['p']
    D,counts=carry_masks(64,p)
    assert D==[row['mask'] for row in cert['rows']]
    assert D==triangular.masks(64,p)
    for row in cert['rows']:
        assert row['patterns']==triangular.patterns(row['mask'],64,p)
    assert len(D)==852
    histogram=dict(sorted(Counter(len(row['patterns']) for row in cert['rows']).items()))
    assert histogram=={1:1,2:435,4:357,8:59}
    assert cert['pattern_counts_by_min_valuation']==[2771,615,127,3,1]
    assert cert['mask_parity_counts']==[248,604]
    assert cert['mask_topbit_counts']==[248,604]
    assert min(d.bit_length()-1 for d in D if d)==60
    assert [d for d in D if d%16==0]==[0]
    small=[]
    for w,p0 in [(4,1),(5,3),(6,7),(8,31),(10,127),(12,509)]:
        dm,cc=carry_masks(w,p0)
        brute={x^y for x in range(1 << w) for y in range(x,1 << w,p0)}
        assert set(dm)==brute==set(triangular.masks(w,p0))
        for d in dm:
            pats={x&d for x in range(1 << w) if (x-(x^d))%p0==0}
            assert pats==set(triangular.patterns(d,w,p0))
        small.append({'w':w,'p':p0,'mask_count':len(dm),'all_pairs_and_patterns':True})
    C=364816
    assert C==604**2==cert['primary_block_numerator']
    assert 2*2771==5542
    assert 5542*852==4721784
    assert 16*255*66==269280<C
    assert 34*33//2==561
    A=Fraction(C,q-561)
    coefficient=(A+Fraction(32,p-2))*(p+1)
    assert 45634<coefficient<45635
    assert cert['corrected_full_hash_constant']==45635
    assert [coefficient.numerator,coefficient.denominator]==cert['full_constant_unrounded']
    joint_num=4721784
    assert Fraction(joint_num,q*q)<Fraction(1,1 << 105)<Fraction(1,1 << 87)
    assert Fraction(joint_num,q*(q-561))<Fraction(1,1 << 105)
    Nr=[sum(d%(1 << r)==0 for d in D) for r in range(5)]
    assert Nr==[852,248,64,2,1]
    jr=[cert['small_r_primary_numerators'][r]*Nr[r]*(1 << r) for r in range(4)]
    assert jr==[2360892,610080,130048,384]
    assert max(jr)<joint_num
    output={'status':'PASS','mask_counts_per_positive_multiple':counts,
            'mask_pattern_histogram':histogram,'mask_divisibility_counts':Nr,
            'small_mask_cross_checks':small,'primality':primality_certificate(),
            'block_primary_numerator':C,'iid_block_primary':ratio(Fraction(C,q)),
            'distinct_block_primary':ratio(A),
            'low_equal_enh_primary':ratio(Fraction(5542,q)),
            'enh_only_joint':ratio(Fraction(joint_num,q*q)),
            'enh_only_joint_after_distinctness':ratio(Fraction(joint_num,q*(q-561))),
            'enh_only_joint_small_r_numerators':jr,
            'full_constant_unrounded':ratio(coefficient),
            'full_constant_ceiling':45635,
            'slack_below_ceiling':ratio(45635-coefficient),
            'corrected_vs_published_envelope_factor':ratio(Fraction(45635,64)),
            'old_over_new_block_numerator':ratio(Fraction(718333281557,C))}
    (root/'constants.json').write_text(json.dumps(output,indent=2)+'\n')
    print(json.dumps({k:v for k,v in output.items() if k not in ('small_mask_cross_checks','primality')},indent=2))


if __name__=='__main__': main()
