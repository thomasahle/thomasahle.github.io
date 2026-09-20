#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/../.."
for b in 1 2 4 8; do
  (
    nice -n 10 ./witness "$b" 268435456 5 1 > "certificates/logs/condition-b${b}.jsonl"
    nice -n 10 ./witness "$b" 68719476736 5 0 > "certificates/logs/witness-b${b}.jsonl"
  ) &
done
wait
printf 'finished\n'
