# komihash: a fixed message pair that collides for 91% of seeds

Reader-runnable companion to the blog post. One C11 file, no dependencies.

## What the hash is

komihash by Aleksey Vaneev, version 5.34, https://github.com/avaneev/komihash
(MIT). 64-bit output; the seed is a 64-bit value that is split by bit parity
into the two state words S1 and S5 and passed through one multiply round
before any message byte is touched.

`komihash_pair.c` embeds the reference header `komihash.h` from release 5.34
verbatim (between the two marker lines near line 270 and at the end of the
file). To check it against upstream:

    awk '/^\/\* >>>> komihash.h 5.34 verbatim begins/{f=1;next} /^\/\* <<<< komihash.h 5.34 verbatim ends/{f=0} f' komihash_pair.c | shasum -a 256
    # ca1a1b40a24ee97d48cca1ac92617d105838483bf71198a6aa3a431ff165e768
    # = sha256 of https://raw.githubusercontent.com/avaneev/komihash/5.34/komihash.h

Validation at startup (the program aborts with exit code 1 if either fails):

* the SMHasher3 verification value of the variant `komihash` (SMHasher3
  `hashes/komihash.cpp`, v5.27 core): 0x8157FF6D, computed by the procedure of
  `lib/Hashinfo.cpp` `_ComputedVerifyImpl` (keys {}, {0}, {0,1}, ..., {0..254}
  hashed with seed 256-i, the 2048 little-endian output bytes hashed with seed
  0, first 4 bytes little-endian). The embedded 5.34 code reproduces it, so
  the two releases agree as functions on this test.
* four vectors from the upstream README (`testvec.c` construction): the 32-byte
  and 16-byte test strings and the 64- and 256-byte incrementing buffers.

## What the pairs exploit

**Pair 1 (unconditional, rate 0.9106).** Messages of 64 to 127 bytes pass
once through the 8-lane loop, where lane i multiplies (S_i ^ m_{i-1}) by
(S_{i+4} ^ m_{i+3}) and the lane seeds are the two secret words with public
constants XORed on: S2 = S1 ^ IVAL2, S6 = S5 ^ IVAL6, and so on. Choosing
m1 = m0 ^ IVAL2 and m5 = m4 ^ IVAL6 makes lanes 1 and 2 compute the same
128-bit product for every seed, so their low halves cancel in the XOR fold
that collapses the state after the loop, and only the high halves, added to
S5 and to S5 ^ IVAL6, remain. Flipping bit 0 of m0 and m1 (bytes 0 and 8)
changes both products identically: the folded state is unchanged whenever the
flip does not carry into the high half (probability 0.64, with m4 tuned to the
bit bias of S5 after the init round), and when it does carry, whenever the
same +-1 flips the same bits of S5 + h and (S5 ^ IVAL6) + h, which IVAL6's two
trailing zero bits make a 3/4 event: 0.64 + 0.36 * 0.75 = 0.9106. Any common
suffix keeps the rate for lengths 64..127; a second loop iteration (128 bytes
and up) destroys it. The panel measured 0.910588 over 2^32 seeds.

**Pair 2 (weak seed, different mechanism).** Messages of 8 to 15 bytes end in
a single multiply r1h * r2h with r1h = S1 ^ (first 8 bytes) and
r2h = S5 ^ (tail word). A seed whose S5 after the init round equals the tail
word gives r2h = 0, a zero product, and an output that ignores the first 8
bytes, so every 15-byte message with that tail collides. The panel's
exhaustive solver found exactly one such seed for this tail, 0xa41b7e7e02118bba
(class density 2^-64, i.e. a curiosity, not an attack). The program checks the
class membership, the dead multiply and the collision under that seed, and
samples 2^16 random prefixes with the same tail; it does not re-run the
exhaustive count.

## Build and run

    cc -O2 -std=c11 -o komihash_pair komihash_pair.c -lm
    ./komihash_pair                # 2^24 seeds, 0.3-0.7 s on an Apple M2 Pro
    ./komihash_pair 26             # 2^26 seeds (1.2 s)
    ./komihash_pair 24 deadbeef    # another RNG seed (hex)

The RNG is splitmix64 seeding xoshiro256** with a fixed default seed, so the
default output below is reproducible byte for byte.  A non-numeric log2 or a
non-hexadecimal RNG seed is rejected (FATAL, exit status 1). Compiled clean with
`clang -std=c11 -Wall -Wextra -pedantic` (Apple clang 17). Single-threaded.

## Expected output

`./komihash_pair` (2^24 seeds, default RNG seed):

```
komihash 5.34 (reference code embedded verbatim), 64-bit output; collision = full 64-bit equality
validation: SMHasher3 verification value 0x8157FF6D (expected 0x8157FF6D) OK
validation: upstream README test vectors 4/4 OK

[pair 1] lane-tie pair, 64 bytes, unconditional (uniform 64-bit seed)
  m  = 0000000000000000447370032e8a191311111111111111112222222222222222f0f9dda4c1c0a15e9cf534900ea6f5e033333333333333334444444444444444
  m' = 0100000000000000457370032e8a191311111111111111112222222222222222f0f9dda4c1c0a15e9cf534900ea6f5e033333333333333334444444444444444
  bytes that differ: 0 8
  N = 2^24 seeds (rng seed 0xc0ffee)
  collisions: 15276614/16777216 = 0.910557 (log2 -0.1352, s.e. 0.000070)
  lane-1 high half unchanged (no carry): 10778220/16777216 = 0.642432 (log2 -0.6384, s.e. 0.000117)
    collisions given no carry (predicted 1): 10778220/10778220 = 1.000000 (log2 0.0000, s.e. 0.000000)
    collisions given a carry (predicted 3/4): 4498394/5998996 = 0.749858 (log2 -0.4153, s.e. 0.000177)
  published seed 0x27d1f77dc2a01269: h(m) = 0x113c6b88bc913857 h(m') = 0x113c6b88bc913857 COLLIDE (record: 0x113c6b88bc913857)
  first colliding seed in this run: 0x120e99a6dde4a550 -> h = 0xa2825cfe6afdd018
  same pair as the first block of longer messages (suffix bytes 0x99, 2^16 seeds each):
    len  63:     0/65536 (0.0000) len  64: 59695/65536 (0.9109) len  65: 59701/65536 (0.9110)
    len  96: 59691/65536 (0.9108) len 127: 59675/65536 (0.9106) len 128:     0/65536 (0.0000)

[pair 2] weak-seed pair, 15 bytes, conditional on S5 == 0x01efb1a780a4e2df after the init round
  m  = 8877665544332211dfe2a480a7b1ef
  m' = 1122334455667788dfe2a480a7b1ef
  bytes that differ: 0 1 2 3 4 5 6 7
  N = 2^24 uniform seeds
  unconditional collisions: 0/16777216 = 0.000000 (< 2^-24.0 at this sample size)
  seed class {S5 == 0x01efb1a780a4e2df}: exactly 1 seed in 2^64, density 2^-64
    (exhaustive count by the panel's solver, not re-run here; unconditional rate is therefore 2^-64)
  class member 0xa41b7e7e02118bba: S5 after init = 0x01efb1a780a4e2df OK, tail word = 0x01efb1a780a4e2df, r2h = S5 ^ tail = 0x0 (dead multiply)
  under that seed: h(m) = 0x722cdb7c77771040 h(m') = 0x722cdb7c77771040 COLLIDE (record: 0x722cdb7c77771040)
  conditional rate over the class: 1/1 = 1.000000 (log2 0)
  random 8-byte prefixes with the same 7-byte tail under that seed: 65536/65536 hash to 0x722cdb7c77771040
```

Other runs on the same machine:

* `./komihash_pair 26`: pair 1 collisions 61109358/67108864 = 0.910600
  (log2 -0.1351); no carry 43110987/43110987 collide, carry 17998371/23997877
  = 0.749998; pair 2 unconditional 0/67108864.
* `./komihash_pair 24 deadbeef`: pair 1 15277295/16777216 = 0.910598; first
  colliding seed 0xc5555444a74d7e83 -> h = 0x1750583f8aa2ab25.

## License

The demo code (everything above the verbatim marker) is MIT, Copyright (c)
2026 Thomas Dybdahl Ahle. The embedded `komihash.h` is MIT, Copyright (c)
2021-2026 Aleksey Vaneev; its license notice is kept inside the file.
