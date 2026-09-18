#!/usr/bin/env bash
set -euo pipefail
cd -- "$(dirname -- "$0")"
if [[ -f ../env.sh ]]; then source ../env.sh; fi
export LEAN_NUM_THREADS=8
python3 generate_audit.py
./build.sh 2>&1 | tee Reproduction.log
nice -n 10 taskset -c 40-47 lake env lean BRWAudit.lean > BRWAudit.txt
python3 - <<'PY'
from pathlib import Path
import re
s=Path('BRWAudit.txt').read_text()
expected=set(re.findall(r'#print axioms (\S+)',Path('BRWAudit.lean').read_text()))
rows=re.findall(r"'([^']+)' depends on axioms: \[([^\]]*)\]",s)
assert {name for name,_ in rows}==expected
for name,axioms in rows:
    assert {a.strip() for a in axioms.split(',') if a.strip()} <= {'propext','Classical.choice','Quot.sound'},(name,axioms)
assert not re.search(r'error:|sorryAx|Lean.ofReduceBool',s)
print(f'BRW/eight-lane audit passed: {len(rows)} theorems and lemmas.')
PY
