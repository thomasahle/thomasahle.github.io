#!/usr/bin/env python3
"""Validate generated Polymur signatures and transitive axiom reports."""
from pathlib import Path
import re
import sys

audit = Path(sys.argv[1] if len(sys.argv) > 1 else 'PolymurAxioms.txt')
source = Path('PolymurAudit.lean').read_text()
output = audit.read_text()
expected = re.findall(r'^#print axioms (\S+)', source, re.M)
rows = re.findall(r"'([^']+)' depends on axioms: \[([^\]]*)\]", output)
no_axioms = re.findall(r"'([^']+)' does not depend on any axioms", output)
assert len(expected) == len(set(expected)), 'Duplicate audited name'
assert set(expected) == {n for n, _ in rows} | set(no_axioms), 'Missing axiom output'
allowed = {'propext', 'Classical.choice', 'Quot.sound'}
for name, axioms in rows:
    found = {a.strip() for a in axioms.split(',') if a.strip()}
    assert found <= allowed, (name, found)
assert not re.search(r'error:|warning:|sorryAx|Lean.ofReduceBool', output), audit
print(f'Polymur axiom audit passed ({len(expected)} proof declarations).')
