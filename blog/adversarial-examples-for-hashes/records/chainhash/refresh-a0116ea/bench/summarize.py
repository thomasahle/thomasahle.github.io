"""Summarize a run_speed.py evidence directory with the reproduction's parse() and select() rules.

Usage: python3 bench/summarize.py EVIDENCE_DIR HOST [REPRO_SCRIPTS_DIR]
HOST is M2Pro (median of three), or Xeon8375C / EPYC9R14 (higher bulk, lower small of two).
Prints one JSON object: name -> selected metrics, runs, spreads, per-length small table.
"""
import json, pathlib, re, sys, hashlib
ev, host = pathlib.Path(sys.argv[1]), sys.argv[2]
sys.path.insert(0, sys.argv[3] if len(sys.argv) > 3 else str(pathlib.Path.home() / 'repos/hash-benchmark-reproduction/scripts'))
from benchmark import parse, select
out = {}
names = sorted({p.name.rsplit('.run', 1)[0] for p in ev.glob('*.run*.txt') if 'lenspeed' not in p.name})
for name in names:
    runs = []
    for raw in sorted(ev.glob(f'{name}.run*.txt')):
        if not re.fullmatch(re.escape(name) + r'\.run\d+\.txt', raw.name): continue
        text = raw.read_text(); meta = json.loads(raw.with_suffix('.json').read_text())
        rep = int(raw.name.rsplit('.run', 1)[1].split('.')[0])
        m = parse(text)
        small = re.search(r'Small key speed test - \[1, 31\]-byte keys\s+(.*?)\n\s*Average', text, re.S)[1]
        per_len = {int(a): float(b) for a, b in re.findall(r'^\s*(\d+)-byte keys\s*-\s*([\d.]+) cycles/hash', small, re.M)}
        runs.append(dict(run=rep, **m, raw_file=raw.name, raw_sha256=hashlib.sha256(raw.read_bytes()).hexdigest(),
                         command=meta['command'], started=meta['before']['utc'], load1_before=meta['before']['load'][0],
                         load1_after=meta['after']['load'][0], binary_sha256=meta['binary_sha256'], small_per_length=per_len))
    sel = select([{k: r[k] for k in ('run', 'bulk_bytes_per_cycle', 'bulk_gib_s', 'small_cycles')} for r in runs], host)
    sel['runs'] = runs
    out[name] = sel
json.dump(out, sys.stdout, indent=1)
