#!/usr/bin/env python3
import hashlib
import json
import os
import platform
import resource
from pathlib import Path

root=Path('.')
paths=[root/'PROOF2.md',root/'PROGRESS.md']
paths += [p for p in (root/'checks').iterdir()
          if p.suffix in ('.py','.cpp','.sh','.md','.json','.log') and p.name!='manifest.json']
result=dict(host=platform.node(),affinity=sorted(os.sched_getaffinity(0)),
            nice=os.getpriority(os.PRIO_PROCESS,0),
            address_space_limit=resource.getrlimit(resource.RLIMIT_AS)[0],
            sha256={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in sorted(paths)})
(root/'checks/manifest.json').write_text(json.dumps(result,indent=2)+'\n')
