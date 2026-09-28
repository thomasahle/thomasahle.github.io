"""Link the PolyXOR128 registration against an existing SMHasher3 build.

Usage: python3 bench/build.py SMHASHER3_SOURCE BASE_BUILD OUTPUT [--verify 0x.. --raw-verify 0x..]

Pattern of the ChainHash-128 lane: reuse the base build's main object and its
two static libraries (timer, tests, every comparison hash), compile only the
new registration, and link the Rust staticlib (polyxor 0.1.0 via polyxor_ffi,
RUSTFLAGS=-C target-cpu=native, release, LTO). Paths are inputs only; the
recorded provenance carries no machine paths.
"""
import argparse, pathlib, subprocess, platform, json, hashlib, os, time, shlex
p=argparse.ArgumentParser();p.add_argument('source',type=pathlib.Path);p.add_argument('base',type=pathlib.Path);p.add_argument('out',type=pathlib.Path)
p.add_argument('--verify',default='0xA9574CA8');p.add_argument('--raw-verify',default='0x23A6E8BB');a=p.parse_args()
bench=pathlib.Path(__file__).resolve().parent
source,base,out=(v.resolve() for v in (a.source,a.base,a.out));out.mkdir(parents=True,exist_ok=True)
mac=platform.system()=='Darwin'
if mac:
 while subprocess.run(['pgrep','-x','SMHasher3'],stdout=subprocess.DEVNULL).returncode==0 or os.getloadavg()[0]>=4.5:
  print('Waiting for preparation gate',os.getloadavg(),flush=True);time.sleep(5)
cargo=str(pathlib.Path.home()/'.cargo/bin/cargo');rustc=str(pathlib.Path.home()/'.cargo/bin/rustc')
env=dict(os.environ,PATH=str(pathlib.Path.home()/'.cargo/bin')+os.pathsep+os.environ['PATH'],RUSTFLAGS='-C target-cpu=native',CARGO_TARGET_DIR=str(out/'cargo-target'))
crate=bench/'polyxor_ffi'
subprocess.run([cargo,'build','--release','--locked']+(['--offline'] if (crate/'vendor').exists() else [])+['--manifest-path',str(crate/'Cargo.toml')],check=True,env=env)
staticlib=out/'cargo-target/release/libpolyxor_ffi.a'
flags=['-O3','-std=c++11','-DNDEBUG','-DHAVE_THREADS','-march=native+crypto' if mac else '-march=native',f'-DPOLYXOR_VERIFY={a.verify}',f'-DPOLYXOR_RAW_VERIFY={a.raw_verify}']
inc=['-I'+str(base/'include'),'-I'+str(source/'include/hashlib'),'-I'+str(source/'include/common')]
reg=bench/'polyxor.cpp';obj=out/'polyxor.o'
subprocess.run(['c++',*flags,*inc,'-c',str(reg),'-o',str(obj)],check=True)
main=base/'main.cpp.o'
if not main.exists():main=base/'CMakeFiles/SMHasher3.dir/main.cpp.o'
libs=[base/'libSMHasher3Tests.a',base/'libSMHasher3Hashlib.a']
cmd=['c++','-O3',str(main),str(obj),*map(str,libs),str(staticlib),'-o',str(out/'SMHasher3')]
if mac:cmd+=shlex.split(subprocess.check_output(['pkg-config','--libs','libcrypto'],text=True))
else:cmd+=['-lpthread','-ldl','-lm']
link_libs=cmd[cmd.index(str(out/'SMHasher3'))+1:]
subprocess.run(cmd,check=True)
subprocess.run(['cc','-O2',str(bench/'probe.c'),str(staticlib),'-o',str(out/'polyxor_probe')]+([] if mac else ['-lpthread','-ldl']),check=True)
probe=subprocess.check_output([str(out/'polyxor_probe')],text=True).strip()
sha=lambda f:hashlib.sha256(pathlib.Path(f).read_bytes()).hexdigest()
lock=crate/'Cargo.lock'
(out/'provenance.json').write_text(json.dumps({
 'compiler':subprocess.check_output(['c++','--version'],text=True).splitlines()[0],
 'flags':flags,'link_extra':['-L<libcrypto dir>' if l.startswith('-L') else (l if l.startswith('-') else 'libpolyxor_ffi.a') for l in link_libs if not l.startswith('/') or l.endswith('libpolyxor_ffi.a')],
 'rustc':subprocess.check_output([rustc,'--version'],text=True).strip(),'cargo':subprocess.check_output([cargo,'--version'],text=True).strip(),
 'rustflags':env['RUSTFLAGS'],'rust_profile':'release, lto=true, codegen-units=1, panic=abort; polyxor default features (aes, std, runtime_detection)',
 'polyxor':{'version':'0.1.0','commit':'3123eb6','src_lib_rs_sha256':sha(bench.parent/'polyxor-src/src/lib.rs'),'src_dispatch_rs_sha256':sha(bench.parent/'polyxor-src/src/dispatch.rs'),'src_x86_64_rs_sha256':sha(bench.parent/'polyxor-src/src/x86_64.rs'),'src_neon_rs_sha256':sha(bench.parent/'polyxor-src/src/neon.rs')},
 'reused_objects':{f.name:sha(f) for f in [main,*libs]},
 'registration_sources':{f.name:sha(f) for f in [reg,crate/'src/lib.rs',crate/'Cargo.toml']+([lock] if lock.exists() else [])},
 'staticlib_sha256':sha(staticlib),'registration_object_sha256':sha(obj),
 'backend_probe':probe,'verification_macros':{'POLYXOR_VERIFY':a.verify,'POLYXOR_RAW_VERIFY':a.raw_verify},
 'binary_sha256':sha(out/'SMHasher3')},indent=2)+'\n')
print(probe)
