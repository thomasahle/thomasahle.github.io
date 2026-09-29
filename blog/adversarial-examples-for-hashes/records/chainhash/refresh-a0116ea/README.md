# ChainHash at a0116ea: same-binary refresh on Xeon and M2

The chart's ChainHash and ChainHash-128 cells on the Xeon and M2 tabs are measured at
[thomasahle/chainhash `a0116ea`](https://github.com/thomasahle/chainhash/tree/a0116ea2072c0d9605acc6b47f0dc9f9d57b0c58).
The AMD EPYC 9R14 (Zen 4) tab measures the same revision ([records/zen4](../../zen4/README.html)),
so all three tabs show one revision of each function.

Since the previously published cells, the headers gained only exact-output speed changes:

- shorter paths for inputs up to 128 bytes, and faster key expansion;
- prefetch-parameterized bulk kernels and a stream fast path;
- fused PMULL+EOR kernels on Apple cores;
- vector tails on every x86 backend;
- a spill-free ChainHash-128 ZMM accumulation and a register-bounded XMM kernel;
- the ChainHash-128 product method chosen by the PCLMULQDQ issue rate.

The calibration header is opt-in and is not used here. Both functions are unchanged: the
verification values are the repository's, 0x66672BD6 (ChainHash) and 0x1FCA728C
(ChainHash-128), on every registration and host.

Each binary links two revisions of the headers side by side: `a0116ea` and `0a03c63` as a
control. PolyXOR128 and XXH3-128 run in the same binary.

| Registration | Xeon bulk | Xeon 1–31 B | M2 bulk | M2 1–31 B |
|---|---:|---:|---:|---:|
| **chainhash.a0116ea** | **28.25** | **80.38** | **30.15** | **89.20** |
| chainhash.0a03c63 | 28.28 | 120.14 | 30.20 | 89.16 |
| **chainhash-128.a0116ea** | **14.90** | **129.30** | **16.85** | **158.25** |
| chainhash-128.0a03c63 | 14.92 | 146.38 | 16.70 | 159.34 |
| polyxor-128 | 15.58 | 172.16 | 16.71 | 142.51 |
| XXH3-128 | 19.94 | 34.11 | 12.55 | 30.97 |

These replace the previously published cells:

| Row | Xeon (bulk / 1–31 B) | M2 (bulk / 1–31 B) |
|---|---|---|
| ChainHash | 28.31 / 155.14 | 26.26 / 87.49 |
| ChainHash-128 | 14.43 / 175.93 | 10.26 / 167.89 |

## Method

- Unmodified SMHasher3 `--test=Speed`, one process at a time. Every start waits until no other
  SMHasher3 process runs. A whole-machine scan every 15 s discards and repeats any run that
  overlaps another SMHasher3 process; none was discarded.
- Xeon 8375C: the post's `build-release-20260917` objects and the PolyXOR128 lane's objects, as
  in [records/polyxor](../../polyxor/README.html). Runs use `nice -n 10 taskset -c 16-23`. The
  cell takes the higher bulk of two passes and, independently, the lower small-key average.
  Units are bytes per TSC tick (TSC 2.900 GHz).
- M2 Pro: the public reproduction's arm64 build. This is the base of the records/polyxor M2
  binary; the post's original M2 base binary is no longer available. Runs use `nice -n 10`,
  gated on load1 < 4.5. The cell takes medians of three passes. Units are SMHasher3's
  calibrated cycles. The largest M2 spreads are about 4% (bulk passes: ChainHash
  30.15 / 31.32 / 30.11, ChainHash-128 16.85 / 17.38 / 16.73), with no run more than 15% from
  its median.
- Every registration passes SMHasher3 Sanity and its verification value on both hosts
  (`evidence/*/NAME.sanity.txt`). The two revisions' verification values are identical, as
  SMHasher3's duplicate-code warning in the logs notes.

## Files

| File | What it is |
|---|---|
| [speeds_chainhash_a0116ea.json](speeds_chainhash_a0116ea.json) | Selected cells, every pass, spreads, per-length 1–31 B costs, load at launch |
| [evidence/Xeon/](evidence/Xeon/provenance.json), [evidence/M2/](evidence/M2/provenance.json) | Raw `--test=Speed` logs (`NAME.runN.txt`), run metadata (`NAME.runN.json`), Sanity logs, gate log, build provenance |
| [bench/](bench/build.py) | Registration sources, link script, runner and summarizer |
