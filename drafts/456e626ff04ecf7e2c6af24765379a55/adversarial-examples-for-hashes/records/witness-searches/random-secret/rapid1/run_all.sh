#!/bin/bash
# Reproduction script: rapidhash v1.0 under the random-secret key model.
# Runs on 8 cores (edit CORES/NICE for your machine).  Each stage logs to logs/.
set -u
CORES=${CORES:-24-31}; NICE=${NICE:-10}; T=8
RUN="nice -n $NICE taskset -c $CORES ./rs_rapid1"
mkdir -p logs
g++ -O3 -std=c++17 -march=native -pthread rs_rapid1.cpp -o rs_rapid1 || exit 1
sha256sum rapidhash.h rs_rapid1.cpp > logs/sha256.txt
$RUN selftest | tee logs/00_selftest.txt || exit 1
A1=9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c
A2=642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c
B1=9bd4604137366abec688a63706aa4a2188d35499de169df6            # 24 B: bytes 0..23 of M
B2=642b9fbec8c99541c688a63706aa4a2188d35499de169df6            # word 0 complemented only
C2=9bd4604137366abe397759c8f955b5de88d35499de169df633e0964e8c04600c   # 32 B, word 1 complemented only
D2=642b9fbec8c99541c688a63706aa4a2188d35499de169df633e0964e8c04600c   # 32 B, word 0 complemented only
M=ffffffffffffffff; Z=0000000000000000; TOP=8000000000000000
{ echo "== stage 1: row pair A, random seed + random secret, 2^32 keys, rngseed 1"; $RUN pair $A1 $A2 32 1 random $T; } 2>&1 | tee logs/01_pairA_random.txt
{ echo "== stage 2: 24-byte pair B (word 0 complemented), random model, 2^32 keys, rngseed 1"; $RUN pair $B1 $B2 32 1 random $T; } 2>&1 | tee logs/02_pairB24_random.txt
{ echo "== stage 3: fold-level differentials, uniform operands, 2^38 samples";
  for d in "$M $M" "$M $Z" "$Z $M"; do $RUN fold $d 38 1 $T; done; } 2>&1 | tee logs/03_fold_main.txt
{ echo "== stage 4: structural fold candidates, 2^35 samples";
  for d in "$M ${M%f}e" "${M%f}e $M" "$M $TOP" "$TOP $TOP" "$TOP $Z" "7fffffffffffffff $M" "$M 7fffffffffffffff" "7fffffffffffffff 7fffffffffffffff" "fffffffffffffffe fffffffffffffffe" "aaaaaaaaaaaaaaaa aaaaaaaaaaaaaaaa" "5555555555555555 5555555555555555" "0000000000000001 0000000000000001" "0000000000000001 $Z" "00000000ffffffff 00000000ffffffff" "ffffffff00000000 ffffffff00000000" "00000000ffffffff $Z" "ffffffff00000000 $Z" "$M ffffffff00000000" "$M 00000000ffffffff" "8000000000000001 8000000000000001" "c000000000000000 c000000000000000" "fffffffffffffffc fffffffffffffffc" "fffffffffffffff0 fffffffffffffff0" "ffffffffffffff00 ffffffffffffff00" "ffffffffffff0000 ffffffffffff0000"; do $RUN fold $d 35 1 $T; done; } 2>&1 | tee logs/04_fold_struct.txt
{ echo "== stage 5: hill-climb (128 single-bit neighbours) around (M,M) and (M,0), 2^33 samples each"; $RUN hill $M $M 33 1 $T; $RUN hill $M $Z 33 1 $T; } 2>&1 | tee logs/05_hill.txt
{ echo "== stage 6: hash-level scan, lengths 1..64 + block boundaries, 2^26 keys per candidate"; $RUN scan 26 1 $T 64; } 2>&1 | tee logs/06_scan.txt
{ echo "== stage 7: controls: pair A default secret 2^30 (row figure 12/2^30); pair A odd-secret 2^30; pair B24 default secret 2^30; 32-byte single-word complements random 2^32";
  $RUN pair $A1 $A2 30 1 default $T; $RUN pair $A1 $A2 30 1 odd $T; $RUN pair $B1 $B2 30 1 default $T; $RUN pair $A1 $D2 32 1 random $T; $RUN pair $A1 $C2 32 1 random $T; } 2>&1 | tee logs/07_controls.txt
{ echo "== stage 8: fresh confirmation samples, rngseed 2, 2^32 keys"; $RUN pair $A1 $A2 32 2 random $T; $RUN pair $B1 $B2 32 2 random $T; } 2>&1 | tee logs/08_confirm.txt
echo DONE > logs/DONE
