# HalftimeHash24 (advanced 24-byte API): which entry point, and one key condition for every leaf

`halftime_hash::advanced::V1<3>` .. `V4<3>` return 24 bytes and use block widths b = 1, 2, 4, 8. The Encode3
encoder has distance 1: a low-half change in one encoded word meets exactly one NH term, which vanishes when the
high 32 bits of the matching core key word are zero (a 2^-32 key class). The witness is 168b zero bytes versus
the same with little-endian word 6b equal to 1.

## Scope: V1<3> versus V4<3>

For V4<3> (b = 8, the entry point the AVX-512 timing uses) every message shorter than 1344 bytes goes through the
finalizer's tail path, where each word meets three NH terms. So the 168-byte pair that scores V1<3> does not
collide on V4<3> even in the key class:

| entry point | pair | high32(core_key[6]) = 0 | outside the class | uniform keys |
|---|---|---|---|---|
| V1<3> (b = 1) | 168 B, word 6 | 1,048,576 / 1,048,576 | 0 / 1,048,576 | 0 / 1,048,576 |
| V4<3> (b = 8) | 1344 B, word 48 (byte 384 = 0x01) | 262,144 / 262,144 | 0 / 262,144 | 0 / 262,144 |
| V4<3> (b = 8) | 168 B, word 6 | 0 / 1,048,576 | 0 / 1,048,576 | 0 / 1,048,576 |

(`logs/runs_xeon_avx512.log`; the scalar build on arm64 repeats all three rows with 4,096 keys each,
`logs/runs_m2_scalar.log`.)  With epsilon = 2^-32: V1<3> caps at log2(21) + 32 = 36.39 bits (L = 21);
V4<3> caps at log2(168) + 32 = 39.39 bits (L = 168).

## Long inputs: the condition frees every leaf

All leaves share the base-layer key, so the same single key condition frees the varied word in every 1344-byte
leaf at once (V4<3>): a family varying 8 leaves of a 64 KB message (2^8 members) collides entirely for 64/64 class
keys and 0/64 outside; varying 10 leaves of a 1 MB message (2^10 members), 16/16 class keys (AVX-512) and 128/128
(scalar arm64), 0 outside, 0 uniform. Since each of the 780 leaves of a 1 MB message offers 8 free 32-bit
halves, the family has about 2^(32 * 8 * 780) ~ 2^200,000 members, all colliding on the same 2^-32 key class.
Only the sub-families above were executed.

Key-free window (the raw core does not encode length): zero messages of lengths 0..63 (b = 8) or 0..7 (b = 1)
collide for 2000/2000 keys, and the next length never joins them (0/2000).

## Build and run

    curl -LO https://raw.githubusercontent.com/jbapple/HalftimeHash/caf7924ceab4721f4e0cc33442b185558ba7f1c4/halftime-hash.hpp
    c++ -O2 -march=native -std=c++17 -pthread -o halftime24_mc halftime24_mc.cpp
    # arm64: the header's NEON dispatch does not compile (upstream issue #1); force the scalar path:
    c++ -O2 -std=c++17 -pthread -U__ARM_NEON -U__ARM_NEON__ -o halftime24_mc halftime24_mc.cpp

    ./halftime24_mc class 1 168 6 6 1048576 101
    ./halftime24_mc class 8 1344 48 6 262144 102
    ./halftime24_mc class 8 168 6 6 1048576 103
    ./halftime24_mc long 1048576 10 16 802

On x86-64 with AVX-512 the program first checks that the dispatched V2..V4 equal the header's scalar V2..V4 on
8,400 random inputs (0 mismatches in the logs).
