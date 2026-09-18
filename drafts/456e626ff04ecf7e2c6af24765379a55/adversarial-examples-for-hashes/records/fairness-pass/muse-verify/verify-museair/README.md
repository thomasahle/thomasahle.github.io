# MuseAir v0.3: a key-free collision pair (every seed, all four variants)

**Hash.** MuseAir v0.3 by K--Aethiax, as shipped in SMHasher3 as `hashes/museair.cpp`
(upstream: <https://github.com/eternal-io/museair>, CC0 1.0). SMHasher3 registers four
variants, `MuseAir` (64-bit), `MuseAir__bfast`, `MuseAir_128` and `MuseAir_128__bfast`;
the secret is the 64-bit seed, passed through unchanged. The program `museair_verify.c`
is a from-the-reference C11 re-implementation of all four (MIT; no dependencies beyond
libc/libm) and exits with status 1 unless it reproduces all four SMHasher3 verification values
(`0xF89F1683`, `0xC61BEE56`, `0xD3DFE238`, `0x27939BF1`; procedure from
`lib/Hashinfo.cpp:_ComputedVerifyImpl`).

**What the pair exploits.** For messages of 17..32 bytes the short path reads the first
16 bytes bijectively into two words `(i, j)` and then XORs onto the same `(i, j)` a
contribution from bytes 16..len-1 that is computed from *public constants only*:
`P(u,v) = (lo(C2·(C3^u)) ^ hi(C4·(C5^v)), lo(C4·(C5^v)) ^ hi(C2·(C3^u)))`. The seed only
touches `(i, j)` afterwards. So for any two tails `T != T'`, replacing the head by
`head ^ (P(T) ^ P(T'))` gives two distinct messages with identical `(i, j)` for every
seed, hence identical output in all four variants with probability 1. Nothing about
the seed needs to be known, and a class of 2^(8·len-128) messages collides pairwise.
The 17-byte and 24-byte pairs both have L = ceil(len/8) = 3 words. Each gives
a score cap log2(3) = 1.5849625..., displayed upward as <= 1.59 bits.

Upstream has since moved to "MuseAir v2", whose short path multiplies the tail with a
seed-dependent operand; the pair here targets v0.3, the version in SMHasher3.

**Pairs** (hex bytes, copied exactly from the record).

| len | M | M2 |
|---|---|---|
| 17 | `0000000000000000000000000000000000` | `8079763bb19a00001a9a1100642d3a3f01` |
| 24 | `000000000000000000000000000000000000000000000000` | `7cd5c18245c15e8ef47bfef8b79181a80101010101010101` |

**Build and run** (about 2 s at the default 2^24 seeds; 2^26 takes about 7 s):

```
cc -O2 -std=c11 -o museair_verify museair_verify.c -lm
./museair_verify            # 2^24 uniformly random seeds
./museair_verify 26         # 2^26 seeds
./museair_verify 24 0xBEEF  # different xoshiro256** RNG seed
```

The program (1) validates the implementation, (2) prints both pairs and re-derives the
head of M2 from the public tail function, (3) hashes both messages under N uniformly
random 64-bit seeds (own splitmix64-seeded xoshiro256**) for each of the four variants
and prints collisions/N with the log2 rate, (4) prints the published example seed
`0x2cb0f69f4abea221` and the first seed of the sweep with both hash values, and (5) runs
a control (the same tail change without the head fix-up), which does not collide.
Hash values are printed as the output bytes in order (little-endian); the record quotes
the 64-bit variants' outputs as integers, and the program shows both.  `MuseAir_128` and
`MuseAir_128__bfast` print identical hashes for these messages because the short path
(len <= 32) is the same for both 128-bit variants; they differ only on the bulk path,
which is why their verification values are checked separately.  A non-numeric argument
is rejected with exit status 2.

**Expected output** (`./museair_verify`, this machine, clang -O2):

```
== MuseAir v0.3 (SMHasher3 hashes/museair.cpp): verification values ==
  MuseAir              computed 0xF89F1683 expected 0xF89F1683 PASS
  MuseAir__bfast       computed 0xC61BEE56 expected 0xC61BEE56 PASS
  MuseAir_128          computed 0xD3DFE238 expected 0xD3DFE238 PASS
  MuseAir_128__bfast   computed 0x27939BF1 expected 0x27939BF1 PASS

== 17-byte pair (L = ceil(17/8) = 3 words, score cap <= 1.59 bits) ==
  M  = 0000000000000000000000000000000000
  M2 = 8079763bb19a00001a9a1100642d3a3f01
  head of M2 derived from the public tail function P(u,v): 8079763bb19a00001a9a1100642d3a3f  -> matches M2

== 24-byte pair (L = 3 words, score cap <= 1.59 bits) ==
  M  = 000000000000000000000000000000000000000000000000
  M2 = 7cd5c18245c15e8ef47bfef8b79181a80101010101010101
  head of M2 derived from the public tail function P(u,v): 7cd5c18245c15e8ef47bfef8b79181a8  -> matches M2

== 16777216 (2^24) uniformly random 64-bit seeds (xoshiro256**, seed 0x243f6a8885a308d3) ==
  17-byte pair:
    MuseAir              collisions 16777216 / 16777216 = 1.000000  (log2 rate = 0.0000)
    MuseAir__bfast       collisions 16777216 / 16777216 = 1.000000  (log2 rate = 0.0000)
    MuseAir_128          collisions 16777216 / 16777216 = 1.000000  (log2 rate = 0.0000)
    MuseAir_128__bfast   collisions 16777216 / 16777216 = 1.000000  (log2 rate = 0.0000)
  24-byte pair:
    MuseAir              collisions 16777216 / 16777216 = 1.000000  (log2 rate = 0.0000)
    MuseAir__bfast       collisions 16777216 / 16777216 = 1.000000  (log2 rate = 0.0000)
    MuseAir_128          collisions 16777216 / 16777216 = 1.000000  (log2 rate = 0.0000)
    MuseAir_128__bfast   collisions 16777216 / 16777216 = 1.000000  (log2 rate = 0.0000)

== explicit colliding seeds ==
  17-byte pair, seed 0x2cb0f69f4abea221 (published example):
    MuseAir              H(M) = e49a52cc7e41edd4  H(M2) = e49a52cc7e41edd4  EQUAL
      as u64 0xd4ed417ecc529ae4, published 0xd4ed417ecc529ae4: reproduced
    MuseAir__bfast       H(M) = 203ee3a245ae4376  H(M2) = 203ee3a245ae4376  EQUAL
    MuseAir_128          H(M) = cda876715e747c91cea410999db59615  H(M2) = cda876715e747c91cea410999db59615  EQUAL
    MuseAir_128__bfast   H(M) = cda876715e747c91cea410999db59615  H(M2) = cda876715e747c91cea410999db59615  EQUAL
  17-byte pair, seed 0x05c9c0954e168e82 (first seed of the sweep):
    MuseAir              H(M) = 5f1132023517769b  H(M2) = 5f1132023517769b  EQUAL
    MuseAir__bfast       H(M) = 443d4e0615bcab6f  H(M2) = 443d4e0615bcab6f  EQUAL
    MuseAir_128          H(M) = 07043a1c1c6c5312f03efe78da3a0600  H(M2) = 07043a1c1c6c5312f03efe78da3a0600  EQUAL
    MuseAir_128__bfast   H(M) = 07043a1c1c6c5312f03efe78da3a0600  H(M2) = 07043a1c1c6c5312f03efe78da3a0600  EQUAL
  24-byte pair, seed 0x2cb0f69f4abea221 (published example):
    MuseAir              H(M) = b9eef195606febb7  H(M2) = b9eef195606febb7  EQUAL
      as u64 0xb7eb6f6095f1eeb9, published 0xb7eb6f6095f1eeb9: reproduced
    MuseAir__bfast       H(M) = d50cc608288c3a8f  H(M2) = d50cc608288c3a8f  EQUAL
    MuseAir_128          H(M) = d39057701da0f11d77dbe4e654673a1e  H(M2) = d39057701da0f11d77dbe4e654673a1e  EQUAL
    MuseAir_128__bfast   H(M) = d39057701da0f11d77dbe4e654673a1e  H(M2) = d39057701da0f11d77dbe4e654673a1e  EQUAL
  24-byte pair, seed 0x05c9c0954e168e82 (first seed of the sweep):
    MuseAir              H(M) = 9a5b59e3041c0aee  H(M2) = 9a5b59e3041c0aee  EQUAL
    MuseAir__bfast       H(M) = 4be4ae20860686fa  H(M2) = 4be4ae20860686fa  EQUAL
    MuseAir_128          H(M) = 91f2d0773304fb6c634fdc4a2cd4a231  H(M2) = 91f2d0773304fb6c634fdc4a2cd4a231  EQUAL
    MuseAir_128__bfast   H(M) = 91f2d0773304fb6c634fdc4a2cd4a231  H(M2) = 91f2d0773304fb6c634fdc4a2cd4a231  EQUAL

== control: M vs (head of M | tail of M2), 17 bytes, MuseAir ==
  collisions 0 / 65536 = 0.000000  (none observed in 2^16 trials; no population bound)
```

**License.** `museair_verify.c` and this README are MIT (Copyright (c) 2026 Thomas Dybdahl
Ahle; the full text is in the source header).  MuseAir itself is CC0 1.0.

## Additional v0.3 witnesses and exact BFast class count

From the archive parent:

```sh
cc -O2 -std=c11 verify/museair/additional_pairs.c -lm -o museair-additional
./museair-additional
python3 verify/museair/count_class.py
```

The C supplement checks all four startup verification values, reproduces C–E
at their explicit seeds, and samples 2^20 seeds for D and 2^20 class members
for E. See `additional_pairs.log` for full outputs. It does not enumerate
all 2^64 seeds. The Python carry-state dynamic program counts the lane-swap
equation exactly: 8796093022208 = 2^43 solutions, all satisfying the 21-bit
mask. Since that mask also admits 2^43 seeds, the two classes are equal.
See `count_class.log`. This exact class contributes 2^-21 through the
source-derived lane-swap identity; collisions outside it are not excluded.
These additions concern deprecated algorithm v0.3 and do not test v2.
