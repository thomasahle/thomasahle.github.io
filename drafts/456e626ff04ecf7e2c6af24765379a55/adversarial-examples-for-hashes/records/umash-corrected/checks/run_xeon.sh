#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
case "$(hostname)" in
  hardware|<xeon-host>) ;;
  *) echo 'Run this certificate on <xeon-host>.' >&2; exit 1 ;;
esac
ulimit -v 31250000  # 32,000,000,000 bytes, including the decimal-GB reading.
export OMP_NUM_THREADS=8
run() { nice -n 10 taskset -c 56-63 "$@"; }
run python3 checks/triangular.py
run python3 checks/certify_constants.py
run g++ -O2 -std=c++17 -fopenmp checks/exhaustive_lift.cpp -o checks/exhaustive_lift
run checks/exhaustive_lift > checks/exhaustive_lift.json
run python3 checks/manifest.py
