# XXH3-64 0.8.3: base-1143 at 16, 32 and 128 bytes

**The reviewer is correct.** For this pair, the 32-byte and 128-byte collision
seed sets are identical for every seed. Changing L from 16 to 4 reduces
`log2(L/epsilon)` by **exactly 2 bits**. The historical pooled estimate therefore
becomes `log2(4 * 2147483648 / 1030)` = approximately **22.991571377929418**,
instead of approximately 24.991571377929418. Two fresh confirmation streams
give `1018/2147483648` and a 32-byte score of approximately
**23.008478153924305**. These are sample estimates, not exact population rates.

The 16-byte prefix pair produced **zero collisions in every run**, including
on all 2048 logged 32-byte collision observations. This rules out transfer of
the same seed set, but does not prove that the 16-byte pair never collides.
32 bytes is the best witnessed length among the three tested; no global
optimality claim over all lengths or all pairs is made.

## Pair and source proof

The tested 32-byte messages are, in byte-order hex:

```text
M  = 8912a3da9fc464368202a1be238b7d1100000000000000000000000000000000
M' = 76ed5c25603b9bc97dfd5e41dc7482ee00000000000000000000000000000000
```

The first 16 bytes come from [xxh3_128b_pairs.json](xxh3_128b_pairs.json),
zero-based records 0 and 1: `id=1143, variant=0, tail=0, length=128`, stages
`confirm1` and `confirm2`. Both records contain identical message pairs.
The new tail is 16 zero bytes. The 16-byte test uses only the prefixes;
the 128-byte test uses the original messages with their original common
112-byte tail, not a newly zero-padded extension. All messages are in
[result.json](result.json) and the full logs.

In the actual embedded source,
[`XXH3_len_17to128_64b`](verify_pkg/xxh3-64/xxh3_64_32B_check.c#L4791)
at length 32 computes, modulo 2^64:

```text
acc = 32 * PRIME64_1
    + mix16B(input,      secret,      seed)
    + mix16B(input + 16, secret + 16, seed)
H = XXH3_avalanche(acc)
```

The second fold and length term are common to both messages. At 128 bytes
there are eight folds, of which only the fold at input offset 0 / secret
offset 0 changes. Addition of a common 64-bit value is invertible, including
wraparound. [`XXH3_avalanche`](verify_pkg/xxh3-64/xxh3_64_32B_check.c#L4608)
is also invertible: two right-xorshifts and multiplication by the odd constant
`0x165667919e3779f9`. Consequently, at either length,
`H_s(M) = H_s(M')` if and only if the first folds are equal. This proves
event-by-event identity for all 2^64 seeds and for any fixed common 16-byte
tail at 32 bytes; it is stronger than equality of sampled counts.

At 16 bytes,
[`XXH3_len_9to16_64b`](verify_pkg/xxh3-64/xxh3_64_32B_check.c#L4704)
uses secret offsets 24/32 and 40/48 to form its two keyed operands, and its
accumulator is `16 + swap64(input_lo) + input_hi + fold(input_lo,input_hi)`.
The preceding cancellation argument does not apply. One explicit distinction:

```text
seed = c8eae1baae13330b
16 B: H(M)=908ab0d640b4feb6  H(M')=28a993b3a78d73a2  unequal
32 B: H(M)=00ffe25bba202dfb  H(M')=00ffe25bba202dfb  equal
128 B: H(M)=448e3716c8effb94 H(M')=448e3716c8effb94 equal
```

## Measurements and intervals

All sampling and compilation ran on `hardware.normalcomputing.net`, under
`~/agents/xxh3-32B`, Intel Xeon Platinum 8375C. Four single-threaded sampling
processes ran at nice 10: at most four sampling threads, below the requested
32-thread limit. No compilation or sampling ran on the Mac. The compiler,
machine details, source/binary hashes and SciPy version are recorded in
[logs/environment.log](logs/environment.log); the warning-free build log is
[logs/build.log](logs/build.log).

The C program calls the complete `XXH3_64bits_withSeed` API twice per length
per seed. It checks the first-fold condition additionally, not as a substitute
for full hashing. The header embedded in the package and new program is
byte-identical, SHA-256
`17973c0dc49d9854ca26caa191f0e12f7a424b68858d9a78de3860d959d85e4b`.
Every run first passed SMHasher3 **0x1AAEE62C**, all ten supplied sanity
vectors, and the package's literal recorded witnesses.

| Run | Stream salt | Full output | Timing / exit status |
|---|---|---|---|
| fresh1 | `0x2026091801000477` | [log](logs/fresh1.log) | [time](logs/fresh1.time) |
| fresh2 | `0x2026091802000477` | [log](logs/fresh2.log) | [time](logs/fresh2.time) |
| historical1 | `0x2026091702000477` | [log](logs/historical1.log) | [time](logs/historical1.time) |
| historical2 | `0x2026091703000477` | [log](logs/historical2.log) | [time](logs/historical2.time) |

Each run contains exactly 2^30 = 1073741824 sampled seeds per length.
Two logical workers each supply 2^29 seeds, processed sequentially in one
physical thread. Initialization is four SplitMix64 outputs from
`(salt * 0x9e3779b97f4a7c15 + 7919 * worker + 1) mod 2^64`, followed by
xoshiro256**. This preserves the historical stream convention and indices.
The two fresh runs use distinct streams from each other and all historical
selection/confirmation streams. These are reproducible pseudorandom samples
of the uniform 64-bit seed domain, not exhaustive enumeration or a proof of
mathematical independence of PRNG streams.

For k hits in N trials, epsilon-hat is exactly k/N. The central equal-tail
95% **exact Poisson (Garwood)** interval is

```text
lower = chi2_quantile(0.025, 2*k) / (2*N), or 0 when k=0
upper = chi2_quantile(0.975, 2*(k+1)) / (2*N)
score = log2(L*N/k), when k>0
```

“Exact” describes the Poisson interval construction; the rare-event Poisson
model approximates the underlying seed-sampling count distribution. These
are not binomial intervals. Numerical endpoints and scores below are binary64
approximations, printed without additional decimal rounding. Exact integer
counts, fraction expressions, exact terminating decimals for sample rates,
and symbolic score/interval expressions are retained in [result.json](result.json).
The precision of the numerical display does not remove sampling uncertainty.

| Stream / pool | B | L | Hits / trials = sample epsilon | Exact-Poisson 95% epsilon interval (numerical) | Score (numerical) |
|---|---:|---:|---|---|---|
| fresh1 | 16 | 2 | 0/1073741824 | [0.0, 3.4355367106515312e-09] | not estimated |
| fresh1 | 32 | 4 | 483/1073741824 | [4.1060167657147987e-07, 4.917927147669023e-07] | 23.084120621164228 |
| fresh1 | 128 | 16 | 483/1073741824 | [4.1060167657147987e-07, 4.917927147669023e-07] | 25.084120621164228 |
| fresh2 | 16 | 2 | 0/1073741824 | [0.0, 3.4355367106515312e-09] | not estimated |
| fresh2 | 32 | 4 | 535/1073741824 | [4.569258159359195e-07, 5.423240808782643e-07] | 22.93660491871149 |
| fresh2 | 128 | 16 | 535/1073741824 | [4.569258159359195e-07, 5.423240808782643e-07] | 24.93660491871149 |
| historical1 | 16 | 2 | 0/1073741824 | [0.0, 3.4355367106515312e-09] | not estimated |
| historical1 | 32 | 4 | 504/1073741824 | [4.292964862157438e-07, 5.122125906764581e-07] | 23.022720076500082 |
| historical1 | 128 | 16 | 504/1073741824 | [4.292964862157438e-07, 5.122125906764581e-07] | 25.022720076500082 |
| historical2 | 16 | 2 | 0/1073741824 | [0.0, 3.4355367106515312e-09] | not estimated |
| historical2 | 32 | 4 | 526/1073741824 | [4.4890060299564103e-07, 5.335858192540014e-07] | 22.961081010707698 |
| historical2 | 128 | 16 | 526/1073741824 | [4.4890060299564103e-07, 5.335858192540014e-07] | 24.961081010707698 |
| fresh_only | 16 | 2 | 0/2147483648 | [0.0, 1.7177683553257656e-09] | not estimated |
| fresh_only | 32 | 4 | 1018/2147483648 | [4.453666711465765e-07, 5.04081714020458e-07] | 23.008478153924305 |
| fresh_only | 128 | 16 | 1018/2147483648 | [4.453666711465765e-07, 5.04081714020458e-07] | 25.008478153924305 |
| historical_replays_only | 16 | 2 | 0/2147483648 | [0.0, 1.7177683553257656e-09] | not estimated |
| historical_replays_only | 32 | 4 | 1030/2147483648 | [4.507834641904563e-07, 5.098407090772407e-07] | 22.991571377929418 |
| historical_replays_only | 128 | 16 | 1030/2147483648 | [4.507834641904563e-07, 5.098407090772407e-07] | 24.991571377929418 |

The fresh pooled epsilon is exactly
`1018/2147483648 = 0.000000474043190479278564453125`.
Its transformed 95% score interval at 32 bytes is approximately
`[22.919839043866432, 23.098503065606845]`; at 128 bytes, add exactly 2.
The historical pooled epsilon is exactly
`1030/2147483648 = 0.000000479631125926971435546875`.
Its transformed score interval at 32 bytes is approximately
`[22.903450091873268, 23.081062069215978]`; at 128 bytes, add exactly 2.

With zero 16-byte hits, there is no finite empirical score to publish.
For the two fresh streams alone, epsilon's central 95% interval is
`[0, -ln(0.025)/2147483648]`, numerically
`[0, 1.7177683553257656e-9]`; transforming it gives the score interval
`[30.11681735485275, +infinity]`. Across all four distinct streams tested
at 16 bytes, there are `0/4294967296` hits, giving
`[0, -ln(0.025)/4294967296]`, numerically `[0, 8.588841776628828e-10]`,
and transformed score interval `[31.11681735485275, +infinity]`.
These are bounds under the stated model, not a collision witness or proof
of zero population probability. The zero-count upper endpoint uses the
central 95% convention, not the smaller one-sided 95% endpoint.

All four runs had zero 32/128 event mismatches and zero 32/first-fold event
mismatches over every sampled seed. [logs/validation.json](logs/validation.json)
also confirms that **all 1030 historical witness indices, seeds and both
128-byte outputs match their original records in order**. All those seeds
collide at 32 bytes too; none collide at 16 bytes. The original historical
runs and their replays are the same observations and must not be added
together. Likewise, 32- and 128-byte tests reuse each seed and must not be
pooled as independent evidence. Fresh and historical estimates are presented
separately; screen/refinement data are excluded.

## What supports the 9 / 12 / 11 / 11 claim?

**The four numbers are supported for one common 32-byte pair A, but that pair
is not the base-1143 pair tested above.** Pair A is:

```text
M  = 9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c
M' = 642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c
```

All listed records contain those exact same bytes, `trials=1073741824`,
`trials_log2=30`, and stream salt 20260917 (decimal). Every record includes
exactly as many equal-output collision witnesses as its count.

| Hash | Count / 2^30 | Exact supporting records and other package evidence |
|---|---:|---|
| wyhash final v4.3 | 9 | [paper_rows.json](paper_rows.json), zero-based index 0, `id=wyhash32`; identical record at index 0 of [wyhash/supplied_rows.json](verify_pkg/wyhash/supplied_rows.json). [README](verify_pkg/wyhash/README.md) explicitly states historical 9 / 2^30. |
| rapidhash v1 | 12 | [paper_rows.json](paper_rows.json), index 3, `id=rapid1_32`; identical record at index 0 of [rapidhash-v1/supplied_rows.json](verify_pkg/rapidhash-v1/supplied_rows.json). [README](verify_pkg/rapidhash-v1/README.md) explicitly states historical 12 / 2^30. |
| rapidhash v3 standard | 11 | [paper_rows.json](paper_rows.json), index 5, `id=rapid3_32`. Micro and nano independently recorded 11 too: indices 8 and 11, `rapid3_micro_32` and `rapid3_nano_32`. The v3 package has no `supplied_rows.json`; its [README](verify_pkg/rapidhash-v3/README.md), C verifier and `run_2p20.txt` reproduce the pair/witness and a zero-hit smoke run, not the historical count 11. |
| XXH3-64 0.8.3 | 11 | [paper_rows.json](paper_rows.json), index 14, `id=xxh64_32`; identical record at index 1 of [xxh3-64/supplied_rows.json](verify_pkg/xxh3-64/supplied_rows.json). [README](verify_pkg/xxh3-64/README.md) explicitly states historical pair-A 11 / 2^30. Its `xxh64_128` record is an extension of pair A, not base-1143. |

[logs/source_audit.json](logs/source_audit.json) records byte-equality checks,
record identities, witness counts, and input SHA-256 hashes. This is an audit
of the supplied historical evidence; the other hashes' 2^30 experiments
were not rerun in this task. Package `run_2p20.txt` files are smoke tests and
do not support or refute a specific historical 2^30 count. None of these
files supports assigning the 9/12/11/11 figures to the new base-1143-plus-zero-tail
pair. For that pair, the measured XXH3 counts are the ones above; its wyhash
and rapidhash rates were not measured here.

## Reproduction and deliverables

The standalone program is
[verify_pkg/xxh3-64/xxh3_64_32B_check.c](verify_pkg/xxh3-64/xxh3_64_32B_check.c).
Its [usage and expected counts](verify_pkg/xxh3-64/CHECK_32B.md) accompany the
[complete expected default output](verify_pkg/xxh3-64/xxh3_64_32B_check.expected.txt).
The default 2^20 run prints the verification checks and known collision first,
then three zero sample counts. This smoke sample is not used for estimation.

```sh
gcc -O3 -std=c11 -Wall -Wextra verify_pkg/xxh3-64/xxh3_64_32B_check.c -lm -o xxh3_64_32B_check
nice -n 10 ./xxh3_64_32B_check 30 0x2026091801000477
nice -n 10 ./xxh3_64_32B_check 30 0x2026091802000477
```

To reproduce all four full runs on the Xeon from this directory layout,
run `bash reproduce.sh`. [reproduce.sh](reproduce.sh) limits concurrency to
four single-threaded nice-10 processes. [build_results.py](build_results.py)
then checks the logs against the original JSON witnesses and generates
[result.json](result.json), [logs/statistics.md](logs/statistics.md) and
[logs/validation.json](logs/validation.json); it requires SciPy (the recorded
run used 1.13.1). [logs/analysis.log](logs/analysis.log) is its actual output.
All large-run stdout logs retain every sampled collision, including both
hash outputs at every length. All timed runs exited with status 0.
