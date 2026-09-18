# t1ha2-64: two fixed message pairs that collide with probability 2^-30 over a random seed

## The hash

t1ha2 ("Fast Positive Hash", generation 2) by Leonid Yuriev / Positive Technologies,
zlib license.  This is the one-shot 64-bit function `t1ha2_atonce(data, len, seed)`
with a 64-bit seed, i.e. the variant SMHasher3 registers as `t1ha2_64`
(`hashes/t1ha.cpp`, template `t1ha2<MODE_LE_NATIVE,false>`, `verification_LE = 0x8F16C948`).
SMHasher3 carries a frozen copy of upstream t1ha v2.1
(<https://web.archive.org/web/20211209095620/https://github.com/erthink/t1ha>;
the live repository is <https://github.com/PositiveTechnologies/t1ha>).

`t1ha2_64_verify.c` re-implements the hash from that source.  The only copied material is
the upstream self-check: the 81-entry known-answer table `t1ha_refval_2atonce`, the 64-byte
test pattern and the probe schedule of `t1ha_selfcheck`, all under the zlib license quoted
in the source header.  The program validates the implementation at startup
against all 81 known answers and against the SMHasher3 verification value, computed the
SMHasher3 way: hash the keys `{}`, `{0}`, `{0,1}`, ... of length 0..255 with seed
`256 - len`, concatenate the little-endian outputs, hash the 2048 bytes with seed 0 and
take the first four bytes little-endian.  The program aborts if either check fails.

## What the pairs exploit

For inputs of 9..16 bytes t1ha2 absorbs only two words.  Word 0 enters as the 128-bit
product `(len + w0) * P2` with `len` public, so after the first step the state word is
`a = seed ^ l0` with `l0` known to the attacker; the tail word `t` then enters as
`u = a + t`, `(L, H) = u * P1`, and the final state is `(a + H, b ^ L)`.  Only one
seed-dependent carry pattern separates a chosen pair from a state collision: pick a
signed-digit difference on the word-0 product, choose the tail difference so that
`hi(du * P1)` plus its carry cancels that difference, and use a length change (16 vs 11
bytes, or 13 vs 16) to make the public part of the state difference match.  The two
messages then collide exactly when 26 (pair A) or 27 (pair B) bits of `L` take fixed
values and four addition carries have the right sign, which happens for a uniformly random
seed with probability `2^-26 * 2^-4 = 2^-30.00` (pair A) and `2^-27 * 2^-4.01 = 2^-31.01`
(pair B), against `2^-64` for an ideal 64-bit hash.  Since `L` is a bijective image of the
seed, the program samples the seed class directly (density exactly `2^-26` / `2^-27`) and
multiplies the conditional rate back out.

Pair B is the same mechanism with a different digit route; it is included because it was
the pair that received the largest independent uniform-seed run (30 collisions at 2^36
seeds, 2^-31.09).  Pair A, the worst pair found, got 28 collisions at 2^35 uniform seeds
(2^-30.19) in the same independent check.  Those figures are the panel verifier's, quoted
from its report; the runs are not part of this package and cannot be re-derived from it.
The program's uniform-seed run stays at 2^24..2^26 seeds, where 0 collisions is the
expected outcome (0.016 / 0.062 expected), and the conditional run supplies the rate.

The product "class density x conditional rate" is the contribution of the stated seed
class alone.  It equals the uniform rate only if the pair collides nowhere outside the
class, which the analysis says but the program does not prove; collisions outside the
class would only raise the rate, so the printed figure is a lower bound.  Every uniform
hit the program does find is tested for class membership and reported on the `first
uniform colliding seed` line (a 2^31-seed run, not in the package, found six uniform hits,
all inside the class).

## Build and run

```
cc -O2 -std=c11 -o t1ha2_64_verify t1ha2_64_verify.c -lm
./t1ha2_64_verify            # 2^24 seeds per experiment, rng seed 1 (about 1.3 s)
./t1ha2_64_verify 26         # 2^26 seeds (about 4.5 s); a value above 63 is taken as the count
./t1ha2_64_verify 24 7       # second argument: rng seed (pair i is run with rng seed + i)
```

A non-numeric argument is rejected with exit status 1.

Requirements: gcc or clang (uses `unsigned __int128`), a little-endian host, no other
dependencies, single-threaded.

## Expected output (`./t1ha2_64_verify`, Apple M-series laptop)

```
t1ha2-64 (t1ha2_atonce, 64-bit output, 64-bit seed): fixed message pairs under random seeds
validation: upstream KAT 81/81 ok; SMHasher3 verification 0x8F16C948 (want 0x8F16C948) ok
rng: xoshiro256** seeded by splitmix64(1 + pair index)

pair A (worst confirmed pair)
  m1 (16 bytes) = 406b68c281a55e00158ed10a0d96f8ff
  m2 (11 bytes) = d1d86241d24d9f990323e0
  mechanism: 16-byte vs 11-byte messages: a signed-digit difference in word 0 (public
    (len+w0)*P2 shift) is cancelled by the tail word's carry pattern through P1;
    only 26 bits of L and 4 carry signs depend on the seed
  uniform seeds: N = 2^24.00 (16777216): 0 collisions, rate unestimated (zero observed in 2^24.00 trials)  (expected at the published 2^-30.00: 0.016)
  seed class: (L & 7f868a5066451822) == 16808a1062000020, L = lo(((seed ^ l0) + t) * P1), density 2^-26
  conditional: N = 2^24.00 seeds in the class: 1048930 collisions, rate 2^-4.00
  => estimated class contribution = 2^-26 * 2^-4.00 = 2^-30.00  (published 2^-30.00)
  first class colliding seed: 81cd5c0da0872b93
  explicit seed 3c805cc67a687f30: H(m1) = f2a1196d24fddaab  H(m2) = f2a1196d24fddaab  COLLIDE (L = 96b1df91fb0202ed, in class: yes)

pair B (same mechanism, second digit route)
  m1 (13 bytes) = 9858cabfcf20f692468be82bd3
  m2 (16 bytes) = 95589a09e664a742a4ac26262cc7fdfc
  mechanism: 13-byte vs 16-byte messages: 4-digit route +2^56-2^46-2^20+2^50 on the
    word-0 product, tail difference chosen so the P1 carries cancel it;
    27 bits of L and 4 carry signs fixed
  uniform seeds: N = 2^24.00 (16777216): 0 collisions, rate unestimated (zero observed in 2^24.00 trials)  (expected at the published 2^-31.01: 0.008)
  seed class: (L & 4d4ce1e6964c5012) == 4c408040000c1012, L = lo(((seed ^ l0) + t) * P1), density 2^-27
  conditional: N = 2^24.00 seeds in the class: 1039373 collisions, rate 2^-4.01
  => estimated class contribution = 2^-27 * 2^-4.01 = 2^-31.01  (published 2^-31.01)
  first class colliding seed: 52b21e2020a5f31f
  explicit seed d870b802f1050157: H(m1) = 63cf67ba079a2997  H(m2) = 63cf67ba079a2997  COLLIDE (L = de608850619f1396, in class: yes)
```

With `./t1ha2_64_verify 26` the summary lines become

```
  uniform seeds: N = 2^26.00 (67108864): 0 collisions, rate unestimated (zero observed in 2^26.00 trials)  (expected at the published 2^-30.00: 0.062)
  conditional: N = 2^26.00 seeds in the class: 4189766 collisions, rate 2^-4.00
  => estimated class contribution = 2^-26 * 2^-4.00 = 2^-30.00  (published 2^-30.00)
  first class colliding seed: dbd56e042bf65913
  uniform seeds: N = 2^26.00 (67108864): 0 collisions, rate unestimated (zero observed in 2^26.00 trials)  (expected at the published 2^-31.01: 0.031)
  conditional: N = 2^26.00 seeds in the class: 4157564 collisions, rate 2^-4.01
  => estimated class contribution = 2^-27 * 2^-4.01 = 2^-31.01  (published 2^-31.01)
  first class colliding seed: 5c781594f9a4cf46
```

The seeds the program prints as colliding (the published ones and the ones it finds) were
also re-checked by the panel's verifier with its own, independently written implementation
of the hash.  That implementation is not shipped here; this package contains only the one
implementation described above.

## License

`t1ha2_64_verify.c` is MIT (full text in the file header).  The t1ha algorithm, the
known-answer table, the test pattern and the probe schedule are from the t1ha project,
zlib license, Copyright (c) 2016-2020 Positive Technologies and Leonid Yuriev; the zlib
notice is reproduced in the source header.
