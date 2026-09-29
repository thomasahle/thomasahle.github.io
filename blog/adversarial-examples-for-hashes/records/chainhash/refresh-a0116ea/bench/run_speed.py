"""Run unmodified SMHasher3 --test=Speed, one process at a time, one hash name per invocation,
(this refresh drops the supplementary per-length driver; pass '-' for LENSPEED).

run_speed.py of the PolyXOR128 lane, unchanged in method:
Xeon: two complete passes, `nice -n 10 taskset -c CPUS`; each start waits while any
other SMHasher3 (or lenspeed) process runs or the one-minute load is >= --xeon-max-load.
M2: three complete passes, `nice -n 10`; each start waits (60 s polls) until no other
SMHasher3 process runs and the one-minute load is below 4.5.
During each run a whole-machine scan every 15 s looks for any other SMHasher3 process;
a hit kills and discards the run (logged in discarded.jsonl), which is then repeated.
Existing completed runs of the same binary are kept (resumable).
"""
import argparse, pathlib, subprocess, os, time, datetime, hashlib, json, platform
p = argparse.ArgumentParser(); p.add_argument('binary', type=pathlib.Path); p.add_argument('lenspeed', type=pathlib.Path); p.add_argument('out', type=pathlib.Path)
p.add_argument('--passes', type=int); p.add_argument('--cpus', default='16-23'); p.add_argument('--xeon-max-load', type=float, default=80.0)
p.add_argument('--names', nargs='+', default=['chainhash-128.a0116ea', 'chainhash-128.0a03c63', 'chainhash.a0116ea', 'chainhash.0a03c63', 'polyxor-128', 'XXH3-128'])
a = p.parse_args()
mac = platform.system() == 'Darwin'; a.out.mkdir(parents=True, exist_ok=True)
binary = a.binary.resolve(); lens = None if str(a.lenspeed) == '-' else a.lenspeed.resolve()
sha = lambda f: hashlib.sha256(pathlib.Path(f).read_bytes()).hexdigest(); bsha = sha(binary); lsha = sha(lens) if lens else None
def stamp(): return datetime.datetime.now(datetime.timezone.utc).isoformat()
def cpu_idle(cpus):
    if mac: return None
    lo, hi = map(int, cpus.split('-'))
    def rd():
        d = {}
        for l in open('/proc/stat'):
            f = l.split()
            if f[0].startswith('cpu') and f[0] != 'cpu': v = list(map(int, f[1:])); d[int(f[0][3:])] = (sum(v), v[3] + v[4])
        return d
    x = rd(); time.sleep(2); y = rd()
    return {str(c): round(100 * (1 - (y[c][1] - x[c][1]) / max(1, y[c][0] - x[c][0])), 1) for c in range(lo, hi + 1)}
def pids():
    return [q for n in ('SMHasher3', 'lenspeed') for q in subprocess.run(['pgrep', '-x', n], capture_output=True, text=True).stdout.split()]
def state():
    return {'utc': stamp(), 'uptime': subprocess.run(['uptime'], capture_output=True, text=True).stdout.strip(), 'load': os.getloadavg(), 'smhasher_pids': pids()}
def gate():
    while True:
        s = state(); s['cpu_busy_percent'] = cpu_idle(a.cpus)
        s['passed'] = not s['smhasher_pids'] and s['load'][0] < (4.5 if mac else a.xeon_max_load)
        with (a.out / 'gate.jsonl').open('a') as f: f.write(json.dumps(s) + '\n')
        if s['passed']: return s
        print('WAIT', s['utc'], s['load'], s['smhasher_pids'], flush=True); time.sleep(60)
prefix = ['nice', '-n', '10'] + ([] if mac else ['taskset', '-c', a.cpus])
def run(raw, argv, shown, row0, want_sha):
    meta = raw.with_suffix('.json')
    if meta.exists(): assert json.loads(meta.read_text())['binary_sha256'] == want_sha; return
    while True:
        before = gate(); print(stamp(), 'START', raw.name, flush=True); start = time.monotonic(); polls = 0; hit = None
        with raw.open('w') as f:
            r = subprocess.Popen(prefix + argv, stdout=f, stderr=subprocess.STDOUT)
            while r.poll() is None:
                time.sleep(1)
                if int(time.monotonic() - start) // 15 > polls:
                    polls += 1; others = [q for q in pids() if q != str(r.pid)]
                    if others: hit = {'utc': stamp(), 'other_pids': others}; r.kill(); r.wait(); break
        if hit is None: break
        with (a.out / 'discarded.jsonl').open('a') as f: f.write(json.dumps({'raw_file': raw.name, 'started': before['utc'], **hit}) + '\n')
        print('DISCARD', raw.name, hit, flush=True); raw.unlink()
    after = state(); after['cpu_busy_percent'] = cpu_idle(a.cpus)
    row = {**row0, 'command': prefix + shown, 'before': before, 'after': after, 'elapsed_seconds': time.monotonic() - start,
           'returncode': r.returncode, 'raw_file': raw.name, 'raw_sha256': sha(raw), 'binary_sha256': want_sha,
           'during_run_scans': polls, 'during_run_rule': 'whole-machine scan every 15 s for any other SMHasher3 process; a hit kills and discards the run, which is then repeated'}
    meta.write_text(json.dumps(row, indent=2) + '\n'); print('DONE', raw.name, round(row['elapsed_seconds'], 1), r.returncode, flush=True)
    if r.returncode: raise SystemExit(r.returncode)
for rep in range(1, (a.passes or (3 if mac else 2)) + 1):
    for name in a.names:
        run(a.out / f'{name}.run{rep}.txt', [str(binary), name, '--test=Speed'], ['SMHasher3', name, '--test=Speed'], {'name': name, 'run': rep}, bsha)
    if lens: run(a.out / f'lenspeed.run{rep}.txt', [str(lens), *a.names], ['lenspeed', *a.names], {'name': 'lenspeed', 'run': rep, 'names': a.names}, lsha)
print('ALLDONE', flush=True)
