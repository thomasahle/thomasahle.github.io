# cityhash-64: key-free collisions for Google CityHash64WithSeed

## The hash

CityHash64WithSeed from **CityHash v1.1.1** (Geoff Pike and Jyrki Alakuijala, Google, MIT),
source: <https://github.com/google/cityhash> (`src/city.cc`).  The exact variant is the one
SMHasher3 tests as `CityHash_64` (`hashes/cityhash.cpp`: 64-bit seed, native little-endian
fetches, verification value `0x5FABC5C5`).  `cityhash64_verify.c` contains a from-scratch C11
transcription of `CityHash64`, `CityHash64WithSeeds` and `CityHash64WithSeed`, and aborts at
startup unless it reproduces SMHasher3's verification value (keys of length 0..255 with
bytes 0..i-1 and seed 256-i, the 2048-byte concatenation hashed with seed 0, first 4 bytes
little-endian).

Note: the two published CityHash64 attacks (Aumasson-Bernstein 2012, Peters 2023) target
CityHash **v1.0.3**, the version inside libc++'s `std::hash`; their messages do not collide
under v1.1.1.  The pairs below are v1.1.1 pairs built on the same observation.

## What the pairs exploit

CityHash64 mixes the seed in one place only: `CityHash64WithSeed(m, seed) =
HashLen16(CityHash64(m) - k2, seed)`, where `HashLen16(u, v)` is xor with `v`, a multiply
by an odd constant and `x ^= x >> 47`, twice.  For any fixed seed that is a bijection of
`u`, so two messages collide under a seed if and only if their *unseeded* `CityHash64`
values are equal, and then they collide under **every** seed (and every `(seed0, seed1)`
of `CityHash64WithSeeds`): the seed adds no collision resistance at all.  Pair A (8 bytes,
L = 1 word) is an unseeded collision found by Brent's rho on the public function at cost
2^32.35; pair B (32 bytes) is analytic: `HashLen17to32` is also a bijection of the third
input word for fixed other words, so one solves that word to hit any target value (here
0x1337), giving 2^192 mutually colliding 32-byte messages.  The program also checks the
explicit inverse of the seed combine (recovering `CityHash64(m)` from a seeded output) and
rebuilds pair B from its recipe.

Pairs (hex, byte order):

    A  m1 = a01109025ea76be1
       m2 = 020bd424b04ae555
    B  m1 = 436974794861736836342d6f6b21212183454502d40dfa393030303030303030
       m2 = 436974794861736836342d6f6b21212183ff1a7c2ab06b6a3030303030303031

Collision rate over uniformly random 64-bit seeds: 1 (log2 rate 0) for both pairs, i.e.
bits = log2(L / eps) = 0 with L = 1 word.  Unconditional (no weak-seed class).

## Build and run

    cc -O2 -std=c11 -o cityhash64_verify cityhash64_verify.c -lm
    ./cityhash64_verify            # 2^24 random seeds (default), about 0.3 s single-threaded
    ./cityhash64_verify 2^26       # or any count: ./cityhash64_verify 1000000 [rng-seed]

Single file, no dependencies (libm for `log2`), one thread.  Exit status 0 only if the
SMHasher3 validation and every check pass.

## Expected output (`./cityhash64_verify`, default 2^24 seeds)

```
cityhash-64: Google CityHash64WithSeed, CityHash v1.1.1 (SMHasher3 variant CityHash_64)
SMHasher3 verification value: 0x5FABC5C5 (expected 0x5FABC5C5) OK
seed combine HashLen16(u, seed) inverted on 2^20 random (u, seed): 0 failures

pair A: 8-byte pair (L = 1 word), found by Brent rho on the public map m -> CityHash64(m)
  m1 = a01109025ea76be1  (8 bytes)
  m2 = 020bd424b04ae555
  mechanism: unseeded CityHash64(m1) == CityHash64(m2); the seed enters only through the bijection u -> HashLen16(u - k2, seed), so the pair collides for every 64-bit seed and every (seed0, seed1) of CityHash64WithSeeds
  seed class: all seeds (unconditional, key-free); class density = 1
  unseeded CityHash64: m1 -> 794ce2bbd3dc7242, m2 -> 794ce2bbd3dc7242 EQUAL (published 794ce2bbd3dc7242)
  random 64-bit seeds: N = 16777216 (2^24.00), collisions = 16777216, rate = 1, log2 rate = 0.0000
  random 128-bit (seed0, seed1), CityHash64WithSeeds: N = 16777216 (2^24.00), collisions = 16777216, rate = 1, log2 rate = 0.0000
  published seed 0x6637c1ce6357a2c8: h(m1) = d4d44b0c5f8bae0a, h(m2) = d4d44b0c5f8bae0a COLLIDE (published d4d44b0c5f8bae0a)
  seed 0x0000000000000000: h(m1) = 3d6746761adc8d10, h(m2) = 3d6746761adc8d10 COLLIDE
  seed 0x0000000000000001: h(m1) = 39336cd0fd4135fe, h(m2) = 39336cd0fd4135fe COLLIDE
  first sampled seed 0xb660d57a1a543321: h(m1) = h(m2) = 34bef59e7a2b2cf1; strip_seed -> 794ce2bbd3dc7242

pair B: 32-byte pair (L = 4 words), analytic preimage of HashLen17to32 with target 0x1337
  m1 = 436974794861736836342d6f6b21212183454502d40dfa393030303030303030  (32 bytes)
  m2 = 436974794861736836342d6f6b21212183ff1a7c2ab06b6a3030303030303031
  mechanism: choose words m0, m1, m3 freely, invert HashLen16(u, v, k2 + 64) and solve m2 = (u - rotr(m0*k1 + m1, 43) - rotr(m3*mul, 30)) * k2^-1: 2^192 messages of length 32 share any chosen unseeded value, hence collide for every seed
  seed class: all seeds (unconditional, key-free); class density = 1
  unseeded CityHash64: m1 -> 0000000000001337, m2 -> 0000000000001337 EQUAL (published 0000000000001337)
  random 64-bit seeds: N = 16777216 (2^24.00), collisions = 16777216, rate = 1, log2 rate = 0.0000
  random 128-bit (seed0, seed1), CityHash64WithSeeds: N = 16777216 (2^24.00), collisions = 16777216, rate = 1, log2 rate = 0.0000
  published seed 0xcbd18ebcd1f9b00d: h(m1) = 9f176442ecd010a8, h(m2) = 9f176442ecd010a8 COLLIDE (published 9f176442ecd010a8)
  seed 0x0000000000000000: h(m1) = ea7a79d69644ac08, h(m2) = ea7a79d69644ac08 COLLIDE
  seed 0x0000000000000001: h(m1) = 710cffbf21377195, h(m2) = 710cffbf21377195 COLLIDE
  first sampled seed 0x6c0f5cf23d5dcf82: h(m1) = h(m2) = 4a96e8cc7eb9e606; strip_seed -> 0000000000001337

pair B rebuilt from its recipe (solve_word2 with target 0x1337): matches the published hex, both hash to 0000000000001337

control: m1 of pair A vs m1 with the last bit flipped (a01109025ea76be0)
  random 64-bit seeds: N = 16777216 (2^24.00), collisions = 0, rate = 0, log2 rate = -inf (< -24.00)

result: ALL CHECKS PASSED
```

The two "first sampled seed" lines are the only lines that depend on the RNG seed (argv[2],
default fixed).  Pair A's line is also independent of N, but pair B's is not (its first
seed is drawn after 3N earlier draws), so it changes with the count too: with `2^26` the
four rate lines read `N = 67108864 (2^26.00), collisions = 67108864`, the control reads
`collisions = 0`, and pair B's first sampled seed differs.  Malformed counts (`2^` without
digits, `2^k` with k > 63, non-numeric text) and a non-numeric RNG seed are rejected with
exit status 2.

The source header carries the full MIT text with both copyright lines: the program's, and
Google's 2011 line for `city.cc`, of which the hash functions are a transcription.
