#!/usr/bin/env python3
import json,itertools,sys,hashlib

def rank(rows):
    pivots={}
    for x in rows:
        while x:
            p=x.bit_length()-1
            if p not in pivots:
                pivots[p]=x;break
            x^=pivots[p]
    return len(pivots)

results=[]
for line in open(sys.argv[1]):
    a=json.loads(line);n,d,k=a['n'],a['d'],a['k'];rows=a['rows']
    checks=[]
    for mask in range(1<<n):
        survivors=[i for i in range(n) if mask>>i&1]
        r=rank([rows[3*i+j] for i in survivors for j in range(3)])
        checks.append({'survivors':survivors,'rank':r})
        if len(survivors)>=n-k+1: assert r==3*d,(k,survivors,r)
    flips=[sum(any(rows[3*i+j]>>bit&1 for j in range(3)) for i in range(n)) for bit in range(3*d)]
    assert min(flips)==k,(k,flips)
    a.update(subsets=checks,all_subsets_checked=len(checks),critical_subsets=sum(len(c['survivors'])==n-k+1 for c in checks),single_block_flip_distances=flips,minimum_distance=k)
    results.append(a)
json.dump({'method':'Generator rows emitted by the compiled header; GF(2) Gaussian elimination on EVERY survivor subset. Rank 3*d for all >=n-k+1 survivors proves distance >=k; a single-bit input attains k. Bitwise linearity lifts to arbitrary blocks and SIMD lanes.','header_sha256':hashlib.sha256(open('halftime-hash.hpp','rb').read()).hexdigest(),'encoders':results},open('certificates/rank-certificate.json','w'),indent=2)
print(json.dumps([{'k':a['k'],'minimum_distance':a['minimum_distance'],'subsets':a['all_subsets_checked'],'critical_subsets':a['critical_subsets']} for a in results]))
