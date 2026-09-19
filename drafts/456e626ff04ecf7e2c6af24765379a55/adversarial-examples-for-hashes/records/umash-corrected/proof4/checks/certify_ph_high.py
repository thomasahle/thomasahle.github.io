#!/usr/bin/env python3
import sys,platform,json
from functools import lru_cache
from fractions import Fraction as F
from exact_common import signed_masks,patterns
assert platform.system()=='Linux'
q=1<<64;p=q//8-1;ds=signed_masks(64,p);ps={m:patterns(m) for m in ds}
@lru_cache(None)
def pivots(N,d,E,O):
 out=[]
 for j in range(64):
  n=64+j
  if n<d:out.append(-1);continue
  mul=n-d if n-d<N else 129
  H=E if n%2==0 else O
  sq=(n-H)//2 if H>=0 and 0<=n-H<2*N else 129
  if mul==sq:out.append(-1)
  else:out.append(min(mul,sq))
 return tuple(out)
def rank(m,N,d,E,O):
 pp=pivots(N,d,E,O)
 return len({pp[j] for j in range(64) if (m>>j)&1 and 0<=pp[j]<N})
def trie(ix,ws,k=62):
 if not ix:return F()
 if k<0:return F(1)
 part=[[m for m in ix if (m>>k)&1==b] for b in [0,1]]
 return max(sum((ws[m][k+1] for m in part[1-b]),F())+trie(part[b],ws,k-1) for b in [0,1])
out=[]
for t in [0,1]:
 shapes=[(0,0,-1)] if t==0 else [(1,-1,1),(1,0,1),(2,2,1)]
 for D in range(4,64-t):
  ws={}
  for m in ds:
   row=[F(1)]
   for j in range(1,64):
    td=64-D+j-1;mr=64
    if td>=64+t:
     row.append(F(0));continue
    for hdeg,E,O in shapes:
     base=D+hdeg
     degg=range(-1,base) if td==base else [max(td,base)]
     mr=min(mr,*(rank(m,64-t,d,E,O) for d in degg))
    row.append(min(F(1),F(len(ps[m]),1<<mr)))
   ws[m]=row
  vals=[trie([m for m in ds if m>>63==b],ws)/(1<<t) for b in [0,1]]
  out.append(dict(t=t,degree_g=D,bounds=[[x.numerator,x.denominator] for x in vals]))
  print(t,D,*[float(x) for x in vals],flush=True)
open('checks/ph_high_certificate.json','w').write(json.dumps(out,indent=2)+'\n')
