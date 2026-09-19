import random,sys
exec(open('valsearch.py').read().split("TRAILS={")[0])
def isr_sb(s):  # bytes list 16 -> InvShiftRows then InvSubBytes
    return [ISB[s[(i-4*(i%4))&15]] for i in range(16)]
def imc(s): return sum((imc_col(s[4*c:4*c+4]) for c in range(4)),[])
def aesdec(s,k): return [a^b for a,b in zip(imc(isr_sb(s)),k)]
def shuffle(s): return [s[MASK[i]] for i in range(16)]
def add64(a,b):
    lo=(int.from_bytes(a[:8],'little')+int.from_bytes(b[:8],'little'))&(2**64-1)
    hi=(int.from_bytes(a[8:],'little')+int.from_bytes(b[8:],'little'))&(2**64-1)
    return list(lo.to_bytes(8,'little')+hi.to_bytes(8,'little'))
def blocks(m):
    n=len(m); return [m[0:16],m[16:32],m[n-32:n-16],m[n-16:n]]
def trace(m1,m2,trials):
    n=len(m1); stats=[0]*5
    for t in range(trials):
        enc=[random.randrange(256) for _ in range(16)]; sm=[random.randrange(256) for _ in range(16)]
        e1,e2,s1,s2=enc[:],enc[:],sm[:],sm[:]
        ok=True
        for i,(b1,b2) in enumerate(zip(blocks(m1),blocks(m2))):
            e1=aesdec(e1,b1); e2=aesdec(e2,b2); s1=add64(shuffle(s1),b1); s2=add64(shuffle(s2),b2)
            ed=[a^b for a,b in zip(e1,e2)]; sd=[a^b for a,b in zip(s1,s2)]
            if t<1: print(" block",i+1,"enc xor",bytes(ed).hex(),"sum xor",bytes(sd).hex())
            if not any(ed) and not any(sd): stats[i+1]+=1; break
    return stats
random.seed(1)
for line in open('confirm_pairs.txt'):
    a,b,lab=line.split(); m1=bytes.fromhex(a); m2=bytes.fromhex(b)
    print(lab,len(m1))
    print("  both-lanes-zero after block k, out of 2^17:",trace(m1,m2,1<<17))
