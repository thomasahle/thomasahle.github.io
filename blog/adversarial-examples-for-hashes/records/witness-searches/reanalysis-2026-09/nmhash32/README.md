# nmhash32 v2: lane-tied trail T2, a fixed set that floods one bucket for every seed

nmhash32 v2 has a 32-bit seed and a 32-bit output. Two ideas combine.

**Lane tying.** Each of the four lanes (33..255-byte path) or 32 lanes (long path, >= 256 bytes) XORs its
first x word onto a public constant (`NMH_PRIME32_1..4`, resp. `NMH_ACC_INIT[j]`). Choose x word j = constant_j ^ A
and every y word = B: all lanes then carry identical state, so a per-lane differential succeeds in all lanes or
in none.

**Trail T2.** Let L(v) = v ^ v<<11 ^ v>>9 (the xorshift before the third multiply). XOR D = L^-1(0x8000) =
0x04008040 into a lane's x and y word. x + y is unchanged exactly when bits 26, 15 and 6 of x ^ y are all 1;
the surviving difference reaches the 16-bit multiply as 0x8000 (the top bit of a 16-bit half, which an odd
multiplier maps to itself), leaves the round as C = 0x00008020, and the next round's words (x' ^= C, y' ^= D)
cancel it. There is no carry condition. With lane tying the condition is three seed bits: bits 6, 15, 26 of
y = seed + len (64-byte path) or y = seed (long path). A = A(g) fixes which of the 8 values of those bits
works.

Families (little-endian words; lane j on, B = 0):

- 64 B: x_j = P_j ^ A ^ D, y_j = D, x'_j = C, y'_j = D at byte offsets 4j, 16+4j, 32+4j, 48+4j; lanes off are
  x_j = P_j ^ A and zeros. The 16 lane subsets form a 16-way family.
- 512 B: the same with `NMH_ACC_INIT[j]` and offsets 4j, 128+4j, 256+4j, 384+4j, 32 lanes: 2^32 lane subsets.
- Fixed per-key set: the union over the 8 guesses g of 2^13 lane subsets = 65,536 distinct 512-byte messages.
  For every seed exactly one guess is right, so 8,192 members share one full output.

## Results

| claim | count | where |
|---|---|---|
| 16-way 64 B family (g = 7) collides | 536,870,912 / 2^32 seeds = exactly 1/8; predictor (3 seed bits) agrees on every seed | `logs/search_runs_xeon.log`, `logs/nm_check_m2.txt` (independent program) |
| T1-tied 16-way 64 B family | 1,079,027,392 / 2^32 = 25.1% | `logs/search_runs_xeon.log` |
| 512 B, {empty, lane 0, all 32 lanes} equal | 536,870,912 / 2^32; 0 disagreements with the 3-bit predictor | `logs/search_runs_xeon.log`, `logs/exh512_m2.txt` |
| 2^16-member 512 B family, class seeds / uniform seeds | 4,096/4,096 and 509/4,096; independent program 1,024/1,024 and 139/1,024 | both logs |
| 2^20-member 512 B family, class seeds | 256/256 | `logs/search_runs_xeon.log` |
| fixed 65,536-message set, largest full-output class | 8,192 for every one of 4,096 seeds (min = median = max); same with low-16/low-20/high-16-bit buckets (max 8,202) | `logs/search_runs_xeon.log`, `logs/nm_check_m2.txt` (4,096 more seeds, min = max = 8,192) |
| fixed 128-message 64 B set (8 guesses x 16) | a 16-member class for all 65,536 seeds tested | `logs/search_runs_xeon.log` |
| position independence (T2 inserted at a 256-byte round inside random content) | 1 KB 32,790/262,144 = 2^-2.999; 64 KB 2,059/16,384 = 2^-2.992; 1 MB 253/2,048 = 2^-3.017 (T1: 2^-1.991 / 2^-1.999 / 2^-2.020) | `logs/long_inputs_xeon.log` |

The 2^32-way statement at 512 B rests on the trail argument (the collision condition is the same three seed
bits for every lane subset), checked exhaustively over all 2^32 seeds for three members and on sampled seeds for
2^16- and 2^20-member subfamilies. It is not an exhaustive check of all 2^32 members.

**Witness** (64 B family, guess 7, seed 0xffffffbf): all 16 members hash to 0x2e04a763. Members in memory order
are in `logs/witness_16_members.txt`; member 0 and member 15:

    b179379e77caeb853daeb2c22febd427000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
    f1f9379a374aeb817d2eb2c66f6bd423408000044080000440800004408000042080000020800000208000002080000040800004408000044080000440800004

## Build and run

    curl -LO https://raw.githubusercontent.com/gzm55/hash-garage/e022156ca866edc46e1aa79b2b6a466822b8b767/nmhash.h
    cc  -O2 -std=c11   -pthread -o nm_check    nm_check.c        # independent check, families from the description
    c++ -O2 -std=c++17 -pthread -o nmhash32_mc nmhash32_mc.cpp   # the search program

    ./nm_check witness              # 16 members, all -> 2e04a763 at seed 0xffffffbf
    ./nm_check exh64 8              # all 2^32 seeds, about 4 CPU-minutes: 536870912, predictor 536870912, 0, 0
    ./nm_check perkey 4096 8        # largest class min 8192, max 8192
    ./nm_check fam16 1024 8         # class seeds 1024/1024
    NT=8 ./nmhash32_mc pair         # published 64 B pair: 1078944392 / 2^32
    NT=8 ./nmhash32_mc exh64 2 7
    NT=8 ./nmhash32_mc exh512 2 5   # about 40 CPU-minutes
    NT=8 ./nmhash32_mc perkey 2 13 4096 14 512
    NT=8 ./nmhash32_mc longins 1048576 2000 2 1 1 2048 606

Both programs check SMHasher3's verification value 0x12A30553 first and exit on mismatch.
