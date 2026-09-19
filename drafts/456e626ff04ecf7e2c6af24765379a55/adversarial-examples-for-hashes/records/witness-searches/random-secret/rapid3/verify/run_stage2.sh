#!/bin/bash
# Stage 2 (extra precision beyond the matched sample sizes): fresh 2^37 samples for both pairs
# under seed + secret uniform, and pair D at 2^34.
set -e
cd "$(dirname "$0")"
export OMP_NUM_THREADS=8
P="nice -n 10 taskset -c 24-31"
A=9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c
A2=642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c
P24=9bd4604137366abec688a63706aa4a2188d35499de169df6
P24b=642b9fbec8c99541c688a63706aa4a2188d35499de169df6
D=9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c48c651edae76208e840fc51f1cccbb02
D2=642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c48c651edae76208e840fc51f1cccbb02
run() { local tag=$1; shift; echo "== $tag: $*"; { /usr/bin/time -f "%e s wall, %U s user" $P "$@"; } > logs/$tag.txt 2>&1; tail -n 3 logs/$tag.txt; }
run 40_pairD_random_2p34   ./verify_rapid3 pair 34 4001 $D $D2 random
run 41_pairA_random_2p37   ./verify_rapid3 pair 37 4002 $A $A2 random
run 42_pair24_random_2p37  ./verify_rapid3 pair 37 4003 $P24 $P24b random
touch logs/STAGE2_DONE
