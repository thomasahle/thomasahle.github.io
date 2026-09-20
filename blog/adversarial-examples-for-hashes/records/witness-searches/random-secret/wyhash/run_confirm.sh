#!/bin/bash
cd "$(dirname "$0")"
until [ -f DONE_ALL ]; do sleep 15; done
T="nice -n 10 taskset -c 24-31 ./wyrs"
A=9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c
B=642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c
$T pair $A $B 35 1 301 8 > confirmA_mode1_2p35.log 2>&1
touch DONE_CONFIRM_A
