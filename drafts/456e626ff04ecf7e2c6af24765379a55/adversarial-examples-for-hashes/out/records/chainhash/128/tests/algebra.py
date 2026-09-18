"""Check the field-generic quintic decoder independently of the C circuit."""
import random
from oracle import mul,MASK,Q
r=random.Random(0x128)
def encode(c):
    c0,c1,c2,c3,c4=c;b=c0^c1;d=mul(c0,c1)
    return [c4^mul(c2,d^c3),d^c3^mul(c0,c2),c0^mul(c2,b),b^c2,1^c2]
def decode(e):
    e0,e1,e2,e3,e4=e;c2=e4^1;b=e3^c2;c0=e2^mul(c2,b);c1=b^c0;d=mul(c0,c1)
    c3=e1^d^mul(c0,c2);c4=e0^mul(c2,d^c3)
    return [c0,c1,c2,c3,c4]
def circuit(c,v):
    y=mul(v,v);z=mul(y^c[0],v^y^c[1]);return mul(v^c[2],z^c[3])^c[4]
for i in range(1000):
    c=[r.getrandbits(128) for _ in range(5)];e=[r.getrandbits(128) for _ in range(5)]
    assert decode(encode(c))==c and encode(decode(e))==e
    v,t=r.getrandbits(128),r.getrandbits(128)
    a=1
    for coeff in reversed(encode(c)):a=mul(a,v)^coeff
    assert a==circuit(c,v)
    assert (((v+t)&MASK)-t)&MASK==v
print('PASS 1000 both-way quintic coefficient maps, circuit/Horner comparisons, integer translations')
