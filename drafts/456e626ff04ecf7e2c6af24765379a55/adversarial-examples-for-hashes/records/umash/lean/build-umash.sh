#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
export ELAN_HOME="$HOME/agents/lean-hash/.elan"
export PATH="$ELAN_HOME/bin:$PATH"
export LEAN_NUM_THREADS=8
mkdir -p logs
nice -n 10 taskset -c 56-63 lake build "$@"
