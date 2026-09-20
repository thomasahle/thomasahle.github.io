#!/bin/bash
# Independent verification of rapidhash v1.0 under the random-secret model (row rapid1).
# 8 threads; edit CORES/NICE for another machine.  Logs to logs/.
set -u
CORES=${CORES:-24-31}; NICE=${NICE:-10}; T=8
RUN="nice -n $NICE taskset -c $CORES ./rs_verify"
mkdir -p logs
g++ -O3 -std=c++17 -march=native -pthread rs_verify.cpp -o rs_verify || exit 1
sha256sum rapidhash_upstream.h rs_verify.cpp > logs/sha256.txt
{ $RUN selftest; } 2>&1 | tee logs/00_selftest.txt
A1=9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c   # row pair A (32 B)
A2=642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c   # words 0 and 1 complemented
B1=9bd4604137366abec688a63706aa4a2188d35499de169df6                   # pair B24 (24 B)
B2=642b9fbec8c99541c688a63706aa4a2188d35499de169df6                   # word 0 complemented
W0=642b9fbec8c99541c688a63706aa4a2188d35499de169df633e0964e8c04600c   # 32 B, word 0 only
W1=9bd4604137366abe397759c8f955b5de88d35499de169df633e0964e8c04600c   # 32 B, word 1 only
NUL=9bd4604137366abfc688a63706aa4a2188d35499de169df633e0964e8c04600c  # 32 B, bit 0 of byte 7 flipped (null control)
t0=$(date +%s)
{ echo "== 1: pair A, random seed + 3 uniform secret words, 2^32, rngseed 1"; $RUN pair $A1 $A2 32 1 random $T; } 2>&1 | tee logs/01_pairA_random_2p32_s1.txt
{ echo "== 2: pair B24, random seed + 3 uniform secret words, 2^32, rngseed 1"; $RUN pair $B1 $B2 32 1 random $T; } 2>&1 | tee logs/02_pairB24_random_2p32_s1.txt
{ echo "== 3: fresh confirmation, pair A, 2^33, rngseed 2"; $RUN pair $A1 $A2 33 2 random $T; } 2>&1 | tee logs/03_pairA_random_2p33_s2.txt
{ echo "== 4: fresh confirmation, pair B24, 2^33, rngseed 2"; $RUN pair $B1 $B2 33 2 random $T; } 2>&1 | tee logs/04_pairB24_random_2p33_s2.txt
{ echo "== 5: controls, 2^32, rngseed 3: pair A default secret; pair A odd secret; pair B24 default secret";
  $RUN pair $A1 $A2 32 3 default $T; $RUN pair $A1 $A2 32 3 odd $T; $RUN pair $B1 $B2 32 3 default $T; } 2>&1 | tee logs/05_controls_secret_models.txt
{ echo "== 6: 32-byte single-word complements under random model, 2^32, rngseed 3 (word 0 only; word 1 only); null control (single bit flip)";
  $RUN pair $A1 $W0 32 3 random $T; $RUN pair $A1 $W1 32 3 random $T; $RUN pair $A1 $NUL 32 3 random $T; } 2>&1 | tee logs/06_single_word_and_null.txt
echo "elapsed $(( $(date +%s) - t0 )) s" | tee logs/elapsed.txt
echo DONE > logs/DONE
