#!/bin/bash
# Fold-only sweep of structured candidates at 2^LG seeds, one process per listed core.
# usage: run_sweep.sh LG IN OUT "27 28 29 30 31"
set -e
cd <xeon-work>/witness/xxh3-64-pair
LG=$1; IN=$2; OUT=$3; CORES=($4); NC=${#CORES[@]}
split -n l/$NC -d "$IN" chunk_$OUT.
i=0
for c in "${CORES[@]}"; do
  nice -n 10 taskset -c $c ./foldsearch $LG 1 < chunk_$OUT.0$i > out_$OUT.0$i.txt &
  i=$((i+1))
done
wait
cat out_$OUT.0*.txt > "$OUT"; rm -f chunk_$OUT.0* out_$OUT.0*
echo done > "$OUT.done"
