#!/bin/bash
# Phase A screen: every single-bit, two-bit and structured difference, all four variants.
# W=1 (8-byte messages): 2^24 random keys per difference; W=2 (16-byte): 2^22.
# 8 worker processes pinned to cores 24-31, nice 10.
set -e; cd "$(dirname "$0")"
clang -O3 -march=native -o sipdiff sipdiff.c && ./sipdiff selftest | tee selftest.log
for W in 1 2; do cat d${W}_single.txt d${W}_double.txt d${W}_struct.txt > d${W}_all.txt; rm -f d${W}_all.part*; split -n l/8 -d d${W}_all.txt d${W}_all.part; done
mkdir -p phaseA; date > phaseA/started.txt
for v in 11 12 13 24; do for W in 1 2; do
  lg=$([ $W = 1 ] && echo 24 || echo 22)
  ls d${W}_all.part* | nice -n 10 taskset -c 24-31 xargs -P 8 -I{} sh -c "./sipdiff $v $W $lg 0x${v}${W}7 R {} > phaseA/v${v}_W${W}_{}.out"
  echo "done v=$v W=$W lg=$lg $(date)" | tee -a phaseA/progress.txt
done; done
date > phaseA/finished.txt
