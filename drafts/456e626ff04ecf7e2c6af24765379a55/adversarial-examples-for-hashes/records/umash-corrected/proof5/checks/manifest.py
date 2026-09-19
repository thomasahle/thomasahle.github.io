#!/usr/bin/env python3
import hashlib
import json
import os
import platform
import resource
from pathlib import Path

root = Path(__file__).resolve().parents[1]
paths = [root/'PROOF5.md', root/'PROGRESS.md']
paths += sorted((root/'checks').glob('*.py'))
paths += sorted((root/'checks').glob('*.c'))
paths += sorted((root/'checks').glob('*.sh'))
paths += sorted(p for p in (root/'checks').glob('*.json') if p.name != 'manifest.json')
for pattern in ('PROOF*.md', 'UMASHObligations.lean', 'LEAN_OBLIGATIONS.md',
                'umash-lemma/umash*', 'umash-lemma/umash-src/*',
                'umash-subcase-b/VERDICT.md', 'umash-enh-verify/VERDICT.md',
                'checks*/**/*.json', 'umash-enh-verify/certificates/phi.json',
                'umash-subcase-b/verification/results/table.json'):
    paths += sorted(p for p in (root/'materials').glob(pattern) if p.is_file())
hashes = {str(p.relative_to(root)):hashlib.sha256(p.read_bytes()).hexdigest()
          for p in sorted(set(paths))}
out = dict(status='PASS', files=hashes, environment=dict(host=platform.node(),
    python=platform.python_version(), affinity=sorted(os.sched_getaffinity(0)),
    nice=os.getpriority(os.PRIO_PROCESS,0),
    address_space_limit=resource.getrlimit(resource.RLIMIT_AS)[0]))
(root/'checks/manifest.json').write_text(json.dumps(out,indent=2)+'\n')
print(f'Manifest: {len(hashes)} files')
