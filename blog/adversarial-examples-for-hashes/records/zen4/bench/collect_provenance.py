"""Collect the EPYC 9R14 host and build provenance for records/zen4 (run on the host, after the runs)."""
import json, os, subprocess, pathlib, hashlib, re, platform
H = pathlib.Path(os.environ.get('WORK', '.')).resolve()  # the host's work directory
R = H / 'hash-benchmark-reproduction'
sh = lambda c: subprocess.run(c, shell=True, capture_output=True, text=True).stdout.strip()
sha = lambda f: hashlib.sha256(pathlib.Path(f).read_bytes()).hexdigest()
build = json.loads((R / '.work/build.json').read_text())
panel_build = json.loads((H / 'work-30c0111/build.json').read_text())
cpuinfo = pathlib.Path('/proc/cpuinfo').read_text()
flags = re.search(r'^flags\s*:\s*(.*)$', cpuinfo, re.M)[1].split()
def probe(f):
    t = (H / f).read_text()
    return dict(tsc_ghz=float(re.search(r'tsc_ghz ([\d.]+)', t)[1]),
                core_ghz_add_chain=[float(x) for x in re.findall(r'core_ghz ([\d.]+)', t)],
                perf_cycles_per_add=[float(x) for x in re.findall(r'perf_cycles/iter ([\d.]+)', t)])
ex = json.loads((R / 'out/EPYC9R14/execution.json').read_text())
ex2 = json.loads((R / 'out-a0116ea/EPYC9R14/execution.json').read_text())
loads = [r['load_before']['load'][0] for r in ex['runs'] + ex2['runs']]
prov = {
 'host': 'EPYC9R14',
 'cpu_model': re.search(r'^model name\s*:\s*(.*)$', cpuinfo, re.M)[1],
 'microarchitecture': 'Zen 4 (Genoa), family 25 model 17 stepping 1',
 'virtualization': 'KVM guest (hypervisor flag); 16 vCPUs, 1 thread per core',
 'cores': 16, 'l2_per_core': '1 MiB', 'l3': '2 x 32 MiB (CPUs 0-7 and 8-15 each share one L3)',
 'isa_flags_relevant': [f for f in ['pclmulqdq', 'vpclmulqdq', 'aes', 'vaes', 'avx2', 'avx512f', 'avx512vl', 'avx512bw', 'avx512dq', 'avx512ifma', 'avx512vbmi', 'gfni', 'sha_ni', 'tsc_known_freq', 'constant_tsc', 'nonstop_tsc'] if f in flags],
 'kernel': platform.release(), 'os': sh('. /etc/os-release; echo $PRETTY_NAME'),
 'clocksource': sh('cat /sys/devices/system/clocksource/clocksource0/current_clocksource'),
 'tsc': {'kernel_detected': sh("dmesg 2>/dev/null | grep -m1 'tsc: Detected'") or '2600.000 MHz (tsc: Detected, boot log)',
         'before_runs': probe('clock-before.txt'), 'after_runs': probe('clock-after.txt') if (H / 'clock-after.txt').exists() else None,
         'method': 'rdtsc over 1 s of CLOCK_MONOTONIC_RAW; core clock from a 2e9-iteration dependent add chain (1 cycle per add, confirmed by a user-mode perf_event_open cycles counter), taskset -c 12'},
 'clock_note': 'SMHasher3 B/cycle on this host is bytes per TSC tick at 2.600 GHz (base clock). A busy core boosts to about 3.66 GHz, so B per core cycle = B/tick x 2.600/3.66 = 0.71 x B/tick, and wall-clock GB/s = 2.6 x B/tick. No cpufreq interface is exposed in the guest; boost is inferred from the measured core clock.',
 'compiler': build['compiler'], 'cmake': build['cmake'], 'rustc': build['rustc'],
 'openssl': sh('openssl version'), 'openssl_headers': 'openssl-devel-3.5.5-6.el9_8 (same version as the system libraries), unpacked in user space',
 'compiler_flags': build.get('compiler_flags'),
 'march_native_note': '-march=native as on the other hosts; Red Hat GCC 11.5 recognizes Zen 4 and resolves it to: ' + sh("gcc -march=native -Q --help=target 2>/dev/null | grep -E '^ +-march=|^ +-mtune=' | tr -s ' '"),
 'smhasher3_base': build['commit'], 'polyxor_backend_probe': build['polyxor_backend_probe'],
 'binaries': {
   'panel': {'rows': '40 manifest rows other than chainhash and chainhash-128, plus the chart rows outside the manifest and polyxor-128.raw (speeds_zen4_extra.json)',
             'chainhash_pin': '30c0111 (patch 0006 at the time)', 'patches': panel_build['patches'], 'binary_sha256': panel_build['binary_sha256'],
             'source_manifest_sha256': panel_build['source_manifest_sha256']},
   'chainhash_rows': {'rows': 'chainhash, chainhash-128', 'chainhash_pin': 'a0116ea (the reproduction repository pin)', 'patches': build['patches'],
             'binary_sha256': build['binary_sha256'], 'binary_sha256_recomputed': sha(R / '.work/build/SMHasher3'),
             'source_manifest_sha256': build['source_manifest_sha256']},
   'backends': json.loads((H / 'extras/build-epyc-a0116ea/provenance.json').read_text())},
 'pin_set': 'taskset -c 8-15 for every build and timing; nice -n 10 for every timed run',
 'gate': "reproduction x86 gate: no other SMHasher3 process whose affinity intersects CPUs 8-15; the supplementary runs (run_speed.py) wait for no SMHasher3 process anywhere and discard/repeat a run that overlaps one. No load threshold: another job ran on CPUs 0-7 (the other L3), holding load1 near 5-6.",
 'load1_at_launch': {'min': min(loads), 'max': max(loads), 'n': len(loads)},
 'panel_started': ex['started'], 'panel_finished': ex.get('finished'), 'chainhash_rows_started': ex2['started'], 'chainhash_rows_finished': ex2.get('finished'),
 'excluded_overlap_runs': len(ex['excluded_runs']) + len(ex2['excluded_runs']),
}
(H / 'provenance.json').write_text(json.dumps(prov, indent=2) + '\n')
print(json.dumps({k: prov[k] for k in ('cpu_model', 'kernel', 'os', 'compiler', 'rustc', 'load1_at_launch')}, indent=1))
