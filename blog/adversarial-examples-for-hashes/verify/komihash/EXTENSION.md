# komihash 5.34 lane-tie family: how far it reaches (extension)

Measurements answering the maintainer's question on komihash issue #23
("what would be problematic is a fast method to flip bits for ANY message
to produce collisions without knowing the seed"). Everything below comes
from runs of `komihash_ext.c` in this directory.

## Setup

* `komihash.h` is the reference header 5.34 extracted from
  `komihash_pair.c` (the text between the two "verbatim" marker lines):

      awk '/^\/\* >>>> komihash.h 5.34 verbatim begins/{f=1;next} /^\/\* <<<< komihash.h 5.34 verbatim ends/{f=0} f' komihash_pair.c > komihash.h
      shasum -a 256 komihash.h
      # 1adfc1bc11979aaebfd6a0f96cdaca7f0c3cb24bbd2a9f408726d15810f44d89  (matches the README)

* `komihash_ext.c` includes that header unchanged and runs one experiment
  per subcommand. It aborts if the 64-byte upstream test vector fails.

      cc -O2 -std=c11 -Wall -Wextra -o komihash_ext komihash_ext.c -lm   # clean, Apple clang
      ./komihash_ext <arb|free|s5bias|tie34|multi|all8|low3|len|control> [log2 seeds=20] [bases=200] [rng seed hex=c0ffee]

* Seeds are uniform 64-bit values from splitmix64 + xoshiro256** (default
  stream `c0ffee`; `deadbeef` used as a second stream). Inside one
  experiment every base message sees the same seed stream, so differences
  between bases are due to the message, not to sampling. Standard error of
  a rate at 2^20 seeds is about 0.0003-0.0005. Collision = full 64-bit
  output equality. Each run takes 0.2-15 s on an Apple M2 Pro.

* Notation: m0..m7 are the eight little-endian 64-bit words of the 64-byte
  block (bytes 0-7, 8-15, ..., 56-63). For 64..127-byte messages the block
  goes once through the loop, where lane i multiplies (m_{i-1} ^ Seed_i)
  by (m_{i+3} ^ Seed_{i+4}), the low half of the product replaces Seed_i
  and the high half is added into Seed_{i+4}. Seed2..4 = Seed1 ^ IVAL2..4
  and Seed6..8 = Seed5 ^ IVAL6..8, with Seed1 and Seed5 the two secret
  words after the init round. After the loop the state that reaches the
  epilogue is S5' = XOR over lanes of (Seed_{i+4} + high_i), and
  S1' = (XOR of the four low halves) ^ S5'.

* Ties used: **tie12** m1 = m0 ^ IVAL2, m5 = m4 ^ IVAL6 (lanes 1 and 2
  multiply identical operands for every seed); **tie34** m3 = m2 ^
  (IVAL3 ^ IVAL4), m7 = m6 ^ (IVAL7 ^ IVAL8) (lanes 3 and 4 likewise);
  **tie-all** m1..m3 = m0 ^ IVAL2..4, m5..m7 = m4 ^ IVAL6..8 (all four
  lanes equal lane 1). "Flip A" = bit 0 of m0 and m1 (bytes 0 and 8);
  "flip B" = bit 0 of m2 and m3 (bytes 16 and 24). Published words:
  m0 = 0, m4 = 0x5ea1c0c1a4ddf9f0.

## 1. Arbitrary content in the free words

Commands: `./komihash_ext free 20 10`, `./komihash_ext s5bias 20`,
`./komihash_ext arb 20 200`, `./komihash_ext arb 20 200 deadbeef`,
`./komihash_ext control 20 20` (outputs in `free.txt`, `s5bias.txt`,
`arb.txt`, `arb_deadbeef.txt`, `control.txt`).

**The words m2, m3, m6, m7 do not matter at all.** With the published m0,
m4 and ten random fills of m2, m3, m6, m7, the per-seed outcome (collide
or not) was identical for all ten fills on all 2^20 seeds: 0 differing
cells out of 9,437,184. Fill 0 gave 954,642/1,048,576 = 0.9104, the same
count the blog's `komihash_pair 20` prints. (Reason: lanes 3 and 4
contribute identical terms to both messages and cancel in the equality.)

**Random everything (tie12 only), 200 bases x 2^20 seeds, m4 uniform:**

| statistic over 200 bases | collision rate | P(no carry into high half) | P(collision given carry) |
|---|---|---|---|
| min | 0.8391 | 0.3568 | 0.7484 |
| median | 0.8552 | 0.4212 | 0.7500 |
| mean | 0.8721 | 0.4885 | 0.7500 |
| max | 0.9111 | 0.6427 | 0.7517 |

Second RNG stream (`deadbeef`): min 0.8392, median 0.8554, mean 0.8741,
max 0.9114. Every seed without a carry collided (checked seed by seed),
and given a carry the rate is 3/4 for every base, so the rate of a base
is p + (1 - p) * 3/4 where p = P(no carry) depends on m4 only.

**Published m4, everything else uniform (200 bases x 2^20):** collision
rate min 0.9100, median 0.9107, mean 0.9107, max 0.9117; P(no carry)
0.6417-0.6437. Second stream: 0.9099 / 0.9107 / 0.9107 / 0.9114. So m0
(and the other 32 free bytes) leave the rate at the published 0.9106.

**Why m4 matters (`s5bias`, 2^20 seeds):** S5 after the init round is
biased. Its most common top byte is 0x1e at frequency 0.0156 (uniform
would be 0.0039), and P(bit k = 1) for k = 63..56 is 0.280, 0.572, 0.422,
0.558, 0.534, 0.512, 0.539, 0.494. The published m4's top byte 0x5e =
0101 1110 has exactly the more likely value in each of those eight bit
positions. That makes v = m4 ^ S5 small more often: E[v / 2^64] is
0.3572 for the published m4 versus 0.5973 for a random one, and P(no
carry) is about 1 - E[v / 2^64] (0.643 vs 0.40). The worst random m4
seen (top bits anti-aligned with the bias) has P(no carry) 0.357 and rate
0.839; the median random m4 gives 0.855.

**Control (20 random bases x 2^20 seeds each, same flip):** no tie
0/20,971,520; only m1 tied (m5 free) 0/20,971,520; only m5 tied (m1 free)
0/20,971,520; full tie 18,284,557/20,971,520 = 0.8719. The flip does
nothing without both halves of the tie.

**Answer 1.** Yes. An attacker who fixes the 16 tie bytes (bytes 8-15 =
bytes 0-7 XOR IVAL2, bytes 40-47 = bytes 32-39 XOR IVAL6) can fill the
other 48 bytes of the block, plus up to 63 suffix bytes, with anything;
the partner (flip bit 0 of bytes 0 and 8) then collides for between
0.839 and 0.911 of seeds, median 0.855 over random content. Which seeds
collide depends only on bytes 0-7 and 32-39 (m0, m4); the rate depends
only on m4. If the attacker also picks m4 (the published value), the rate
is 0.9106 whatever the other 40 + 63 bytes are.

## 2. A second tie on lanes 3 and 4

Commands: `./komihash_ext tie34 20 200`, `./komihash_ext tie34 20 200
deadbeef` (`tie34.txt`, `tie34_deadbeef.txt`). Each run measures the
flip-B pair on bases that carry both ties, for j = 0 and j = 2, with m6
uniform and with m6 tuned.

Constants: IVAL3 ^ IVAL4 = 0xac27c2bac5d15d59 (bit 0 set, bit 1 clear,
bit 2 clear); IVAL7 ^ IVAL8 = 0xff28fc027c3b59ca (bit 0 clear, bit 1
set, bit 2 clear). The lowest bit clear in both is j = 2.

The clear-bit condition turns out not to be needed for the tie: m3 =
m2 ^ D, and flipping bit j in both m2 and m3 gives m3' = m2' ^ D for any
D. What the bit position does change is the carry: flipping bit j changes
the lane operand by +-2^j and the 128-bit product by +-2^j * v, whose top
j bits land directly in the high half. So j = 0 is the right choice, and
j = 2 was measured only to show the cost.

| pair (both ties present) | rate min / median / mean / max over 200 bases | P(high half unchanged) | P(collision given a change) |
|---|---|---|---|
| j = 0, m6 uniform | 0.6777 / 0.7111 / 0.7455 / 0.8214 | 0.357-0.643 | 0.5001 (0.4984-0.5021) |
| j = 0, m6 = published m4 ^ IVAL7 | 0.8207 / 0.8214 / 0.8214 / 0.8225 | 0.6428 | 0.5000 |
| j = 2, m6 uniform | 0.2042 / 0.2556 / 0.3048 / 0.4692 | 0.030-0.244 | 0.2134 |
| j = 2, m6 tuned | 0.4683 / 0.4696 / 0.4696 / 0.4706 | 0.2435 | 0.2988 |

Second stream, j = 0: uniform m6 0.6783 / 0.7115 / 0.7486 / 0.8217;
tuned m6 0.8203 / 0.8214 / 0.8214 / 0.8223.

The conditional rate given a carry is 1/2 here, not the 3/4 of lanes 1-2:
IVAL7 ^ IVAL8 has bit 1 set, so Seed7 and Seed8 agree only in bit 0 and
a +-1 flips the same bits in both sums only when that bit is 0. Tuning
m6 = m4 ^ IVAL7 makes lane 3 multiply by the same v = m4 ^ S5 as lane 1,
which is why the tuned rate is 0.643 + 0.357 * 1/2 = 0.821.

**Answer 2.** The lanes-3/4 pair collides for 0.68-0.82 of seeds on
random bases (0.821 with m6 tuned), flipping bit 0; using j = 2 costs
most of it (0.20-0.47).

## 3. Multicollisions

### 3a. Four messages with both ties

Commands: `./komihash_ext multi 20 200`, `./komihash_ext multi 24 1`
(`multi.txt`, `multi24.txt`). Best base: published m0, m2, m4 and
m6 = m4 ^ IVAL7; the other four words fixed by the ties:

    m0..m7 = 0000000000000000 13198a2e03707344 1111111111111111 bd36d3abd4c04c48
             5ea1c0c1a4ddf9f0 e0f5a60e9034f59c 9e0de9766da1a92d 61251574119af0e7
    msg 0 = 0000000000000000447370032e8a19131111111111111111484cc0d4abd336bdf0f9dda4c1c0a15e9cf534900ea6f5e02da9a16d76e90d9ee7f09a1174152561
    msg 1 = 0100000000000000457370032e8a19131111111111111111484cc0d4abd336bdf0f9dda4c1c0a15e9cf534900ea6f5e02da9a16d76e90d9ee7f09a1174152561  (flip A)
    msg 2 = 0000000000000000447370032e8a19131011111111111111494cc0d4abd336bdf0f9dda4c1c0a15e9cf534900ea6f5e02da9a16d76e90d9ee7f09a1174152561  (flip B)
    msg 3 = 0100000000000000457370032e8a19131011111111111111494cc0d4abd336bdf0f9dda4c1c0a15e9cf534900ea6f5e02da9a16d76e90d9ee7f09a1174152561  (both)

2^24 seeds: A-pair 0.9106, B-pair 0.8212, product 0.7477, **all four
equal 12,709,897/16,777,216 = 0.7576**. Largest same-output class among
the four: 1: 0.0258, 2: 0.2166, 3: never, 4: 0.7576. (2^20 seeds:
794,675/1,048,576 = 0.7579.) The 4-way rate sits slightly above the
product because with the tuned m6 both lanes multiply by the same v, so
their no-carry events are positively correlated.

200 random bases with both ties and all free words uniform, 2^20 seeds
each: 4-way rate min 0.5804, median 0.6425, mean 0.6553, max 0.7570.

### 3b. All four lanes tied to lane 1 (16 free bytes: m0 and m4)

Commands: `./komihash_ext all8 20 5`, `./komihash_ext all8 24 1`
(`all8.txt`, `all8_24.txt`). With every product equal, flipping bit 0 in
an even-size set of lanes cancels the low halves, so the eight messages
are the flip sets {}, {1,2}, {3,4}, {1,3}, {2,4}, {1,4}, {2,3},
{1,2,3,4} (each message differs from msg 0 in 2 or 4 bytes). Best base
(published m0, m4; m1..m3, m5..m7 by the tie):

    m0..m7 = 0000000000000000 13198a2e03707344 a4093822299f31d0 082efa98ec4e6c89
             5ea1c0c1a4ddf9f0 e0f5a60e9034f59c 9e0de9766da1a92d 61251574119af0e7

2^24 seeds: **8-way collision 10,778,220/16,777,216 = 0.6424**, which is
exactly P(no carry) (8-way given no carry 10,778,220/10,778,220; given a
carry 0/5,998,996). Pair rates against msg 0: {1,2} 0.9106, {3,4}
0.8212, {1,3} {2,4} {1,4} {2,3} 0.6424 each, {1,2,3,4} 0.7317. Largest
same-output class: 2: 0.2683, 4: 0.0893, 8: 0.6424. Control: a
single-lane flip (odd set) collided 0 times in 2^24.

Five random (m0, m4) at 2^20 seeds: 8-way 0.3932, 0.5844, 0.3852,
0.6424, 0.5991, each equal to that base's P(no carry); random base 0 at
2^24: 6,611,965/16,777,216 = 0.3941.

Reading: when the bit-0 flip does not carry into the high half, nothing
but the cancelling low halves changes, so all eight collide at once; when
it carries, the +-1 must flip the same bits of each affected
(Seed_{i+4} + h), and IVAL7, IVAL8, IVAL6^IVAL7, IVAL6^IVAL8 all have
bit 0 set, so lanes 1/3, 1/4, 2/3, 2/4 never agree; {1,2} agree with
probability 3/4, {3,4} 1/2, {1,2,3,4} 1/4.

### 3c. Length-free variation: the low 3 bits of m0

Commands: `./komihash_ext low3 20 5`, `./komihash_ext low3 24 1`
(`low3.txt`, `low3_24.txt`). Lanes 1-2 tied (lanes 3-4 also tied but not
varied); eight messages = m0 with its low 3 bits set to 0..7 and m1
re-tied each time.

Best base (published m0, m4), 2^24 seeds, largest same-output class
among the 8: 4: 0.1088, 5: 0.2252, 6: 0.1225, 7: 0.0745, **8: 0.4690**
(7,868,058/16,777,216). Classes 1-3 never occur. P(all eight lane-1 high
halves equal) is only 0.1552; the rest of the 8-way rate comes from the
carry-agreement structure: the map h -> (Seed5 + h) ^ (Seed6 + h) is
constant on aligned blocks of four consecutive h (IVAL6 has two trailing
zero bits), and the high halves of eight consecutive multiplier values
form a non-decreasing run with steps of 0 or 1, so at least four of the
eight always share an output.

Five random bases (2^20 seeds): 8-way 0.1627, 0.1726, 0.1593, 0.3564,
0.1673; the smallest largest-class was 4 in every run.

**Answer 3.** With both ties (32 constrained bytes) the four messages
collide simultaneously for 0.758 of seeds (0.58-0.76 on random content);
tying all four lanes (48 constrained bytes, 16 free) gives an 8-way
collision for 0.642 of seeds with the published m4 (0.39-0.64 random),
and never larger than 8 from bit-0 flips because odd flip sets never
collide; varying the low 3 bits of m0 gives eight messages of which at
least four always agree and all eight agree for 0.469 of seeds.

## 4. Length scope

Command: `./komihash_ext len 22` (`len22.txt`; `len.txt` is the 2^20
run). The 4-message set of 3a as the first block of longer messages
(suffix bytes 0x99), 2^22 seeds per length:

| length | A-pair | B-pair | 4-way |
|---|---|---|---|
| 63 | 0 | 0 | 0/4,194,304 |
| 64 | 0.9104 | 0.8212 | 3,177,677/4,194,304 = 0.7576 |
| 96 | 0.9108 | 0.8210 | 3,178,275/4,194,304 = 0.7578 |
| 127 | 0.9104 | 0.8212 | 3,177,035/4,194,304 = 0.7575 |
| 128 | 0 | 0 | 0/4,194,304 |

**Answer 4.** Confirmed: 64..127 bytes only. At 128 bytes the second
loop pass re-mixes the state and nothing survives; below 64 bytes the
loop is not entered.

## What this means for the "ANY message" bar

There is no method here that takes an arbitrary message and produces a
colliding partner: the family needs 16 of the 64 block bytes (2-way), 32
(4-way) or 48 (8-way) to satisfy fixed XOR relations with public
constants, and on a message that lacks them the same flips collided 0
times in 20 x 2^20 trials. What an attacker who chooses the inputs does
get, without knowing the seed, is a 2-way collision for 84-91% of seeds
with 48 block bytes plus up to 63 suffix bytes free, a 4-way collision
for 58-76%, an 8-way collision for 39-64%, and eight length-64 messages
that always contain at least four with the same output, at any length
from 64 to 127 bytes.
