#!/usr/bin/env bash
set -euo pipefail
cd -- "$(dirname -- "$0")"
if [[ -f ../env.sh ]]; then
  source ../env.sh
fi
export LEAN_NUM_THREADS=8
nice -n 10 taskset -c 72-79 lake build
python3 audit_polymur.py
nice -n 10 taskset -c 72-79 lake env lean PolymurAudit.lean > PolymurAxioms.txt
python3 check_polymur_audit.py
