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
    dict(title='Pair-wise collision', color='#866528', tint='#faf7ef',
         definition='The same two inputs collide for some keys.',
         applications='Fingerprinting, hash-based equality, deduplication, approximate membership filters.'),
    dict(title='Seed-independent pair', color='#a45d37', tint='#fbf4ef',
         definition='The same two distinct inputs collide for every key.',
         applications='Fingerprinting, hash-based equality, deduplication, approximate membership filters.'),
    dict(title='Weak-key multicollision', color='#a45d37', tint='#fbf4ef',
         definition='A large fixed set collides for a fraction of keys.',
         applications='Randomized dictionaries, long-lived caches, hash-based partitioning.'),
    dict(title='Seed-independent multicollisions', color='#ad3640', tint='#fcf0f1',
         definition='A large fixed set collides for every key.',
         applications='Maps and sets, hash-based database operations, sharding, distinct-count sketches.'),
]
FEW_TITLE = 'Few-way collisions'
FEW_DEFINITION = 'A small set collides for some keys.'
FEW_APPLICATIONS = ('Cuckoo hashing, bounded-capacity buckets, hash-indexed caches '
                    'and hardware tables.')
DESCRIPTION = (
    'Hash robustness has two dimensions: how many inputs collide, and which keys '
    'make them collide. Rows are one fixed pair and a large fixed set. Columns '
    'are a fraction of keys and every key. The four cells show pair-wise '
    'collisions, seed-independent pairs, weak-key multicollisions, and '
    'seed-independent multicollisions. Few-way collisions sit between the rows. '
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
        'applications': wrap(item['applications'], width),
    }


def card_metrics(lines, title_size):
    title_end = 28 + (len(lines['title']) - 1) * (title_size + 4)
    definition_y = title_end + 27
    application_y = definition_y + len(lines['definition']) * 21 + 20
    return definition_y, application_y


def card(item, x, y, width, height, lines, title_size, application_y):
    definition_y, _ = card_metrics(lines, title_size)
    return '\n'.join([
        f'<g class="robustness-cell" data-category="{escape(item["title"])}">',
        f'<rect x="{x}" y="{y}" width="{width}" height="{height}" rx="7" fill="{item["tint"]}"/>',
        f'<path d="M {x + 18} {y} H {x + width - 18}" stroke="{item["color"]}" stroke-width="3"/>',
        text(x + 18, y + 28, lines['title'], title_size, item['color'], 700, step=title_size + 4),
        text(x + 18, y + definition_y, lines['definition'], step=21),
        f'<path d="M {x + 18} {y + application_y - 19} H {x + width - 18}" stroke="{item["color"]}" stroke-opacity=".2"/>',
        text(x + 18, y + application_y, 'Applications', 13, MUTED, 700),
        text(x + 18, y + application_y + 25, lines['applications'], step=21),
        '</g>',
    ])


def row_height(lines, title_size):
    application_y = max(card_metrics(item, title_size)[1] for item in lines)
    height = max(application_y + 25
                 + (len(item['applications']) - 1) * 21 + 22 for item in lines)
    return application_y, height


def few_way(x, y, width):
    definitions = wrap(FEW_DEFINITION, width)
    applications = wrap(FEW_APPLICATIONS, width)
    application_y = y + 25 + len(definitions) * 21 + 12
    parts = [
        text(x, y, FEW_TITLE, 16, weight=700),
        text(x, y + 25, definitions, step=21),
        text(x, application_y, 'Applications', 13, MUTED, 700),
        text(x, application_y + 25, applications, step=21),
    ]
    return '\n'.join(parts), application_y + 25 + len(applications) * 21


def render(layout):
    mobile = layout == 'mobile'
    width = 340 if mobile else 680 if layout == 'compact' else 1000
    title_size = 19 if mobile else 18 if layout == 'compact' else 20
    parts = []
    if mobile:
        # Keep the row groups and repeat the column label on narrow screens.
        # This is the same matrix, reflowed so its prose stays readable.
        y = 20
        parts.append(text(0, y, 'COLLIDING INPUTS × AFFECTED KEYS', 13, MUTED, 700))
        y += 36
        for row in range(2):
            parts.append(text(0, y, 'One fixed pair' if row == 0 else 'A large fixed set', 20, weight=700))
            y += 35
            for col in range(2):
                item = ITEMS[row * 2 + col]
                parts.append(text(0, y, 'For a fraction of keys' if col == 0 else 'For every key', 16, MUTED, 700))
                y += 18
                lines = card_lines(item, width - 36, title_size)
                application_y, height = row_height([lines], title_size)
                parts.append(card(item, 0, y, width, height, lines, title_size, application_y))
                y += height + 32
            if row == 0:
                note, y = few_way(18, y, width - 36)
                parts.append(note)
                y += 36
        height = y - 20
    else:
        left = 124 if layout == 'desktop' else 94
        gap = 16
        card_width = (width - left - gap) / 2
        inner_width = card_width - 36
        parts.append(text(left + (width - left) / 2, 18, 'Which keys make the inputs collide?', 14, MUTED, anchor='middle'))
        parts.append(text(0, 51, ['Colliding', 'inputs ↓'], 14, MUTED, step=20))
        for col, label in enumerate(['For a fraction of keys', 'For every key']):
            parts.append(text(left + col * (card_width + gap) + 18, 51, label, 17, weight=700))
        y = 76
        for row in range(2):
            row_items = ITEMS[row * 2:row * 2 + 2]
            lines = [card_lines(item, inner_width, title_size) for item in row_items]
            application_y, height = row_height(lines, title_size)
            parts.append(text(0, y + 28, ['One fixed', 'pair'] if row == 0 else ['A large', 'fixed set'], 17, weight=700, step=23))
            for col, item in enumerate(row_items):
                parts.append(card(item, left + col * (card_width + gap), y,
                                  card_width, height, lines[col], title_size, application_y))
            y += height
            if row == 0:
                y += 30
                note, y = few_way(left + 18, y, width - left - 36)
                parts.append(note)
                y += 16
        height = y + 8
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
block = start + '\n' + ''.join(inline) + end
page, count = re.subn(re.escape(start) + r'.*?' + re.escape(end),
                     lambda match: block, page, count=1, flags=re.S)
if count != 1:
    raise ValueError('Expected one robustness figure in the article')
article.write_text(page)
