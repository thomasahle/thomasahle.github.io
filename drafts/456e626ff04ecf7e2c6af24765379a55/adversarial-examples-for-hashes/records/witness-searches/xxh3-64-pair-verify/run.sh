#!/bin/sh
# Build against upstream v0.8.3 and run the confirmation set on cores 24-31.
set -e
cd <xeon-work>/witness/xxh3-64-pair-verify
gcc -O2 -std=c11 -Wall -c xxHash/xxhash.c -o xxhash.o
gcc -O2 -std=c11 -Wall -IxxHash -o verify_pair verify_pair.c xxhash.o -lpthread -lm
sha256sum verify_pair.c xxHash/xxhash.c xxHash/xxhash.h > sha256.txt
NEW_M=0000000000000000000000000000000051151210404400000000008204000105
NEW_MP=00000000000000000000000000000000aeeaedefbfbbffffffffff7dfbfffefa
ROW_M=8912a3da9fc464368202a1be238b7d1100000000000000000000000000000000
ROW_MP=76ed5c25603b9bc97dfd5e41dc7482ee00000000000000000000000000000000
MEMO_M=b8fe6c3923a44bbe83fe7ed308de52e300000000000000000000000000000000
MEMO_MP=470193c6dc5bb4417c01812cf721ad1c00000000000000000000000000000000
R="nice -n 10 taskset -c 24-31"
$R ./verify_pair 20 0x7e51 8 $NEW_M $NEW_MP 2468b3bc26a44073 dd686b61e2f6dc69 > smoke_new_2p20.txt
for s in 0x0be5719ab2026091 0x0be5719ab2026092 0x0be5719ab2026093; do
  /usr/bin/time -v $R ./verify_pair 30 $s 8 $NEW_M $NEW_MP 2468b3bc26a44073 dd686b61e2f6dc69 > new_2p30_$s.txt 2> new_2p30_$s.time
done
/usr/bin/time -v $R ./verify_pair 30 0x0be5719ab2026094 8 $ROW_M $ROW_MP c8eae1baae13330b 448e3716c8effb94 > row_2p30.txt 2> row_2p30.time
/usr/bin/time -v $R ./verify_pair 30 0x0be5719ab2026095 8 $MEMO_M $MEMO_MP 0 ab88e1e38f85bdca > memo_2p30.txt 2> memo_2p30.time
echo DONE > DONE.marker
