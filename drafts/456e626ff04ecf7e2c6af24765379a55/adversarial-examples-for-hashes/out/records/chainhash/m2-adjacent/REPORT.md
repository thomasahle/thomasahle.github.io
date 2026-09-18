ChainHash adjacent pairing on Apple M2 Pro

The delivered `chainhash_arm.h` implements the adjacent-pair function from
`SPEC.md`: **1024-byte blocks, S=1, 137 words / 1096 key bytes**, canonical
little-endian input and output. Defining `CHAINHASH_X86_BLOCK_BYTES=256`
selects the separate **41-word / 328-byte** definition. Its batch-1, batch-2,
batch-4 and batch-8 entry points are byte-identical; batching changes the
evaluation order, not the function. The supplied `chainhash_x86.h` is unchanged
and remains the portable reference and key constructor implementation.

The fastest measured adjacent variant was **chainhash-adjacent-1k at 11.19
SMHasher3 bytes/cycle**. The best 256-byte variant was **chainhash-adjacent-256 at
10.47**, versus **11.19** for
the 1 KB definition. The table below reports medians of exactly three full
accepted Speed runs. All performance outliers in those samples are retained.
These are this harness's calibrated
monotonic-clock cycle estimates, not hardware PMU core-cycle measurements.
The harness's printed GiB/s assumes 3.5 GHz; it is not an independently
measured bandwidth. No timer, benchmark loop, or timing conversion was changed.

| Implementation | Fixed 256 KiB B/cycle | Variable bulk B/cycle | 1–31 byte cycles/hash | Bulk vs old same block size |
| --- | --- | --- | --- | --- |
| chainhash-256 | 22.29 | 21.44 | 73.69 | +0.0% |
| chainhash-adjacent-256 | 10.47 | 10.41 | 88.32 | -53.0% |
| chainhash-adjacent-256-b2 | 10.46 | 10.45 | 88.50 | -53.1% |
| chainhash-adjacent-256-b4 | 10.45 | 9.95 | 87.94 | -53.1% |
| chainhash-adjacent-256-b8 | 10.31 | 9.95 | 88.91 | -53.7% |
| chainhash-1k | 18.95 | 17.92 | 75.05 | +0.0% |
| chainhash-adjacent-1k | 11.19 | 11.08 | 89.85 | -40.9% |

The 1 KB median is 6.9% faster than
the best 256-byte median. None of batch-2, batch-4 or batch-8 improved the unbatched 256-byte median in these samples.
The delivered adjacent kernels are slower than the old strided controls
at both block sizes; removal of explicit shuffles does not by itself
guarantee higher throughput on this host.

The old `chainhash-256` has strided pairs and S=1. The old `chainhash-1k`
has strided pairs and **S=2**, while the new 1 KB definition has **S=1**.
Thus the 1 KB before/after comparison includes both pairing and recurrence
frequency changes. The strided and adjacent families are not output-compatible.
The adjacent 256-byte batch rows share one digest with each other; they
do not share the 1 KB digest. A batch of four 256-byte steps is not one
1024-byte PH block.
The old SMHasher adapter also caches derived key terms for its short-input
path; the new key is exactly the specified words with no cache. The new
partial-pair path uses a bounded 16-byte scratch copy. Short-input timings
therefore include these implementation differences as well as the new pairing.

Controls, measured in the exact same executable:

| Control | Bulk B/cycle | Small cycles/hash |
| --- | --- | --- |
| chainhash-256 | 22.29 | 73.69 |
| komihash | 8.15 | 24.44 |
| rapidhash | 15.39 | 20.63 |
| XXH3-64 | 12.70 | 24.93 |

Throughput ratios use each row's median divided by the control's median:

| Variant | / old chainhash-256 | / komihash | / rapidhash | / XXH3-64 |
| --- | --- | --- | --- | --- |
| chainhash-adjacent-1k | 0.502× | 1.373× | 0.727× | 0.881× |
| chainhash-adjacent-256 | 0.470× | 1.285× | 0.680× | 0.824× |
| chainhash-adjacent-256-b2 | 0.469× | 1.283× | 0.680× | 0.824× |
| chainhash-adjacent-256-b4 | 0.469× | 1.282× | 0.679× | 0.823× |
| chainhash-adjacent-256-b8 | 0.463× | 1.265× | 0.670× | 0.812× |

All three fixed-bulk and small-key measurements are retained here; the JSON
also retains variable-length bulk, random small-key results, load samples,
commands, timestamps, binary/raw-file SHA-256 values and outlier flags.

| Name | Bulk runs B/cycle | Bulk median | Small runs cycles/hash | Small median |
| --- | --- | --- | --- | --- |
| chainhash-adjacent-1k | 11.13, 11.24, 11.19 | 11.19 | 89.85, 88.93, 91.23 | 89.85 |
| chainhash-adjacent-256 | 10.30, 10.67, 10.47 | 10.47 | 88.32, 85.26, 88.73 | 88.32 |
| chainhash-adjacent-256-b2 | 10.46, 10.08, 10.51 | 10.46 | 87.15, 90.32, 88.50 | 88.50 |
| chainhash-adjacent-256-b4 | 10.34, 10.47, 10.45 | 10.45 | 87.94, 87.88, 88.54 | 87.94 |
| chainhash-adjacent-256-b8 | 10.31, 10.21, 10.39 | 10.31 | 88.21, 89.18, 88.91 | 88.91 |
| chainhash-256 | 22.29, 21.60, 22.44 | 22.29 | 72.10, 74.61, 73.69 | 73.69 |
| chainhash-1k | 18.95, 18.56, 19.44 | 18.95 | 74.88, 77.53, 75.05 | 75.05 |
| komihash | 8.15, 7.95, 9.03 | 8.15 | 24.44, 25.08, 22.06 | 24.44 |
| rapidhash | 15.39, 15.28, 21.73 | 15.39 | 20.63, 20.76, 14.47 | 20.63 |
| XXH3-64 | 12.70, 12.66, 13.08 | 12.70 | 24.93, 25.00, 24.68 | 24.93 |

An outlier means an absolute deviation **greater than 15% from that metric's
three-run median**. The JSON additionally records the max/min range, which
is a different measure. Outlier results are retained in the medians.
Flagged measurements:

- rapidhash, bulk_bytes_per_cycle: runs 15.39, 15.28, 21.73; deviations +0.0%, -0.7%, +41.2%
- rapidhash, variable_bulk_bytes_per_cycle: runs 15.39, 15.28, 21.23; deviations +0.0%, -0.7%, +37.9%
- rapidhash, small_cycles_per_hash: runs 20.63, 20.76, 14.47; deviations +0.0%, +0.6%, -29.9%
- rapidhash, random_small_cycles_per_hash: runs 20.63, 20.78, 14.49; deviations +0.0%, +0.7%, -29.8%

Every Speed process started only after confirming no SMHasher process was
running and the one-minute load was strictly below 4.5. Accepted start loads
ranged from **2.5103 to 4.4980**. The runner waited without
a timeout override. It rotated run order over three passes and checked for
other SMHasher processes every five seconds during each run.
No overlapping SMHasher process was detected in the accepted runs.

One attempt was archived and replaced because another SMHasher process was observed, independent of measured speed. The original attempts remain in the JSON `excluded_runs` and their raw files. They are excluded from the three accepted samples per variant.

The gate constrains startup load; the full during-run load history remains
in the JSON. macOS scheduling and background activity remain possible sources
of variation; no CPU affinity or hardware frequency measurement is claimed.

The pairing change affects level 1. One
`LD2 {v0.2d,v1.2d}` loads `[w0,w2]` and `[w1,w3]`; another does the same for
the key. XOR followed by pinned `PMULL` and `PMULL2` computes exactly
`(w0^k0)(w1^k1)` and `(w2^k2)(w3^k3)`. Counts below are measured from Apple
clang output for a **32-byte / two-product** group. `LDP` loads two registers.
The naive adjacent control applies two `vuzp*_u64` operations after XOR;
clang emits their equivalent `ZIP1/ZIP2` form for two 64-bit lanes.

| 32-byte group | LDP | LD2 | Pairing shuffles | PMULL | PMULL2 |
| --- | --- | --- | --- | --- | --- |
| Old strided | 2 | 0 | 0 | 1 | 1 |
| Adjacent ordinary-load audit control | 2 | 0 | 2 | 1 | 1 |
| Adjacent delivered LD2 | 0 | 2 | 0 | 1 | 1 |

The delivered PH loop processes 64 bytes with four LD2s (two message, two
key), two PMULLs, two PMULL2s, four EORs and two EOR3 accumulations. It has
two independent accumulators, folded once after each PH block. There are
16 PH products per 256 bytes and 64 per 1 KB. All PH loops in **the actual
benchmark object files** have zero pairing shuffles, zero SIMD stack accesses,
and zero SIMD/GPR transfers. Counting both message and key loads, each full
256-byte block has 16 LD2 instructions; each 1 KB block has 64. See
`evidence/actual-object-loop-audit.json`,
`evidence/instruction_counts.json` and the retained disassemblies. LD2 does
de-interleaving in the load instruction; this does not establish that it has
the same micro-op cost or throughput as LDP. The old ARM strided loop was
already shuffle-free, unlike the old x86 ZMM layout.

PMULL scheduling and batching: field elements remain in lane 0. Inline asm
pins low products to PMULL and high products to PMULL2, avoiding clang's
DUP/PMULL2 and SIMD/GPR round trips. Reduction uses two dependent PMULL2
folds with 27; the addend XOR overlaps those folds. A coefficient-side EXT
outside the PH loop brings `b+y` into lane 0; the recurrence state itself
does not move lanes. Final length duplication, tail assembly, finalization
and the final result transfer are outside the full-block PH loop.

For nonfinal blocks let `Q=P^u`, `A=a^u`, `B=b^y`. Each step is `F(Q)=A^B*Q`.
For consecutive left/right steps, composition gives
`A'=Ar ^ Br*Al`, `B'=Br*Bl`. A balanced tree composes 2, 4 or 8 steps entirely
from message/key coefficients before one multiplication uses the incoming Q.
This identity holds even when a slope is zero; no division or key restriction
is introduced. Coefficient depth is log2(k); total field multiplications
are `2k-1` per batch, versus k without batching. Each field multiplication
uses three PMULL-family instructions with this reducer:

| 256-byte batch k | PH products/batch | Field multiplies/batch | PMULL-family instructions/256 B |
| --- | --- | --- | --- |
| 1 | 16 | 1 | 19 |
| 2 | 32 | 3 | 20.5 |
| 4 | 64 | 7 | 21.25 |
| 8 | 128 | 15 | 21.625 |

Those counts exclude the once-per-message finalizer and final/remainder
handling. The final block is always peeled: only it receives total length
in both PH halves, and its step yields P directly. Incomplete batches use
ordinary sequential steps. Only the last active 16-byte pair is padded;
1–8 byte messages activate one product and the empty message activates none.
The API never forms `len+B-1` and never reads beyond the input.

Apple clang initially unrolled the 256-byte PH kernels. In batch-4/batch-8
that produced 39/86 static spill instructions across the whole standalone
driver (including ABI and tail handling). Disabling PH-loop unrolling
reduced both to five, all outside the bulk loops. The actual benchmark
outer loops also contain no SIMD stack accesses. No performance claim is
based on the rejected unrolled version; it was changed before Speed timing.
The rolled loop intentionally trades a small branch/address cost for bounded
register pressure. The default API keeps batch=1; batching remains explicit.
The rolled PH loop reloads canonical key words as it advances. A separately
tuned unrolled 256-byte kernel with more key values held in registers was
not timed. These results characterize the delivered kernels and do not
establish an ARM hardware ceiling or the best possible adjacent implementation.

Verification ran locally on this M2, for both block definitions. Each used
20,535 complete input cases: 12,000 random lengths in 0..4096 with fresh random
keys/data and offsets 0..63; every length 0..4096; 4,097 input tails ending
at a guard page; 16 long lengths through 1 MiB; 120 block/batch boundary
cases; all-zero and all-one keys/data; high-bit keys; and null empty input. Every
NEON entry point matched both `chainhash_x86_portable` and an independent
SPEC-derived bit-serial oracle. The latter accumulates a shifting 128-bit
polynomial then performs high-to-low polynomial long division, separately
from the canonical reference's field shift/reduction recurrence. The tests
also cover all 128 reduction basis polynomials and 10,000 random affine
compositions, including zero slopes. Key-byte/word constructors and exact
key sizes are checked. Equality of the returned 64-bit words gives equality
of their canonical eight-byte LE serializations.

The full identity suite passed again under UBSan. A separate guarded-memory
suite passed 8,194 short cases per definition with protected pages before
and after valid regions, plus long inputs through 1 MiB. C99 NEON and
forced-portable smoke builds passed. All five final SMHasher registrations
passed Sanity, including thread-safety checks. The canonical endian hooks
have constants `0x363953CA` (1 KB) and `0xDB5810D3` (all 256-byte batches),
also derived with the portable API. Duplicate-code registration warnings
for the batching variants are expected because they implement one function.

AddressSanitizer could not reach main on macOS 27.0 / Apple clang 17.0.0:
the captured stack shows recursive ASan initialization through dyld's
allocator and a spin mutex. Those processes were stopped before timing;
**no ASan pass is claimed**. See `evidence/asan-runtime-status.txt` and
`evidence/memory-startup-sample.txt`. UBSan, guard pages and the independent
identity checks above completed successfully.

Build and reproduction: `smhasher3/` is a scratch copy of the supplied
`../m2-rerun/smhasher3`. Existing controls, `chainhash.cpp`, SpeedTest.cpp and
timer sources are unchanged, verified by SHA-256. `evidence/provenance.json`
records the exact identical CXX flags, compiler and OS. The final binary
SHA-256 is `59f0b173d9d6fbb2e2eb5ccd71540ecd7dbc5159601a3ee25006c6efdf1b60ca`. Key expansion is outside timed hashes,
as for the existing seeded-key registration; no key cache was added to the
new header. The native baseline remains the M2's `-march=native` plus AES
feature, with `-O3`, C++11, arm64 and the same explicit macOS SDK.

```sh
python3 run_tests.py
cp chainhash_arm.h chainhash_x86.h chainhash_smhasher3_adjacent* smhasher3/hashes/
cmake -S smhasher3 -B build -DCMAKE_BUILD_TYPE=Release \
  -DENDIAN_DETECT_BUILDTIME=OFF -DDETECTED_LITTLE_ENDIAN=ON \
  '-DCMAKE_CXX_FLAGS=-Xclang -target-feature -Xclang +aes' \
  -DCMAKE_OSX_SYSROOT=/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DOPENSSL_ROOT_DIR=/opt/homebrew/opt/openssl@3
cmake --build build --target SMHasher3 -j4
python3 run_speeds.py
python3 replace_overlaps.py
python3 make_report.py
```

`run_speeds.py` resumes completed records only for an identical binary; use
a fresh evidence/speeds directory for a new measurement campaign. The raw
Speed outputs and gate log are under `evidence/speeds/`. `run_tests.py`
reproduces the arithmetic, memory, C99 and standalone assembly checks.
The header requires `chainhash_x86.h` alongside it. The separate SMHasher
adapters and common adapter header are delivered beside the new header.
