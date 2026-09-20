#!/usr/bin/env python3
import json,platform
from fractions import Fraction as F
from decimal import Decimal,localcontext
assert platform.system()=='Linux'
q=1<<64;p=(1<<61)-1
A=F(205,q-561);B=F(435,q-561);S=B+F(2,p-2)
generic=(1<<61)*(A+F(32,p-2));special=(1<<61)*S
assert 57<generic<58 and 56<special<57
assert generic<64 and special<64
assert A+(1-A)*F(2,p-2)<S
R=2/S
assert R**50>2**2809 and R**100<2**5619 # 56.18 < log2 R < 56.19
assert R<q-561
lengths=set(range(1,4097))
for h in [1,2,3,31,1<<20,1<<50,1<<61,1<<64]:lengths.update([512*h-1,512*h,512*h+1])
for L in sorted(lengths):
    eps=F(1,q-561) if L==1 else max(A+(1-A)*min(1,F(2*((L+31)//32),p-2)),S)
    assert eps<=F(58*((L+511)//512),1<<61)
    if L>=2:assert eps/L<=S/2
with localcontext() as c:
    c.prec=50;score=(Decimal(R.numerator)/Decimal(R.denominator)).ln()/Decimal(2).ln()
def pack(x):return [x.numerator,x.denominator]
out=dict(q=q,p=p,PH_numerator=151,ENH_with_PH_numerator=205,tail_weighted_numerator=435,
         generic_envelope=pack(generic),tail_envelope=pack(special),published_coefficient=64,proved_coefficient=58,
         epsilon_L2=pack(S),score_ratio=pack(R),score_display=str(score),score_interval=['56.18','56.19'],
         lengths_checked=len(lengths),Published64_proved_on_paper=True,SharpPrimaryProjection_proved=False,
         counterexample=False)
open('checks/envelope_certificate.json','w').write(json.dumps(out,indent=2)+'\n')
print(json.dumps(out,indent=2))
