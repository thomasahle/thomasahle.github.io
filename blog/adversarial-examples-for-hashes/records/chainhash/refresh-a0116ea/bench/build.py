"""Link ChainHash registrations at pinned header revisions into an existing SMHasher3 build.

Usage:
  python3 bench/build.py SOURCE BASE POLYXOR OUT            (Xeon, M2: the ch128-vs-polyxor lane's inputs)
  python3 bench/build.py SOURCE BASE - OUT --backends       (EPYC: the reproduction's own build)

Pattern of the ch128-vs-polyxor lane: reuse the base build's main object and its two
static libraries (timer, tests, every control hash), and compile only the ChainHash
registrations:

  chainhash-128.a0116ea / chainhash.a0116ea   headers of thomasahle/chainhash @ a0116ea (main)
  chainhash-128.0a03c63 / chainhash.0a03c63   headers @ 0a03c63 (control: the previous main)

With POLYXOR a directory, the PolyXOR128 lane's registration object and Rust staticlib are
linked unchanged (checked by SHA-256). With POLYXOR '-', BASE is a reproduction build whose
Hashlib already contains PolyXOR128; its own link line (CMakeFiles/SMHasher3.dir/link.txt)
is reused with the extra objects added. --backends also builds forced-backend registrations
of the a0116ea headers: chainhash-128.{xmm,ymm,zmm} and chainhash.{xmm,ymm,zmm}.

All registrations use the public entry points (or chainhash*_with_backend for the forced
ones) and the repository's verification values 0x1FCA728C / 0x66672BD6. Paths are inputs
only; the recorded provenance carries no machine paths.
"""
import argparse, pathlib, subprocess, platform, json, hashlib, shlex, os
p = argparse.ArgumentParser()
for n in ('source', 'base', 'polyxor', 'out'): p.add_argument(n)
p.add_argument('--backends', action='store_true')
a = p.parse_args()
bench = pathlib.Path(__file__).resolve().parent
source, base, out = (pathlib.Path(v).resolve() for v in (a.source, a.base, a.out))
out.mkdir(parents=True, exist_ok=True)
mac = platform.system() == 'Darwin'
sha = lambda f: hashlib.sha256(pathlib.Path(f).read_bytes()).hexdigest()
CXX = os.environ.get('CXX', 'c++')

COMMITS = {'a0116ea': 'a0116ea2072c0d9605acc6b47f0dc9f9d57b0c58', '0a03c63': '0a03c6345ac95acc12bbd739fb55191f5d31187b'}
# (registration source, tag, header revision, backend macro or None, verification)
VARIANTS = [('chainhash128_reg.cpp', t, t, None, '0x1FCA728C') for t in COMMITS] + \
           [('chainhash_reg.cpp', t, t, None, '0x66672BD6') for t in COMMITS]
if a.backends:
    VARIANTS += [('chainhash128_reg.cpp', b, 'a0116ea', 'CH128_' + b.upper(), '0x1FCA728C') for b in ('xmm', 'ymm', 'zmm')]
    VARIANTS += [('chainhash_reg.cpp', b, 'a0116ea', 'CH_' + b.upper(), '0x66672BD6') for b in ('xmm', 'ymm', 'zmm')]

flags = ['-O3', '-std=c++11', '-DNDEBUG', '-DHAVE_THREADS', '-march=native+crypto' if mac else '-march=native']
inc = ['-I' + str(base / 'include'), '-I' + str(source / 'include/hashlib'), '-I' + str(source / 'include/common')]
objs, record = [], {}
for src, tag, rev, backend, ver in VARIANTS:
    width = '128' if src.startswith('chainhash128') else '64'
    o = out / f'chainhash{width}_{tag}.o'
    cmd = [CXX, *flags, f'-DCH_TAG={tag}', f'-DCH_VERIFY={ver}', '-I' + str(bench / f'chainhash-{rev}'), *inc]
    if backend: cmd.append(f'-DCH_BACKEND={backend}')
    subprocess.run(cmd + ['-c', str(bench / src), '-o', str(o)], check=True)
    objs.append(o)
    name = ('chainhash-128.' if width == '128' else 'chainhash.') + tag
    record[name] = {'repository': 'https://github.com/thomasahle/chainhash', 'commit': COMMITS[rev],
                    'header_sha256': sha(bench / f'chainhash-{rev}/chainhash/chainhash{"128" if width == "128" else ""}.h'),
                    'entry': (f'chainhash{"128" if width == "128" else ""}_with_backend({backend})' if backend
                              else f'chainhash{"128" if width == "128" else ""}() (run-time dispatch)'),
                    'verification': ver, 'registration_object_sha256': sha(o)}
binary = out / 'SMHasher3'
if a.polyxor == '-':
    # Reuse the reproduction's link command verbatim, adding the registration objects.
    link = (base / 'CMakeFiles/SMHasher3.dir/link.txt').read_text().strip()
    argv = shlex.split(link)
    i = argv.index('-o'); argv[i + 1] = str(binary)
    argv = argv[:i] + [str(o) for o in objs] + argv[i:]
    subprocess.run(argv, check=True, cwd=base)
    polyxor = {'note': "the reproduction build's own PolyXOR128 registration and Rust staticlib (in its Hashlib link line)"}
    reused = {'link.txt': sha(base / 'CMakeFiles/SMHasher3.dir/link.txt'), 'SMHasher3 (base binary)': sha(base / 'SMHasher3')}
    for f in ['libSMHasher3Tests.a', 'libSMHasher3Hashlib.a', 'CMakeFiles/SMHasher3.dir/main.cpp.o']:
        reused[f] = sha(base / f)
else:
    pbuild = pathlib.Path(a.polyxor).resolve()
    POLYXOR = {True: ('5c613ee29778a8dc4c2c63f8f165a871c19b9d3446682b1212a0897aea5c3c77',
                      'e9ab3e05318a20a85cc0d9c0a6fcb1406f16f80ef3f7002f1b785a0a18ec729f'),
               False: ('7bca9cd56b5f6c67fcbc4f5e00d774f9782643bfa2c169096edaf89f897c23a1',
                       'db99c1b11c841164a5dfad4b8fcb5e8777508ec81080773bd15baa00d038e672')}[mac]
    staticlib = pbuild / 'cargo-target/release/libpolyxor_ffi.a'; pobj = pbuild / 'polyxor.o'
    assert (sha(staticlib), sha(pobj)) == POLYXOR, 'polyxor objects differ from the record'
    main = base / 'main.cpp.o'
    if not main.exists(): main = base / 'CMakeFiles/SMHasher3.dir/main.cpp.o'
    libs = [base / 'libSMHasher3Tests.a', base / 'libSMHasher3Hashlib.a']
    extra = shlex.split(subprocess.check_output(['pkg-config', '--libs', 'libcrypto'], text=True)) if mac else ['-lpthread', '-ldl', '-lm']
    subprocess.run([CXX, '-O3', str(main), str(pobj), *map(str, objs), *map(str, libs), str(staticlib), '-o', str(binary)] + extra, check=True)
    polyxor = {'note': 'registration object and Rust staticlib reused byte-for-byte from the PolyXOR128 lane (records/polyxor/evidence/<host>/provenance.json)',
               'staticlib_sha256': POLYXOR[0], 'registration_object_sha256': POLYXOR[1]}
    reused = {f.name: sha(f) for f in [main, *libs]}
(out / 'provenance.json').write_text(json.dumps({
    'compiler': subprocess.check_output([CXX, '--version'], text=True).splitlines()[0],
    'flags': flags, 'chainhash': record, 'polyxor': polyxor, 'reused_objects': reused,
    'registration_sources': {f: sha(bench / f) for f in ['chainhash128_reg.cpp', 'chainhash_reg.cpp', 'build.py']},
    'binary_sha256': sha(binary)}, indent=2) + '\n')
print(binary, sha(binary))
