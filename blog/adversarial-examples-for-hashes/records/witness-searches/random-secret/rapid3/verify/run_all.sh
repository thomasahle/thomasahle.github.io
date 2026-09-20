#!/bin/bash
# Independent verification of the rapidhash v3 random-secret rates.  Runs on 8 shared cores.
# Same sample sizes as the study being verified: 2^30, fresh 2^34, fresh 2^35 per pair (seed + secret
# uniform); default-secret 2^34 controls; ChaCha20-RNG 2^30 cross-checks; pair D at 2^30.
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
gcc -O3 -march=native -fopenmp -o verify_rapid3 verify_rapid3.c -lm
gcc -O3 -march=native -fopenmp -DRNG_CHACHA -o verify_rapid3_chacha verify_rapid3.c -lm
gcc -O2 -c upstream_driver.c && gcc -O2 -c port_driver.c && gcc -O2 -o crosscheck crosscheck.c upstream_driver.o port_driver.o
./verify_rapid3 selftest | tee logs/00_selftest.txt
./crosscheck | tee logs/01_crosscheck.txt
run() { local tag=$1; shift; echo "== $tag: $*"; { /usr/bin/time -f "%e s wall, %U s user" $P "$@"; } > logs/$tag.txt 2>&1; tail -n 3 logs/$tag.txt; }
run 10_pairA_random_2p30   ./verify_rapid3 pair 30 1001 $A $A2 random
run 11_pair24_random_2p30  ./verify_rapid3 pair 30 1002 $P24 $P24b random
run 12_pairD_random_2p30   ./verify_rapid3 pair 30 1003 $D $D2 random
run 13_pairA_chacha_2p30   ./verify_rapid3_chacha pair 30 1004 $A $A2 random
run 14_pair24_chacha_2p30  ./verify_rapid3_chacha pair 30 1005 $P24 $P24b random
run 20_pairA_random_2p34   ./verify_rapid3 pair 34 2001 $A $A2 random
run 21_pair24_random_2p34  ./verify_rapid3 pair 34 2002 $P24 $P24b random
run 22_pairA_default_2p34  ./verify_rapid3 pair 34 2003 $A $A2 default
run 23_pair24_default_2p34 ./verify_rapid3 pair 34 2004 $P24 $P24b default
run 30_pairA_random_2p35   ./verify_rapid3 pair 35 3001 $A $A2 random
run 31_pair24_random_2p35  ./verify_rapid3 pair 35 3002 $P24 $P24b random
touch logs/ALL_DONE
