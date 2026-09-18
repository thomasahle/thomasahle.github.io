# MurmurHash3_x64_128: key-free collisions

Reader-runnable reproduction of two message pairs that collide in
MurmurHash3_x64_128 under **every** seed.

## The hash

MurmurHash3 by Austin Appleby (final version, 2012; public domain),
`MurmurHash3_x64_128` in
<https://github.com/aappleby/smhasher/blob/master/src/MurmurHash3.cpp>.
The program implements the exact SMHasher3 variant `MurmurHash3_128`
(<https://gitlab.com/fwojcik/smhasher3>, `hashes/murmurhash3.cpp`): two 64-bit
lanes seeded as `h1 = h2 = (uint32_t)seed`.  The reference API takes a
`uint32_t` seed and SMHasher3 truncates its 64-bit `seed_t` to 32 bits, so the
secret is 32 bits and "a uniformly random seed" means uniform over 2^32 values.

Validation at startup: the program recomputes SMHasher3's verification value
with SMHasher3's own procedure (`lib/Hashinfo.cpp`, `_ComputedVerifyImpl`:
hash the keys `{}`, `{0}`, `{0,1}`, ..., `{0..254}` with seed `256-i`,
concatenate the 256 outputs, hash that with seed 0, first 4 bytes
little-endian) and aborts unless it equals `0x6384BA69`, the value SMHasher3
registers for `MurmurHash3_128` (the original SMHasher lists the same value
for "Murmur3F").

## What the pairs exploit

The seed only sets the initial state `(h1, h2)`; every message word enters
through a public, seed-independent bijection (`g1(k) = rotl(k*c1,31)*c2`,
`g2(k) = rotl(k*c2,33)*c1`) that is XORed into a lane, after which the lane is
rotated, added to the other lane and mapped by `x -> 5x + c`.  Because
`x ^ 2^63 == x + 2^63`, a difference of exactly `2^63` in a lane is preserved by
every one of those operations, and the rotations `rotl 27` / `rotl 31` carry a
chosen low bit (`2^36` in `h1`, `2^32` in `h2`) into bit 63.  So the attacker
picks word differences (four multiplications by the inverse constants) that
put `(2^63, 0)` into the state after block 1 for every seed and cancel it with
the 8-byte tail; the lengths are equal, so the finalization sees identical
input.  This is the top-bit trick of Aumasson, Bernstein and Bosslet
(29C3, 2012) for MurmurHash2/3; the 24-byte pair is one block plus a tail,
`L = 3` words, hence `bits = log2(L / eps) = log2(3 / 1) = 1.585`.  The 32-byte
pair is the two-block form, and because its state difference returns to
`(0, 0)`, any concatenation of such block pairs collides too (2^n messages from
n block pairs, the hash-flooding multicollision).

Pairs (hex bytes, same length in each pair; the base words are arbitrary):

    24 bytes  m  = 000000000000000000000000000000000000000000000000
              m' = 60a0fd219e0ef2cd42e098d38ee8728d000000007d4dce82
    32 bytes  m  = 0000000000000000000000000000000000000000000000000000000000000000
              m' = 60a0fd219e0ef2cd000000000000000060a0fd2121c1234b000000c01f8b7776

Both were confirmed by an independent implementation over all 2^32 seeds
(4294967296 / 4294967296 collisions each).  The program samples 2^24 random
seeds by default; `--exhaustive` re-runs the full 2^32 enumeration
(one to a few minutes per pair, single-threaded and machine-dependent).

## Build and run

    cc -std=c11 -O2 -o murmurhash3_128_verify murmurhash3_128_verify.c -lm
    ./murmurhash3_128_verify                 # 2^24 random seeds, rng seed 1
    ./murmurhash3_128_verify 26 7            # 2^26 seeds, rng seed 7
    ./murmurhash3_128_verify --exhaustive    # additionally all 2^32 seeds

Single C11 file, no dependencies (`-lm` for `log2`), about 1.2 s for the
default run.  For each pair it prints the hex messages, a trace of the state
XOR-difference after each block and the tail at two seeds, the collision rate
over N random seeds next to a control pair (`m'` with its last bit flipped,
which must not collide), the published example seed 0 with both hash values,
and the first sampled colliding seed with both hash values; finally the 4-way
multicollision from pair 2, sampled on min(N, 2^20) seeds (which is why the
default run reports 1048576/1048576 there).  Hash values are printed as the
16 output bytes in SMHasher3 order (`h1` little-endian, then `h2`).  A
non-numeric argument or a misspelled flag is rejected with exit status 2.

## Expected output

    $ ./murmurhash3_128_verify
    MurmurHash3_x64_128 (SMHasher3 "MurmurHash3_128", seed truncated to 32 bits)
    SMHasher3 verification value: computed 0x6384BA69, expected 0x6384BA69 -> OK
    Sampling N = 2^24 uniformly random 32-bit seeds (xoshiro256**, rng seed 1)
    
    == Pair 1: 24-byte pair (one block + 8-byte tail): confirmed worst pair, L = 3 words, bits = log2(3/1) = 1.585 ==
      length 24 bytes each, condition on the seed: none (key-free)
      m  = 000000000000000000000000000000000000000000000000
      m' = 60a0fd219e0ef2cd42e098d38ee8728d000000007d4dce82
      mechanism: block 1: g1(k1') = g1(k1) ^ 2^36 and g2(k2') = g2(k2) ^ 2^32 give state difference (2^63, 0) deterministically (rotl 27 / rotl 31 move the bits to the top; += and *5+c preserve 2^63); 8-byte tail k3 with g1(k3') = g1(k3) ^ 2^63 cancels h1; no k2 tail so h2 untouched
      trace at seed 0x00000000 (state difference h1^h1', h2^h2'):
        block 1: g1 diff 0000001000000000  g2 diff 0000000100000000  -> state diff (8000000000000000, 0000000000000000)
        tail   : g1 diff 8000000000000000                            -> state diff (0000000000000000, 0000000000000000)
        same length, same state -> same finalization -> same hash
      trace at seed 0x9e3779b9 (state difference h1^h1', h2^h2'):
        block 1: g1 diff 0000001000000000  g2 diff 0000000100000000  -> state diff (8000000000000000, 0000000000000000)
        tail   : g1 diff 8000000000000000                            -> state diff (0000000000000000, 0000000000000000)
        same length, same state -> same finalization -> same hash
      random seeds : collisions/N = 16777216/16777216 = 1.000000, rate 2^0.0000
      control pair : collisions/N = 0/16777216, rate unestimated (zero observed in 2^24 trials)  (m' with last bit flipped)
      published example seed 0x00000000:
        H(m)  = 46328447a2022b04964f2f614c774125
        H(m') = 46328447a2022b04964f2f614c774125
        equal
      first sampled colliding seed 0xb3f2af6d:
        H(m)  = bd2dcb051acc710a23874cecfe4f47df
        H(m') = bd2dcb051acc710a23874cecfe4f47df
        equal
    
    == Pair 2: 32-byte pair (two blocks): the 2012-style two-block universal collision, L = 4 words, bits = 2 ==
      length 32 bytes each, condition on the seed: none (key-free)
      m  = 0000000000000000000000000000000000000000000000000000000000000000
      m' = 60a0fd219e0ef2cd000000000000000060a0fd2121c1234b000000c01f8b7776
      mechanism: block 1: g1 diff 2^36 -> (2^63, 2^63); block 2: g1 diff 2^63|2^36, g2 diff 2^63 -> (0, 0); n such block pairs give 2^n messages colliding for every seed (hash-flooding multicollision)
      trace at seed 0x00000000 (state difference h1^h1', h2^h2'):
        block 1: g1 diff 0000001000000000  g2 diff 0000000000000000  -> state diff (8000000000000000, 8000000000000000)
        block 2: g1 diff 8000001000000000  g2 diff 8000000000000000  -> state diff (0000000000000000, 0000000000000000)
        same length, same state -> same finalization -> same hash
      trace at seed 0x9e3779b9 (state difference h1^h1', h2^h2'):
        block 1: g1 diff 0000001000000000  g2 diff 0000000000000000  -> state diff (8000000000000000, 8000000000000000)
        block 2: g1 diff 8000001000000000  g2 diff 8000000000000000  -> state diff (0000000000000000, 0000000000000000)
        same length, same state -> same finalization -> same hash
      random seeds : collisions/N = 16777216/16777216 = 1.000000, rate 2^0.0000
      control pair : collisions/N = 0/16777216, rate unestimated (zero observed in 2^24 trials)  (m' with last bit flipped)
      published example seed 0x00000000:
        H(m)  = d05cde4a283c72313c2d34b31b1dc010
        H(m') = d05cde4a283c72313c2d34b31b1dc010
        equal
      first sampled colliding seed 0x1a28690d:
        H(m)  = 8a7b010b8bed1073e911a028f82cd0c2
        H(m') = 8a7b010b8bed1073e911a028f82cd0c2
        equal
    
    == Multicollision: the four 64-byte messages {m,m'} x {m,m'} of pair 2 ==
      seeds where all four hashes agree: 1048576/1048576
      example seed 0x0afee077:
        H(AA) = 43ff2fce6ca96bd127f1381812f90117
        H(AB) = 43ff2fce6ca96bd127f1381812f90117
        H(BA) = 43ff2fce6ca96bd127f1381812f90117
        H(BB) = 43ff2fce6ca96bd127f1381812f90117
    
    Done.

## Notes

* The x86_32 and x86_128 variants of MurmurHash3 fall to the same trick with
  8-byte and 32-byte pairs respectively (different SMHasher3 hashes,
  `MurmurHash3_32` and `MurmurHash3_128__int32`); they are not included here.
* Different `rng_seed` values change only the "first sampled colliding seed"
  lines; the rates are 1 for both pairs and 0 for the controls regardless.

## License

The program is MIT-licensed (Copyright (c) 2026 Thomas Dybdahl Ahle).
MurmurHash3 is public domain (Austin Appleby); the implementation here is
written from the reference source.
