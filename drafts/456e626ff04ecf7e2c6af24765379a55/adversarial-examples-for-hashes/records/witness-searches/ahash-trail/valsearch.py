# Value-level trail search for the two 3-S-box truncated trails of aHash 0.8.12 (AES path, 4-block regime).
# Enc lane: three inverse-S-box transitions (DDT of the inverse AES S-box); sum lane: additive byte
# differences transported by the byte shuffle, needing no byte carry at S1 (v1 diffs), S2 (v1 and v2 diffs).
import itertools, math, json, sys
def gmul(a,b):
    r=0
    while b:
        if b&1: r^=a
        a=(a<<1)^(((a>>7)&1)*0x11b); b>>=1
    return r
SB=[0]*256; ISB=[0]*256
for x in range(256):
    inv=0
    if x:
        for y in range(1,256):
            if gmul(x,y)==1: inv=y;break
    s=inv;t=inv
    for i in range(4):
        t=((t<<1)|(t>>7))&255; s^=t
    SB[x]=s^0x63
for x in range(256): ISB[SB[x]]=x
IMC=[[14,11,13,9],[9,14,11,13],[13,9,14,11],[11,13,9,14]]
# inverse S-box DDT: DDT[a][b] = #{x : ISB[x]^ISB[x^a] = b}
DDT=[[0]*256 for _ in range(256)]
for a in range(1,256):
    for x in range(256): DDT[a][ISB[x]^ISB[x^a]]+=1
assert all(sum(1 for b in range(256) if DDT[a][b]==4)==1 for a in range(1,256)), "row uniqueness"
assert all(sum(1 for a in range(1,256) if DDT[a][b]==4)==1 for b in range(1,256)), "column uniqueness"
# additive diffs achievable for xor a: {y - y' : y ^ y' = a}
COMP=[set() for _ in range(256)]
for y in range(256):
    for yp in range(256): COMP[y^yp].add(y-yp)
# best |d| for coupled bytes: d in COMP[a], -d in COMP[b]
def bestd(a,b):
    c=[d for d in COMP[a] if -d in COMP[b]]
    return min(c,key=abs) if c else None
def bestd1(a):
    return min(COMP[a],key=abs)
MASK=[0x4,0xb,0x9,0x6,0x8,0xd,0xf,0x5,0xe,0x3,0x1,0xc,0x0,0x7,0xa,0x2]
def shpos(p): return MASK.index(p)          # sum-lane byte p moves to position shpos(p)
def isrpos(s): return (s+4*(s%4))&15
def imc_col(colvec):  # colvec: 4 bytes (rows 0..3) -> 4 bytes
    return [gmul(IMC[r][0],colvec[0])^gmul(IMC[r][1],colvec[1])^gmul(IMC[r][2],colvec[2])^gmul(IMC[r][3],colvec[3]) for r in range(4)]
TRAILS={
 'A':dict(P1=[0,10],P2=[1,3],n=56),   # shipped structure
 'B':dict(P1=[8,15],P2=[9,11],n=64),  # mirror
}
def search(name,T,mintier=32):
    P1,P2=T['P1'],T['P2']
    q=[isrpos(p) for p in P1]; c=q[0]//4; assert q[1]//4==c
    rows=[x%4 for x in q]
    P2rows=[p%4 for p in P2]; assert all(p//4==c for p in P2)
    rem=[r for r in range(4) if r not in P2rows]
    # sum-lane bookkeeping: v1 positions -> shpos -> shpos ; v2 positions -> shpos
    s3pos={}
    for p in P1: s3pos[shpos(shpos(p))]=('v1',p)
    for p in P2: s3pos[shpos(p)]=('v2',p)
    P3=sorted(s3pos)
    assert P3==list(range(P3[0],P3[0]+4)) and P3[0]%4==0
    c3=P3[0]//4
    out=[]
    for a_p in range(1,256):
        for b_p in range(1,256):
            if DDT[a_p][b_p]==0: continue
            # find b_q so that IMC column with (b_p at row rows[0], b_q at row rows[1]) vanishes at the vanish row
            # we don't know which of rem rows vanishes; try each
            for rv in rem:
                rr=[r for r in rem if r!=rv][0]
                # IMC[rv][rows0]*b_p ^ IMC[rv][rows1]*b_q = 0 -> b_q = IMC[rv][rows0]/IMC[rv][rows1] * b_p
                # solve by brute force (256)
                cands=[bq for bq in range(1,256) if gmul(IMC[rv][rows[0]],b_p)^gmul(IMC[rv][rows[1]],bq)==0]
                assert len(cands)==1; b_q=cands[0]
                col=[0]*4; col[rows[0]]=b_p; col[rows[1]]=b_q
                o=imc_col(col); assert o[rv]==0
                a3=o[rr]; assert a3
                # remaining byte at position 4c+rr -> ISR -> must be column c3
                pos3=isrpos(4*c+rr)
                if pos3//4!=c3: continue
                r3=pos3%4
                for a_q in range(1,256):
                    if DDT[a_q][b_q]==0: continue
                    t12=DDT[a_p][b_p]*DDT[a_q][b_q]
                    if t12*4<mintier: continue
                    for b3 in range(1,256):
                        if DDT[a3][b3]==0: continue
                        tier=t12*DDT[a3][b3]
                        if tier<mintier: continue
                        col3=[0]*4; col3[r3]=b3
                        d3=imc_col(col3)  # xor diffs at positions 4*c3 + r
                        # v2 xor diffs at P2 rows
                        x2={p:o[p%4] for p in P2}
                        x1={P1[0]:a_p,P1[1]:a_q}
                        # coupled d's
                        logp=math.log2(tier)-24
                        ds={}
                        ok=True
                        for pos in P3:
                            src,p=s3pos[pos]
                            xa = x1[p] if src=='v1' else x2[p]
                            d=bestd(xa,d3[pos%4])
                            if d is None: ok=False;break
                            ds[p]=d
                            m=2 if src=='v1' else 1
                            logp+=m*math.log2((256-abs(d))/256)
                        if not ok: continue
                        out.append((logp,name,tier,a_p,b_p,a_q,b_q,a3,b3,rv,dict(x1=x1,x2=x2,x3={P3[i]:d3[i] for i in range(4)},ds=ds)))
    out.sort(key=lambda r:-r[0])
    return out
allres=[]
for name,T in TRAILS.items():
    res=search(name,T)
    print(name,"candidates",len(res))
    for r in res[:8]:
        print("  log2p=%.3f tier=%d a_p=%02x b_p=%02x a_q=%02x b_q=%02x a3=%02x b3=%02x vanish=%d"%(r[0],r[2],r[3],r[4],r[5],r[6],r[7],r[8],r[9]), {k:(v if k=='ds' else {kk:'%02x'%vv for kk,vv in v.items()}) for k,v in r[10].items()})
    allres+=res
json.dump([dict(logp=r[0],trail=r[1],tier=r[2],a_p=r[3],b_p=r[4],a_q=r[5],b_q=r[6],a3=r[7],b3=r[8],vanish=r[9],**{k:{str(kk):vv for kk,vv in v.items()} for k,v in r[10].items()}) for r in allres[:200]],open('valsearch_top.json','w'),indent=1)
