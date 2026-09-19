#!/usr/bin/env python3
import sys,json,platform
from fractions import Fraction as F
from collections import Counter,defaultdict
sys.path.insert(0,'materials')
from certify_primary import signed_masks,patterns,label
assert platform.system()=='Linux'
q=1<<64;p=q//8-1;ds=signed_masks(64,p);ps={m:patterns(m) for m in ds}
def ceil(x):return -(-x.numerator//x.denominator)
def weight(m):
 # Exact upper pattern probability for a PH high mask: fixed degree j
 # its high word is uniform on [0,2^j); bits >=j are forced zero.
 tot=F(1,q)
 for j in range(64):
  c=Counter(t>>j for t in ps[m])
  tot+=F((1<<j)*max(c.values()),q*(1<<(bin(m&((1<<j)-1)).count("1"))))
 return min(F(1),tot)
def naive(m):
 return min(F(1),len(ps[m])*(F(1,q)+sum(F(1<<j,q*(1<<(bin(m&((1<<j)-1)).count("1")))) for j in range(64))))
one=two=old=F(0)
rows=[]
for m in ds:
 h=bin(m).count("1");t=m>>63
 sparse=1<<(h-t)
 kh=min(sparse,276*(1<<(64-h)),6*((1<<((h+1)//2))+1))
 gap=(1<<m.bit_length())-m if m else q
 kap=min(1<<h,5+(1<<(71-h)),5+ (4*q+gap-256)//(gap-255) if gap>255 else q)
 # zero mask total 1/q
 kt=min(2*len(ps[m]),kh*weight(m)) if m else F(1)
 ko=min(2*len(ps[m]),kap*weight(m)) if m else F(1)
 oldt=min(2*len(ps[m]),min(2*sparse,276*(1<<(64-h)),16*((1<<((h+1)//2))+1))*naive(m)) if m else F(1)
 two+=kt;one+=ko;old+=oldt
 rows.append(dict(mask=m,popcount=h,patterns=len(ps[m]),weight=[weight(m).numerator,weight(m).denominator],KH=kh,kappa=kap,two=[kt.numerator,kt.denominator],one=[ko.numerator,ko.denominator]))
print('high common PH:', 'two',str(two),float(two),ceil(two),'one',float(one),ceil(one),'old',float(old),ceil(old),flush=True)
# Grouped pure ENH, now with necessary actual full-hash multiplier condition.
weighted=[]
for r in range(4,64):
 groups=defaultdict(set)
 for m in ds:
  for t in ps[m]:
   j=(2*t-m)//p
   groups[j].add(label(m,t,r))
 total=F(1)
 for j,gs in groups.items():
  if j==0:continue
  v=(abs(j)&-abs(j)).bit_length()-1
  divisor=1<<max(0,3-v)
  prob=F((p-1)//divisor-(1//divisor),p-2)
  total+=2*len(gs)*prob
 weighted.append(dict(r=r,groups={str(j):len(gs) for j,gs in sorted(groups.items())},constant=[total.numerator,total.denominator]))
print('pure ENH max',max(float(F(*r['constant'])) for r in weighted),'ceil',ceil(max(F(*r['constant']) for r in weighted)),flush=True)
print('small PH excluding odd ENH')
for s in [1,2,3]:
 M=1<<s;hist=Counter(m%M for m in ds);mx=0
 for de in range(0,M,2):
  for ep in range(0,M,2):
   v=F(sum(hist[(a*b%M)^((a+de)*(b+ep)%M)] for a in range(M) for b in range(M)),M)
   mx=max(mx,v)
 print(s,mx)
open('checks/bounds_probe.json','w').write(json.dumps(dict(high_rows=rows,two=[two.numerator,two.denominator],one=[one.numerator,one.denominator],weighted=weighted),indent=2)+'\n')
