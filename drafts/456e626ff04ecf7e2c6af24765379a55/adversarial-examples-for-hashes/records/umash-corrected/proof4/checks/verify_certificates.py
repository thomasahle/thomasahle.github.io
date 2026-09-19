#!/usr/bin/env python3
"""Independent reader. Does not import any generator's mask/rank/group functions."""
import json,platform
from collections import Counter,defaultdict
from fractions import Fraction as F
assert platform.system()=='Linux'
q=1<<64;p=(1<<61)-1
z=json.load(open('checks/mask_certificate.json'))
rows=z['rows'];ds=[v['mask'] for v in rows];ps={v['mask']:v['patterns'] for v in rows}
assert len(ds)==len(set(ds))==852
mass=0
for m in ds:
    assert len(ps[m])==len(set(ps[m]))
    for t in ps[m]:
        assert 0<=t<q and t&m==t
        assert (2*t-m)%p==0 and abs(2*t-m)<=8*p
        mass+=1<<(64-m.bit_count())
assert mass==q+2*sum(q-j*p for j in range(1,9))==8*q+72
# Each valid (m,t) accounts for a disjoint set of ordered congruent pairs;
# equality with the independently counted total proves table completeness.
lo=json.load(open('checks/ph_low_table.json')); ind=json.load(open('checks/ph_low_independent.json'))
lt={(r['s'],r['t']):r for r in lo['rows']};scale=int(lo['scale']);assert scale==q==int(ind['scale'])
for r in ind['rows']:
    assert int(r['numerator'])==int(lt[r['s'],r['t']]['by_prefix'][r['prefix']])
# All tables used at small PH valuations are below 147 after the rank bound.
maxsmall=F()
for (s,t),r in lt.items():
    evenmax=max(F(int(n),q) for a,n in enumerate(r['by_prefix']) if a%2==0)
    bound=min(evenmax,F(248*604,1<<t))
    maxsmall=max(maxsmall,bound)
assert maxsmall==F(4681,32)<147
lin=json.load(open('checks/ph_linear_certificate.json'))
assert max(F(*v) for r in lin['low'] for v in r['bounds'])<85
assert max(F(*v) for v in lin['high'])<=F(23,2)
# Independent high-row construction from literal monomials S and h.
rankcache={}
def high_rank(m,t,d,h):
    key=t,d,h
    if key not in rankcache:
        N=64-t;mat=[0]*64
        for i in range(N):
            polynomial=(0 if d<0 else 1<<(d+i)) ^ (h<<(2*i))
            high=polynomial>>64
            for j in range(64):
                if high>>j&1:mat[j]|=1<<i
        rankcache[key]=[None if (64+j<d or not row) else (row&-row).bit_length()-1 for j,row in enumerate(mat)]
    leads=rankcache[key]
    return len({leads[j] for j in range(64) if m>>j&1 and leads[j] is not None})
def high_tree(ix,weights,k):
    if not ix:return F()
    if k<0:return F(1)
    a=[m for m in ix if not(m>>k&1)];b=[m for m in ix if m>>k&1]
    return max(sum((weights[m][k+1] for m in b),F())+high_tree(a,weights,k-1),
               sum((weights[m][k+1] for m in a),F())+high_tree(b,weights,k-1))
hi=json.load(open('checks/ph_high_certificate.json'));highmax=F()
for row in hi:
    t,D=row['t'],row['degree_g'];weights={}
    hs=[1] if t==0 else [2,3,6]
    for m in ds:
        w=[F(1)]
        for j in range(1,64):
            td=64-D+j-1
            if td>=64+t:w.append(F());continue
            rank=64
            for h in hs:
                c=D+h.bit_length()-1
                degrees=range(-1,c) if c==td else [max(c,td)]
                rank=min(rank,*(high_rank(m,t,d,h) for d in degrees))
            w.append(min(F(1),F(len(ps[m]),1<<rank)))
        weights[m]=w
    result=[high_tree([m for m in ds if m>>63==b],weights,62)/(1<<t) for b in range(2)]
    assert result==[F(*v) for v in row['bounds']]
    highmax=max(highmax,*result)
assert highmax<27
# PH high-product law: conditional degree j gives a uniform j-bit word.
enh=json.load(open('checks/enh_certificate.json'));one=two=F()
for row in enh['rows']:
    m=row['mask'];h=m.bit_count()
    W=F(1,q)
    for j in range(64):
        high_patterns={t>>j for t in ps[m]}
        multiplicity=max(sum(t>>j==u for t in ps[m]) for u in high_patterns)
        W+=F(1<<j,q)*F(multiplicity,1<<(m&((1<<j)-1)).bit_count())
    W=min(1,W);assert W==F(*row['weight'])
    KH=min(1<<(h-(m>>63)),6*((1<<((h+1)//2))+1),276*(1<<(64-h)))
    g=(1<<m.bit_length())-m if m else q
    kap=min(1<<h,5+(1<<(71-h)),5+(4*q+g-256)//(g-255) if g>255 else q)
    assert KH==row['KH'] and kap==row['kappa']
    a=min(2*len(ps[m]),KH*W) if m else F(1)
    b=min(2*len(ps[m]),kap*W) if m else F(1)
    assert a==F(*row['two']) and b==F(*row['one'])
    two+=a;one+=b
assert two==F(*enh['two'])<205 and one==F(*enh['one'])<134
print('Masks, independent low trie, literal high rows and ENH weights passed.',flush=True)
# Independent restricted functions: literal affine coefficient signatures.
allpairs=[(m,t) for m in ds if m for t in ps[m]]
def fn(m,t,x,R):return (m-2*((x&m)^t))%R
def mulcount(m,t):
    j=abs((2*t-m)//p);divisor=8//(j&-j)
    return (p-1)//divisor-1//divisor
counts={(m,t):mulcount(m,t) for m,t in allpairs}
tail=json.load(open('checks/tail_certificate.json'));tailmax=F()
for row in tail:
    r=row['r'];R=1<<r;Q=q>>r
    signatures={(m,t):tuple((fn(m,t,1<<j,R)-fn(m,t,0,R))%R for j in range(4,r)) for m,t in allpairs}
    if Q>=16:
        values=[]
        for L in range(16):
            table={}
            for m,t in allpairs:
                key=(fn(m,t,L,R),signatures[m,t]);table[key]=max(table.get(key,0),counts[m,t])
            values.append(sum(table.values()))
        totals=[sum(values[u*a*a%16] for a in range(16)) for u in range(16)]
        assert values==row['weighted_by_L'] and totals==row['by_u']
        result=1+F(2*max(totals),16*(p-2))
    else:
        fsmall={(m,t):tuple(fn(m,t,l,16) for l in range(16)) for m,t in allpairs}
        table=[]
        for D in range(1,Q,2):
            for E in range(Q):
                for gap in range(16//Q):
                    total=0
                    for alpha in range(2):
                        d=D-alpha*Q;length=D if alpha else Q-D
                        for beta in ([0] if E==0 else [0,1]):
                            e=E-beta*Q
                            for A in range(16):
                                solved={};groups={}
                                for m,t in allpairs:
                                    ff=fsmall[m,t]
                                    if ff not in solved:
                                        bs=[B for B in range(16) if (d*B+e*A-Q*(ff[A*B%16]-gap))%16==0]
                                        assert len(bs)==1;solved[ff]=bs[0]
                                    B=solved[ff];key=(B,fn(m,t,A*B%16,R),signatures[m,t])
                                    groups[key]=max(groups.get(key,0),counts[m,t])
                                total+=length*sum(groups.values())
                    table.append([D,E,gap,total])
        assert table==row['table']
        result=1+F(max(x[3] for x in table),Q*16*(p-2))
    assert result==F(*row['constant']) and result<435
    tailmax=max(tailmax,result)
print('Independent tail signatures and literal 16-residue solvers passed.',flush=True)
validation=json.load(open('checks/validation.json'));assert validation['failures']==0
out=dict(mask_completeness=True,low_table_rows=len(ind['rows']),ph_small_max=[maxsmall.numerator,maxsmall.denominator],
         ph_high_max=[highmax.numerator,highmax.denominator],enh_two=[two.numerator,two.denominator],enh_one=[one.numerator,one.denominator],
         tail_max=[tailmax.numerator,tailmax.denominator],tail_rows=len(tail),validation=validation,failures=0)
open('checks/independent_verification.json','w').write(json.dumps(out,indent=2)+'\n')
