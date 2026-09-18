# nmhash32: standalone fixed-pair reproduction

NMHASH32 v2 uses a 32-bit seed and returns 32 bits. Verification is 0x12A30553. The last algorithmic change was 2021-06-08; the 2021-06-09 commit changed the version label, and the December 2024 undefined-behaviour fix preserved verification values. The suite timing is the AVX2 implementation; the reproduction is scalar.

NEW: no prior literature is identified in the supplied checked record. Statistical failures in SMHasher3 and rurban are recorded separately from this fixed same-seed pair.

## Build and run

```sh
cc -O2 -std=c11 -o nmhash32_verify nmhash32_verify.c -lm
./nmhash32_verify 20
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
| nmhash32 v2 | 32 | 32 | `0x12A30553` | little-endian |

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
| selected pair / nmhash32 v2 | 64/64 | sampled rate; recorded witness asserted |

The changed lane is j = 0. XORing 0x80400000 into both words on lines 13–14 leaves the addition on line 7 unchanged precisely when bit 22 of seed + 64 is set. The difference passes through the lane multiplications and XOR shifts on lines 8–10; the low 16-bit C3 multiplication additionally requires no carry across bit 13. The resulting x difference is 0x80202808 and the retained y difference is 0x80400000. The fresh words on line 16 cancel them, leaving the entire final round and fold equal.

The claimant and an independent scalar verifier each enumerated all 2^32 seeds and counted 1,078,944,392 collisions. Thus ε for this fixed pair is exactly 1078944392/2^32 ≈ 0.251211; L = 8 gives score ≤ log2(8·2^32/1078944392) ≈ 4.993. The verifier’s separate sampled run found 269,753,620/1,073,741,823. The package’s smaller sample is reported separately and does not replace that exhaustive count.

* Not verified (claimant only): the proposed exclusion of L ≤ 7, the 8-byte fold histogram, the 24-byte null search (0/2^32; resolution 2^-32), and the 1,251-cell generic scan at 2^26 seeds per cell (resolution about 2^-26). No optimality claim is made.

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
`262968`.

```text
SMHasher3 nmhash32 v2: 12A30553 expected 12A30553 PASS
SMHasher3 checks complete; recorded output assertions follow

selected pair / nmhash32 v2
M (64 B) = 00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
M' (64 B) = 00004080000000000000000000000000000040800000000000000000000000000828208000000000000000000000000000004080000000000000000000000000
recorded colliding seed 00000000b54cda26: H(M)=e00f99e0 H(M')=e00f99e0
collisions = 262968 / 1048576; rate = 0.250785827637; log2(rate) = -1.995472; sampled score = 4.995472
first sampled colliding seed 00000000c266a3a7: H(M)=H(M')=750a4444
```

## Attribution and licensing

The supplied claimant/verifier authors and paper driver are credited by path
above and in the source header. Algorithm authors and license notices are
preserved in the C file. Driver additions are Copyright (c) 2026 Thomas
Dybdahl Ahle, MIT; this does not relicense embedded algorithm code.
Pengyhash v0.3 carries GPLv3-or-later, NMHASH BSD 2-Clause, mx3 CC0,
and mir/fasthash/MUM/rapidhash MIT notices, as applicable.
