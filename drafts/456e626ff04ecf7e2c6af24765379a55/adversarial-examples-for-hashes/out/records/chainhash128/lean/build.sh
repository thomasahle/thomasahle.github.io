#!/usr/bin/env bash
set -euo pipefail
cd -- "$(dirname -- "$0")"
source ../env.sh
export LEAN_NUM_THREADS=8
run() { nice -n 10 taskset -c 48-55 "$@"; }
run python3 generate_modulus_certificate.py
run lake build
run python3 ../scripts/audit128.py
run lake env lean ChainHash128Audit.lean > ChainHash128Audit.txt
run python3 vectors/generate.py
run lake env lean --run vectors/Main.lean vectors/corpus.txt > vectors/lean.txt
run cc -O2 -mpclmul -msse4.1 vectors/header.c -o vectors/header
run vectors/header < vectors/corpus.txt > vectors/header.txt
run python3 ../scripts/verify_audit128.py
