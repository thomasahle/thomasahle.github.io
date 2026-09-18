# SMHasher3 two-host speed report

Completed both passes on both hosts.

Both hosts use the same current working-tree source snapshot. The rebuilt M2 binary and the Xeon binary register matching ChainHash descriptions: strided word pairing and a degree-5 finalizer behind an additive twist. The hardware backends differ (`hwpmull` on M2, `hwclmul` on Xeon). All final M2 measurements use the new binary. The superseded degree-7 M2 measurements, report, and provenance are preserved in `out/M2Pro_oldbinary/` and excluded from the final columns.

Source HEAD for both builds: `3de870c7ab449ad11cf450848d9270e3f54102d1`. The tree is dirty; the commit alone does not identify these builds. Full read-only status is in `provenance/tree-status.txt`, tracked changes in `provenance/tree-diff.patch`, and all 795 transferred file hashes (including untracked ChainHash files) in `provenance/source-sha256.json`. The current M2 source was checked against the Xeon snapshot before rebuilding; no changed, missing, or added source files were found. No git write commands or source edits were performed. The new M2 build writes only to `build-speed`; `build-advtest` is preserved.

| Host | Build directory | Compiler | Binary SHA-256 |
|---|---|---|---|
| M2Pro | `/Users/ahle/repos/smhasher3/build-speed` | Apple clang version 17.0.0 (clang-1700.6.4.2) | `33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541` |
| Xeon8375C | `/home/thomas-ahle/agents/speedbench/build-release-20260917` | c++ (GCC) 11.5.0 20240719 (Red Hat 11.5.0-14) | `03c2ff9f38cc564400eeb1906da90cb74ea6efcff2bea6c660626fdbe1571a99` |

M2 build: `nice -n 10 cmake -S /Users/ahle/repos/smhasher3 -B /Users/ahle/repos/smhasher3/build-speed -DCMAKE_BUILD_TYPE=Release -DENDIAN_DETECT_BUILDTIME=OFF "-DCMAKE_CXX_FLAGS=-Xclang -target-feature -Xclang +aes"`, followed by `nice -n 10 cmake --build /Users/ahle/repos/smhasher3/build-speed --target SMHasher3 -j8`. This was a new build directory. The first configure attempt encountered a missing `Modules/TestEndianess.c.in` in CMake 4.1.2; the successful configuration uses the existing M2 build’s runtime-endianness setting and explicit AES target flag, without changing source. Both attempts and the build log are retained as `provenance/M2Pro-cmake-*.log`.

Xeon build: the working tree was transferred with rsync, excluding `build*/` and `.git`, to `/home/thomas-ahle/agents/speedbench/source`. It used `nice -n 10 cmake -S source -B build-release-20260917 -DCMAKE_BUILD_TYPE=Release`, then `nice -n 10 cmake --build build-release-20260917 --target SMHasher3 -j48`. Its `master-unknown` version banner reflects the omitted `.git`; the source manifest and dirty status identify the build. All 110 Xeon runs were already complete and are retained. Remote binary, source, registrations, and every raw-output hash were rechecked in `provenance/Xeon8375C-recheck.json`. Configure/build logs, both hosts’ CMake caches, flags, compiler versions, generated platform headers, and timer implementations are retained in provenance.

Apple M2 Pro: ProductName:		macOS; ProductVersion:		27.0; BuildVersion:		26A428. Intel Xeon Platinum 8375C @ 2.90 GHz: 96 logical CPUs, 48 cores, two sockets, KVM guest; Red Hat Enterprise Linux 9.8. Full host details are in `provenance/*-cpu.txt`, `*-os.txt`, and `*-uname.txt`.

Each host runs one benchmark process at a time, in two full passes through the manifest. Commands use exactly `<binary> <name> --test=Speed`, with the binary’s defaults. No CPU affinity, frequency governor, or process priority changes were applied to timings. `nice` was used for building.

The parser extracts the Average of the fixed 262144-byte bulk test and the Average over small lengths 1..31. The varying-length bulk section is retained but does not supply the headline result. Higher bulk bytes/cycle wins, with GiB/sec taken from that same run; lower small cycles/hash wins independently. Equal rounded bulk bytes/cycle is resolved by higher reported GiB/sec, with remaining ties selecting run 1. `bulk_selected_run` and `small_selected_run` identify both choices. Raw `<name>.run1.txt` and `.run2.txt` preserve merged stdout/stderr; `<name>.txt` concatenates both with run markers.

Registration coverage: M2 has 309 names and Xeon has 338. Full lists are not identical across architectures. The exact comparison and all ChainHash descriptions are in `provenance/registration-comparison.json`.

Names registered on Xeon but absent on M2: `AquaHash`, `CLhash`, `CLhash.bitmix`, `CityHashCrc-128.seed1`, `CityHashCrc-128.seed2`, `CityHashCrc-128.seed3`, `CityHashCrc-256`, `FarmHash-32.NT`, `FarmHash-32.SA`, `FarmHash-32.SU`, `FarmHash-64.TE`, `MeowHash`, `MeowHash.32`, `MeowHash.64`, `MetroHashCrc-128.var1`, `MetroHashCrc-128.var2`, `MetroHashCrc-64.var1`, `MetroHashCrc-64.var2`, `UMASH-128`, `UMASH-128.reseed`, `UMASH-64`, `UMASH-64.reseed`, `aesnihash-majek`, `aesnihash-peterrk`, `falkhash1`, `falkhash2`, `injective-hash.clhash`, `t1ha0.aesA`, `t1ha0.aesB`.

Names registered on M2 but absent on Xeon: none.

Requested names not registered on M2Pro: CLhash, UMASH-64, UMASH-128, UMASH-64.reseed.
Requested names not registered on Xeon8375C: none.

CLhash and UMASH registrations are conditional on `HAVE_X86_64_CLMUL`; no substitute implementations were used. Newly present M2 registrations compared with the old binary: chainhash-g4-256, chainhash-g5-1k, chainhash-g5-256. These additional registrations are recorded for provenance; the requested timing manifest is unchanged.

Name resolution: `mir` → `mir.exact`; `mum3` → all four `mum3.exact.unroll1..4`; `mx3` → all three `mx3.v1..v3`; `nmhash32` → `NMHASH`; `nmhash32x` → `NMHASHX`; `fasthash32/64` → `fasthash-32/64`. `CLhash` and `UMASH-*` preserve registered casing. UMASH-64.reseed is included because the table notes mention it. Registered komihash is v5.27; no separate v5.34 registration exists.

Paper rows without a matching registration:

- Horner / unrolled, GF(2^64).
- BRW, GF(2^64).
- Injective recurrence, one chain.
- Injective recurrence, eight lanes.
- Carryless NH (CLNH).
- NH with 64-bit words.
- Vector multiply-shift.
- Simple tabulation.

The Horner row includes sequential and unrolled GF(2^64) variants; neither is registered. The `injective-hash.mum`, `.mersenne`, and Xeon `.clhash` registrations are different constructions from the one-chain/eight-lane GF(2^64) paper rows. `multiply-shift` wraps its finite key array and `tabulation-64` is a composite string hash; their requested measurements are not assigned to the ideal paper families.

Load conditions:

- M2Pro: 102/102 runs completed; sampled 1-minute load range at run boundaries: [2.85, 6.25].
- Xeon8375C: 110/110 runs completed; sampled 1-minute load range at run boundaries: [2.16, 18.34].

M2 waits before starting the two passes, polling every 60 seconds for 1-minute load < 3.0, for up to 3 hours (10800 seconds). The gate is a start condition; later load is recorded before and after each run.

M2 met the load gate before starting.

M2 start gate: `{"timestamp": "2026-09-18T00:33:41.392206+00:00", "uptime": "2:33  up 14:25, 1 user, load averages: 2.95 5.23 4.98", "load1": 2.95, "load5": 5.23, "load15": 4.98, "elapsed_seconds": 240.10624054200002, "threshold": 3.0, "deadline_seconds": 10800, "poll_seconds": 60, "timeout": false}`.

Timing qualifications: SMHasher3 hard-codes a **3.5 GHz** multiplier for every reported GiB/sec line. These are assumed-clock conversions, not observed throughput, including on the nominal 2.90 GHz Xeon. The M2 timer estimates cycles using a per-process calibrated monotonic clock; Xeon uses RDTSC/RDTSCP. The two hosts’ cycle units are not interchangeable physical core-cycle measurements. M2 process-to-process calibration can add variation; its output does not print the calibration multiplier. Architecture-specific SIMD/AES/PMULL/CLMUL or portable backends can also differ. No frequency corrections or extra runs were applied.

Two-run variation greater than 10% (larger divided by smaller, minus one):

None observed among completed pairs.

M2 boundary load reached 6.25; the measurements must not be presented as taken under sustained quiet conditions. Best-of-two selection retains the requested protocol, with per-run results available for assessing variability.

Observed clock labels: {"3.5 ghz": 212}.

Validation checks every run for exit status, both bulk Average sections, the small-key Average, raw-file SHA-256, exact run numbers, binary path/hash consistency, and correct independent best-of-two selection. Parse errors: []. `speeds.json` retains all per-run timestamps, loads, parsed sections, and selected runs.

The transferred source archive `provenance/source-snapshot.tar.gz` contains all 795 files from the manifest. Its SHA-256 and verification are in `provenance/source-archive-verification.json`. Source commit, status, and file hashes are compared again during finalization.

Final source comparison: {"commit_unchanged": true, "status_unchanged": true, "source_files_changed_since_snapshot": [], "source_files_added_since_snapshot": [], "oldbinary_unchanged": true}.
