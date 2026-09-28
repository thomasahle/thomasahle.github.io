# komihash 5.34: an every-seed pair at 73 bytes, and a 2^17-member family

Key model: `komihash(msg, len, UseSeed)` with one uniform 64-bit seed; the messages are fixed before
the seed is drawn; a hit is equality of the full 64-bit output. Hex strings are bytes in memory order.

## Mechanism

The published 64-byte pair ties one lane pair of the 64-byte loop block (w1 = w0 ^ IVAL2, so lanes 0
and 1 multiply identical operands). Tie both lane pairs at once:

    w1 = w0 ^ IVAL2,   w3 = w2 ^ IVAL3 ^ IVAL4,   w5 = w4 ^ IVAL6,   w7 = w6 ^ IVAL7 ^ IVAL8.

Then lane 0 equals lane 1 and lane 2 equals lane 3 for every seed. The low product halves cancel
pairwise in the fold, and after the block the two folded words are equal: Seed1 = Seed5 = X, for
every seed. The first epilogue step computes (m0 ^ X) * (m1 ^ X), which is symmetric in m0 and m1.
Exchanging the two tail words therefore gives the same hash for every seed.

At 73 bytes the 9-byte tail is read as m0 = bytes 64..71 and m1 = 0x100 | byte 72, so the pair is a
tied block followed by [y, 01, 00 x 6, z] versus [z, 01, 00 x 6, y]. At 80 bytes the two 8-byte tail
words are exchanged. L = 10 either way, so the score is log2(10) = 3.32 bits. This does not beat the
recorded 64-byte pair (0.9106 of seeds, 3.135 bits).

## The pair (73 bytes)

    M  = 2a87bfdc2277e89c6ef4cfdf0cfdf18f7827cd8bcaebf22c217a1c4e7029d580
         6e9047ff9934dc7f029caecb565288c17557d8339262c6b9bf0ee34f909eee46
         3c01000000000000a7
    M' = 2a87bfdc2277e89c6ef4cfdf0cfdf18f7827cd8bcaebf22c217a1c4e7029d580
         6e9047ff9934dc7f029caecb565288c17557d8339262c6b9bf0ee34f909eee46
         a7010000000000003c

Seed 0 gives 0xf5d54adfc8d4643d for both (upstream `komihash.h` 5.34). Any tied block works; the
programs below also build their own.

## Family (2^17 messages, 80 bytes)

Start from a tied block whose shared multiplier words are common (w4, w6 = w4 ^ IVAL7) and vary the
low 8 bits of w0 and of w2 (with w1, w3 following the ties): 2^16 blocks. Append the 16-byte tail
(m8, m9) or the swapped tail (m9, m8): 2^17 messages. The swap half matches the unswapped half for
every seed; the 2^16 grid collides fully only on a weak-key class. All 2^17 messages collide for
157 / 16,384 seeds (2^-6.71) on one base and 50 / 8,192 (2^-7.36) on another. On every seed sampled,
at least 2^10 of the 2^17 messages share one output.

## Longer inputs

The mechanism is limited to 64..127 bytes. After one more loop block the lanes are no longer tied.
Re-tying the second block needs three carry relations at once, 2^-57 over the seed (measured law
2^-7.00, 2^-10.00, 2^-13.00, 2^-16.02 at truncated widths 8, 12, 16, 20 bits, as predicted). The pair
with one common extra block (137 B) gives 0 / 2^30.

## Files

| file | what it checks |
|---|---|
| `pair73_check.c` | includes upstream `komihash.h`; the exact 73-byte pair above over uniform seeds |
| `komi_swap.c`, `komihash_core.h` | own port (checks SMHasher3 verification 0x8157FF6D); the published 64-byte pair and the 73 / 80-byte every-seed pairs |
| `komihash_tie_check.c` | independent checker, includes upstream `komihash.h`; README vectors + verification value; 73-byte pair on a random tied block; control with two different tied blocks |
| `komihash_grid_swap.c` | includes upstream `komihash.h`; the 2^17 grid-times-swap family |
| `komi_long.c` | includes upstream `komihash.h`; beyond one loop block (137 B), and the re-tie law |

Upstream: see `SOURCES.md`.

## Build and run

    K=<path to komihash checkout>
    cc -O2 -std=c11 -I$K pair73_check.c -o pair73_check && ./pair73_check 24
    cc -O2 -std=c11 komi_swap.c -lm -o komi_swap && ./komi_swap swap 24 && ./komi_swap pub 24
    cc -O2 -std=c11 -I$K komihash_tie_check.c -o komihash_tie_check && ./komihash_tie_check 24 1
    cc -O2 -std=c11 -I$K komihash_grid_swap.c -lm -o komihash_grid_swap && ./komihash_grid_swap 14 31
    cc -O2 -std=c11 -I$K komi_long.c -lm -o komi_long && ./komi_long 30 26

## Expected output (from `logs/`)

| run | result |
|---|---|
| `pair73_check 24` | 16,777,216 / 16,777,216; seed 0 -> f5d54adfc8d4643d for both |
| `komihash_tie_check` (the run that produced the pair above) | 67,108,864 / 67,108,864 seeds collide; control with two different tied blocks 0 / 65,536 |
| `komihash_tie_check 24 1` | another random tied block: 16,777,216 / 16,777,216 |
| `komi_swap swap 24` | 73-byte and 80-byte pairs 16,777,216 / 16,777,216 each |
| `komi_swap pub 24` | published 64-byte pair 15,274,904 / 2^24 = 0.91046 (3.135 bits) |
| `komihash_grid_swap 14 31` | swap half = unswapped half on 16,384 / 16,384 seeds; all 2^17 equal on 157 (2^-6.71) |
| `komihash_grid_swap 13 32` | 8,192 / 8,192; all 2^17 equal on 50 (2^-7.36) |
| `komi_long` | 73 B control 2^20 / 2^20; 137 B plain and re-tied 0 / 2^30; re-tie law as above |
