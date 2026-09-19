#!/bin/bash
# Stage 3: (a) re-measure the sweep's one apparent outlier (len 31 cw0: 4/2^27) at 2^30;
# (b) second fresh 2^35 samples of the two candidate pairs under the random-secret model;
# (c) a second 2^38 sample of the primitive P(M,0) with a different RNG seed.
cd "$(dirname "$0")"; mkdir -p logs
R="nice -n 10 taskset -c 24-31 ./rs_rapid3"
A=9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c
A2=642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c
P24=9bd4604137366abec688a63706aa4a2188d35499de169df6
P24b=642b9fbec8c99541c688a63706aa4a2188d35499de169df6
{ echo "# len 31 cw0 (sweep outlier), random seed + secret, 2^30, fresh rngseed"; time $R sweeppair 31 0 30 301 8; } > logs/20_len31_cw0_2p30.txt 2>&1
{ echo "# 24 B (M,0) candidate, random seed + secret, 2^35, second fresh rngseed"; time $R pair 35 205 $P24 $P24b 8; } > logs/21_pair24_random_2p35.txt 2>&1
{ echo "# row pair A, random seed + secret, 2^35, second fresh rngseed"; time $R pair 35 206 $A $A2 8; } > logs/22_pairA_random_2p35.txt 2>&1
{ $R foldrate 64 ffffffffffffffff 0 38 104 8; } > logs/23_foldrate64_M0_2p38_b.txt 2>&1
echo done > logs/STAGE3_DONE
