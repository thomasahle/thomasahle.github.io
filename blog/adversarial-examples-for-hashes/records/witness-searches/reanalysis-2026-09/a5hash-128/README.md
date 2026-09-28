# a5hash-128: an 18-byte every-seed family, and a 2^384 long-input family

## 1. 18 bytes

On the 17..32-byte path, c = lu32(m + len - 16)<<32 | lu32(m + len - 12) and d = the last 8 bytes enter
one product umul128(c + Seed3, d + Seed4) with the public constants Seed3 = 0xA4093822299F31D0 and
Seed4. Setting c = -Seed3 makes that product (0, 0) whatever d is. At 18 bytes, bytes 16..17 occur only
in d, so any 16-byte prefix with c = -Seed3 followed by all 2^16 two-byte tails collides on the full
128-bit output for every seed (256 members at 17 bytes). The 64-bit a5hash has no such public product.

Example members (memory order), from `mc_a5.cpp`:

    5e74ddc7f65b30ce60d68503aceb37f20000
    5e74ddc7f65b30ce60d68503aceb37f20100

| program | implementation | result | log |
|---|---|---|---|
| `mc_a5.cpp` (x86-64) | SMHasher3 `a5hash.cpp` v5.21 (89406B11, 14AD402C) | 18 B, 65,536 members: 65,536/65,536 seeds (full 128 bits); 17 B, 256 members: 65,536/65,536 | `logs/mc_a5_xeon_s16.txt` |
| `../museair/indep_museair_a5_murmur.cc` (M2) | upstream `a5hash.h` v5.25, unmodified; family regenerated independently | 65,536 members, 4097/4097 seeds (full 128 bits) | `logs/indep_museair_a5_murmur_m2.txt` |

    c++ -O2 -std=c++17 mc_a5.cpp -o mc_a5 && ./mc_a5 16

## 2. Long inputs: six zeroed products in the first 192 bytes

Only lane 1 carries the seed at the start of the bulk loop; lanes 2..4 start from public constants and
stay public for one or two more 64-byte blocks. Zeroing words (block 0: m8 = -Seed3, m16 = -Seed5,
m24 = -Seed7; block 1: m16 = -Seed3, m24 = -Seed5; block 2: m24 = -Seed3) make six public products
(0, 0) and free their six partner words: 2^384 messages, anchored in the first 192 bytes, with any
content and length after that (at least 96 bytes for block 0, 224 for all three).

| length | full 128-bit collisions | control (one zeroing word broken) |
|---|---|---|
| 96 B | 65,536/65,536 | 0/256 |
| 224 B, 256 B, 1 KiB, 64 KiB | 65,536/65,536 each | 0/256 |
| 1 MiB | 256/256 | 0/256 |
| 2^16 family at 1000 B | all members equal for 1024/1024 seeds | |

`a5_long.cpp`: `c++ -O2 -std=c++17 a5_long.cpp -o a5_long && ./a5_long`; log `logs/a5_long_xeon.txt`.
