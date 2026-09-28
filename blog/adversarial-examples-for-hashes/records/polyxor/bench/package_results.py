"""Assemble speeds_polyxor.json from evidence/{Xeon,M2} (layout of records/chainhash/speeds_chainhash128.json).

Selection (post convention, records/README): Xeon, two passes: higher fixed 262144-byte
bulk B/cycle (ties: higher GiB/s, then earlier pass) and, independently, lower 1-31-byte
average (ties: earlier pass). M2, three passes: medians of bulk and of small,
independently; runs more than 15% from the median are flagged, never dropped.
"""
import hashlib, json, pathlib, re, statistics
root=pathlib.Path(__file__).resolve().parent.parent
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
HOSTS={'Xeon8375C':dict(folder='Xeon',passes=2,aggregation='higher bulk of two passes; independently lower small average',clock='3.5 GHz printed GiB/s; TSC ticks'),
       'M2Pro':dict(folder='M2',passes=3,aggregation='median of three; >15% deviations flagged, not discarded',clock='3.5 GHz printed GiB/s; calibrated monotonic estimate')}
ORDER=['polyxor-128','polyxor-128.raw','XXH3-128','UMASH-128','rapidhash','komihash']
BULK=re.compile(r'(Bulk speed test - [^\n]+)\n(.*?)(?:\n\n|\Z)',re.S)
AVG=re.compile(r'Average\s+-\s+([\d.]+) bytes/cycle\s+-\s+([\d.]+) GiB/sec @ ([\d.]+) ghz')
SMALL=re.compile(r'(Small key speed test - [^\n]+)\n.*?Average\s+-\s+([\d.]+) cycles/hash',re.S)
def ld(l):return {'load1':l[0],'load5':l[1],'load15':l[2]}
def parse(host,meta,folder):
    raw=folder/meta['raw_file'];text=raw.read_text();assert sha(raw)==meta['raw_sha256'],raw
    secs=[]
    for m in BULK.finditer(text):
        a=AVG.search(m[2]);assert a,(raw,m[1])
        secs.append({'header':m[1],'bulk_bytes_per_cycle':float(a[1]),'bulk_gib_s':float(a[2]),'clock_assumption':f'{a[3]} ghz','clock_ghz':float(a[3]),'average_line':a[0]})
    fixed=[s for s in secs if s['header']=='Bulk speed test - 262144-byte keys'];assert len(fixed)==1,raw
    sm=SMALL.search(text);assert sm and sm[1]=='Small key speed test - [1, 31]-byte keys',raw
    b,af=meta['before'],meta['after']
    return {'host':host,'name':meta['name'],'test':'Speed','run':meta['run'],'command':meta['command'],
            'raw_file':str(raw.relative_to(root)),'raw_sha256':meta['raw_sha256'],'binary_sha256':meta['binary_sha256'],
            'started':b['utc'],'load_before':{'uptime':b['uptime'],**ld(b['load'])},'cpu_busy_percent_before':b.get('cpu_busy_percent'),
            'finished':af['utc'],'load_after':{'uptime':af['uptime'],**ld(af['load'])},'cpu_busy_percent_after':af.get('cpu_busy_percent'),
            'elapsed_seconds':meta['elapsed_seconds'],'returncode':meta['returncode'],
            'gate_passed_at_launch':b['passed'],'smhasher_pids_at_launch':b['smhasher_pids'],
            'bulk_bytes_per_cycle':fixed[0]['bulk_bytes_per_cycle'],'bulk_gib_s':fixed[0]['bulk_gib_s'],'small_cycles':float(sm[2]),
            'bulk_sections':secs,'small_header':sm[1],'clock_assumption':HOSTS[host]['clock'],'clock_ghz':fixed[0]['clock_ghz'],
            'sha256_verified':True,'valid':meta['returncode']==0}
def summarize(host,runs):
    P=HOSTS[host];bulks=[r['bulk_bytes_per_cycle'] for r in runs];smalls=[r['small_cycles'] for r in runs]
    mb,ms=statistics.median(bulks),statistics.median(smalls)
    for r in runs:
        r['deviation_from_median_percent']=100*(r['bulk_bytes_per_cycle']/mb-1);r['outlier']=abs(r['deviation_from_median_percent'])>15
        r['small_deviation_from_median_percent']=100*(r['small_cycles']/ms-1);r['small_outlier']=abs(r['small_deviation_from_median_percent'])>15
    if host=='M2Pro':
        srt=sorted(runs,key=lambda r:(r['bulk_bytes_per_cycle'],r['run']));brun=srt[len(srt)//2]
        srt=sorted(runs,key=lambda r:(r['small_cycles'],r['run']));srun=srt[len(srt)//2]
    else:
        brun=sorted(runs,key=lambda r:(-r['bulk_bytes_per_cycle'],-r['bulk_gib_s'],r['run']))[0]
        srun=sorted(runs,key=lambda r:(r['small_cycles'],r['run']))[0]
    return {'status':'complete' if len(runs)==P['passes'] else f"partial: {len(runs)} of {P['passes']} passes",
            'bulk_bytes_per_cycle':brun['bulk_bytes_per_cycle'],'bulk_gib_s':brun['bulk_gib_s'],'small_cycles':srun['small_cycles'],
            'bulk_selected_run':brun['run'],'small_selected_run':srun['run'],
            'bulk_run_spread_percent':100*(max(bulks)/min(bulks)-1),'small_run_spread_percent':100*(max(smalls)/min(smalls)-1),
            'aggregation':P['aggregation'],'flagged_runs':sorted({r['run'] for r in runs if r['outlier'] or r['small_outlier']}),'runs':runs}
out={}
for host,P in HOSTS.items():
    folder=root/'evidence'/P['folder'];prov=json.loads((folder/'provenance.json').read_text())
    for name in ORDER:
        metas=sorted(folder.glob(f'{name}.run*.json'),key=lambda p:int(re.search(r'\.run(\d+)\.json$',p.name)[1]))
        runs=[parse(host,json.loads(m.read_text()),folder) for m in metas]
        for r in runs:assert r['binary_sha256']==prov['binary_sha256']
        runs=[r for r in runs if r['valid']]
        if not runs:continue
        e=summarize(host,runs)
        sf=folder/f'{name}.sanity.txt'
        if sf.exists():e['verification']={'raw_file':str(sf.relative_to(root)),'raw_sha256':sha(sf),'lines':[l.strip() for l in sf.read_text().splitlines() if l.startswith('Verification value') or l.startswith('Running ')]}
        out.setdefault(name,{})[host]=e
x=json.loads((root/'evidence/Xeon/provenance.json').read_text());m=json.loads((root/'evidence/M2/provenance.json').read_text())
out['meta']={
 'definition':'PolyXOR128 (Orson Peters), crate polyxor 0.1.0 (commit 3123eb6, zlib license), https://github.com/orlp/polyxor; 128-bit output',
 'registrations':{'polyxor-128':'hasher(); update(bytes); finalize_avalanche() (the crate\'s documented default output, a bijection of finalize_raw), 16 little-endian bytes; the table row',
                  'polyxor-128.raw':'hasher(); update(bytes); finalize_raw(), 16 little-endian bytes',
                  'XXH3-128':'same-binary control (base build registration)','UMASH-128':'same-binary control (base build registration)','rapidhash':'same-binary control (base build registration)','komihash':'same-binary control (base build registration)'},
 'key_model':'PolyXor128::from_entropy of 4160 uniformly random bytes; the harness expands the 64-bit SMHasher3 seed with SplitMix64 into those 4160 bytes in seedfn, outside the timed calls (as for ChainHash-128); key setup is not timed',
 'verification_values':{'polyxor-128':'0xA9574CA8','polyxor-128.raw':'0x23A6E8BB','note':'SMHasher3 CE verification codes of this registration (identical on x86-64 and arm64); computed by SMHasher3 with the value set to 0, then fixed; Sanity PASS on both hosts'},
 'backend':{'Xeon8375C':x['backend_probe'],'M2Pro':m['backend_probe'],'note':'with RUSTFLAGS=-C target-cpu=native the crate selects its backend at compile time: hash_blocks_avx512 (AVX-512F/VL + VPCLMULQDQ) on the Xeon, neon::hash_blocks (NEON + AES/PMULL) on the M2'},
 'affinity':{'Xeon8375C':'nice -n 10 taskset -c 16-23 (the post\'s range; per-CPU busy samples of 16-23 in each run record)','M2Pro':'nice -n 10, no affinity'},
 'load_gate':{'M2Pro':{'no_process':'SMHasher3','load1_less_than':4.5,'poll_seconds':60},'Xeon8375C':{'no_process':'SMHasher3','load1_less_than':80.0,'poll_seconds':60,'note':'shared host (~40/96 load from other users\' unpinned jobs); one timing job at a time'}},
 'protocol':{'command':'SMHasher3 NAME --test=Speed (unmodified harness, no overrides)','bulk':'Average of alignments 0..7 at fixed 262144 bytes (SMHasher3 Average line)','small':'Average of the 31 separately measured 1..31-byte costs',
             'xeon':'two complete passes; higher fixed-size bulk (ties: higher GiB/s, then earlier) and lower small average (ties: earlier) selected independently',
             'm2':'three complete passes; median of bulk and median of small, independently; any run deviating more than 15 percent from its median is flagged, never dropped',
             'key_setup_timed':False,'completed_runs_excluded':0},
 'builds':{'Xeon8375C':{**x,'base':'the post\'s Xeon SMHasher3 Release build (build-release-20260917, binary sha256 03c2ff9f38cc564400eeb1906da90cb74ea6efcff2bea6c660626fdbe1571a99); main.cpp.o and both libraries are the same objects the ChainHash-128 lane reused','evidence_folder':'evidence/Xeon','provenance_file':'evidence/Xeon/provenance.json'},
           'M2Pro':{**m,'base':'the public timing reproduction\'s arm64 build (pin 3de870c7 + patch series incl. the arm64 family.cmake fix; binary sha256 52c54407205727175eb0034f030a7956f187f99dd98131fe9dae45f237947391). The post\'s M2 base (m2-rerun/build-fixed, binary fb2f75fb...) no longer exists on disk; controls here are same-binary, so polyxor is compared against them rather than against the chart\'s older M2 cells','evidence_folder':'evidence/M2','provenance_file':'evidence/M2/provenance.json'}},
 'units':{'Xeon8375C':'TSC ticks','M2Pro':'SMHasher3 calibrated cycles (per-process monotonic-clock estimate)','gib_s':'printed GiB/s assumes 3.5 GHz on both hosts; a conversion of B/cycle'},
}
(root/'speeds_polyxor.json').write_text(json.dumps(out,indent=1)+'\n')
for n in ORDER:
    if n in out:print(n,{h:(e['bulk_bytes_per_cycle'],e['small_cycles'],[(r['bulk_bytes_per_cycle'],r['small_cycles']) for r in e['runs']],e['flagged_runs']) for h,e in out[n].items()})
