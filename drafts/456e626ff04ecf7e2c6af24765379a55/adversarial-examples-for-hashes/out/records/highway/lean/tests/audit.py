from pathlib import Path
import ast
import re
import sys
modules = set()
def visit(module):
    if module in modules:
        return
    modules.add(module)
    p = Path(module.replace('.', '/') + '.lean')
    for dep in re.findall(r'^import (ProvenHashes\.Highway(?:\.\w+)?)$', p.read_text(), re.M):
        visit(dep)
visit('ProvenHashes.Highway')
files = [Path(m.replace('.', '/') + '.lean') for m in sorted(modules)]
names = []
for p in files:
    t = p.read_text()
    assert not re.search(r'\b(sorry|admit|native_decide)\b', t), p
    assert not re.search(r'^\s*(axiom|unsafe)\b', t, re.M), p
    names += ['ProvenHashes.Highway.' + n for n in re.findall(r'^(?:theorem|lemma)\s+(\w+)', t, re.M)]
for n in ('wordHalves', 'keyCoordinates', 'padEquiv', 'xorCoordinates', 'key1Coordinates',
          'byteCoordinates', 'fullKeyCoordinates'):
    names.append('ProvenHashes.Highway.' + n)
for struct in ('Lane', 'PairState', 'State'):
    names += [f'ProvenHashes.Highway.{struct}.{suffix}' for suffix in ('ext', 'ext_iff')]
if '--generate' in sys.argv:
    Path('HighwayAudit.lean').write_text('import ProvenHashes.Highway\n\n' + '\n'.join(
        f'#check @{n}\n#print axioms {n}' for n in names) + '\n')
    raise SystemExit
s = Path('HighwayAudit.txt').read_text()
rows = re.findall(r"'([^']+)' depends on axioms: \[([^\]]*)\]", s)
rows += [(n, '') for n in re.findall(r"'([^']+)' does not depend on any axioms", s)]
assert {n for n, _ in rows} == set(names), 'Missing axiom output'
allowed = {'propext', 'Classical.choice', 'Quot.sound'}
for name, axioms in rows:
    found = {a.strip() for a in axioms.split(',') if a.strip()}
    assert found <= allowed, (name, found)
assert not re.search(r'error:|sorryAx|Lean.ofReduceBool', s)
c_rows = Path('tests/c-vectors.txt').read_text().splitlines()
lean_rows = Path('tests/lean-vectors.txt').read_text().splitlines()
assert c_rows == lean_rows and len(c_rows) == 12, 'Vector comparison failed'
for row in c_rows:
    values = ast.literal_eval(row)
    assert len(values) == 23 and all(isinstance(x, int) and 0 <= x < 2**64 for x in values)
print(f'Highway audit passed: {len(rows)} proof-bearing declarations; 12 complete C/Lean vector rows agree.')
