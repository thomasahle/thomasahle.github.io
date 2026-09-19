# MuseAir v2 timing records

[Timing report](REPORT.md), [measurements and provenance](speeds_museair_v2.json), and [registration source](bench/museair_v2.cpp).

The separately registered algorithm-v2 standard hash uses a direct 64-bit seed. Xeon selects higher bulk and lower small latency from two runs; M2 uses the median of three. Final bulk values are 7.53 B/cycle (Xeon) and 10.42 B/cycle (M2); short-input values are 29.27 and 22.32 cycles/hash respectively. All Sanity checks pass, and no >15% repeatability/control flags were reported. The timing source is a validated C/C++ port of crate 0.6.0, not Rust code generation.

The supplied port and vectors referenced by the original timing report are archived next door in [muse-v2](../muse-v2/README.md); its [port](../muse-v2/museair2.h) and [vectors](../muse-v2/vectors.txt.gz) were the timing inputs. The [independent upstream Rust verification](../muse-v2-verify/README.md) is separate. Historical command strings and build paths in the evidence describe the execution environment; article links use the copied records here.
