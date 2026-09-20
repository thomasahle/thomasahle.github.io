#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/../.."
for backend in scalar sse2 avx2 avx512; do
  case "$backend" in
    scalar) flags='-DHH_SCALAR -fno-tree-vectorize';;
    sse2) flags='-msse2 -mno-avx';;
    avx2) flags='-mavx2 -mno-avx512f';;
    avx512) flags='-march=native';;
  esac
  nice -n 10 g++ -std=c++17 -O2 -Wno-ignored-attributes $flags certificates/src/verify.cpp -o "verify-$backend" 2> "certificates/logs/build-$backend.txt"
  nice -n 10 "./verify-$backend" "vectors-$backend.bin" > "certificates/logs/verify-$backend.json"
  sha256sum "vectors-$backend.bin" >> certificates/logs/vector-sha256.txt
done
cmp vectors-scalar.bin vectors-sse2.bin
cmp vectors-scalar.bin vectors-avx2.bin
cmp vectors-scalar.bin vectors-avx512.bin
nice -n 10 ./verify-avx512 guard > certificates/logs/guard.jsonl 2> certificates/logs/guard-summary.txt
nice -n 10 clang++ -std=c++17 -O1 -g -march=native -Wno-ignored-attributes -fsanitize=undefined -fno-sanitize-recover=all certificates/src/verify.cpp -o verify-ubsan 2> certificates/logs/build-ubsan.txt
nice -n 10 ./verify-ubsan > certificates/logs/ubsan.json 2> certificates/logs/ubsan.txt
printf 'passed\n'
