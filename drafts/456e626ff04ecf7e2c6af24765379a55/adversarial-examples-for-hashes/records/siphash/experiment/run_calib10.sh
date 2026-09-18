#!/bin/bash
# Calibration of the full-collision channel: SipHash-1-0 (one compression round, NO finalization round;
# 2 SipRounds total for an 8-byte message), all 8-byte differences at 2^24 random keys, random base message.
set -e; cd "$(dirname "$0")"; clang -O3 -march=native -o sipdiff sipdiff.c && ./sipdiff selftest
mkdir -p calib10; ls d1_all.part* | nice -n 10 taskset -c 24-31 xargs -P 8 -I{} sh -c "./sipdiff 10 1 24 0x1017 R {} > calib10/v10_W1_{}.out"; date > calib10/finished.txt
