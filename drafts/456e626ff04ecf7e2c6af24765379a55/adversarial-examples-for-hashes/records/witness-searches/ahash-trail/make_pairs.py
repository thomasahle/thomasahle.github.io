# Build concrete message pairs from valsearch2 candidates.  Filler bytes = shipped pair A's m1.
import json, sys
SHIP_M1="40313233343536373839253b3c3d3e3f408142094445464748494a4b4c4d4e4f8d896d065c5d5e5f606162636465666768696a6b6c6d6e6f"
SHIP_M2="3e313233343536373839403b3c3d3e3f405e42204445464748494a4b4c4d4e4f727290085c5d5e5f606162636465666768696a6b6c6d6e6f"
base=bytearray.fromhex(SHIP_M1)+bytearray(range(0x70,0x78))   # 64-byte filler
def bytepair(a,d,pref):
    # y ^ y' = a, y - y' = d; prefer y close to pref
    c=[(abs(y-pref),y) for y in range(256) if 0<=y-d<=255 and (y^(y-d))==a]
    assert c; return min(c)[1]
def span32(X,D,pref):
    # y' = y + D, y ^ y' = X.  Signs from the DP: y' - y = sum s_i 2^i over bits of X.
    # reconstruct: forward DP with parents
    T=D; levels=[{T:None}]
    for i in range(32):
        nxt={}
        for r in levels[-1]:
            if (X>>i)&1:
                if r&1:
                    nxt[(r-1)//2]=(r,+1); nxt[(r+1)//2]=(r,-1)
            else:
                if not r&1: nxt[r//2]=(r,0)
        levels.append(nxt)
    assert 0 in levels[32], "infeasible"
    signs=[0]*32; r=0
    for i in range(31,-1,-1):
        prev,s=levels[i+1][r]; signs[i]=s; r=prev
    y=0
    for i in range(32):
        if (X>>i)&1: bit = 0 if signs[i]==+1 else 1   # s=+1: y'_i=1,y_i=0
        else: bit=(pref>>i)&1
        y|=bit<<i
    yp=y+D
    assert 0<=yp<2**32 and (y^yp)==X and yp-y==D, (hex(y),hex(yp))
    return y,yp
def build(c):
    n=c['n']; m1=bytearray(base[:n]); m2=bytearray(base[:n])
    # block offsets: v1 at 0, v2 at 16, v3 at n-32
    for pos,a in c['x1'].items():
        pos=int(pos); a=int(a,16); d=c['ds'][str(pos)]
        y=bytepair(a,d,base[pos]); m1[pos]=y; m2[pos]=y-d
    for pos,a in c['x2'].items():
        pos=int(pos); a=int(a,16); d=c['ds'][str(pos)]
        y=bytepair(a,d,base[16+pos]); m1[16+pos]=y; m2[16+pos]=y-d
    P3=sorted(int(p) for p in c['x3']); X=sum(int(c['x3'][str(P3[k])],16)<<(8*k) for k in range(4))
    # D = sum of transported diffs at span byte k: source per position from the trail mapping
    MASK=[0x4,0xb,0x9,0x6,0x8,0xd,0xf,0x5,0xe,0x3,0x1,0xc,0x0,0x7,0xa,0x2]
    D=0
    for k in range(4):
        pos=P3[k]; src=MASK[pos]           # sum-lane byte at pos came from src after one shuffle
        if str(src) in c['x2']: d=c['ds'][str(src)]
        else:
            src2=MASK[src]; assert str(src2) in c['x1']; d=c['ds'][str(src2)]
        D+=d<<(8*k)
    off=n-32+P3[0]
    pref=int.from_bytes(base[off:off+4],'little')
    y,yp=span32(X,-(-D) if False else -D,pref)  # y' = y + (-D)  <=> y - y' = D ... careful below
    # we need v3 - v3' = -D  => y - y' = -D => y' = y + D
    y,yp=span32(X,D,pref)
    m1[off:off+4]=y.to_bytes(4,'little'); m2[off:off+4]=yp.to_bytes(4,'little')
    return m1.hex(),m2.hex()
cands=json.load(open('valsearch2_out.json'))
out=[]
for c in cands:
    m1,m2=build(c)
    out.append(dict(m1=m1,m2=m2,pred=c['logp'],trail=c['trail'],tier=c['tier'],n=c['n'],cand=c))
out.sort(key=lambda r:-r['pred'])
json.dump(out,open('pairs.json','w'),indent=1)
with open('pairs.txt','w') as f:
    f.write("%s %s shipped\n"%(SHIP_M1,SHIP_M2))
    for i,r in enumerate(out):
        f.write("%s %s %s_%d_pred%.3f\n"%(r['m1'],r['m2'],r['trail'],i,r['pred']))
for r in out: print("%.3f %s n=%d %s / %s"%(r['pred'],r['trail'],r['n'],r['m1'],r['m2']))
# sanity: shipped must be reproduced by its own candidate (a_p=7e...)
