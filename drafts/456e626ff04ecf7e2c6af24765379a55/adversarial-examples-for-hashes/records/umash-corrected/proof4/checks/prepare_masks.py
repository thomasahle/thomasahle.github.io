#!/usr/bin/env python3
import platform,json
from exact_common import *
assert platform.system()=='Linux'
ds=signed_masks(64,P)
rows=[dict(mask=m,patterns=patterns(m)) for m in ds]
assert len(ds)==852
assert sum(len(r['patterns'])*(1<<(64-r['mask'].bit_count())) for r in rows)==8*Q+72
assert [sum(m%(1<<r)==0 for m in ds) for r in range(5)]==[852,248,64,2,1]
assert [sum(m>>63==i for m in ds) for i in range(2)]==[248,604]
with open('checks/masks.h','w') as f:
    f.write('#pragma once\nstruct MaskRow { uint64_t mask; unsigned np; uint64_t pats[8]; };\nstatic const MaskRow rows[] = {\n')
    for m in ds:
        ps=patterns(m)
        f.write('{'+str(m)+'ULL,'+str(len(ps))+',{'+','.join(str(x)+'ULL' for x in ps)+'}},\n')
    f.write('};\n')
open('checks/mask_certificate.json','w').write(json.dumps(dict(q=Q,p=P,rows=rows,congruent_ordered_pairs=8*Q+72),indent=2)+'\n')
