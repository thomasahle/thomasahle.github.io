#!/usr/bin/env python3
"""Keep the hand-written cells of index.html consistent with data.json.

Three places in the page quote a hash's score or rate as text: the Figure 1 data table
(`<tr data-hash-id=ID>`, columns "Evidence / proof status" and "Collision score"), the
appendix "Selected score" paragraphs (`<p data-score-id=ID>`), and the profiles the chart
inspector shows (figure/profiles.json, checked by figure/build.py). This script checks the
first two against the canonical record and can rewrite a row on request.

    python3 cells.py --check          # exit 1 and list every cell whose number disagrees
    python3 cells.py --fix ID [ID...]  # rewrite the table's evidence and score cells for ID
                                       # from data.json (heuristic rows: collision.display /
                                       # score.display_text; proven rows: score only)

What --check verifies, per row: wherever a cell quotes a bits number it agrees with
`score.display` to within 0.05, and wherever the evidence cell or the appendix paragraph quotes
a 2^-x collision rate it agrees with `collision.display`. Cells that quote no number (curated
prose such as "proved (audited)" or "no score") are not judged; prose is never rewritten by
--check.
"""
from pathlib import Path
import json
import re
import sys

HERE = Path(__file__).resolve().parent
PAGE = HERE / 'index.html'
DATA = HERE / 'data.json'

ROW = re.compile(r'<tr data-hash-id="([^"]+)">(.*?)</tr>', re.S)
TD = re.compile(r'(<td[^>]*>)(.*?)(</td>)', re.S)
SCORE_P = re.compile(r'<p class="l-body" data-score-id="([^"]+)">(.*?)</p>', re.S)


def text(html):
    return re.sub(r'<[^>]+>', '', html)


def load():
    d = json.loads(DATA.read_text())
    rows = {r['id']: r for sec in ('heuristics', 'proven') for r in d[sec]}
    return PAGE.read_text(), rows


def table_span(html):
    t0 = html.find('<table id="chart-data-table"')
    t1 = html.find('</table>', t0)
    if t0 < 0 or t1 < 0:
        raise SystemExit('cells.py: #chart-data-table not found')
    return t0, t1


NUM = re.compile(r'(?<![\d^−-])(\d+(?:\.\d+)?)\s*(?:\*?\s*bits?)')
EXP = re.compile(r'2\s*(?:\^\s*)?[−-]\s*(\d+(?:\.\d+)?)')


def expectations(r):
    """(score number or None, rate exponent or None) from the canonical record."""
    score = r.get('score', {})
    num = score.get('display')
    num = float(num) if isinstance(num, (int, float)) else None
    exp = None
    disp = r.get('collision', {}).get('display', '')
    m = EXP.search(disp)
    if m and r.get('family') == 'heuristic' and r.get('bits_kind') != 'claimed':
        exp = float(m.group(1))
    return num, exp


def judge(where, t, num, exp, problems):
    """A cell that quotes a bits number or a 2^-x rate must agree with data.json (±0.05)."""
    if num is not None:
        quoted = [float(x) for x in NUM.findall(t)]
        if quoted and not any(abs(q - num) <= 0.05 for q in quoted):
            problems.append(f'{where}: quotes {quoted[0]:g} bits, data.json says {num:g}: "{t[:70]}"')
    if exp is not None:
        quoted = [float(x) for x in EXP.findall(t)]
        if quoted and not any(abs(q - exp) <= 0.05 for q in quoted):
            problems.append(f'{where}: quotes 2^-{quoted[0]:g}, data.json says 2^-{exp:g}: "{t[:70]}"')


def check(html, rows):
    problems = []
    t0, t1 = table_span(html)
    for m in ROW.finditer(html[t0:t1]):
        hid, body = m.groups()
        r = rows.get(hid)
        if not r:
            problems.append(f'table row {hid}: no data.json row')
            continue
        tds = [text(x[1]) for x in TD.findall(body)]
        num, exp = expectations(r)
        judge(f'table {hid} score cell', tds[3], num, None, problems)
        judge(f'table {hid} evidence cell', tds[2], None, exp, problems)
    for m in SCORE_P.finditer(html):
        hid, body = m.groups()
        r = rows.get(hid)
        if not r:
            problems.append(f'score paragraph {hid}: no data.json row')
            continue
        num, exp = expectations(r)
        judge(f'appendix {hid} selected-score paragraph', text(body), num, exp, problems)
    return problems


def fix(html, rows, ids):
    t0, t1 = table_span(html)
    tbl = html[t0:t1]
    for hid in ids:
        r = rows.get(hid)
        if not r:
            raise SystemExit(f'cells.py: unknown id {hid}')
        m = re.search(r'<tr data-hash-id="' + re.escape(hid) + r'">(.*?)</tr>', tbl, re.S)
        if not m:
            raise SystemExit(f'cells.py: no table row for {hid}')
        tds = list(TD.finditer(m.group(1)))
        body = m.group(1)
        new_score = r['score']['display_text']
        pieces = []
        last = 0
        for i, td in enumerate(tds):
            pieces.append(body[last:td.start()])
            inner = td.group(2)
            if i == 3:
                inner = new_score
            elif i == 2 and r.get('family') == 'heuristic' and r.get('collision', {}).get('display'):
                inner = r['collision']['display']
            pieces.append(td.group(1) + inner + td.group(3))
            last = td.end()
        pieces.append(body[last:])
        tbl = tbl[:m.start(1)] + ''.join(pieces) + tbl[m.end(1):]
        print(f'fixed table row {hid}')
    return html[:t0] + tbl + html[t1:]


def main():
    html, rows = load()
    if '--fix' in sys.argv:
        ids = sys.argv[sys.argv.index('--fix') + 1:]
        if not ids:
            raise SystemExit('cells.py: --fix needs at least one id')
        html = fix(html, rows, ids)
        PAGE.write_text(html)
    problems = check(html, rows)
    for p in problems:
        print(p)
    if '--check' in sys.argv or '--fix' in sys.argv:
        print(f'cells.py: {len(problems)} inconsistent cells' if problems else 'cells.py: table and appendix cells agree with data.json')
        return 1 if problems else 0
    return 0


if __name__ == '__main__':
    sys.exit(main())
