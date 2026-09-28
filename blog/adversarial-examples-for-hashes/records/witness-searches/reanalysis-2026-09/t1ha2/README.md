# t1ha2_atonce (v2.1.4): a top-bit trail through the 32-byte loop

Key model: one uniform 64-bit seed; the messages are fixed before the seed is drawn; a hit is
equality of the full 64-bit output (and, where stated, of the full 128-bit output of
`t1ha2_atonce128`). Hex strings are bytes in memory order.

## Mechanism

For inputs longer than 32 bytes, each 32-byte block (words w0..w3, little-endian) updates the state
(a, b, c, d):

    d02 = w0 + rotr(w2 + d, 56);  c13 = w1 + rotr(w3 + c, 19);
    d ^= b + rotr(w1, 38);        c ^= a + rotr(w0, 57);
    b ^= P6 * (c13 + w2);         a ^= P5 * (d02 + w3);

A difference of exactly 2^63 is both an XOR and an additive difference, and it passes through `+`,
`^` and multiplication by an odd constant unchanged.

* Block b ("switch"): flip bit 56 of w0 and bit 37 of w1 (both 0 in the base) and subtract 2^56
  from w3. c and d gain 2^63; `d02 + w3` is unchanged; `c13` is unchanged iff
  y = (w3 + c) mod 2^64 >= 2^56. This is the only condition.
* Blocks b+1 and b+2: flip bit 63 of w2 and of w3. The state difference goes
  (0,0,2^63,2^63) -> (2^63,2^63,2^63,2^63) -> 0.

Three blocks (96 bytes, L = 12) give a pair. In the first block c = rotr(96, 23) + ~seed, which is a
bijection of the seed, so the pair fails for exactly 2^56 of the 2^64 seeds: eps = 1 - 2^-8 and
log2(12 / eps) = 3.5906 bits. The 128-bit function uses the same loop and collides on the same seeds.

## The pair

    M  = 4b2cc652222de32c6eb402b35cdd18cae775ec4962530f8652665a38df9912e3
         1dc78f5ae19a0cc6f585de2f819640042e1f4a635bba50f7fc6de8bf179150d9
         8d19298b36bf110cb6a7a887c622f90d33c44afcfeaeffe3cec888710f1c64b9
    M' = 4b2cc652222de32d6eb402b37cdd18cae775ec4962530f8652665a38df9912e2
         1dc78f5ae19a0cc6f585de2f819640042e1f4a635bba5077fc6de8bf17915059
         8d19298b36bf110cb6a7a887c622f90d33c44afcfeaeff63cec888710f1c6439

Failing seeds: the interval [0xe21359df385a6652, 0xe31359df385a6651], exactly 2^56 seeds.
Seed 0 gives 0x8851841f758af2cd for both; seed 0x7d4799c23d231d40 gives 0xcd72df04507e4a98 for both
(128-bit: eae1666093081008:640f3d4e94bf7e81 for both).

## Families

Any subset of the first n-2 blocks of an n-block base may be switched on (the parity flips of the next
two blocks are applied per member), giving 2^(n-2) messages of 32n bytes. All of them collide whenever
the n-2 carry conditions hold for the common base. 576 bytes give 2^16 members, 704 bytes give 2^20.
The conditions are not all independent: the one at block 1 depends on c_1 = c_0 ^ (seed +
rotr(w0_0, 57)), a fixed function of the seed with a non-uniform top byte. For some bases it never
fails, for others it fails about 10% of the time (`block_conditions.c`), so p(S) varies with the base
between about 0.85 and 0.94. Placing the switched blocks in disjoint three-block windows
(`t1ha2_longpos.c cube`) avoids the correlated block, giving (1 - 2^-8)^r.

The trail is local. Its only condition involves the state entering the switched block, so the same
three-block difference works at any 32-byte-aligned position inside a long message with arbitrary
content (checked from 256 B to 1 MiB).

## Files

| file | what it checks |
|---|---|
| `pair_check.c` | links upstream t1ha; the pair, the exact edges of the failing interval, uniform sample vs the condition |
| `t1ha2_topbit.c`, `t1ha2_core.h` | search/measurement program (own port, checked against the SMHasher3 verification values 0x8F16C948 / 0xB44C43A1): pair and 2^r-way families |
| `t1ha2_trail_check.c` | independent checker written from the mechanism description; links upstream t1ha; random bases |
| `block_conditions.c` | per-block failure rates of the family conditions (why p(S) depends on the base) |
| `t1ha2_longpos.c` | position independence inside 256 B .. 1 MiB random messages; disjoint-window cubes |

Upstream sources: see `SOURCES.md`.

## Build and run

    T=<path to t1ha checkout>
    cc -O2 -std=c11 -I$T pair_check.c $T/src/t1ha2.c $T/src/t1ha2_selfcheck.c $T/src/t1ha_selfcheck.c -lm -o pair_check
    ./pair_check 24
    cc -O2 -std=c11 t1ha2_topbit.c -lm -o t1ha2_topbit
    ./t1ha2_topbit pair 30 0x51              # 2^30 seeds
    ./t1ha2_topbit fam 18 16 12 0x61         # 2^16 members, 576 B, 4096 seeds
    ./t1ha2_topbit fam 22 20 9 0x71          # 2^20 members, 704 B, 512 seeds
    cc -O2 -std=c11 -I$T t1ha2_trail_check.c $T/src/t1ha2.c $T/src/t1ha2_selfcheck.c $T/src/t1ha_selfcheck.c -lm -o t1ha2_trail_check
    ./t1ha2_trail_check pair 28 101
    ./t1ha2_trail_check family 18 14 404
    cc -O2 -std=c11 block_conditions.c -o block_conditions && ./block_conditions 18 606
    cc -O2 -std=c11 -pthread -I$T t1ha2_longpos.c $T/src/t1ha2.c $T/src/t1ha2_selfcheck.c $T/src/t1ha_selfcheck.c -lm -o t1ha2_longpos
    THREADS=8 ./t1ha2_longpos rand 1048576 65536
    ./t1ha2_longpos cube 65536 16 64

## Expected output (from `logs/`)

| run | result |
|---|---|
| `pair_check 24` | 16,711,577 / 2^24 = 0.996088; 128-bit same; 0 mismatches with the condition; interval edges as above |
| `t1ha2_topbit pair 30 0x51` | 1,069,547,538 / 2^30 = 0.996094; misses 4,194,286 (2^-8 predicts 4,194,304); predictor agrees on all 2^30 |
| `t1ha2_trail_check pair 28 101` (other random pair) | 267,388,667 / 2^28 = 0.9961004; 128-bit identical; 0 mismatches |
| `t1ha2_trail_check pair 26 202 / 303` | 66,846,684 and 66,847,086 / 2^26 |
| `t1ha2_topbit fam 18 16 12` (4 runs) | all 2^16 equal on 3848, 3839, 3881, 3866 of 4096 seeds (pooled 15,434 / 16,384 = 0.942) |
| `t1ha2_topbit fam 22 20 9` (2 runs) | all 2^20 equal on 474 and 485 of 512 seeds (pooled 959 / 1024 = 0.937) |
| `t1ha2_trail_check family 18 14 404` | 15,447 / 16,384 = 0.943 |
| `t1ha2_trail_check family 18 13 606` | 6,960 / 8,192 = 0.850 (block-1 condition fails ~25/256) |
| `t1ha2_trail_check family 22 10 505` | 944 / 1,024 = 0.922 |
| `t1ha2_longpos rand` | failure rate 2^-8.01 (256 B), 2^-8.01 (1 KiB), 2^-7.96 (64 KiB), 2^-8.10 (1 MiB: 239 / 65,536); predictor exact |
| `t1ha2_longpos fixed 1048576` | 32,644 / 32,768 at byte offset 861,792 |
| `t1ha2_longpos cube 65536 16 64` | whole 2^16 set equal for 60 / 64 keys; `cube 1048576 10 64`: 2^10 set for 61 / 64 |

Every run first checks the upstream self-test and the SMHasher3 verification value 0x8F16C948 and
exits with status 1 on a mismatch. Note on `logs/t1_pair30.txt`: the 128-bit high halves on its
"example seed" line came from an unsequenced printf argument and are wrong; the program in this folder
sequences the calls (the counts were never affected). `pair_check` prints the correct values.
