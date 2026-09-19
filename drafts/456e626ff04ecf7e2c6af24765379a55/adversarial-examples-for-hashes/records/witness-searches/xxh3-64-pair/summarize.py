#!/usr/bin/env python3
"""Pool 2^30 runs: rate, exact (Clopper-Pearson) 95% CI on epsilon, resulting bits = log2(L/eps), L=4."""
import sys, re, math
from scipy.stats import beta
def cp(k,n,a=0.05):
    lo = 0.0 if k==0 else beta.ppf(a/2,k,n-k+1)
    hi = 1.0 if k==n else beta.ppf(1-a/2,k+1,n-k)
    return lo,hi
L=4; tot=0; totn=0
for f in sys.argv[1:]:
    txt=open(f).read()
    m=re.search(r'summary len=32 L=4 collisions=(\d+) trials=(\d+)',txt)
    salt=re.search(r'salt=0x([0-9a-f]+)',txt).group(1)
    chk=re.search(r'checks .* (PASS|FAIL)',txt).group(1)
    k,n=int(m.group(1)),int(m.group(2)); tot+=k; totn+=n
    lo,hi=cp(k,n)
    print(f"{f}: salt=0x{salt} {k}/{n} = 2^{math.log2(k/n):.4f}  95% CI eps [2^{math.log2(lo):.4f}, 2^{math.log2(hi):.4f}]  bits {math.log2(L*n/k):.4f} [{math.log2(L/hi):.4f}, {math.log2(L/lo):.4f}]  checks {chk}")
lo,hi=cp(tot,totn)
print(f"pooled: {tot}/{totn} = 2^{math.log2(tot/totn):.4f}  95% CI eps [2^{math.log2(lo):.4f}, 2^{math.log2(hi):.4f}]  bits {math.log2(L*totn/tot):.4f} [{math.log2(L/hi):.4f}, {math.log2(L/lo):.4f}]")
