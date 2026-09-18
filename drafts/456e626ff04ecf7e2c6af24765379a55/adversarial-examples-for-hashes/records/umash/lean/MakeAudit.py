"""Generate #check / #print axioms for every local theorem and lemma."""
from pathlib import Path
import re

names = []
for path in sorted(Path('ProvenHashes').glob('*.lean')):
    source = path.read_text()
    namespace = re.search(r'^namespace (\S+)', source, re.M).group(1)
    names += [namespace + '.' + n for n in re.findall(
        r'^(?:@\[[^\n]*\]\s*)?(?:theorem|lemma)\s+(\w+)', source, re.M)]
Path('AuditAll.lean').write_text('import ProvenHashes\n\n' + '\n'.join(
    f'#check {n}\n#print axioms {n}' for n in names) + '\n')
print(f'Auditing {len(names)} local theorems and lemmas')
