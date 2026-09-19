#!/usr/bin/env python3
import hashlib,json,pathlib,statistics
root=pathlib.Path(__file__).resolve().parent.parent
d=json.loads((root/'speeds_museair_v2.json').read_text())
assert set(d)=={'MuseAir-v2','MuseAir','XXH3-64','rapidhash','chainhash-256'}
total=0
for host,n in [('Xeon8375C',2),('M2Pro',3)]:
 hashes=set()
 for name,hosts in d.items():
  e=hosts[host];assert e['status']=='complete';assert len(e['runs'])==n
  assert e['verification']['status']=='PASS';hashes.add(e['binary_sha256'])
  for r in e['runs']:
   assert r['valid'] and r['returncode']==0 and not r['overlaps']
   assert not r['before']['smhasher_processes']
   assert host!='M2Pro' or r['before']['load'][0]<4.5
   assert hashlib.sha256((root/r['raw_file']).read_bytes()).hexdigest()==r['raw_sha256']
   assert r['command']==[e['binary'],name,'--test=Speed']
   total+=1
  for key,fn in [('bulk_bytes_per_cycle',max),('small_cycles',min)]:
   values=[r[key] for r in e['runs']]
   assert e[key]==(statistics.median(values) if n==3 else fn(values))
  sane=e['verification'];assert hashlib.sha256((root/sane['sanity_raw_file']).read_bytes()).hexdigest()==sane['sanity_raw_sha256']
 assert len(hashes)==1
 x=json.loads((root/'evidence'/host/'execution.json').read_text())
 assert x['binary_sha256']==x['binary_sha256_after']==next(iter(hashes))
 assert len(x['runs'])==n*5
 manifest=json.loads((root/'evidence'/host/'build-sha256.json').read_text())
 assert manifest[x['binary']]==x['binary_sha256']
 for suffix,local in [('hashes/museair2.h','muse-v2/museair2.h')]:
  value=next(v for k,v in manifest.items() if k.endswith(suffix))
  assert value==hashlib.sha256((root/local).read_bytes()).hexdigest()
p=d['MuseAir-v2']['Xeon8375C']['registered_pair_measurement']
assert p['poisson95_log2'][0]<-17.45<p['poisson95_log2'][1]
assert total==25
print('PASS: 25 runs, 10 Sanity passes, per-host single binaries, raw hashes, gates, selections and registered-pair rate checked')
