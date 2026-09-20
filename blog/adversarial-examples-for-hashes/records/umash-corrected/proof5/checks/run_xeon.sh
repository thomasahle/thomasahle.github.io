#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
test "$(uname -s)" = Linux
ulimit -v 31250000
export OMP_NUM_THREADS=8
export OPENBLAS_NUM_THREADS=1
run() { nice -n 10 taskset -c 40-47 "$@"; }
run python3.12 checks/certify_fingerprint.py > checks/certificate.log
run gcc -O2 -shared -fPIC -mpclmul -msse2 -DUMASH_DYNAMIC_DISPATCH=0 -DUMASH_LONG_INPUTS=0 checks/c_bridge.c -o checks/c_bridge_scalar.so
run gcc -O2 -shared -fPIC -mpclmul -msse2 -DUMASH_DYNAMIC_DISPATCH=0 -DUMASH_LONG_INPUTS=1 checks/c_bridge.c -o checks/c_bridge_optimized.so
run python3.12 checks/check_c_model.py > checks/model.log
run python3.12 checks/verify_certificate.py > checks/verification.log
run python3.12 checks/manifest.py
