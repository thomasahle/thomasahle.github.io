#!/usr/bin/env python3
import sys,json,platform
from collections import Counter
sys.path.insert(0,'materials')
from certify_primary import signed_masks,patterns
assert platform.system()=='Linux'
w=64;q=1<<w;p=q//8-1
ps=[(m,t) for m in signed_masks(w,p) for t in patterns(m)]
for r in [8,16,24,32,40,48,56,60,61,62,63]:
 rows=[]
 for s in [1,2,3,4,5,6,7,8]:
  if s>=r:continue
  counts=[]
  for l in range(1<<s):
   labs=set()
   for m,t in ps:
    # f on l + 2^s z; ignore common XOR M, absorb it into l.
    const=(m-2*((l&m)^t))% (1<<r)
    bits=m & ((1<<(r-1))-1) & ~((1<<s)-1)
    # original label also retained signs low active bits; const+bits suffices?
    signs=t & bits & ((1<<(r-2))-1)
    labs.add((bits,signs,const))
   counts.append(len(labs))
  rows.append([s,min(counts),max(counts)])
 print(r,rows,flush=True)
