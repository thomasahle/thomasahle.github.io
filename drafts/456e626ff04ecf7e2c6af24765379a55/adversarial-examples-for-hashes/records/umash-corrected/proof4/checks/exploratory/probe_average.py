#!/usr/bin/env python3
import sys,platform,json
sys.path.insert(0,'materials')
from certify_primary import signed_masks,patterns,label
from fractions import Fraction
assert platform.system()=='Linux'
q=1<<64;p=q//8-1;ps=[(m,t) for m in signed_masks(64,p) for t in patterns(m)]
def ct(reps,r,k,A,d,e,gap,Q,M=0):
 R=1<<r;K=1<<k;wm=K-1; inv=pow(d,-1,K); labs=set()
 for m,t in reps:
  B=0
  for _ in range(k//2+1):B=(inv*(Q*(m-2*((((A*B)^M)&m)^t)-gap)-e*A-R*d*e))&wm
  const=(m-2*(((((A*B)^M)&wm)&m)^t))%R
  mask=m&((R>>1)-1)&~wm;sign=t&mask&((R>>2)-1)
  labs.add((B,mask,sign,const))
 return len(labs)
for r in [48,56,60]:
 k=min(8,64-r);R=1<<r;Q=q>>r;K=1<<k
 reps=list({label(m,t,r):(m,t) for m,t in ps}.values())
 counts=[]
 for l in range(K):
  labs=set()
  for m,t in reps:
   c=(m-2*((l&m)^t))%R
   mask=m&((R>>1)-1)&~(K-1);sign=t&mask&((R>>2)-1)
   labs.add((mask,sign,c))
  counts.append(len(labs))
 rows=[sum(counts[u*A*A%K] for A in range(K)) for u in range(K)]
 print(r,k,'maxmean',max(rows)/K,'arg',rows.index(max(rows)),flush=True)
for r in [61,62,63]:
 Q=q>>r;R=1<<r;k=4;K=1<<k
 reps=list({label(m,t,r):(m,t) for m,t in ps}.values())
 best=(-1,None)
 for delta in range(1,Q,2):
  for eps in range(Q):
   for gap in range(16):
    numer=0
    for alpha in range(2):
     d=delta-alpha*Q;alen=delta if alpha else Q-delta
     for beta in ([0] if eps==0 else [0,1]):
      e=eps-beta*Q
      numer+=alen*sum(ct(reps,r,k,A,d,e,gap,Q) for A in range(K))
    if numer>best[0]:best=(numer,[delta,eps,gap])
 print(r,k,'maxC',best[0]/(Q*K),best[1],flush=True)
