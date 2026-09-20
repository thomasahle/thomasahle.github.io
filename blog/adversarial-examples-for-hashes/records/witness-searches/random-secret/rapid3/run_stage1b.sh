#!/bin/bash
# Stage 1b: trimmed structural sweep (replaces the untrimmed one), then stage 2 follows via its marker.
cd "$(dirname "$0")"; mkdir -p logs
R="nice -n 10 taskset -c 24-31 ./rs_rapid3"
{ time $R sweep 27 24 20 41 8; } > logs/08_sweep.txt 2>&1
echo done > logs/STAGE1_DONE
