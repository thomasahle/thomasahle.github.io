# Independent check of the "no key-free L=2 pair" argument for MurmurHash3_x64_128.
# Tail-only messages of lengths r != r' in 0..15; state before finalization is
# (seed ^ mix1(k1) ^ r, seed ^ mix2(k2) ^ r'), mix2 present only if r > 8.
M=(1<<64)-1
c1=0x87c37b91114253d5; c2=0x4cf5ad432745937f
def rotl(x,n): return ((x<<n)|(x>>(64-n)))&M
def rotr(x,n): return rotl(x,64-n)
inv=lambda a: pow(a,-1,1<<64)
def mix2(k): return (rotl((k*c2)&M,33)*c1)&M
def mix2inv(y): return (rotr((y*inv(c1))&M,33)*inv(c2))&M
def mix1(k): return (rotl((k*c1)&M,31)*c2)&M
def mix1inv(y): return (rotr((y*inv(c2))&M,31)*inv(c1))&M
def signed(x): return x-(1<<64) if x>>63 else x
# Case A: r == 8 (or r<8 with no k2) vs r'>8: need mix2(k2') = d, k2' < 2^(8(r'-8)) <= 2^56
print("Case A: mix2^-1(d) for d in 1..15 (must be < 2^56 to fit a tail):")
for d in range(1,16):
    k=mix2inv(d); print(f"  d={d:2d} mix2inv={k:016x} bits={k.bit_length()}")
# also mix1^-1(d) for r<8 side (k1 must fit in r bytes): 
print("Case A': mix1^-1(d):")
for d in range(1,16):
    k=mix1inv(d); print(f"  d={d:2d} mix1inv={k:016x} bits={k.bit_length()}")
# Case B: both r,r'>8: mix2(k2)^mix2(k2')=d.  mix2(k2)^d = mix2(k2)+delta, delta in the set
# {x^d - x}, which for d<16 lies in (-16,16). Then v'=v+D, D=delta*c1^-1; rotr(v+D,33)=rotr(v,33)+rotr(D,33)+c-c'*2^31
# so k2'-k2 = c2^-1*(rotr(D,33)+c-c'*2^31), c,c' in {0,1}.  Enumerate and report the minimum |k2'-k2|.
best=None
for d in range(1,16):
    deltas=set()
    for x in range(256):  # delta depends only on low 4 bits of x, sample low byte
        deltas.add(((x^d)-x))
    for delta in deltas:
        D=(delta*inv(c1))&M
        for c in (0,1):
            for cp in (0,1):
                diff=(inv(c2)*((rotr(D,33)+c-(cp<<31))&M))&M
                s=abs(signed(diff))
                if best is None or s<best[0]: best=(s,d,delta,c,cp,diff)
                if s < (1<<57): print("  SMALL:",d,delta,c,cp,hex(diff),s.bit_length())
print("Case B: min |k2'-k2| over all (d,delta,c,c'):", best[0].bit_length(),"bits", [hex(best[5])], best[1:5])
# Case B, brute-force confirmation: random k2 < 2^56 for each d, check whether k2' < 2^56
import random
random.seed(1); hits=0
for d in range(1,16):
    for _ in range(1<<14):
        k2=random.getrandbits(56); k2p=mix2inv(mix2(k2)^d)
        if k2p < (1<<56): hits+=1; print("HIT",d,hex(k2),hex(k2p))
print("Case B random sample hits (k2' < 2^56):",hits,"of",15*(1<<14))
