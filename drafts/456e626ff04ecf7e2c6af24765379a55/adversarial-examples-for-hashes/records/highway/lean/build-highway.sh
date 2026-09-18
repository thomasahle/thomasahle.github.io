#!/usr/bin/env bash
set -euo pipefail
cd -- "$(dirname -- "$0")"
if [[ "$(uname -s)" != Linux ]]; then
  echo 'Run this build on the Xeon; the Mac is a source mirror.' >&2
  exit 1
fi
source "$HOME/agents/lean-hash/env.sh"
export LEAN_NUM_THREADS=8
run() { taskset -c 80-87 nice -n 10 "$@"; }
test "$(git -C .lake/packages/mathlib rev-parse HEAD)" = f897ebcf72cd16f89ab4577d0c826cd14afaafc7
run lake env lean --version
run sha256sum --check tests/reference.sha256
python3 tests/audit.py --generate
run lake build
run lake env lean -s 131072 HighwayAudit.lean > HighwayAudit.txt
run lake env lean ProvenHashes/Highway/Vectors.lean > tests/lean-vectors.txt
mkdir -p tests/c
cp ../materials/highwayhash.h tests/c/highwayhash.h
run cc -O2 -std=c11 -Wall -Wextra -Itests tests/vectors.c -o tests/vectors
run tests/vectors > tests/c-vectors.txt
diff -u tests/c-vectors.txt tests/lean-vectors.txt
python3 tests/audit.py
