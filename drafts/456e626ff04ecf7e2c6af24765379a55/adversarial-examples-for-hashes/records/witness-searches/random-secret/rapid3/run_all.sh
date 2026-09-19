#!/bin/bash
# Stage 1 of the rapidhash v3 random-secret study. Runs on 8 cores, sequentially.
cd "$(dirname "$0")"; mkdir -p logs
R="nice -n 10 taskset -c 24-31 ./rs_rapid3"
A=9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c
A2=642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c
D=${A}48c651edae76208e840fc51f1cccbb02
D2=${A2}48c651edae76208e840fc51f1cccbb02
# 24-byte candidate: first 24 bytes of pair A, only word 0 complemented (M,0) on the first fold
P24=9bd4604137366abec688a63706aa4a2188d35499de169df6
P24b=642b9fbec8c99541c688a63706aa4a2188d35499de169df6
./check_witness > logs/00_check_witness.txt 2>&1
{ echo "# row pair A, DEFAULT secret (seed-only model), 2^30"; time $R pair 30 11 $A $A2 8 default; } > logs/01_pairA_default_2p30.txt 2>&1
{ echo "# row pair A, random seed + secret, 2^30"; time $R pair 30 12 $A $A2 8; } > logs/02_pairA_random_2p30.txt 2>&1
{ echo "# row pair D (48 B), random seed + secret, 2^30"; time $R pair 30 13 $D $D2 8; } > logs/03_pairD_random_2p30.txt 2>&1
{ echo "# 24 B (M,0) candidate, random seed + secret, 2^30"; time $R pair 30 14 $P24 $P24b 8; } > logs/04_pair24_random_2p30.txt 2>&1
{ $R foldrate 64 ffffffffffffffff ffffffffffffffff 32 21 8; $R foldrate 64 ffffffffffffffff 0 32 22 8; $R foldrate 64 0 ffffffffffffffff 32 23 8; } > logs/05_foldrate64_2p32.txt 2>&1
{ time $R fold10; } > logs/06_fold10.txt 2>&1
{ time $R climb 32 22 31 6 8; } > logs/07_climb32.txt 2>&1
{ time $R sweep 28 41 8; } > logs/08_sweep_2p28.txt 2>&1
echo done > logs/STAGE1_DONE
