#!/bin/bash
# Driver: XXH3-64 random-secret experiments on the Xeon (8 threads on cores 24-31, nice 10; the cores were shared
# with other jobs, so sample sizes were chosen for ~8,000 core-seconds). Logs go to logs/.
cd "$(dirname "$0")"
mkdir -p logs
R="nice -n 10 taskset -c 24-31 ./xxh3_rs"
T=8
NAF=0000000000000000000000000000000051151210404400000000008204000105
NAF2=00000000000000000000000000000000aeeaedefbfbbffffffffff7dfbfffefa
Z24=000000000000000000000000000000000000000000000000
W0=ffffffffffffffff00000000000000000000000000000000
W1=0000000000000000ffffffffffffffff0000000000000000
Z32=0000000000000000000000000000000000000000000000000000000000000000
B1=00000000000000000000000000000000ffffffffffffffffffffffffffffffff
{
echo "host: $(hostname)  cpu: $(grep -m1 'model name' /proc/cpuinfo | cut -d: -f2)"; echo "gcc: $(gcc --version | head -1)"; sha256sum xxhash.h xxh3_rs.c; date -u
} > logs/env.txt
# 1. the row's current pair under the random-secret models (and the two seed-only APIs as controls)
$R pair both   35 101 $T $NAF $NAF2 > logs/pair_naf_both_2p35.txt 2>&1
$R pair seed   28 103 $T $NAF $NAF2 > logs/pair_naf_seed_2p28.txt 2>&1
$R pair ss     28 104 $T $NAF $NAF2 > logs/pair_naf_ss_2p28.txt 2>&1
# 2. structural candidate: 24-byte pair, first word complemented (single fold, XOR difference (M,0)), L=3
$R pair secret 35 112 $T $Z24 $W0 > logs/pair_w0_24_secret_2p35.txt 2>&1
$R pair both   35 111 $T $Z24 $W0 > logs/pair_w0_24_both_2p35.txt 2>&1
touch logs/PAIRS_DONE
# 3. the fold differential itself, precisely
$R fold 36 121 $T ffffffffffffffff ffffffffffffffff ffffffffffffffff 0000000000000000 > logs/fold_precise_2p36.txt 2>&1
touch logs/FOLD_DONE
# 4. structured scan, lengths 1..40 (main candidates 2^27, cheap ones 2^25)
$R scan both 27 131 $T 1 40 25 > logs/scan_1_40.txt 2>&1
touch logs/SCAN1_DONE
# 5. hill-climb on the fold differential (2^30 screening, re-measure at 2^32)
$R foldclimb 30 141 $T 2 > logs/foldclimb.txt 2>&1
touch logs/CLIMB_DONE
# 6. secondary pairs and block boundaries / long lengths
$R pair secret 33 102 $T $NAF $NAF2 > logs/pair_naf_secret_2p33.txt 2>&1
$R pair both   33 113 $T $Z24 $W1 > logs/pair_w1_24_both_2p33.txt 2>&1
$R pair seed   32 115 $T $Z24 $W0 > logs/pair_w0_24_seed_2p32.txt 2>&1
for L in 65 128 129 240 241 256; do
  $R scan both 26 151 $T $L $L 26 >> logs/scan_long.txt 2>&1
done
touch logs/SCAN2_DONE
date -u >> logs/env.txt
touch logs/ALL_DONE
