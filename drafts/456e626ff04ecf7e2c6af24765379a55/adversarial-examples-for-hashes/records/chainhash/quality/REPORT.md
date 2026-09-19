# ChainHash: complete SMHasher3 and rurban/smhasher suites

Run date: 2026-09-19. All full-suite runs finished.

The 64-bit ChainHash function (`include/chainhash.h` in
[thomasahle/chainhash](https://github.com/thomasahle/chainhash); comb PH,
Horner chain, twisted quintic finalizer; the Lean-proved function) was put
through the complete SMHasher3 suite on two hosts and through the complete
rurban/smhasher suite on one host, registered as `chainhash`. The
128-bit function was not run through either full suite; its sanity and
verification checks are recorded with its measurements.

## Header and verification constants

| Registration | SMHasher3 LE | SMHasher3 BE | rurban (LE) |
| --- | --- | --- | --- |
| `chainhash` | `66672BD6` | `FA8A8D3B` | `66672BD6` |

The standalone evaluator reproduces the constants independently of either
suite, on both the portable and the PMULL path. The BE registration swaps
only the digest serialization; message words stay canonical little-endian.

## SMHasher3 full suite (`--test=All`)

| Host | Registration | Passed | Failed | Wall minutes |
| --- | --- | ---: | ---: | ---: |
| xeon | `chainhash` | 200/200 | 0 | 22.92 |
| m2 | `chainhash` | 200/200 | 0 | 20.43 |

The hash passes every test on both hosts, and the two hosts print
identical diagnostics (all 1863 collision/distribution rows and the
p-value histograms agree line for line). The suite is deterministic given
the hash, so this is expected; it also rules out an ISA-specific backend
bug, since the Xeon binary exercises the PCLMUL/VPCLMUL paths and the M2
binary the NEON PMULL paths.

Suite composition. Upstream SMHasher3 at the pinned commit
`3de870c7ab449ad11cf450848d9270e3f54102d1` reports 188 cases for
`--test=All`. The test tree adds one family, SeedDifferential (fixed
message pairs, 2^24 random seeds per pair, 12 message lengths 16–1024
bytes), which brings the reported total to 200. This is the same 200-case
suite the paper's measurements table reports against, so the counts are
directly comparable. The 22 sections run are Sanity, Speed, Hashmap speed,
Avalanche, BIC, Keyset Zeroes / Cyclic / Sparse / Permutation / Text /
TwoBytes / PerlinNoise / Bitflip, Seed Zeroes, SeedSparse, Seed
BlockLength / BlockOffset, Keyset Seed, Seed Avalanche, Seed BIC, Seed
Bitflip, and Seed Differential. `BadSeeds` is not part of `All` at this
revision. This is upstream's standard `All`, not the separate `--extra`
torture mode. Every run used `--test=All --ncpu=8 --noexit-on-failure
--exit-code-on-failure`, so a failure would have been printed and counted
rather than aborting the run.

The `-log2(p-value)` histogram over the 6883 individual statistics
(identical on both hosts):

| 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12+ |
| ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 4405 | 1280 | 592 | 289 | 162 | 79 | 34 | 21 | 13 | 6 | 0 | 2 | 0 |

The worst statistic has p ≈ 2^-11; nothing reaches the failure region.

Hosts. Xeon: Intel Xeon Platinum 8375C, Clang 21.1.8, CMake 3.31.8, each
invocation `taskset -c 8-15 nice -n 10 …`, runs sequential. Other users'
jobs ran on the host throughout (1-minute load 37–50 on 64 cores), so the
Speed sections embedded in these logs are not a benchmark and must not be
cited as one. M2: Apple M2 Pro, Apple Clang, `nice -n 10`. The M2 runner
admitted each run only when no `SMHasher3` process existed and the
1-minute load was below 4.5; the gate only checks at launch, and another
short timing job very likely resumed while the 20-minute run was in
flight. This does not affect the quality verdicts, because the suite is
deterministic and the M2 diagnostics equal the Xeon's.

## rurban/smhasher full suite (`--test=All,BIC`)

| Host | Registration | Quality sections passed | Failed sections | Wall minutes |
| --- | --- | ---: | ---: | ---: |
| xeon | `chainhash` | 17/17 | 0 | 40.03 |

Pinned to commit `2688c165595ad68a0543f96940617adc5e22d87e`; Clang
21.1.8. Each invocation is `taskset -c 8-15 nice -n 10 SMHasher
--test=All,BIC chainhash`. At this revision `All` runs Sanity, Speed,
Hashmap, Avalanche, Sparse, Permutation, Window, Cyclic, TwoBytes, Text,
Zeroes, Seed, PerlinNoise, Diff, DiffDist, MomentChi2, Prng and BadSeeds;
it adds BIC only for hashes wider than 64 bits under `--extra`, so BIC is
selected explicitly. LongNeighbors is compiled out upstream. `--extra`
(the exhaustive 32-bit bad-seed scan) is not the standard suite and was
not used.

The 17 quality sections (Avalanche, Sparse, Permutation, Window, Cyclic,
TwoBytes, Text, Zeroes, Seed, PerlinNoise, Diff, DiffDist, MomentChi2,
Prng, BIC, BadSeeds, plus the Sanity checks) print no failure mark; the
Sanity section confirms the verification value `66672BD6`. Worst
statistics: Avalanche worst bias 0.84% over 24–1024-bit keys; BIC max bias
0.0088; MomentChi2 1.61 / 0.93 (bits 1/0), rated "Great"; Prng 2^25 values
with no 64-bit collision and 32-bit collision ratio 1.00×; BadSeeds seed 0
PASS. The suite's own timer reports 2520 s. As with SMHasher3, the
embedded Speed sections are not a benchmark: rurban's small-key loop
changes the seed after every call, so its cycle counts include a full key
setup, and the host was shared.

This fork prints no aggregate counter. A failing statistic is marked
` !!!!!` and a failing test `*********FAIL*********`; the summary counts
both, and the table reports sections (excluding the two timing sections)
with and without such marks. Exit status is not used as evidence of a
pass, because this fork can exit zero after a reported failure. The
`pfHash` API carries a 32-bit seed, zero-extended into the same SplitMix64
mapping as SMHasher3 (below), so rurban's seed tests cover 32-bit seeds.
With no bad seeds registered, its standard BadSeeds section tests seed
zero.

## Seed model (outside the theorem)

SMHasher3 hands the hash a 64-bit seed. The adapter expands it with
SplitMix64 starting from that seed, takes eight outputs as
`s, y, c0..c4, tau` and calls the header's `chainhash_key_from_seed`,
which sets the block keys to powers of `s` in GF(2^64). Keys live in
thread-local storage returned by the suite's seed callback.

This expansion is outside the collision theorem: a 64-bit (or, for
rurban, 32-bit) seed does not supply eight independent uniform words. The
suites therefore test a deterministic seeded adapter; they neither prove
the theorem's assumptions nor establish cryptographic strength.

## The twist, and what a failure would have meant

The header keeps the integer addition `v = P ⊞ tau` before the degree-5
GF(2^64) circuit. The paper's twist section explains why it is there: over
GF(2^64), `v ↦ v^e` has F2-degree equal to the popcount of `e`, so a
polynomial of degree ≤ 6 is at most quadratic in the input bits, with
affine second derivatives, and SMHasher3's fixed-seed keysets (Zeroes,
Sparse, Permutation, TwoBytes, Bitflip) detect exactly that structure. The
paper's measurements on the same 200-case suite:

| Finalizer | Twist | Tests passed |
| --- | --- | ---: |
| degree 7 (4 mult.) | none | 200/200 |
| degree 5 (3 mult.) | none | 178/200 (22 failures: Zeroes, Sparse, Permutation, TwoBytes, Bitflip) |
| degree 5 (3 mult.) | input, `v ⊞ tau` | 200/200 |
| degree 3 (2 mult.) | input | 183/200 |

ChainHash is the third row; an SMHasher3 failure here would have
contradicted the paper. There was none. No untwisted control was rerun
in this lane. The paper is explicit that the twist's sufficiency for a
given suite is empirical, not a theorem, and the exact seeded adapter is
documented above so the result can be reproduced.

## Files

`summary.json` beside this report lists each run (suite, host, start,
end, exit code, verdict and section counts). No upstream pull request has
been filed for either suite.
