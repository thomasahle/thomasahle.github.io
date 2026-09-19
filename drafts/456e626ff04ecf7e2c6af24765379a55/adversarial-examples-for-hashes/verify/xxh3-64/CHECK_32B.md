# Selected block-1 length check

Build `xxh3_64_pair_check.c`, the current standalone program, as described in [README.md](README.md). This file describes the default (seed-only, default-secret) mode, which is the caveat model for the seeded interfaces; the scored random-secret mode (`./check random-secret 20`) is described in the README. Arguments of the default mode: `[log2 N (1..40), default 20] [stream salt] [M hex] [Mprime hex]`; custom messages must both be 32 bytes.

The selected witness seed `2468b3bc26a44073` produces `dd686b61e2f6dc69` for both strings. Seed 0 differs. The default salt is `0x2026091901000477`.

For 2^20 samples the Xeon observed:

| Length | Collisions | Interpretation |
|---|---:|---|
| 16 | not sampled | identical zero prefixes, not a distinct pair |
| 24 | 0 | no finite empirical cap from this smoke run |
| 32 | 725 | selected pair, L = 4 |
| 128 | 725 | block 1 moves to bytes 112..127; same seeded secret words |

Both event-mismatch counts are zero. [Full expected output](xxh3_64_32B_check.expected.txt) is the actual Xeon output of the **new** program, despite the retained expected-file name. `xxh3_64_32B_check.c` is a compatibility entrypoint including the new selected program; the original base-1143 program is retained as `historical_base1143_check.c`.

The 128-byte extension has L = 16, so the same rate yields a cap two bits higher. Its observations are correlated with the 32-byte run and are not pooled as fresh samples. The three independent upstream confirmation streams recorded with the article, not this smoke sample, supply 2,264,081/(3·2^30) and the 12.47-bit default-secret cap, which the article reports as a caveat beside the scored random-secret result.
