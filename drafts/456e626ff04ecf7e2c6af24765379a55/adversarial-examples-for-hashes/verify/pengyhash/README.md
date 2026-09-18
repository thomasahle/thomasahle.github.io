# pengyhash: standalone fixed-pair reproduction

The attacked function is SMHasher3’s sequenced pengyhash v0.3, verification 0x861A1254, with a 64-bit seed. The task’s original v0.2 label and rurban’s v0.2 results concern an older 32-bit-seed implementation. Upstream v0.3 has a reported unsequenced modification; this result is pinned to the sequenced SMHasher3 form. v0.3 uses GPLv3; earlier versions used BSD 2-Clause.

NEW: the supplied checked record identifies no prior literature for this pair or seed-free bulk collision. The SeedBlockLen failures in SMHasher3 are related-seed collisions, not the same-seed event scored here.

## Build and run

```sh
cc -O2 -std=c11 -o pengyhash_verify pengyhash_verify.c -lm
./pengyhash_verify 20
```

Arguments: `[log2 N (0..40), default 20] [RNG seed, default 1]`.
The program samples exactly N = 2^log2N seeds **per pair and variant**.
Everything is single-threaded. It requires a C11 compiler; exact 128-bit
products use the GCC/Clang `__uint128_t` extension where applicable.
It reads no external files and writes only stdout/stderr.

## Implementation and validation

Adapted from `heur2_scratch/verify-pengyhash/pengy_own.h and its verifier` in the supplied workspace, credited in the C
header. Loads are explicit little-endian and work on either host byte order.
Algorithm notices are retained. NMHASH's 16-bit products use unsigned
32-bit intermediates to avoid signed integer-promotion overflow.

| variant | output bits | sampled seed bits | SMHasher3 verification | output encoding for verification |
|---|---:|---:|---|---|
| pengyhash v0.3 | 64 | 64 | `0x861A1254` | little-endian |

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
| equal-length / pengyhash v0.3 | 32/32 | every seed |
| cross-length / pengyhash v0.3 | 32/1 | every seed |

Lines 3–7 compress each block before the seed enters on line 10. For the selected pair, w2 = w3 = 0 and w1 + 2w0 = 0 on both sides. The remaining public word-pair function has the same value, u1 = 0x3ad679fa2cd9554d, on both inputs. A Brent-rho search found this equality. Both bulk states are (0x20, 0xe38334c0faa0af61, 0x3ad679fa2cd9554d, 0x20) and both tails are empty. Identical inputs to lines 9–15 prove equal outputs for every seed.

The independent verifier measured 1,073,741,824/1,073,741,824 collisions. State equality proves ε = 1, so L = 4 gives score ≤ 2. The separately verified 32-byte-versus-one-byte pair has the same cap; the package includes it too.

* Not verified (claimant only): the generic scan of 2,000 cells × 2^26 seeds (zero hits, resolution about 2^-26 per cell), the heuristic for lengths below 32 bytes, and the other solver targets. They do not affect the score.

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
SMHasher3 pengyhash v0.3: 861A1254 expected 861A1254 PASS
structural checks PASS

equal-length / pengyhash v0.3
M (32 B) = 7d664c02e4863788063367fb37f290ef00000000000000000000000000000000
M' (32 B) = 2bbb99a5513852e1aa89ccb45c8f5b3d00000000000000000000000000000000
recorded colliding seed 3a34ce6380fc0bc5: H(M)=90f2edac34d9a4d5 H(M')=90f2edac34d9a4d5
collisions = 1048576 / 1048576; rate = 1; log2(rate) = 0.000000; sampled score = 2.000000
first sampled colliding seed b3f2af6d0fc710c5: H(M)=H(M')=08f74a78723385cd

cross-length / pengyhash v0.3
M (32 B) = c5e359af9bb2c4c857384ca1c89a566e1eb7630b6d21d924c49138e925bd4db6
M' (1 B) = 00
recorded colliding seed 3a34ce6380fc0bc5: H(M)=cd93fc86a39c2c20 H(M')=cd93fc86a39c2c20
collisions = 1048576 / 1048576; rate = 1; log2(rate) = 0.000000; sampled score = 2.000000
first sampled colliding seed b3f2af6d0fc710c5: H(M)=H(M')=aa93b6cef9a97f26
```

## Attribution and licensing

The supplied claimant/verifier authors and paper driver are credited by path
above and in the source header. Algorithm authors and license notices are
preserved in the C file. Driver additions are Copyright (c) 2026 Thomas
Dybdahl Ahle, MIT; this does not relicense embedded algorithm code.
Pengyhash v0.3 carries GPLv3-or-later, NMHASH BSD 2-Clause, mx3 CC0,
and mir/fasthash/MUM/rapidhash MIT notices, as applicable.
