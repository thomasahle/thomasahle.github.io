#!/usr/bin/env python3
import sys,platform,json
sys.path.insert(0,'materials')
from certify_primary import signed_masks,patterns
assert platform.system()=='Linux'
ds=signed_masks(64,(1<<61)-1)
with open('checks/masks.h','w') as f:
 f.write('#pragma once\nstruct MaskRow { uint64_t mask; unsigned np; uint64_t pats[8]; };\nstatic const MaskRow rows[] = {\n')
 for m in ds:
  ps=patterns(m)
  f.write('{'+str(m)+'ULL,'+str(len(ps))+',{'+','.join(str(x)+'ULL' for x in ps)+'}},\n')
 f.write('};\n')
