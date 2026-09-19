#!/usr/bin/env python3
import sys,platform,json
from functools import lru_cache
from fractions import Fraction as F
from collections import Counter
sys.path.insert(0,'materials')
from certify_primary import signed_masks,patterns
assert platform.system()=='Linux'
q=1<<64;p=q//8-1;ds=signed_masks(64,p);ps={m:patterns(m) for m in ds}
def ceil(x):return -(-x.numerator//x.denominator)
@lru_cache(None)
def rankmask(N,v,E,O):
 # rows with distinct known highest nonzero input; return bit-mask of outputs
 used=set(); selected=0
 # Rank per mask must count distinct pivots among its bits, so return map.
 piv=[]
 for n in range(64):
  if n>=N+v: piv.append(-1);continue
  mu=n-v if n>=v else -1
  h=E if n%2==0 else O
  sq=(n-h)//2 if n>=h else -1
  if mu==sq:piv.append(-1)
  else:piv.append(max(mu,sq))
 return tuple(piv)
def rank(m,N,v,E,O):
 piv=rankmask(N,v,E,O)
 return len({piv[j] for j in range(64) if (m>>j)&1 and piv[j]>=0})
def trie(items,weights,s,k=None):
 if not items:return F(0)
 if k is None:k=s
 if k>=64:return F(1)
 parts=[[m for m in items if (m>>k)&1==b] for b in [0,1]]
 return max(sum((weights[m][k] for m in parts[1-b]),F())+trie(parts[b],weights,s,k+1) for b in [0,1])
for s in [1,2,3]:
 def linear(m,k):
  v=k-s
  lowmask=(1<<v)-1
  return F(max(Counter(t&lowmask for t in ps[m]).values()),1<<(m>>v).bit_count())
 ws={m:{k:linear(m,k) for k in range(s,64)} for m in ds}
 maxv=max(trie([m for m in ds if m%(1<<s)==a],ws,s) for a in range(1<<s))*(1<<s)
 print('linear',s,float(maxv),flush=True)
for s in [1,2,3]:
 for t in range(0,12):
  pairs=[(E,O) for E in list(range(0,2*t+1,2))+[128] for O in list(range(1,2*t+1,2))+[129] if min(E,O)<=t]
  if t==0:pairs=[(0,129)]
  # Safe bound: maximize each individual weight over all parity-leading patterns.
  ws={}
  for m in ds:
   weights={}
   for k in range(s,64):
    vt=k-s;minrank=64
    for E,O in pairs:
     base=s+min(E,O)
     vset=range(base+1,65+t) if vt==base else [min(vt,base)]
     rr=min(rank(m,64-t,v,E,O) for v in vset)
     minrank=min(minrank,rr)
    weights[k]=min(F(1),F(len(ps[m]),1<<minrank))
   ws[m]=weights
  maxv=max(trie([m for m in ds if m%(1<<s)==a],ws,s) for a in range(1<<s))*(1<<s)
  print('quadratic',s,t,float(maxv),ceil(maxv),flush=True)
