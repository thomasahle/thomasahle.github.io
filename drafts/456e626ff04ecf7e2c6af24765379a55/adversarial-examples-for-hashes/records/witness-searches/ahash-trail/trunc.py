# Truncated (active-byte) trail enumeration for aHash 0.8.12 AES path, 33..64-byte messages:
# four hash_in(v_i): enc <- IMC(ISB(ISR(enc))) ^ v ; sum <- shuffle(sum) + v (2x64-bit adds).
# Both lanes must end with zero difference (finish = 2 AES rounds on aesenc(sum, enc), truncated).
# Cost = number of S-box transitions in the enc lane (each at best 4/256).
import itertools, sys
MASK=[0x4,0xb,0x9,0x6,0x8,0xd,0xf,0x5,0xe,0x3,0x1,0xc,0x0,0x7,0xa,0x2]
def sh(S): return frozenset(i for i in range(16) if MASK[i] in S)
def isr(E): return frozenset(((s+4*(s%4))&15) for s in E)   # source byte s (row s%4) -> dest s+4r
def col(i): return i//4
BEST=int(sys.argv[1]) if len(sys.argv)>1 else 3
# R-pattern choices: for each active column with input set I, output set O with |I|+|O|>=5
def rpatterns(E):
    A=isr(E)
    cols=[c for c in range(4) if any(col(i)==c for i in A)]
    per=[]
    for c in cols:
        j=sum(1 for i in A if col(i)==c)
        opts=[]
        for k in range(max(1,5-j),5):
            for O in itertools.combinations(range(4*c,4*c+4),k): opts.append(frozenset(O))
        per.append(opts)
    for combo in itertools.product(*per):
        yield frozenset().union(*combo) if combo else frozenset()
results=[]
def step(E,S,i,cost,trail):
    # apply block i (2..4): choose R pattern then P_i
    for O in rpatterns(E):
        shS=sh(S)
        # candidate positions for P_i: any subset of O ∪ shS (new bytes outside only add cost; allow a few)
        cand=sorted(O|shS)
        # P must cancel enough; enumerate subsets of cand plus up to 0 new bytes
        for k in range(0,len(cand)+1):
            for P in itertools.combinations(cand,k):
                P=frozenset(P)
                # enc: bytes in P∩O cancel (choose cancel); bytes in P\O become active
                E2=(O-P)|(P-O)
                S2=(shS-P)|(P-shS)
                if i==4:
                    if not E2 and not S2:
                        results.append((cost,trail+[P]))
                    continue
                c2=cost+len(E2)
                if c2>BEST: continue
                if E2 and not S2 and i<4: pass
                step(E2,S2,i+1,c2,trail+[P])
for k in range(1,BEST+1):
    for P1 in itertools.combinations(range(16),k):
        P1=frozenset(P1)
        step(P1,P1,2,k,[P1])
results.sort(key=lambda r:r[0])
seen=set()
for cost,tr in results:
    key=tuple(tuple(sorted(p)) for p in tr)
    if key in seen: continue
    seen.add(key)
    print(cost,[sorted(p) for p in tr])
print("total distinct",len(seen))
