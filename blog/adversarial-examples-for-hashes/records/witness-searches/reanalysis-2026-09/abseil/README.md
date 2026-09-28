# absl::Hash (abseil-cpp 73d2688 = LTS 20260817.0): every-seed families by build

absl::Hash has three compile-time variants that matter here. Which one a program gets depends on
`ABSL_OPTION_INLINE_HW_ACCEL_STRATEGY` (default 0) and on the target:

| build | <= 16 bytes | 17..32 bytes | > 32 bytes (LowLevelHash) |
|---|---|---|---|
| default, x86-64 without `-maes`, Linux aarch64 | scalar Mix path | scalar | scalar Mix32Bytes lanes |
| default, Apple silicon (Apple clang defines `__ARM_FEATURE_CRYPTO`) | scalar Mix path | scalar | ARM AES lanes |
| default, x86-64 with `-msse4.2 -maes` | scalar Mix path | scalar | x86 AES lanes |
| HW-accelerated (strategy 1 or 2, CRC32C available) | CRC32C path for 9..32 bytes | CRC32C | as above |

All programs hash through `absl::hash_internal::HashWithSeed().hash(absl::Hash<std::string_view>{}, s, seed)`,
the hook `raw_hash_set` uses, against the real library; "SwissTable seeds" are the 32 per-table seeds.

## 1. Default build, 9..16 bytes: 10-byte family, every member hashes to 0

The 9..16-byte path returns Mix(seed ^ D(n) ^ load64(m), kMul ^ load64(m + n - 8)) and Mix(x, 0) = 0.
Any string whose last 8 bytes are kMul (f58c1edee0f9d579 in memory order) hashes to 0 for every seed:
at 10 bytes, 2 free bytes give 2^16 members (`0000f58c1edee0f9d579`, `0100f58c1edee0f9d579`, ...);
2^64 at 16 bytes. (The kMul observation is in abseil/abseil-cpp#2171, 19 September 2026.)

| program | host / build | result | log |
|---|---|---|---|
| `absl_native_kf.cc` | x86-64 default | 10 B family: 65,536/65,536 seeds, 32/32 table seeds | `logs/absl_native_default_F10_default.txt` |
| `kf_abseil.cpp` (transcription, validated 132 vectors x 2 builds, 0 mismatches) | x86-64 | 4096/4096 seeds, 32/32 | `logs/kf_abseil_xeon_2p24.txt` |
| `indep_absl.cc` | M2 default (Apple silicon) | 65,536 members, 4096/4096 seeds (common value 0 on all), 32/32 | `logs/indep_absl_default_build_m2.txt` |
| same family, HW-accelerated build | x86-64 and M2 | 0/65,536 and 0/4096 seeds (the CRC path replaces this branch) | `logs/absl_native_crc_F10_default.txt`, `logs/indep_absl_hwaccel_strategy1_m2.txt` |

## 2. HW-accelerated build, 9..32 bytes: CRC32C kernel families

The CRC path absorbs each 8-byte read with crc32c, which is GF(2)-affine in the data with the seed
only in the initial value; the result Mix(mul - crc1, crc2 - mul) uses no further seed. Two
equal-length strings whose difference has crc32c(0, .) = 0 on every 8-byte read collide for every
seed. ker crc32c(0, .) on 64 bits has dimension 32, so the families are subspaces: 2^24 at 9 bytes,
2^16 at 10, 2^24 at 11, 2^32 at 12 ... 2^64 at 16 bytes (measured, `kernel_dims.cc`), and at least
2^128 at 32 bytes (four reads, not run).

Shortest pair (9 bytes; lengths <= 8 use bijective CRC reads):

    m  = 414243444546474849   ("ABCDEFGHI")
    m' = 41b335a84047474849

Both give 3146e4736b5c9f18 at seed 0 and c3b38cd616e9b537 at seed 1; they are equal for every seed,
but the common value depends on the seed.

| program | host / build | result | log |
|---|---|---|---|
| `hwaccel_check.cc` (+ `CMakeLists.txt`) | x86-64, strategy 1, `-msse4.2 -maes` | lengths 9..16, each family: 32 table seeds x 2^16 members 2,097,152/2,097,152; uniform seeds 16,777,216/16,777,216 per pair; real `flat_hash_set<std::string>` with 2^16 nine-byte members: 1 distinct hash, 1 H2 byte (random strings: 65,536 and 128) | `logs/hwaccel_check_x86_strategy1_2p24.txt` |
| `absl_native_kf.cc` + `pairs/crc_and_default_pairs.txt` | x86-64, strategy 1 | 9 B pair 000000000000000000 / 00f176ec0501000000: 16,777,216/16,777,216; 10 B family (b499258007be2a0a211b, ...): 65,536/65,536 | `logs/absl_native_crc_*.txt` |
| `indep_absl.cc` | M2, strategy 1, `-march=armv8-a+crc+crypto` | 9 B pair: 4096/4096 seeds | `logs/indep_absl_hwaccel_strategy1_m2.txt` |

Note: `absl_native_kf.cc` prints its build label from a macro that the public header does not define,
so the `_crc` logs say "default path" although that binary was built with strategy 1; the results
(the CRC pairs collide, the default-build pair does not) show which path ran.

Also in this build: `absl::Hash<uint64_t>` (CombineRawImpl, two crc32c lanes of v and 3v) has
every-seed integer pairs, e.g. 0xd4c5434b454823a5 and 0xc8823c07fd48c0dd (`int_pair_search.cc`,
`logs/int_pair_search.txt`).

## 3. Longer than 32 bytes

Inputs of 33..1024 bytes go to LowLevelHashLenGt32; longer inputs are 1024-byte pieces with chained
state and no length mixing. absl::Hash returns this value with no seed-reinjecting finalizer.

- Scalar (default x86-64, Linux aarch64): each 64-byte chunk updates lane_i = Mix(word ^ K_i,
  other ^ lane_i). A chunk whose words 0, 2, 4, 6 equal kStaticRandomData[1..4] sends all four lanes to
  Mix(0, .) = 0, erasing the seed, the length and everything before it; words 1, 3, 5, 7 are free.
  Every-seed collisions from 65 bytes to 1 MiB, at any 64-byte chunk.
- ARM AES (Apple silicon default): lane <- InvMC(InvSB(InvSR(m ^ s))) with no feed-forward; each
  1024-byte piece restarts from Set128(state, len) whose low 8 bytes are the public length, so a
  difference in the first 8 bytes of a lane word of the first chunk of a piece is a known S-box
  difference that the next chunk's word cancels: every-seed collisions at every piece start. Elsewhere a
  one-S-box trail gives about 2^-6.
- x86 AES (`-maes`): trails only, 2^-16.1 at piece starts, 2^-23.0 elsewhere.

| check | result | log |
|---|---|---|
| scalar reset chunk, real library, x86-64 default (`absl_long.cc`) | 65 B pair 1,048,576/1,048,576 seeds; 256 B, 1 KiB, 64 KiB 1,048,576/1,048,576 each; 1 MiB 1024/1024; 525 B vs 909 B pair 1,048,576/1,048,576; 2^16 family at 1000 B, 1024/1024 | `logs/absl_long_scalar_2p20.txt` |
| scalar 65 B pair, real library, M2 built without the crypto extension (scalar path) | 65,536/65,536 seeds, 32/32 (the ARM AES build: 0/65,536) | `logs/indep_absl_pairs_m2.txt` |
| ARM AES piece start, real library, M2 default (`absl_long.cc`) | 128 B, 1 KiB, 64 KiB 65,536/65,536 each; 1 MiB 4096/4096; 2^16 families at 128 B and 4173 B, 1024/1024 | `logs/absl_long_arm_2p16.txt` |
| ARM AES 128 B pair, M2 default, separate harness | 65,536/65,536 seeds, 32/32 (scalar build: 0/65,536) | `logs/indep_absl_pairs_m2.txt` |
| ARM AES one-S-box trail anywhere | 256 B 1044/65,536 = 2^-5.97; 1 KiB 2^-6.01; 64 KiB 2^-5.94; 1 MiB 2^-6.04 | `logs/absl_long_arm_2p16.txt` |
| x86 AES piece start | 13/2^20 .. 18/2^20 (about 2^-16) | `logs/absl_long_aes_chunk0.txt` |

Build (real library; `<abseil>` is an abseil-cpp checkout at 73d2688):

    c++ -O2 -std=c++17 -I<abseil> indep_absl.cc <abseil>/absl/hash/internal/{hash,city}.cc -o indep_absl
    c++ -O2 -std=c++17 -I<abseil> indep_absl_pairs.cc <abseil>/absl/hash/internal/{hash,city}.cc -o indep_absl_pairs
    ./indep_absl 12 ; ./indep_absl_pairs 16 < pairs/arm_aes_piece_start_128B.txt
    c++ -O2 -std=c++17 -I<abseil> absl_long.cc <abseil>/absl/hash/internal/{hash,city}.cc -o absl_long   # add -msse4.2 -maes for x86 AES
    # HW-accelerated build: set ABSL_OPTION_INLINE_HW_ACCEL_STRATEGY to 1 in <abseil>/absl/base/options.h,
    # add -msse4.2 (x86-64) or -march=armv8-a+crc (arm64); hwaccel_check.cc builds with CMake (-DABSEIL_DIR=<abseil>)
