# HighwayHash: a fixed 96-byte pair that collides for 2^-24.22 of the keys in a 2^-32 key class

Reader-runnable companion to the blog post. One C11 file, no dependencies, single-threaded.

## What the hash is

[HighwayHash](https://github.com/google/highwayhash) by Google (Apache-2.0): a keyed hash with a
256-bit key (four 64-bit words) and 64-, 128- or 256-bit output, built on 32x32-bit multiplies and a
byte "zipper" permutation over four 64-bit lanes. The repository declares all three output widths
frozen ("unchanging forever"), so there is no version number to quote; the reference is the
repository's portable C implementation `c/highwayhash.c` (sha256
`fb316726f8d95ab83ad4721058586abb2e98ede635d48a2fd7ec5d6b15543fca`, the copy vendored at
`tools/bench/adversarial/vendor/highwayhash/c/` in the paper repository).

`highwayhash_verify.c` embeds that file **verbatim** between the two marker lines (the only edit is
line 1, `#include "c/highwayhash.h"`, replaced by a comment; the two typedefs from that header are
copied just above). To check it against the repository file:

    sed -n '/BEGIN verbatim copy of c\/highwayhash.c/,/END verbatim copy of c\/highwayhash.c/p' highwayhash_verify.c | sed '1d;$d' | diff - path/to/highwayhash/c/highwayhash.c
    # -> only line 1 (the #include) differs

Validation at startup.  The program exits with status 1 if either of the first two checks
fails, 2 if the pair record disagrees with its description (or an argument is malformed),
and 4 if the explicit key does not reproduce the published values:

* the SMHasher3 verification values of the three registered variants `HighwayHash_64`
  (0xF3246108), `HighwayHash_128` (0x232D434E) and `HighwayHash_256` (0x0D50D328), from
  SMHasher3's `hashes/highwayhash.cpp`, computed by the procedure of `lib/Hashinfo.cpp`
  `_ComputedVerifyImpl` (keys {}, {0}, {0,1}, ..., {0..254} hashed with seed 256-i, the
  little-endian outputs concatenated and hashed with seed 0, first 4 bytes little-endian).
  SMHasher3 turns its 64-bit seed into the 256-bit key as `key[i] = {1,2,3,4}[i] ^ seed`; the
  program does the same for this step only;
* the 33-byte test vector of the repository's `c/highwayhash_test.c` (key {1,2,3,4}, bytes
  128..160, expected 53c516cce478cad7);
* the published explicit colliding key (below): both messages must hash to the published
  `HighwayHash64` value `f5eba26391be727f` and collide at all three widths (exit code 4
  otherwise).  This is stronger than "collides": flipping, say, the lowest bit of `key[0]`,
  which the lemma leaves unconstrained, still gives a full-state collision but a different
  value, and is caught.

Threat model: the attacker knows the message pair and nothing about the key, which is uniformly
random. HighwayHash's native "seed" *is* the 256-bit key, so the seeds sampled below are keys.

## What the pair exploits

Two 96-byte messages, three 32-byte packets each, identical except in lane 0 (the first 8 bytes)
of every packet, where m2 = m1 + 2^8 in packet 1, - (2^9 + 2^24) in packet 2 and + 2^8 in packet
3 (mod 2^64). Lane 0 of packet 1 is chosen as the negative of the lane-0 initialisation constant,
so the packet-1 update `v1[0] += mul0[0] + lane0` adds zero for m1 and exactly +2^8 for m2. If the
key satisfies `hi32(key[0]) = hi32(init0[0]) = 0xdbe6d5d5` (32 fixed bits: density exactly 2^-32)
then `Reset` leaves `hi32(v0[0]) = 0`, so packet 1's lane-0 multiply `lo32(v1[0]) * hi32(v0[0])` is
zero whatever the message: the +2^8 survives packet 1 as a single-byte additive difference that the
zipper byte permutation only relocates (to byte 5 of `v0[0]` and byte 3 of `v1[0]`; the panel's
lemma shows the relocations cannot carry for any key in the class). Packet 2's -(2^9 + 2^24)
collapses those differences to a single -2^8 in `v1[0]` exactly when the two operands of the
packet-2 lane-0 multiply satisfy `lo32(v1[0]) - hi32(v0[0]) = 2^8 (mod 2^32)` (event E_2), and
packet 3's +2^8 cancels that. The full 1024-bit state is then identical, so HighwayHash-64, -128
and -256, and any common suffix, all collide.

The panel proved Pr[E_2 | class] = 235 * 239 / 2^40 = 2^-24.22 exactly (a byte-equation count;
the two operands share key bits, which is why it is 2^7.8 times more than the 2^-32 of a random
32-bit relation) and confirmed it by sampling (about 2100 collisions over 4.3e10 class keys, all
of them full-state collisions at every width, none outside E_2). So over a uniform 256-bit key the
pair collides with probability at least 2^-32 * 2^-24.22 = 2^-56.22 at every output width, against
2^-64 / 2^-128 / 2^-256 for an ideal hash; whether other trails add to that is not known.

What the program measures, and at what resolution: the uniform-key run cannot see 2^-56 (2^24
keys expect 1e-10 collisions) and is a null check. The class run hashes N class keys in full,
where 2^24 keys expect 0.86 collisions and 2^26 expect 3.4, so its count is a Poisson check of the
proved rate, not a measurement. The measurement is the E_2 screen: 16N class keys pass through
`Reset` plus one packet (all the event needs), which at 2^28 keys expects 13.7 hits and at 2^30
expects 54.8; every hit is then hashed in full at all three widths and must collide. The program
reports both, plus the class density, and the first colliding class key it meets.

With the third argument `sm3` the class run uses SMHasher3's seed-to-key map instead
(`hi32(seed) = 0xdbe6d5d5`, also density 2^-32 over 64-bit seeds). There `key[1] = key[0] ^ 3`
is no longer independent of `key[0]`, so the lemma's count does not apply; the measured E_2 rate
in that class is 0 in 2^28 seeds (13.7 expected at the lemma's rate), so this class is not weak
under SMHasher3's map. Whether some other 64-bit seed class is was not analysed; the attack as
stated is against the real 256-bit-key API.

## Build and run

    cc -O2 -std=c11 -o highwayhash_verify highwayhash_verify.c -lm
    ./highwayhash_verify            # 2^24 keys per experiment (2^28 for the E_2 screen)
    ./highwayhash_verify 26         # 2^26 keys (2^30 for the screen)
    ./highwayhash_verify 24 7       # second argument: RNG seed
    ./highwayhash_verify 24 7 sm3   # class run under SMHasher3's seed-to-key map

The RNG is splitmix64 seeding xoshiro256** with a fixed default seed (20260917), so the default
output below is reproducible byte for byte. Compiled clean with
`clang -std=c11 -Wall -Wextra -pedantic` (Apple clang 17). Single-threaded. A
`-fsanitize=undefined` build runs clean with `-fno-sanitize=pointer-overflow`; that one check trips
on the reference's own `remainder[i + size_mod4 - 4]` (the upstream read-before-the-tail trick in
`HighwayHashUpdateRemainder`, a wrapped unsigned offset), which is upstream code, not the demo's.

## Expected output

`./highwayhash_verify` (2^24 keys, 2^28 for the E_2 screen, default RNG seed; 16 s single-threaded on an Apple M2 Pro):

```
HighwayHash (google/highwayhash reference C, frozen; SMHasher3 HighwayHash_64/128/256): weak-key collision pair
validation: HighwayHash_64  SMHasher3 verification 0xF3246108 (expected 0xF3246108) OK
            HighwayHash_128 SMHasher3 verification 0x232D434E (expected 0x232D434E) OK
            HighwayHash_256 SMHasher3 verification 0x0D50D328 (expected 0x0D50D328) OK
            upstream 33-byte test vector 53c516cce478cad7 (expected 53c516cce478cad7) OK

pair: 96 bytes = three 32-byte packets; additive lane-0 trail +2^8 | -(2^9+2^24) | +2^8; weak-key class hi32(key[0]) = 0xdbe6d5d5
mechanism: in the class hi32(v0[0]) = 0 after Reset, so packet 1's lane-0 multiply is 0 and the +2^8 in v1[0] stays
  a single-byte additive difference that the zipper merely relocates; packet 2's -(2^9+2^24) collapses it to one -2^8
  iff the packet-2 lane-0 multiplier operands satisfy lo32(v1[0]) - hi32(v0[0]) = 2^8 mod 2^32 (event E_2, proved
  probability 235*239/2^40 over the other key bits); packet 3's +2^8 cancels it, the whole 1024-bit state is equal,
  so all three output widths and any common suffix collide.
m1 = d131b3012a2a1924000000000000000000000000000000000000000000000000d132b3b42a2a19240000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
m2 = d132b3012a2a1924000000000000000000000000000000000000000000000000d130b3b32a2a19240000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000
lane-0 words of m2 - m1 = +2^8 | -(2^9+2^24) | +2^8 (mod 2^64), lanes 1..3 identical: OK

uniform 256-bit keys, N = 2^24 (rng seed 20260917):
  HighwayHash64 collisions: 0/16777216, none observed in 2^24 trials; no population bound
  (proved lower bound over uniform keys 2^-32 * 235*239/2^40 = 2^-56.22: 2.0e-10 expected here, so 0 is the null check)

weak-key class hi32(key[0]) = 0xdbe6d5d5, 32 of the 256 key bits fixed (density exactly 2^-32), other bits uniform:
  N = 2^24 class keys, both messages hashed in full (expected 0.86 collisions at the proved rate):
    HighwayHash64 collisions: 0/16777216, none observed in 2^24 trials; no population bound
    HighwayHash128 0, HighwayHash256 0, full 1024-bit state equal after packet 3 0, E_2 0
  E_2 screen of 16N = 2^28 class keys (Reset + packet 1 each; every hit then hashed in full):
    E_2 hits: 11/268435456 = 4.098e-08, rate 2^-24.54
    expected 13.7 +- 3.7 at the proved rate 235*239/2^40 = 2^-24.22; hits colliding at 64/128/256 bits: 11/11/11, full state: 11
    first colliding class key: dbe6d5d59d0b3dbd 1941e8bd6e52518f c2f54017a2da251d cf7062fc9a79fdd1  ->  HighwayHash64 0dede7ad74435b80 for both
  class density 2^-32 x proved conditional rate 2^-24.22 => eps >= 56165/2^72 = 2^-56.22 over uniform keys at every width

explicit key dbe6d5d58afad71e a0142b42de197939 5bd2b2861106bd66 b6c304527caad524 (published: HighwayHash64 = f5eba26391be727f for both)
  HighwayHash64  h(m1) = f5eba26391be727f  h(m2) = f5eba26391be727f  COLLIDE
  HighwayHash128 h(m1) = 46b567c5d05f08f37b1fa476689ef04e  h(m2) = 46b567c5d05f08f37b1fa476689ef04e  COLLIDE
  HighwayHash256 h(m1) = 38ac5302dbcd9e232fafb0b99164d0f3ce0f093fd9d7df5ff198aa4797f74288
                 h(m2) = 38ac5302dbcd9e232fafb0b99164d0f3ce0f093fd9d7df5ff198aa4797f74288  COLLIDE
```

With `./highwayhash_verify 26` (2^26 keys, 2^30 for the screen; 54 s) the two sampling blocks become

```
uniform 256-bit keys, N = 2^26 (rng seed 20260917):
  HighwayHash64 collisions: 0/67108864, none observed in 2^26 trials; no population bound
  (proved lower bound over uniform keys 2^-32 * 235*239/2^40 = 2^-56.22: 8.0e-10 expected here, so 0 is the null check)

weak-key class hi32(key[0]) = 0xdbe6d5d5, 32 of the 256 key bits fixed (density exactly 2^-32), other bits uniform:
  N = 2^26 class keys, both messages hashed in full (expected 3.43 collisions at the proved rate):
    HighwayHash64 collisions: 1/67108864 = 1.490e-08, rate 2^-26.00
    HighwayHash128 1, HighwayHash256 1, full 1024-bit state equal after packet 3 1, E_2 1
  E_2 screen of 16N = 2^30 class keys (Reset + packet 1 each; every hit then hashed in full):
    E_2 hits: 47/1073741824 = 4.377e-08, rate 2^-24.45
    expected 54.8 +- 7.4 at the proved rate 235*239/2^40 = 2^-24.22; hits colliding at 64/128/256 bits: 47/47/47, full state: 47
    first colliding class key: dbe6d5d58d1f62e8 bf94a5f17ccab12f b4d41aba2c175442 a3577e0a16136f50  ->  HighwayHash64 ecc5c01dadf99230 for both
  class density 2^-32 x proved conditional rate 2^-24.22 => eps >= 56165/2^72 = 2^-56.22 over uniform keys at every width
```

Both screens sit within one standard deviation of the proved rate (11 hits for 13.7 expected, 47 for 54.8), and every
hit collided at all three widths.

With `./highwayhash_verify 24 20260917 sm3` (SMHasher3's seed-to-key map) the class block becomes

```
weak-key class hi32(seed) = 0xdbe6d5d5 under SMHasher3's map key[i] = {1,2,3,4}[i] ^ seed (density exactly 2^-32), other bits uniform:
  N = 2^24 class keys, both messages hashed in full (expected 0.86 collisions at the proved rate):
    HighwayHash64 collisions: 0/16777216, none observed in 2^24 trials; no population bound
    HighwayHash128 0, HighwayHash256 0, full 1024-bit state equal after packet 3 0, E_2 0
  E_2 screen of 16N = 2^28 class keys (Reset + packet 1 each; every hit then hashed in full):
    E_2 hits: 0/268435456, none observed in 2^28 trials; no population bound
    expected 13.7 +- 3.7 at the proved rate 235*239/2^40 = 2^-24.22; hits colliding at 64/128/256 bits: 0/0/0, full state: 0
  class density 2^-32 x proved conditional rate 2^-24.22 => eps >= 56165/2^72 = 2^-56.22 over uniform keys at every width
```

Every colliding key the program prints (the published one and the first one it finds) collides at all three
output widths and in the full 1024-bit state; the panel's independent verifiers reproduced the published key
and rate with their own samplers.

## License

The demo code (everything outside the verbatim section) is MIT, Copyright (c) 2026 Thomas Dybdahl Ahle.
The embedded `c/highwayhash.c` is Copyright 2017 Google Inc., Apache License 2.0.  Upstream's file
carries no notice of its own (the license lives in the repository's LICENSE file), so the Google copyright
line, the Apache-2.0 reference and a statement of the one modification are given in `highwayhash_verify.c`'s
own header; the vendored section itself is unmodified apart from its first line.
