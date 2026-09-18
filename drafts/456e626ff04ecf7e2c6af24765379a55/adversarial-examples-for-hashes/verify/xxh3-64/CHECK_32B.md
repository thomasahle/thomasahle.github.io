# Base-1143 length-normalisation check

`xxh3_64_32B_check.c` is a standalone C11 program embedding the same unmodified
xxHash 0.8.3 header as `xxh3_64_verify.c`, with the original license notices.
It needs no other source file or input file. It runs in one thread and writes
only stdout/stderr. Run large samples on the Xeon, not the timing Mac.

```sh
gcc -O3 -std=c11 -Wall -Wextra xxh3_64_32B_check.c -lm -o xxh3_64_32B_check
nice -n 10 ./xxh3_64_32B_check
nice -n 10 ./xxh3_64_32B_check 30 0x2026091801000477
nice -n 10 ./xxh3_64_32B_check 30 0x2026091802000477
```

Arguments are `[log2 N (1..40), default 20] [stream salt, default
0x2026091801000477]`. The salt is not the API seed. Each of two logical workers
initializes xoshiro256** through four SplitMix64 outputs from
`salt * 0x9e3779b97f4a7c15 + 7919 * worker + 1` modulo 2^64 and supplies N/2
full-width API seeds. Both workers run sequentially in the same thread.
Each sampled seed is used at all three lengths. This reproduces the historical
two-worker sample and its indices independently of CPU scheduling.

Startup first reproduces SMHasher3 **0x1AAEE62C**, then checks all ten original
sanity vectors and the package's literal pair-A and base-1143 witnesses.
It prints the 16-, 32-, and 128-byte messages and the base-1143 witness at
each length. At 32 bytes the tail is 16 zero bytes. At 128 bytes it retains
the original published 112-byte suffix. At 16 bytes it uses the prefix alone.

For every sampled seed, both messages are hashed through the full
`XXH3_64bits_withSeed` API at each length. A non-inlined API wrapper prevents
the compiler from replacing these comparisons with a specialized fold check.
The first-fold check is additional: both the 32/128 collision-event mismatch
count and the 32/first-fold mismatch count must be zero or the program fails.
Every sampled collision prints its seed, global trial index, and both complete
outputs at all lengths. The known startup witnesses are excluded from counts.

The complete expected default output is
[`xxh3_64_32B_check.expected.txt`](xxh3_64_32B_check.expected.txt), copied from the
actual Xeon smoke run. Its sample counts are `0 0 0`; that smoke sample is too
small to estimate these rates. The startup witness is:

```text
witness len=16 seed=c8eae1baae13330b a=908ab0d640b4feb6 b=28a993b3a78d73a2 collision=0
witness len=32 seed=c8eae1baae13330b a=00ffe25bba202dfb b=00ffe25bba202dfb collision=1
witness len=128 seed=c8eae1baae13330b a=448e3716c8effb94 b=448e3716c8effb94 collision=1
```

Expected counts at N = 2^30 (16 B, 32 B, 128 B):

| Salt | Counts |
|---|---|
| `0x2026091801000477` | `0 483 483` |
| `0x2026091802000477` | `0 535 535` |
| `0x2026091702000477` | `0 504 504` |
| `0x2026091703000477` | `0 526 526` |

All four runs end with both mismatch counts and the 16/32 intersection count
equal to zero, followed by `PASS`. Full expected large-run outputs and timing
logs are in [`../../logs/`](../../logs/). Historical salts replay original
observations; they must not be counted as additional data beyond those originals.

[`../../RESULT.md`](../../RESULT.md) gives the proof, measured probabilities,
exact-Poisson interval definition, score calculations, and the distinction
between base-1143 and the separate pair A supporting the 9/12/11/11 counts.
[`../../result.json`](../../result.json) retains exact integer counts, fraction
and score expressions, numerical intervals, and provenance.
