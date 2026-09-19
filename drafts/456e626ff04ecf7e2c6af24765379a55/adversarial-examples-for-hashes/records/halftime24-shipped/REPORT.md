# Shipped HalftimeHash24: SMHasher3 timing on both hosts

Plot **HalftimeHash24-shipped at (12.81 B/cycle, 0 bits) on Xeon** and **(1.37 B/cycle, 0 bits) on M2**. The score concerns the **full 24-byte advanced value**; SMHasher3 receives its first eight bytes. These are measurements of the shipped implementation, including its defects.

The upstream header is unchanged: SHA-256 `7ef5dd48f54537b430f85bc1867b23a93551ab1c56415cfcef362d1651956cc3`, matching `verify-halftime/README.md`. Xeon selects `advanced::V4<3>` → AVX-512. The literal header does **not compile with its default ARM NEON dispatch**: it defines `VjSse2` but references absent `VjNeon`. On M2, the adapter undefines `__ARM_NEON` and `__ARM_NEON__` only after including SMHasher3 platform headers, so the unchanged upstream header selects **V4Scalar, eight scalar 64-bit lanes**. No NEON dispatch aliases or algorithm repairs are applied. The repaired control uses NEON; this is consequently not an isolated measurement of the algorithmic repair on M2.

| Host | Registration | SMHasher output bits | Backend | Bulk B/cycle ↑ | Small 1–31 B cycles/hash ↓ |
|---|---|---:|---|---:|---:|
| Xeon8375C | HalftimeHash24-shipped | 64 | avx512f | 12.81 | 64.91 |
| Xeon8375C | chainhash-256 | 64 | hwclmul | 14.45 | 103.03 |
| Xeon8375C | XXH3-64 | 64 | avx512 | 19.98 | 29.55 |
| Xeon8375C | HalftimeHash-512 | 64 | avx512f | 19.39 | 84.15 |
| Xeon8375C | HalftimeHash24-fixed | 192 | avx512f | 14.07 | 78.46 |
| M2Pro | HalftimeHash24-shipped | 64 | scalar | 1.37 | 60.69 |
| M2Pro | chainhash-256 | 64 | hwpmull | 22.76 | 71.62 |
| M2Pro | XXH3-64 | 64 | neon | 13.05 | 25.07 |
| M2Pro | HalftimeHash-512 | 64 | neon | 7.51 | 63.90 |
| M2Pro | HalftimeHash24-fixed | 192 | neon | 5.66 | 48.56 |

| Host | Shipped / fixed bulk | Shipped / fixed small | Current fixed bulk vs previous | Current fixed small vs previous |
|---|---:|---:|---:|---:|
| Xeon8375C | 0.910 (-9.0%) | 0.827 (-17.3%) | 14.07 / 14.12 | 78.46 / 79.42 |
| M2Pro | 0.242 (-75.8%) | 1.250 (+25.0%) | 5.66 / 5.55 | 48.56 / 48.63 |

Bulk ratios above one mean faster; small ratios above one mean slower. All five rows on each host use **one unchanged binary**, including the retained 192-bit fixed registration and the existing `HalftimeHash-512` 64-bit Style port. Style speed must not be relabeled as advanced-24 speed.

## Protocol and run variation

Commands are exactly `SMHasher3 NAME --test=Speed`, default priority and affinity. Headline bulk is the **fixed 262144-byte Average**, not the varying-length bulk section. Xeon ran two complete passes, independently selecting higher bulk and lower small; equal rounded bulk uses higher printed GiB/s, then the earlier run. M2 ran three complete passes and uses independent medians. Every M2 run waited until no other process named `SMHasher3` existed and one-minute load was **<4.5**, polling every **60 seconds**, without a timeout. Preparation was gated too. This is a start gate, not a promise of sustained low load.

| Host | Registration | Bulk runs (B/cycle) | Small runs (cycles/hash) | Selected bulk / small run |
|---|---|---|---|---|
| Xeon8375C | HalftimeHash24-shipped | 12.80, 12.81 | 64.91, 65.05 | 2 / 1 |
| Xeon8375C | chainhash-256 | 14.33, 14.45 | 103.05, 103.03 | 2 / 2 |
| Xeon8375C | XXH3-64 | 19.38, 19.98 | 29.58, 29.55 | 2 / 2 |
| Xeon8375C | HalftimeHash-512 | 19.18, 19.39 | 84.15, 84.72 | 2 / 1 |
| Xeon8375C | HalftimeHash24-fixed | 14.02, 14.07 | 78.46, 78.57 | 2 / 1 |
| M2Pro | HalftimeHash24-shipped | 1.37, 1.41, 1.37 | 61.49, 60.04, 60.69 | 3 / 3 |
| M2Pro | chainhash-256 | 22.37, 22.80, 22.76 | 73.25, 71.62, 70.70 | 3 / 2 |
| M2Pro | XXH3-64 | 13.05, 13.36, 13.03 | 25.07, 24.73, 25.11 | 1 / 1 |
| M2Pro | HalftimeHash-512 | 7.49, 7.60, 7.51 | 64.15, 62.30, 63.90 | 3 / 3 |
| M2Pro | HalftimeHash24-fixed | 5.76, 5.65, 5.66 | 47.79, 48.56, 49.11 | 3 / 2 |

Xeon8375C: start-load range **30.22–44.47**, end-load range **30.22–44.47**; 0 runs saw another SMHasher3 process at the end. UTC interval: 2026-09-18T21:21:26.535125+00:00 to 2026-09-18T21:29:58.333364+00:00.

M2Pro: start-load range **3.33–4.37**, end-load range **3.55–8.57**; 0 runs saw another SMHasher3 process at the end. UTC interval: 2026-09-18T22:19:01.811903+00:00 to 2026-09-18T23:11:34.881702+00:00.

Flags use `max/min − 1 >15%`, and separately any absolute deviation from the per-hash median >15%. Historical control differences >15% are flagged separately; different median/best selection and M2 clock calibration can affect those comparisons. All runs are retained.

**No >15% variation or historical-control flags.**

Xeon uses RDTSC/RDTSCP. M2 uses SMHasher3’s calibrated monotonic-clock cycle estimate, not measured hardware core cycles. Printed GiB/s assumes 3.5 GHz on both hosts and is not independently measured throughput. The original timer/test/control objects were reused; no timer, sample-count, buffer, or affinity changes were made.

## Verification

On **both hosts**, `--test=Sanity` passes the two basic sanity checks, prepend-zeroes, and both thread-safety checks. It **fails append-zeroes**, as expected for the shipped hash. The verification fingerprint is `0x8A105352`; its registration value remains zero, so that fingerprint check says `SKIP (unverifiable)`. SMHasher3 returns exit status zero despite the printed Sanity failure; this report does not call the suite a pass.

The standalone harness resolves `findHash("HalftimeHash24-shipped")` and calls the registered seed and native-hash callbacks. For **1,000 seeds** on each host it verifies: empty string and one zero byte collide through the registration; the direct full 24-byte values also agree; the registered value is exactly their first eight bytes; an output canary proves only eight bytes are written; all **9,000 expanded key words** match the fixed registration. Seed zero gives full output `30eda4f4a8d04716 bb3bb4eef446fd47 2d109b83c51db252` (three uint64 words) for both messages, on both hosts.

The key-free conclusion follows from the code, not from counting those seeds: both inputs produce the same all-zero padded tail block, with identical forest and entropy positions, and the shipped finalizer never incorporates total length. The fixed finalizer adds a length-dependent NH term and reserves separate tail entropy. The benchmark key adapter is copied verbatim apart from symbol names: four SplitMix64 words seed its w/x/y/z generator, ten rounds are discarded, and 9,000 words are stored in aligned thread-local memory. SMHasher3 calls `seedfn` outside the timed hash loop. The benchmark’s 64-bit seed family is not a claim of independent uniform entropy words.

## Why shipped and fixed differ at instruction level

**Xeon bulk:** both timed cores use ZMM integer arithmetic, and each 1344-byte leaf executes **27 `vpmuludq`**, **27 `vpaddd`**, and **27 `vpsrlq`** instructions. The shipped encoder retains a **26-iteration loop** at object offsets `0x600–0x627`: three `vpxord`, three vector register moves, decrement, branch per iteration. Including the other leaf XORs, that is **81 dynamic `vpxord` per leaf**, versus **49** in the repaired straight-line leaf. The repaired row-stride/capture logic removes that loop. This extra dependency/loop work explains the direction of the shipped bulk slowdown; the counts are disassembly evidence, not a hardware-counter allocation of every cycle. See [instruction audit](provenance/instruction-audit.json) and the actual [shipped](evidence/Xeon8375C/shipped.asm) / [fixed](evidence/Xeon8375C/fixed.asm) objects’ disassembly.

**M2 bulk:** the shipped `V4Scalar<7,3,9,3>` uses scalar `add w`, masks, shifts and `umull`/`umaddl` for the NH arithmetic, plus out-of-line encoder-lambda calls and stack traffic. The fixed `V4Sse2<7,3,9,3>` symbol is the header’s ARM **NEON** implementation: packed `add.4s`, `xtn`/`shrn`, `umull`/`umlal.2d` and `eor3.16b`. Thus the M2 gap includes loss of explicit SIMD as well as the encoder repair. Vector load/store instructions in the scalar object do not make its NH arithmetic the NEON path. See [shipped ARM disassembly](evidence/M2Pro/shipped.asm), [fixed ARM disassembly](evidence/M2Pro/fixed.asm), and the [literal NEON compilation errors](evidence/M2Pro/stock-neon-compile.txt).

**Small keys and output width:** 1–31-byte inputs skip the leaf encoder, so bulk’s encoder-loop explanation does not apply. Shipped omits the fixed hash’s three length-tag multiply/add terms and altered tail-key addressing. On M2 this saving competes with scalar lane arithmetic and stack traffic. The old fixed registration writes 24 bytes and shipped writes eight, but **both timed shipped objects call the full three-word core out of line and store all 24 bytes before copying the prefix**. The output projection has not optimized away two thirds of the hash. Shipped uses `-fwrapv` to give its signed SIMD reduction additions wrapping semantics; the header itself is untouched.

## Reproduction and provenance

The new source lives alongside the existing fixed registration in `../halftime-fixed/smhasher3-m2/hashes/halftimeshipped.cpp` and Xeon `<xeon-work>/speedbench-hhfixed/source/hashes/halftimeshipped.cpp`. Separate output binaries preserve the earlier binaries. [prepare_m2.py](bench/prepare_m2.py) compiles one new registration object and relinks the prior main/fixed/control/test objects. [prepare_xeon.sh](bench/prepare_xeon.sh) does the equivalent in the Xeon scratch tree. The only initial setup failure was the standalone Xeon verification harness’s static-library link order; `--start-group/--end-group` resolved it before verification or timing.

| Host | Timed binary | SHA-256 |
|---|---|---|
| Xeon8375C | `/home/thomas-ahle/agents/speedbench-hhfixed/build-shipped24/SMHasher3` | `71c7b7366ab1301e01d13ac17fbe42143f41baf89fb9bdbdd00d6509e4e7120d` |
| M2Pro | `<scratch>/codex/halftime24-shipped-timing/build-m2/SMHasher3` | `f93b38e394031b0f6bfabffbdfcefee7c8d5f6d43071f50e727cb59b26b6a99d` |

M2 compiler: Apple clang 17.0.0; Xeon: GCC 11.5.0. New registration flags include `-O3 -march=native -std=c++17 -fwrapv`; M2 retains the AES target feature used by the earlier build. The unchanged baseline binary hashes match the prior JSON: M2 `97c124f86a1f1e1dac50fecb4a289e9e4ca2744d302a11c38f042d47be296184`, Xeon `f92528649d7ba774e075505518a3e8321980146b8d92f7098b615a0e7f3a3a52`. Build commands, input hashes, load/process gates, raw output, timestamps and per-length timings are retained under `bench/`, `provenance/`, and `evidence/`.

The deliverable [speeds_halftime24_shipped.json](speeds_halftime24_shipped.json) follows the earlier `protocol` / `hosts` / `hashes` / `runs` schema, with explicit selected-run fields for the requested Xeon two-pass rule; median-run fields are null there. It also supplies `chart_points`, full-value versus registered widths, score scope, controls, and variation flags. Run `python3 bench/collect.py` to validate and regenerate it, then `python3 bench/report.py` to regenerate this report. Collection verifies exact run counts, gate conditions, raw hashes, unchanged timed binaries, both bulk sections, all 31 small lengths, and selection rules.
