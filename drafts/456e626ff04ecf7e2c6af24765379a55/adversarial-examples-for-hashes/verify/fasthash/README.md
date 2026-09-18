# fasthash: standalone fixed-pair reproduction

The unversioned fast-hash code is pinned to upstream commit 08a25db2 (2018-10-22). SMHasher3 verification is 0xA16231A7 at 64 bits and 0xE9481AFC at 32 bits. Its wrappers pass a 64-bit seed to both widths; upstream fasthash32 takes a 32-bit seed. The every-seed identity covers both models, which the reproduction program measures separately.

NEW: the supplied checked record identifies no prior literature for these fixed-pair, hidden-seed collisions.

## Build and run

```sh
cc -O2 -std=c11 -o fasthash_verify fasthash_verify.c -lm
./fasthash_verify 20
```

Arguments: `[log2 N (0..40), default 20] [RNG seed, default 1]`.
The program samples exactly N = 2^log2N seeds **per pair and variant**.
Everything is single-threaded. It requires a C11 compiler; exact 128-bit
products use the GCC/Clang `__uint128_t` extension where applicable.
It reads no external files and writes only stdout/stderr.

## Implementation and validation

Adapted from `heur2_scratch/verify-fasthash/vf.c and mine/vfh.c` in the supplied workspace, credited in the C
header. Loads are explicit little-endian and work on either host byte order.
Algorithm notices are retained. NMHASH's 16-bit products use unsigned
32-bit intermediates to avoid signed integer-promotion overflow.

| variant | output bits | sampled seed bits | SMHasher3 verification | output encoding for verification |
|---|---:|---:|---|---|
| fasthash-64 | 64 | 64 | `0xA16231A7` | little-endian |
| fasthash-32 SMHasher3 64-bit seed | 32 | 64 | `0xE9481AFC` | little-endian |
| fasthash-32 upstream 32-bit seed | 32 | 32 | `0xE9481AFC` | little-endian |

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
| 7 vs 8 bytes / fasthash-64 | 7/8 | every seed |
| 7 vs 8 bytes / fasthash-32 SMHasher3 64-bit seed | 7/8 | every seed |
| 7 vs 8 bytes / fasthash-32 upstream 32-bit seed | 7/8 | every seed |
| 16-byte top-bit pair / fasthash-64 | 16/16 | every seed |
| 16-byte top-bit pair / fasthash-32 SMHasher3 64-bit seed | 16/16 | every seed |
| 16-byte top-bit pair / fasthash-32 upstream 32-bit seed | 16/16 | every seed |

The seed enters on line 4, but every message word first passes through the public bijection mix. Seven zero bytes and an eight-byte word both take one message step. Choosing w = mix⁻¹(7m XOR 8m) = 0xf375d1d1e77b175e cancels their length terms before multiplication. Both states entering line 7 are identical for every seed; the 32-bit fold on line 9 preserves equality. For equal lengths, two words with mix difference 2^63 cancel across the odd multiplier m, giving a separately verified 16-byte pair.

The independent verifier measured 1,073,741,824/1,073,741,824 collisions at both output widths for the selected seven-byte-versus-eight-byte pair. The one-step identity proves ε = 1, L = 1, score 0. The equal-length 16-byte pair was independently checked on 4,294,967,296 seeds and has score 1. Equal-length one-word full 64-bit collisions are impossible because that path is bijective; truncation to 32 bits is a different question.

No generic scan was run after reaching the metric floor. The shorter-total-length one-byte-versus-eight-byte pair is claimant-only and is not used for scoring.

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
`1048576 1048576 1048576 1048576 1048576 1048576`.

```text
SMHasher3 fasthash-64: A16231A7 expected A16231A7 PASS
SMHasher3 fasthash-32 SMHasher3 64-bit seed: E9481AFC expected E9481AFC PASS
SMHasher3 fasthash-32 upstream 32-bit seed: E9481AFC expected E9481AFC PASS
structural checks PASS

7 vs 8 bytes / fasthash-64
M (7 B) = 00000000000000
M' (8 B) = 5e177be7d1d175f3
recorded colliding seed 0123456789abcdef: H(M)=3f624b9140899669 H(M')=3f624b9140899669
collisions = 1048576 / 1048576; rate = 1; log2(rate) = 0.000000; sampled score = 0.000000
first sampled colliding seed b3f2af6d0fc710c5: H(M)=H(M')=0ce33c766543c3a8

7 vs 8 bytes / fasthash-32 SMHasher3 64-bit seed
M (7 B) = 00000000000000
M' (8 B) = 5e177be7d1d175f3
recorded colliding seed 0123456789abcdef: H(M)=01274ad8 H(M')=01274ad8
collisions = 1048576 / 1048576; rate = 1; log2(rate) = 0.000000; sampled score = 0.000000
first sampled colliding seed b3f2af6d0fc710c5: H(M)=H(M')=58608732

7 vs 8 bytes / fasthash-32 upstream 32-bit seed
M (7 B) = 00000000000000
M' (8 B) = 5e177be7d1d175f3
recorded colliding seed 0000000000000000: H(M)=33f6e94b H(M')=33f6e94b
collisions = 1048576 / 1048576; rate = 1; log2(rate) = 0.000000; sampled score = 0.000000
first sampled colliding seed 000000000fc710c5: H(M)=H(M')=ff25f634

16-byte top-bit pair / fasthash-64
M (16 B) = 00000000000000000000000000000000
M' (16 B) = f69e1c7bcf4db16ff69e1c7bcf4db16f
recorded colliding seed 0123456789abcdef: H(M)=1ac53b33e0d7dd39 H(M')=1ac53b33e0d7dd39
collisions = 1048576 / 1048576; rate = 1; log2(rate) = 0.000000; sampled score = 1.000000
first sampled colliding seed b3f2af6d0fc710c5: H(M)=H(M')=3b3d32ec00108e61

16-byte top-bit pair / fasthash-32 SMHasher3 64-bit seed
M (16 B) = 00000000000000000000000000000000
M' (16 B) = f69e1c7bcf4db16ff69e1c7bcf4db16f
recorded colliding seed 0123456789abcdef: H(M)=c612a206 H(M')=c612a206
collisions = 1048576 / 1048576; rate = 1; log2(rate) = 0.000000; sampled score = 1.000000
first sampled colliding seed b3f2af6d0fc710c5: H(M)=H(M')=c4d35b75

16-byte top-bit pair / fasthash-32 upstream 32-bit seed
M (16 B) = 00000000000000000000000000000000
M' (16 B) = f69e1c7bcf4db16ff69e1c7bcf4db16f
recorded colliding seed 0000000000000000: H(M)=b5f0e529 H(M')=b5f0e529
collisions = 1048576 / 1048576; rate = 1; log2(rate) = 0.000000; sampled score = 1.000000
first sampled colliding seed 000000000fc710c5: H(M)=H(M')=3e9578f6
```

## Attribution and licensing

The supplied claimant/verifier authors and paper driver are credited by path
above and in the source header. Algorithm authors and license notices are
preserved in the C file. Driver additions are Copyright (c) 2026 Thomas
Dybdahl Ahle, MIT; this does not relicense embedded algorithm code.
Pengyhash v0.3 carries GPLv3-or-later, NMHASH BSD 2-Clause, mx3 CC0,
and mir/fasthash/MUM/rapidhash MIT notices, as applicable.
