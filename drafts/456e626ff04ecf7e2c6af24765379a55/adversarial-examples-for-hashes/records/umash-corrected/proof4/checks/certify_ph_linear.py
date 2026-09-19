#!/usr/bin/env python3
import platform,json
from fractions import Fraction as F
from collections import Counter
from exact_common import *
assert platform.system()=='Linux'
ds=signed_masks(64,P);ps={m:patterns(m) for m in ds}
def trie(items,weights,k,step,last):
    if not items:return F()
    if k==last:return F(1)
    part=[[m for m in items if (m>>k)&1==b] for b in [0,1]]
    return max(sum((weights[m][k] for m in part[1-b]),F())+trie(part[b],weights,k+step,step,last) for b in [0,1])
low=[]
for s in [1,2,3]:
    ws={m:{k:F(max(Counter(t&((1<<(k-s))-1) for t in ps[m]).values()),1<<(m>>(k-s)).bit_count()) for k in range(s,64)} for m in ds}
    vals=[(1<<s)*trie([m for m in ds if m%(1<<s)==a],ws,s,1,64) for a in range(1<<s)]
    assert max(vals)<85
    low.append(dict(s=s,bounds=[frac(x) for x in vals]))
ws={m:{k:F(max(Counter(t>>(k+1) for t in ps[m]).values()),1<<(m&((1<<(k+1))-1)).bit_count()) for k in range(63)} for m in ds}
high=[trie([m for m in ds if m>>63==b],ws,62,-1,-1) for b in [0,1]]
assert max(high)<=F(23,2)
open('checks/ph_linear_certificate.json','w').write(json.dumps(dict(low=low,high=[frac(x) for x in high]),indent=2)+'\n')
print('linear PH bounds certified')
