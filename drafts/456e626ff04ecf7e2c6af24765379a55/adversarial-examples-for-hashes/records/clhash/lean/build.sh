#!/usr/bin/env bash
set -euo pipefail
cd -- "$(dirname -- "$0")"
source ../env.sh
export LEAN_NUM_THREADS=8
# Allocation supplied for CLHASH goal-loop round 2.
clhash_cpuset=${CPUSET:-48-55}
run_lane() { nice -n 10 taskset -c "$clhash_cpuset" "$@"; }
# Fail before doing any work if the allocation is unavailable.
run_lane true
run_lane python3 generate_clhash_certificate.py
run_lane lake build
run_lane python3 MakeCLHashAudit.py
run_lane lake env lean CLHashAudit.lean > CLHashAudit.txt
run_lane lake env lean FullAudit.lean > FullAudit.txt
run_lane python3 - <<'PY'
from pathlib import Path
import re
allowed = {'propext', 'Classical.choice', 'Quot.sound'}
for source in Path('ProvenHashes').glob('*.lean'):
    s = source.read_text()
    assert not re.search(r'\b(sorry|admit|native_decide)\b', s), source
    assert not re.search(r'^\s*(axiom|unsafe)\b', s, re.M), source
for stem in ('FullAudit', 'CLHashAudit'):
    s = Path(stem + '.txt').read_text()
    rows = re.findall(r"'([^']+)' depends on axioms: \[([^\]]*)\]", s)
    rows += [(n, '') for n in re.findall(r"'([^']+)' does not depend on any axioms", s)]
    expected = set(re.findall(r'#print axioms (\S+)', Path(stem + '.lean').read_text()))
    assert {n for n, _ in rows} == expected, 'Missing axiom output: ' + stem
    for name, axioms in rows:
        assert {a.strip() for a in axioms.split(',') if a.strip()} <= allowed, (name, axioms)
    assert not re.search(r'error:|sorryAx|Lean.ofReduceBool', s)
    print(f'{stem}: checked {len(rows)} declarations')
print('Build and axiom audit finished; the specification is proved by')
print('ProvenHashes.CLHash.collision_bound and collision_bound_bytes.')
PY
run_lane python3 CheckCLHashVectors.py
