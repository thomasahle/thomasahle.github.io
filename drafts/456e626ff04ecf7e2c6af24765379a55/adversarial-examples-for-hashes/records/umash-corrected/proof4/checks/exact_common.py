"""Shared exact definitions for the round-4 certificates (standard library)."""
from functools import lru_cache
from fractions import Fraction
Q=1<<64
P=(1<<61)-1
@lru_cache(None)
def supports(n,w):
    if abs(n)>=1<<w:return frozenset()
    if not w:return frozenset([0]) if n==0 else frozenset()
    if n%2==0:return frozenset(2*x for x in supports(n//2,w-1))
    return frozenset(2*x+1 for a in ((n-1)//2,(n+1)//2) for x in supports(a,w-1))
def signed_masks(w,p):
    return sorted(set().union(*(supports(j*p,w) for j in range(((1<<w)-1)//p+1))))
def patterns(m,w=64,p=P):
    limit=((1<<w)-1)//p
    return sorted({(m+j*p)//2 for j in range(-limit,limit+1)
                   if m+j*p>=0 and (m+j*p)%2==0
                   and ((m+j*p)//2)&m==(m+j*p)//2})
def label(m,t,r):return m% (1<<(r-1)),(m-2*t)%(1<<r)
def frac(x):return [x.numerator,x.denominator]
def ceil(x):return -(-x.numerator//x.denominator)
