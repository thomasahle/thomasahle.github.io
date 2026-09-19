#!/usr/bin/env python3
import hashlib,json,pathlib,subprocess
root=pathlib.Path(__file__).resolve().parents[2]
speeds=json.loads((root/'speeds_halftime_fixed.json').read_text())
checks=json.loads((root/'certificates/verification-summary.json').read_text())
assert all(r['completed']==2**36 and r['collisions']==0 for r in checks['witness'])
repo=root/'HalftimeHash-fork'
if not repo.exists():repo=root
commit=subprocess.check_output(['git','-C',str(repo),'rev-parse','HEAD'],text=True).strip()
if subprocess.check_output(['git','-C',str(repo),'diff','HEAD','--','halftime-hash.hpp'],text=True):
    commit='Pending final commit; the header is frozen at the SHA-256 below.'
rows=[];ratios=[];variation=[]
names=['komihash','rapidhash','HalftimeHash-512','HalftimeHash-512-fixed','HalftimeHash24-fixed']
for host,label in [('Xeon8375C','Xeon 8375C'),('M2Pro','M2 Pro')]:
    h=speeds['hosts'][host]
    for name in names:
        e=h['hashes'][name]
        rows.append(f"| {label} | {name} | {e['bits']} | {e['backend_token'] or '(none printed)'} | {e['bulk_bytes_per_cycle']:.2f} | {e['small_cycles_per_hash']:.2f} |")
        bv=e['variability']['bulk_bytes_per_cycle']['range_spread_fraction']
        sv=e['variability']['small_cycles_per_hash']['range_spread_fraction']
        variation.append(f"| {label} | {name} | {100*bv:.2f}% | {100*sv:.2f}% |")
    for name in ['HalftimeHash24-fixed','HalftimeHash-512-fixed']:
        for control,r in h['hashes'][name]['control_ratios'].items():
            ratios.append(f"| {label} | {name} / {control} | {r['bulk_ratio']:.3f} | {r['small_ratio']:.3f} |")
flags=speeds['flags']
flagtext='\n'.join('- `'+json.dumps(f,sort_keys=True)+'`' for f in flags) or 'None.'
xeon=speeds['hosts']['Xeon8375C'];m2=speeds['hosts']['M2Pro']
overlap=xeon['other_smhasher_processes_seen_after_runs']
overlaptext=(f"Another SMHasher3 process was visible immediately after {len(overlap)} final Xeon cell(s); the exact processes and cells are retained in the JSON."
             if overlap else 'No other SMHasher3 process was visible at the final Xeon run boundaries.')
raw24=xeon['hashes']['HalftimeHash24-fixed'];m224=m2['hashes']['HalftimeHash24-fixed']
text=f'''# Fixed HalftimeHash24: construction, verification, and speed

The fixed header implements the distance-three construction and appends a
length word under fresh terminal Toeplitz keys, while returning exactly 24 bytes.
All four widths completed **68,719,476,736 (2^36) keys with zero collisions** on
the old witness. Style wrapper outputs remain unchanged.

`HalftimeHash24-fixed` measured **{raw24['bulk_bytes_per_cycle']:.2f} bytes/cycle and {raw24['small_cycles_per_hash']:.2f} cycles/hash**
on the Xeon, and **{m224['bulk_bytes_per_cycle']:.2f} bytes/cycle and {m224['small_cycles_per_hash']:.2f} cycles/hash** on the M2.
Each figure is the median of three complete SMHasher3 runs. Small-key numbers
are the average over lengths 1–31 bytes.

* Branch: `fixed-24byte-core`, based on `fix-neon-dispatch` (`8b03edf`).
* Commit: {commit}
* Commit is local and has not been pushed.
* Header SHA-256: `{checks['final_header_sha256']}`.
* Deliverables: [header](HalftimeHash24-fixed.hpp), [header patch](FIX.diff),
  [exact theorem](THEOREM.md), [certificates](certificates/README.md),
  [all timing data](speeds_halftime_fixed.json), and [tracker note](ISSUE_NOTE.md).

## Changes and compatibility

1. Encode3/4/5 reference the changing input row pointer, advance entire rows,
   and stop the first parity pass at the last data row. Encoder interfaces now
   take actual three-block rows. Exhaustive ranks establish distances 3/4/5;
   Encode2 remains distance 2.
2. `GetEntropyBytesNeeded<W,k>(n)` takes a byte length and reports the exact
   required key-prefix extent for every supported width and output size. For
   the 24-byte core, widths 1/2/4/8 need 432/647/1,077/1,937 words. Holes in the
   prefix are permitted; the highest accessed word is included. The public
   Style allocation constant is 59,736 bytes.
3. Only the three-word core receives a new length rule. Its raw tail is right
   aligned in a separate Toeplitz pool and ends with one scalar word containing
   the total byte length. Its position is fixed across all message lengths and
   all forest shapes. Output shifts remain whole NH pairs. This gives the
   unequal-length bound `2^-96` without increasing the output size.
4. Tabulation indexes the real flat key array, covering all 24 byte tables in
   a Style wrapper. EHC key loads also use the real flat array. Horizontal sums
   use unsigned scalar addition, including an explicit AVX-512 reduction to
   avoid the signed scalar addition inside GCC 11's convenience intrinsic.

The parent NEON dispatch repair is retained. No unrelated hashing rule, matrix,
tree schedule, tabulation address, or Style length rule is changed. Given the
same key prefix, the Style wrappers produce the same modular results as the
parent: 40,000 direct comparisons agree. The table repair changes no output.
The unsigned sums preserve intended results and remove the need for `-fwrapv`.
The fixed raw three-word outputs intentionally change; Encode4/5 repairs also
change their affected raw outputs.

## What the proof certifies

For independent uniform key words and fixed messages in the supported domain,
the 24-byte core satisfies the all-target difference bound
`(h+2)^2(h+5)·2^-96` for equal-layout forests. Differing equal-length tails,
or **any unequal lengths**, instead have the stronger bound `2^-96`.
The exact hypotheses, key addresses, conditioning argument, and Lean mapping
are in `THEOREM.md`. The new terminal-length lemma passes the Lean kernel and
uses only `propext`, `Classical.choice`, and `Quot.sound`.

For the write-up, keep collision-bound bits and the requested normalized
score distinct:

| Function | Output | Collision statement | `min_L log2(L/ε(L))`, L in eight-byte words |
|---|---:|---|---:|
| HalftimeHash24, shipped, refuted | 192 bits | Empty and one zero byte collide for every key; the old equal-length witness has probability at least `2^-32` | 0 with unequal lengths admitted |
| HalftimeHash24, fixed | 192 bits | `6804·2^-96` at abstract `h=16`: **83.2678 bits** | **96 bits**, sharp for ideal keys |
| Fixed Style wrappers | 64 bits | Existing sharp wrapper bound, now with valid table objects and unsigned sums | **`64-log2(2-2^-64)`**, conservatively **63 bits** |

If the shipped comparison is restricted to equal lengths, its b=8 witness at
`L=168` already caps the normalized score at `32+log2(168) = 39.3923` bits.

The fixed header retains the nine-entry stack and needs a zero sentinel. Its
supported lengths are `<168b·19,173,961` bytes for the 24-byte core and
`<144b·19,173,961` for Style wrappers. The actual maximum height is 7, giving
the stronger uniform core bound `972·2^-96` (**86.0752 bits**). At b=8 the
exclusive raw-core limit is 25,769,803,584 bytes. No runtime length check or
stack expansion was added. Thus “fixed, proved 83 bits” is a conservative
collision-bound label on this stated domain; it is not the normalized score
and does not claim that this stack can execute a height-16 tree.

The theorem concerns ideal independent key words. The benchmark's 64-bit
seed expansion and the experiment's AES-CTR generator do not themselves
instantiate that mathematical probability space. The source-to-model mapping
and GF(2) certificate accompany the Lean results; a complete C++ compiler
refinement is not claimed.

## Verification results

| Check | Result |
|---|---|
| All generator survivor subsets | Encode2: 128; Encode3: 512; Encode4: 1,024; Encode5: 512. Every critical survivor set has full rank. Exact distances 2/3/4/5. |
| Encode3 direct execution | 20,160 single-bit flips across all block widths; every flip changes exactly three packets. |
| Exact key extents | 592 guard-page cases covering all widths and k=2/3/4/5; exact allocations pass and one word less faults in every case. |
| Old witness, width 1 | 0 / 68,719,476,736 collisions |
| Old witness, width 2 | 0 / 68,719,476,736 collisions |
| Old witness, width 4 | 0 / 68,719,476,736 collisions |
| Old witness, width 8 | 0 / 68,719,476,736 collisions |
| `high32(key[6])=0` conditioning | 0 / 268,435,456 at **each** width; the shipped witness collided on every conditioned key. |
| Scalar/SSE2/AVX2/AVX-512 and scalar/NEON | 10,000 shared random inputs per binary, unaligned inputs and boundary lengths; 160,000 core and 40,000 Style calls each. All result files identical. |
| UBSan | Clean on Xeon Clang 21, GCC 11 instrumentation, and M2 Apple Clang 17; no suppressions or wrap/aliasing overrides for fixed code. |
| Additional length/read-set checks | 40,000 unequal-length zero-message pairs: zero collisions; 40,000 hole-rerandomization checks; 40,000 independent short-tail scalar formula checks. |
| Parent Style comparison | 40,000 identical 64-bit outputs. |
| SMHasher24 Sanity | Six runtime sanity checks pass. New verification fingerprint `3F1372EA`; unregistered digest check is explicitly reported as skipped. |

The shared 4,800,000-byte vector file SHA-256 is
`{checks['vector_sha256']}`.
The witness harness invokes the actual header through a separate noinline
translation unit, without LTO, and compares all three result words. It draws
every live key word from AES-128-CTR, checks the FIPS-197 known-answer vector,
and records a `getrandom` seed and disjoint per-thread counters. Unread holes
are integrated out and independently tested. These are regression experiments;
zero empirical collisions does not establish the exponent in the theorem.

Heavy work ran on `<xeon-host>` under
`<xeon-work>/halftime-fixed`, at nice 10 with at most 24 compute workers.
Only reported completed work is counted. Widths 1/2/4 combine a conservative
printed checkpoint with a separately seeded completed continuation. The last
AVX-512 reduction edit left those three function bodies, relocations, and
constants identical, as certified in `final-object-equivalence.json`; width 8
was rerun in full with the final binary. Earlier width-8 and timing runs are
preserved but excluded. The final Xeon vector, guard, rank, and UBSan checks
were rebuilt against the final header. The M2 preprocessing certificate proves
the AVX-512-only edit is absent from its active code, preserving its completed
checks and timings.

## SMHasher3 measurements

The scratch trees are `<xeon-work>/speedbench-hhfixed/source` on the Xeon and
`smhasher3-m2/` in this workspace on the M2. The original trees were not edited.
The adapter registers a genuine 192-bit result and a 64-bit Style result. The
192-bit dispatcher supports Speed and Sanity using the unchanged untemplated
Speed engine; it does not round the output to 256 bits. Control implementations,
timer code, sample counts, and buffers are unchanged.

Commands are exactly `SMHasher3 NAME --test=Speed`, at default timing priority
and affinity. Each host ran three complete passes through all five names. The
headline bulk number is the fixed **262,144-byte** Average; the varying-length
bulk section is retained in the JSON. Bulk and small-key medians are selected
independently. All 31 individual small lengths are retained per run.

| Host | Registration | Output bits | Backend token | Bulk bytes/cycle ↑ | 1–31 B cycles/hash ↓ |
|---|---|---:|---|---:|---:|
{chr(10).join(rows)}

The shipped `HalftimeHash-512` control is the existing SMHasher3 **64-bit Style
port**, not the shipped raw 24-byte function. The fixed registrations call the
literal upstream-derived header. Consequently a Style speed difference can
include port/adapter/compiler effects even though its functional outputs agree.
No number in the control row should be relabeled as shipped HalftimeHash24.

| Host | Fixed / control | Bulk ratio (above 1 faster) | Small ratio (above 1 slower) |
|---|---|---:|---:|
{chr(10).join(ratios)}

### Variability and controls

The table below uses `max/min - 1` across the three runs. The JSON separately
flags any individual deviation from its median greater than 15%.

| Host | Registration | Bulk spread | Small spread |
|---|---|---:|---:|
{chr(10).join(variation)}

Recorded flags, including historical control comparisons:

{flagtext}

The historical M2 shipped-Style result in `speedbench_REPORT.md` used the
**portable** backend (3.66 bytes/cycle, 78.79 cycles/hash). The current control
uses **NEON** ({m2['hashes']['HalftimeHash-512']['bulk_bytes_per_cycle']:.2f}, {m2['hashes']['HalftimeHash-512']['small_cycles_per_hash']:.2f}), explaining its flagged historical change.
The corrected NEON reference from the M2 rerun is 7.49 bytes/cycle and 63.38
cycles/hash. Historical references used best-of-two selection; they are drift
checks, while this report uses median-of-three.

Every M2 timing starts with no SMHasher3 process and one-minute load below 4.5.
Its observed start-load range was {m2['load1_before_range'][0]:.4f}–{m2['load1_before_range'][1]:.4f}; after-run
load ranged {m2['load1_after_range'][0]:.4f}–{m2['load1_after_range'][1]:.4f}. The gate is a start condition, not a sustained
load guarantee. The Mac did no heavy key experiments or Lean compilation;
short verification/build preparation was gated, and unchanged object archives
were reused. The M2 remained otherwise idle for this task during timing.

The final Xeon batch ran after this task's witness workers exited. Its sampled
start-load range was {xeon['load1_before_range'][0]:.2f}–{xeon['load1_before_range'][1]:.2f}.
{overlaptext}
The final binary hashes are:

* Xeon: `{xeon['binary_sha256']}`.
* M2: `{m2['binary_sha256']}`.

Xeon uses RDTSC/RDTSCP; M2 uses SMHasher3's calibrated monotonic-clock cycle
estimate. These cycle units are not interchangeable physical core cycles.
The program's printed GiB/s assumes 3.5 GHz on both hosts; it is not measured
wall-clock throughput. Exact compilers, flags, source/archive hashes, timer
headers, raw outputs, and gates accompany the numbers in `certificates/`.
'''
(root/'REPORT.md').write_text(text)
print('Wrote REPORT.md from validated results and timing medians.')
