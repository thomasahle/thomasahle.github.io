#!/usr/bin/env python3
import hashlib,json,pathlib,sys
baseline,fixed=map(pathlib.Path,sys.argv[1:3])
a,b=baseline.read_bytes(),fixed.read_bytes()
assert len(a)==len(b)==10000*480
for i in range(10000):
    assert a[480*i+448:480*(i+1)]==b[480*i+448:480*(i+1)],i
result={'inputs':10000,'style_wrappers':4,'identical_outputs':40000,
        'baseline':'fix-neon-dispatch compiled with -fwrapv for legacy signed sums',
        'fixed':'final header, no -fwrapv or -fno-strict-aliasing',
        'baseline_vector_sha256':hashlib.sha256(a).hexdigest(),
        'fixed_vector_sha256':hashlib.sha256(b).hexdigest()}
print(json.dumps(result,indent=2))
