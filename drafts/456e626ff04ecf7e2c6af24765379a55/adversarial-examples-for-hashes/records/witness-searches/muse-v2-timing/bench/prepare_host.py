#!/usr/bin/env python3
import datetime,gzip,hashlib,json,os,pathlib,re,shutil,subprocess,sys,time
root=pathlib.Path(__file__).resolve().parent.parent
mac=sys.platform=='darwin';host='M2Pro' if mac else 'Xeon8375C'
out=root/('build-m2' if mac else 'build');out.mkdir(exist_ok=True)
log=root/'evidence'/host;log.mkdir(parents=True,exist_ok=True)
old=(root/'../m2-rerun/build-fixed').resolve() if mac else pathlib.Path('/home/thomas-ahle/agents/speedbench/build-release-20260917')
base=(root/'../m2-rerun/smhasher3').resolve() if mac else pathlib.Path('/home/thomas-ahle/agents/speedbench/source')
src=root/('smhasher3-m2' if mac else 'source');commands=[]
def sha(p):return hashlib.sha256(pathlib.Path(p).read_bytes()).hexdigest()
def gate():
 if not mac:return
 while True:
  ps=subprocess.check_output(['ps','-axo','pid=,comm='],text=True)
  processes=[s.strip() for s in ps.splitlines() if s.strip().split() and pathlib.Path(s.strip().split()[-1]).name.lower()=='smhasher3']
  row={'time':datetime.datetime.now(datetime.timezone.utc).isoformat(),'load':list(os.getloadavg()),'smhasher_processes':processes}
  with (log/'preparation-gate.jsonl').open('a') as f:f.write(json.dumps(row)+'\n')
  if not processes and row['load'][0]<4.5:return
  print('WAIT preparation',row,flush=True);time.sleep(60)
def run(cmd,file,check=True):
 cmd=list(map(str,cmd));commands.append(cmd);(log/'commands.json').write_text(json.dumps(commands,indent=2)+'\n')
 with (log/file).open('w') as f:p=subprocess.run(cmd,stdout=f,stderr=subprocess.STDOUT)
 if check and p.returncode:raise RuntimeError(file+' exit '+str(p.returncode))
 return p.returncode
gate()
if not src.exists():subprocess.run(['rsync','-a','--exclude=build*','--exclude=.git',str(base)+'/',str(src)+'/'],check=True)
for name in ['museair_v2.cpp','verify.cpp']:shutil.copy2(root/'bench'/name,src/'hashes'/name)
shutil.copy2(root/'muse-v2/museair2.h',src/'hashes/museair2.h')
flags=['-std=c++17','-O3','-march=native','-DNDEBUG','-DHAVE_THREADS','-I'+str(old/'include'),'-I'+str(src/'include/hashlib'),'-I'+str(src/'include/common')]
if mac:flags+=['-Xclang','-target-feature','-Xclang','+aes']
tests=old/'libSMHasher3Tests.a';hashlib_a=old/'libSMHasher3Hashlib.a';main=old/'CMakeFiles/SMHasher3.dir/main.cpp.o'
extra=['/opt/homebrew/opt/openssl@3/lib/libcrypto.dylib'] if mac else []
inputs=[main,tests,hashlib_a,*[base/('hashes/'+n+'.cpp') for n in ['xxhash','rapidhash','chainhash','museair']],base/'tests/SpeedTest.cpp']
(log/'reused-inputs.json').write_text(json.dumps({str(p):sha(p) for p in inputs},indent=2)+'\n')
def compile_obj():run(['nice','-n','10','c++',*flags,'-c',src/'hashes/museair_v2.cpp','-o',out/'museair_v2.o'],'compile.txt')
compile_obj()
run(['nice','-n','10','c++',*flags,src/'hashes/verify.cpp',out/'museair_v2.o',hashlib_a,tests,hashlib_a,*extra,'-o',out/'verify'],'compile-verify.txt')
vectors=out/'vectors.txt'
with gzip.open(root/'muse-v2/vectors.txt.gz','rb') as f, vectors.open('wb') as g:shutil.copyfileobj(f,g)
gate();run([out/'verify',vectors,*([] if mac else ['30'])],'verification.txt')
text=(log/'verification.txt').read_text();assert text.endswith('PASS\n')
m=re.search(r'REGISTRATION name=(\S+) backend=(\S+) bits=(\d+) verify=([0-9a-f]+)',text);assert m
n,b,w,v=m.groups();(log/'registrations.json').write_text(json.dumps([dict(name=n,backend=b,bits=int(w),verification=v)],indent=2)+'\n')
p=src/'hashes/museair_v2.cpp';p.write_text(p.read_text().replace('$.verification_LE = 0, $.verification_BE = 0',f'$.verification_LE = 0x{v}, $.verification_BE = 0x{v}'))
p=src/'hashes/Hashsrc.cmake'
if 'museair_v2.cpp' not in p.read_text():
 with p.open('a') as f:f.write('\nlist(APPEND HASH_SRC_FILES hashes/museair_v2.cpp)\n')
gate();compile_obj()
run(['nice','-n','10','c++',main,out/'museair_v2.o',tests,hashlib_a,*extra,'-o',out/'SMHasher3'],'link.txt')
# Re-link the verifier to the final timing registration object.
run(['nice','-n','10','c++',*flags,src/'hashes/verify.cpp',out/'museair_v2.o',hashlib_a,tests,hashlib_a,*extra,'-o',out/'verify'],'compile-verify-final.txt')
gate();run([out/'verify',vectors],'verification-final.txt')
for name in ['MuseAir-v2','MuseAir','XXH3-64','rapidhash','chainhash-256']:
 gate();run([out/'SMHasher3',name,'--test=Sanity'],name+'.sanity.txt')
 assert 'FAIL' not in (log/(name+'.sanity.txt')).read_text()
run(['otool','-tvV',out/'museair_v2.o'] if mac else ['objdump','-d',out/'museair_v2.o'],'museair_v2.asm')
run(['c++','--version'],'compiler.txt');run(['uname','-a'],'host.txt')
(log/'build-sha256.json').write_text(json.dumps({str(p):sha(p) for p in [out/'SMHasher3',out/'museair_v2.o',src/'hashes/museair_v2.cpp',src/'hashes/museair2.h',vectors]},indent=2)+'\n')
run([sys.executable,'-u',root/'bench/run_speed.py',host,out/'SMHasher3',log,'--passes','3' if mac else '2'],'runner.txt')
