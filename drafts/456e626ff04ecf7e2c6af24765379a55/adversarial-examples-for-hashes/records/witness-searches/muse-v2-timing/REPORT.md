# MuseAir v2 timing

Generated 2026-09-19T00:38:46.482118+00:00.

MuseAir algorithm v2 (crate `museair` 0.6.0) is registered as **MuseAir-v2**, with 64-bit output. All requested timings are complete: two Xeon passes and three gated M2 passes, including XXH3-64, rapidhash, chainhash-256 and the unchanged **MuseAir** v0.3 registration in each host’s timing binary. All five registrations pass full Sanity on both hosts.

| Host | Registration | Backend | Bulk B/cycle | Small cycles/hash | Runs |
|---|---|---|---:|---:|---:|
| Xeon8375C | MuseAir-v2 | scalar-mul128 | 7.53 | 29.27 | 2 |
| Xeon8375C | XXH3-64 | avx512 | 19.80 | 30.22 | 2 |
| Xeon8375C | rapidhash | generic | 10.69 | 27.70 | 2 |
| Xeon8375C | chainhash-256 | hwclmul | 14.44 | 103.98 | 2 |
| Xeon8375C | MuseAir | generic | 7.82 | 19.84 | 2 |
| M2Pro | MuseAir-v2 | scalar-mul128 | 10.42 | 22.32 | 3 |
| M2Pro | XXH3-64 | neon | 13.04 | 25.18 | 3 |
| M2Pro | rapidhash | generic | 15.69 | 20.88 | 3 |
| M2Pro | chainhash-256 | hwpmull | 22.08 | 73.77 | 3 |
| M2Pro | MuseAir | generic | 9.82 | 25.86 | 3 |

| Host | v2/v0.3 bulk | v2/v0.3 small latency | >15% version difference |
|---|---:|---:|---|
| Xeon8375C | 0.9629 (-3.71%) | 1.4753 (+47.53%) | YES |
| M2Pro | 1.0611 (+6.11%) | 0.8631 (-13.69%) | no |

Higher bulk is better; lower small-key latency is better. Version differences compare different algorithms and are separate from repeatability/control flags.

## Protocol

Commands are exactly `SMHasher3 NAME --test=Speed`, with no affinity or priority adjustment. Each pass runs MuseAir-v2, XXH3-64, rapidhash, chainhash-256, then MuseAir. Bulk is the **fixed 262144-byte Average** over alignments; small is the **Average over lengths 1–31**. The varying-length bulk results are preserved in the JSON but never selected for the headline.

Xeon independently selects higher bulk and lower small from two passes. Rounded bulk ties select higher GiB/s, then the earlier run. M2 independently selects the median of three for each metric, retaining GiB/s from the selected bulk run.

Every M2 preparation/timing start requires no other SMHasher3 process and one-minute load **<4.5**, with 60-second polling and no timeout bypass. Five-second process/load monitoring archives and retries any overlapping timing run. The load gate is a start condition, not a sustained-load condition. Xeon also waits for no other SMHasher3 process. This task ran no search or substantial collision scan on the Mac: local work was file editing, brief gated compilation/verification and gated timing; the 2^30-seed scan ran on Xeon before timing.

The unchanged harness uses TSC ticks on Xeon and a calibrated monotonic-clock cycle estimate on M2; these are not interchangeable physical core-cycle counts. GiB/s is SMHasher3’s conversion assuming 3.5 GHz on both hosts, not independently measured throughput.

## Registration and correctness

`bench/museair_v2.cpp` calls the supplied validated `museair2_hash(bytes, length, uint64_t(seed), 0)`, equivalent to the crate’s standard **`museair::hash(bytes, seed)`**. SMHasher3’s 64-bit seed maps identically to the crate’s single 64-bit seed: no expansion, random secret, truncation, zero-seed exclusion or seed hook. The final argument fixes **BFast=false**. The crate’s ordinary seed mixing remains inside the timed call. This measures the supplied C/C++ port, not Rust code generation. Both callback slots expose the same little-endian byte function; no big-endian-host claim is made.

The supplied audit identifies tag `crate-0.6.0` = `f3092ae` and upstream `src/lib.rs` SHA-256 `123772c6360a31ef29f5d525d87d8f7738c8e8d883b284a6b20dd29fb1d42b1c`. The port and decompressed vector hashes are recorded in each host’s `build-sha256.json`.

Both hosts reproduce all **200,000 crate-generated vectors**, each at offsets 0, 1 and 7 (**600,000 registered calls**, zero mismatches, lengths 0–511). Verification uses `findHash("MuseAir-v2")`, `HashInfo::Seed` and the actual registered callback; it is relinked against the final registration object used by the timing binary. Native verification is **0x7140cabc** on both hosts. Full Sanity passes: implementation verification, both sanity checks, append/prepend-zeroes and both thread-safety checks.

The audited 32-byte pair is:

```
M  = 0000000000000000000000000000000000000000000000000000000000000000
M2 = 00000000000000404a048402a910048a00000000000000a80000000000000000
```

A fresh Xeon sample through the registration gives **6,028 / 1,073,741,824 = 2^-17.443**, with Poisson 95% interval **[2^-17.479, 2^-17.406]**. This reproduces the audited 2^-17.45 rate. Seeds are successive SplitMix64 outputs from initial state `0x763220260919`; complete 64-bit outputs are compared. The approximate Poisson interval uses Wilson–Hilferty. This is a measured rate, not an exact probability or key-free collision.

Both hosts reproduce the known collision seeds `040963434fe5e368` → `ef46cda19c0bbc67` and `acac861647d5f7f2` → `b1f37635e72a255f`, and confirm seed zero does not collide. The M2 rate claim transfers through the shared implementation and crate-vector equality; a separate large M2 seed scan was deliberately omitted to keep the Mac idle. The prior independent Rust audit and its logs remain in `muse-v2/independent-verify/`.

## Variation and controls

Flags use **>15%**, strictly: Xeon two-run max/min spread; M2 each run’s absolute deviation from its three-run median. Controls, including old MuseAir, are also compared against the prior host baseline (`../speedbench/speeds.json` for Xeon, `../m2-rerun/speeds_m2_v2_final.json` for M2). Baseline file hashes and values are recorded in the JSON.

| Host | Registration | Bulk run spread | Small run spread | Run flag |
|---|---|---:|---:|---|
| Xeon8375C | MuseAir-v2 | 0.00% | 0.10% | no |
| Xeon8375C | XXH3-64 | 1.12% | 0.03% | no |
| Xeon8375C | rapidhash | 0.00% | 0.07% | no |
| Xeon8375C | chainhash-256 | 0.56% | 0.03% | no |
| Xeon8375C | MuseAir | 0.13% | 0.15% | no |
| M2Pro | MuseAir-v2 | 0.48% | 3.20% | no |
| M2Pro | XXH3-64 | 0.85% | 0.72% | no |
| M2Pro | rapidhash | 0.19% | 0.19% | no |
| M2Pro | chainhash-256 | 3.78% | 2.71% | no |
| M2Pro | MuseAir | 3.06% | 3.10% | no |

| Host | Control | Bulk new/old | Small new/old | >15% baseline flag |
|---|---|---:|---:|---|
| Xeon8375C | XXH3-64 | 1.0056 | 1.0020 | no |
| Xeon8375C | rapidhash | 1.0019 | 1.0044 | no |
| Xeon8375C | chainhash-256 | 1.0014 | 1.0008 | no |
| Xeon8375C | MuseAir | 1.0000 | 0.9965 | no |
| M2Pro | XXH3-64 | 0.9909 | 1.0273 | no |
| M2Pro | rapidhash | 1.0439 | 0.9905 | no |
| M2Pro | chainhash-256 | 0.9684 | 1.0465 | no |
| M2Pro | MuseAir | 0.9618 | 1.0491 | no |

**No >15% run-deviation or control-baseline flags.**

Xeon8375C: 10 accepted runs; start load1 10.93–19.54; boundary load1 6.27–19.54; all sampled load1 6.27–19.54; 0 overlapping attempts excluded.

M2Pro: 15 accepted runs; start load1 3.34–4.49; boundary load1 3.34–9.01; all sampled load1 3.24–9.36; 0 overlapping attempts excluded.

## Build and evidence

The scratch trees are `./smhasher3-m2` and `/home/thomas-ahle/agents/speedbench-museair-v2/source`, copied from the earlier M2/Xeon source trees. The builds reuse `../m2-rerun/build-fixed` and `/home/thomas-ahle/agents/speedbench/build-release-20260917` main, test and hash-library objects. Only the new registration translation unit is added, compiled with C++17, `-O3 -march=native -DNDEBUG -DHAVE_THREADS` (plus the prior M2 AES target option). The source is also appended to each scratch `hashes/Hashsrc.cmake`. Existing harness, timer and control objects are unchanged.

| Host | Binary | SHA-256 |
|---|---|---|
| Xeon8375C | `/home/thomas-ahle/agents/speedbench-museair-v2/build/SMHasher3` | `3346f0ca2bf43422b3ef4f8741eda9968804095022cf368924abeaeafb640886` |
| M2Pro | `/private/tmp/claude-501/-Users-ahle-repos-fast-polynomials/624f2aa7-83b9-480b-aeba-96fbb5117dcd/scratchpad/codex/museair-v2-timing/build-m2/SMHasher3` | `b20d1938180dbd9ae00b319e3c490ab3f84557a2a6365358e66bda1b4cddeca4` |

`speeds_museair_v2.json` preserves the earlier nested `hash → host → metrics/runs` schema. `evidence/<host>/` contains every raw Speed/Sanity log, complete execution and gate records, verification output, registration constants, compile/link commands, source/object/binary and reused-input hashes, compiler/host details, and disassembly. The supplied port and vectors remain unchanged in `muse-v2/`.

Reproduce with `python3 bench/prepare_host.py` on each host after transferring this directory; this includes preparation, verification, Sanity and the required timing passes. Copy the Xeon evidence directory back, then run `python3 bench/collect.py`, `python3 bench/report.py` and `python3 bench/validate_results.py`. Existing timing execution files allow resuming only with the same binary hash; use fresh evidence directories for a new measurement.
