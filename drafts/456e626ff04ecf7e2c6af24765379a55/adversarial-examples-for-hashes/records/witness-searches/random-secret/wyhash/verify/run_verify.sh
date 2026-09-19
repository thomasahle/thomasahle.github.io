#!/bin/bash
# Independent re-measurement on the Xeon: 8 cores, low priority.
cd "$(dirname "$0")"
g++ -O2 -std=c++17 -pthread -o wyverify wyverify.cpp || exit 1
R="nice -n 10 taskset -c 24-31 ./wyverify"
A=9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c
B=642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c
C=38ae2f8efa92bafc796ada2e8cdcc1f30ce3f58993dd4f2e
D=c751d071056d4503796ada2e8cdcc1f30ce3f58993dd4f2e
$R $A $B 35 8 1001 mt random > pairA_mt_2p35.log 2>&1
$R $C $D 35 8 1002 mt random > pair24_mt_2p35.log 2>&1
$R $A $B 35 8 2001 sm random > pairA_sm_2p35.log 2>&1
$R $C $D 35 8 2002 sm random > pair24_sm_2p35.log 2>&1
$R $A $B 31 8 3001 mt default > pairA_mt_default_2p31.log 2>&1
$R $C $D 31 8 3002 mt default > pair24_mt_default_2p31.log 2>&1
touch DONE_VERIFY
