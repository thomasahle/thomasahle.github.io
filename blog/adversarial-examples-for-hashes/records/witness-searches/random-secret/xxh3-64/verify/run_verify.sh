#!/bin/bash
# Independent verification plan (Xeon, cores 24-31, nice 10, 8 threads).  Logs in logs/.
cd "$(dirname "$0")"; mkdir -p logs
R="nice -n 10 taskset -c 24-31 ./xxh3_verify"; T=8
NAF=0000000000000000000000000000000051151210404400000000008204000105
NAF2=00000000000000000000000000000000aeeaedefbfbbffffffffff7dfbfffefa
Z24=000000000000000000000000000000000000000000000000
W0=ffffffffffffffff00000000000000000000000000000000
{ echo "host: $(hostname)"; grep -m1 'model name' /proc/cpuinfo; gcc --version | head -1; sha256sum xxhash.h xxh3_verify.c; echo "start $(date -u)"; } > logs/env.txt
# controls first (cheap): the two seed-only APIs and the streaming API on the page's NAF pair
$R pair seed   28 9105 $T $NAF $NAF2 > logs/naf_seed_2p28.txt 2>&1
$R pair ss     28 9106 $T $NAF $NAF2 > logs/naf_ss_2p28.txt 2>&1
$R pair stream 24 9107 $T $NAF $NAF2 > logs/naf_stream_2p24.txt 2>&1
# the fold differential, own implementation (same 2^34 sample size as results_fold.md)
$R fold 34 9109 $T ffffffffffffffff ffffffffffffffff > logs/fold_MM_2p34.txt 2>&1
$R fold 34 9110 $T ffffffffffffffff 0000000000000000 > logs/fold_M0_2p34.txt 2>&1
touch logs/STAGE1_DONE
# the row's current pair under the random-secret models (same sizes as the searcher's plan)
$R pair both   35 9101 $T $NAF $NAF2 > logs/naf_both_2p35.txt 2>&1
$R pair secret 33 9104 $T $NAF $NAF2 > logs/naf_secret_2p33.txt 2>&1
touch logs/STAGE2_DONE
# the proposed best pair (24 B, word 0 complemented), both models
$R pair secret 35 9102 $T $Z24 $W0 > logs/w0_secret_2p35.txt 2>&1
$R pair both   35 9103 $T $Z24 $W0 > logs/w0_both_2p35.txt 2>&1
$R pair seed   32 9108 $T $Z24 $W0 > logs/w0_seed_2p32.txt 2>&1
echo "end $(date -u)" >> logs/env.txt
touch logs/ALL_DONE
