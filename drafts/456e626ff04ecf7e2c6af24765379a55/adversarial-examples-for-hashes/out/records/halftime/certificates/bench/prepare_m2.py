#!/usr/bin/env python3
# Reuse unchanged SMHasher3 object archives to keep preparation on the Mac short.
import datetime,hashlib,json,os,pathlib,shlex,shutil,subprocess,time
root=pathlib.Path(__file__).resolve().parents[2];os.chdir(root)
oldsrc=(root/'../m2-rerun/smhasher3').resolve();oldbuild=(root/'../m2-rerun/build-fixed').resolve();src=root/'smhasher3-m2';build=root/'build-m2';build.mkdir(exist_ok=True)
log=root/'certificates/bench/M2Pro';log.mkdir(exist_ok=True)
def gate():
 while True:
  ps=subprocess.check_output(['ps','-axo','pid,comm'],text=True)
  processes=[s.strip() for s in ps.splitlines() if s.strip().split() and s.strip().split()[-1].endswith('/SMHasher3')]
  row={'time':datetime.datetime.now(datetime.timezone.utc).isoformat(),'load':list(os.getloadavg()),'smhasher_processes':processes}
  with (log/'preparation-gate.jsonl').open('a') as f:f.write(json.dumps(row)+'\n')
  if not processes and row['load'][0]<4.5:break
  time.sleep(15)
gate()
for name in ['libSMHasher3Tests.a','libSMHasher3Hashlib.a']:shutil.copy2(oldbuild/name,build/name)
shutil.copytree(oldbuild/'include',build/'include',dirs_exist_ok=True)
commands=[]
def run(cmd,name):
 commands.append(cmd)
 (log/'preparation-commands.json').write_text(json.dumps(commands,indent=2)+'\n')
 with (log/name).open('w') as f:subprocess.run(['nice','-n','10']+cmd,check=True,cwd=build,stdout=f,stderr=subprocess.STDOUT)
entries=json.load(open(oldbuild/'compile_commands.json'))
for original,replacement,obj in [('main.cpp','main.cpp','main.o'),('hashes/halftimehash.cpp','hashes/halftimefixed.cpp','halftimefixed.o')]:
 e=next(e for e in entries if e['file']==str(oldsrc/original))
 cmd=shlex.split(e['command'])
 cmd=[x.replace(str(oldsrc),str(src)).replace(str(oldbuild),str(build)) for x in cmd]
 cmd[cmd.index('-o')+1]=str(build/obj);cmd[cmd.index('-c')+1]=str(src/replacement)
 if 'fixed' in obj:cmd=[x if x!='-std=c++11' else '-std=c++17' for x in cmd]
 run(cmd,'compile-'+obj+'.txt')
cmd=shlex.split((oldbuild/'CMakeFiles/SMHasher3.dir/link.txt').read_text())
cmd=[str(build/'main.o') if x=='CMakeFiles/SMHasher3.dir/main.cpp.o' else x for x in cmd]
cmd.insert(cmd.index('libSMHasher3Tests.a'),str(build/'halftimefixed.o'))
run(cmd,'link.txt')
# Required architecture check; these are short, single-worker verification runs.
(root/'halftime-hash.hpp').write_bytes((root/'HalftimeHash-fork/halftime-hash.hpp').read_bytes())
(root/'base-neon.hpp').write_bytes((root/'certificates/base-neon.hpp').read_bytes())
for name,flags in [('neon',[]),('scalar',['-DHH_SCALAR','-fno-vectorize','-fno-slp-vectorize']),('ubsan',['-fsanitize=undefined','-fno-sanitize-recover=all'])]:
 run(['/usr/bin/clang++','-std=c++17','-O2','-flax-vector-conversions']+flags+[str(root/'certificates/src/verify.cpp'),'-o',str(build/('verify-'+name))],'build-verify-'+name+'.txt')
 run([str(build/('verify-'+name)),str(build/('vectors-'+name+'.bin'))],'verify-'+name+'.json')
shas={name:hashlib.sha256((build/('vectors-'+name+'.bin')).read_bytes()).hexdigest() for name in ['neon','scalar','ubsan']}
assert len(set(shas.values()))==1,shas
(log/'vectors.json').write_text(json.dumps(shas,indent=2)+'\n')
# Preserve cached-object provenance and verify unchanged test/timer sources.
reused={str(p.relative_to(oldsrc)):hashlib.sha256(p.read_bytes()).hexdigest() for p in oldsrc.rglob('*') if p.is_file() and p.suffix in ['.cpp','.h','.in','.c'] and '.git' not in p.parts}
for rel,h in reused.items():
 if rel!='main.cpp':assert hashlib.sha256((src/rel).read_bytes()).hexdigest()==h,rel
(log/'reused-sources.json').write_text(json.dumps(reused,indent=2)+'\n')
(log/'cached-archives.json').write_text(json.dumps({name:hashlib.sha256((build/name).read_bytes()).hexdigest() for name in ['libSMHasher3Tests.a','libSMHasher3Hashlib.a']},indent=2)+'\n')
subprocess.run(['python3',str(root/'certificates/bench/run_speed.py'),'M2Pro',str(build/'SMHasher3'),str(log),'--gate'],check=True)
