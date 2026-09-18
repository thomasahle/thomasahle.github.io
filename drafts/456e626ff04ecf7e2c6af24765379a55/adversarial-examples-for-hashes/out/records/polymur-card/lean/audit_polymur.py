#!/usr/bin/env python3
"""Generate signature/axiom checks for every declared Polymur proof in the import closure."""
from pathlib import Path
import re
seen = set()
names = []
def visit(module):
    if module in seen or not module.startswith('ProvenHashes.Polymur'):
        return
    seen.add(module)
    path = Path(module.replace('.', '/') + '.lean')
    source = path.read_text()
    assert not re.search(r'\b(sorry|admit|native_decide)\b', source), path
    assert not re.search(r'^\s*(axiom|unsafe)\b', source, re.M), path
    for dep in re.findall(r'^import (\S+)', source, re.M):
        visit(dep)
    for name in re.findall(r'^(?:@\[[^\n]*\]\s*)?(?:theorem|lemma)\s+(\w+)', source, re.M):
        names.append('ProvenHashes.Polymur.' + name)
    for name in re.findall(r'^instance (\w+)\s*:', source, re.M):
        names.append('ProvenHashes.Polymur.' + name)
visit('ProvenHashes.Polymur')
Path('PolymurAudit.lean').write_text('import ProvenHashes.Polymur\n\n' + ''.join(
    f'#check @{name}\n#print axioms {name}\n' for name in names))
print(f'Generated audit for {len(names)} declarations in {len(seen)} modules.')
