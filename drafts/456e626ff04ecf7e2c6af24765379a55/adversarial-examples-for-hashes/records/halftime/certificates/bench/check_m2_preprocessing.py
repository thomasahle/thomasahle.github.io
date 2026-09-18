#!/usr/bin/env python3
import pathlib,subprocess,shlex,json,hashlib,tempfile,time,os
root=pathlib.Path(__file__).resolve().parents[2]
while True:
 ps=subprocess.check_output(['ps','-axo','pid,comm'],text=True)
 active=[s for s in ps.splitlines() if s.strip().split() and s.strip().split()[-1].rsplit('/',1)[-1]=='SMHasher3']
 gate={'load1':os.getloadavg()[0],'smhasher_processes':active}
 if not active and gate['load1']<4.5:break
 time.sleep(15)
commands=json.loads((root/'certificates/bench/M2Pro/preparation-commands.json').read_text())
base=next(c for c in commands if any(x.endswith('/halftimefixed.cpp') for x in c))
old=root/'certificates/before-avx512-sum.hpp';new=root/'HalftimeHash-fork/halftime-hash.hpp'
values={};runcommands=[]
with tempfile.TemporaryDirectory(prefix='hh-preprocess-') as tmp:
 for label,header in [('timed',old),('final',new)]:
  d=pathlib.Path(tmp)/label;d.mkdir()
  (d/'halftimefixed.cpp').write_bytes((root/'certificates/bench/halftimefixed.cpp').read_bytes())
  (d/'halftime-fixed.hpp').write_bytes(header.read_bytes())
  cmd=base.copy();cmd[cmd.index('-c')]='-E';cmd[cmd.index('-o')+1]=str(d/'out.ii');cmd[-1]=str(d/'halftimefixed.cpp')
  cmd.extend(['-P','-fmacro-prefix-map='+str(d)+'=IDENTICAL_SOURCE'])
  runcommands.append(cmd)
  subprocess.run(['nice','-n','10']+cmd,check=True,stdout=subprocess.DEVNULL,stderr=subprocess.PIPE)
  # Drop empty lines only; no changed text in string literals is normalized.
  pp='\n'.join(s.rstrip() for s in (d/'out.ii').read_text().splitlines() if s.strip())
  values[label]=hashlib.sha256(pp.encode()).hexdigest()
 assert values['timed']==values['final'],values
result={'preprocessor':'Apple Clang, exact adapter build flags, -E -P','gate':gate,'commands':runcommands,'timed_header_sha256':hashlib.sha256(old.read_bytes()).hexdigest(),'final_header_sha256':hashlib.sha256(new.read_bytes()).hexdigest(),'preprocessed_sha256':values,'identical':True,'implication':'All active M2 C++ code is identical; the AVX-512-only reduction change is excluded. Completed M2 vectors, UBSan and timings apply to the final header.'}
(root/'certificates/bench/M2Pro/final-header-equivalence.json').write_text(json.dumps(result,indent=2)+'\n')
print(json.dumps(values))
