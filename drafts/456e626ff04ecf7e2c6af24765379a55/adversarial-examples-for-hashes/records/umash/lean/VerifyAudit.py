"""Fail unless each axiom report has only the three standard logical axioms."""
from pathlib import Path
import hashlib
import json
import re

expected = re.findall(r'^#print axioms (\S+)', Path('AuditAll.lean').read_text(), re.M)
reports = re.findall(r"'([^']+)' depends on axioms: \[([^\]]*)\]", Path('AuditAll.txt').read_text())
reports += [(n, '') for n in re.findall(r"'([^']+)' does not depend on any axioms", Path('AuditAll.txt').read_text())]
actual = dict(reports)
assert set(expected) == set(actual), (len(expected), len(actual), set(expected)-set(actual))
allowed = {'propext', 'Classical.choice', 'Quot.sound'}
for n, axioms in reports:
    assert set(filter(None, (a.strip() for a in axioms.split(',')))) <= allowed, (n, axioms)
for path in Path('ProvenHashes').glob('*.lean'):
    assert not re.search(r'\b(sorry|admit|native_decide|unsafe)\b|^\s*axiom\s', path.read_text(), re.M), path
files = list(Path('ProvenHashes').glob('*.lean')) + [Path('ProvenHashes.lean'),
    Path('lakefile.toml'), Path('lake-manifest.json'), Path('lean-toolchain'), Path('AuditAll.lean')]
files += list(Path('.').glob('*.py')) + list(Path('.').glob('*.sh')) + [Path('CheckModel.lean')]
files += [p for p in Path('sources').rglob('*') if p.suffix in {'.py', '.md'}]
manifest = {str(p): hashlib.sha256(p.read_bytes()).hexdigest() for p in sorted(files)}
Path('SourceHashes.json').write_text(json.dumps(manifest, indent=2)+'\n')
print(json.dumps({'result': 'PASS', 'theorems_audited': len(expected),
                  'permitted_axioms': sorted(allowed),
                  'sorry_admit_native_decide_custom_axioms': False,
                  'unsafe_declarations': False}, indent=2))
