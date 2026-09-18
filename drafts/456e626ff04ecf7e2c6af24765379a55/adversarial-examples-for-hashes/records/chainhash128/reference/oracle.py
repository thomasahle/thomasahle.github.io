"""Independent Python polynomial oracle + Rabin irreducibility certificate."""
import json, pathlib, sys
Q=1<<128; MASK=Q-1; MOD=Q|0x87

def raw(a,b):
    r=0
    while b:
        if b&1:r^=a
        a<<=1;b>>=1
    return r

def rem(a,b=MOD):
    while a.bit_length()>=b.bit_length():a^=b<<(a.bit_length()-b.bit_length())
    return a

def mul(a,b):return rem(raw(a,b))
def egcd(a,b):
    x0,x1,y0,y1=1,0,0,1
    while b:
        q,r=0,a
        while r.bit_length()>=b.bit_length():
            d=r.bit_length()-b.bit_length();q^=1<<d;r^=b<<d
        a,b=b,r;x0,x1=x1,x0^raw(q,x1);y0,y1=y1,y0^raw(q,y1)
    return a,x0,y0

def hash128(msg,k,W):
    size=16*W; n=max(1,(len(msg)+size-1)//size);state=k[W+2]
    for i in range(n):
        block=msg[i*size:(i+1)*size];acc=0
        for g in range((len(block)+63)//64):
            words=[int.from_bytes(block[64*g+16*j:64*g+16*(j+1)],'little') for j in range(4)]
            acc^=raw(words[0]^k[4*g],words[2]^k[4*g+2])^raw(words[1]^k[4*g+1],words[3]^k[4*g+3])
        a,b=acc&MASK,acc>>128
        if i==n-1:a^=len(msg);b^=len(msg)
        state=a^mul(b^k[W+1],state^k[W])
    v=(state+k[W+8])&MASK;y=mul(v,v)
    z=mul(y^k[W+3],v^y^k[W+4])
    return mul(v^k[W+5],z^k[W+6])^k[W+7]

def main():
    root=pathlib.Path(__file__).resolve().parent
    x=2;squares=[]
    for i in range(1,129):x=mul(x,x);squares.append(hex(x))
    assert x==2
    mid=int(squares[63],16)^2;g,a,b=egcd(mid,MOD)
    assert g==1 and raw(a,mid)^raw(b,MOD)==1
    cert={'modulus':hex(MOD),'successive_frobenius_squares':squares,'gcd64':g,'bezout_a':hex(a),'bezout_b':hex(b),'rabin':'X^(2^128)=X and gcd(X^(2^64)-X,Pi)=1; 2 is the only prime divisor of 128'}
    (root/'irreducibility.json').write_text(json.dumps(cert,indent=2)+'\n')
    allv={}
    for W in (16,32):
        kb=bytes((i*73+19)%256 for i in range(16*(W+9)))
        k=[int.from_bytes(kb[16*i:16*(i+1)],'little') for i in range(W+9)]
        ns=[0,1,15,16,17,31,32,33,63,64,65,127,128,129,255,256,257,511,512,513,1024,4096,8193]
        lines=[]
        for n in ns:
            msg=bytes((i*137+29)%256 for i in range(n));h=hash128(msg,k,W).to_bytes(16,'little').hex()
            lines.append(f'{n} {h}')
        (root/f'vectors-{16*W}.txt').write_text('\n'.join(lines)+'\n')
        allv[str(16*W)]={str(n):line.split()[1] for n,line in zip(ns,lines)}
    (root/'vectors.json').write_text(json.dumps({'key_bytes':'(73*i+19) mod 256; ideal key','message_bytes':'(137*i+29) mod 256','output':'16 bytes, little endian','vectors':allv},indent=2)+'\n')
    print('PASS independent Python vectors and Rabin certificate')
if __name__=='__main__':main()
