#!/usr/bin/env python3
import sys,json,random,platform
from collections import Counter
sys.path.insert(0,'materials')
from certify_primary import signed_masks,patterns,label
assert platform.system()=='Linux'
w=64;q=1<<w;p=q//8-1; rng=random.Random(41019)
ds=signed_masks(w,p); pats={m:patterns(m) for m in ds}
out=[]
for r in [8,16,24,32,40,48,56,60,61,62,63]:
 R=1<<r; Q=q//R
 reps={label(m,t,r):(m,t) for m in ds for t in pats[m]}
 rows=[]
 for case in range(12):
  delta=R*(rng.randrange(Q)//2*2+1);eps=R*rng.randrange(Q)
  A=rng.randrange(q); M=[0,q-1,rng.randrange(q)][case%3]; gap=0
  Ap=(A+delta)%q;d=(Ap-A)//R;di=pow(d,-1,q)
  Bs=set(); good=set(); branchcounts=[]
  for beta in [0,1]:
   e=eps//R-beta*Q;base=-e*Ap
   bs=set()
   for m,t in reps.values():
    B=0
    for it in range((w+ (w-r))//(w-r+1)+1):
     B=(di*(base+Q*(m-2*(((A*B)^M)&m ^t)-gap)))%q
    assert (d*B+e*Ap-Q*(m-2*(((A*B)^M)&m ^t)-gap))%q==0
    if (B+eps)//q!=beta:continue
    bs.add(B);Bs.add(B)
    N=A*B;Np=Ap*((B+eps)%q)
    if N%q!=Np%q:raise Exception('low')
    if ((N//q)^ (N%q)^M)%p==((Np//q)^(Np%q)^M)%p:good.add(B)
   branchcounts.append(len(bs))
  rows.append(dict(case=case,A=A,delta=delta,epsilon=eps,common=M,candidates=len(Bs),actual=len(good),branches=branchcounts))
 out.append(dict(r=r,functions=len(reps),rows=rows))
 print(r,len(reps),max(x['candidates'] for x in rows),sum(x['candidates'] for x in rows)//len(rows),max(x['actual'] for x in rows),flush=True)
open('checks/candidate_probe.json','w').write(json.dumps(out,indent=2)+'\n')
