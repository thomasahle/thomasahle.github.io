#!/usr/bin/env python3
"""Number the footnotes in index.html and keep their labels and backlinks in sync.

Authoring is two pieces of markup and no numbers:

    inline:   <a class="note-ref" href="#note-foo"></a>
    endnotes: <li id="note-foo"><span class="note-text"><b>Title.</b> Body.</span></li>

This script (run from anywhere) numbers notes by the first appearance of a reference in the
article, writes the number into each reference and its aria-label ("Note 3: Title", the title
taken from the note's own <b>...</b>), gives every reference a stable id (note-ref-foo,
note-ref-foo-2, ...), rewrites the backlinks so each note points back at every place it is
cited, and reorders the <ol> so the CSS numbering matches. It is idempotent.

    python3 notes.py           # rewrite index.html in place
    python3 notes.py --check   # exit 1 if anything would change or is inconsistent

Errors: a reference to a note that does not exist, a note nobody cites, a duplicate note id, or
a note without a <b>Title.</b>.
"""
from pathlib import Path
import re
import sys

HERE = Path(__file__).resolve().parent
PAGE = HERE / 'index.html'

REF = re.compile(r'<a class="note-ref"[^>]*?href="#(note-[A-Za-z0-9_-]+)"[^>]*>.*?</a>', re.S)
ASIDE = re.compile(r'(<aside aria-label="Footnotes"[^>]*>.*?<ol>\n?)(.*?)(</ol>)', re.S)
ITEM_START = re.compile(r'<li id="(note-[A-Za-z0-9_-]+)"[^>]*>')
TITLE = re.compile(r'<span class="note-text">\s*<b>(.*?)</b>', re.S)


def strip_tags(s):
    return re.sub(r'<[^>]+>', '', s).strip().rstrip('.')


def renumber(html):
    m = ASIDE.search(html)
    if not m:
        raise SystemExit('notes.py: no <aside aria-label="Footnotes"> with an <ol> found')
    head, body, tail = m.group(1), m.group(2), m.group(3)
    # each note runs from its <li id="note-..."> to the next one (notes may contain nested lists)
    starts = list(ITEM_START.finditer(body))
    if body[:starts[0].start()].strip() if starts else body.strip():
        raise SystemExit('notes.py: unexpected markup before the first note in the <ol>')
    items = {}
    for k, im in enumerate(starts):
        nid = im.group(1)
        if nid in items:
            raise SystemExit(f'notes.py: duplicate note id {nid}')
        end = starts[k + 1].start() if k + 1 < len(starts) else len(body)
        chunk = body[im.start():end].rstrip()
        if not chunk.endswith('</li>'):
            raise SystemExit(f'notes.py: {nid} does not end with </li> (unclosed tags inside the note?)')
        items[nid] = chunk

    # references in article order, excluding anything inside the endnotes aside itself
    article = html[:m.start()] + html[m.end():]
    order = []
    for rm in REF.finditer(article):
        if rm.group(1) not in order:
            order.append(rm.group(1))
    missing = [n for n in order if n not in items]
    unused = [n for n in items if n not in order]
    if missing:
        raise SystemExit('notes.py: referenced but no <li id=...>: ' + ', '.join(missing))
    if unused:
        raise SystemExit('notes.py: notes nobody references: ' + ', '.join(unused))

    number = {n: i + 1 for i, n in enumerate(order)}
    titles = {}
    for nid, li in items.items():
        tm = TITLE.search(li)
        if not tm:
            raise SystemExit(f'notes.py: {nid} has no <span class="note-text"><b>Title.</b>')
        titles[nid] = strip_tags(tm.group(1))

    # rewrite references (article part only), assigning ids by occurrence
    seen = {}
    ref_ids = {n: [] for n in order}

    def fix_ref(rm):
        nid = rm.group(1)
        k = seen.get(nid, 0) + 1
        seen[nid] = k
        rid = f'note-ref-{nid[5:]}' + ('' if k == 1 else f'-{k}')
        ref_ids[nid].append(rid)
        n = number[nid]
        return (f'<a class="note-ref" href="#{nid}" id="{rid}" role="doc-noteref" '
                f'aria-label="Note {n}: {titles[nid]}">{n}</a>')

    before = REF.sub(fix_ref, html[:m.start()])
    after = REF.sub(fix_ref, html[m.end():])

    # rewrite items: fresh backlinks, one per citation, and list order = numbering order
    new_items = []
    for nid in order:
        li = items[nid]
        li = re.sub(r'(\s*<a class="note-backlink"[^>]*>[^<]*</a>)+\s*</li>$', '</li>', li)
        links = ''.join(
            f' <a class="note-backlink" href="#{rid}" aria-label="Back to '
            f'{"note" if len(ref_ids[nid]) == 1 else f"citation {j + 1} of note"} {number[nid]}">↩</a>'
            for j, rid in enumerate(ref_ids[nid]))
        li = li[:-len('</li>')].rstrip() + links + '</li>'
        li = re.sub(r'<li id="' + re.escape(nid) + r'"[^>]*>', f'<li id="{nid}" tabindex="-1">', li, count=1)
        new_items.append(li + '\n')
    return before + head + ''.join(new_items) + tail + after, order, titles


def main():
    html = PAGE.read_text()
    out, order, titles = renumber(html)
    if '--check' in sys.argv:
        if out != html:
            print('notes.py: index.html is out of date; run python3 notes.py')
            return 1
        print(f'notes.py: {len(order)} notes consistent')
        return 0
    if out != html:
        PAGE.write_text(out)
    for n, nid in enumerate(order, 1):
        print(f'{n}. {titles[nid]}  ({nid})')
    return 0


if __name__ == '__main__':
    sys.exit(main())
