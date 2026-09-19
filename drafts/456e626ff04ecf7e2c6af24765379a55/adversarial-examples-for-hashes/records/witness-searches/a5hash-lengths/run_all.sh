#!/bin/bash
# a5hash-64 length search: exact low-33-bit histogram + beam extension per (len, v). 8 cores, nice 10.
cd <xeon-work>/witness/a5hash-lengths
run(){ len=$1; v=$2; shift 2; f=logs/${len}_${v}.log; [ -s $f ] && grep -q '^control' $f && { echo "skip $f"; return; }
  echo "=== $(date -u +%FT%TZ) modebeam $len $v $*" | tee -a logs/driver.log
  ( time nice -n 10 taskset -c 24-31 ./modebeam $len $v -t 8 -o logs/pair_${len}_${v}.txt "$@" ) > $f 2>&1
  grep -E 'EXACT|BEAM|score|verification|control' $f | tee -a logs/driver.log; }
# validation of the beam path against the recorded len-23 (156800 seeds) and len-8 (7290) classes
run 23 13
run 8 1
# the memo's named point and the next candidates by estimated score
run 4567 31
run 471 23
run 2519 29
run 6615 35
run 215 19
run 3 9
run 2 7
run 151 17
run 87 15
run 5079 29
run 14807 33
run 343 19
run 19 9
run 3543 27
run 135 15
run 2263 25
run 12759 31
run 1 3
run 31 9
run 55 11
run 15 7
run 21 7
run 18 7
run 22 7
run 4439 27
run 4951 27
run 471 21
run 2519 27
run 983 21
run 4567 29
run 199 15
run 3287 25
run 119 13
run 39 9
run 375 17
run 71 11
run 6487 27
run 407 17
run 131 13
run 2199 23
run 6999 27
run 22999 31
run 147 13
run 2455 23
run 9 3
run 10 3
run 11 5
run 12 1
run 13 5
run 14 3
run 16 1
run 17 3
echo ALLDONE | tee -a logs/driver.log
