"""Regenerate standalone and selectable inline robustness figures."""
from html import escape
from pathlib import Path
import re
import textwrap

ROOT = Path(__file__).resolve().parent.parent
OUT = ROOT / 'illustrations'
TEXT = '#263330'
MUTED = '#626e69'
ITEMS = [
    dict(title='Seed-independent pair', color='#866528', tint='#faf7f0',
         definition='The same two distinct inputs collide for every key.',
         applications='Fingerprinting, hash-based equality, deduplication, approximate membership filters.'),
    dict(title='Few-way collisions', color='#9a612f', tint='#fcf6ee',
         definition='A small set collides for some keys.',
         applications='Cuckoo hashing, bounded-capacity buckets, hash-indexed caches and hardware tables.'),
    dict(title='Weak-key multicollision', color='#b34d33', tint='#fcf4ef',
         definition='A large fixed set collides for a fraction of keys.',
         applications='Randomized dictionaries, long-lived caches, hash-based partitioning.'),
    dict(title='Multicollisions', color='#ad3640', tint='#fcf1f2',
         definition='A large fixed set collides for every key.',
         applications='Maps and sets, hash-based database operations, sharding, distinct-count sketches.'),
]
DESCRIPTION = (
    'Four categories of hash robustness issues, shown side by side: '
    'seed-independent pairs, few-way collisions, weak-key '
    'multicollisions, and seed-independent multicollisions. Each category has '
    'a short definition and its most directly relevant applications. '
    'All inputs discussed here are chosen without knowing the secret key.'
)


def text(x, y, content, size=15, fill=TEXT, weight=400, anchor='start', step=22):
    lines = content if isinstance(content, list) else [content]
    spans = '\n'.join(
        f'<tspan x="{x}" y="{y + i * step}">{escape(line)}</tspan>'
        for i, line in enumerate(lines)
    )
    return (f'<text font-size="{size}" fill="{fill}" font-weight="{weight}" '
            f'text-anchor="{anchor}">{spans}</text>')


def wrap(content, width, size=15):
    # Conservative line lengths; verify the generated SVG at its responsive sizes.
    return textwrap.wrap(content, width=int(width / (size * .51)),
                         break_long_words=False, break_on_hyphens=False)


def card_lines(item, width, title_size):
    return {
        'title': wrap(item['title'], width, title_size),
        'definition': wrap(item['definition'], width),
        'applications': wrap('Use for: ' + item['applications'], width),
    }


def card(item, x, width, height, lines, title_size, definition_y, application_y):
    return '\n'.join([
        f'<g class="robustness-cell" data-category="{escape(item["title"])}">',
        f'<rect x="{x}" y="0" width="{width}" height="{height}" rx="5" fill="{item["tint"]}"/>',
        text(x + 12, 28, lines['title'], title_size, item['color'], 700, step=title_size + 4),
        text(x + 12, definition_y, lines['definition'], step=21),
        text(x + 12, application_y, lines['applications'], step=21),
        '</g>',
    ])


def render(layout):
    # All layouts keep one row. The page scrolls this row on narrow screens
    # rather than shrinking its text below a readable size.
    width, gap, title_size = 1000, 12, 18
    card_width = (width - gap * (len(ITEMS) - 1)) / len(ITEMS)
    lines = [card_lines(item, card_width - 24, title_size) for item in ITEMS]
    definition_y = [28 + (len(item['title']) - 1) * (title_size + 4) + 23
                    for item in lines]
    application_y = [definition_y[col] + (len(item['definition']) - 1) * 21 + 29
                     for col, item in enumerate(lines)]
    height = max(application_y[col] + (len(item['applications']) - 1) * 21 + 22
                 for col, item in enumerate(lines))
    parts = [card(item, col * (card_width + gap), card_width, height,
                  lines[col], title_size, definition_y[col], application_y[col])
             for col, item in enumerate(ITEMS)]
    svg = '\n'.join([
        f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}" viewBox="0 0 {width} {height}" role="img" aria-labelledby="title desc">',
        '<title id="title">Hash robustness: colliding inputs and affected keys</title>',
        f'<desc id="desc">{escape(DESCRIPTION)}</desc>',
        f'<rect width="{width}" height="{height}" fill="white"/>',
        '<g font-family="Arial, Helvetica, sans-serif">',
        *parts,
        '</g>\n</svg>\n',
    ])
    suffix = '' if layout == 'desktop' else '-' + layout
    (OUT / ('robustness-scale' + suffix + '.svg')).write_text(svg)
    return svg


# Inline SVG exposes real text to browser selection. Prefix IDs because the
# three responsive layouts share one document.
inline = []
for layout in ('desktop', 'compact', 'mobile'):
    svg = render(layout)
    prefix = 'robustness-' + layout + '-'
    for name in ('title', 'desc'):
        svg = svg.replace('id="' + name + '"', 'id="' + prefix + name + '"')
    svg = svg.replace('aria-labelledby="title desc"',
                      'aria-labelledby="' + prefix + 'title ' + prefix + 'desc"')
    svg = svg.replace('<svg ', '<svg class="robustness-graphic robustness-graphic--' + layout + '" ', 1)
    inline.append(svg)

article = ROOT / 'index.html'
page = article.read_text()
start = '<!-- BEGIN ROBUSTNESS SVG -->'
end = '<!-- END ROBUSTNESS SVG -->'
block = (start + '\n<div class="robustness-scroll" tabindex="0" role="region" '
         'aria-label="Hash robustness categories; scroll horizontally on narrow screens">\n'
         + ''.join(inline) + '</div>\n' + end)
page, count = re.subn(re.escape(start) + r'.*?' + re.escape(end),
                     lambda match: block, page, count=1, flags=re.S)
if count != 1:
    raise ValueError('Expected one robustness figure in the article')
article.write_text(page)
