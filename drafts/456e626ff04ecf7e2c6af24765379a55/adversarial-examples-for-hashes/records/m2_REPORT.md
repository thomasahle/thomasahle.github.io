# Corrected Apple M2 Pro SMHasher3 speed pass

Status: complete. The M2 results replace the previous M2 column; every reported M2 run uses one corrected binary. Both original timing-job PIDs were observed exited before the scratch source was copied or built.

Important qualifications: six hashes have pre-existing verification-code failures, including NMHASH/NMHASHX values that differ from Xeon. M2 load was not consistently quiet after the start gate, and ChainHash-1k has a 173.79% bulk run spread. The data retain the requested protocol and must be read with these qualifications; details and both raw runs follow.

## Build and detection evidence

The source was copied from `../arm-umash-clhash/smhasher3`, excluding build directories. It retains the verified ARM UMASH/CLhash adapters. `classic_openssl.cpp` and `patch_tree.py` from `../classic-bench` add the OpenSSL 3 wrappers. Only the copied source was edited. `provenance/source-sha256.json`, `integration.diff`, and the saved original files identify the exact tree and changes.

One-line classifier fix (`provenance/family.cmake.diff`):

```diff
--- a/platform/family.cmake
+++ b/platform/family.cmake
@@ -13,6 +13,7 @@
     OR (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86"))
   set(PROCESSOR_FAMILY "x86")
 elseif ((CMAKE_SYSTEM_PROCESSOR STREQUAL "arm")
+    OR (CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64")
     OR (CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64"))
   set(PROCESSOR_FAMILY "Arm")
 else ()
```

Fresh build at `./build-fixed`; exact commands are in `provenance/configure-command.json` and `build-command.json`. Release preserves the original explicit `-Xclang -target-feature -Xclang +aes` and the tree’s `-O3 -march=native` flags. `DETECTED_LITTLE_ENDIAN=ON` and `ENDIAN_DETECT_BUILDTIME=OFF` avoid the missing CMake 4.1 `Modules/TestEndianess.c.in` template. OpenSSL root is `/opt/homebrew/opt/openssl@3`. No benchmark loop or timer changes were made.

The inherited vendor C files retain `-O3 -std=c11 -march=armv8-a+crypto`. Configure evidence:

```text
-- CMAKE_SYSTEM_PROCESSOR: arm64
-- ARM NEON available
-- ARM ACLE available
--   ARM AES intrinsics available
--   ARM 32-bit __asm__() available
--   ARM 64-bit __asm__() available
```

Generated `build-fixed/include/Platform.h`:

```c
#define HAVE_ARM_NEON
#define HAVE_ARM_ACLE
#define HAVE_ARM_AES
/* #undef HAVE_ARM_SHA1 */
/* #undef HAVE_ARM_SHA2 */
#define HAVE_ARM_ASM
#define HAVE_ARM64_ASM
```

NEON, ACLE and AES changed from undefined to defined. ARM_ASM and ARM64_ASM were already defined before the fix. SHA1/SHA2 remain undefined: both fresh probes now see the intrinsic declarations but fail because the preserved flags do not enable target feature `sha2`. Exact diagnostics are in `provenance/CMakeConfigureLog.yaml`. No requested hash uses a SHA backend, so no extra target feature was added.

Relative to the original speedbench source manifest, changed existing files are exactly `CMakeLists.txt`, `hashes/Hashsrc.cmake`, `hashes/umash.cpp`, `hashes/clhash.cpp`, and `platform/family.cmake`; additions are the inherited vendor files and `classic_openssl.cpp`. No original source files are missing. In particular, all other hash sources and the timing implementation match the original source snapshot. See `provenance/source-comparison.json`.

M2 binary SHA-256: `fb2f75fb2f170a6956441e7db8ae2ae95e3944ef44eecbb0aeeb46d4115b025a`. The runner checks this hash before every invocation and again at completion. Compiler commands, cache, platform/timing headers, library linkage and environment are preserved under `provenance/`.

## Backend audit

`--list` prints the implementation token verbatim; a blank token is not evidence that the compiler emitted no ARM instructions. UMASH and CLhash retain the inherited token `hwclmul` even though their ARM adapters execute PMULL. The detailed source audit is in `provenance/backend-audit.json`.

The initial expectation about gxhash/rust-ahash is not supported by this tree: `include/hashlib/AES.h` selects `AES-arm.h` under `HAVE_ARM_AES`. gxhash’s generic path reports `g+arm`. rust-ahash still reports `portable` because its token names the shuffle implementation, while its AES rounds use the ARM helper. Disassembly excerpts confirm hardware AES in both objects. NMHASH/NMHASHX select only scalar/SSE2/AVX2/AVX512 and remain scalar. SipHash’s SIMD choices are SSE2/SSSE3 only, so both variants remain portable on ARM. t1ha2’s alignment token uses compiler predefines, including `__ARM_FEATURE_UNALIGNED`, rather than this CMake detection; it remains `1Y+a0`.

| Hash | Before token | Corrected token | Source / implementation |
|---|---|---|---|
| rapidhash | — (blank) | — (blank) | `rapidhash.cpp:443`; generic hash, shared Mathmult ARM64 multiply |
| rapidhash.protected | — (blank) | — (blank) | `rapidhash.cpp:458`; generic hash, shared Mathmult ARM64 multiply |
| XXH3-64 | `scalar` | `neon` | `xxhash.cpp:1596`; NEON via xxh3-arm.h |
| XXH3-128 | `scalar` | `neon` | `xxhash.cpp:1702`; NEON via xxh3-arm.h |
| wyhash | — (blank) | — (blank) | `wyhash.cpp:267`; generic hash, shared Mathmult ARM64 multiply |
| komihash | — (blank) | — (blank) | `komihash.cpp:298`; generic hash, shared Mathmult ARM64 multiply |
| a5hash | — (blank) | — (blank) | `a5hash.cpp:459`; generic hash, shared Mathmult ARM64 multiply |
| a5hash-128 | — (blank) | — (blank) | `a5hash.cpp:490`; generic hash, shared Mathmult ARM64 multiply |
| MuseAir | — (blank) | — (blank) | `museair.cpp:272`; generic hash, shared Mathmult ARM64 multiply |
| MuseAir.bfast | — (blank) | — (blank) | `museair.cpp:288`; generic hash, shared Mathmult ARM64 multiply |
| t1ha2-64 | `1Y+a0` | `1Y+a0` | `t1ha.cpp:1667`; generic hash, shared Mathmult ARM64 multiply; token reflects compiler alignment predefines |
| CityHash-64 | — (blank) | — (blank) | `cityhash.cpp:773`; generic hash |
| FarmHash-64.NA | — (blank) | — (blank) | `farmhash.cpp:1695`; generic hash |
| MurmurHash3-128 | — (blank) | — (blank) | `murmurhash3.cpp:337`; generic hash |
| gxhash-64 | `g+portable` | `g+arm` | `gxhash.cpp:423`; generic layout + AES.h ARM rounds |
| rust-ahash | `portable` | `portable` | `rust-ahash.cpp:488`; portable shuffle token + AES.h ARM rounds |
| rust-ahash-fb | — (blank) | — (blank) | `rust-ahash.cpp:521`; generic hash, shared Mathmult ARM64 multiply |
| SpookyHash2-64 | — (blank) | — (blank) | `spookyhash.cpp:423`; generic hash |
| SpookyHash2-32 | — (blank) | — (blank) | `spookyhash.cpp:408`; generic hash |
| HighwayHash-64 | `portable` | `neon` | `highwayhash.cpp:444`; hash-neon.h |
| HighwayHash-128 | `portable` | `neon` | `highwayhash.cpp:466`; hash-neon.h |
| pengyhash | — (blank) | — (blank) | `pengyhash.cpp:75`; generic hash |
| NMHASH | `scalar` | `scalar` | `nmhash.cpp:766`; scalar fallback; SIMD only SSE2/AVX2/AVX512 |
| NMHASHX | `scalar` | `scalar` | `nmhash.cpp:785`; scalar fallback; SIMD only SSE2/AVX2/AVX512 |
| fasthash-32 | — (blank) | — (blank) | `fasthash.cpp:99`; generic hash |
| fasthash-64 | — (blank) | — (blank) | `fasthash.cpp:113`; generic hash |
| HalftimeHash-64 | `portable` | `neon` | `halftimehash.cpp:1407`; HAVE_ARM_NEON vector backend |
| HalftimeHash-128 | `portable` | `neon` | `halftimehash.cpp:1425`; HAVE_ARM_NEON vector backend |
| HalftimeHash-256 | `portable` | `neon` | `halftimehash.cpp:1443`; HAVE_ARM_NEON vector backend |
| HalftimeHash-512 | `portable` | `neon` | `halftimehash.cpp:1461`; HAVE_ARM_NEON vector backend |
| polymurhash | — (blank) | — (blank) | `polymur.cpp:250`; generic hash, shared Mathmult ARM64 multiply |
| CLhash | not registered / not in baseline | `hwclmul` | `clhash.cpp:548`; vendored ARM PMULL, inherited hwclmul label |
| poly-mersenne.deg1 | `int128` | `int128` | `poly_mersenne.cpp:374`; generic hash, shared Mathmult ARM64 multiply |
| poly-mersenne.deg2 | `int128` | `int128` | `poly_mersenne.cpp:392`; generic hash, shared Mathmult ARM64 multiply |
| poly-mersenne.deg3 | `int128` | `int128` | `poly_mersenne.cpp:410`; generic hash, shared Mathmult ARM64 multiply |
| poly-mersenne.deg4 | `int128` | `int128` | `poly_mersenne.cpp:428`; generic hash, shared Mathmult ARM64 multiply |
| multiply-shift | — (blank) | — (blank) | `multiply_shift.cpp:300`; generic hash, int128 arithmetic |
| pair-multiply-shift | — (blank) | — (blank) | `multiply_shift.cpp:317`; generic hash, int128 arithmetic |
| tabulation-64 | `int128` | `int128` | `tabulation.cpp:363`; generic hash, shared Mathmult ARM64 multiply |
| SipHash-2-4 | `portable` | `portable` | `siphash.cpp:441`; portable fallback; SIMD only SSE2/SSSE3 |
| SipHash-1-3 | `portable` | `portable` | `siphash.cpp:477`; portable fallback; SIMD only SSE2/SSSE3 |
| chainhash-256 | `hwpmull` | `hwpmull` | `chainhash.cpp:1200`; PMULL, compiler-predefine path already active |
| chainhash-1k | `hwpmull` | `hwpmull` | `chainhash.cpp:1215`; PMULL, compiler-predefine path already active |
| HalfSipHash | — (blank) | — (blank) | `siphash.cpp:513`; generic half-width code |
| mir.exact | — (blank) | — (blank) | `mum_mir.cpp:1048`; generic hash, shared Mathmult ARM64 multiply |
| mum3.exact.unroll1 | — (blank) | — (blank) | `mum_mir.cpp:798`; generic hash, shared Mathmult ARM64 multiply |
| mum3.exact.unroll2 | — (blank) | — (blank) | `mum_mir.cpp:814`; generic hash, shared Mathmult ARM64 multiply |
| mum3.exact.unroll3 | — (blank) | — (blank) | `mum_mir.cpp:830`; generic hash, shared Mathmult ARM64 multiply |
| mum3.exact.unroll4 | — (blank) | — (blank) | `mum_mir.cpp:846`; generic hash, shared Mathmult ARM64 multiply |
| mx3.v1 | — (blank) | — (blank) | `mx3.cpp:191`; generic hash |
| mx3.v2 | — (blank) | — (blank) | `mx3.cpp:176`; generic hash |
| mx3.v3 | — (blank) | — (blank) | `mx3.cpp:162`; generic hash |
| UMASH-64 | not registered / not in baseline | `hwclmul` | `umash.cpp:1250`; vendored ARM PMULL, inherited hwclmul label |
| UMASH-128 | not registered / not in baseline | `hwclmul` | `umash.cpp:1287`; vendored ARM PMULL, inherited hwclmul label |
| UMASH-64.reseed | not registered / not in baseline | `hwclmul` | `umash.cpp:1268`; vendored ARM PMULL, inherited hwclmul label |
| poly1305-hash | not registered / not in baseline | — (blank) | `classic_openssl.cpp:71`; OpenSSL 3 runtime dispatch; blank token |
| ghash | not registered / not in baseline | — (blank) | `classic_openssl.cpp:81`; OpenSSL 3 runtime dispatch; blank token |

No dedicated ARM hash backend is present for: `rapidhash`, `rapidhash.protected`, `wyhash`, `komihash`, `a5hash`, `a5hash-128`, `MuseAir`, `MuseAir.bfast`, `t1ha2-64`, `CityHash-64`, `FarmHash-64.NA`, `MurmurHash3-128`, `rust-ahash-fb`, `SpookyHash2-64`, `SpookyHash2-32`, `pengyhash`, `NMHASH`, `NMHASHX`, `fasthash-32`, `fasthash-64`, `polymurhash`, `poly-mersenne.deg1`, `poly-mersenne.deg2`, `poly-mersenne.deg3`, `poly-mersenne.deg4`, `multiply-shift`, `pair-multiply-shift`, `tabulation-64`, `SipHash-2-4`, `SipHash-1-3`, `HalfSipHash`, `mir.exact`, `mum3.exact.unroll1`, `mum3.exact.unroll2`, `mum3.exact.unroll3`, `mum3.exact.unroll4`, `mx3.v1`, `mx3.v2`, `mx3.v3`. These use the generic hash implementation. Some use shared `Mathmult.h` helpers with ARM assembly options (already enabled in the original binary), and t1ha2 uses ARM unaligned-access compiler information; this list does not imply entirely architecture-independent generated machine code. gxhash and rust-ahash are excluded because their shared AES backend is ARM accelerated. The OpenSSL wrappers use library CPU dispatch outside SMHasher3’s family detector.

## Verification

`speeds_existing.json` has no implementation verification codes; Speed’s trailing `0x00000001` is not such a code. To compare reliably, verbose VerifyAll output was collected from the unchanged original M2 and Xeon binaries, with their hashes checked against the baseline metadata. The corrected binary’s codes are compared with those observations. OpenSSL wrapper codes are also compared with the preceding classic-bench records. All requested hashes receive a separate native/default `--test=Sanity` run; per-hash details and raw paths are in the JSON.

Requested native Sanity/verification failures: NMHASH; NMHASHX; poly-mersenne.deg1; poly-mersenne.deg2; poly-mersenne.deg3; poly-mersenne.deg4.

All 57 default/native Sanity invocations completed. Every hash passed the six structural checks (sanity 1/2, append/prepend zeroes, and two thread-safety checks). Fifty-one also passed implementation verification; the other six failed only that code check. Exit status alone is insufficient here: the harness defaults to returning zero even when these checks fail.

Pre-existing verification defects are distinct from changes caused by this repair. NMHASH and NMHASHX reproduce the old scalar M2 values but differ from the Xeon vector implementation and the registered expected constants in both byte orders. The four requested poly-mersenne degrees reproduce both old hosts’ values, which disagree with their registered constants. These rows are retained as requested, with verification status FAIL; they must not be treated as fully validated cross-host results. The standalone VerifyAll command exits 1 on both original binaries and the corrected binary; its raw logs also contain degree 0, which is outside this manifest.

| Hash | Corrected M2 LE / BE | Old M2 LE / BE | Xeon LE / BE | Registered expected LE / BE |
|---|---|---|---|---|
| NMHASH | `0x4B575DFB` / `0x9ED08EA8` | `0x4B575DFB` / `0x9ED08EA8` | `0x12A30553` / `0xE3222AC8` | `0x12a30553` / `0xe3222ac8` |
| NMHASHX | `0x9CF42B2D` / `0x3E08D2D5` | `0x9CF42B2D` / `0x3E08D2D5` | `0xA8580227` / `0x83B36886` | `0xa8580227` / `0x83b36886` |
| poly-mersenne.deg1 | `0xD7C72603` / `0x86FD5F12` | `0xD7C72603` / `0x86FD5F12` | `0xD7C72603` / `0x86FD5F12` | `0x2c5c1b0e` / `0xe85e0414` |
| poly-mersenne.deg2 | `0x9A320363` / `0xDBB79A0A` | `0x9A320363` / `0xDBB79A0A` | `0x9A320363` / `0xDBB79A0A` | `0x35af4ea2` / `0xea3bfb05` |
| poly-mersenne.deg3 | `0xAEDC3147` / `0xEBEE4702` | `0xAEDC3147` / `0xEBEE4702` | `0xAEDC3147` / `0xEBEE4702` | `0x8197a37d` / `0x601cf718` |
| poly-mersenne.deg4 | `0x045825BB` / `0x68821F25` | `0x045825BB` / `0x68821F25` | `0x045825BB` / `0x68821F25` | `0x27c2f53b` / `0x6857dc31` |

| Hash | Default verification | Sanity | Reference comparison |
|---|---|---|---|
| rapidhash | LE `0x1FDC65EE` | PASS | existing_M2, existing_Xeon, existing_arm_adapters |
| rapidhash.protected | LE `0x72C9270A` | PASS | existing_M2, existing_Xeon |
| XXH3-64 | CE `0x1AAEE62C` | PASS | existing_M2, existing_Xeon |
| XXH3-128 | CE `0x288DAA94` | PASS | existing_M2, existing_Xeon |
| wyhash | LE `0x9DAE7DD3` | PASS | existing_M2, existing_Xeon |
| komihash | CE `0x8157FF6D` | PASS | existing_M2, existing_Xeon, existing_arm_adapters |
| a5hash | CE `0xADDE79B3` | PASS | existing_M2, existing_Xeon |
| a5hash-128 | CE `0x89406B11` | PASS | existing_M2, existing_Xeon |
| MuseAir | CE `0xF89F1683` | PASS | existing_M2, existing_Xeon |
| MuseAir.bfast | CE `0xC61BEE56` | PASS | existing_M2, existing_Xeon |
| t1ha2-64 | LE `0x8F16C948` | PASS | existing_M2, existing_Xeon |
| CityHash-64 | LE `0x5FABC5C5` | PASS | existing_M2, existing_Xeon |
| FarmHash-64.NA | LE `0xEBC4A679` | PASS | existing_M2, existing_Xeon |
| MurmurHash3-128 | LE `0x6384BA69` | PASS | existing_M2, existing_Xeon |
| gxhash-64 | CE `0x48F84240` | PASS | existing_M2, existing_Xeon |
| rust-ahash | LE `0x3BF4383B` | PASS | existing_M2, existing_Xeon |
| rust-ahash-fb | LE `0x53C9F167` | PASS | existing_M2, existing_Xeon |
| SpookyHash2-64 | LE `0x972C4BDC` | PASS | existing_M2, existing_Xeon |
| SpookyHash2-32 | LE `0xA48BE265` | PASS | existing_M2, existing_Xeon |
| HighwayHash-64 | CE `0xF3246108` | PASS | existing_M2, existing_Xeon |
| HighwayHash-128 | CE `0x232D434E` | PASS | existing_M2, existing_Xeon |
| pengyhash | CE `0x861A1254` | PASS | existing_M2, existing_Xeon |
| NMHASH | LE `0x4B575DFB` | FAIL | MISMATCH |
| NMHASHX | LE `0x9CF42B2D` | FAIL | MISMATCH |
| fasthash-32 | LE `0xE9481AFC` | PASS | existing_M2, existing_Xeon |
| fasthash-64 | LE `0xA16231A7` | PASS | existing_M2, existing_Xeon |
| HalftimeHash-64 | LE `0xED42E424` | PASS | existing_M2, existing_Xeon |
| HalftimeHash-128 | LE `0x952DF141` | PASS | existing_M2, existing_Xeon |
| HalftimeHash-256 | LE `0x912330EA` | PASS | existing_M2, existing_Xeon |
| HalftimeHash-512 | LE `0x1E0F99EA` | PASS | existing_M2, existing_Xeon |
| polymurhash | LE `0x0722B1A7` | PASS | existing_M2, existing_Xeon |
| CLhash | LE `0x2E554CB4` | PASS | existing_Xeon, existing_arm_adapters |
| poly-mersenne.deg1 | LE `0xD7C72603` | FAIL | existing_M2, existing_Xeon |
| poly-mersenne.deg2 | LE `0x9A320363` | FAIL | existing_M2, existing_Xeon |
| poly-mersenne.deg3 | LE `0xAEDC3147` | FAIL | existing_M2, existing_Xeon |
| poly-mersenne.deg4 | LE `0x045825BB` | FAIL | existing_M2, existing_Xeon |
| multiply-shift | LE `0xB7A5E66D` | PASS | existing_M2, existing_Xeon |
| pair-multiply-shift | LE `0x4FBA804D` | PASS | existing_M2, existing_Xeon |
| tabulation-64 | LE `0x53B08B2D` | PASS | existing_M2, existing_Xeon |
| SipHash-2-4 | LE `0x57B661ED` | PASS | existing_M2, existing_Xeon |
| SipHash-1-3 | LE `0x8936B193` | PASS | existing_M2, existing_Xeon |
| chainhash-256 | LE `0xAA4E2A3B` | PASS | existing_M2, existing_Xeon |
| chainhash-1k | LE `0x7A1ED2E0` | PASS | existing_M2, existing_Xeon |
| HalfSipHash | LE `0xD2BE7FD8` | PASS | existing_M2, existing_Xeon |
| mir.exact | LE `0x00A393C8` | PASS | existing_M2, existing_Xeon |
| mum3.exact.unroll1 | LE `0x3D14C6E2` | PASS | existing_M2, existing_Xeon |
| mum3.exact.unroll2 | LE `0x3A556EB2` | PASS | existing_M2, existing_Xeon |
| mum3.exact.unroll3 | LE `0x8BD72B8C` | PASS | existing_M2, existing_Xeon |
| mum3.exact.unroll4 | LE `0x0AD998DF` | PASS | existing_M2, existing_Xeon |
| mx3.v1 | LE `0x4DB51E5B` | PASS | existing_M2, existing_Xeon |
| mx3.v2 | LE `0x527399AD` | PASS | existing_M2, existing_Xeon |
| mx3.v3 | LE `0x7B287B65` | PASS | existing_M2, existing_Xeon |
| UMASH-64 | LE `0x36A264CD` | PASS | existing_Xeon, existing_arm_adapters |
| UMASH-128 | LE `0x63857D05` | PASS | existing_Xeon, existing_arm_adapters |
| UMASH-64.reseed | LE `0x161495C6` | PASS | existing_Xeon, existing_arm_adapters |
| poly1305-hash | CE `0xBD015C42` | PASS | existing_classic |
| ghash | CE `0x1F397201` | PASS | existing_classic |

## Protocol, load and results

Each host runs one timing process at a time, in two full passes through its manifest. Commands are exactly `SMHasher3 NAME --test=Speed`, with default 1–31-byte and bulk settings. M2 uses no affinity or priority adjustment. Build alone uses nice 10. Xeon prefixes both Sanity and Speed with `taskset -c CORE`, selected using a three-second per-CPU `/proc/stat` sample that maximizes the minimum idle fraction across SMT siblings. The Xeon binary is the original `~/agents/speedbench/build-release-20260917/SMHasher3`.

The M2 start gate samples `uptime` once per 60 seconds until load1 < 3.0, or for 10,800 seconds before running with a timeout flag. The gate applies once immediately before the two Speed passes, as in `speedbench_REPORT.md`; subsequent load is recorded at every run boundary.

Gate result: `{"timestamp": "2026-09-18T10:17:57.330056+00:00", "uptime": "12:17  up 1 day, 9 mins, 1 user, load averages: 2.92 5.56 6.35", "load1": 2.92, "load5": 5.56, "load15": 6.35, "elapsed_seconds": 1680.770058542, "threshold": 3.0, "deadline_seconds": 10800, "poll_seconds": 60, "timeout": false}`.

M2Pro: 114/114 Speed runs recorded; boundary load1 range 2.88–27.80.
Xeon8375C: 4/4 Speed runs recorded; boundary load1 range 9.45–10.13.
Xeon CPU 6, SMT siblings [6, 54], sampled idle fraction 1.0000. Binary SHA-256 `03c2ff9f38cc564400eeb1906da90cb74ea6efcff2bea6c660626fdbe1571a99`.

Selection: headline bulk is the fixed 262144-byte Average across alignments, taking higher bytes/cycle, then higher GiB/sec on rounded ties, then run 1. Small keys independently take the lower Average over 1–31 bytes. The varying-length bulk section is retained but not used for selection. Spread is 100 × (larger/smaller − 1). No extra runs, frequency correction, or normalization is applied.

| Host | Hash | Bulk bytes/cycle | Small cycles/hash | Bulk / small selected run | Bulk / small spread % |
|---|---|---:|---:|---|---|
| M2Pro | rapidhash | 16.10 | 20.09 | 2 / 2 | 1.77 / 0.50 |
| M2Pro | rapidhash.protected | 11.31 | 24.40 | 2 / 2 | 2.82 / 2.58 |
| M2Pro | XXH3-64 | 14.20 | 22.82 | 1 / 1 | 7.90 / 7.41 |
| M2Pro | XXH3-128 | 13.37 | 29.76 | 1 / 1 | 1.36 / 1.28 |
| M2Pro | wyhash | 8.75 | 21.20 | 1 / 2 | 0.00 / 1.13 |
| M2Pro | komihash | 8.80 | 22.03 | 1 / 1 | 6.67 / 10.76 |
| M2Pro | a5hash | 3.20 | 22.36 | 1 / 1 | 0.31 / 0.22 |
| M2Pro | a5hash-128 | 12.22 | 21.47 | 2 / 2 | 1.41 / 1.82 |
| M2Pro | MuseAir | 10.21 | 24.65 | 2 / 2 | 5.04 / 4.46 |
| M2Pro | MuseAir.bfast | 12.52 | 21.49 | 1 / 1 | 2.54 / 2.51 |
| M2Pro | t1ha2-64 | 6.19 | 39.30 | 2 / 2 | 0.16 / 0.25 |
| M2Pro | CityHash-64 | 6.10 | 42.30 | 2 / 2 | 2.01 / 2.08 |
| M2Pro | FarmHash-64.NA | 5.96 | 43.15 | 1 / 1 | 2.94 / 2.83 |
| M2Pro | MurmurHash3-128 | 1.92 | 40.31 | 1 / 1 | 9.71 / 6.60 |
| M2Pro | gxhash-64 | 31.81 | 40.02 | 2 / 2 | 2.45 / 2.00 |
| M2Pro | rust-ahash | 1.12 | 39.30 | 2 / 2 | 2.75 / 1.35 |
| M2Pro | rust-ahash-fb | 4.89 | 18.25 | 2 / 2 | 14.25 / 14.68 |
| M2Pro | SpookyHash2-64 | 5.81 | 37.74 | 1 / 1 | 26.30 / 22.60 |
| M2Pro | SpookyHash2-32 | 4.30 | 50.37 | 1 / 1 | 0.70 / 0.58 |
| M2Pro | HighwayHash-64 | 1.48 | 90.16 | 2 / 2 | 2.07 / 1.84 |
| M2Pro | HighwayHash-128 | 1.67 | 104.24 | 2 / 2 | 13.61 / 13.89 |
| M2Pro | pengyhash | 3.68 | 74.39 | 2 / 2 | 0.27 / 0.70 |
| M2Pro | NMHASH | 1.99 | 59.96 | 2 / 2 | 8.15 / 6.80 |
| M2Pro | NMHASHX | 1.94 | 37.18 | 1 / 1 | 4.30 / 2.26 |
| M2Pro | fasthash-32 | 1.40 | 36.81 | 1 / 1 | 1.45 / 3.04 |
| M2Pro | fasthash-64 | 1.39 | 34.96 | 1 / 1 | 0.00 / 2.60 |
| M2Pro | HalftimeHash-64 | 4.63 | 48.06 | 2 / 1 | 1.09 / 0.83 |
| M2Pro | HalftimeHash-128 | 9.49 | 58.62 | 2 / 1 | 0.32 / 1.50 |
| M2Pro | HalftimeHash-256 | 10.49 | 59.12 | 1 / 1 | 4.17 / 2.79 |
| M2Pro | HalftimeHash-512 | 7.49 | 63.38 | 1 / 1 | 3.74 / 2.41 |
| M2Pro | polymurhash | 5.59 | 32.86 | 2 / 2 | 0.90 / 0.58 |
| M2Pro | CLhash | 15.53 | 39.53 | 1 / 1 | 2.51 / 2.53 |
| M2Pro | poly-mersenne.deg1 | 2.38 | 45.41 | 1 / 1 | 3.48 / 2.51 |
| M2Pro | poly-mersenne.deg2 | 2.39 | 51.60 | 1 / 1 | 3.46 / 2.44 |
| M2Pro | poly-mersenne.deg3 | 2.37 | 58.26 | 1 / 1 | 3.04 / 3.18 |
| M2Pro | poly-mersenne.deg4 | 2.37 | 65.27 | 1 / 1 | 3.49 / 3.43 |
| M2Pro | multiply-shift | 4.04 | 17.75 | 2 / 1 | 0.25 / 0.68 |
| M2Pro | pair-multiply-shift | 5.90 | 18.08 | 1 / 1 | 5.73 / 3.87 |
| M2Pro | tabulation-64 | 7.43 | 31.70 | 1 / 1 | 2.77 / 2.33 |
| M2Pro | SipHash-2-4 | 0.59 | 73.41 | 2 / 2 | 0.00 / 0.15 |
| M2Pro | SipHash-1-3 | 1.40 | 40.65 | 1 / 1 | 25.00 / 24.99 |
| M2Pro | chainhash-256 | 31.42 | 53.03 | 1 / 1 | 44.79 / 39.62 |
| M2Pro | chainhash-1k | 48.46 | 28.76 | 1 / 1 | 173.79 / 158.80 |
| M2Pro | HalfSipHash | 0.30 | 103.19 | 1 / 2 | 3.45 / 0.73 |
| M2Pro | mir.exact | 1.86 | 34.63 | 1 / 1 | 1.09 / 0.72 |
| M2Pro | mum3.exact.unroll1 | 2.38 | 26.17 | 2 / 2 | 1.28 / 2.33 |
| M2Pro | mum3.exact.unroll2 | 4.29 | 23.19 | 1 / 1 | 3.37 / 1.85 |
| M2Pro | mum3.exact.unroll3 | 7.54 | 23.42 | 1 / 1 | 4.00 / 2.09 |
| M2Pro | mum3.exact.unroll4 | 14.12 | 23.16 | 2 / 1 | 6.17 / 0.82 |
| M2Pro | mx3.v1 | 3.39 | 45.56 | 1 / 1 | 3.04 / 0.29 |
| M2Pro | mx3.v2 | 3.31 | 47.41 | 1 / 1 | 1.22 / 1.35 |
| M2Pro | mx3.v3 | 4.32 | 43.23 | 2 / 2 | 0.47 / 2.61 |
| M2Pro | UMASH-64 | 14.27 | 33.52 | 2 / 2 | 0.42 / 2.00 |
| M2Pro | UMASH-128 | 7.82 | 41.84 | 1 / 2 | 0.51 / 0.72 |
| M2Pro | UMASH-64.reseed | 14.07 | 34.25 | 2 / 2 | 0.79 / 1.11 |
| M2Pro | poly1305-hash | 2.20 | 156.96 | 2 / 2 | 0.92 / 2.25 |
| M2Pro | ghash | 2.48 | 1382.75 | 2 / 2 | 3.77 / 5.09 |
| Xeon8375C | SipHash-1-3 | 1.01 | 70.58 | 1 / 1 | 0.00 / 0.44 |
| Xeon8375C | SipHash-2-4 | 0.53 | 95.55 | 1 / 2 | 0.00 / 0.15 |

Spreads above 10%: M2Pro komihash: bulk 6.67%, small 10.76%; M2Pro rust-ahash-fb: bulk 14.25%, small 14.68%; M2Pro SpookyHash2-64: bulk 26.30%, small 22.60%; M2Pro HighwayHash-128: bulk 13.61%, small 13.89%; M2Pro SipHash-1-3: bulk 25.00%, small 24.99%; M2Pro chainhash-256: bulk 44.79%, small 39.62%; M2Pro chainhash-1k: bulk 173.79%, small 158.80%.

**Large M2 timing variation limits interpretation.** ChainHash-1k reports 48.46 versus 17.70 bytes/cycle and 28.76 versus 74.43 small-key cycles: bulk spread 173.79%, small spread 158.80%. Both runs used the same binary and default arguments. The headline deliberately retains the specified best-of-two rule; it is not a stable repeated estimate for this row. The experiment records varying load and uses the harness’s per-process calibrated M2 cycle estimates, but does not establish the cause of this spread. No extra runs, rescaling, or discarded outliers were introduced.

M2 uses calibrated monotonic-clock cycle estimates; Xeon uses TSC ticks. These are not interchangeable physical core cycles. The printed GiB/sec conversions assume a hard-coded 3.5 GHz on both hosts. Meeting the start gate does not establish sustained quiet load during the experiment.

`ghash` is the inherited fixed-IV OpenSSL GMAC proxy, including AES-128 key setup and the seed-dependent tag mask, not bare GHASH. `poly1305-hash` includes per-call seed expansion, key setup, EVP processing and finalization. Small-input figures include all that overhead. UMASH/CLhash preserve the preceding adapters and verification semantics; CLhash and UMASH reseed seed setup is outside the Speed timed hash loop.

Deliverables: `speeds_m2_v2.json`, `speeds_xeon_add.json`, raw outputs and per-run records under `out/`, this report, and `ISSUE_DRAFT_smhasher3.md` (draft only; not filed). `collect_results.py` validates raw SHA-256 hashes, binary identity, output parsing, and best-of-two selection. Reproduction scripts are included.
