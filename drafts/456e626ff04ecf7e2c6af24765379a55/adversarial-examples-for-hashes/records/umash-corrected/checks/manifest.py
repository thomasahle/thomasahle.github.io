#!/usr/bin/env python3
"""Record source/output hashes and the permitted execution environment."""
import hashlib
import json
import os
from pathlib import Path
import platform
import resource
import subprocess

root=Path(__file__).resolve().parent
paths=[p for p in root.iterdir() if p.suffix in ('.py','.cpp','.sh','.json','.md')
       and p.name!='manifest.json']
result={'hostname':platform.node(),'platform':platform.platform(),
        'python':platform.python_version(),'niceness':os.getpriority(os.PRIO_PROCESS,0),
        'cpu_affinity':sorted(os.sched_getaffinity(0)),
        'OMP_NUM_THREADS':os.environ.get('OMP_NUM_THREADS'),
        'address_space_limit_bytes':resource.getrlimit(resource.RLIMIT_AS)[0],
        'compiler':subprocess.check_output(['g++','--version'],text=True).splitlines()[0],
        'sha256':{p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in sorted(paths)}}
assert result['niceness']>=10
assert set(result['cpu_affinity'])<=set(range(56,64))
assert 0 < result['address_space_limit_bytes']<=32000000000
(root/'manifest.json').write_text(json.dumps(result,indent=2)+'\n')
print('Manifest and environment: PASS')
