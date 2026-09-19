#!/usr/bin/env python3
import platform,json
from collections import Counter
from fractions import Fraction as F
from exact_common import *
assert platform.system()=='Linux'
ds=signed_masks(64,P);ps={m:patterns(m) for m in ds}
def weight(m):
    total=F(1,Q)
    for j in range(64):
        multiplicity=max(Counter(t>>j for t in ps[m]).values())
        total+=F((1<<j)*multiplicity,Q*(1<<((m&((1<<j)-1)).bit_count())))
    return min(F(1),total)
rows=[];one=two=F()
for m in ds:
    h=m.bit_count();top=m>>63
    sparse=1<<(h-top)
    quadratic=6*((1<<((h+1)//2))+1)
    dense=276*(1<<(64-h))
    KH=min(sparse,quadratic,dense)
    gap=(1<<m.bit_length())-m if m else Q
    kappa=min(1<<h,5+(1<<(71-h)),5+ceil(F(4*Q,gap-255)) if gap>255 else Q)
    W=weight(m)
    a=min(F(2*len(ps[m])),KH*W) if m else F(1)
    b=min(F(2*len(ps[m])),kappa*W) if m else F(1)
    two+=a;one+=b
    rows.append(dict(mask=m,weight=frac(W),KH=KH,kappa=kappa,two=frac(a),one=frac(b)))
assert 204<two<205 and 133<one<134
out=dict(rows=rows,two=frac(two),one=frac(one),two_ceiling=ceil(two),one_ceiling=ceil(one))
open('checks/enh_certificate.json','w').write(json.dumps(out,indent=2)+'\n')
print(json.dumps({k:v for k,v in out.items() if k!='rows'},indent=2))
