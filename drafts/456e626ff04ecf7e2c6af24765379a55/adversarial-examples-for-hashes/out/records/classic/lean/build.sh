#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
if [[ -d "$HOME/agents/lean-hash/.elan" ]]; then
  export ELAN_HOME="$HOME/agents/lean-hash/.elan"
  export PATH="$ELAN_HOME/bin:$PATH"
fi
export LEAN_NUM_THREADS=8
taskset -c 72-79 nice -n 10 lake build > Build.txt 2>&1
python3 MakeAudit.py
taskset -c 72-79 nice -n 10 lake env lean AuditAll.lean > AuditAll.txt 2>&1
python3 VerifyAudit.py > Verification.json
taskset -c 72-79 nice -n 10 lake env lean --version > Toolchain.txt
git -C .lake/packages/mathlib rev-parse HEAD >> Toolchain.txt
cat Verification.json
