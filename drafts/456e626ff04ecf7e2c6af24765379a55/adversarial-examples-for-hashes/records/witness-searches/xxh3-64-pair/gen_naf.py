#!/usr/bin/env python3
"""NAF family: complement pairs (x, x-D) at secret offset o, D = K[2o]+K[2o+1]+1, where x is
chosen so that popcount(x ^ (x-D)) equals the NAF weight of D (the minimum): x_i=1 at +1 digits,
x_i=0 at -1 digits, other bits free.  Free bits: 0, 1, K[2o], ~K[2o], and random fills."""
import sys, random
sec = bytes.fromhex("b8fe6c3923a44bbe7c01812cf721ad1cded46de9839097db7240a4a4b7b3671fcb79e64eccc0e578825ad07dccff7221b8084674f743248ee03590e6813a264c3c2852bb91c300cb88d0658b1b532ea371644897a20df94e3819ef46a9deacd8a8fa763fe39c343ff9dcbbc7c70b4f1d8a51e04bcdb45931c89f7ec9d9787364eac5ac8334d3ebc3c581a0fffa1363eb170ddd51b7f0da49d3165526 29d4689e2b16be587d47a1fc8ff8b8d17ad031ce45cb3a8f95160428afd7fbcabb4b407e".replace(" ",""))
K=[int.from_bytes(sec[8*i:8*i+8],'little') for i in range(24)]
M=(1<<64)-1
def naf(d):
    digs=[]; i=0
    while d:
        if d&1:
            z=2-(d&3); digs.append((i,z)); d-=z
        d>>=1; i+=1
    return digs
nrand=int(sys.argv[1]) if len(sys.argv)>1 else 1000
random.seed(20260919)
out=set()
for o in range(8):
    ka,kb=K[2*o],K[2*o+1]; D=(ka+kb+1)&M
    ones=0; zeros=0
    for i,z in naf(D):
        if i>=64: continue
        if z==1: ones|=1<<i
        else: zeros|=1<<i
    free=M&~(ones|zeros)
    def emit(fill,tag):
        x=((fill&free)|ones)&M
        assert bin(x^((x-D)&M)).count("1")==len([1 for i,z in naf(D) if i<64])
        out.add((o,x,(x-D)&M,tag))
    emit(0,"naf0"); emit(M,"naf1"); emit(ka,"nafk"); emit(~ka,"nafnk"); emit(kb,"nafk2"); emit(~kb,"nafnk2")
    for _ in range(nrand if o<2 else nrand//4):
        emit(random.getrandbits(64),"nafr")
for (o,w0,w1,tag) in sorted(out): print(f"{o} {w0:016x} {w1:016x} {tag}")
