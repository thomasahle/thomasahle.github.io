#!/usr/bin/env python3
import pathlib,json,re,hashlib,statistics,math
root=pathlib.Path(__file__).resolve().parents[2]
controls=['komihash','rapidhash','HalftimeHash-512'];fixed=['HalftimeHash24-fixed','HalftimeHash-512-fixed']
result={'protocol':{'command':'SMHasher3 NAME --test=Speed','bulk':'Average for fixed 262144-byte keys','small':'Average for 1..31-byte keys','selection':'independent medians of three complete runs on each host','variation_flags':'max/min - 1 > 0.15, and separately any absolute relative deviation from median > 0.15','M2_gate':'no SMHasher3 process and 1-minute load <4.5 before every run','timing_priority':'default; no affinity changes','bulk_ratio':'fixed bytes/cycle divided by control bytes/cycle; above 1 is faster','small_ratio':'fixed cycles/hash divided by control cycles/hash; above 1 is slower','cycle_caveat':'Xeon RDTSC/RDTSCP; M2 calibrated monotonic-clock cycle estimate. Printed GiB/s assumes 3.5 GHz, not measured frequency.'},'hosts':{},'flags':[]}
reference=json.loads((root/'certificates/bench/reference-controls.json').read_text())
for host in ['Xeon8375C','M2Pro']:
 folder=root/'certificates/bench'/host;execution=json.loads((folder/'execution.json').read_text())
 assert len(execution['runs'])==15 and execution.get('finished'),(host,'incomplete')
 assert execution['binary_sha256']==execution['binary_sha256_after']
 groups={n:[] for n in controls+fixed}
 for row in execution['runs']:
  assert row['returncode']==0 and not row['before']['smhasher_processes']
  if host=='M2Pro':assert row['before']['load'][0]<4.5
  raw=folder/row['raw_file'];assert hashlib.sha256(raw.read_bytes()).hexdigest()==row['sha256']
  s=raw.read_text();head=re.search(r'^--- Testing (\S+) "([^"]+)"(?: \[([^\]]+)\])?',s,re.M)
  assert head and head[1]==row['name']
  small=re.search(r'^Average\s+-\s+([0-9.]+) cycles/hash',s,re.M);assert small
  per_length={int(k):float(v) for k,v in re.findall(r'^\s*(\d+)-byte keys\s+-\s+([0-9.]+) cycles/hash',s,re.M)}
  assert set(per_length)==set(range(1,32))
  sections=re.split(r'^Bulk speed test - ',s,flags=re.M);assert len(sections)==3
  assert sections[1].startswith('262144-byte keys')
  bulk=[]
  for section in sections[1:]:
   m=re.search(r'^Average\s+-\s+([0-9.]+) bytes/cycle -\s+([0-9.]+) GiB/sec @ ([^\n]+)',section,re.M);assert m
   bulk.append({'header':section.splitlines()[0],'bytes_per_cycle':float(m[1]),'gib_per_second_assumed_clock':float(m[2]),'clock_label':m[3]})
  parsed=dict(row,backend_token=head[3] or '',description=head[2],small_cycles_per_hash=float(small[1]),small_by_length=per_length,bulk_bytes_per_cycle=bulk[0]['bytes_per_cycle'],bulk_sections=bulk,raw_file=str(raw.relative_to(root)))
  groups[row['name']].append(parsed)
 entries={}
 for name,runs in groups.items():
  assert sorted(r['run'] for r in runs)==[1,2,3]
  assert len({r['backend_token'] for r in runs})==1
  br=sorted(runs,key=lambda r:(r['bulk_bytes_per_cycle'],r['bulk_sections'][0]['gib_per_second_assumed_clock'],r['run']))[1]
  sr=sorted(runs,key=lambda r:(r['small_cycles_per_hash'],r['run']))[1]
  entry={'bits':192 if name=='HalftimeHash24-fixed' else 64,'backend_token':runs[0]['backend_token'],'bulk_bytes_per_cycle':br['bulk_bytes_per_cycle'],'bulk_median_run':br['run'],'small_cycles_per_hash':sr['small_cycles_per_hash'],'small_median_run':sr['run'],'runs':runs,'variability':{}}
  for metric in ['bulk_bytes_per_cycle','small_cycles_per_hash']:
   values=[r[metric] for r in runs];med=statistics.median(values);spread=max(values)/min(values)-1;dev=max(abs(v/med-1) for v in values)
   entry['variability'][metric]={'range_spread_fraction':spread,'max_deviation_from_median_fraction':dev,'range_exceeds_15pct':spread>0.15,'median_deviation_exceeds_15pct':dev>0.15}
   if spread>0.15 or dev>0.15:result['flags'].append({'host':host,'name':name,'metric':metric,**entry['variability'][metric]})
  if name in controls:
   prior=reference['values'][name][host]
   entry['reference_control_ratios']={'reference_bulk':prior['bulk_bytes_per_cycle'],'reference_small':prior['small_cycles'],'bulk_ratio':entry['bulk_bytes_per_cycle']/prior['bulk_bytes_per_cycle'],'small_ratio':entry['small_cycles_per_hash']/prior['small_cycles']}
   for metric in ['bulk_ratio','small_ratio']:
    ratio=entry['reference_control_ratios'][metric]
    if abs(ratio-1)>0.15:result['flags'].append({'host':host,'name':name,'kind':'historical_control_deviation','metric':metric,'ratio':ratio})
  entries[name]=entry
 for name in fixed:
  entries[name]['control_ratios']={c:{'bulk_ratio':entries[name]['bulk_bytes_per_cycle']/entries[c]['bulk_bytes_per_cycle'],'small_ratio':entries[name]['small_cycles_per_hash']/entries[c]['small_cycles_per_hash']} for c in controls}
 result['hosts'][host]={'binary':execution['binary'],'binary_sha256':execution['binary_sha256'],'started':execution['runs'][0]['started'],'finished':execution['finished'],'load1_before_range':[min(r['before']['load'][0] for r in execution['runs']),max(r['before']['load'][0] for r in execution['runs'])],'load1_after_range':[min(r['after']['load'][0] for r in execution['runs']),max(r['after']['load'][0] for r in execution['runs'])],'other_smhasher_processes_seen_after_runs':[r for r in execution['runs'] if r['after']['smhasher_processes']],'hashes':entries}
result['reference_controls']=reference
result['fixed_header_sha256']=hashlib.sha256((root/'halftime-hash.hpp').read_bytes()).hexdigest()
result['api_bindings']={'HalftimeHash24-fixed':'advanced::V4<3>; exactly 24 output bytes','HalftimeHash-512-fixed':'HalftimeHashStyle512; exactly 8 output bytes'}
result['ideal_key_theorem']={'scope':'Independent uniform key words, fixed messages, stack-safe domains in THEOREM.md; separate from the benchmark seeded family','core24_normalized_word_cap_bits':96,'core24_h16_collision_bound_bits':96-math.log2(6804),'core24_actual_max_height':7,'style_normalized_bits_exact':'64-log2(2-2^-64)','style_normalized_bits_conservative':63}
(root/'speeds_halftime_fixed.json').write_text(json.dumps(result,indent=2)+'\n')
for host,h in result['hosts'].items():
 for name,e in h['hashes'].items():print(host,name,e['backend_token'] or '(empty)',e['bulk_bytes_per_cycle'],e['small_cycles_per_hash'])
print('FLAGS',json.dumps(result['flags']))
