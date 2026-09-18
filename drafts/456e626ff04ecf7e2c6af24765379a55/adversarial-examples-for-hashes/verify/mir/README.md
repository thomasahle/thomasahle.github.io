# mir: standalone fixed-pair reproduction

mir has no algorithm version number; its arithmetic is unchanged since April 2019. SMHasher3 mir.exact (0x00A393C8) and mir.inexact (0x422A66FC) are output-equivalent to upstream mir_hash and mir_hash_strict on little-endian hosts. The strict fold drops one cross-product carry; it does not drop every carry. The 64-bit seed is passed verbatim, including seed+len = 0; SMHasher3’s inexact seed-fixup is a harness option, not the API model here. The chart uses the exact variant’s timing; inexact is 48.62 cycles/hash and 1.33 B/cycle.

EXTENSION of the paper’s MUM result. mir is MUM-derived and uses the same public-term failure mechanism. The supplied record finds no earlier mir_hash-specific literature; that does not make the underlying MUM mechanism new. The VMUM/MUM-V3 issue and later collision-prevention default concern other functions.

## Build and run

```sh
cc -O2 -std=c11 -o mir_verify mir_verify.c -lm
./mir_verify 20
```

Arguments: `[log2 N (0..40), default 20] [RNG seed, default 1]`.
The program samples exactly N = 2^log2N seeds **per pair and variant**.
Everything is single-threaded. It requires a C11 compiler; exact 128-bit
products use the GCC/Clang `__uint128_t` extension where applicable.
It reads no external files and writes only stdout/stderr.

## Implementation and validation

Adapted from `heur2_scratch/verify-mir/fable_71582/fmir.h and fderive.py; 16-byte case from verify-mir/README.md` in the supplied workspace, credited in the C
header. Loads are explicit little-endian and work on either host byte order.
Algorithm notices are retained. NMHASH's 16-bit products use unsigned
32-bit intermediates to avoid signed integer-promotion overflow.

| variant | output bits | sampled seed bits | SMHasher3 verification | output encoding for verification |
|---|---:|---:|---|---|
| mir.exact | 64 | 64 | `0x00A393C8` | little-endian |
| mir.inexact | 64 | 64 | `0x422A66FC` | little-endian |

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
| 8-byte p1 pair / mir.exact | 8/8 | every seed |
| 8-byte p1 pair / mir.inexact | 8/8 | every seed |
| 16-byte p2 pair / mir.exact | 16/16 | every seed |
| 16-byte p2 pair / mir.inexact | 16/16 | every seed |

For eight-byte messages, the only message term is the public mum(w,p1) on line 11. The second word is p1⁻¹ mod (2^64−1) = 0x3cc02a2b092b150b. Its exact product halves sum to 2^64, hence the folded term is zero, just as for w = 0. Direct evaluation of lines 4–7 also gives zero for this word in the inexact variant. The states after line 11 are therefore equal for every seed in both variants. This extends the paper’s MUM public-term collision to the MUM-derived mir function.

The independent verifier measured 1,073,741,824/1,073,741,824 collisions per variant, with checks against both SMHasher3 verification values. Public-term equality proves ε = 1. The eight-byte pair has L = 1 and score 0. The independently verified 16-byte pair at the p2 site also collides for every seed and has score 1; it is included in the program.

* Not verified (claimant only): the 24-byte variant and the seven-byte-versus-eight-byte half-seed pair. Neither is used in the table, chart or score. No generic scan was run after reaching the metric floor.

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
`1048576 1048576 1048576 1048576`.

```text
SMHasher3 mir.exact: 00A393C8 expected 00A393C8 PASS
SMHasher3 mir.inexact: 422A66FC expected 422A66FC PASS
structural checks PASS

8-byte p1 pair / mir.exact
M (8 B) = 0000000000000000
M' (8 B) = 0b152b092b2ac03c
recorded colliding seed 910a2dec89025cc1: H(M)=5e900c9f273619d2 H(M')=5e900c9f273619d2
collisions = 1048576 / 1048576; rate = 1; log2(rate) = 0.000000; sampled score = 0.000000
first sampled colliding seed b3f2af6d0fc710c5: H(M)=H(M')=044d95a6bb73224d

8-byte p1 pair / mir.inexact
M (8 B) = 0000000000000000
M' (8 B) = 0b152b092b2ac03c
recorded colliding seed 910a2dec89025cc1: H(M)=5e900c9f273619d2 H(M')=5e900c9f273619d2
collisions = 1048576 / 1048576; rate = 1; log2(rate) = 0.000000; sampled score = 0.000000
first sampled colliding seed b3f2af6d0fc710c5: H(M)=H(M')=044d95a6bb73224d

16-byte p2 pair / mir.exact
M (16 B) = 00000000000000005555555555555555
M' (16 B) = 0000000000000000aaaaaaaaaaaaaaaa
recorded colliding seed 22118258a9d111a0: H(M)=4d337930c595fdd0 H(M')=4d337930c595fdd0
collisions = 1048576 / 1048576; rate = 1; log2(rate) = 0.000000; sampled score = 1.000000
first sampled colliding seed b3f2af6d0fc710c5: H(M)=H(M')=e8b1651bb48f268a

16-byte p2 pair / mir.inexact
M (16 B) = 00000000000000005555555555555555
M' (16 B) = 0000000000000000aaaaaaaaaaaaaaaa
recorded colliding seed 22118258a9d111a0: H(M)=66c4631e3e47d736 H(M')=66c4631e3e47d736
collisions = 1048576 / 1048576; rate = 1; log2(rate) = 0.000000; sampled score = 1.000000
first sampled colliding seed b3f2af6d0fc710c5: H(M)=H(M')=2f3484b13a2ca6fc
```

## Attribution and licensing

The supplied claimant/verifier authors and paper driver are credited by path
above and in the source header. Algorithm authors and license notices are
preserved in the C file. Driver additions are Copyright (c) 2026 Thomas
Dybdahl Ahle, MIT; this does not relicense embedded algorithm code.
Pengyhash v0.3 carries GPLv3-or-later, NMHASH BSD 2-Clause, mx3 CC0,
and mir/fasthash/MUM/rapidhash MIT notices, as applicable.
