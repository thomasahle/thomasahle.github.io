# SpookyHash V2 (64-bit): a collision pair that works for approximately half of sampled seeds

## The hash

SpookyHash V2 by Bob Jenkins (2012, public domain), <https://www.burtleburtle.net/bob/hash/spooky.html>.
The variant reproduced here is SMHasher3's `SpookyHash2_64`
(<https://gitlab.com/fwojcik/smhasher3>, `hashes/spookyhash.cpp`): the 128-bit `Hash128` is seeded with
`h1 = h2 = seed` and the 64-bit result is `h1`.  Messages of 192 bytes or more go through the long path:
96-byte blocks are absorbed by `Mix` into a 12-word state, then V2's `End` adds the zero-padded last block
(with the remainder length in its final byte) word-wise and runs three `EndPartial` rounds.

`spookyhash2_64_pair.c` contains a from-the-reference C11 implementation of the whole hash (short and long
paths) and validates it at startup by recomputing the SMHasher3 verification values (keys `{}`, `{0}`,
`{0,1}`, ... up to 255 bytes with seed `256 - i`, the little-endian outputs concatenated and hashed with
seed 0, first four bytes little-endian): `SpookyHash2_64 = 0x972C4BDC`, and as extra checks
`SpookyHash2_32 = 0xA48BE265` and `SpookyHash2_128 = 0x893CFCBE`.  It aborts if any of them differs.

## What the pair exploits

Step 11 of `Mix` is `s11 += w11; s1 ^= s9; s10 ^= s11; s11 = rotl(s11, 46); s10 += s0`, and it is the
last thing that happens to the state before `End` adds the final block's words in.  Adding 2^63 to word 11
of the second (last full) Mix block flips only the top bit of `s11`; the xor into `s10` then adds exactly
2^63 to `s10`, and the rotate by 46 turns the top-bit difference in `s11` into an additive difference of
+2^45 or -2^45, with the sign given by bit 63 of `s11` just before the rotate.  With a 286-byte message
(2 Mix blocks + a 94-byte tail) the tail is added word-wise by `End`, so End word 10 += 2^63 cancels the
`s10` difference and End word 11 -= 2^45 cancels the `s11` difference whenever the sign was +, which is
the case for approximately half of the seeds (measured; exact balance unproved); the 768-bit state is then identical, so the 64-bit and the 128-bit
outputs both collide.  Pair 2 uses End word 11 += 2^45 instead and collides on the complementary half, so
the program also checks that for every seed exactly one of the two pairs collides and that the bit-63
predictor is right every time.

Both pairs are 286 bytes and differ in three bytes: `[191] 26->a6` (Mix block 2, word 11, bit 63),
`[279] ea->6a` (End word 10, bit 63) and `[285] 34->14` (pair 1) or `[285] 14->34` (pair 2) (End word 11,
bit 45).  Published colliding seeds: pair 1 at seed 4 (both hash to `e3f63e3f415763ac`), pair 2 at seed 0
(both hash to `f6c014070dfbbadc`).

## Build and run

    cc -O2 -std=c11 -o spookyhash2_64_pair spookyhash2_64_pair.c -lm
    ./spookyhash2_64_pair                # 2^24 uniform seeds, h1 = h2 = seed (SMHasher3's seeding), ~4 s
    ./spookyhash2_64_pair 26             # 2^26 seeds (~17 s on one Apple M2 Pro core)
    ./spookyhash2_64_pair 24 1 1         # independent uniform (h1, h2): the native 128-bit seed

Arguments: `[log2 of the number of seeds = 24] [RNG seed = 1] [seed mode = 0 | 1]`.  The RNG is the
program's own splitmix64-seeded xoshiro256**.  Single-threaded, no dependencies beyond the C standard library.

## Expected output (`./spookyhash2_64_pair`, default 2^24 seeds)

```
SMHasher3 verification: SpookyHash2_64 0x972C4BDC (expect 0x972C4BDC), SpookyHash2_32 0xA48BE265 (expect 0xA48BE265), SpookyHash2_128 0x893CFCBE (expect 0x893CFCBE)
verification OK
pair 1: End word 11 -= 2^45, collides iff bit 63 of s11 (Mix block 2, before the step-11 rotate) is 0
  M1 (286 bytes) = a0ed1dcaeb2e421c17cf8516645510b61c1e80eb014c077fdbb3b3ab867298f2ccfb12c325d96dd09c3de65c247a0257a6d22a9da89710cfb724db191c748806e9d6b0272f125818a2c061b9e406999b788261dcd2487f7064e1bf4d00670a3d09230d92b37de4141a5e5d12e205c37e42c13193ab064833476cb9e5a22bd74c85073313646062ca36b300b4a62fa6797864ca181b97739bf4dd0567f5ce9a24398e0ddb544ee4926c6c033f6397e4790f51641c3d6dc04d5dcfe95ffe3054262ff9e5d8a8f17cce6b3c565acbecc693b77d90b1564fd1ddc71df2c56f2fd52fd6d8c9fb3b84612e5f83397bb5ac932651ce382aa90592ebb81d61c29ac53c2bace0711020c947c1ce55f2cfd79f821e92ba90562ba05eeab1b8f0771534
  M2 (286 bytes) = a0ed1dcaeb2e421c17cf8516645510b61c1e80eb014c077fdbb3b3ab867298f2ccfb12c325d96dd09c3de65c247a0257a6d22a9da89710cfb724db191c748806e9d6b0272f125818a2c061b9e406999b788261dcd2487f7064e1bf4d00670a3d09230d92b37de4141a5e5d12e205c37e42c13193ab064833476cb9e5a22bd74c85073313646062ca36b300b4a62fa6797864ca181b97739bf4dd0567f5ce9a24398e0ddb544ee4926c6c033f6397e4790f51641c3d6dc04d5dcfe95ffe3054a62ff9e5d8a8f17cce6b3c565acbecc693b77d90b1564fd1ddc71df2c56f2fd52fd6d8c9fb3b84612e5f83397bb5ac932651ce382aa90592ebb81d61c29ac53c2bace0711020c947c1ce55f2cfd79f821e92ba90562ba05e6ab1b8f0771514
  differing bytes: [191] 26->a6 [279] ea->6a [285] 34->14
pair 2: End word 11 += 2^45, collides iff that bit is 1 (the complementary state-bit class)
  M1 (286 bytes) = a0ed1dcaeb2e421c17cf8516645510b61c1e80eb014c077fdbb3b3ab867298f2ccfb12c325d96dd09c3de65c247a0257a6d22a9da89710cfb724db191c748806e9d6b0272f125818a2c061b9e406999b788261dcd2487f7064e1bf4d00670a3d09230d92b37de4141a5e5d12e205c37e42c13193ab064833476cb9e5a22bd74c85073313646062ca36b300b4a62fa6797864ca181b97739bf4dd0567f5ce9a24398e0ddb544ee4926c6c033f6397e4790f51641c3d6dc04d5dcfe95ffe3054262ff9e5d8a8f17cce6b3c565acbecc693b77d90b1564fd1ddc71df2c56f2fd52fd6d8c9fb3b84612e5f83397bb5ac932651ce382aa90592ebb81d61c29ac53c2bace0711020c947c1ce55f2cfd79f821e92ba90562ba05eeab1b8f0771514
  M2 (286 bytes) = a0ed1dcaeb2e421c17cf8516645510b61c1e80eb014c077fdbb3b3ab867298f2ccfb12c325d96dd09c3de65c247a0257a6d22a9da89710cfb724db191c748806e9d6b0272f125818a2c061b9e406999b788261dcd2487f7064e1bf4d00670a3d09230d92b37de4141a5e5d12e205c37e42c13193ab064833476cb9e5a22bd74c85073313646062ca36b300b4a62fa6797864ca181b97739bf4dd0567f5ce9a24398e0ddb544ee4926c6c033f6397e4790f51641c3d6dc04d5dcfe95ffe3054a62ff9e5d8a8f17cce6b3c565acbecc693b77d90b1564fd1ddc71df2c56f2fd52fd6d8c9fb3b84612e5f83397bb5ac932651ce382aa90592ebb81d61c29ac53c2bace0711020c947c1ce55f2cfd79f821e92ba90562ba05e6ab1b8f0771534
  differing bytes: [191] 26->a6 [279] ea->6a [285] 14->34
explicit colliding seeds (h1 = h2 = seed):
  pair 1, seed 0x0000000000000004: SpookyHash2_64(M1) = e3f63e3f415763ac, SpookyHash2_64(M2) = e3f63e3f415763ac  COLLISION  (128-bit: e3f63e3f415763ac324f320615c4e120 / e3f63e3f415763ac324f320615c4e120  collision)
  pair 2, seed 0x0000000000000000: SpookyHash2_64(M1) = f6c014070dfbbadc, SpookyHash2_64(M2) = f6c014070dfbbadc  COLLISION  (128-bit: f6c014070dfbbadcdcf3045f4f09c242 / f6c014070dfbbadcdcf3045f4f09c242  collision)
random seeds: N = 2^24, seed mode 0 (h1 = h2 = uniform 64-bit seed), rng seed 1
  pair 1: 64-bit collisions 8387160 / 16777216 = 0.499914 = 2^-1.0002   (128-bit collisions 8387160, 2^-1.0002)
  pair 2: 64-bit collisions 8390056 / 16777216 = 0.500086 = 2^-0.9998   (128-bit collisions 8390056, 2^-0.9998)
  mechanism: exactly one of the two pairs collides for 16777216 / 16777216 seeds (both: 0, neither: 0);
             the bit-63 predictor is right for 16777216 / 16777216 seeds
  pair 1: first colliding seed 0x853b559647364cea, both hash to fc4393dd2b72f7eb
  pair 2: first colliding seed 0xb3f2af6d0fc710c5, both hash to 4e746ff68ea195ec
```

(the blank lines of the real output are omitted above; `run_2^24.log` is the verbatim run.)

Other runs in this directory: `run_2^26.log` (2^26 seeds: pair 1 33552409 / 67108864 = 2^-1.0001,
pair 2 33556455 / 67108864 = 2^-0.9999, predictor right for all 67108864) and `run_2^24_mode1.log`
(independent 128-bit seeds: pair 1 2^-0.9996, pair 2 2^-1.0004, predictor right for all 16777216).
The panel's own runs at 2^30 seeds gave 536868916 and 536880211 collisions for pair 1 (2^-1.0000).

## License

`spookyhash2_64_pair.c` and this README are MIT-licensed (Copyright (c) 2026 Thomas Dybdahl Ahle).
SpookyHash V2 itself is public domain (Bob Jenkins); the implementation here is a rewrite from the
reference, not a copy.


## Integration pass 2: selected 275-byte pair and 32-bit output

The program also includes the verifier-confirmed 275-byte pair from
`experiment/verify-spooky32/pair275/` (credited in the C header).
It validates the recorded seed 0 outputs and non-colliding seed 3, prints both
literal inputs, and samples the 32-, 64- and full 128-bit outputs on a fresh
copy of the same RNG stream. L=35 gives an estimated score cap of 6.13.
The earlier 286-byte pairs remain available and keep their original counts.

The same fixed-pair state collision survives 32-bit truncation at the same
measured rate; occasional extra 32-bit-only chance collisions are possible.
Mode 0 is the scored SMHasher3 model: a uniform 64-bit seed duplicated into
both seed words. Native Hash32 takes a 32-bit seed. The exact one-half
population balance is not proved. Complementary predictor classes need not
be balanced. Historical independent-seed-word and 2^32 claimant runs were
not independently reproduced by the supplied verifier.

Expected integration run: see `run_2p20_pass2.txt`, produced by
`./spookyhash2_64_pair 20`. The package README lists all eight parsed counts.
