#!/usr/bin/env python3
import datetime,json,pathlib
root=pathlib.Path(__file__).resolve().parent.parent
d=json.loads((root/'speeds_museair_v2.json').read_text())
hosts=['Xeon8375C','M2Pro'];names=list(d)
lines=['# MuseAir v2 timing', '', 'Generated '+datetime.datetime.now(datetime.timezone.utc).isoformat()+'.','',
'MuseAir algorithm v2 (crate `museair` 0.6.0) is registered as **MuseAir-v2**, with 64-bit output. All requested timings are complete: two Xeon passes and three gated M2 passes, including XXH3-64, rapidhash, chainhash-256 and the unchanged **MuseAir** v0.3 registration in each host’s timing binary. All five registrations pass full Sanity on both hosts.','',
'| Host | Registration | Backend | Bulk B/cycle | Small cycles/hash | Runs |',
'|---|---|---|---:|---:|---:|']
for host in hosts:
 for name in names:
  e=d[name][host];assert e['status']=='complete'
  lines.append(f"| {host} | {name} | {e['backend_token'] or 'generic'} | {e['bulk_bytes_per_cycle']:.2f} | {e['small_cycles']:.2f} | {len(e['runs'])} |")
lines+=['','| Host | v2/v0.3 bulk | v2/v0.3 small latency | >15% version difference |', '|---|---:|---:|---|']
for host in hosts:
 v2=d['MuseAir-v2'][host];v0=d['MuseAir'][host]
 bulk=v2['bulk_bytes_per_cycle']/v0['bulk_bytes_per_cycle'];small=v2['small_cycles']/v0['small_cycles']
 lines.append(f"| {host} | {bulk:.4f} ({100*(bulk-1):+.2f}%) | {small:.4f} ({100*(small-1):+.2f}%) | {'YES' if max(abs(bulk-1),abs(small-1))>.15 else 'no'} |")
lines+=['','Higher bulk is better; lower small-key latency is better. Version differences compare different algorithms and are separate from repeatability/control flags.','', '## Protocol','',
'Commands are exactly `SMHasher3 NAME --test=Speed`, with no affinity or priority adjustment. Each pass runs MuseAir-v2, XXH3-64, rapidhash, chainhash-256, then MuseAir. Bulk is the **fixed 262144-byte Average** over alignments; small is the **Average over lengths 1–31**. The varying-length bulk results are preserved in the JSON but never selected for the headline.','',
'Xeon independently selects higher bulk and lower small from two passes. Rounded bulk ties select higher GiB/s, then the earlier run. M2 independently selects the median of three for each metric, retaining GiB/s from the selected bulk run.','',
'Every M2 preparation/timing start requires no other SMHasher3 process and one-minute load **<4.5**, with 60-second polling and no timeout bypass. Five-second process/load monitoring archives and retries any overlapping timing run. The load gate is a start condition, not a sustained-load condition. Xeon also waits for no other SMHasher3 process. This task ran no search or substantial collision scan on the Mac: local work was file editing, brief gated compilation/verification and gated timing; the 2^30-seed scan ran on Xeon before timing.','',
'The unchanged harness uses TSC ticks on Xeon and a calibrated monotonic-clock cycle estimate on M2; these are not interchangeable physical core-cycle counts. GiB/s is SMHasher3’s conversion assuming 3.5 GHz on both hosts, not independently measured throughput.','',
'## Registration and correctness','',
'`bench/museair_v2.cpp` calls the supplied validated `museair2_hash(bytes, length, uint64_t(seed), 0)`, equivalent to the crate’s standard **`museair::hash(bytes, seed)`**. SMHasher3’s 64-bit seed maps identically to the crate’s single 64-bit seed: no expansion, random secret, truncation, zero-seed exclusion or seed hook. The final argument fixes **BFast=false**. The crate’s ordinary seed mixing remains inside the timed call. This measures the supplied C/C++ port, not Rust code generation. Both callback slots expose the same little-endian byte function; no big-endian-host claim is made.','',
'The supplied audit identifies tag `crate-0.6.0` = `f3092ae` and upstream `src/lib.rs` SHA-256 `123772c6360a31ef29f5d525d87d8f7738c8e8d883b284a6b20dd29fb1d42b1c`. The port and decompressed vector hashes are recorded in each host’s `build-sha256.json`.','',
'Both hosts reproduce all **200,000 crate-generated vectors**, each at offsets 0, 1 and 7 (**600,000 registered calls**, zero mismatches, lengths 0–511). Verification uses `findHash("MuseAir-v2")`, `HashInfo::Seed` and the actual registered callback; it is relinked against the final registration object used by the timing binary. Native verification is **0x7140cabc** on both hosts. Full Sanity passes: implementation verification, both sanity checks, append/prepend-zeroes and both thread-safety checks.','',
'The audited 32-byte pair is:','', '```',
'M  = '+'00'*32,
'M2 = 00000000000000404a048402a910048a00000000000000a80000000000000000','```','']
p=d['MuseAir-v2']['Xeon8375C']['registered_pair_measurement'];lo,hi=p['poisson95_log2']
lines += [f"A fresh Xeon sample through the registration gives **{p['collisions']:,} / {p['trials']:,} = 2^{p['log2_rate']:.3f}**, with Poisson 95% interval **[2^{lo:.3f}, 2^{hi:.3f}]**. This reproduces the audited 2^-17.45 rate. Seeds are successive SplitMix64 outputs from initial state `0x763220260919`; complete 64-bit outputs are compared. The approximate Poisson interval uses Wilson–Hilferty. This is a measured rate, not an exact probability or key-free collision.", '',
'Both hosts reproduce the known collision seeds `040963434fe5e368` → `ef46cda19c0bbc67` and `acac861647d5f7f2` → `b1f37635e72a255f`, and confirm seed zero does not collide. The M2 rate claim transfers through the shared implementation and crate-vector equality; a separate large M2 seed scan was deliberately omitted to keep the Mac idle. The prior independent Rust audit and its logs remain in `muse-v2/independent-verify/`.','',
'## Variation and controls','',
'Flags use **>15%**, strictly: Xeon two-run max/min spread; M2 each run’s absolute deviation from its three-run median. Controls, including old MuseAir, are also compared against the prior host baseline (`../speedbench/speeds.json` for Xeon, `../m2-rerun/speeds_m2_v2_final.json` for M2). Baseline file hashes and values are recorded in the JSON.','',
'| Host | Registration | Bulk run spread | Small run spread | Run flag |',
'|---|---|---:|---:|---|']
flags=[]
for host in hosts:
 for name in names:
  e=d[name][host]
  lines.append(f"| {host} | {name} | {e['bulk_run_spread_percent']:.2f}% | {e['small_run_spread_percent']:.2f}% | {'YES' if e['flag_over_15_percent'] else 'no'} |")
  if e['flag_over_15_percent']:flags.append(host+' '+name+' run variation')
lines+=['','| Host | Control | Bulk new/old | Small new/old | >15% baseline flag |','|---|---|---:|---:|---|']
for host in hosts:
 for name in names[1:]:
  e=d[name][host]['control_baseline'];r=e['ratios']
  lines.append(f"| {host} | {name} | {r['bulk_bytes_per_cycle']:.4f} | {r['small_cycles']:.4f} | {'YES' if e['flag_over_15_percent'] else 'no'} |")
  if e['flag_over_15_percent']:flags.append(host+' '+name+' control baseline')
lines+=['',('**Flags:** '+ '; '.join(flags)+'.') if flags else '**No >15% run-deviation or control-baseline flags.**','']
for host in hosts:
 x=json.loads((root/'evidence'/host/'execution.json').read_text())
 boundaries=[r[t]['load'][0] for r in x['runs'] for t in ['before','after']]
 samples=[s['load'][0] for r in x['runs'] for s in [r['before'],r['after'],*r['samples']]]
 starts=[r['before']['load'][0] for r in x['runs']]
 lines.append(f"{host}: {len(x['runs'])} accepted runs; start load1 {min(starts):.2f}–{max(starts):.2f}; boundary load1 {min(boundaries):.2f}–{max(boundaries):.2f}; all sampled load1 {min(samples):.2f}–{max(samples):.2f}; {len(x['excluded_runs'])} overlapping attempts excluded.")
 lines.append('')
lines+=['## Build and evidence','',
'The scratch trees are `./smhasher3-m2` and `/home/thomas-ahle/agents/speedbench-museair-v2/source`, copied from the earlier M2/Xeon source trees. The builds reuse `../m2-rerun/build-fixed` and `/home/thomas-ahle/agents/speedbench/build-release-20260917` main, test and hash-library objects. Only the new registration translation unit is added, compiled with C++17, `-O3 -march=native -DNDEBUG -DHAVE_THREADS` (plus the prior M2 AES target option). The source is also appended to each scratch `hashes/Hashsrc.cmake`. Existing harness, timer and control objects are unchanged.','',
'| Host | Binary | SHA-256 |','|---|---|---|']
for host in hosts:
 e=d['MuseAir-v2'][host];lines.append(f"| {host} | `{e['binary']}` | `{e['binary_sha256']}` |")
lines+=['',
'`speeds_museair_v2.json` preserves the earlier nested `hash → host → metrics/runs` schema. `evidence/<host>/` contains every raw Speed/Sanity log, complete execution and gate records, verification output, registration constants, compile/link commands, source/object/binary and reused-input hashes, compiler/host details, and disassembly. The supplied port and vectors remain unchanged in `muse-v2/`.','',
'Reproduce with `python3 bench/prepare_host.py` on each host after transferring this directory; this includes preparation, verification, Sanity and the required timing passes. Copy the Xeon evidence directory back, then run `python3 bench/collect.py`, `python3 bench/report.py` and `python3 bench/validate_results.py`. Existing timing execution files allow resuming only with the same binary hash; use fresh evidence directories for a new measurement.','']
(root/'REPORT.md').write_text('\n'.join(lines))
