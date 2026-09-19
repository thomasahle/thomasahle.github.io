#!/usr/bin/env python3
import sys,platform,json
from fractions import Fraction as F
sys.path.insert(0,'materials')
from certify_primary import signed_masks,patterns,label
assert platform.system()=='Linux'
q=1<<64;p=q//8-1;k=4;K=1<<k
ps=[(m,t) for m in signed_masks(64,p) for t in patterns(m) if m]
nums={j:((p-1)//(1<<max(0,3-((abs(j)&-abs(j)).bit_length()-1))))-(1//(1<<max(0,3-((abs(j)&-abs(j)).bit_length()-1)))) for j in list(range(-8,0))+list(range(1,9))}
def reduced(m,t,r,L):
 R=1<<r;bits=m&((R>>1)-1)&~(K-1)
 return bits,t&bits&((R>>2)-1),(m-2*((L&m)^t))%(1<<r)
out=[]
for r in range(8,61):
 vals=[]
 for L in range(K):
  labs={}
  for m,t in ps:
   key=reduced(m,t,r,L);n=nums[(2*t-m)//p]
   labs[key]=max(n,labs.get(key,0))
  vals.append(sum(labs.values()))
 sums=[sum(vals[(u*A*A)%K] for A in range(K)) for u in range(K)]
 c=1+F(2*max(sums),K*(p-2))
 out.append(dict(r=r,kind='Q_at_least_16',weighted_by_L=vals,by_u=sums,constant=[c.numerator,c.denominator],u=sums.index(max(sums))))
 print(r,float(c),flush=True)
for r in [61,62,63]:
 Q=q>>r;R=1<<r
 reps=list({label(m,t,r):(m,t) for m,t in ps}.values())
 best=(-1,None);tables=[]
 for D in range(1,Q,2):
  for E in range(Q):
   for gap in range(K//Q):
    total=0
    for alpha in range(2):
     d=D-alpha*Q;alen=D if alpha else Q-D
     for beta in ([0] if E==0 else [0,1]):
      e=E-beta*Q
      for A in range(K):
       labs={}
       for m,t in reps:
        B=0
        for bit in range(k):
         residue=d*B+e*A-Q*(m-2*(((A*B)&m)^t)-gap)
         if (residue>>bit)&1:B|=1<<bit
        assert (d*B+e*A-Q*(m-2*(((A*B)&m)^t)-gap))%K==0
        key=(B,)+reduced(m,t,r,(A*B)%K)
        n=nums[(2*t-m)//p]
        labs[key]=max(n,labs.get(key,0))
       total+=alen*sum(labs.values())
    tables.append([D,E,gap,total])
    if total>best[0]:best=(total,[D,E,gap])
 c=1+F(best[0],Q*K*(p-2))
 out.append(dict(r=r,kind='small_Q',table=tables,constant=[c.numerator,c.denominator],witness=best[1]))
 print(r,float(c),best[1],flush=True)
open('checks/tail_prefix_probe.json','w').write(json.dumps(out,indent=2)+'\n')
