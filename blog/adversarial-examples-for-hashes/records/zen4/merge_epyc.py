#!/usr/bin/env python3
"""Add the AMD EPYC 9R14 (Zen 4) host cells to the post's data.json and records/speeds.json.

    python3 records/zen4/merge_epyc.py            # from the post directory

Reads records/zen4/speeds_zen4.json (the reproduction's 42-row panel, benchmark.py output)
and records/zen4/speeds_zen4_extra.json (chart rows outside the reproduction manifest, same
binary and protocol). Writes smh_epyc_bulk_Bpc / smh_epyc_small_cycles on every chart row
(value null where a row has no EPYC timing), the EPYC9R14 entry of the HalftimeHash style
speeds, EPYC9R14 in each row's chart_hosts list, and an EPYC9R14 cell for every hash in
records/speeds.json that was measured. Existing M2Pro/Xeon8375C cells are not touched.
"""
import json
import pathlib

POST = pathlib.Path(__file__).resolve().parents[2]
HOST, SHORT = 'EPYC9R14', 'epyc'
PANEL = 'records/zen4/speeds_zen4.json'
EXTRA = 'records/zen4/speeds_zen4_extra.json'
# Chart rows outside the reproduction manifest -> registration timed in the extra runs.
EXTRA_ROWS = {'wyhash': 'wyhash', 'xxh64': 'XXH-64', 'xxh32': 'XXH-32', 'murmur64a': 'MurmurHash2-64',
              'murmur2': 'MurmurHash2-32', 'murmur2a': 'MurmurHash2a'}
MANIFEST = json.loads((POST / 'records/zen4/manifest.json').read_text())


def load(path):
    return json.loads((POST / path).read_text())


def dump(path, obj, like):
    raw = (POST / like).read_text()
    indent = 1 if raw.startswith('{\n "') else 2
    (POST / path).write_text(json.dumps(obj, indent=indent, ensure_ascii=False) + ('\n' if raw.endswith('\n') else ''))


def cells(source, name, path):
    out = {}
    for suffix, metric, unit, sel, spread in [
            ('bulk_Bpc', 'bulk_bytes_per_cycle', '262144 B bulk, B/cycle (B per TSC tick, TSC 2.600 GHz)', 'bulk_selected_run', 'bulk_run_spread_percent'),
            ('small_cycles', 'small_cycles', '1–31 B, cycles/hash (TSC ticks)', 'small_selected_run', 'small_run_spread_percent')]:
        out['smh_' + SHORT + '_' + suffix] = dict(
            value=source[metric], source=f'SMHasher3 {HOST}, {unit}', source_short='SMHasher3 ' + HOST,
            url=path, record=f'{path}#/{name}/{HOST}', registered_name=source.get('registered_name', name), host=HOST,
            selection='higher bulk of two passes; independently lower small average', selected_run=source[sel],
            spread_percent=source[spread], binary_sha256=source['binary_sha256'],
            verification=source.get('verification', {'status': 'VerifyAll PASS; see records/zen4/verifyall.txt'}))
    return out


def main():
    data, panel, extra = load('data.json'), load(PANEL), load(EXTRA)
    by_row = {item['row_id']: item['name'] for item in MANIFEST}
    count = 0
    for row in data['heuristics'] + data['proven']:
        speeds = row.setdefault('speeds', {})
        name = by_row.get(row['id'])
        if name and panel.get(name, {}).get(HOST, {}).get('status') == 'complete':
            speeds.update(cells(panel[name][HOST], name, PANEL)); count += 1
        elif row['id'] in EXTRA_ROWS and HOST in extra.get(EXTRA_ROWS[row['id']], {}):
            speeds.update(cells(extra[EXTRA_ROWS[row['id']]][HOST], EXTRA_ROWS[row['id']], EXTRA)); count += 1
        else:
            for suffix in ('bulk_Bpc', 'small_cycles'):
                speeds['smh_' + SHORT + '_' + suffix] = dict(value=None, source='Not timed on ' + HOST,
                                                             record='records/zen4/README.html', source_short='unavailable')
        for style, hosts in (row.get('style_speeds') or {}).items():
            source = extra[style][HOST]
            hosts[HOST] = dict(status='complete', bulk_bytes_per_cycle=source['bulk_bytes_per_cycle'],
                               bulk_gib_s=source['bulk_gib_s'], small_cycles=source['small_cycles'],
                               record=f'{EXTRA}#/{style}/{HOST}', binary_sha256=source['binary_sha256'],
                               aggregation='higher bulk of two passes; independently lower small average')
        if row.get('chart_hosts') and HOST not in row['chart_hosts']:
            row['chart_hosts'].append(HOST)
    dump('data.json', data, 'data.json')
    # records/speeds.json: one EPYC9R14 cell per measured hash, pointing at the full record.
    records = load('records/speeds.json')
    for doc, path in [(panel, PANEL), (extra, EXTRA)]:
        for name, hosts in doc.items():
            if name in records and HOST in hosts:
                c = hosts[HOST]
                records[name][HOST] = {k: c[k] for k in ('status', 'bulk_bytes_per_cycle', 'bulk_gib_s', 'small_cycles',
                                                         'bulk_selected_run', 'small_selected_run',
                                                         'bulk_run_spread_percent', 'small_run_spread_percent') if k in c}
                records[name][HOST].update(record=f'{path}#/{name}/{HOST}', binary_sha256=c['binary_sha256'],
                    clock_assumption='B per TSC tick (TSC 2.600 GHz); printed GiB/s assumes 3.5 GHz')
    dump('records/speeds.json', records, 'records/speeds.json')
    print('EPYC9R14 cells on', count, 'chart rows')


if __name__ == '__main__':
    main()
