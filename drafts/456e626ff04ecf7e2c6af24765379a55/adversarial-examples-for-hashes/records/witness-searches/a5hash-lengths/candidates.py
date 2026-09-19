# Enumerate (len, p, q) for the a5hash-64 dead-first-operand class and estimate the score.
# S1_init = lo64(X*Y), X = K2^len ^ (seed & AA..), Y = K1^len ^ (seed & 55..).
# v2(X)=p, v2(Y)=q, v=p+q, m=64-v residual bits; S = cost + fx + fy determined seed bits.
# Pr[S1_init = W*] = mode/2^S; score = log2(ceil(len/8)) + S - log2(mode).
# h_est(m) = log2(mode) estimate fitted to enum_mode logs (v odd, m<=31) and beam fibers (m~51..57).
import math, sys, itertools
K1=0x243F6A8885A308D3; K2=0x452821E638D01377
def bit(x,i): return (x>>i)&1
def build(L,p,q):
    K2l=K2^L; K1l=K1^L; cost=0
    for i in range(p):
        if i&1: cost+=1
        elif bit(K2l,i): return None
    if p&1: cost+=1
    elif not bit(K2l,p): return None
    for j in range(q):
        if not (j&1): cost+=1
        elif bit(K1l,j): return None
    if not (q&1): cost+=1
    elif not bit(K1l,q): return None
    m=64-p-q
    fx=sum(1 for t in range(1,m) if p+t<64 and (p+t)&1)
    fy=sum(1 for t in range(1,m) if q+t<64 and not ((q+t)&1))
    return cost,fx,fy,m
def pattern(p,q):
    fixed={}
    for i in range(p):
        if not (i&1): fixed[i]=bit(K2,i)
    if not (p&1): fixed[p]=bit(K2,p)^1
    for j in range(q):
        if j&1:
            if j in fixed and fixed[j]!=bit(K1,j): return None
            fixed[j]=bit(K1,j)
    if q&1:
        if q in fixed and fixed[q]!=(bit(K1,q)^1): return None
        fixed[q]=bit(K1,q)^1
    return fixed
def h_est(m):
    if m<=33: return 0.19*m+1.7
    return 7.9+0.1*(m-33)
LIM=1<<22
lens=set(range(1,4097))
for p in range(0,64):
    for q in range(0,64-p):
        f=pattern(p,q)
        if f is None: continue
        base=sum(b<<i for i,b in f.items())
        if base>=LIM: continue
        free=[i for i in range(22) if i not in f]
        for k in range(0,3):
            for sub in itertools.combinations(free,k):
                L=base
                for i in sub: L|=1<<i
                if 0<L<LIM: lens.add(L)
rows=[]
for L in sorted(lens):
    Lw=max(1,-(-L//8))
    for p in range(0,64):
        for q in range(0,64-p):
            r=build(L,p,q)
            if r is None: continue
            cost,fx,fy,m=r
            S=cost+fx+fy
            v=p+q
            est=math.log2(Lw)+S-h_est(m)
            if v%2==0: est+=6   # even v: near-uniform residual map (enum 6615/36 gave mode 1)
            rows.append((est,L,p,q,v,m,cost,fx,fy,S))
rows.sort()
print("lens considered:",len(lens))
print("est   len      p  q  v  m cost fx fy  S   (est = log2(Lw)+S-h_est(m); +6 penalty for even v)")
for r in rows[:70]:
    print("%5.2f %8d %3d %2d %2d %2d %3d %3d %2d %3d"%r)
print("--- short lengths (<=24), best (p,q) per length, v odd only")
best={}
for r in rows:
    if r[1]<=24 and r[4]%2==1 and (r[1] not in best): best[r[1]]=r
for L in sorted(best): print("%5.2f %8d %3d %2d %2d %2d %3d %3d %2d %3d"%best[L])
