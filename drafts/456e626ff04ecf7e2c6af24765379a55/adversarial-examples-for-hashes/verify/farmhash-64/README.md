# farmhash-64: seed-independent collisions for FarmHash64 (NA)

## The hash

**FarmHash v1.1, `farmhashna::Hash64WithSeed`** by Geoff Pike (Google, 2014, MIT),
<https://github.com/google/farmhash>. SMHasher3 (<https://gitlab.com/fwojcik/smhasher3>)
registers it as `FarmHash_64__NA` with `verification_LE = 0xEBC4A679`; the implementation
in `farmhash64_pairs.c` is transcribed from SMHasher3's `hashes/farmhash.cpp` (NA namespace,
little-endian path). It is validated at startup by reproducing SMHasher3's verification value
(`HashInfo::_ComputedVerifyImpl`: keys `{}`, `{0}`, `{0,1}`, ..., `{0..254}` with seed `256-i`,
the 2048-byte result array hashed with seed 0, first four bytes little-endian) and, as an
independent published vector, by checking that Orson Peters' string
`orlp-farmhash64-?VrJ@L7ytzwheaaa` has unseeded `Hash64 == 1337`. The program aborts if
either check fails.

## What the pairs exploit

The seed enters FarmHash64 only in the very last step:
`Hash64WithSeed(m, seed) = HashLen16(Hash64(m) - k2, seed)`, and the 128-bit interface is
`Hash64WithSeeds(m, s0, s1) = HashLen16(Hash64(m) - s0, s1)`. For a fixed second argument,
`HashLen16(u, v)` is a bijection of `u` (xor with `v`, multiply by an odd constant, the
involution `u ^= u >> 47`, repeated), so two messages with equal *unseeded* `Hash64` collide
under **every** seed: the collision probability over a uniform hidden seed is exactly 1, and
`bits = log2(L / eps) = 0`. The attacker therefore only needs a collision of the public,
unkeyed function. Pair A (8 bytes) is such a collision found by a distinguished-point rho
walk (about 2^31.8 evaluations, a plain birthday attack); pair B (32 bytes) needs no search
at all: `HashLen17to32` is affine-invertible in its last two words, so `w2, w3` are solved in
closed form to place both messages in one `HashLen16` collision class (the same construction
gives arbitrary multicollisions). Pair C is Peters' published pair
(<https://orlp.net/blog/breaking-hash-functions/>), included as an external cross-check.

Pairs A and B were confirmed by an independent implementation at 2^30 seeds each. By code
identity (not run here) the same pairs collide for every seed in `FarmHash_64__UO/XO/TE`
(which call the NA code for `len <= 64`) and in CityHash64 v1.1, whose short-input paths and
seed combine are identical.

## Build and run

    cc -O2 -std=c11 -o farmhash64_pairs farmhash64_pairs.c -lm
    ./farmhash64_pairs            # 2^24 seeds per pair (about 1.5 s single-threaded)
    ./farmhash64_pairs 26         # 2^26 seeds (about 5 s); optional 2nd argument = RNG seed

For each pair the program prints both messages in hex, their unseeded `Hash64` values, the
collision count over N random 64-bit seeds (`Hash64WithSeed`) and over N random
`(seed0, seed1)` pairs (`Hash64WithSeeds`), the explicit seed `82bdf567d8ebbf4f` with both
hash values, and the first sampled colliding seed. There is no weak-seed class to sample:
the pairs are key-free, so the "class" is all 2^64 seeds (density 1).

The RNG is re-seeded with the same seed at the start of every pair, so the three pairs are
tested against the identical seed stream (which is why they all report the same first
colliding seed); the streams are not independent per pair, and need not be, since every
seed collides.  The exit status is 0 only if the validation passes and every pair collides
under every sampled seed and the explicit seed; a non-colliding seed makes it 1, and a
non-numeric argument 2.

## Expected output (`./farmhash64_pairs`, 2^24 seeds, Apple clang, arm64)

```
farmhash-64: FarmHash v1.1 NA, farmhashna::Hash64WithSeed (SMHasher3 FarmHash_64__NA)
validation: SMHasher3 verification_LE = 0xEBC4A679 (expected 0xEBC4A679) OK
validation: unseeded Hash64("orlp-farmhash64-?VrJ@L7ytzwheaaa") = 1337 (published: 1337) OK

mechanism: Hash64WithSeed(m, seed) = HashLen16(Hash64(m) - k2, seed) and
           Hash64WithSeeds(m, s0, s1) = HashLen16(Hash64(m) - s0, s1).  For a fixed second
           argument HashLen16(u, v) is a bijection of u (xor with v, multiply by an odd
           constant, the involution u ^= u >> 47), so the seed never separates two messages
           with equal unseeded Hash64: every such pair collides for EVERY seed (rate 1, key-free).
conditional on: none (no weak-seed class; the class is all 2^64 seeds, density 1)

seeds: N = 2^24 = 16777216 uniformly random 64-bit seeds per pair (xoshiro256**, seed 1)

pair A: 8-byte pair: collision of the public unseeded Hash64 found by a distinguished-point rho
      walk (~2^31.8 evaluations); verifier-confirmed at 2^30 seeds, rate exactly 1
  m1 = 574522e6dce98176  (8 bytes)
  m2 = 0df3b1f900d36296  (8 bytes)
  unseeded Hash64: m1 -> 25ddc82d6c8a301a   m2 -> 25ddc82d6c8a301a   (equal)
  Hash64WithSeed(seed):            collisions = 16777216 / 16777216   rate = 1   log2 rate = 0.0000
  Hash64WithSeeds(seed0, seed1):   collisions = 16777216 / 16777216   rate = 1   log2 rate = 0.0000
  explicit seed 82bdf567d8ebbf4f:   H(m1) = 49186432ae14e980   H(m2) = 49186432ae14e980   COLLIDE
  first sampled colliding seed b3f2af6d0fc710c5:   H = 6fcc1f5ef69df188 for both
  non-colliding seeds found: 0

pair B: 32-byte pair: closed form, NO search -- HashLen17to32 is solved for words w2,w3 so both
      messages land in one HashLen16 collision class; verifier-confirmed at 2^30 seeds
  m1 = 726c70206661726d706c61696e683332acc3a05fcc8dbead6d1998c94a300be8  (32 bytes)
  m2 = 736c70206661726de0d8c2d2dcd06664cdeab4eacd29799b6803df46f137a3cd  (32 bytes)
  unseeded Hash64: m1 -> b49071fd6dc4354d   m2 -> b49071fd6dc4354d   (equal)
  Hash64WithSeed(seed):            collisions = 16777216 / 16777216   rate = 1   log2 rate = 0.0000
  Hash64WithSeeds(seed0, seed1):   collisions = 16777216 / 16777216   rate = 1   log2 rate = 0.0000
  explicit seed 82bdf567d8ebbf4f:   H(m1) = 6aeaa95070e78f9e   H(m2) = 6aeaa95070e78f9e   COLLIDE
  first sampled colliding seed b3f2af6d0fc710c5:   H = 9cda96ba16a5b776 for both
  non-colliding seeds found: 0

pair C: Orson Peters' published strings 'orlp-farmhash64-?VrJ@L7ytzwheaaa' and
      'orlp-farmhash64-p3`!SQb}fmxheaaa' (orlp.net/blog/breaking-hash-functions): Hash64 == 1337
  m1 = 6f726c702d6661726d6861736836342d3f56724a404c3779747a776865616161  (32 bytes)
  m2 = 6f726c702d6661726d6861736836342d703360215351627d666d786865616161  (32 bytes)
  unseeded Hash64: m1 -> 0000000000000539   m2 -> 0000000000000539   (equal)
  Hash64WithSeed(seed):            collisions = 16777216 / 16777216   rate = 1   log2 rate = 0.0000
  Hash64WithSeeds(seed0, seed1):   collisions = 16777216 / 16777216   rate = 1   log2 rate = 0.0000
  explicit seed 82bdf567d8ebbf4f:   H(m1) = f8cf13cec7b1d83f   H(m2) = f8cf13cec7b1d83f   COLLIDE
  first sampled colliding seed b3f2af6d0fc710c5:   H = 54673fdd5512d429 for both
  non-colliding seeds found: 0
```

A 2^26 run (`./farmhash64_pairs 26`) gives 67108864 / 67108864 collisions for all three
pairs on both seed interfaces, log2 rate 0.0000, and again finds no non-colliding seed.

## License

`farmhash64_pairs.c` and this README are MIT (Copyright (c) 2026 Thomas Dybdahl Ahle).  The
farmhashna code is transcribed from SMHasher3's `hashes/farmhash.cpp`, so the source header
also reproduces the two upstream MIT copyright lines, Google, Inc. (2014) and Frank J. T.
Wojcik (2021-2022), above the full MIT text.
