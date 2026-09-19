#!/usr/bin/env python3
from hashlib import sha256
from pathlib import Path
import json, os, platform, subprocess

assert platform.system() == 'Linux'
paths = [Path('PROOF3.md'), Path('PROGRESS.md'),
         Path('materials/PROOF.md'), Path('materials/PROOF2.md'),
         Path('materials/UMASHObligations.lean')]
paths += sorted(p for p in Path('checks').iterdir()
                if p.suffix in ('.py','.cpp','.sh','.h','.json','.log','.md')
                and p.name != 'manifest.json')
result = dict(hostname=platform.node(),platform=platform.platform(),
              python=platform.python_version(),affinity=sorted(os.sched_getaffinity(0)),
              niceness=os.getpriority(os.PRIO_PROCESS,0),
              virtual_memory_limit_bytes=__import__('resource').getrlimit(__import__('resource').RLIMIT_AS)[0],
              compiler=subprocess.check_output(['g++','--version'],text=True).splitlines()[0],
              sha256={str(p):sha256(p.read_bytes()).hexdigest() for p in paths if p.exists()})
Path('checks/manifest.json').write_text(json.dumps(result,indent=2)+'\n')
