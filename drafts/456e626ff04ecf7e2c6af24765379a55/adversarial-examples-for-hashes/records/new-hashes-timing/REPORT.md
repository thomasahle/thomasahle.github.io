# Go map hash, Abseil Hash and .NET Marvin timing

Generated 2026-09-19T00:11:40.568776+00:00.

Native implementation checks pass for all three ports. **Marvin32 cannot pass the requested full Sanity test with the specified direct seed mapping:** its zero seed makes a full zero word a no-op, so the prepend-zeroes check fails. The mapping and tests were preserved; no failure was suppressed. Go and Abseil pass Sanity.

The JSON uses the earlier nested `hash → host → metrics/runs` schema, with the three audited IDs and three control entries. Each host entry records the registered name, backend token (`null` for the generic rapidhash registration), binary SHA-256, raw-file hashes, per-run measurements, gate/load observations, selection, deviations and Sanity status.

## Protocol

Xeon: two complete serial passes, selecting the higher fixed 262144-byte bulk Average (GiB/s from that same run) and independently the lower Average over lengths 1–31. Rounded bulk ties use the higher GiB/s, then the earlier run. M2: three serial passes and independent medians for bulk and small keys. The varying-length bulk section is retained but never used for headline speeds. Commands are exactly `SMHasher3 NAME --test=Speed`, without affinity or timing-priority changes.

M2 preparation and every timing start require no other SMHasher3 process and one-minute load <4.5, with 60-second polling and no timeout bypass. A five-second process monitor records overlap and archives/retries any overlapping run. The gate is a start condition, not a sustained load promise. Both hosts time XXH3-64, rapidhash and chainhash-256 in the same binary as the new hashes.

M2 flags deviations exceeding 15% from each three-run median; Xeon flags two-run max/min spread exceeding 15%. Control changes exceeding 15% from the earlier host baseline are separately flagged. Baselines are `../m2-rerun/speeds_m2_v2_final.json` and `../speedbench/speeds.json`, with their hashes recorded in the JSON.

SMHasher3 uses its unchanged calibrated monotonic-clock cycle estimate on M2 and TSC ticks on Xeon. These are not interchangeable physical core-cycle counts. Its GiB/s conversion assumes 3.5 GHz on both hosts; it is not independently measured throughput.

## Results

| Host | Hash / registration | Backend | Bulk B/cycle | Small cycles/hash | Runs | Sanity |
|---|---|---|---:|---:|---:|---|
| Xeon8375C | GoMapHash | GO_AMD64_AES | 17.67 | 64.32 | 2 | PASS |
| Xeon8375C | AbseilHash-default | scalar-mul128 | 8.22 | 20.67 | 2 | PASS |
| Xeon8375C | Marvin32 | scalar | 0.95 | 31.07 | 2 | FAIL |
| Xeon8375C | XXH3-64 | avx512 | 19.80 | 30.22 | 2 | control |
| Xeon8375C | rapidhash | generic | 10.70 | 27.56 | 2 | control |
| Xeon8375C | chainhash-256 | hwclmul | 14.30 | 103.89 | 2 | control |
| M2Pro | GoMapHash | GO_ARM64_AES | 15.69 | 26.94 | 3 | PASS |
| M2Pro | AbseilHash-default | arm-crypto | 14.31 | 16.15 | 3 | PASS |
| M2Pro | Marvin32 | scalar | 0.64 | 46.72 | 3 | FAIL |
| M2Pro | XXH3-64 | neon | 13.09 | 24.89 | 3 | control |
| M2Pro | rapidhash | generic | 15.70 | 20.82 | 3 | control |
| M2Pro | chainhash-256 | hwpmull | 22.42 | 73.61 | 3 | control |

## Registrations and audited witnesses

**GoMapHash**, 64 bits: `GO_AMD64_AES` on Xeon; `GO_ARM64_AES` on M2. The map seed passes through unchanged. One OS-random 128-byte process key was captured once and installed by the initialization hook, identically on both hosts; see `evidence/go-key.json` and `bench/go-process-key.bin`. It stays fixed throughout each timing process. Key generation/setup is outside the timed call. This reproducible captured random key models the runtime process key; it is not freshly generated for each hash or derived from the map seed.

The registration is Go runtime `memhash` on the complete byte string. It is **not** the separate `hash/maphash.Bytes` 128-byte chaining API for long messages (nor its special empty-input behavior), and not integer-key `memhash32/64`. The audited 15/16-byte pair lies in the common nonempty short-input path. These are hardware-intrinsic C++ port measurements, not the Go runtime assembly itself. ARM hardware primitives replace only AESE/AESMC operations in the supplied portable port; Xeon uses AES-NI.

The Go vector checker validates both fixed-key and random-key corpora per architecture: 5,031 vectors each, zero mismatches. In each corpus 1,635 runtime byte-hash vectors go through `findHash` and the exact registered function; the remaining integer-hash and maphash.Bytes/String vectors validate the companion port APIs. On Xeon, the audited pair `000000000000000000000000000000` / `a3fe5da3a3fe5da342a3bcfe42a3bcfe` collides for all 256 combinations of the four DDT choices, tested over 16 map seeds each (4,096 registered comparisons). Each relevant key byte has choices `{0f,10,ab,b4}`; the sufficient event has probability exactly `(4/256)^4 = 2^-24`. A new deterministic SplitMix64 registered-wrapper sample gives 17 / 2^28 collisions, consistent with that event. The key varies only in validation, then initialization restores the timing key.

**Do not transfer the Go amd64 25-bit collision cap to the M2 ARM64 function.** The JSON explicitly sets `audited_pair_applies_to_backend=false` and `collision_bits_for_backend=null` for Go on M2. The ARM64 backend is different and its vectors are checked independently; the audited amd64 witness/rate is not claimed for it. `rows.json` calls the Go result measured; that classification is preserved, while distinguishing the exact sufficient DDT event from sampled complete-output equality.

**AbseilHash-default**, 64 bits: Abseil 73d2688300440c8af028eec865ee0dcd85e93025, direct supplied seed, no CRC path. Xeon uses the library’s default generic x86 build (`scalar-mul128`), not the optional `-maes -msse4.2` Abseil build. M2 uses `arm-crypto` for >32 bytes, including the actual 1024-byte piecewise chaining. The <=32-byte multiply path is shared. Although the SMHasher wrapper is compiled with host-native optimization, the Xeon algorithm choice is explicitly scalar, matching the audited default.

The existing source registration `Abseil64_llh` is listed by SMHasher3 as `Abseil64-llh` (underscore normalization). It and the new Xeon wrapper match the real pinned library on the full 47,439 `(length, seed, alignment)` schedule from the audit, plus the supplied scalar vector files. The duplicate-verification-code warning on Xeon is expected: both implement the same function, with native code `07203cdb`. M2’s new wrapper also passes the full 47,439-case schedule against the real pinned library with the default Apple ARM-crypto build, plus the supplied 5,575-string native ARM corpus, including long messages; its ARM round instructions are retained in `evidence/M2Pro/newhashes.asm`.

The empty string / `a6e02637c07bd386` pair reproduces through the registration for all 32 SwissTable seeds and 99,968 further seeds, and the nonempty alternative for 10,000 seeds. The common first multiplicand proves equality for every seed; this remains **exact, key-free, 0 bits**, not a sampled-rate claim.

**Marvin32**, 32 bits, scalar on both hosts: `p0=lo32(seed)`, `p1=hi32(seed)` and raw input bytes. The 5,832 real-runtime vectors from the three files plus the published known-answer case give 5,833 passing checks. The audited 12-byte UTF-16LE pair `610061002f546d1662006200` / `610061802f9579a262fc6100` produces 34,958 / 2^24 collisions on Xeon, consistent with the audited measured epsilon 0.002082855. M2 confirms 530 / 2^18 collisions, logged in its verification file. This remains **measured**, not key-free or exact. Its 32-bit output is preserved.

The Sanity limitation is intrinsic to the requested seed API: at seed zero, Marvin starts with `(p0,p1)=(0,0)`; adding a zero word and applying Block leaves `(0,0)`. Prepending four zero bytes therefore preserves the hash. The real-runtime vectors themselves show seed 0 hashing both empty input and four zero bytes to `92805aae`. SMHasher3 explicitly forces seed zero for that check. Implementation verification, basic sanity checks, append-zeroes and thread-safety checks pass. Remapping/excluding seed zero would change the specified function, so it was not done. These timings are the C++ byte-hash port, not .NET JIT, string encoding, map lookup or allocation costs.

## Build and evidence

The Xeon scratch tree is `/home/thomas-ahle/agents/speedbench-newhashes/source`, copied from the earlier `speedbench/source`; it reuses that lane’s main/test/hash library objects from `build-release-20260917`. M2 uses `./smhasher3-m2`, a scratch copy of `../m2-rerun/smhasher3`, and reuses `../m2-rerun/build-fixed` objects. Only the new registration translation unit is compiled and linked into each new timing binary. This retains exactly the earlier harness/timer/control objects, avoiding unrelated rebuild changes. The new hash source is also recorded in each scratch CMake source list.

`bench/newhashes.cpp`, `bench/go_hardware.h`, `bench/absl_hardware.h`, `bench/absl_native_arm.h`, and `bench/marvin.h` are the adapters. The scratch copies of `newhashes.cpp` contain the final architecture-specific native verification constants. Both callback slots expose the native little-endian byte function; no big-endian runtime claim is made. `bench/prepare_host.py`, `bench/run_speed.py`, `bench/collect.py` and `bench/report.py` reproduce preparation, gating, collection and this report.

`evidence/<host>/` contains compile/link commands, reused-input SHA-256 values, compiler/host details, registration names/constants, vector/pair output, Sanity logs, disassembly, gate records, all raw Speed logs and execution metadata. `bench/absl_oracle.cpp` generates the original extended corpus against the unmodified pinned library; `bench/absl_extended.cpp` checks the actual wrapper and existing scalar registration.

| Host | Binary | SHA-256 |
|---|---|---|
| Xeon8375C | `/home/thomas-ahle/agents/speedbench-newhashes/build/SMHasher3` | `ae073fff3d359ce7a5c8191cde44115f880f1694e1eebc080e8598f9539a441f` |
| M2Pro | `<scratch>/codex/new-hashes-timing/build-m2/SMHasher3` | `566fa74a7894b1420e16ae5f5f4463a858a8d00c5f30b6099ed88dc86c8ad0bd` |

The first M2 port representation spilled Abseil AES lane state into general registers. It was replaced with native NEON vector state and the upstream prefetch pattern before final timing. Two completed preliminary runs and one interrupted Marvin run are archived under `evidence/M2Pro-portstruct-superseded/` and excluded; every final M2 hash and control is retimed in the new binary.

## Variation and control checks

| Host | Control | Bulk new/old | Small new/old | >15% |
|---|---|---:|---:|---|
| Xeon8375C | XXH3-64 | 1.0056 | 1.0020 | no |
| Xeon8375C | rapidhash | 1.0028 | 0.9993 | no |
| Xeon8375C | chainhash-256 | 0.9917 | 0.9999 | no |
| M2Pro | XXH3-64 | 0.9947 | 1.0155 | no |
| M2Pro | rapidhash | 1.0446 | 0.9877 | no |
| M2Pro | chainhash-256 | 0.9833 | 1.0443 | no |

No >15% run-deviation flags.

Xeon8375C: 12 accepted Speed runs; boundary load1 36.85–63.63, including five-second in-run samples 30.67–63.68; 0 overlapping attempts archived and excluded.

M2Pro: 18 accepted Speed runs; boundary load1 3.44–6.43, including five-second in-run samples 3.28–7.06; 0 overlapping attempts archived and excluded.

All 30 requested timing runs are complete (12 Xeon, 18 M2). The requested universal Sanity-pass condition is not met because of the intrinsic Marvin seed-zero failure described above.
