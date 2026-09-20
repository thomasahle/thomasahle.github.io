#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
test "$(uname -s)" = Linux
ulimit -v 31250000
export OMP_NUM_THREADS=8
nice -n 10 taskset -c 40-47 python3.12 checks/certify_open_ph_enh.py > checks/certificate.log
nice -n 10 taskset -c 40-47 g++ -O3 -std=c++17 -fopenmp checks/validate_quadratics.cpp -o checks/validate_quadratics
nice -n 10 taskset -c 40-47 ./checks/validate_quadratics > checks/validation.json
nice -n 10 taskset -c 40-47 python3.12 checks/verify_certificate.py > checks/independent_verification.log
nice -n 10 taskset -c 40-47 python3.12 checks/manifest.py
