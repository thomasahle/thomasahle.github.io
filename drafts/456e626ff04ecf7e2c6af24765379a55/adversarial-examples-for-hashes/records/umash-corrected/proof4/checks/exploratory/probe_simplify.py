#!/usr/bin/env python3
import sys,random,json,platform
sys.path.insert(0,'materials')
from certify_primary import signed_masks,patterns,label
assert platform.system()=='Linux'
q=1<<64;p=q//8-1;rng=random.Random(12)
ps=[(m,t) for m in signed_masks(64,p) for t in patterns(m)]
for r in [48,56,60,61,62,63]:
 Q=q>>r; R=1<<r
 reps=list({label(m,t,r):(m,t) for m,t in ps}.values())
 for k in [2,3,4,6,8,12,16]:
  K=1<<k; wmask=K-1;counts=[]
  for sample in range(100):
   A=rng.randrange(K);d=rng.randrange(K)|1;e=rng.randrange(K);M=rng.randrange(K);gap=rng.randrange(32)-16
   inv=pow(d,-1,K);labs=set()
   for m,t in reps:
    B=0
    for _ in range((k+1)//2+1):B=(inv*(Q*(m-2*(((A*B)^M)&m ^t)-gap)-e*A-R*d*e))&wmask
    L=(A*B)^M
    const=(m-2*((L&wmask&m)^t))%R
    mask=m&((R>>1)-1)&~wmask
    sign=t&mask&((R>>2)-1)
    labs.add((B,mask,sign,const))
   counts.append(len(labs))
  print(r,k,max(counts),min(counts),sum(counts)//len(counts),flush=True)
