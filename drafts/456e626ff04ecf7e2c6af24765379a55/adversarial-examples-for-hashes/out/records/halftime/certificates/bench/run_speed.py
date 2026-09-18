#!/usr/bin/env python3
import argparse,datetime,hashlib,json,os,pathlib,re,subprocess,time
ap=argparse.ArgumentParser();ap.add_argument('host');ap.add_argument('binary');ap.add_argument('out');ap.add_argument('--gate',action='store_true');a=ap.parse_args()
out=pathlib.Path(a.out);out.mkdir(parents=True,exist_ok=True);binary=str(pathlib.Path(a.binary).resolve())
def now():return datetime.datetime.now(datetime.timezone.utc).isoformat()
def state():
 ps=subprocess.check_output(['ps','-axo','pid,comm'],text=True)
 return {'time':now(),'load':list(os.getloadavg()),'smhasher_processes':[s.strip() for s in ps.splitlines() if s.strip().split() and s.strip().split()[-1].rsplit('/',1)[-1]=='SMHasher3']}
def gate():
 while True:
  s=state()
  with (out/'gate.jsonl').open('a') as f:f.write(json.dumps(s)+'\n')
  if not s['smhasher_processes'] and (not a.gate or s['load'][0]<4.5):return s
  time.sleep(15)
def save(x):(out/'execution.json').write_text(json.dumps(x,indent=2)+'\n')
summary={'host':a.host,'binary':binary,'binary_sha256':hashlib.sha256(pathlib.Path(binary).read_bytes()).hexdigest(),'runs':[],'aggregation':'median of three runs, independently for fixed 262144-byte bulk and average 1..31-byte small'}
(out/'list.txt').write_text(subprocess.check_output([binary,'--list'],text=True,stderr=subprocess.STDOUT))
for rep in range(1,4):
 for name in ['komihash','rapidhash','HalftimeHash-512','HalftimeHash-512-fixed','HalftimeHash24-fixed']:
  before=gate();raw=out/(name+f'.run{rep}.txt');cmd=[binary,name,'--test=Speed']
  row={'name':name,'run':rep,'command':cmd,'started':now(),'before':before,'raw_file':raw.name}
  print(now(),name,rep,'start',flush=True)
  start=time.monotonic()
  with raw.open('w') as f:p=subprocess.run(cmd,stdout=f,stderr=subprocess.STDOUT)
  row.update(returncode=p.returncode,elapsed_seconds=time.monotonic()-start,finished=now(),after=state(),sha256=hashlib.sha256(raw.read_bytes()).hexdigest())
  summary['runs'].append(row);save(summary)
  print(now(),name,rep,'done',p.returncode,flush=True)
  if p.returncode:raise SystemExit(p.returncode)
summary['binary_sha256_after']=hashlib.sha256(pathlib.Path(binary).read_bytes()).hexdigest();summary['finished']=now();save(summary)
