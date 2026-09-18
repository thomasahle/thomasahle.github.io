# ChainHash-128 evaluation

The delivered family is ChainHash over GF(2^128), with **512-byte blocks,
32 words, S=1, raw 256-bit PH accumulators and a 128-bit output**. The ideal-key
collision bound is (n+2)/2^128, including arbitrary byte lengths and empty
input. The length-normalized ideal score is **126.4150374993 bits**. Model A
uses 160 independent random bytes, expands PH from one 128-bit field seed,
and has an **at-most-length score of 125 bits**. The benchmark's 64-bit
SplitMix expansion is a timing fixture and satisfies neither key hypothesis.

`SPEC.md` gives the complete definition, proof transfer, seeded bounds,
length domains, score units and the remaining Lean work. This is a mathematical
extension supported by implementation tests; the new 128-bit construction has
not been machine checked in Lean.

## Construction and cost

| Family | Output | Block | Ideal random/resident key | Model-A random input | Ideal epsilon for ≤1 MiB |
|---|---:|---:|---:|---:|---:|
| ChainHash-64, supplied family | 64 bits | 256 B | 328 B | 80 B | 4098 / 2^64 |
| Two independently keyed ChainHash-64s | 128 bits | 256 B each | 656 B | 160 B | 4098² / 2^128 |
| ChainHash-128, 256-byte comparison | 128 bits | 256 B | 400 B | 160 B | 4098 / 2^128 |
| **ChainHash-128, default** | **128 bits** | **512 B** | **656 B** | **160 B** | **2050 / 2^128** |

The 512-byte choice halves the recurrence work per byte relative to 256 B
and doubles the ceiling imposed by the serial recurrence. It also improves
the large-message collision coefficient. Its cost is a larger resident PH
key (656 versus 400 bytes). Short messages touch the same number of groups in
both configurations. Their key layouts and block boundaries define distinct
hash families.

The implementation uses three 64×64 CLMULs for each 128×128 raw product via
Karatsuba, accumulating its three components before reconstructing the raw
256-bit block sum. The x86 wide path computes two products in parallel with
256-bit VPCLMULQDQ. The ARM path pins PMULL and PMULL2 with inline assembly.
A four-product schoolbook variant is supplied for comparison. Field reduction
uses two extra CLMULs and folds the remaining at-most-seven overflow bits with
shifts/XORs; it is checked on arbitrary 256-bit polynomials. Squaring needs
only two products because the cross terms cancel. The main loop carries
Q=P+u, and the final block handles both length masks.

The portable path is a direct bit-serial definition. The header is C99,
requires no external library or 128-bit integer extension, and includes
ideal-key and model-A constructors, explicit portable and hardware calls,
canonical serialization, and an embedded self-test. Dispatch is at compile
time, so executables must run on CPUs supporting their compilation flags.

## Measurement method and provenance

All rows for a host come from the **same SMHasher3 binary**. The existing
harness and control objects are preserved and linked with the new registration
objects. The remote tree is `~/agents/speedbench-ch128`, copied from
`~/agents/speedbench`; the Mac uses a copy of `../m2-rerun/smhasher3` and that
scratch job's already built control libraries, including native ARM UMASH.
No canonical repository is modified. `benchmarks/build_smh.py` records object
hashes and exact commands, and also updates the scratch source list for a
normal full rebuild.

Each Xeon hash receives two complete `--test=Speed` passes, retaining the
higher bulk result and lower small result independently. Each M2 hash receives
**three complete passes, with the median reported separately for bulk and
small**. The first two M2 passes are preserved and a third is added under the
resume instruction. Bulk is the reported Average over alignments 0..7 at
exactly 262144 bytes. Small is each run's arithmetic mean of the 1..31-byte
results; the median is taken across these three means, not across lengths.
`speeds_chainhash128.json` retains every run, all 31 length and eight alignment
results, raw-file hashes, launch conditions and the aggregation rule.

M2 values more than 15% above or below their three-run median are flagged;
no observations are discarded. Both summary metrics and individual
length/alignment diagnostics are checked. The report marks every flagged run,
and the JSON records each deviation. These diagnostic medians do not replace
the median of the per-run small averages in the main table.

Every Xeon run waits for ≥95% idle on both the chosen CPU and its SMT sibling
in a five-second sample and is pinned with taskset. Every Mac build, test and
initial timing launch used no SMHasher3 process and 1-minute load <3. Under
the explicit resume instruction, the third M2 pass and final probe use
**no SMHasher3 process and 1-minute load <4.5**. There is no load-gate timeout
override. These are launch conditions; other workloads can
start during a run. The recorded before/after load and repeat variation are
necessary qualifications on a shared machine.

SMHasher3 measures Xeon TSC reference ticks; on Mac it estimates cycles from
a per-process calibrated monotonic clock. They are **not interchangeable
physical core-cycle counters**. Its printed GiB/s assumes 3.5 GHz on both
hosts and is not used here. The throughput/latency probe uses the same timer
implementation and records the Mac calibration multiplier.

The initial probe used eight independent CLMUL chains. On the M2 its measured
reciprocal rate was close to instruction latency divided by eight, indicating
that dependencies could constrain that test. A final gated probe therefore
compares sixteen and twenty-four chains on the Mac and uses their faster
rate for the issue estimate. Both probes and their launch evidence are kept.
The Xeon's latency/rate ratio is about six, so eight chains provide enough
parallelism for its measured scalar issue rate.

<!-- BEGIN GENERATED RESULTS -->
## Same-binary speed results

Bulk is bytes per timer cycle (higher is better). Small is mean
cycles/hash for lengths 1–31 (lower is better). “Before” and “after”
identify the supplied 64-bit family and this 128-bit extension;
both are measured in the same binary, with the same controls.

### Xeon Platinum 8375C

Higher bulk and lower small result from two complete runs, selected independently.

| Hash | Output | Backend token | Bulk B/cycle | Small cycles/hash |
|---|---:|---|---:|---:|
| ChainHash-64, existing SMHasher3 | 64 | `hwclmul` | 14.37 | 103.91 |
| ChainHash-64, supplied header (before) | 64 | `pclmul` | 14.29 | 110.00 |
| Two ChainHash-64 evaluations | 128 | `pclmul` | 7.20 | 207.56 |
| ChainHash-128, 512 B, scalar Karatsuba | 128 | `pclmul` | 7.26 | 164.40 |
| ChainHash-128, 256 B | 128 | `vpclmul256` | 6.20 | 164.29 |
| ChainHash-128, 512 B, four products | 128 | `vpclmul256` | 7.42 | 160.99 |
| **ChainHash-128, 512 B (after)** | 128 | `vpclmul256` | 8.16 | 165.06 |
| UMASH-128 | 128 | `hwclmul` | 6.02 | 40.29 |
| komihash | 64 | unlabelled | 7.36 | 27.49 |
| rapidhash | 64 | unlabelled | 10.68 | 27.58 |
| XXH3-128 | 128 | `avx512` | 19.86 | 34.90 |

The default delivers 0.571× the supplied ChainHash-64 bulk throughput and uses 1.501× its small-key cycles. Relative to UMASH-128 the ratios are 1.355× bulk and 4.097× small-key cycles.
The 512-byte configuration has 1.316× the 256-byte configuration’s measured bulk throughput.

| Repetition range (both full passes) | Bulk B/cycle | Small cycles/hash |
|---|---:|---:|
| chainhash-64-header | 14.28–14.29 | 110.00–110.03 |
| chainhash-128.256 | 6.16–6.20 | 164.29–164.37 |
| chainhash-128 | 8.13–8.16 | 165.06–165.10 |
| UMASH-128 | 6.01–6.02 | 40.29–40.29 |

The one-minute load at launch ranged from 2.18 to 44.59; after each run it ranged from 2.29 to 24.10. The gate constrains launch conditions. It does not reserve the host against work started later; small performance gaps should be read alongside the repetition ranges and timer calibration caveat.

### Apple M2 Pro

Median of three complete runs per hash; no runs excluded.

| Hash | Output | Backend token | Bulk B/cycle | Small cycles/hash |
|---|---:|---|---:|---:|
| ChainHash-64, existing SMHasher3 | 64 | `hwpmull` | 22.03 | 73.02 |
| ChainHash-64, supplied header (before) | 64 | `pmull` | 21.98 | 83.67 |
| Two ChainHash-64 evaluations | 128 | `pmull` | 10.27 | 85.15 |
| ChainHash-128, 512 B, scalar Karatsuba | 128 | `pmull` | 10.34 | 160.94 |
| ChainHash-128, 256 B | 128 | `pmull` | 9.20 | 158.95 |
| ChainHash-128, 512 B, four products | 128 | `pmull` | 11.00 | 149.21 |
| **ChainHash-128, 512 B (after)** | 128 | `pmull` | 10.28 | 161.84 |
| UMASH-128 | 128 | `hwclmul` | 7.67 | 42.35 |
| komihash | 64 | unlabelled | 8.32 | 23.95 |
| rapidhash | 64 | unlabelled | 15.46 | 20.54 |
| XXH3-128 | 128 | `neon` | 12.97 | 29.90 |

The default delivers 0.468× the supplied ChainHash-64 bulk throughput and uses 1.934× its small-key cycles. Relative to UMASH-128 the ratios are 1.340× bulk and 3.821× small-key cycles.
The 512-byte configuration has 1.117× the 256-byte configuration’s measured bulk throughput.

The “scalar Karatsuba” and default registrations use the same PMULL
path on ARM; their differences measure run variation. On Xeon the
default and 256-byte registrations use two-lane VPCLMULQDQ.

All three runs follow. Each cell is **bulk B/cycle / small cycles/hash**.
A ★ flags a run with an absolute deviation greater than 15% from the
three-run median in a summary metric or any individual length/alignment.
Flags describe variation; flagged runs remain in the median.

| Hash | Run 1 | Run 2 | Run 3 |
|---|---:|---:|---:|
| chainhash-256 | 22.01 / 73.08 | 22.03 / 73.02 | 22.04 / 72.92 |
| chainhash-64-header | 21.88 / 84.51 | 22.45 / 81.88 | 21.98 / 83.67 |
| chainhash-64x2 | 10.26 / 85.15 | 10.27 / 85.31 | 10.34 / 83.97 |
| chainhash-128.scalar | 10.55 / 157.69 | 10.30 / 161.62 | 10.34 / 160.94 |
| chainhash-128.256 | 9.23 / 158.50 | 9.20 / 158.95 | 9.17 / 159.57 |
| chainhash-128.schoolbook | 11.25 / 145.82 | 10.95 / 150.06 | 11.00 / 149.21 |
| chainhash-128 | 10.26 / 162.26 | 10.28 / 161.84 | 10.75 / 155.42 |
| UMASH-128 | 7.67 / 42.35 | 7.83 / 41.36 | 7.58 / 43.98 |
| komihash | 8.32 / 23.96 | 8.32 / 23.95 | 8.39 / 23.51 |
| rapidhash | 15.34 / 20.67 | 15.74 / 20.18 | 15.46 / 20.54 |
| XXH3-128 | 12.97 / 29.90 | 13.01 / 29.82 | 12.89 / 30.10 |

**Summary-metric deviations greater than 15%:**

None.

**Individual-length/alignment diagnostics:**

None.

The one-minute load at launch ranged from 2.63 to 4.39; after each run it ranged from 2.63 to 11.91. The gate constrains launch conditions. It does not reserve the host against work started later; small performance gaps should be read alongside the repetition ranges and timer calibration caveat.

### Measured instruction and recurrence ceilings

All entries use the host’s SMHasher3 timer units. The issue estimates
include PH plus the five-CLMUL recurrence; the derivation follows below.

| Host | CLMUL latency | Scalar reciprocal throughput | VP256 reciprocal throughput | Recurrence latency |
|---|---:|---:|---:|---:|
| Xeon8375C | 5.011 | 0.829 | 1.657 | 21.765 |
| M2Pro | 2.803 | 0.233 | — | 27.685 |

| Host | Block | Scalar PH-only B/cycle | Wide PH-only B/cycle | Scalar PH + recurrence issue B/cycle | Wide PH + recurrence issue B/cycle | Recurrence latency B/cycle |
|---|---:|---:|---:|---:|---:|---:|
| Xeon8375C | 256 B | 12.87 | 12.87 | 10.65 | 10.65 | 11.76 |
| Xeon8375C | 512 B | 12.87 | 12.87 | 11.66 | 11.66 | 23.52 |
| M2Pro | 256 B | 45.82 | — | 37.92 | — | 9.25 |
| M2Pro | 512 B | 45.82 | — | 41.50 | — | 18.49 |

Doubling the block doubles the recurrence ceiling, while the PH instruction
budget per byte stays constant. The smaller issue and recurrence estimate
is the first-order ceiling; the measured hash also includes loads,
shuffles, XORs, reductions, and loop overhead. The 512-byte choice buys
recurrence headroom at the cost of 256 extra resident key bytes.

The final M2 probe measured 0.3500, 0.2328, and 0.2333 cycles/instruction with 8, 16, and 24 independent chains. The last two differ by 0.22%; the issue estimate uses the faster 16-chain measurement. Its timer multiplier is 3.276459123.

<!-- END GENERATED RESULTS -->

## Interpreting the implementations

On M2, the 256-byte variant's 9.20 B/cycle is close to its estimated
9.25 B/cycle recurrence ceiling, while its CLMUL issue estimate is 37.92.
The 512-byte default raises the recurrence ceiling to 18.49 and improves
measured bulk speed to 10.28 B/cycle. This supports the larger block as a way
to relieve the serial dependency; it does not imply that recurrence alone
explains the remaining gap. On Xeon the corresponding recurrence ceilings
are 11.76 and 23.52, versus PH-plus-recurrence issue estimates of 10.65 and
11.66. The measured 256-to-512 improvement is 6.20 to 8.16 B/cycle. These
estimates and the results favor 512 bytes on both hosts, with the 256-byte
option retaining its smaller key footprint.

On Xeon, two-lane VPCLMUL reaches 8.16 B/cycle versus 7.26 for scalar
Karatsuba. The measured VP instruction costs 1.657 timer cycles versus 0.829
for scalar CLMUL, while producing two lanes. Thus the wide path has about
the same CLMUL product capacity per byte. Its measured gain is consistent
with reducing other instruction overhead; these measurements alone do not
attribute that gain to particular loads, shuffles or loop instructions.

On the M2 Pro, the four-product schoolbook option reaches median 11.00 B/cycle
and 149.21 small-key cycles, versus 10.28 and 161.84 for the default Karatsuba
path: about 7.0% more bulk throughput and 7.8% fewer small-key cycles. This
shows why counting CLMULs alone does not rank complete implementations.
Define `CHAINHASH128_SCHOOLBOOK` before including the header to select that
option; the family, output bytes and theorem are unchanged. The delivered
default uses Karatsuba, with the measured alternatives available explicitly.

Two complete ChainHash-64 evaluations deliver 7.20 B/cycle on Xeon and
10.27 on Mac, compared with 14.29 and 21.98 for one supplied-header
evaluation. Small-key costs are 207.56 versus 110.00 cycles on Xeon but
85.15 versus 83.67 on Mac. Twice the algorithmic work therefore does not
imply twice the measured short-message latency; instruction overlap and
compiler scheduling can matter, and this experiment does not isolate them.

## Ceiling derivation

Let t128 be measured reciprocal throughput in timer cycles per independent
64×64→128 CLMUL instruction, t256 the corresponding VPCLMUL instruction cost
for two lanes, and lambda the measured serial recurrence latency (including
its state XOR). Ignore finalization in the bulk limit.

For block size B, scalar Karatsuba PH needs 3B/32 CLMULs, and one field
recurrence needs another 3+2=5. The scalar PH-only ceiling is 32/(3t128)
bytes/cycle; including recurrence instruction issue it is

\[
 C_{scalar}(B)=B/[(3B/32+5)t_{128}].
\]

The two-lane wide PH needs 3B/64 instructions, while the recurrence still uses
128-bit instructions. Assuming their shared CLMUL issue resource,

\[
 C_{wide}(B)=B/[(3B/64)t_{256}+5t_{128}],\quad
 C_{rec}(B)=B/\lambda.
\]

The smaller of the applicable issue and recurrence ceilings is the useful
first-order limit. Actual code also pays for loads, XORs, shuffles, reduction
shifts, loop control and tail handling. The schoolbook raw-product numerator
is four instead of three, and its recurrence costs six CLMULs. The finalizer
costs 4+5+5=14 CLMULs plus XORs/twist in Karatsuba mode, once per message;
this and safe partial-word loading explain why bulk throughput does not
predict small-key latency.

These are instruction-budget estimates from measured rates, not hard vendor
specification limits. Different turbo frequency or Mac calibration between
probe and hash processes can shift a numerical comparison. The probe includes
loop overhead; the raw output is retained for interpretation.

## Correctness evidence and limits

The C tests compare 12,000 random inputs of lengths 0..4096 per backend,
including every such length, plus 32 long inputs, zero/ones cases, model-A
schedules and protected-page tails. They separately compare raw 256-bit
products, field products, squaring and reduction against bit-serial arithmetic;
all monomial products are covered. Python independently generates 23
known-answer vectors for each block size, verifies the GCM polynomial's Rabin
certificate and tests the quintic circuit against polynomial coefficients
and their inverse map.

On Xeon, VPCLMUL, scalar PCLMUL, four-product schoolbook, 256-byte and portable
builds pass. Clang ASan/UBSan passes. The initial GCC sanitizer build could not
link its missing runtime libraries; the successful Clang run replaces it.
SMHasher3 Sanity passes, including exact verification values 0x742DE5A5 for
512-byte ChainHash-128 and 0xA16BCD95 for the 256-byte variant. Duplicate-value
warnings for alternate backends and the supplied 64-bit header are expected:
they are implementations of the same functions.

On the M2 Pro, PMULL, four-product schoolbook, 256-byte and portable builds
also pass the complete C suite. All four vector files match the independent
oracle, including exact agreement with Xeon for each block size. The combined
Mac binary passes ChainHash-128, the 256-byte variant and UMASH-128 Sanity.
The initial and expanded PMULL probe measurements are preserved separately;
the results table uses the expanded probe and its own calibration multiplier.

Backend agreement and guard-page/sanitizer checks do not prove compilation
correctness or the collision theorem. No exhaustive SMHasher3 distribution
suite is claimed. The raw logs and reproducible scripts distinguish the
mathematical proof, independent arithmetic evidence and measured speed.

## Score interpretation

The cross-family metric uses L in positive eight-byte words and the common
byte domain ell<2^64. For ideal ChainHash-128, the minimum occurs at L=1 and
is 128-log2(3)=126.4150374993. Model A's refined arbitrary-length bound gives
125; its fixed-length certificate gives 126.4150374993 when measured in
16-byte words. These are scores of proved upper bounds, not assertions that
the bounds are attained.

Two independent ideal ChainHash-64 evaluations need the same 656 ideal key
bytes as the default ChainHash-128, and have epsilon≤(n+2)²/2^128. Their
certificate has a full-domain score of approximately **77 bits** because of
quadratic length growth. This is why doubling the output and squaring epsilon
does not double min_L log2(L/epsilon(L)). A smaller maximum length changes that
score; it must be stated explicitly. `SPEC.md` derives the exact minimizing
block boundary and also handles the original reference's overflow-related
length restriction.

For example, capping messages at 1 MiB raises the two-independent-64
certificate's score to **120.99825 bits** (L=131041 eight-byte words is the
minimizing block start). ChainHash-128's ideal score remains 126.41504 bits
on that domain. The 77-bit figure applies to the full common length domain.
