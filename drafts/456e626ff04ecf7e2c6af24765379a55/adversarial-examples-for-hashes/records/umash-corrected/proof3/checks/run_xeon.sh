#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
test "$(uname -s)" = Linux
ulimit -v 31250000
export OMP_NUM_THREADS=8
nice -n 10 taskset -c 40-47 python3.12 checks/certify_primary.py > checks/certificate.log
nice -n 10 taskset -c 40-47 g++ -O3 -std=c++17 -fopenmp checks/validate_primary.cpp -o checks/validate_primary
nice -n 10 taskset -c 40-47 ./checks/validate_primary > checks/validation.json
nice -n 10 taskset -c 40-47 python3.12 checks/verify_certificate.py > checks/independent_verification.json
nice -n 10 taskset -c 40-47 python3.12 checks/manifest.py
