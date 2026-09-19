# foldhash 0.2.0: verified C++ port and SMHasher3 measurements

Updated 2026-09-18T14:59:46.931867+00:00. All requested measurements and full suites are complete.

The checked-out tag is `v0.2.0`, commit `8f878c636fda9c9e93384824ea45e06d03f009f5`, from [orlp/foldhash](https://github.com/orlp/foldhash/tree/8f878c636fda9c9e93384824ea45e06d03f009f5). The fetched master is `77d8e3dae0d43abe65753e888b9e74efbe4ee3f7`. `provenance/version.json` verifies that all four supplied Rust files match this master. `provenance/master.diff` contains the full tag-to-master source diff: only the conditional random-seed initialization for no-std platforms without pointer atomics changed. The byte hashing, explicit seed expansion, fast finish, and quality finish are identical. No upstream checkout was patched.

`foldhash.cpp` is the single-file C++ translation, carrying the original Zlib notice and explicitly marked as an altered port. The names are `foldhash-fast` and `foldhash-quality`, both 64-bit. The native 64-bit little-endian path is the Rust path on both measured hosts. SMHasher3’s swapped-input/output path also has registered verification constants; it is not a claim that a big-endian Rust host was tested. The 32-bit Rust fallback algorithm is outside this port’s scope.

The registration emulates exactly `FoldHasher::with_seed(S, &SharedSeed::from_u64(S)); h.write(bytes); h.finish()`. This is one raw Hasher byte write, with no prefix, suffix, or integer sponge contents. It is the byte hashing component used for byte-vector and string keys, but **is not the complete stable Rust `Hash` call sequence for either key type**. Calling the whole sequence equivalent would be incorrect.

| Key/call path | Sequence on tested stable Rust | Relationship to registration |
|---|---|---|
| Raw bytes | `write(bytes); finish()` | Exact match |
| `Vec<u8>` / `[u8]` / `&[u8]` key | `write_usize(len); write(bytes); finish()` | Length is buffered in the integer sponge; finish additionally computes `fold(raw ^ len, shared[0])` on these 64-bit hosts |
| `String` / `str` / `&str` key, stable default features | `write(bytes); write_u8(0xFF); finish()` | Terminator is buffered in the integer sponge; finish additionally computes `fold(raw ^ 255, shared[0])` |
| String with foldhash’s `nightly` feature | Foldhash overrides `write_str` to call only `write(s.as_bytes())` | Matches raw registration, by source inspection; no nightly toolchain run |

For quality, apply its final `fold(fast_finish, 0x243f6a8885a308d3)` after any sponge flush. `raw` in the table means the fast raw result. The integer write is not concatenation to the byte buffer: `write(bytes || [0xFF])` is a different operation. Rust’s [Hasher implementation](https://doc.rust-lang.org/src/core/hash/mod.rs.html) supplies the default string terminator and length-prefix behavior; the pinned foldhash overrides are in `src/fast.rs:113–132` and `src/quality.rs:66–75`. The vector generator independently asserts the manual sequence against actual `Hash` on `Vec<u8>` and `String`. These are key-hashing equivalences, not measurements of allocation, lookup, or complete HashMap operations.

For the requested SMHasher3 seed mapping, let `F(a,b) = low64(a*b) XOR high64(a*b)` and `x0 = S`. For `i=1..6`, `xi = F(F(F(x(i-1), C), C), C)` with `C=0xbe5466cf34e90c6c`; shared word `i-1` is `xi OR 0x8000000080000001`. The forced bits are applied only to the stored word, not fed back into the next mixing chain. The per-hasher accumulator is `S`, rotated right by the input length modulo 64 before hashing. No `FixedState`/`SeedableRandomState` ARBITRARY3 XOR or quality builder seed pre-mix is applied: these are direct `FoldHasher::with_seed` registrations. The expensive expansion runs in the thread-local `seedfn`, outside the timed call, matching the style of `hashes/rust-ahash.cpp`. The timed function loads the prepared state and hashes the bytes. RandomState construction and seed expansion are therefore excluded from the reported hash speeds. This deterministic one-dimensional seed family differs from independently chosen per-hasher and shared random states.

`rust-vectors/` is the Cargo oracle program, with a path dependency on the unmodified pinned checkout. Cargo and rustc were already installed on the Xeon (rustc 1.92.0); no rustup installation or sudo was needed. `vectors.json` has 26,040 records and 52,080 expected 64-bit hashes. Each record stores the seed pair, pattern, length, mode and both hashes as fixed-width hexadecimal strings. All lengths 0..300 and 511, 512, 513, 1023, 1024, 1025, 4096, 65536, 262144 are covered. Pattern 0 is `byte[i]=(131*i+17*(i>>3)+29)&255`, pattern 1 is all zeroes, pattern 2 is ASCII `a+(i%26)`. Per-hasher seeds are `0`, `1`, `ffffffffffffffff`, `0123456789abcdef`, `8000000000000000`, `deadbeefcafebabe`; each is paired with shared input `S` and `S XOR a5a5a5a5a5a5a5a5`. Raw-write and Vec Hash modes use every pattern; String Hash uses the ASCII pattern. There are 11,160 raw-write, 11,160 vector-key, and 3,720 string-key records. The public API has no six-word constructor and its `seeds` field is `pub(crate)`, so explicit six-word injection is unavailable without changing upstream source or using an unsupported representation trick; neither was used.

Xeon C++ with SMHasher3 Platform/Mathmult headers: **PASS**, 52,080 comparisons. Evidence: `provenance/xeon/equivalence-integrated-xeon.json`.
M2 C++ with SMHasher3 Platform/Mathmult headers: **PASS**, 52,080 comparisons. Evidence: `provenance/equivalence-m2.json`.

AddressSanitizer and UndefinedBehaviorSanitizer also passed the complete 52,080-output corpus using clang++ on the Xeon; see `provenance/xeon/equivalence-sanitized-xeon.json`. The system GCC sanitizer link failed because its libasan runtime was missing; clang’s runtime was available.

The Rust oracle also independently reproduces the native SMHasher3 verification algorithm (`cargo run --release --manifest-path rust-vectors/Cargo.toml -- --verify`), including the final 2,048-byte digest aggregation. Thus the LE constants are not merely copied from the port’s own output. The standalone and integrated C++ vector programs share the port’s byte hashing functions. `verify_vectors.py` checks every record, not just an aggregate checksum.

| Registration | LE verification (also independently Rust-derived) | BE verification (C++ swapped path) |
|---|---|---|
| foldhash-fast | `0xA167BA5E` | `0x71AF646C` |
| foldhash-quality | `0xE269316E` | `0xE43F6B30` |

The inherited SMHasher3 base commit recorded by the preceding benchmark is `3de870c7ab449ad11cf450848d9270e3f54102d1`, with its existing local registrations retained; `provenance/smhasher-base.json` preserves the base tree status. The Mac is an Apple M2 Pro (Darwin 27.0.0, Apple clang 17.0.0); the server is an Intel Xeon Platinum 8375C (48 physical cores / 96 logical CPUs, Linux 5.14, GCC 11.5.0). Control sources on the copied Xeon tree exactly match the originals, as recorded in `provenance/xeon/hash-source-comparison.json`.

The Mac source target is `../m2-rerun/smhasher3`; its new build is `./build-m2` to preserve the prior timing binary. Xeon source was copied from `<xeon-work>/speedbench` to `<xeon-work>/speedbench-foldhash`, with a fresh `build-foldhash` build. Only the copied Xeon tree is modified. The hash source is added to `hashes/Hashsrc.cmake` in each target. Both use Release and inherited `-O3 -march=native`; the Mac retains the previous ARM64 detector correction, explicit AES target feature, little-endian configuration and OpenSSL root. `provenance/mac-configure-command.json`, `provenance/mac-build.txt`, and `provenance/xeon/{configure,build,compiler,host}.txt` record the actual builds. Benchmark loops and timers were not edited.

The Mac job polls every 60 seconds and requires both no running SMHasher3 process (also waits for the known previous timing runner to exit, avoiding gaps between its runs) and one-minute load <3.0 before building; it repeats the gate before timing. There is no timeout override. `provenance/mac-gate.jsonl` records the observations. The Xeon timing runner samples `/proc/stat` for three seconds and selects the core maximizing the minimum idle fraction across SMT siblings; it waits until both siblings are at least 95% idle, then pins all Sanity and Speed invocations with `taskset -c CORE`. No low aggregate-load threshold was specified for the many-core Xeon in the earlier protocol. The full test suites run separately, sequentially, with `nice -n 10` and `--ncpu=7`, with no `--test` flag. The busy Xeon initially had no idle core, so full suites run before the idle-gated Speed passes. Their embedded speed sections are not used for the reported timings.

M2Pro: 8/8 Speed runs completed. Boundary load1 range 2.84–7.72.
M2 timing gate: `{"timestamp": "2026-09-18T13:52:35.233175+00:00", "uptime": "15:52  up 1 day,  3:44, 1 user, load averages: 2.84 3.97 4.29", "load1": 2.84, "load5": 3.97, "load15": 4.29, "elapsed_seconds": 0.005834833000000206, "threshold": 3.0, "deadline_seconds": null, "poll_seconds": 60, "timeout": false, "active_SMHasher3_pids": []}`.
Xeon8375C: 8/8 Speed runs completed. Boundary load1 range 4.40–21.41.
Xeon selected CPU 4, SMT siblings [4, 52], sampled idle fraction 1.0000.

`speeds_foldhash.json` follows the previous table schema: hash → host → `bulk_bytes_per_cycle`, `bulk_gib_s`, `small_cycles`, `runs`, registration/backend and verification. The headline bulk value selects the better of two fixed 262144-byte Average results across alignments; the small result independently selects the lower Average for 1..31 bytes. Run spread and selected run numbers are recorded. A blank backend token denotes the generic registration. Structural sanity failures are recorded separately from correct implementation verification.

| Host | Hash | Bulk bytes/cycle | Small cycles/hash | Bulk spread | Small spread |
|---|---|---:|---:|---:|---:|
| M2Pro | foldhash-fast | 14.24 | 14.94 | 0.85% | 0.27% |
| M2Pro | foldhash-quality | 14.42 | 18.92 | 0.49% | 1.16% |
| M2Pro | komihash | 8.31 | 24.16 | 2.21% | 1.45% |
| M2Pro | rapidhash | 16.02 | 20.15 | 2.69% | 1.34% |
| Xeon8375C | foldhash-fast | 9.07 | 20.09 | 0.00% | 0.05% |
| Xeon8375C | foldhash-quality | 9.07 | 25.28 | 0.00% | 0.00% |
| Xeon8375C | komihash | 7.34 | 26.76 | 0.00% | 0.07% |
| Xeon8375C | rapidhash | 10.69 | 27.60 | 0.00% | 0.58% |

Control ratios below are new / existing table: bulk >1 means higher throughput, small >1 means more cycles. M2 uses `../m2-rerun/speeds_m2_v2.json` when present, otherwise `../speedbench/speeds.json`; Xeon uses `../speedbench/speeds.json`. The JSON records baseline values and the file SHA-256 used. The Mac baseline itself was still running when this work began; each ratio records the baseline entry status actually observed.

| Host | Control | Bulk ratio | Small-cycle ratio |
|---|---|---:|---:|
| M2Pro | komihash | 0.9443 | 1.0967 |
| M2Pro | rapidhash | 0.9950 | 1.0030 |
| Xeon8375C | komihash | 0.9986 | 0.9734 |
| Xeon8375C | rapidhash | 1.0019 | 1.0007 |

M2 cycle figures are calibrated monotonic-clock estimates and Xeon figures are TSC ticks, not directly interchangeable physical CPU cycles. SMHasher3 prints GiB/sec using a hard-coded 3.5 GHz assumption on both. The gates establish start conditions; boundary loads and control/run ratios indicate subsequent variability. These are C++ port timings, not Rust compiler code-generation measurements.

The final suite command uses `--ncpu=7`: the harness launches that many workers while retaining a waiting main thread, so this stays within eight total threads. `provenance/xeon/thread-monitor.json` records one-second samples: foldhash-fast max 8 total threads, nice 10, foldhash-quality max 8 total threads, nice 10. An initial eight-worker run was interrupted and archived as diagnostic output; it is excluded from all final verdict counts.

The full-suite verdict uses the exact final summary counters, in the form used by `results/raw/` and `results/README.md`; these are test-case counts, not a count of PASS/FAIL words. A normal exit code alone does not establish success. `verdict.json` retains commands, timestamps, binary hashes, complete failure lists and raw-output hashes.

| Failed test family | Fast failed cases | Quality failed cases |
|---|---:|---:|
| Sanity | 1 | 1 |
| Avalanche | 12 | 3 |
| BIC | 4 | 2 |
| Zeroes | 1 | 1 |
| Cyclic | 1 | 0 |
| Sparse | 26 | 26 |
| Permutation | 14 | 14 |
| Text | 28 | 6 |
| TwoBytes | 6 | 6 |
| PerlinNoise | 1 | 1 |
| Bitflip | 3 | 3 |
| SeedZeroes | 2 | 2 |
| SeedBlockLen | 16 | 12 |
| SeedBlockOffset | 6 | 6 |

Detailed case labels below are retained from the harness summary.

**foldhash-fast: FAIL — 79/200 passed, 121/200 failed.**
Ran 2026-09-18T11:46:14.770705+00:00 to 2026-09-18T12:02:15.126123+00:00; load1 132.13 → 215.61. Raw log: `out/full/foldhash-fast.txt`.

- Sanity: Basic 2
- Avalanche: 3, 4, 5, 6, 7, 8, 9, 10, 12, 16, 20, 128
- BIC: 3, 8, 11, 15
- Zeroes: unnamed case (`[]` in the summary)
- Cyclic: 4 cycles of 4 bytes
- Sparse: 6/2, 4/3, 4/4, 4/5, 3/6, 3/7, 3/8, 3/9, 3/10, 3/12, 3/14, 10/2, 20/3, 9/4, 5/9, 4/14, 4/16, 3/32, 3/48, 3/64, 3/96, 2/128, 2/256, 2/512, 2/1024, 2/1280
- Permutation: 4-bytes [3 low bits; LE], 4-bytes [3 low bits; BE], 4-bytes [3 high bits; LE], 4-bytes [3 high bits; BE], 4-bytes [3 high+low bits; LE], 4-bytes [3 high+low bits; BE], 4-bytes [0, low bit; LE], 4-bytes [0, low bit; BE], 4-bytes [0, high bit; LE], 4-bytes [0, high bit; BE], 8-bytes [0, low bit; LE], 8-bytes [0, low bit; BE], 8-bytes [0, high bit; LE], 8-bytes [0, high bit; BE]
- Text: dictionary, numbers without commas, numbers with commas, FXXXXB, FBXXXX, XXXXFB, FooXXXXBar, FooBarXXXX, XXXXFooBar, FooooXXXXBaaar, FooooBaaarXXXX, XXXXFooooBaaar, FooooooXXXXBaaaaar, FooooooBaaaaarXXXX, XXXXFooooooBaaaaar, FooooooooXXXXBaaaaaaar, FooooooooBaaaaaaarXXXX, XXXXFooooooooBaaaaaaar, FooooooooooXXXXBaaaaaaaaar, FooooooooooBaaaaaaaaarXXXX, XXXXFooooooooooBaaaaaaaaar, Words alnum 1-4, Words alnum 5-8, Words alnum 1-16, Words alnum 1-32, Long alnum last 1968-2128, Long alnum last 4016-4176, Long alnum last 8112-8272
- TwoBytes: 20, 32, 48, 1024, 2048, 4096
- PerlinNoise: 2
- Bitflip: 3, 4, 8
- SeedZeroes: 1280, 8448
- SeedBlockLen: 8, 9, 10, 11, 12, 13, 14, 15, 16, 25, 26, 27, 28, 29, 30, 31
- SeedBlockOffset: 0, 1, 2, 3, 4, 5

**foldhash-quality: FAIL — 117/200 passed, 83/200 failed.**
Ran 2026-09-18T12:02:15.129999+00:00 to 2026-09-18T12:20:54.597047+00:00; load1 215.61 → 215.70. Raw log: `out/full/foldhash-quality.txt`.

- Sanity: Basic 2
- Avalanche: 3, 4, 8
- BIC: 3, 8
- Zeroes: unnamed case (`[]` in the summary)
- Sparse: 6/2, 4/3, 4/4, 4/5, 3/6, 3/7, 3/8, 3/9, 3/10, 3/12, 3/14, 10/2, 20/3, 9/4, 5/9, 4/14, 4/16, 3/32, 3/48, 3/64, 3/96, 2/128, 2/256, 2/512, 2/1024, 2/1280
- Permutation: 4-bytes [3 low bits; LE], 4-bytes [3 low bits; BE], 4-bytes [3 high bits; LE], 4-bytes [3 high bits; BE], 4-bytes [3 high+low bits; LE], 4-bytes [3 high+low bits; BE], 4-bytes [0, low bit; LE], 4-bytes [0, low bit; BE], 4-bytes [0, high bit; LE], 4-bytes [0, high bit; BE], 8-bytes [0, low bit; LE], 8-bytes [0, low bit; BE], 8-bytes [0, high bit; LE], 8-bytes [0, high bit; BE]
- Text: dictionary, numbers without commas, numbers with commas, Words alnum 1-4, Words alnum 1-16, Words alnum 1-32
- TwoBytes: 20, 32, 48, 1024, 2048, 4096
- PerlinNoise: 2
- Bitflip: 3, 4, 8
- SeedZeroes: 1280, 8448
- SeedBlockLen: 8, 9, 10, 11, 12, 13, 14, 15, 16, 29, 30, 31
- SeedBlockOffset: 0, 1, 2, 3, 4, 5

These are the supplied modified SMHasher3 scratch trees, not pristine upstream checkouts. The Xeon copy retains the pre-existing SeedDifferential test extension, whose default tier also ran. `provenance/xeon/source-manifests.json` and `source-diff.json` compare every source-tree file with the original `<xeon-work>/speedbench/source`: only `hashes/foldhash.cpp` and its `hashes/Hashsrc.cmake` entry differ. The relevant suite sources are archived under `provenance/xeon/source-evidence/`. The verdict and denominator refer to this exact default test configuration.

The requested no-`--test` invocation is the full **default** suite. It reports 200 cases here; the denominator is not assumed to be the 250 seen in many repository result files. For example, the repository aHash result includes 24–56, 160 and 192-byte Avalanche cases that the current source enables under `--extra`; the requested invocation does not enable those extended cases. The opt-in `BadSeeds` test is also disabled in `main.cpp`’s default test table. No default test was explicitly disabled, and failures did not stop the remaining tests.

Both preliminary Sanity runs fail “sanity check 2”, while their implementation verification passes. The requested seed mapping includes per-hasher seed zero. For a 2-byte input whose first byte is zero, the raw fast short path has a zero first multiplicand, so the result is zero regardless of the second byte; the quality final multiply preserves zero. This is a property of the requested direct seeded raw-byte path, not evidence of a translation mismatch. No bad seed is excluded and no test failure is suppressed. The full-suite verdict should not be generalized to a differently seeded Rust RandomState or to a different key call sequence.

The following quotes are verbatim from the supplied `README.md`, with original line numbers. The pinned README is compared in provenance; spelling such as “quadratric” is preserved.

```text
3: This repository contains foldhash, a fast, non-cryptographic, minimally
4: DoS-resistant hashing algorithm implemented in Rust designed for computational
5: uses such as hash maps, bloom filters, count sketching, etc.
```

```text
7: When should you **not** use foldhash:
8: 
9: - You are afraid of people studying your long-running program's behavior to
10:   reverse engineer its internal random state and using this knowledge to create
11:   many colliding inputs for computational complexity attacks. For more details
12:   see the section "HashDoS resistance".
13: 
14: - You expect foldhash to have a consistent output across versions or
15:   platforms, such as for persistent file formats or communication protocols.
16:   
17: - You are relying on foldhash's properties for any kind of security.
18:   Foldhash is **not appropriate for any cryptographic purpose**.
```

```text
252: The folded multiply has a fairly glaring flaw: if one of the halves is zero, the
253: output is zero. This makes it trivial to create a large number of hash
254: collisions (even by accident, as zeroes are a common input to hashes). To combat
255: this, every folded multiply in foldhash has the following form:
256: 
257: ```rust
258: folded_multiply(input1 ^ secret1, input2 ^ secret2)
259: ```
260: 
261: Here `secret1` or `secret2` are either secret random numbers generated by
262: foldhash beforehand, or partial hash results influenced by such a secret prior.
263: This (plus other careful design throughout the hash function) ensures that it is
264: not possible to create a list of inputs that collide for every instance of
265: foldhash, and also prevents certain access patterns on hash tables going
266: quadratric by ensuring that each hash table uses a different seed and thus a
267: different access pattern. It is these two properties that we refer to when we
268: claim foldhash is "minimally DoS-resistant": it does the bare minimum to defeat
269: very simple attacks.
270: 
271: However, to be crystal clear, **foldhash does not claim to provide HashDoS
272: resistance against interactive attackers**. For a student of cryptography it
273: should be trivial to derive the secret values from direct observation of hash
274: outputs, and feasible to derive the secret values from indirect observation of
275: hashes, such as through timing attacks or hash table iteration. Once an attacker
276: knows the secret values, they can once again create infinite hash collisions
277: with ease.
```

Reproduction: build the pinned Cargo program to regenerate `vectors.json`; run `verify_vectors.py` against a C++ executable produced by `compile_vector_test.py BUILD_DIR OUTPUT`; then use `run_bench.py HOST BINARY OUTDIR manifest.json` with the recorded gates. `collect.py` and `make_report.py` regenerate the JSON summaries and this report from preserved raw logs. `mac_job.py` and `xeon_job.py` retain the orchestration commands. No test verdict is inferred from the README’s HashDoS claims.
