"""Run unmodified SMHasher3 --test=Speed, one process at a time, one hash name per invocation.

Xeon: two complete passes, `nice -n 10 taskset -c CPUS`; each start waits while any
other SMHasher3 process runs or the one-minute load is >= --xeon-max-load.
M2: three complete passes, `nice -n 10`; each start waits (60 s polls) until no
other SMHasher3 process runs and the one-minute load is below 4.5 (run_speed.py
of the ChainHash-128 lane). Existing completed runs of the same binary are kept.
"""
import argparse,pathlib,subprocess,os,time,datetime,hashlib,json,platform
p=argparse.ArgumentParser();p.add_argument('binary',type=pathlib.Path);p.add_argument('out',type=pathlib.Path)
p.add_argument('--passes',type=int);p.add_argument('--cpus',default='16-23');p.add_argument('--xeon-max-load',type=float,default=80.0)
p.add_argument('--names',nargs='+',default=['polyxor-128','polyxor-128.raw','XXH3-128','UMASH-128','rapidhash','komihash']);a=p.parse_args()
mac=platform.system()=='Darwin';a.out.mkdir(parents=True,exist_ok=True);binary=a.binary.resolve()
sha=lambda f:hashlib.sha256(pathlib.Path(f).read_bytes()).hexdigest();bsha=sha(binary)
def stamp():return datetime.datetime.now(datetime.timezone.utc).isoformat()
def cpu_idle(cpus):
 if mac:return None
 lo,hi=map(int,cpus.split('-'))
 def rd():
  d={}
  for l in open('/proc/stat'):
   f=l.split()
   if f[0].startswith('cpu') and f[0]!='cpu':v=list(map(int,f[1:]));d[int(f[0][3:])]=(sum(v),v[3]+v[4])
  return d
 x=rd();time.sleep(2);y=rd()
 return {str(c):round(100*(1-(y[c][1]-x[c][1])/max(1,y[c][0]-x[c][0])),1) for c in range(lo,hi+1)}
def state():
 return {'utc':stamp(),'uptime':subprocess.run(['uptime'],capture_output=True,text=True).stdout.strip(),'load':os.getloadavg(),
  'smhasher_pids':subprocess.run(['pgrep','-x','SMHasher3'],capture_output=True,text=True).stdout.split()}
def gate():
 while True:
  s=state();s['cpu_busy_percent']=cpu_idle(a.cpus)
  s['passed']=not s['smhasher_pids'] and s['load'][0]<(4.5 if mac else a.xeon_max_load)
  with (a.out/'gate.jsonl').open('a') as f:f.write(json.dumps(s)+'\n')
  if s['passed']:return s
  print('WAIT',s,flush=True);time.sleep(60)
prefix=['nice','-n','10']+([] if mac else ['taskset','-c',a.cpus])
for rep in range(1,(a.passes or (3 if mac else 2))+1):
 for name in a.names:
  raw=a.out/f'{name}.run{rep}.txt';meta=raw.with_suffix('.json')
  if meta.exists():assert json.loads(meta.read_text())['binary_sha256']==bsha;continue
  before=gate();print(stamp(),'START',rep,name,flush=True);start=time.monotonic()
  cmd=prefix+[str(binary),name,'--test=Speed']
  with raw.open('w') as f:r=subprocess.run(cmd,stdout=f,stderr=subprocess.STDOUT)
  after=state();after['cpu_busy_percent']=cpu_idle(a.cpus)
  row={'name':name,'run':rep,'command':prefix+['SMHasher3',name,'--test=Speed'],'before':before,'after':after,'elapsed_seconds':time.monotonic()-start,'returncode':r.returncode,'raw_file':raw.name,'raw_sha256':sha(raw),'binary_sha256':bsha}
  meta.write_text(json.dumps(row,indent=2)+'\n');print('DONE',rep,name,row['elapsed_seconds'],r.returncode,flush=True)
  if r.returncode:raise SystemExit(r.returncode)
print('ALLDONE',flush=True)
