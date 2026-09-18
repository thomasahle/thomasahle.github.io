#!/usr/bin/env python3
import pathlib,os,signal,subprocess,json,datetime
root=pathlib.Path(__file__).resolve().parents[2]
exe=str(root/'witness');paused=[]
for p in pathlib.Path('/proc').iterdir():
 if p.name.isdigit():
  try:
   if os.readlink(p/'exe')==exe:
    os.kill(int(p.name),signal.SIGSTOP);paused.append(int(p.name))
  except (FileNotFoundError,PermissionError,ProcessLookupError):pass
log=root/'certificates/bench/Xeon8375C';log.mkdir(parents=True,exist_ok=True)
state={'paused_witness_pids':paused,'paused_at':datetime.datetime.now(datetime.timezone.utc).isoformat()}
(log/'witness-pause.json').write_text(json.dumps(state,indent=2))
try:
 subprocess.run(['python3',str(root/'certificates/bench/run_speed.py'),'Xeon8375C',str(root.parent/'speedbench-hhfixed/build/SMHasher3'),str(log)],check=True)
finally:
 for pid in paused:
  try:
   if os.readlink(f'/proc/{pid}/exe')==exe:os.kill(pid,signal.SIGCONT)
  except (FileNotFoundError,PermissionError,ProcessLookupError):pass
 state['resumed_at']=datetime.datetime.now(datetime.timezone.utc).isoformat()
 (log/'witness-pause.json').write_text(json.dumps(state,indent=2))
