# Same enumeration as valsearch.py but with the word-level (signed-digit) cancellation condition in the
# terminal block v3: exists y,y' (32-bit) with y^y' = X3 and y - y' = -D, D = sum of the four transported
# additive differences.  Carry cost: v1 diffs twice, v2 diffs once, v3 none.
import math, json, sys
exec(open('valsearch.py').read().split("TRAILS={")[0])   # reuse tables/functions
TRAILS={'A':dict(P1=[0,10],P2=[1,3],n=56),'B':dict(P1=[8,15],P2=[9,11],n=64)}
THRESH=float(sys.argv[1]) if len(sys.argv)>1 else -19.85   # report candidates with predicted log2 p above this
def feasible(X,T):
    # T == sum_{i in bits(X)} s_i 2^i with s_i in {+1,-1}?
    R={T}
    for i in range(32):
        if (X>>i)&1:
            R={ (r-1)//2 for r in R if r&1 } | { (r+1)//2 for r in R if r&1 }
        else:
            R={ r//2 for r in R if not r&1 }
        if not R: return False
    return 0 in R
def dlist(a,m):
    return sorted(((m*math.log2((256-abs(d))/256),d) for d in COMP[a] if abs(d)<256), key=lambda t:-t[0])
def search(name,T):
    P1,P2=T['P1'],T['P2']
    q=[isrpos(p) for p in P1]; c=q[0]//4; rows=[x%4 for x in q]
    P2rows=[p%4 for p in P2]; rem=[r for r in range(4) if r not in P2rows]
    s3pos={}
    for p in P1: s3pos[shpos(shpos(p))]=('v1',p)
    for p in P2: s3pos[shpos(p)]=('v2',p)
    P3=sorted(s3pos); c3=P3[0]//4
    srcs=[s3pos[pos] for pos in P3]   # byte k of the 32-bit span comes from srcs[k]
    out=[]; ncand=0
    for a_p in range(1,256):
        for b_p in range(1,256):
            if DDT[a_p][b_p]==0: continue
            for rv in rem:
                rr=[r for r in rem if r!=rv][0]
                b_q=[bq for bq in range(1,256) if gmul(IMC[rv][rows[0]],b_p)^gmul(IMC[rv][rows[1]],bq)==0][0]
                col=[0]*4; col[rows[0]]=b_p; col[rows[1]]=b_q
                o=imc_col(col); a3=o[rr]
                pos3=isrpos(4*c+rr)
                if pos3//4!=c3: continue
                r3=pos3%4
                for a_q in range(1,256):
                    if DDT[a_q][b_q]==0: continue
                    t12=DDT[a_p][b_p]*DDT[a_q][b_q]
                    if t12<16: continue
                    for b3 in range(1,256):
                        if DDT[a3][b3]==0: continue
                        tier=t12*DDT[a3][b3]
                        if tier<32: continue
                        ncand+=1
                        col3=[0]*4; col3[r3]=b3
                        d3=imc_col(col3)
                        X3=sum(d3[k]<<(8*k) for k in range(4))
                        x1={P1[0]:a_p,P1[1]:a_q}; x2={p:o[p%4] for p in P2}
                        base=math.log2(tier)-24
                        budget=THRESH-base            # carry bits allowed (negative number)
                        if budget>0: continue          # impossible to be below threshold? no: budget>0 means any carry ok
                        lists=[]
                        for (src,p) in srcs:
                            a = x1[p] if src=='v1' else x2[p]
                            lists.append(dlist(a,2 if src=='v1' else 1))
                        best=None
                        # DFS ordered by cost with pruning
                        def rec(k,acc,ds):
                            nonlocal best
                            if k==4:
                                D=sum(ds[j]<<(8*j) for j in range(4))
                                if feasible(X3,-D):
                                    if best is None or acc>best[0]: best=(acc,list(ds))
                                return
                            for cost,d in lists[k]:
                                if acc+cost<budget: break
                                if best is not None and acc+cost<=best[0]: break
                                rec(k+1,acc+cost,ds+[d])
                        rec(0,0.0,[])
                        if best is not None:
                            out.append(dict(logp=base+best[0],trail=name,tier=tier,a_p=a_p,b_p=b_p,a_q=a_q,b_q=b_q,a3=a3,b3=b3,vanish=rv,
                                            x1={str(k):'%02x'%v for k,v in x1.items()},x2={str(k):'%02x'%v for k,v in x2.items()},
                                            x3={str(P3[k]):'%02x'%d3[k] for k in range(4)},
                                            ds={str(srcs[k][1]):best[1][k] for k in range(4)},n=T['n']))
    out.sort(key=lambda r:-r['logp'])
    print(name,"enc-lane candidates (tier>=32):",ncand,"; within threshold:",len(out))
    return out
allres=[]
for name,T in TRAILS.items():
    res=search(name,T)
    for r in res[:10]: print("  %.3f"%r['logp'],{k:v for k,v in r.items() if k!='logp'})
    allres+=res
allres.sort(key=lambda r:-r['logp'])
json.dump(allres,open('valsearch2_out.json','w'),indent=1)
