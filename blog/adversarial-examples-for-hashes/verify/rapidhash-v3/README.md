# rapidhash-v3: standalone fixed-pair reproduction

Paper pairs A (32 B) and D (48 B), in standard, micro and nano v3. The supplied C++ implementation is specialized to C with unprotected multiplication, little-endian reads and the unrolled standard path. All three complete implementations, including their long paths, pass their own SMHasher3 value. Protected variants and the 16-byte null case are not scored or inferred here. Two key models: `default` (uniformly sampled 64-bit API seed, shipped eight-word `rapid_secret`; the historical experiment and the article's default-secret control) and `random-secret` (uniform seed and eight uniform secret words per trial through the `rapidhash_internal`-equivalent bodies, 576 hidden bits; the model the article scores, added 2026-09-19).

## Build and run

```sh
cc -O2 -std=c11 -o rapidhash_v3_verify rapidhash_v3_verify.c -lm
./rapidhash_v3_verify 20
./rapidhash_v3_verify 20 1 random-secret   # random-secret model: seed + eight secret words per trial
./rapidhash_v3_verify 30 1 random-secret   # 2^30 keys per case, about 4 min
```

Arguments: `[log2 N (0..40), default 20] [RNG seed, default 1] [default|random-secret]`.
The program samples exactly N = 2^log2N seeds **per pair and variant**.
Everything is single-threaded. It requires a C11 compiler; exact 128-bit
products use the GCC/Clang `__uint128_t` extension where applicable.
It reads no external files and writes only stdout/stderr.

## Implementation and validation

Adapted from `paper_rows/rapid_v3_port.h (validated verbatim SMHasher3 bodies) and collision_driver.cpp` in the supplied workspace, credited in the C
header. Loads are explicit little-endian and work on either host byte order.
Algorithm notices are retained. NMHASH's 16-bit products use unsigned
32-bit intermediates to avoid signed integer-promotion overflow.

| variant | output bits | sampled seed bits | SMHasher3 verification | output encoding for verification |
|---|---:|---:|---|---|
| rapidhash v3 | 64 | 64 | `0x1FDC65EE` | little-endian |
| rapidhash v3 micro | 64 | 64 | `0x6F183D61` | little-endian |
| rapidhash v3 nano | 64 | 64 | `0x2C200DC7` | little-endian |

The SMHasher3 `_ComputedVerifyImpl` procedure hashes byte prefixes of lengths
0..255 with seeds 256..1, concatenates their encoded outputs, hashes that
array with seed 0, and reads the first four output bytes little-endian.
Thus the check exercises the complete long-input path as well as short inputs.
No seed fixup is applied. For fasthash32, both upstream 32-bit and SMHasher3
64-bit seed interfaces are tested; the verification inputs fit either width.

Every recorded witness below is **asserted against its expected output**
before sampling. In `random-secret` mode the program additionally asserts the recorded
random-model witness for the three 32-byte cases: seed `27d3b5addafed424` with secret words
`8201394795b91ef9, bc80d672c2c377f6, 5afd557e26c19903, a3a9fab4fc0d80e2, 5108fe3feb9bd088,
6742ee43cec628a4, 2267c78c10996237, d96b61ed51f62b33` hashes both messages to `d832bac31b8fda6c`
(from the 2026-09-19 verification against the unmodified upstream tag,
`../../records/witness-searches/random-secret/rapid3/verify/logs/10_pairA_random_2p30.txt`). A validation mismatch, wrong output, identical built-in
messages, or any counterexample to an every-seed claim makes the program fail.

## Pairs and mechanism

| case (output order) | input lengths, bytes | claim |
|---|---:|---|
| rapid3_32 / rapidhash v3 | 32/32 | sampled rate; recorded witness asserted |
| rapid3_48 / rapidhash v3 | 48/48 | sampled rate; recorded witness asserted |
| rapid3_micro_32 / rapidhash v3 micro | 32/32 | sampled rate; recorded witness asserted |
| rapid3_micro_48 / rapidhash v3 micro | 48/48 | sampled rate; recorded witness asserted |
| rapid3_nano_32 / rapidhash v3 nano | 32/32 | sampled rate; recorded witness asserted |
| rapid3_nano_48 / rapidhash v3 nano | 48/48 | sampled rate; recorded witness asserted |

Paper pairs A (32 B) and D (48 B), in standard, micro and nano v3. The supplied C++ implementation is specialized to C with unprotected multiplication, little-endian reads and the unrolled standard path. All three complete implementations, including their long paths, pass their own SMHasher3 value. Protected variants and the 16-byte null case are not scored or inferred here.

## Sampling and expected output

The RNG is xoshiro256**, initialized from four splitmix64 outputs with
default seed 1. Each case restarts the same stream, so counts across cases
are deliberately correlated (in `random-secret` mode each trial draws the seed and then the
eight secret words from that stream, so all six cases see the same keys). Seeds are truncated only for 32-bit APIs.
These are reproducible samples of the specified seed domain, not a fresh
exhaustive enumeration. Historical larger measurements remain in the article
and supplied records; this run does not replace them.

For rare paper fold differentials, zero at 2^20 is expected: the program
still verifies and prints a known colliding seed, explicitly outside the
sample count. It reports no probability/score estimate from a zero-hit run.
For every-seed cases it checks every trial. The sampled score uses
L = ceil(max(input lengths)/8).

The article's score for rapidhash v3 uses the random-secret model: pooled **2386 / 2^37.83 =
2^-26.61**, cap 28.6 bits [28.5, 28.7] (search harness and independent verifier, the latter on the
unmodified upstream tag `rapidhash_v3`), recorded in `../../records/witness-searches/random-secret/rapid3/`.
The fold differential never uses the secret value, so the shipped secret gives the same rate
(pooled 539 / 2^35.64); the shipped secret additionally admits the every-seed annihilation
(the word at len-16 equal to secret[1] ^ len), which the random-secret model excludes.

The following output was built and run on the Xeon (2026-09-19, `nice -n 10 taskset -c 24-31`);
`run_2p20.txt` contains the same output. Reference counts of the default mode, in order:
`0 0 0 0 0 0`. The random-secret smoke run `./rapidhash_v3_verify 20 1 random-secret` prints
`0 0 0 0 0 0` (`run_2p20_random_secret.txt`); the 2^30 run printed **12 / 2^30 = 2^-26.415** for
every case (`run_2p30_random_secret.txt`; the six cases share the key stream and the same first-fold
event, so their counts coincide), consistent with the pooled 2^-26.61 above.

```text
SMHasher3 rapidhash v3: 1FDC65EE expected 1FDC65EE PASS
SMHasher3 rapidhash v3 micro: 6F183D61 expected 6F183D61 PASS
SMHasher3 rapidhash v3 nano: 2C200DC7 expected 2C200DC7 PASS
SMHasher3 checks complete; recorded output assertions follow
key model: uniform 64-bit API seed; shipped public secret words (default)

rapid3_32 / rapidhash v3
M (32 B) = 9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c
M' (32 B) = 642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c
recorded colliding seed 3187ae8a8617e034: H(M)=a7ee6375a78a86f0 H(M')=a7ee6375a78a86f0
collisions = 0 / 1048576; rate = 0; no rate/score estimate from zero hits (resolution 1/N)
no sampled collision; the recorded witness above was checked separately

rapid3_48 / rapidhash v3
M (48 B) = 9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c48c651edae76208e840fc51f1cccbb02
M' (48 B) = 642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c48c651edae76208e840fc51f1cccbb02
recorded colliding seed 3187ae8a8617e034: H(M)=b52b5b5759f1d08a H(M')=b52b5b5759f1d08a
collisions = 0 / 1048576; rate = 0; no rate/score estimate from zero hits (resolution 1/N)
no sampled collision; the recorded witness above was checked separately

rapid3_micro_32 / rapidhash v3 micro
M (32 B) = 9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c
M' (32 B) = 642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c
recorded colliding seed 3187ae8a8617e034: H(M)=a7ee6375a78a86f0 H(M')=a7ee6375a78a86f0
collisions = 0 / 1048576; rate = 0; no rate/score estimate from zero hits (resolution 1/N)
no sampled collision; the recorded witness above was checked separately

rapid3_micro_48 / rapidhash v3 micro
M (48 B) = 9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c48c651edae76208e840fc51f1cccbb02
M' (48 B) = 642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c48c651edae76208e840fc51f1cccbb02
recorded colliding seed 3187ae8a8617e034: H(M)=b52b5b5759f1d08a H(M')=b52b5b5759f1d08a
collisions = 0 / 1048576; rate = 0; no rate/score estimate from zero hits (resolution 1/N)
no sampled collision; the recorded witness above was checked separately

rapid3_nano_32 / rapidhash v3 nano
M (32 B) = 9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c
M' (32 B) = 642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c
recorded colliding seed 3187ae8a8617e034: H(M)=a7ee6375a78a86f0 H(M')=a7ee6375a78a86f0
collisions = 0 / 1048576; rate = 0; no rate/score estimate from zero hits (resolution 1/N)
no sampled collision; the recorded witness above was checked separately

rapid3_nano_48 / rapidhash v3 nano
M (48 B) = 9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c48c651edae76208e840fc51f1cccbb02
M' (48 B) = 642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c48c651edae76208e840fc51f1cccbb02
recorded colliding seed 3187ae8a8617e034: H(M)=b52b5b5759f1d08a H(M')=b52b5b5759f1d08a
collisions = 0 / 1048576; rate = 0; no rate/score estimate from zero hits (resolution 1/N)
no sampled collision; the recorded witness above was checked separately
```

## Attribution and licensing

The supplied claimant/verifier authors and paper driver are credited by path
above and in the source header. Algorithm authors and license notices are
preserved in the C file. Driver additions are Copyright (c) 2026 Thomas
Dybdahl Ahle, MIT; this does not relicense embedded algorithm code.
Pengyhash v0.3 carries GPLv3-or-later, NMHASH BSD 2-Clause, mx3 CC0,
and mir/fasthash/MUM/rapidhash MIT notices, as applicable.
