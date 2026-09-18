# abseil-hash: seed-independent collisions

**Hash.** `absl::Hash` from [abseil-cpp](https://github.com/abseil/abseil-cpp) (Google,
Apache-2.0), the default hasher of `absl::flat_hash_set` / `flat_hash_map` and of
`absl::Hash<std::string>` / `std::string_view` / `absl::Cord`.  Pinned at commit
`73d2688300440c8af028eec865ee0dcd85e93025` (master, 2026-09-17); its
`absl/hash/internal/hash.h` (sha256 `65f71f11…e288`) and `hash.cc` (`f2b6084b…857e`) are
the files LTS 20260817.0 (2026-08-18) ships.  For byte strings the algorithm is
`CombineContiguousImpl` (hash.h): `len <= 8`, `9..16` and `17..32` are three fixed
multiply-fold formulas built on `Mix(a, b) = hi64(a*b) ^ lo64(a*b)`; `len > 32` is
`LowLevelHash`, whose backend depends on the build (scalar on the x86-64 default build,
x86 AES-NI with `-msse4.2 -maes`, ARM crypto on arm64 macOS by default).  The pairs below
are at most 16 bytes long, so they are the same on every 64-bit build with the shipped
`ABSL_OPTION_INLINE_HW_ACCEL_STRATEGY 0`.

**Seed protocol, as the library uses it.**  Every entry point ends in
`MixingHashState::hash_with_seed(value, seed)`:

* `absl::Hash<T>{}(x)` uses `Seed()`, the *address* of a global
  (`&MixingHashState::kSeed`): a fixed number in a non-PIE binary, an ASLR-slid one in a
  PIE binary.  The header says of it: "It is not meant as a security feature right now".
* `flat_hash_set` / `flat_hash_map` call `hash_internal::HashWithSeed().hash(hasher, key,
  seed)` with the table's `PerTableSeed`, a 5-bit random value shifted left by 6, i.e. one
  of the 32 seeds `{0, 64, ..., 1984}`.

The programs here sample the full 64-bit seed of `HashWithSeed` (the post's model), the 32
table seeds exhaustively, and (in `native/`) the process seed of a real binary and the seeds
of real containers.  The pairs collide under all of them.

**What the pairs exploit.**  For `len <= 8` the whole hash is
`Mix(seed ^ D(len) ^ v, kMul)`, where `v` is a public packing of the bytes (`Read4To8`:
`load32(p) << 32 | load32(p + len - 4)`; `Read1To3`: `p[0] << 16 | p[len/2] << 8 | p[len-1]`;
`0x57` for the empty string) and `D(len)` is the "length mix": an unaligned 8-byte window of
the constant table `kStaticRandomData` at byte offset `len`.  The seed and the length both
enter by XOR into the same multiplicand of one fixed map, so any two `(len, bytes)` with
equal `v ^ D(len)` collide for **every** seed: `m' = bytes8(D(0) ^ D(8) ^ 0x57)` is the
8-byte twin of the empty string (pair 1), and every string of 1..8 bytes has such a twin
(pair 2 is `"a"`).  For `9..16` bytes the hash is `Mix(seed ^ D(len) ^ w0, kMul ^ w1)` with
`w1` the last 8 bytes, so every string ending in the 8 bytes of `kMul` hashes to 0 for
every seed (pair 3, a same-length pair; the upstream comment next to that code expects an
"unlucky seed", but the `kMul` side is seed-free).

## Pairs

| # | mechanism | m | m' | rate over random seeds |
|---|-----------|---|----|------------------------|
| 1 | 0 vs 8 bytes (L = 1), length-mix cancellation; the post's row | `` (empty) | `a6e02637c07bd386` | 1 (2^0), all seeds |
| 2 | 1 vs 8 bytes (L = 1), the same identity for `"a"` | `61` | `44b53d572db1948b` | 1 (2^0), all seeds |
| 3 | 16 vs 16 bytes (L = 2), last word = `kMul` zeroes a multiplicand; the post's `other_pair` | `6162636465666768f58c1edee0f9d579` | `4142434445464748f58c1edee0f9d579` | 1 (2^0), all seeds; both hashes are 0 |

Explicit values from the real library (any seed works; these two are asserted by the
program): seed `0` → pair 1 `2bda3ac53577c4b7`, pair 2 `a65e8ab2c950c927`; seed `0x4055c8`
(the process seed of a non-PIE Linux build seen during the search) → `06bb4292e0eae907` and
`e3a0eef311f0006a`; pair 3 gives `0000000000000000` at every seed.

The score is exact: rate 1 at L = 1 gives 0 bits.  Sampling only confirms that the
implementation is the real `absl::Hash`; the post's sample size, the 32 table seeds plus
2^28 uniform seeds, is the `28` run below.

## Build and run

    cc -O2 -std=c11 -o abseil_hash_verify abseil_hash_verify.c -lm   # scalar LowLevelHash (x86-64 default)
    cc -O2 -std=c11 -msse4.2 -maes -o abseil_hash_verify_aes abseil_hash_verify.c -lm   # x86 AES-NI build
    ./abseil_hash_verify            # 2^20 random seeds (default), 0.2 s of CPU
    ./abseil_hash_verify 28         # the row's 2^28 seeds, about 45 s of CPU (Xeon 8375C)
    ./abseil_hash_verify 24 7       # different RNG master seed
    ./abseil_hash_verify --vectors  # print the vector schedule, to diff against native/absl_check vectors

On arm64 macOS the compiler enables the ARM-crypto backend by default and the program
validates against vectors recorded from the real library on that build; `-DABSL_VERIFY_SCALAR`
forces the scalar backend anywhere.  The first line of the output names the backend.

The program aborts unless (1) all 325 recorded `(len, seed, hash)` vectors from the real
library reproduce (165 for `len 0..32`, identical on every build, plus 160 for
`len 33..8192` of the backend compiled in), (2) the pair recipes re-derive the published hex
from `kStaticRandomData` and `kMul`, and (3) the recorded hash values at seeds `0` and
`0x4055c8` reproduce.  Then every pair must collide on all 32 SwissTable seeds and on all
N sampled seeds.  Exit status 0 only if every check passes; a non-numeric or out-of-range
argument is rejected with status 2.  Single-threaded, reads no files, writes only stdout.

## Expected output

Xeon 8375C, GCC 11.5, `cc -O2 -std=c11`, default arguments (2^20 seeds, 0.17 s of CPU), exactly as
saved in `run_2p20_xeon_scalar.txt`:

```
absl::Hash key-free collision check  (LowLevelHash backend for len > 32: scalar (x86-64 default build))
hash: absl::Hash<std::string_view>, abseil-cpp 73d2688 (= LTS 20260817.0), re-implemented from absl/hash/internal/hash.{h,cc}
validation: 325 vectors recorded from the real library (len 0..8192, 5 seeds): 0 mismatches OK
validation: pair 1 recipe bytes8(D(0) ^ D(8) ^ 0x57) = a6e02637c07bd386 matches the published hex
validation: pair 2 recipe bytes8(D(1) ^ D(8) ^ 0x616161) = 44b53d572db1948b matches the published hex
validation: pair 3 last 8 bytes = kMul little-endian (f58c1edee0f9d579) on both strings: yes

Pair 1: 0-byte vs 8-byte cross-length pair (L = 1 word), key-free -- the post's row
   mechanism: len 0: Mix(seed ^ D(0) ^ 0x57, kMul); len 8: Mix(seed ^ D(8) ^ v, kMul) with v = load32(m')<<32 | load32(m'+4).
   Choosing m' so that v = D(0) ^ D(8) ^ 0x57 makes the two multiplicands identical for every seed.
  m  ( 0 bytes) = (empty)
  m' ( 8 bytes) = a6e02637c07bd386
  seed 0x0:      H(m) = 2bda3ac53577c4b7  H(m') = 2bda3ac53577c4b7  (real library: 2bda3ac53577c4b7)
  seed 0x4055c8: H(m) = 06bb4292e0eae907  H(m') = 06bb4292e0eae907  (real library: 06bb4292e0eae907)  recorded values reproduced, both COLLIDE
  SwissTable per-table seeds {0, 64, ..., 1984} (exhaustive): collisions = 32 / 32
  uniform 64-bit seeds, N = 1048576 (2^20): collisions = 1048576 / 1048576, rate = 1.000000, log2 rate = 0.000
  first random colliding seed 0xb3f2af6d0fc710c5: H(m) = H(m') = 216d4614c709b2b6

Pair 2: 1-byte vs 8-byte cross-length pair (L = 1 word), key-free -- non-empty variant of the same identity
   mechanism: len 1: v = p[0]<<16 | p[0]<<8 | p[0] = 0x616161 for 'a'; m' packs v = D(1) ^ D(8) ^ 0x616161.
  m  ( 1 bytes) = 61
  m' ( 8 bytes) = 44b53d572db1948b
  seed 0x0:      H(m) = a65e8ab2c950c927  H(m') = a65e8ab2c950c927  (real library: a65e8ab2c950c927)
  seed 0x4055c8: H(m) = e3a0eef311f0006a  H(m') = e3a0eef311f0006a  (real library: e3a0eef311f0006a)  recorded values reproduced, both COLLIDE
  SwissTable per-table seeds {0, 64, ..., 1984} (exhaustive): collisions = 32 / 32
  uniform 64-bit seeds, N = 1048576 (2^20): collisions = 1048576 / 1048576, rate = 1.000000, log2 rate = 0.000
  first random colliding seed 0xb3f2af6d0fc710c5: H(m) = H(m') = ee7feffd0ea4f277

Pair 3: 16-byte same-length pair (L = 2 words), key-free -- the post's other_pair
   mechanism: len 9..16: Mix(seed ^ D(16) ^ w0, kMul ^ w1) with w1 = the last 8 bytes.  With w1 = kMul the second
   multiplicand is 0, so every such string hashes to 0 for every seed (a 2^64-way multicollision).
  m  (16 bytes) = 6162636465666768f58c1edee0f9d579
  m' (16 bytes) = 4142434445464748f58c1edee0f9d579
  seed 0x0:      H(m) = 0000000000000000  H(m') = 0000000000000000  (real library: 0000000000000000)
  seed 0x4055c8: H(m) = 0000000000000000  H(m') = 0000000000000000  (real library: 0000000000000000)  recorded values reproduced, both COLLIDE
  SwissTable per-table seeds {0, 64, ..., 1984} (exhaustive): collisions = 32 / 32
  uniform 64-bit seeds, N = 1048576 (2^20): collisions = 1048576 / 1048576, rate = 1.000000, log2 rate = 0.000
  first random colliding seed 0xb3f2af6d0fc710c5: H(m) = H(m') = 0000000000000000

ALL CHECKS PASSED: all three pairs collide for every SwissTable seed and every sampled 64-bit seed.
```

`make check` compares the counts in printed order: **`32 1048576 32 1048576 32 1048576`**.

The `-msse4.2 -maes` build prints the same numbers with backend `x86 AES-NI`
(`run_2p20_xeon_aes.txt`).  The row's sample size, `./abseil_hash_verify 28`, gives
`268435456 / 268435456` for all three pairs on both builds (`run_2p28_xeon_scalar.txt`,
`run_2p28_xeon_aes.txt`; 43 s and 39 s of CPU), as does the real library through `HashWithSeed`
(`native/run_pairs_2p28_xeon_{scalar,aes}.txt`, about 3 s each).

## Upstream check against the real library (`native/`)

`native/absl_check.cc` runs the same three pairs and the same vector schedule through the
real implementation, compiled from the pinned checkout: `absl::hash_internal::HashWithSeed().hash(absl::Hash<std::string_view>{}, sv, seed)`,
the hook `raw_hash_set` uses.  `make native` in this directory fetches the commit
(`git fetch --depth 1 origin 73d2688…`, then checks the two sha256s above), builds
`native/absl_check_scalar` (and `native/absl_check_aes` on x86-64), diffs the 325 vectors
against the C program's `--vectors` output (identical on both builds), prints the process
seed protocol (`Seed()` = `&kSeed`; `absl::Hash<string_view>{}(x)` = `absl::Hash<string>{}(x)`
= `hash_with_seed(x, Seed())`; the three pairs collide under it) and runs the pairs on the
32 table seeds plus `2^LOG2N` uniform seeds with the same RNG stream as the C program.  The
recorded Xeon run is `native/run_native_xeon.txt`; `native/run_pairs_2p28_xeon_{scalar,aes}.txt`
are the real library at the row's 2^28 sample size.

`make swiss` builds `native/swiss_check.cc` against the real containers (CMake,
`add_subdirectory(abseil-cpp)`): 800 real `flat_hash_set<std::string>` /
`flat_hash_map<std::string,int>` tables, reading each table's seed and `hash_of()` through
the `RawHashSetTestOnlyAccess` friend hook that `raw_hash_set.h` declares.  It checks that
every observed seed is in `{0, 64, ..., 1984}`, that `hash_of(k)` equals
`HashWithSeed().hash(Hash<string>{}, k, seed)`, and that the three pairs collide inside
every table (`native/run_swiss_xeon.txt`: 800 tables, 32 distinct seeds, exactly `{0, 64, ..., 1984}`, 0 of 1200 `hash_of` mismatches, 2400 of 2400 pair checks collide).

## Files

- `abseil_hash_verify.c` — single-file C11 program (MIT, full text in the header); the hash
  is re-implemented from `hash.h`/`hash.cc`, no Abseil source text is copied, and the two
  constant tables are Abseil's (Apache-2.0, Copyright 2018 The Abseil Authors).
- `Makefile` — `make` (the program), `make check`, `make native`, `make swiss`, `make clean`.
- `native/absl_check.cc`, `native/swiss_check.cc`, `native/CMakeLists.txt` — the checks
  against the real library; `native/abseil-cpp/` is the pinned checkout `make native` creates.
- `run_2p20_xeon_{scalar,aes}.txt`, `run_2p28_xeon_{scalar,aes}.txt`,
  `native/run_native_xeon.txt`, `native/run_pairs_2p28_xeon_{scalar,aes}.txt`,
  `native/run_swiss_xeon.txt` — the runs quoted above, exactly as printed.
- `README.md` — this file.
