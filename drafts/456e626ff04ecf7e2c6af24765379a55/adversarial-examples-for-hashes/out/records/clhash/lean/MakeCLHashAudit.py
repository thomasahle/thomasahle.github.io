"""Create an audit of every new declaration, including proof-bearing definitions."""
from pathlib import Path
import re

names = []
for p in sorted(Path('ProvenHashes').glob('CLHash*.lean')):
    source = p.read_text()
    assert not re.search(r'\b(sorry|admit|native_decide)\b', source), p
    assert not re.search(r'^\s*(axiom|unsafe)\b', source, re.M), p
    for name in re.findall(r'^(?:theorem|lemma|def|abbrev)\s+(\w+)', source, re.M):
        names.append('ProvenHashes.CLHash.' + name)
assert len(names) == len(set(names))
out = 'import ProvenHashes\n\n'
for name in names:
    out += f'#check @{name}\n#print axioms {name}\n'
Path('CLHashAudit.lean').write_text(out)
