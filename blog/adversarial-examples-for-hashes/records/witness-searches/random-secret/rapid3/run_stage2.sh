#!/bin/bash
# Stage 2: fresh-RNG confirmations at 2^34 keys and precise fold-primitive rates. Waits for stage 1.
cd "$(dirname "$0")"; mkdir -p logs
while [ ! -f logs/STAGE1_DONE ]; do sleep 20; done
R="nice -n 10 taskset -c 24-31 ./rs_rapid3"
A=9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c
A2=642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c
P24=9bd4604137366abec688a63706aa4a2188d35499de169df6
P24b=642b9fbec8c99541c688a63706aa4a2188d35499de169df6
{ $R foldrate 64 ffffffffffffffff ffffffffffffffff 38 101 8; $R foldrate 64 ffffffffffffffff 0 38 102 8; $R foldrate 64 0 ffffffffffffffff 38 103 8; } > logs/10_foldrate64_2p38.txt 2>&1
{ echo "# 24 B (M,0) candidate, random seed + secret, 2^34, fresh rngseed"; time $R pair 34 201 $P24 $P24b 8; } > logs/11_pair24_random_2p34.txt 2>&1
{ echo "# row pair A, random seed + secret, 2^34, fresh rngseed"; time $R pair 34 202 $A $A2 8; } > logs/12_pairA_random_2p34.txt 2>&1
{ echo "# 24 B (M,0) candidate, DEFAULT secret, 2^34"; time $R pair 34 203 $P24 $P24b 8 default; } > logs/13_pair24_default_2p34.txt 2>&1
{ echo "# row pair A, DEFAULT secret, 2^34, fresh rngseed"; time $R pair 34 204 $A $A2 8 default; } > logs/14_pairA_default_2p34.txt 2>&1
echo done > logs/STAGE2_DONE
