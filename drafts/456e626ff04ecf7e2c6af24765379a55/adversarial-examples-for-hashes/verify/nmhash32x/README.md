# nmhash32x: standalone fixed-pair reproduction

NMHASH32X v2 is a separate function from NMHASH32, with 32×32 short-key multiplication, a 32-bit seed, 32-bit output and verification 0xA8580227. It has its own row and witness. Its statistical failures and AVX2 timing must not be merged with NMHASH32’s.

NEW: the supplied checked record identifies no prior literature for this deterministic short-path trail.

## Build and run

```sh
cc -O2 -std=c11 -o nmhash32x_verify nmhash32x_verify.c -lm
./nmhash32x_verify 20
```

Arguments: `[log2 N (0..40), default 20] [RNG seed, default 1]`.
The program samples exactly N = 2^log2N seeds **per pair and variant**.
Everything is single-threaded. It requires a C11 compiler; exact 128-bit
products use the GCC/Clang `__uint128_t` extension where applicable.
It reads no external files and writes only stdout/stderr.

## Implementation and validation

Adapted from `heur2_scratch/verify-nmhash32/own_x28/own_nmhash.h; NMHASH32 trail also independently checked in verify-nmhash32/nm.h` in the supplied workspace, credited in the C
header. Loads are explicit little-endian and work on either host byte order.
Algorithm notices are retained. NMHASH's 16-bit products use unsigned
32-bit intermediates to avoid signed integer-promotion overflow.

| variant | output bits | sampled seed bits | SMHasher3 verification | output encoding for verification |
|---|---:|---:|---|---|
| nmhash32x v2 | 32 | 32 | `0xA8580227` | little-endian |

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
| selected pair / nmhash32x v2 | 28/28 | every seed |

The first two words differ by Δ = 0x08008008. Their changes cancel in x ^= y on line 4, then re-enter on line 6 as R(Δ,4) = 0x80080080. The next XOR shift maps that to exactly 0x80000000. Multiplication by odd C3 preserves this top-bit XOR difference for every value, and line 7 leaves x differing by 0x80080000. The tail words on line 14 cancel x and y before the remaining fold; the independent a,b stream is unchanged. The trail therefore holds for every seed.

The independent verifier re-derived the trail, sampled 2^20 and 2^30 seeds without a counterexample, and exhaustively counted 4,294,967,296/4,294,967,296 collisions. With 28 bytes, L = 4 and ε = 1 give score ≤ 2. Confirmed 32- and 33-byte variants are retained in the source record; the 28-byte pair is selected here.

* Not verified (claimant only): the proposed L ≤ 3 impossibility argument and the 1,251-cell generic scan at 2^26 seeds per cell (resolution about 2^-26). The score is a witnessed cap, not a proof of optimality.

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
`1048576`.

```text
SMHasher3 nmhash32x v2: A8580227 expected A8580227 PASS
SMHasher3 checks complete; recorded output assertions follow

selected pair / nmhash32x v2
M (28 B) = 00000000000000000000000000000000000000000000000000000000
M' (28 B) = 08800008088000080000000000000000000000000000088080000880
recorded colliding seed 0000000000000000: H(M)=dc5952ce H(M')=dc5952ce
collisions = 1048576 / 1048576; rate = 1; log2(rate) = 0.000000; sampled score = 2.000000
first sampled colliding seed 000000000fc710c5: H(M)=H(M')=15cd32a0
```

## Attribution and licensing

The supplied claimant/verifier authors and paper driver are credited by path
above and in the source header. Algorithm authors and license notices are
preserved in the C file. Driver additions are Copyright (c) 2026 Thomas
Dybdahl Ahle, MIT; this does not relicense embedded algorithm code.
Pengyhash v0.3 carries GPLv3-or-later, NMHASH BSD 2-Clause, mx3 CC0,
and mir/fasthash/MUM/rapidhash MIT notices, as applicable.
