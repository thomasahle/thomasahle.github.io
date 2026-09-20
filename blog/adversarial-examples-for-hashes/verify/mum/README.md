# mum: standalone fixed-pair reproduction

Paper pair G: public _mum(w,p0) terms are identical. Both complete pre-fix MUM v3 unroll variants are validated; all 2^20 sampled seeds must collide. This is not the later collision-prevention default.

## Build and run

```sh
cc -O2 -std=c11 -o mum_verify mum_verify.c -lm
./mum_verify 20
```

Arguments: `[log2 N (0..40), default 20] [RNG seed, default 1]`.
The program samples exactly N = 2^log2N seeds **per pair and variant**.
Everything is single-threaded. It requires a C11 compiler; exact 128-bit
products use the GCC/Clang `__uint128_t` extension where applicable.
It reads no external files and writes only stdout/stderr.

## Implementation and validation

Adapted from `paper_rows/collision_driver.cpp and rows.json (pair G); complete C transcription of sources/mum_mir.cpp` in the supplied workspace, credited in the C
header. Loads are explicit little-endian and work on either host byte order.
Algorithm notices are retained. NMHASH's 16-bit products use unsigned
32-bit intermediates to avoid signed integer-promotion overflow.

| variant | output bits | sampled seed bits | SMHasher3 verification | output encoding for verification |
|---|---:|---:|---|---|
| MUM v3 exact unroll3 | 64 | 64 | `0x8BD72B8C` | little-endian |
| MUM v3 exact unroll4 | 64 | 64 | `0x0AD998DF` | little-endian |

The SMHasher3 `_ComputedVerifyImpl` procedure hashes byte prefixes of lengths
0..255 with seeds 256..1, concatenates their encoded outputs, hashes that
array with seed 0, and reads the first four output bytes little-endian.
Thus the check exercises the complete long-input path as well as short inputs.
No seed fixup is applied. For fasthash32, both upstream 32-bit and SMHasher3
64-bit seed interfaces are tested; the verification inputs fit either width.

Every recorded witness below is **asserted against its expected output**
before sampling. A validation mismatch, wrong output, identical built-in
messages, or any counterexample to an every-seed claim makes the program fail.

## Pairs and mechanism

| case (output order) | input lengths, bytes | claim |
|---|---:|---|
| paper pair G / MUM v3 exact unroll3 | 8/8 | every seed |
| paper pair G / MUM v3 exact unroll4 | 8/8 | every seed |

Paper pair G: public _mum(w,p0) terms are identical. Both complete pre-fix MUM v3 unroll variants are validated; all 2^20 sampled seeds must collide. This is not the later collision-prevention default.

## Sampling and expected output

The RNG is xoshiro256**, initialized from four splitmix64 outputs with
default seed 1. Each case restarts the same stream, so counts across cases
are deliberately correlated. Seeds are truncated only for 32-bit APIs.
These are reproducible samples of the specified seed domain, not a fresh
exhaustive enumeration. Historical larger measurements remain in the article
and supplied records; this run does not replace them.

For rare paper fold differentials, zero at 2^20 is expected: the program
still verifies and prints a known colliding seed, explicitly outside the
sample count. It reports no probability/score estimate from a zero-hit run.
For every-seed cases it checks every trial. The sampled score uses
L = ceil(max(input lengths)/8).

The following output was built and run locally, with no network access;
`run_2p20.txt` contains the same output. Reference counts, in order:
`1048576 1048576`.

```text
SMHasher3 MUM v3 exact unroll3: 8BD72B8C expected 8BD72B8C PASS
SMHasher3 MUM v3 exact unroll4: 0AD998DF expected 0AD998DF PASS
structural checks PASS

paper pair G / MUM v3 exact unroll3
M (8 B) = 7954a998719b89b0
M' (8 B) = 2d69169b259feb08
recorded colliding seed 0000000000000000: H(M)=b341726e9ea37186 H(M')=b341726e9ea37186
collisions = 1048576 / 1048576; rate = 1; log2(rate) = 0.000000; sampled score = 0.000000
first sampled colliding seed b3f2af6d0fc710c5: H(M)=H(M')=022285467e5c10ca

paper pair G / MUM v3 exact unroll4
M (8 B) = 7954a998719b89b0
M' (8 B) = 2d69169b259feb08
recorded colliding seed 0000000000000000: H(M)=b341726e9ea37186 H(M')=b341726e9ea37186
collisions = 1048576 / 1048576; rate = 1; log2(rate) = 0.000000; sampled score = 0.000000
first sampled colliding seed b3f2af6d0fc710c5: H(M)=H(M')=022285467e5c10ca
```

## Attribution and licensing

The supplied claimant/verifier authors and paper driver are credited by path
above and in the source header. Algorithm authors and license notices are
preserved in the C file. Driver additions are Copyright (c) 2026 Thomas
Dybdahl Ahle, MIT; this does not relicense embedded algorithm code.
Pengyhash v0.3 carries GPLv3-or-later, NMHASH BSD 2-Clause, mx3 CC0,
and mir/fasthash/MUM/rapidhash MIT notices, as applicable.
