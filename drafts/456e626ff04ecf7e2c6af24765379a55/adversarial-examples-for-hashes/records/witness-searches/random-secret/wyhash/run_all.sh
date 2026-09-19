#!/bin/bash
# Driver: wyhash final v4.3 under the random-secret model, 8 cores (taskset 24-31), nice 10.
cd "$(dirname "$0")"
T="nice -n 10 taskset -c 24-31 ./wyrs"
A=9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c
B=642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c
{
echo "== stage A: row pair A under key models (mode1 uniform words, mode2 odd words, mode3 make_secret table 2^17, mode0 default secret)"
$T pair $A $B 32 1 101 8
$T pair $A $B 31 2 102 8
$T pair $A $B 31 0 103 8
$T pair $A $B 31 3 104 8 17
} > stageA.log 2>&1
$T batch batch_struct.txt 30 1 201 8 > struct.log 2>&1
$T batch batch_xlen.txt 28 1 202 8 > xlen.log 2>&1
$T batch batch_site24.txt 29 1 203 8 > site24.log 2>&1
$T batch batch_site32.txt 29 1 204 8 > site32.log 2>&1
$T batch batch_sweep_w.txt 29 1 205 8 > sweep_w.log 2>&1
$T batch batch_sweep_b.txt 27 1 206 8 > sweep_b.log 2>&1
touch DONE_ALL
