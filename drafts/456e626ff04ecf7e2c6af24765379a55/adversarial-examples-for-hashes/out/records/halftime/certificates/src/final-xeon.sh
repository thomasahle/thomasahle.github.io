#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/../.."
python3 certificates/src/check_objects.py api.o api-unsigned512.o
bash certificates/src/verify-xeon.sh
nice -n 10 g++ -std=c++17 -O2 -Wno-ignored-attributes certificates/src/matrix.cpp -o matrix
./matrix > certificates/generator-matrices.jsonl
python3 certificates/src/rank.py certificates/generator-matrices.jsonl
nice -n 10 g++ -std=c++17 -O2 -march=native -Wno-ignored-attributes certificates/src/flips.cpp -o flips
./flips > certificates/logs/flips.jsonl
sha256sum halftime-hash.hpp api.o api-unsigned512.o witness witness-final > certificates/logs/witness-final-build-sha256.txt
mv certificates/logs/condition-b8.jsonl certificates/logs/condition-b8-before-unsigned512.jsonl
nice -n 10 ./witness-final 8 268435456 24 1 > certificates/logs/condition-b8.jsonl
nice -n 10 ./witness-final 8 68719476736 24 0 > certificates/logs/witness-b8-final.jsonl
mv certificates/bench/Xeon8375C certificates/bench/Xeon8375C-before-unsigned512
python3 certificates/bench/run_speed.py Xeon8375C "$HOME/agents/speedbench-hhfixed/build/SMHasher3" certificates/bench/Xeon8375C
printf 'Final AVX-512 witness and final Xeon benchmark batch completed.\n'
