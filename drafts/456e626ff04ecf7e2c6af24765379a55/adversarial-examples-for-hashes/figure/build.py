"""Rebuild publication SVGs, PNGs, and inspection data from ../data.json.
Run: python3 blog/adversarial-examples-for-hashes/figure/build.py
Requires matplotlib. Also refreshes the existing witness-context note from data.json.
Does not alter measurements or chart controls.
"""
from pathlib import Path
import copy
import hashlib
import json
import math
import html
import re
import xml.etree.ElementTree as ET

import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.transforms import Bbox
from matplotlib.ticker import FixedLocator, FuncFormatter
from matplotlib.lines import Line2D

ROOT = Path(__file__).resolve().parent.parent
SOURCE = ROOT / 'data.json'
DATA = json.loads(SOURCE.read_text())
OUT = ROOT / 'figure'
INK, MUTED, GRID = '#263330', '#626e69', '#e5ebe7'
BLUE, ORANGE, CLAIM = '#176f83', '#bd5637', '#737d8d'
BG = '#ffffff'
plt.rcParams.update({'font.family': 'sans-serif', 'font.sans-serif': ['Arial', 'DejaVu Sans'],
                     'svg.fonttype': 'none', 'svg.hashsalt': 'hash-collision-figure',
                     'axes.unicode_minus': False})
HOSTS = {
 'm2': dict(key='smh_m2_bulk_Bpc', host='M2Pro', name='APPLE M2 PRO', provisional=False),
 'xeon': dict(key='smh_xeon_bulk_Bpc', host='Xeon8375C', name='INTEL XEON 8375C', provisional=False),
}
SCALES = {
    'linear': dict(name='Linear', power=1, ticks=list(range(0, 129, 16))),
    'sqrt': dict(name='Square root', power=.5, ticks=[0, 4, 16, 36, 64, 100, 128]),
    'quadratic': dict(name='Quadratic', power=2, ticks=[0, 32, 64, 80, 96, 112, 128]),
    'log': dict(name='Logarithmic', ticks=[0, 1, 2, 4, 8, 16, 32, 64, 128]),
}


def signed_power(values, power):
    """Keep the small below-zero plot padding finite; data are nonnegative."""
    values = np.asarray(values)
    return np.sign(values) * np.abs(values) ** power


def place_power_labels(ax, annotations, rows):
    """Fit the power-scale labels without ever moving a scientific point.

    Search in display coordinates so the clearance is independent of scale.
    Existing linear/log layouts keep their hand-placed annotations unchanged.
    """
    renderer = ax.figure.canvas.get_renderer()
    axes_box = ax.get_window_extent()
    unit = ax.figure.dpi / 72
    points = [(row['id'], ax.transData.transform((row['speed'], row['bits'])))
              for row in rows]
    occupied = []
    for annotation, ids in annotations:
        box = annotation.get_bbox_patch().get_window_extent(renderer)
        origin = ax.transData.transform(annotation.xyann)
        offsets = box.extents - np.tile(origin, 2)
        candidates = [(0, 0)] + sorted(
            ((dx, dy) for dx in range(-240, 241, 8)
             for dy in range(-160, 161, 8) if dx or dy),
            key=lambda offset: offset[0] ** 2 + 1.3 * offset[1] ** 2)
        for dx, dy in candidates:
            anchor = origin + np.array([dx, dy]) * unit
            candidate = Bbox.from_extents(*(offsets + np.tile(anchor, 2)))
            padded = candidate.padded(4 * unit)
            if (candidate.x0 < axes_box.x0 or candidate.x1 > axes_box.x1
                    or candidate.y0 < axes_box.y0 or candidate.y1 > axes_box.y1 + 4 * unit):
                continue
            if any(padded.overlaps(other) for other in occupied):
                continue
            if any(rid not in ids and padded.contains(*point) for rid, point in points):
                continue
            annotation.set_position(ax.transData.inverted().transform(anchor))
            occupied.append(candidate)
            break
        else:
            raise ValueError(f'No unobstructed power-scale label position for {ids}')
    ax.figure.canvas.draw()

def rows_for(host):
    rows = []
    for source in DATA['heuristics'] + DATA['proven']:
        variants = source.get('style_speeds')
        expanded = []
        if variants:
            for name, speeds in variants.items():
                row = copy.deepcopy(source)
                style = name.split('-')[-1]
                row.update(id=source['id'] + '-' + style, name='HalftimeHash ' + style, label='HalftimeHash ' + style)
                row['speeds'][host['key']]['value'] = speeds[host['host']]['bulk_bytes_per_cycle']
                expanded.append(row)
        else:
            expanded = [source]
        for row in expanded:
            speed = row['speeds'][host['key']]['value']
            if not row.get('chart_eligible') or speed is None or not math.isfinite(speed) or speed <= 0:
                continue
            if row.get('chart_hosts') and host['host'] not in row['chart_hosts']:
                continue  # a row may be plotted on a subset of hosts when measurements are host-specific
            kind = 'claim' if row['bits_kind'] == 'claimed' else 'proof' if row['family'] == 'proven' else 'witness'
            row.update(speed=speed, kind=kind)
            rows.append(row)
    return rows

# Landmark labels use fixed data coordinates; every other point remains inspectable.
LABELS = {'m2': {'siphash-1-3': (2.05, 20.0, 'left', 'SipHash-1-3', ''),
        'ghash': (3.1296782804570187, 222.86094420380775, 'left', 'GHASH', ''),
        'poly1305': (1.9238814757276592, 168.89701257893051, 'right', 'Poly1305', ''),
        'umash128': (8.28213798512708, 222.86094420380775, 'left', 'UMASH-128', ''),
        'chain128-v3': (13.472985573602719, 128.0, 'left', 'ChainHash-128 v3 (ours)', ''),
        'halftime24-fixed': (0.57, 147.0333894396205, 'left', 'HalftimeHash24 (fixed)', ''),
        'chain-v3': (45.47443397859695, 48.50293012833273, 'right', 'ChainHash v3 (ours)', ''),
        'polymur': (4.508070852474694, 21.112126572366314, 'left', 'PolymurHash', ''),
        'highway': (1.0471926475891462, 73.51669471981025, 'right', 'HighwayHash', ''),
        'rapid3': (19.40684253056874, 18.37917367995256, 'left', 'rapidhash v3', ''),
        'xxh3-64': (21.917207922939696, 9.18958683997628, 'left', 'XXH3-64', ''),
        'komi': (10.563386085541142, 8.0, 'left', 'komihash', '')},
 'xeon': {'siphash-1-3': (1.9, 20.0, 'left', 'SipHash-1-3', ''),
          'ghash': (8.28213798512708, 256.0, 'left', 'GHASH', ''),
          'poly1305': (2.771209442996604, 194.0117205133309, 'right', 'Poly1305', ''),
          'umash128': (5.74978260458602, 21.112126572366314, 'right', 'UMASH-128', ''),
          'chain128-v3': (6.493543741487783, 194.0117205133309, 'right', 'ChainHash-128 v3 (ours)', ''),
          'halftime24-fixed': (11.929809233130745, 222.86094420380775, 'left', 'HalftimeHash24 (fixed)', ''),
          'chain-v3': (27.9541260508884, 147.0333894396205, 'left', 'ChainHash v3 (ours)', ''),
          'clhash': (24.752300760967383, 42.22425314473263, 'left', 'CLHASH', ''),
          'polymur': (4.508070852474694, 42.22425314473263, 'right', 'PolymurHash', ''),
          'highway': (2.4537991092912335, 36.75834735990512, 'right', 'HighwayHash', ''),
          'rapid3': (6.493543741487783, 18.37917367995256, 'left', 'rapidhash v3', ''),
          'xxh3-64': (24.752300760967383, 9.18958683997628, 'left', 'XXH3-64', ''),
          'komi': (10.563386085541142, 8.0, 'left', 'komihash', '')}}
MOBILE_LABELS = {'m2': {'ghash': (3.1296782804570187, 256.0, 'left', 'GHASH', ''),
        'poly1305': (0.57, 168.89701257893051, 'left', 'Poly1305', ''),
        'chain-v3': (45.47443397859695, 17.0, 'right', 'ChainHash v3 (ours)', ''),
        'chain128-v3': (45.47443397859695, 147.0333894396205, 'right', 'ChainHash-128 v3 (ours)', ''),
        'halftime24-fixed': (0.57, 42.22425314473263, 'left', 'HalftimeHash24 (fixed)', ''),
        'highway': (0.57, 84.44850628946526, 'left', 'HighwayHash', ''),
        'xxh3-64': (45.47443397859695, 10.556063286183154, 'right', 'XXH3-64', ''),
        'komi': (10.563386085541142, 6.062866266041593, 'left', 'komihash', '')},
 'xeon': {'ghash': (4.508070852474694, 256.0, 'left', 'GHASH', ''),
          'poly1305': (0.57, 222.86094420380775, 'left', 'Poly1305', ''),
          'chain-v3': (45.47443397859695, 17.0, 'right', 'ChainHash v3 (ours)', ''),
          'chain128-v3': (51.35676363205364, 194.0117205133309, 'right', 'ChainHash-128 v3 (ours)', ''),
          'halftime24-fixed': (0.57, 111.43047210190387, 'left', 'HalftimeHash24 (fixed)', ''),
          'highway': (0.57, 42.22425314473263, 'left', 'HighwayHash', ''),
          'xxh3-64': (45.47443397859695, 10.556063286183154, 'right', 'XXH3-64', ''),
          'komi': (10.563386085541142, 6.062866266041593, 'left', 'komihash', '')}}

LINEAR_LABELS = {'m2': {'siphash-1-3': (0.62, 54, 'left', 'SipHash-1-3', ''),
        'ghash': (2.9, 132, 'left', 'GHASH', ''),
        'poly1305': (2.7, 109, 'left', 'Poly1305', ''),
        'umash128': (9.2, 91, 'left', 'UMASH-128', ''),
        'chain-v3': (35, 77, 'right', 'ChainHash v3 (ours)', ''),
        'clhash': (18, 96, 'left', 'CLHASH', ''),
        'polymur': (5.8, 49, 'left', 'PolymurHash', ''),
        'highway': (1.05, 74, 'right', 'HighwayHash', ''),
        'rapid3': (20, 38, 'left', 'rapidhash v3', ''),
        'xxh3-64': (23, 18, 'left', 'XXH3-64', ''),
        'komi': (8.4, 17, 'left', 'komihash', ''),
        'chain128-v3': (12.4, 132, 'left', 'ChainHash-128 v3 (ours)', ''),
        'halftime24-fixed': (4.7, 105, 'left', 'HalftimeHash24 (fixed)', '')},
 'xeon': {'siphash-1-3': (0.62, 54, 'left', 'SipHash-1-3', ''),
          'ghash': (6.3, 133, 'right', 'GHASH', ''),
          'poly1305': (3.6, 110, 'right', 'Poly1305', ''),
          'umash128': (4.8, 91, 'right', 'UMASH-128', ''),
          'clhash': (10.5, 100, 'left', 'CLHASH', ''),
          'polymur': (4.4, 43, 'right', 'PolymurHash', ''),
          'highway': (2.6, 76, 'right', 'HighwayHash', ''),
          'rapid3': (8.8, 43, 'left', 'rapidhash v3', ''),
          'xxh3-64': (24, 25, 'left', 'XXH3-64', ''),
          'komi': (8.4, 15, 'left', 'komihash', ''),
          'chain128-v3': (16.5, 126, 'left', 'ChainHash-128 v3 (ours)', ''),
          'chain-v3': (26, 77, 'left', 'ChainHash v3 (ours)', ''),
          'halftime24-fixed': (24, 110, 'left', 'HalftimeHash24 (fixed)', '')}}
LINEAR_MOBILE_LABELS = {'m2': {'ghash': (0.58, 134, 'left', 'GHASH', ''),
        'poly1305': (0.58, 111, 'left', 'Poly1305', ''),
        'chain-v3': (40, 76, 'right', 'ChainHash v3 (ours)', ''),
        'highway': (0.58, 84, 'left', 'HighwayHash', ''),
        'xxh3-64': (50, 37, 'right', 'XXH3-64', ''),
        'komi': (8.5, 18, 'left', 'komihash', ''),
        'chain128-v3': (52, 116, 'right', 'ChainHash-128 v3 (ours)', ''),
        'halftime24-fixed': (0.58, 91, 'left', 'HalftimeHash24 (fixed)', '')},
 'xeon': {'ghash': (0.58, 134, 'left', 'GHASH', ''),
          'poly1305': (0.58, 111, 'left', 'Poly1305', ''),
          'highway': (0.58, 84, 'left', 'HighwayHash', ''),
          'xxh3-64': (48, 40, 'right', 'XXH3-64', ''),
          'komi': (8.5, 18, 'left', 'komihash', ''),
          'chain128-v3': (52, 116, 'right', 'ChainHash-128 v3 (ours)', ''),
          'halftime24-fixed': (0.58, 91, 'left', 'HalftimeHash24 (fixed)', ''),
          'chain-v3': (40, 76, 'right', 'ChainHash v3 (ours)', '')}}

def render(key, mobile=False, compact=False, scale='linear'):
    host = HOSTS[key]
    rows = rows_for(host)
    logarithmic = scale == 'log'
    settings = SCALES[scale]
    power = settings.get('power', 1)
    # Keep 0 and 128 at identical heights in the linear and power views.
    min_y = -.25 if logarithmic else -128 * (2 / 128) ** (1 / power)
    max_y = 256 if logarithmic else 128 * (134 / 128) ** (1 / power)
    width, height = (390, 875) if mobile else (760, 840) if compact else (1100, 820)
    fig = plt.figure(figsize=(width / 72, height / 72), dpi=144, facecolor=BG)
    def text(x, y, label, size=14, color=INK, weight='normal', **kw):
        return fig.text(x/width, 1-y/height, label, size=size, color=color, weight=weight,
                        va='top', **kw)
    if mobile:
        text(22, 22, 'FAST HASHES NEED PROOFS.', 11, BLUE, 'bold')
        text(22, 49, 'Speed and collision bounds', 21, INK, 'bold')
        text(22, 80, host['name'] + ' · BULK', 11, MUTED, 'bold').set_gid('host-name')
        text(22, 102, 'Provisional timings' if host['provisional'] else ('Score: log above 1 · linear 0–1' if logarithmic else 'B/cycle · logarithmic speed axis'), 11, MUTED).set_gid('host-note')
        top, bottom, left, right = 295, 688, 49, 372
        legends = [(22,173,BLUE,'●','Proved lower bound'),(22,199,ORANGE,'◆','Limit from a colliding pair'),(22,225,CLAIM,'○','Unresolved claim')]
    else:
        text(40, 26, 'ADVERSARIAL EXAMPLES FOR FAST HASH FUNCTIONS', 11, MUTED, 'bold')
        text(40, 54, 'Fast hashes need proofs.', 30 if compact else 36, INK, 'bold')
        text(40, 104, 'A passing test suite cannot rule out a bad pair of inputs.', 12 if compact else 16, MUTED).set_gid('figure-subtitle')
        text(width-40, 33, host['name'] + ' · BULK', 11, MUTED, 'bold', ha='right').set_gid('host-name')
        text(width-40, 54, 'Provisional timings' if host['provisional'] else 'Bulk · B/cycle', 11, MUTED, ha='right').set_gid('host-note')
        top, bottom, left, right = (238 if compact else 218), (655 if compact else 635), 68 if compact else 83, width-40
        legends = [(40,174 if compact else 154,BLUE,'●','Proved lower bound  ↑'), (278 if compact else 395,174 if compact else 154,ORANGE,'◆','Limit from a colliding pair  ↓'), (528 if compact else 786,174 if compact else 154,CLAIM,'○','Unresolved claim')]
    # Text legend uses explicit Unicode glyphs and bound directions.
    for x,y,c,glyph,label in legends:
        marker = 'D' if glyph == '◆' else 'o'
        fig.add_artist(Line2D([x/width+6/width],[1-(y+11)/height],transform=fig.transFigure,
            marker=marker,markersize=7,markerfacecolor='white' if glyph == '○' else c,
            markeredgecolor=c,markeredgewidth=1.3,linestyle='none'))
        text(x+25,y+2,label,12 if compact else 13 if mobile else 14,INK)
    ax = fig.add_axes([left/width, (height-bottom)/height, (right-left)/width, (bottom-top)/height])
    ax.set_xscale('log'); ax.set_xlim(.5, 64)
    # Preserve zero exactly: 0–1 is linear, and above 1 every doubling has
    # equal spacing. Scores remain in bits; this is only a display transform.
    if logarithmic:
        ax.set_yscale('symlog', base=2, linthresh=1, linscale=.5)
    elif power != 1:
        ax.set_yscale('function', functions=(lambda y: signed_power(y, power),
                                           lambda y: signed_power(y, 1 / power)))
    ax.set_ylim(min_y, max_y)
    ax.set_facecolor(BG)
    for spine in ax.spines.values(): spine.set_visible(False)
    ax.set_axisbelow(True)
    ax.xaxis.set_major_locator(FixedLocator([.5,1,2,5,10,20,50]))
    ax.xaxis.set_major_formatter(FuncFormatter(lambda n,_: f'{n:g}'))
    ax.xaxis.set_minor_locator(FixedLocator([]))
    ax.yaxis.set_major_locator(FixedLocator(settings['ticks']))
    ax.yaxis.set_major_formatter(FuncFormatter(lambda n,_: f'{n:g}'))
    ax.yaxis.set_minor_locator(FixedLocator([]))
    ax.tick_params(axis='both', length=0, labelsize=11 if mobile else 12, colors=MUTED, pad=9)
    ax.grid(axis='y',color=GRID,lw=.7)
    # Mark the linear segment explicitly; every-seed pairs can also score > 0.
    ax.axhspan(min_y,1 if logarithmic else 128 * (2.5 / 128) ** (1 / power),color='#fcf2ed',zorder=0)
    ax.set_xlabel('Bulk speed · B/cycle · log scale  →',fontsize=12 if mobile else 14,labelpad=15,color=INK)
    axis_title = ('Score · bits · log scale' if mobile else 'Collision score · bits · log scale (linear 0–1)') if logarithmic else 'Collision score · bits'
    if power != 1:
        axis_title = ('Score · bits' if mobile else 'Collision score · bits') + ' · ' + settings['name'].lower() + ' scale'
    text(left, top-28, axis_title, 11 if mobile else 13, MUTED)
    point_positions = {}
    annotations = []
    for row in rows:
        x,y=row['speed'],row['bits']
        if not (.5 <= x <= 64 and 0 <= y <= max_y):
            raise ValueError(f"Update the figure limits before plotting {row['id']}: ({x}, {y})")
        color=BLUE if row['kind']=='proof' else CLAIM if row['kind']=='claim' else ORANGE
        marker='o' if row['kind'] in ('proof','claim') else 'x' if row.get('key_free') else 'D'
        artist=ax.scatter([x],[y],s=66 if row['kind']=='proof' else 45,marker=marker,
                          facecolors='white' if row['kind']=='claim' else color,
                          **({} if marker == 'x' else {'edgecolors':color}),linewidths=1.6 if row['kind']=='claim' or row.get('key_free') else .7,
                          zorder=4,alpha=.85 if row.get('key_free') else 1)
        artist.set_gid('point-'+row['id'])
        px,py=ax.transData.transform((x,y))
        point_positions[row['id']]=(px*72/fig.dpi,height-py*72/fig.dpi)
    label_set = MOBILE_LABELS if mobile or compact else LABELS
    if not logarithmic:
        label_set = LINEAR_MOBILE_LABELS if mobile or compact else LINEAR_LABELS
    labels = dict(label_set[key])
    if scale == 'sqrt' and (mobile or compact):
        # Give these neighboring labels separate rows before fitting them.
        for rid, score in [('halftime24-fixed', 86), ('highway', 57)]:
            tx, _, align, name, number = labels[rid]
            labels[rid] = (tx, score, align, name, number)
    row_by_id={r['id']:r for r in rows}
    for id,(tx,ty,align,name,number) in labels.items():
        if id not in row_by_id: continue
        row=row_by_id[id]
        # Keep annotations to names; exact scores and key models stay in the
        # accessible point description, hover details, and inspection record.
        label = name + ('*' if row['bits_kind'] == 'measured' else '')
        c=BLUE if row['kind']=='proof' else CLAIM if row['kind']=='claim' else ORANGE
        # Deliberate label positions, with leaders terminating at the true data.
        annotation=ax.annotate(label,xy=(row['speed'],row['bits']),xytext=(tx,ty),
            fontsize=12 if mobile else 14,weight='bold' if id in ('chain-v3','chain128-v3') else 'normal',
            color=c,ha=align,va='top',linespacing=1.5,
            bbox=dict(boxstyle='square,pad=.2',fc='white',ec='none',alpha=.96),
            arrowprops=dict(arrowstyle='-',color=c,alpha=.5,lw=.8,shrinkA=5,shrinkB=8),zorder=5)
        annotation.set_gid('label-'+id)
        annotation.arrow_patch.set_gid('leader-'+id)
        annotations.append((annotation,{id}))
    if not (mobile or compact):
        half=[r for r in rows if r['id'].startswith('halftimehash-')]
        center=math.exp(sum(math.log(r['speed']) for r in half)/len(half))
        tx,ty=((8.8,40) if key=='m2' else (14.5,17)) if logarithmic else ((5.4,77) if key=='m2' else (15,82))
        representative = min(half,key=lambda r:abs(math.log(r['speed']/center)))
        a=ax.annotate('HalftimeHash',xy=(representative['speed'],representative['bits']),xytext=(tx,ty),
            ha='center',va='top',fontsize=12,color=BLUE,linespacing=1.4,
            bbox=dict(fc='white',ec='none',pad=3),arrowprops=dict(arrowstyle='-',color=BLUE,lw=.8,shrinkB=8),zorder=3)
        a.set_gid('label-'+representative['id'])
        a.arrow_patch.set_gid('leader-halftimehash')
        annotations.append((a,{r['id'] for r in half}))
    fig.canvas.draw()
    if power != 1:
        place_power_labels(ax, annotations, rows)
    label_boxes = [(annotation.get_bbox_patch().get_window_extent(), ids) for annotation,ids in annotations]
    for box, ids in label_boxes:
        for row in rows:
            if row['id'] not in ids and box.contains(*ax.transData.transform((row['speed'],row['bits']))):
                raise ValueError(f"{key}-{scale}{'-mobile' if mobile else '-compact' if compact else ''}: label {ids} obscures {row['id']}")
    for i,(box,ids) in enumerate(label_boxes):
        for other,other_ids in label_boxes[i+1:]:
            if box.overlaps(other):
                raise ValueError(f"{key}-{scale}{'-mobile' if mobile else '-compact' if compact else ''}: labels {ids} and {other_ids} overlap")
    key_free=sum(bool(r.get('key_free')) for r in rows)
    if mobile:
        text(22,748,f'×  {key_free} variants: a pair collides for every seed.',12,ORANGE,'bold')
        text(22,776,'Guarantees require the stated random keys.\nGHASH timing includes setup costs.\n* Rate estimated from samples; worse pairs may exist.',10.5,MUTED,linespacing=1.6)
        text(22,848,'Thomas Dybdahl Ahle · thomasahle.com',10,MUTED)
    else:
        text(40,711+ (20 if compact else 0),f'×  {key_free} variants have a fixed pair that collides for every seed.',14,ORANGE,'bold')
        text(40,744+ (20 if compact else 0),'Guarantees require the stated random keys. GHASH timing includes setup costs.',12,MUTED)
        text(40,765+ (20 if compact else 0),'* Collision rates estimated from samples. A worse pair may exist; these limits are not a ranking.',12,MUTED)
        text(width-40,height-26,'Thomas Dybdahl Ahle · thomasahle.com',10,MUTED,ha='right')
    suffix = ('' if scale == 'linear' else '-' + scale) + ('-mobile' if mobile else '-compact' if compact else '')
    destination=OUT/f'{key}{suffix}.svg'
    fig.savefig(destination,format='svg',facecolor=BG,metadata={'Date':None,'Creator':'Thomas Dybdahl Ahle; generated from data.json'})
    fig.savefig(OUT/f'{key}{suffix}.png',dpi=144,facecolor=BG)
    plt.close(fig)
    # Keep semantic, generously sized targets in the exact exported SVG.
    ns='http://www.w3.org/2000/svg'; ET.register_namespace('',ns); ET.register_namespace('xlink','http://www.w3.org/1999/xlink')
    tree=ET.parse(destination); svg=tree.getroot()
    svg.set('role','img'); svg.set('aria-label','Fast hashes need proofs: collision bounds versus bulk speed on '+host['name']+ ('. Logarithmic axes; score is linear from zero to one bit.' if logarithmic else '. '+settings['name']+' collision-score scale; logarithmic speed axis.'))
    for g in svg.iter('{'+ns+'}g'):
        id=g.get('id','')
        if id.startswith('point-'):
            rid=id[6:]; row=row_by_id[rid]
            g.set('data-point-id',rid); g.set('data-speed',str(row['speed']));g.set('data-bits',str(row['bits']))
            g.set('tabindex','0'); g.set('role','button')
            g.set('aria-label',row['name']+': '+row['score']['display_text']+' bits; '+str(row['speed'])+' B/cycle. Inspect result.')
            px,py=point_positions[rid]
            ET.SubElement(g,'{'+ns+'}circle',{'cx':str(px),'cy':str(py),'r':'12','fill':'transparent','class':'point-hit'})
            ET.SubElement(g,'{'+ns+'}circle',{'cx':str(px),'cy':str(py),'r':'10','fill':'none','stroke':INK,'stroke-width':'1.4','opacity':'0','class':'point-ring'})
        if id.startswith('label-'):
            g.set('data-label-for',id[6:])
            g.set('data-label-key','halftimehash' if id.startswith('label-halftimehash-') else id[6:])
        if id.startswith('leader-'):
            g.set('data-leader-for',id[7:])
    style=ET.SubElement(svg,'{'+ns+'}style')
    style.text='[data-point-id],[data-label-for]{cursor:pointer}[data-point-id]:focus{outline:none}[data-point-id]:focus .point-ring,[data-point-id].is-active .point-ring{opacity:1}'
    tree.write(destination,encoding='unicode',xml_declaration=True)
    return rows

profile_source = OUT / 'profiles.json'
profiles = json.loads(profile_source.read_text())['profiles']
profile_for = {}
for profile in profiles:
    if not 1 <= len(profile['links']) <= 4:
        raise ValueError(f'Profiles require one to four public documents: {profile["ids"]}')
    for link in profile['links']:
        if not link['url'].startswith(('https://', 'http://')) and re.search(r'\.(md|tex)(?:#|$)', link['url']):
            raise ValueError(f'Unrendered profile document: {link["url"]}')
    for row_id in profile['ids']:
        if row_id in profile_for:
            raise ValueError(f'Duplicate hash profile: {row_id}')
        if not profile['results'].get(row_id):
            raise ValueError(f'Missing result explanation: {row_id}')
        notes = profile.get('reader_notes', {}).get(row_id, {})
        for field in ('evidence', 'key', 'scope'):
            if not notes.get(field):
                raise ValueError(f'Missing reader explanation: {row_id}.{field}')
        profile_for[row_id] = profile
for host in HOSTS.values():
    for row in rows_for(host):
        if row['id'] not in profile_for:
            raise ValueError(f'Missing hash profile: {row["id"]}')

manifest={'source_sha256':hashlib.sha256(SOURCE.read_bytes()).hexdigest(),
          'profiles_sha256':hashlib.sha256(profile_source.read_bytes()).hexdigest(),
          'profiles':{p['ids'][0]:{k:p[k] for k in ('authors','background','links','sources')} for p in profiles},
          'hosts':{}}
for key,host in HOSTS.items():
    rows=render(key)
    render(key,True)
    render(key,compact=True)
    for scale in ('sqrt', 'quadratic', 'log'):
        render(key,scale=scale)
        render(key,True,scale=scale)
        render(key,compact=True,scale=scale)
    manifest['hosts'][key]={'name':host['name'],'provisional':host['provisional'],'key':host['key'],'rows':[]}
    for r in rows:
        profile = profile_for[r['id']]
        manifest['hosts'][key]['rows'].append({
          'id':r['id'],'name':r['name'],'label':r['label'],'kind':r['kind'],'key_free':bool(r.get('key_free')),
          'speed':r['speed'],'bits':r['bits'],'score':(f"{r['bits']:g} (claimed)" if r['kind']=='claim' else f"≥ {math.floor(r['bits']*100)/100:g}" if r['kind']=='proof' else f"≈ {r['bits']:.2f}*" if r['bits_kind']=='measured' else f"≤ {math.ceil(r['bits']*100)/100:g}"),'anchor':r['anchor'],
          'evidence':r.get('collision',{}).get('display') or r.get('bound'),
          'scope':r.get('qualification') or r.get('domain') or r.get('key_model'),
          'source':r['speeds'][host['key']].get('url','records/speeds.json'),
          'family':r.get('mechanism_family',''),'key_model':r.get('key_model') or r.get('domain') or r.get('domain_short',''),
          'profile':profile['ids'][0], 'summary':profile['results'][r['id']],
          'reader_notes':profile['reader_notes'][r['id']],
          'output_bits':r['output_bits'], 'code_url':r['code_url']})
(OUT/'data.json').write_text(json.dumps(manifest,ensure_ascii=False,indent=2)+'\n')
(ROOT/'feature.svg').write_bytes((OUT/'m2-sqrt.svg').read_bytes())
(ROOT/'feature.png').write_bytes((OUT/'m2-sqrt.png').read_bytes())
print('Generated 24 SVGs, 24 PNGs, inspection data, and feature.svg/png from data.json.')

# The existing figure note distinguishes the historical comparison from the
# currently selected pair. Keep it generated from the same scientific record.
selected = next(row for row in DATA['heuristics'] if row['id'] == 'xxh3-64')
context_note = (
    '<p>The separate historical <a href="verify/xxh3-64/HISTORICAL.html">32-byte pair A</a> '
    'gave 9, 12, 11 and 11 collisions per 2<sup>30</sup> keys in wyhash, rapidhash v1, '
    'rapidhash v3 and XXH3-64. The selected XXH3-64 pair has a different measured rate: '
    + html.escape(selected['collision']['display']) + '. Search effort was unequal; '
    'these witness caps do not rank hashes. <a href="'
    + html.escape(selected['pair']['provenance'][-1], quote=True)
    + '">Current pair and count provenance</a>.</p>'
)
article = ROOT / 'index.html'
page, count = re.subn(
    r'<p>The (?:separate historical <a href="verify/xxh3-64/[^"\n]+">32-byte pair A</a>|historical 32-byte pair A).*?</p>',
    lambda match: context_note, article.read_text(), count=1, flags=re.S)
if count != 1:
    raise ValueError('Missing existing figure witness-context note')
# Provenance lives in the canonical data; update the existing caption link line.
reproduction_url = DATA['benchmark']['reproduction_url']
page = re.sub(r'(<a href="data.json">Data and provenance</a>)(?: · <a href="https://github.com/thomasahle/hash-benchmark-reproduction">Timing reproduction</a>)?',
              lambda match: match.group(1) + ' · <a href="' + html.escape(reproduction_url, quote=True) + '">Timing reproduction</a>', page, count=1)
article.write_text(page)
