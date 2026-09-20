#!/usr/bin/env python3
"""Exact certificates for the ENH triangular-lifting argument.

Run on the specified Xeon, not on the editing machine. Standard library only.
"""
from functools import lru_cache
from fractions import Fraction
from pathlib import Path
import argparse
import json


@lru_cache(None)
def signed_supports(n, w):
    if abs(n) >= 1 << w:
        return frozenset()
    if w == 0:
        return frozenset([0]) if n == 0 else frozenset()
    if n % 2 == 0:
        return frozenset(2*x for x in signed_supports(n//2, w-1))
    return frozenset(2*x+1 for z in ((n-1)//2, (n+1)//2)
                     for x in signed_supports(z, w-1))


def masks(w, p):
    return sorted(set().union(*(signed_supports(j*p, w)
                               for j in range(((1 << w)-1)//p+1))))


def patterns(d, w, p):
    m = ((1 << w)-1)//p
    return sorted({(d+j*p)//2 for j in range(-m, m+1)
                   if d+j*p >= 0 and (d+j*p) % 2 == 0
                   and ((d+j*p)//2) & d == (d+j*p)//2})


def valuation(n):
    return (n & -n).bit_length()-1


def solve_lift(w, delta, epsilon, A, beta, mask, pat, common_high, tag_gap):
    """Unique candidate B modulo q in a prescribed B-wrap branch.

    delta is nonzero and has minimum valuation of the two increments.
    pat is the projected first high word restricted to mask.
    Low equality is a premise. Checks of the full event are separate.
    """
    q = 1 << w
    r = valuation(delta)
    R = 1 << r
    Q = q//R
    alpha = int(A+delta >= q)
    d = (delta-alpha*q)//R
    e = (epsilon-beta*q)//R
    assert d % 2
    B = 0
    for j in range(w):
        z = ((A*B) & mask) ^ (common_high & mask) ^ pat
        residue = d*B+e*A+R*d*e-Q*(mask-2*z-tag_gap)
        if (residue >> j) & 1:
            B |= 1 << j
    z = ((A*B) & mask) ^ (common_high & mask) ^ pat
    assert (d*B+e*A+R*d*e-Q*(mask-2*z-tag_gap)) % q == 0
    return B


def count_actual(w, delta, epsilon, tag, tag2, common_high, p):
    q = 1 << w
    D = masks(w, p)
    allowed = {d:set(patterns(d,w,p)) for d in D}
    count = 0
    checks = 0
    for A in range(q):
        Ap = (A+delta) % q
        for B in range(q):
            Bp = (B+epsilon) % q
            N, Np = A*B, Ap*Bp
            L, Lp = N % q, Np % q
            if L != Lp:
                continue
            U, Up = (N//q+tag) % q, (Np//q+tag2) % q
            X, Y = common_high ^ L ^ U, common_high ^ L ^ Up
            if (X-Y) % p:
                continue
            d = X ^ Y
            t = X & d
            assert d in allowed and t in allowed[d]
            beta = int(B+epsilon >= q)
            candidate = solve_lift(w,delta,epsilon,A,beta,d,t,common_high,tag2-tag)
            assert candidate == B, (w,delta,epsilon,A,B,d,t,candidate)
            count += 1
            checks += 1
    bound = 2*sum(len(v) for v in allowed.values())*q
    assert count <= bound
    return {'width':w,'p':p,'delta':delta,'epsilon':epsilon,
            'tags':[tag,tag2],'common_high':common_high,
            'key_fibre_count':count,'denominator':q*q,'lift_checks':checks}


def certify64():
    w = 64
    q, p = 1 << w, (1 << 61)-1
    D = masks(w,p)
    rows = [{'mask':d,'patterns':patterns(d,w,p)} for d in D]
    totals = [sum(len(row['patterns']) for row in rows
                  if row['mask'] % (1 << r) == 0) for r in range(5)]
    low_bounds = [(1 << r)*totals[r] for r in range(4)]
    high_bound = 2*totals[0]
    joint_num = high_bound*len(D)
    assert joint_num < (1 << 41)
    parity = [sum(d % 2 == b for d in D) for b in (0,1)]
    topbit = [sum(d >> 63 == b for d in D) for b in (0,1)]
    ph_even = max(parity)*max(topbit)
    tag_bound = 16*255*66
    C = max(ph_even, 17, high_bound, *low_bounds, tag_bound, 82)
    root_slope = Fraction(32,p-2)
    proposed = (Fraction(C,q-561)+root_slope)*(p+1)
    rounded = -(-proposed.numerator//proposed.denominator)
    return {'word_bits':w,'p':p,'q':q,'mask_count':len(D),
            'pattern_counts_by_min_valuation':totals,
            'small_r_primary_numerators':low_bounds,
            'low_equal_primary_numerator':high_bound,
            'enh_only_high_r_joint_numerator':joint_num,
            'mask_parity_counts':parity,
            'mask_topbit_counts':topbit,
            'even_ph_primary_numerator':ph_even,
            'odd_ph_primary_numerator':17,
            'tag_primary_numerator':tag_bound,
            'primary_block_numerator':C,
            'corrected_full_hash_constant':rounded,
            'full_constant_unrounded':[proposed.numerator,proposed.denominator],
            'rows':rows}


def main():
    parser=argparse.ArgumentParser()
    parser.add_argument('--out',default='checks/triangular.json')
    args=parser.parse_args()
    cases=[]
    for w,p in [(4,3),(5,3),(6,7),(8,31)]:
        q=1 << w
        for delta,epsilon in [(q//2,q//2),(q//4,q//2),(q//4,3*q//4),
                              (q//8,q//4),(q//2,0),(2,6),(1,3)]:
            for tag,tag2,M in [(0,0,0),(q-1,0,q//3),(q//2-1,q//2,q-1)]:
                cases.append(count_actual(w,delta,epsilon,tag,tag2,M,p))
    result=certify64()
    result['scaled_exact_cases']=cases
    result['scaled_total_key_pairs']=sum(v['denominator'] for v in cases)
    result['scaled_total_lift_checks']=sum(v['lift_checks'] for v in cases)
    Path(args.out).write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps({k:v for k,v in result.items() if k not in ('rows','scaled_exact_cases')},indent=2))


if __name__ == '__main__':
    main()
