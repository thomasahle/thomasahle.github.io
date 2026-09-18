# mx3: standalone fixed-pair reproduction

This is mx3 v3.0.0, tag 48924ee7, with a 64-bit seed. rurban’s mx3 entry tests v1.0.0 (0x4DB51E5B) with a 32-bit seed cast to 64 bits; its GOOD label and older bad-seed result do not describe the attacked v3.

NEW: the supplied checked record identifies no prior literature. The known seed-equals-length zero-hash case concerns v1/v2. The v3 pair here does not depend on learning or choosing the seed.

## Build and run

```sh
cc -O2 -std=c11 -o mx3_verify mx3_verify.c -lm
./mx3_verify 20
```

Arguments: `[log2 N (0..40), default 20] [RNG seed, default 1]`.
The program samples exactly N = 2^log2N seeds **per pair and variant**.
Everything is single-threaded. It requires a C11 compiler; exact 128-bit
products use the GCC/Clang `__uint128_t` extension where applicable.
It reads no external files and writes only stdout/stderr.

## Implementation and validation

Adapted from `experiment/verify-mx3/mymx3.h and construct.c` in the supplied workspace, credited in the C
header. Loads are explicit little-endian and work on either host byte order.
Algorithm notices are retained. NMHASH's 16-bit products use unsigned
32-bit intermediates to avoid signed integer-promotion overflow.

| variant | output bits | sampled seed bits | SMHasher3 verification | output encoding for verification |
|---|---:|---:|---|---|
| mx3 v3 | 64 | 64 | `0x7B287B65` | little-endian |

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
| 1 vs 8 bytes / mx3 v3 | 1/8 | every seed |
| 7 vs 8 bytes / mx3 v3 | 7/8 | every seed |

Every message term in lines 8–10 goes through the public bijection g. The one-byte and eight-byte paths each perform exactly one message step, so their seed terms in line 13 are identical. Set w = g⁻¹(g(0) + C·(g(2) − g(9))) = 0x5c26a409b8e3cbb6; the remaining public terms are then equal too. The bijective finalizer cannot separate equal states. The verifier independently reconstructed partners for all 256 one-byte messages.

The verifier measured 1,073,741,823/1,073,741,823 collisions for the selected pair, reproduced verification 0x7B287B65, and compared 200,000 inputs against the SMHasher3 reference without mismatches. Algebra proves ε = 1 and L = 1, hence score 0, the metric floor. Its separately verified seven-byte-versus-eight-byte pair also has score 0.

No generic scan was run because the selected confirmed pair already reaches the metric floor. The record’s equal-length 16-byte family and broader word-count characterization are claimant-only and are not used for scoring.

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
SMHasher3 mx3 v3: 7B287B65 expected 7B287B65 PASS
structural checks PASS

1 vs 8 bytes / mx3 v3
M (1 B) = 00
M' (8 B) = b6cbe3b809a4265c
recorded colliding seed 2cb0f69f4abea221: H(M)=730d2d8dbe4d729e H(M')=730d2d8dbe4d729e
collisions = 1048576 / 1048576; rate = 1; log2(rate) = 0.000000; sampled score = 0.000000
first sampled colliding seed b3f2af6d0fc710c5: H(M)=H(M')=8b0eb1c4bbc21329

7 vs 8 bytes / mx3 v3
M (7 B) = 00000000000000
M' (8 B) = f55d0131fc859d54
recorded colliding seed 3bb548a553e612ba: H(M)=eebfade252d2c35f H(M')=eebfade252d2c35f
collisions = 1048576 / 1048576; rate = 1; log2(rate) = 0.000000; sampled score = 0.000000
first sampled colliding seed b3f2af6d0fc710c5: H(M)=H(M')=82d77d28d52c9566
```

## Attribution and licensing

The supplied claimant/verifier authors and paper driver are credited by path
above and in the source header. Algorithm authors and license notices are
preserved in the C file. Driver additions are Copyright (c) 2026 Thomas
Dybdahl Ahle, MIT; this does not relicense embedded algorithm code.
Pengyhash v0.3 carries GPLv3-or-later, NMHASH BSD 2-Clause, mx3 CC0,
and mir/fasthash/MUM/rapidhash MIT notices, as applicable.
