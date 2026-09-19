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
def linear_w(m,j):
 return F(max(Counter(t>>j for t in ps[m]).values()),1<<(m&((1<<j)-1)).bit_count())
@lru_cache(None)
def rank(m,cdeg):
 piv=set()
 for j in range(64):
  if not(m>>j)&1:continue
  n=64+j
  opts=[]
  if n-cdeg<64 and cdeg>=0:opts.append(n-cdeg)
  if n%2==0:opts.append(n//2)
  if len(opts)==2 and opts[0]==opts[1]:continue
  if opts:piv.add(min(opts))
 return len(piv)
def quad_w(m,dd,j):
 if j==0:return F(1)
 td=64-dd+j-1
 if td==dd: cs=range(-1,dd)
 else:cs=[max(td,dd)]
 return max(min(F(1),F(len(ps[m]),1<<rank(m,c))) for c in cs)
def trie_max(items,weights,k=62):
 if not items:return F(0)
 if k<0:return F(1)
 parts=[[m for m in items if (m>>k)&1==b] for b in [0,1]]
 costs=[]
 for b in [0,1]:
  other=sum((weights[m][k+1] for m in parts[1-b]),F())
  costs.append(other+trie_max(parts[b],weights,k-1))
 return max(costs)
wts={m:[linear_w(m,j) for j in range(64)] for m in ds}
print('one word:',[float(trie_max([m for m in ds if m>>63==b],wts)) for b in [0,1]],flush=True)
for dd in range(4,64):
 wts={m:[quad_w(m,dd,j) for j in range(64)] for m in ds}
 vals=[trie_max([m for m in ds if m>>63==b],wts) for b in [0,1]]
 print('dd',dd,'two equal',*[float(v) for v in vals],flush=True)
