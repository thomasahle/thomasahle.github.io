# a5hash: seed-collision pairs, reader-runnable check

## The hash

[a5hash](https://github.com/avaneev/a5hash) v5.21 by Aleksey Vaneev (MIT), exactly as registered in
SMHasher3 (`hashes/a5hash.cpp`, port by Frank J. T. Wojcik): the 64-bit `a5hash` (SMHasher3 verification
value `0xADDE79B3`) and the 128-bit `a5hash_128` (`0x89406B11`).  `a5hash_verify.c` re-implements both from
that source, reproduces both verification values with SMHasher3's own procedure
(`lib/Hashinfo.cpp _ComputedVerifyImpl`: keys of length 0..255 with key *i* = bytes 0..*i*-1 and seed 256-*i*,
outputs concatenated little-endian, hashed once more with seed 0, first 4 bytes little-endian) and aborts
if either value differs.

Threat model: the attacker knows the message pair, the 64-bit seed is uniformly random and hidden.

## What the pairs exploit

a5hash mixes the seed into the state with a single product `(s1_0, s2_0) = umul128(K2^len ^ (seed & AA..), K1^len ^ (seed & 55..))`,
where the odd seed bits go to one operand and the even seed bits to the other.  Bit *j* of a product depends only
on bits 0..*j* of its operands, so `s1_0 = lo64(...)` is a very non-uniform function of the seed: a well-chosen
target value `T` has thousands to hundreds of thousands of preimage seeds instead of about one.  The program
enumerates a preimage class exactly (2-adic lifting: fix seed bits from bit 0 upward, prune any prefix whose low
product bits already differ from `T`), cross-checks the low-24-bit level against brute force, and then hashes
both messages under every seed in the class.

* **Pair 1 (worst, 64-bit, len 23, verifier-confirmed).**  Every message word is XORed into the state before a
  multiply, so when `s1_0(seed) == W0 = 0xf01bbe7047b3e000` (bytes 0..7 of the message) the first block's operand
  `W0 ^ s1_0` is zero, the 128-bit block product is zero, and the state becomes `(val01, val10)` regardless of
  bytes 8..15.  The pair differs in byte 8 only.  Exactly 156800 of the 2^64 seeds are in the class
  (`eps = 2^-46.74`, a lower bound on the true collision probability: the beam search that chose `T` is
  heuristic), all of them collide; with `L = 3` words that is `bits = log2(L/eps) = 48.3`.
  The record's hex lists 24 bytes; the hashed length is 23, and the trailing `00` is never read.
* **Pair 2 (64-bit, len 8, verifier-confirmed).**  Same seed-expansion weakness, aimed at the tail: if
  `s1_0(seed) == A = 0x1248299ed31fa942` (the tail word built from bytes 0..3 and 4..7), the tail operand
  `s1 ^ A` is zero, `umul128(0, s2) = (0,0)`, and the output is 0 whatever the other tail word is.  The
  message with byte 4 flipped gets `s1 = 1`, whose product has a zero high half and also hashes to 0.  The
  class is 7290 seeds with `s1_0 == A` plus 1 with `s1_0 == A^1`: 7291 seeds, `eps = 2^-51.17`, `bits = 51.2`.
* **Pair 3 (a5hash_128 only, len 25, key-free).**  The 128-bit variant never seeds `Seed3`/`Seed4`; on the
  17..32-byte path the last 16 bytes enter only through `umul128(c + K3, d + K4)` with public constants.
  The pair sets `(c+K3, d+K4) = (1, 1+2^24)` versus `(1+2^24, 1)`, so the product is identical and all 128
  output bits collide for every seed (probability 1; `bits = log2(4/1) = 2`).  The 64-bit `a5hash` of the same
  pair does not collide (printed for contrast).  This pair was not reviewed by the panel's independent
  verifier; the program checks it directly on 2^24 seeds.

Uniform random seeds cannot resolve rates of 2^-47 or 2^-51 (2^24 seeds expect ~1e-7 collisions), so the
random-seed lines for pairs 1 and 2 are a null check; the rates rest on the exact class enumeration.

## Build and run

    cc -O2 -std=c11 -o a5hash_verify a5hash_verify.c -lm
    ./a5hash_verify          # 2^24 random seeds per pair, about 0.5 s
    ./a5hash_verify 26       # 2^26 random seeds
    ./a5hash_verify 24 7     # second argument: RNG seed (default 0xA5A5A5A5C0FFEE00)

Single file, C11, no dependencies.  `unsigned __int128` is used for the 64x64->128 multiply when the compiler has
it; `-DA5_PORTABLE_MUL` forces the portable 32x32 fallback (tested, same output).  The random-seed stream is
fixed by the RNG seed (splitmix64-seeded xoshiro256**; pair p uses the RNG seed plus its message length), so the
output below is reproducible bit for bit, and another RNG seed changes only pair 3's `first random colliding
seed` line.  A non-numeric argument is rejected with exit status 1.

## Expected output (`./a5hash_verify`, Apple M2 Pro, clang, 0.4 s)

```
a5hash v5.21 (SMHasher3 hashes/a5hash.cpp; https://github.com/avaneev/a5hash) -- seed-collision pairs
validation: a5hash      SMHasher3 verification 0xADDE79B3 (expected 0xADDE79B3) OK
            a5hash_128  SMHasher3 verification 0x89406B11 (expected 0x89406B11) OK

== pair 1: a5hash (64-bit), len 23 -- weak-seed class, dead first block ==
mechanism: seeds with s1_0(seed) == W0 = 0xf01bbe7047b3e000 make the first block's operand W0 ^ s1_0 == 0,
  so the 128-bit block product is 0 and the state becomes (val01, val10) whatever W1 (bytes 8..15) is:
  the pair differs only in byte 8.  The class was enumerated exhaustively over all 2^64 seeds.
m  = 70be1bf000e0b34700000000000000000000000000000000
m2 = 70be1bf000e0b34701000000000000000000000000000000
(record lists 24 bytes; the hashed length is 23, the trailing 0x00 is not read)
uniform seeds: N = 2^24, collisions = 0/16777216, none observed in 2^24 trials; no population bound
seed class s1_0(seed) == 0xf01bbe7047b3e000: 156800 seeds (2-adic lift; 24-bit brute-force cross-check 384 == 384 OK)
conditional rate: 156800/156800 = 1.000000 colliding class members
class density:    156800/2^64 = 2^-46.74
=> collision probability over uniform seeds >= 2^-46.74 (this class alone); L = 3 words -> bits = log2(L/eps) = 48.3
   (2^24 uniform seeds expect 1.4e-07 collisions from this class, so 0 above is consistent)
explicit seed 0x016c190d78e85d64: h(m) = d6dfd54ea0f0a3a4  h(m2) = d6dfd54ea0f0a3a4  COLLIDE  (s1_0 = 0xf01bbe7047b3e000, class member)

== pair 2: a5hash (64-bit), len 8 -- weak-seed class, dead tail multiply (both hashes are 0) ==
mechanism: seeds with s1_0(seed) == A = 0x1248299ed31fa942 (or A^1) zero the tail operand s1 ^ A, so
  umul128(s1, s2) = (0, 0) and the output is 0 whatever the other word B is; the flipped byte 4 gives
  s1 = 1 for the other message, whose product then has hi == 0 and also hashes to 0.
m  = 9e29481242a91fd3
m2 = 9e29481243a91fd3
uniform seeds: N = 2^24, collisions = 0/16777216, none observed in 2^24 trials; no population bound
seed class s1_0(seed) == 0x1248299ed31fa942: 7290 seeds (2-adic lift; 24-bit brute-force cross-check 50 == 50 OK)
seed class s1_0(seed) == 0x1248299ed31fa943: 1 seeds (2-adic lift; 24-bit brute-force cross-check 1 == 1 OK)
conditional rate: 7291/7291 = 1.000000 colliding class members
class density:    7291/2^64 = 2^-51.17
=> collision probability over uniform seeds >= 2^-51.17 (this class alone); L = 1 words -> bits = log2(L/eps) = 51.2
   (2^24 uniform seeds expect 6.6e-09 collisions from this class, so 0 above is consistent)
explicit seed 0x7b9e6e3ec8b1997d: h(m) = 0000000000000000  h(m2) = 0000000000000000  COLLIDE  (s1_0 = 0x1248299ed31fa942, class member)

== pair 3: a5hash_128 (full 128-bit output), len 25 -- key-free, public-constant product ==
mechanism: on the 17..32-byte path the last 16 bytes (c, d) enter only through umul128(c + K3, d + K4) with the
  PUBLIC constants K3 = 0xA4093822299F31D0, K4 = 0xC0AC29B7C97C50DD (Seed3/Seed4 are never seeded).
  The pair sets (c+K3, d+K4) = (1, 1+2^24) vs (1+2^24, 1): equal 128-bit products for every seed.
m  = 000000000000000000ddc7f65b31ce60d648d6533f24af8337
m2 = 000000000000000000ddc7f65b31ce60d748d6533f24af8336
uniform seeds: N = 2^24, collisions = 16777216/16777216, rate = 1 (2^0.00)
seed class: unconditional (every seed collides); L = 4 words -> bits = log2(L/1) = 2.0
explicit seed 0xdeadbeefcafef00d: h(m) = 30fb5b4e29fc72cbd0077c500047f77e  h(m2) = 30fb5b4e29fc72cbd0077c500047f77e  COLLIDE
first random colliding seed 0xae59c8595aa56d69
(the 64-bit a5hash of the same pair under this seed differs: a65a86661b034da8 vs 5c9f43df87ecb269)

```

Exit status 0.  A non-zero exit means the implementation failed SMHasher3 validation (1), a pair record
is malformed (2), the lifting cross-check failed (3) or a published example seed did not collide (4).

## License

`a5hash_verify.c` is MIT (Copyright (c) 2026 Thomas Dybdahl Ahle).  The a5hash algorithm is Copyright (c) 2025
Aleksey Vaneev, MIT; the SMHasher3 port it was written from is Copyright (C) 2021-2025 Frank J. T. Wojcik, MIT.

## Selected page witnesses

The primary program above checks the historical 23-byte/8-byte classes and
25-byte all-seed pair. It does not re-enumerate the selected 6615-byte class.
`selected_pairs.c` checks the page’s 6615-byte and 17-byte literal witnesses,
including full outputs, against the same implementation and startup checks.
From the archive parent:

```sh
cc -O2 -std=c11 verify/a5hash/selected_pairs.c -lm -o a5-selected
./a5-selected
```

Expected outputs are `9cda28706bf2e550` for the 6615-byte pair at seed
`bdd730a20c451ba4`, and high:low `e70d0e48e169dd4c:754f558f576fdbdc` for
the 17-byte 128-bit pair at seed `5f642f87d5e23888`. The historical class
contribution `118 × 2^-45` gives a score cap of 47.81 at L = 827; the 17-byte
all-seed construction gives a cap of 1.59 at L = 3. This witness check does
not re-establish the former class count or prove the latter identity.
