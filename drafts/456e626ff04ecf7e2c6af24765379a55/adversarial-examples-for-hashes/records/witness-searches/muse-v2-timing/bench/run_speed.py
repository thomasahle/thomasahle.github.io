#!/usr/bin/env python3
import argparse,datetime,hashlib,json,os,pathlib,subprocess,time
ap=argparse.ArgumentParser();ap.add_argument('host');ap.add_argument('binary');ap.add_argument('out');ap.add_argument('--passes',type=int,required=True);a=ap.parse_args()
out=pathlib.Path(a.out);out.mkdir(parents=True,exist_ok=True);binary=str(pathlib.Path(a.binary).resolve());mac=a.host=='M2Pro'
def sha(p):return hashlib.sha256(pathlib.Path(p).read_bytes()).hexdigest()
def now():return datetime.datetime.now(datetime.timezone.utc).isoformat()
def state():
 ps=subprocess.check_output(['ps','-axo','pid=,comm='],text=True)
 return {'time':now(),'load':list(os.getloadavg()),'smhasher_processes':[{'pid':int(s.strip().split(None,1)[0]),'command':s.strip().split(None,1)[1]} for s in ps.splitlines() if len(s.strip().split(None,1))==2 and pathlib.Path(s.strip().split(None,1)[1]).name.lower()=='smhasher3']}
def gate():
 while True:
  s=state()
  with (out/'gate.jsonl').open('a') as f:f.write(json.dumps(s)+'\n')
  if not s['smhasher_processes'] and (not mac or s['load'][0]<4.5):return s
  print(now(),'WAIT',s,flush=True);time.sleep(60)
summary={'host':a.host,'binary':binary,'binary_sha256':sha(binary),'runs':[],'excluded_runs':[]}
if (out/'execution.json').exists():
 prev=json.loads((out/'execution.json').read_text());assert prev['binary_sha256']==summary['binary_sha256'];summary=prev
(out/'list.txt').write_text(subprocess.check_output([binary,'--list'],text=True,stderr=subprocess.STDOUT))
def save():(out/'execution.json').write_text(json.dumps(summary,indent=2)+'\n')
for rep in range(1,a.passes+1):
 for name in ['MuseAir-v2','XXH3-64','rapidhash','chainhash-256','MuseAir']:
  if any(r['name']==name and r['run']==rep for r in summary['runs']):continue
  while True:
   before=gate();raw=out/(name+f'.run{rep}.txt');cmd=[binary,name,'--test=Speed'];assert sha(binary)==summary['binary_sha256']
   row={'name':name,'run':rep,'command':cmd,'started':now(),'before':before,'raw_file':raw.name,'overlaps':[],'samples':[]}
   print(now(),name,rep,'start',flush=True);start=time.monotonic()
   with raw.open('w') as f:
    p=subprocess.Popen(cmd,stdout=f,stderr=subprocess.STDOUT)
    while p.poll() is None:
     snap=state();others=[x for x in snap['smhasher_processes'] if x['pid']!=p.pid];row['samples'].append(snap)
     if others:row['overlaps'].append({'time':snap['time'],'processes':others})
     time.sleep(5)
   row.update(returncode=p.returncode,elapsed_seconds=time.monotonic()-start,finished=now(),after=state(),sha256=sha(raw))
   if row['overlaps']:
    archived=raw.with_name(raw.stem+'.excluded-'+str(time.time_ns())+'.txt');raw.rename(archived);row['raw_file']=archived.name;summary['excluded_runs'].append(row);save();continue
   if p.returncode:save();raise SystemExit(p.returncode)
   summary['runs'].append(row);save();print(now(),name,rep,'done',flush=True);break
summary['binary_sha256_after']=sha(binary);summary['finished']=now();save()
