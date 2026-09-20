#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
test "$(uname -s)" = Linux
ulimit -v 31250000
export OMP_NUM_THREADS=8
run() { nice -n 10 taskset -c 40-47 "$@"; }
run python3.12 checks/prepare_masks.py
run g++ -O3 -std=c++17 -fopenmp checks/ph_low_table.cpp -o checks/ph_low_table
run ./checks/ph_low_table > checks/ph_low_table.json
run python3.12 checks/certify_ph_linear.py > checks/ph_linear.log
run python3.12 checks/certify_ph_high.py > checks/ph_high.log
run python3.12 checks/certify_enh_weights.py > checks/enh.log
run python3.12 checks/certify_tail.py > checks/tail.log
run g++ -O3 -std=c++17 -fopenmp checks/validate_new.cpp -o checks/validate_new
run ./checks/validate_new > checks/validation.json
run g++ -O3 -std=c++17 -fopenmp checks/verify_ph_low.cpp -o checks/verify_ph_low
run ./checks/verify_ph_low > checks/ph_low_independent.json
run python3.12 checks/verify_certificates.py > checks/verification.log
run python3.12 checks/certify_envelope.py > checks/envelope.log
run python3.12 checks/manifest.py
