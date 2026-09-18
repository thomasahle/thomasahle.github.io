#!/bin/bash
# Phase B: the phase-A candidates (top |z| / any collision / low16 excess per variant and width),
# re-measured at 2^28 random keys in three base-message modes:
#   R   random base message per key (differential average over messages)
#   F0  fixed base message m = 0        -> a FIXED PAIR (0, D) over random keys
#   FF  fixed base message m = ff..ff   -> a FIXED PAIR (ff.., ff..^D)
# Input: candidates/v<var>_W<W>.txt (difference-list format). 8 workers on cores 24-31, nice 10.
set -e; cd "$(dirname "$0")"; mkdir -p phaseB; date > phaseB/started.txt
LG=${LG:-28}
jobs=phaseB/jobs.txt; : > $jobs
for f in candidates/v*_W*.txt; do b=$(basename $f .txt); v=${b#v}; v=${v%%_*}; W=${b##*_W}
  for mode in R F0 "Fffffffffffffffff,ffffffffffffffff"; do
    tag=$mode; [ ${#tag} -gt 3 ] && tag=FF
    echo "./sipdiff $v $W $LG 0x${v}${W}b $mode $f > phaseB/${b}_${tag}.out" >> $jobs
  done
done
nice -n 10 taskset -c 24-31 xargs -P 8 -I{} sh -c "{}" < $jobs
date > phaseB/finished.txt
