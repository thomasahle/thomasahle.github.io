#!/usr/bin/env python3
# Continue the final-header experiment from conservative, printed completion counts.
# Interrupted, unreported work is excluded; requested counts are never credited.
import pathlib,os,signal,subprocess,json,time,datetime,hashlib
root=pathlib.Path(__file__).resolve().parents[2];os.chdir(root)
bench=json.loads((root/'certificates/bench/Xeon8375C/execution.json').read_text())
assert bench.get('finished') and len(bench['runs'])==15
assert 'resumed_at' in json.loads((root/'certificates/bench/Xeon8375C/witness-pause.json').read_text())
exe=str(root/'witness');pids=[]
for p in pathlib.Path('/proc').iterdir():
 if p.name.isdigit():
  try:
   if os.readlink(p/'exe')==exe:os.kill(int(p.name),signal.SIGSTOP);pids.append(int(p.name))
  except (FileNotFoundError,PermissionError,ProcessLookupError):pass
for pid in pids:
 try:os.kill(pid,signal.SIGTERM);os.kill(pid,signal.SIGCONT)
 except ProcessLookupError:pass
for _ in range(100):
 if not any(pathlib.Path(f'/proc/{pid}/exe').exists() for pid in pids):break
 time.sleep(.1)
else:raise RuntimeError('old worker did not exit')
folder=root/'certificates/logs/chunks';folder.mkdir(exist_ok=True)
plan={'timestamp':datetime.datetime.now(datetime.timezone.utc).isoformat(),'header_sha256':hashlib.sha256((root/'halftime-hash.hpp').read_bytes()).hexdigest(),'target_per_width':2**36,'reason':'Use the full 24-worker allowance with more workers on the slower widths after timing has finished. Count only printed completed checkpoints from interrupted runs; discard their unreported work.','parts':[]}
threads={1:6,2:4,4:5,8:9};procs=[]
for b in [1,2,4,8]:
 original=root/f'certificates/logs/witness-b{b}.jsonl';events=[]
 for line in original.read_text().splitlines():
  try:events.append(json.loads(line))
  except json.JSONDecodeError:pass
 checkpoints=[e for e in events if e.get('event') in ['progress','result']]
 checkpoint=max(checkpoints,key=lambda e:e['completed']);assert checkpoint['collisions']==0
 credited=checkpoint['completed'];remaining=2**36-credited;assert remaining>=0
 first=folder/f'witness-b{b}.part1.jsonl';original.rename(first)
 second=folder/f'witness-b{b}.part2.jsonl'
 row={'b':b,'part1_log':str(first.relative_to(root)),'part1_counted_completed':credited,'part1_checkpoint':checkpoint,'unreported_part1_work_counted':False,'part2_log':str(second.relative_to(root)),'part2_requested':remaining,'part2_threads':threads[b]}
 if remaining:
  f=second.open('w');cmd=['nice','-n','10',exe,str(b),str(remaining),str(threads[b]),'0'];row['part2_command']=cmd
  procs.append((b,subprocess.Popen(cmd,stdout=f,stderr=subprocess.STDOUT),f));f.close()
 plan['parts'].append(row)
(root/'certificates/logs/witness-parts.json').write_text(json.dumps(plan,indent=2)+'\n')
print(json.dumps(plan),flush=True)
for b,p,f in procs:
 rc=p.wait();print('width',b,'continuation exited',rc,flush=True)
 if rc:raise SystemExit(rc)
summary=[]
for row in plan['parts']:
 events=[json.loads(line) for line in (root/row['part2_log']).read_text().splitlines()]
 end=next(e for e in reversed(events) if e['event']=='result')
 assert end['completed']==row['part2_requested'] and end['collisions']==0
 total=row['part1_counted_completed']+end['completed'];assert total==2**36
 summary.append({'b':row['b'],'completed':total,'collisions':0,'parts':[row['part1_counted_completed'],end['completed']]})
(root/'certificates/logs/witness-summary.json').write_text(json.dumps({'header_sha256':plan['header_sha256'],'results':summary,'finished':datetime.datetime.now(datetime.timezone.utc).isoformat()},indent=2)+'\n')
print('all four widths completed 2^36 counted keys, zero collisions',flush=True)
