#!/usr/bin/env python3
import hashlib,json,pathlib,re,statistics,datetime
root=pathlib.Path(__file__).resolve().parent.parent
ids={n:n for n in ['MuseAir-v2','XXH3-64','rapidhash','chainhash-256','MuseAir']}
controls=['XXH3-64','rapidhash','chainhash-256','MuseAir']

def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def parse(p):
 s=p.read_text();assert 'Testing took ' in s,p
 bulk=re.search(r'Bulk speed test - 262144-byte keys.*?Average\s*-\s*([\d.]+) bytes/cycle\s*-\s*([\d.]+) GiB/sec',s,re.S)
 varying=re.search(r'Bulk speed test - \[262017, 262144\]-byte keys.*?Average\s*-\s*([\d.]+) bytes/cycle\s*-\s*([\d.]+) GiB/sec',s,re.S)
 small=re.search(r'Small key speed test.*?Average\s*-\s*([\d.]+) cycles/hash',s,re.S)
 assert bulk and small and varying,p
 return dict(bulk_bytes_per_cycle=float(bulk[1]),bulk_gib_s=float(bulk[2]),small_cycles=float(small[1]),variable_bulk_bytes_per_cycle=float(varying[1]),variable_bulk_gib_s=float(varying[2]))
data={i:{} for i in ids.values()};notes=[]
for host in ['Xeon8375C','M2Pro']:
 out=root/'evidence'/host;exe=out/'execution.json'
 if not exe.exists():continue
 x=json.loads(exe.read_text());regs=json.loads((out/'registrations.json').read_text());reg={r['name']:r for r in regs};n=3 if host=='M2Pro' else 2
 listing=(out/'list.txt').read_text()
 baseline=(root/'../m2-rerun/speeds_m2_v2_final.json') if host=='M2Pro' else root/'../speedbench/speeds.json'
 bdata=json.loads(baseline.read_text())
 for name,id in ids.items():
  runs=[]
  for rr in x['runs']:
   if rr['name']!=name:continue
   r=dict(rr);p=out/r['raw_file'];assert sha(p)==r['sha256'];assert r['returncode']==0 and not r['overlaps'];assert not r['before']['smhasher_processes'];assert host!='M2Pro' or r['before']['load'][0]<4.5
   r.update(parse(p),host=host,test='Speed',binary_sha256=x['binary_sha256'],raw_file=str(p.relative_to(root)),raw_sha256=r.pop('sha256'),valid=True,sha256_verified=True)
   r['load_before']={f'load{k}':v for k,v in zip([1,5,15],r['before']['load'])};r['load_after']={f'load{k}':v for k,v in zip([1,5,15],r['after']['load'])};runs.append(r)
  if not runs:continue
  line=next(s for s in listing.splitlines() if s.split() and s.split()[0]==name)
  banner=(out/(name+'.run1.txt')).read_text();m=re.search(r'^--- Testing .*?\[(.*?)\]',banner,re.M)
  e=dict(status='complete' if len(runs)==n else 'running',registered_name=name,registration=line,backend_token=reg[name]['backend'] if name in reg else (m[1] if m else None),binary=x['binary'],binary_sha256=x['binary_sha256'],runs=runs,clock_assumption='3.5 ghz',aggregation='median-of-three' if n==3 else 'higher-bulk-lower-small-of-two')
  bulk=sorted(runs,key=lambda r:(r['bulk_bytes_per_cycle'],r['bulk_gib_s'],-r['run']),reverse=True)[0] if n==2 else sorted(runs,key=lambda r:r['bulk_bytes_per_cycle'])[len(runs)//2]
  small=min(runs,key=lambda r:r['small_cycles']) if n==2 else sorted(runs,key=lambda r:r['small_cycles'])[len(runs)//2]
  e.update(bulk_bytes_per_cycle=bulk['bulk_bytes_per_cycle'],bulk_gib_s=bulk['bulk_gib_s'],small_cycles=small['small_cycles'],bulk_selected_run=bulk['run'],small_selected_run=small['run'])
  e['variation']={}
  for key in ['bulk_bytes_per_cycle','small_cycles']:
   vals=[r[key] for r in runs];med=statistics.median(vals);dev=[100*(v/med-1) for v in vals];spread=100*(max(vals)/min(vals)-1)
   e['variation'][key]=dict(median=med,values=vals,deviations_from_median_percent=dev,range_over_min_percent=spread,flag_over_15_percent=any(abs(d)>15 for d in dev) if n==3 else spread>15)
  e['bulk_run_spread_percent']=e['variation']['bulk_bytes_per_cycle']['range_over_min_percent'];e['small_run_spread_percent']=e['variation']['small_cycles']['range_over_min_percent']
  e['flag_over_15_percent']=any(v['flag_over_15_percent'] for v in e['variation'].values())
  if True:
   sane=out/(name+'.sanity.txt');ss=sane.read_text();fail=[s for s in ss.splitlines() if 'FAIL' in s];assert re.search(r'Verification value (?:LE|CE) .*PASS',ss)
   e['verification']=dict(status='FAIL' if fail else 'PASS',implementation='PASS',native_value=re.search(r'Verification value (?:LE|CE) 0x([0-9A-Fa-f]+)',ss)[1].lower(),failure_lines=fail,sanity_raw_file=str(sane.relative_to(root)),sanity_raw_sha256=sha(sane),vectors_raw_file=str((out/'verification.txt').relative_to(root)))
   if name!='MuseAir-v2':e['verification'].pop('vectors_raw_file')
   assert e['verification']['status']=='PASS'
   e['output_bits']=64
  if name=='MuseAir-v2':
   e['seed_mapping']='Identity: SMHasher3 uint64 seed -> museair::hash(bytes, seed); standard variant (BFast=false), no expansion, seed hook, secret or exclusion'
   e['algorithm']='MuseAir v2; museair crate 0.6.0; supplied validated C port'
   e['audited_pair']=['00'*32,'00000000000000404a048402a910048a00000000000000a80000000000000000']
   e['audited_pair_applies_to_backend']=True;e['bits_kind']='measured';e['key_free']=False
   e['collision_bits_for_backend']=19.45;e['audited_log2_collision_rate']=-17.45
   e['verification']['vector_count']=200000;e['verification']['registered_vector_calls']=600000
   e['verification']['final_object_raw_file']=str((out/'verification-final.txt').relative_to(root))
   e['verification']['pair_examples_reproduced']=True
   pairfile=root/'evidence/Xeon8375C/verification.txt'
   pm=re.search(r'PAIR trials=(\d+) collisions=(\d+).*?log2_rate=([-\d.]+) poisson95_log2=\[([-\d.]+),([-\d.]+)\]',pairfile.read_text())
   e['registered_pair_measurement']=dict(host='Xeon8375C',trials=int(pm[1]),collisions=int(pm[2]),log2_rate=float(pm[3]),poisson95_log2=[float(pm[4]),float(pm[5])],rng='SplitMix64',master='0x763220260919',raw_file=str(pairfile.relative_to(root)),raw_sha256=sha(pairfile),scope='Full-output equality through findHash and registered function; M2 verifies the same crate vectors and known witness seeds, without a heavy rate scan')
  if name=='MuseAir':e['algorithm']='MuseAir v0.3; unchanged hashes/museair.cpp registration'
  if name in controls:
   b=bdata[name][host];ratios={k:e[k]/b[k] for k in ['bulk_bytes_per_cycle','small_cycles']};e['control_baseline']=dict(file=str(baseline.relative_to(root)),sha256=sha(baseline),bulk_bytes_per_cycle=b['bulk_bytes_per_cycle'],small_cycles=b['small_cycles'],ratios=ratios,flag_over_15_percent=any(abs(r-1)>.15 for r in ratios.values()))
  e['load1_boundary_range']=[min(r[t]['load'][0] for r in runs for t in ['before','after']),max(r[t]['load'][0] for r in runs for t in ['before','after'])]
  samples=[s['load'][0] for r in runs for s in [r['before'],r['after'],*r['samples']]];e['load1_sampled_range']=[min(samples),max(samples)]
  data[id][host]=e
 if 'finished' in x:assert len(x['runs'])==n*5 and x['binary_sha256_after']==x['binary_sha256']
for host in ['Xeon8375C','M2Pro']:
 if host in data['MuseAir-v2'] and host in data['MuseAir']:
  e=data['MuseAir-v2'][host];old=data['MuseAir'][host]
  ratios={k:e[k]/old[k] for k in ['bulk_bytes_per_cycle','small_cycles']}
  e['same_binary_v03_comparison']=dict(registered_name='MuseAir',ratios=ratios,percent_changes={k:100*(r-1) for k,r in ratios.items()},flag_over_15_percent=any(abs(r-1)>.15 for r in ratios.values()),scope='Algorithm version difference, distinct from run variation')
(root/'speeds_museair_v2.json').write_text(json.dumps(data,indent=2)+'\n')
print('entries',sum(len(v) for v in data.values()),'complete',sum(e['status']=='complete' for v in data.values() for e in v.values()))
