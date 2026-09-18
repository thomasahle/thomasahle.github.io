"""Rebuild publication SVGs, PNGs, and inspection data from ../data.json.
Run: python3 blog/adversarial-examples-for-hashes/figure/build.py
Requires matplotlib. Does not alter measurements or the article.
"""
from pathlib import Path
import copy
import hashlib
import json
import math
import xml.etree.ElementTree as ET

import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
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
 'm2': dict(key='smh_m2_bulk_Bpc', host='M2Pro', name='APPLE M2 PRO', max_y=256, provisional=False),
 'xeon': dict(key='smh_xeon_bulk_Bpc', host='Xeon8375C', name='INTEL XEON 8375C', max_y=256, provisional=False),
}

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
            kind = 'claim' if row['bits_kind'] == 'claimed' else 'proof' if row['family'] == 'proven' else 'witness'
            row.update(speed=speed, kind=kind)
            rows.append(row)
    return rows

# Landmark labels use fixed data coordinates; every other point remains inspectable.
LABELS = {'m2': {'ghash': (3.1296782804570187, 222.86094420380775, 'left', 'GHASH', ''),
        'poly1305': (1.9238814757276592, 168.89701257893051, 'right', 'Poly1305', ''),
        'umash128': (8.28213798512708, 222.86094420380775, 'left', 'UMASH-128', ''),
        'chain128': (13.472985573602719, 128.0, 'left', 'ChainHash-128 (ours)', ''),
        'halftime24-fixed': (0.57, 147.0333894396205, 'left', 'HalftimeHash24 (fixed)', ''),
        'chain-v2': (45.47443397859695, 48.50293012833273, 'right', 'ChainHash v2 (ours)', ''),
        'polymur': (4.508070852474694, 21.112126572366314, 'left', 'PolymurHash', ''),
        'highway': (1.0471926475891462, 73.51669471981025, 'right', 'HighwayHash', ''),
        'rapid3': (19.40684253056874, 18.37917367995256, 'left', 'rapidhash v3', ''),
        'xxh3-64': (21.917207922939696, 9.18958683997628, 'left', 'XXH3-64', ''),
        'komi': (10.563386085541142, 8.0, 'left', 'komihash', '')},
 'xeon': {'ghash': (8.28213798512708, 256.0, 'left', 'GHASH', ''),
          'poly1305': (2.771209442996604, 194.0117205133309, 'right', 'Poly1305', ''),
          'umash128': (5.74978260458602, 21.112126572366314, 'right', 'UMASH-128', ''),
          'chain128': (6.493543741487783, 194.0117205133309, 'right', 'ChainHash-128 (ours)', ''),
          'halftime24-fixed': (11.929809233130745, 222.86094420380775, 'left', 'HalftimeHash24 (fixed)', ''),
          'chain-v2': (27.9541260508884, 147.0333894396205, 'left', 'ChainHash v2 (ours)', ''),
          'clhash': (24.752300760967383, 42.22425314473263, 'left', 'CLHASH', ''),
          'polymur': (4.508070852474694, 42.22425314473263, 'right', 'PolymurHash', ''),
          'highway': (2.4537991092912335, 36.75834735990512, 'right', 'HighwayHash', ''),
          'rapid3': (6.493543741487783, 18.37917367995256, 'left', 'rapidhash v3', ''),
          'xxh3-64': (24.752300760967383, 9.18958683997628, 'left', 'XXH3-64', ''),
          'komi': (10.563386085541142, 8.0, 'left', 'komihash', '')}}
MOBILE_LABELS = {'m2': {'ghash': (3.1296782804570187, 256.0, 'left', 'GHASH', ''),
        'poly1305': (0.57, 168.89701257893051, 'left', 'Poly1305', ''),
        'chain-v2': (45.47443397859695, 21.112126572366314, 'right', 'ChainHash v2 (ours)', ''),
        'chain128': (45.47443397859695, 147.0333894396205, 'right', 'ChainHash-128 (ours)', ''),
        'halftime24-fixed': (0.57, 42.22425314473263, 'left', 'HalftimeHash24 (fixed)', ''),
        'highway': (0.57, 84.44850628946526, 'left', 'HighwayHash', ''),
        'xxh3-64': (45.47443397859695, 10.556063286183154, 'right', 'XXH3-64', ''),
        'komi': (10.563386085541142, 6.062866266041593, 'left', 'komihash', '')},
 'xeon': {'ghash': (4.508070852474694, 256.0, 'left', 'GHASH', ''),
          'poly1305': (0.57, 222.86094420380775, 'left', 'Poly1305', ''),
          'chain-v2': (45.47443397859695, 21.112126572366314, 'right', 'ChainHash v2 (ours)', ''),
          'chain128': (51.35676363205364, 147.0333894396205, 'right', 'ChainHash-128 (ours)', ''),
          'halftime24-fixed': (0.57, 111.43047210190387, 'left', 'HalftimeHash24 (fixed)', ''),
          'highway': (0.57, 42.22425314473263, 'left', 'HighwayHash', ''),
          'xxh3-64': (45.47443397859695, 10.556063286183154, 'right', 'XXH3-64', ''),
          'komi': (10.563386085541142, 6.062866266041593, 'left', 'komihash', '')}}

def render(key, mobile=False, compact=False):
    host = HOSTS[key]
    rows = rows_for(host)
    width, height = (390, 845) if mobile else (760, 820) if compact else (1100, 820)
    fig = plt.figure(figsize=(width / 72, height / 72), dpi=144, facecolor=BG)
    def text(x, y, label, size=14, color=INK, weight='normal', **kw):
        return fig.text(x/width, 1-y/height, label, size=size, color=color, weight=weight,
                        va='top', **kw)
    if mobile:
        text(22, 22, 'FAST HASHES NEED PROOFS.', 11, BLUE, 'bold')
        text(22, 49, 'Speed and collision bounds', 21, INK, 'bold')
        text(22, 80, host['name'] + ' · BULK', 11, MUTED, 'bold').set_gid('host-name')
        text(22, 102, 'Provisional timings' if host['provisional'] else 'Log axes · score linear from 0 to 1', 11, MUTED).set_gid('host-note')
        top, bottom, left, right = 265, 658, 49, 372
        legends = [(22,143,BLUE,'●','Proved lower bound'),(22,169,ORANGE,'◆','Witness upper bound'),(22,195,CLAIM,'○','Unresolved claim')]
    else:
        text(40, 26, 'ADVERSARIAL EXAMPLES FOR FAST HASH FUNCTIONS', 11, MUTED, 'bold')
        text(40, 54, 'Fast hashes need proofs.', 30 if compact else 36, INK, 'bold')
        text(40, 104, 'A passing test suite cannot certify a collision bound for every fixed pair.', 12 if compact else 16, MUTED)
        text(width-40, 33, host['name'] + ' · BULK', 11, MUTED, 'bold', ha='right').set_gid('host-name')
        text(width-40, 54, 'Provisional timings' if host['provisional'] else 'Bulk · B/cycle', 11, MUTED, ha='right').set_gid('host-note')
        top, bottom, left, right = 218, 635, 68 if compact else 83, width-40
        legends = [(40,154,BLUE,'●','Proved lower bound  ↑'), (278 if compact else 395,154,ORANGE,'◆','Witness upper bound  ↓'), (528 if compact else 786,154,CLAIM,'○','Unresolved claim')]
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
    ax.set_yscale('symlog', base=2, linthresh=1, linscale=.5)
    ax.set_ylim(-.25, host['max_y'])
    ax.set_facecolor(BG)
    for spine in ax.spines.values(): spine.set_visible(False)
    ax.set_axisbelow(True)
    ax.xaxis.set_major_locator(FixedLocator([.5,1,2,5,10,20,50]))
    ax.xaxis.set_major_formatter(FuncFormatter(lambda n,_: f'{n:g}'))
    ax.xaxis.set_minor_locator(FixedLocator([]))
    ax.yaxis.set_major_locator(FixedLocator([0,1,2,4,8,16,32,64,128]))
    ax.yaxis.set_major_formatter(FuncFormatter(lambda n,_: f'{n:g}'))
    ax.yaxis.set_minor_locator(FixedLocator([]))
    ax.tick_params(axis='both', length=0, labelsize=11 if mobile else 12, colors=MUTED, pad=9)
    ax.grid(axis='y',color=GRID,lw=.7)
    # Mark the linear segment explicitly; every-seed pairs can also score > 0.
    ax.axhspan(-.25,1,color='#fcf2ed',zorder=0)
    ax.set_xlabel('Bulk speed · B/cycle · log scale  →',fontsize=12 if mobile else 14,labelpad=15,color=INK)
    text(left, top-28, 'Score · bits · log scale' if mobile else 'Collision score · bits · log scale (linear 0–1)', 11 if mobile else 13, MUTED)
    point_positions = {}
    annotations = []
    for row in rows:
        x,y=row['speed'],row['bits']
        if not (.5 <= x <= 64 and 0 <= y <= host['max_y']):
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
    labels=MOBILE_LABELS[key] if mobile or compact else LABELS[key]
    row_by_id={r['id']:r for r in rows}
    for id,(tx,ty,align,name,number) in labels.items():
        if id not in row_by_id: continue
        row=row_by_id[id]
        # Derive label values from the record, so data updates cannot leave stale text.
        if row['kind'] == 'proof':
            number = f"≥ {math.floor(row['bits']*100)/100:g}" + ('' if mobile else ' bits')
            if id in ('chain-v2', 'chain128'): number += ' · model A'
        elif row['kind'] == 'claim':
            number = f"{row['bits']:g}" + ('' if mobile else ' bits') + ' · claim'
        elif row['bits_kind'] == 'measured':
            number = f"≈ {row['bits']:.2f}" + ('*' if mobile else '-bit cap*')
        else:
            number = f"≤ {math.ceil(row['bits']*100)/100:g}" + ('' if mobile else ' bits')
        c=BLUE if row['kind']=='proof' else CLAIM if row['kind']=='claim' else ORANGE
        # Deliberate label positions, with leaders terminating at the true data.
        annotation=ax.annotate(name+'\n'+number,xy=(row['speed'],row['bits']),xytext=(tx,ty),
            fontsize=12 if mobile else 14,weight='bold' if id in ('chain-v2','chain128') else 'normal',
            color=c,ha=align,va='top',linespacing=1.5,
            bbox=dict(boxstyle='square,pad=.2',fc='white',ec='none',alpha=.96),
            arrowprops=dict(arrowstyle='-',color=c,alpha=.5,lw=.8,shrinkA=5,shrinkB=8),zorder=5)
        annotation.set_gid('label-'+id)
        annotation.arrow_patch.set_gid('leader-'+id)
        annotations.append((annotation,{id}))
    if not (mobile or compact):
        half=[r for r in rows if r['id'].startswith('halftimehash-')]
        center=math.exp(sum(math.log(r['speed']) for r in half)/len(half))
        tx,ty=(8.8,40) if key=='m2' else (14.5,17)
        representative = min(half,key=lambda r:abs(math.log(r['speed']/center)))
        a=ax.annotate('HalftimeHash\n4 styles · ≥ 63 bits',xy=(representative['speed'],representative['bits']),xytext=(tx,ty),
            ha='center',va='top',fontsize=12,color=BLUE,linespacing=1.4,
            bbox=dict(fc='white',ec='none',pad=3),arrowprops=dict(arrowstyle='-',color=BLUE,lw=.8,shrinkB=8),zorder=3)
        a.set_gid('label-'+representative['id'])
        a.arrow_patch.set_gid('leader-halftimehash')
        annotations.append((a,{r['id'] for r in half}))
    fig.canvas.draw()
    label_boxes = [(annotation.get_bbox_patch().get_window_extent(), ids) for annotation,ids in annotations]
    for box, ids in label_boxes:
        for row in rows:
            if row['id'] not in ids and box.contains(*ax.transData.transform((row['speed'],row['bits']))):
                raise ValueError(f"{key}{'-mobile' if mobile else ''}: label {ids} obscures {row['id']}")
    for i,(box,ids) in enumerate(label_boxes):
        for other,other_ids in label_boxes[i+1:]:
            if box.overlaps(other):
                raise ValueError(f"{key}{'-mobile' if mobile else ''}: labels {ids} and {other_ids} overlap")
    key_free=sum(bool(r.get('key_free')) for r in rows)
    if mobile:
        text(22,718,f'×  {key_free} variants have an every-seed pair.',12,ORANGE,'bold')
        text(22,746,'Bounds require their stated key models.\nGHASH timing uses an OpenSSL GMAC proxy.\n* Sampled cap; upper caps are not a ranking.',10.5,MUTED,linespacing=1.6)
        text(22,818,'Thomas Dybdahl Ahle · thomasahle.com',10,MUTED)
    else:
        text(40,711,f'×  {key_free} variants have a fixed pair that collides for every seed.',14,ORANGE,'bold')
        text(40,744,'Bounds require their stated key models. GHASH timing uses an OpenSSL GMAC proxy.',12,MUTED)
        text(40,765,'* Sampled upper caps describe discovered witnesses, not a ranking. See each family’s domain.',12,MUTED)
        text(width-40,794,'Thomas Dybdahl Ahle · thomasahle.com',10,MUTED,ha='right')
    suffix='-mobile' if mobile else '-compact' if compact else ''
    destination=OUT/f'{key}{suffix}.svg'
    fig.savefig(destination,format='svg',facecolor=BG,metadata={'Date':None,'Creator':'Thomas Dybdahl Ahle; generated from data.json'})
    fig.savefig(OUT/f'{key}{suffix}.png',dpi=144,facecolor=BG)
    plt.close(fig)
    # Keep semantic, generously sized targets in the exact exported SVG.
    ns='http://www.w3.org/2000/svg'; ET.register_namespace('',ns); ET.register_namespace('xlink','http://www.w3.org/1999/xlink')
    tree=ET.parse(destination); svg=tree.getroot()
    svg.set('role','img'); svg.set('aria-label','Fast hashes need proofs: collision bounds versus bulk speed on '+host['name']+'. Logarithmic axes; score is linear from zero to one bit.')
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
    for row_id in profile['ids']:
        if row_id in profile_for:
            raise ValueError(f'Duplicate hash profile: {row_id}')
        if not profile['results'].get(row_id):
            raise ValueError(f'Missing result explanation: {row_id}')
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
          'output_bits':r['output_bits'], 'code_url':r['code_url']})
(OUT/'data.json').write_text(json.dumps(manifest,ensure_ascii=False,indent=2)+'\n')
(ROOT/'feature.svg').write_bytes((OUT/'m2.svg').read_bytes())
(ROOT/'feature.png').write_bytes((OUT/'m2.png').read_bytes())
print('Generated 6 SVGs, 6 PNGs, inspection data, and feature.svg/png from data.json.')
