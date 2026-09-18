#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
if [[ "$(uname -s)" != Linux ]]; then
  echo 'Run this script on the Xeon in ~/agents/lean-multishift.' >&2
  exit 1
fi
source ./env.sh
export LEAN_NUM_THREADS=8
cd lean
if rg -n '\b(sorry|admit|native_decide|axiom|unsafe)\b' ProvenHashes; then
  echo 'Forbidden declaration or proof escape detected.' >&2
  exit 1
fi
nice -n 10 taskset -c 64-71 lake build 2>&1 | tee ../multishift-build.log
nice -n 10 taskset -c 64-71 lake env lean MultishiftAudit.lean 2>&1 | tee ../multishift-axioms.log
python3 - <<'PY'
import pathlib, re
expected = re.findall(r'^#print axioms (\S+)', pathlib.Path('MultishiftAudit.lean').read_text(), re.M)
log = pathlib.Path('../multishift-axioms.log').read_text()
found = re.findall(r"'([^']+)' depends on axioms: \[([^\]]*)\]", log)
allowed = {'propext', 'Classical.choice', 'Quot.sound'}
assert {name for name, _ in found} == set(expected), 'Incomplete axiom audit'
for name, axioms in found:
    actual = {a.strip() for a in axioms.split(',') if a.strip()}
    assert actual <= allowed, (name, actual - allowed)
print(f'AUDIT PASSED: {len(found)} theorems; only the three standard axioms.')
PY
