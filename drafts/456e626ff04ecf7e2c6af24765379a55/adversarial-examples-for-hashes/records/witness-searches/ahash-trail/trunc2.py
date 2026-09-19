# Truncated trail enumeration, extended: (1) finish-time cancellation combined = aesenc(sum, enc):
#   MC(SB(SR(Dsum))) = Denc costs |Dsum| S-boxes; (2) the last block may inject fresh bytes.
import itertools, sys
MASK=[0x4,0xb,0x9,0x6,0x8,0xd,0xf,0x5,0xe,0x3,0x1,0xc,0x0,0x7,0xa,0x2]
def sh(S): return frozenset(i for i in range(16) if MASK[i] in S)
def isr(E): return frozenset(((s+4*(s%4))&15) for s in E)
def sr(E): return frozenset(((s-4*(s%4))&15) for s in E)
def col(i): return i//4
BEST=int(sys.argv[1]) if len(sys.argv)>1 else 3
def mixpatterns(A):
    cols=[c for c in range(4) if any(col(i)==c for i in A)]
    per=[]
    for c in cols:
        j=sum(1 for i in A if col(i)==c)
        opts=[frozenset(O) for k in range(max(1,5-j),5) for O in itertools.combinations(range(4*c,4*c+4),k)]
        per.append(opts)
    for combo in itertools.product(*per):
        yield frozenset().union(*combo) if combo else frozenset()
def rpatterns(E): return mixpatterns(isr(E))
results=[]
def terminal(E,S,cost,trail):
    if not E and not S: results.append((cost,trail,'zero')); return
    if E and S:
        c=cost+len(S)
        if c>BEST: return
        for O in mixpatterns(sr(S)):
            if O==E: results.append((c,trail,'finish')); return
def step(E,S,i,cost,trail):
    for O in rpatterns(E):
        shS=sh(S)
        cand=sorted(O|shS)
        fresh=[p for p in range(16) if p not in cand] if i==4 else []
        for k in range(0,len(cand)+1):
            for P in itertools.combinations(cand,k):
                P=frozenset(P)
                E2=(O-P)|(P-O); S2=(shS-P)|(P-shS)
                if i==4:
                    terminal(E2,S2,cost,trail+[P])
                    # fresh bytes in last block: up to 4
                    for kf in range(1,5):
                        for F in itertools.combinations(fresh,kf):
                            F=frozenset(F); terminal(E2|F,S2|F,cost,trail+[P|F])
                    continue
                c2=cost+len(E2)
                if c2>BEST: continue
                step(E2,S2,i+1,c2,trail+[P])
for k in range(1,BEST+1):
    for P1 in itertools.combinations(range(16),k):
        P1=frozenset(P1); step(P1,P1,2,k,[P1])
seen=set()
for cost,tr,kind in sorted(results,key=lambda r:r[0]):
    key=(kind,tuple(tuple(sorted(p)) for p in tr))
    if key in seen: continue
    seen.add(key); print(cost,kind,[sorted(p) for p in tr])
print("distinct",len(seen))
