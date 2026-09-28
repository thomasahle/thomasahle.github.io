# MuseAir v2 (crate museair 0.6.0): an 80-byte pair that collides for every seed

Key model: `museair::hash(bytes, seed)` (and `museair::bfast::hash`) with one uniform 64-bit seed;
the messages are fixed before the seed is drawn; a hit is equality of the full 64-bit output. Hex
strings are bytes in memory order.

## Mechanism (product exchange)

The 64-bit seed enters every state word in one of two classes: s_x = C[x] ^ SA for even x and
C[x] ^ SB for odd x, where SA = seed & 0xAAAA...AA and SB = seed & 0x5555...55. On the long path
(more than 32 bytes, up to 96 bytes, no compress round) the finalize computes six products
P_m = A_m * B_m. Each operand is s_x XOR message words, so it is SA or SB XOR a value the message
controls. Only these reach the output:

    i <- s0 - s1,  g(P3) + g(P4)
    j <- s2 - s3,  g(P5) + g(P0)
    k <- s4 - s5,  g(P1) + g(P2)            (g(P) = lo ^ hi)

If two products that feed the same accumulator have operands of matching classes, the message can
make the operands equal up to bit 63. Flipping bit 63 of four chosen words then exchanges the two
products (their sum is unchanged), and the 2^63 shifts cancel in pairs in the differences s_even - s_odd.
Nothing depends on the seed value, so every seed collides.

At 80 bytes (L = 10; P3 is not computed at this length): take A0 = B5 ^ 2^63 (t3 = 2^63),
B0 = A5, B1 = A2 ^ 2^63 (r4 = 2^63), B2 = A1 ^ 2^63, and flip bit 63 of words 0, 2, 3 and 5. This
exchanges P0 with P5 and P1 with P2. At 88 and 96 bytes the analogous choice exchanges P1 with P2 and
P3 with P4. Under 80 bytes the construction does not close: at 65..72 bytes a flipped word overlaps
the tail words that feed P4, and at 64 bytes or fewer the k accumulator has only one product.

This extends pair D of eternal-io/museair#4 (a factor swap in the v0.3 BFast long path, which works
because a ^ b is seed-free there). In v2 the seed masks make single-lane swaps seed-dependent, but
the two-class structure still allows whole products to be exchanged.

## The pair (80 bytes)

    M  = 909dea05e80e02d243265f3567044597aa01c47742b811c67ad20fc2a1b5b144
         000000000000008017cedf86b534a0667d8a71f0e684fc7ff1bc7e2b4f655fdb
         3319d238bcff82080000000000000080
    M' = 909dea05e80e025243265f3567044597aa01c47742b811467ad20fc2a1b5b1c4
         000000000000008017cedf86b534a0e67d8a71f0e684fc7ff1bc7e2b4f655fdb
         3319d238bcff82080000000000000080

They differ in bit 7 of bytes 7, 23, 31 and 47. `hash`: seed 0 -> 0x6553a84485e3fc6a for both;
seed 0xdeadbeefcafef00d -> 0x90fcb15f1b2e6ddd for both. `bfast::hash`: seed 0 -> 0x5113830ad473dfdd for both.
Score log2(10 / 1) = 3.32 bits.

`hash128` and `bfast::hash128` do not collide on this pair (0 / 2^20). They seed with three masks and
two seeds, so operands of the same class cannot be tied.

## Limits

* Size of the set: the exchange units at a given length need incompatible base relations when
  combined, so this mechanism gives a set of two messages, not a larger family.
* Longer inputs: one compress round (inputs over 96 bytes) turns every state word into a chain of
  products and leaves no class relation. The 80-byte pair placed after a common 96-byte chunk
  (176 B), or as the start of a 192-byte message, gives 0 / 2^24 collisions (`museair_long`).

## Files

| file | what it checks |
|---|---|
| `museair2_swap.c`, `museair2_core.h` | own C port of crate 0.6.0 (checks all eight `stability_v2` values at startup); builds the 80, 88 and 96-byte pairs from the relations and counts collisions |
| `museair2_check/` | independent Rust checker linked against the crate; the 80-byte pair, special seeds, `hash128`, and a 96-byte variant built from the stated relations |
| `museair_long/` | Rust, crate-linked: the pair embedded in 176 and 192-byte inputs |

Upstream: see `SOURCES.md`.

## Build and run

    cc -O2 -std=c11 museair2_swap.c -o museair2_swap && ./museair2_swap 24
    (cd museair2_check && cargo run --release -- 26 26)
    (cd museair_long && cargo run --release -- 24)

## Expected output (from `logs/`)

| run | result |
|---|---|
| `museair2_check 26 26` | `hash` 67,108,870 / 67,108,870 and `bfast::hash` 67,108,870 / 67,108,870 (2^26 random seeds plus 0, 2^64-1, 0xAA..AA, 0x55..55, 1, 2^63); `hash128` 0 / 65,536 |
| `museair2_check 20 26` | same pair 2^20 + 6 / 2^20 + 6; 96-byte variant 2^20 / 2^20 on both functions |
| `museair2_swap 24` | 80, 88 and 96 bytes: 2^24 / 2^24 on `hash` and `bfast::hash`; `hash128` 0 / 2^20 |
| `museair_long 24` | 80 B control 2^20 / 2^20; 176 B and 192 B 0 / 2^24 on both functions |

`logs/museair2_check_2p26.log` was produced before the 96-byte section was added to the program;
`logs/museair2_check_2p20_96B.log` is a rerun of the current program.
