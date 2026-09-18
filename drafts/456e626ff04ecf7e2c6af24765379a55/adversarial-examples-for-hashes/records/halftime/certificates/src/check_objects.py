#!/usr/bin/env python3
"""Compare ELF64 function bytes and normalized relocations across the AVX-512 edit."""
import hashlib, json, pathlib, struct, sys

def elf(path):
    data = pathlib.Path(path).read_bytes()
    assert data[:6] == b'\x7fELF\x02\x01'
    h = struct.unpack_from('<16sHHIQQQIHHHHHH', data)
    sections = [struct.unpack_from('<IIQQQQIIQQ', data, h[6] + i*h[11]) for i in range(h[12])]
    def contents(s): return data[s[4]:s[4]+s[5]]
    def cstr(s, i): return s[i:s.index(b'\0', i)].decode()
    names = contents(sections[h[13]])
    snames = [cstr(names, s[0]) for s in sections]
    symsec = next(s for s in sections if s[1] == 2)
    strings = contents(sections[symsec[6]])
    symbols = [struct.unpack_from('<IBBHQQ', contents(symsec), i) for i in range(0, symsec[5], symsec[9])]
    def symbol_name(i):
        s = symbols[i]
        return cstr(strings, s[0]) or (snames[s[3]] if s[3] < len(snames) else str(s[3]))
    relocs = []
    for s in sections:
        if s[1] == 4:
            for off in range(0, s[5], s[9]):
                address, info, addend = struct.unpack_from('<QQq', contents(s), off)
                relocs.append((s[7], address, info & 0xffffffff, symbol_name(info >> 32), addend))
    funcs = {}
    for i,s in enumerate(symbols):
        if s[1] & 15 == 2 and s[5] and s[3] < len(sections):
            body = contents(sections[s[3]])[s[4]:s[4]+s[5]]
            funcs[symbol_name(i)] = {
                'size': s[5], 'sha256': hashlib.sha256(body).hexdigest(),
                'relocations': [(addr-s[4], kind, name, addend) for sec,addr,kind,name,addend in relocs if sec == s[3] and s[4] <= addr < s[4]+s[5]]}
    constants = {name:hashlib.sha256(contents(s)).hexdigest() for name,s in zip(snames,sections) if name.startswith('.rodata') or name in ['.data','.bss']}
    return {'file':path, 'sha256':hashlib.sha256(data).hexdigest(), 'functions':funcs, 'constants':constants}

before, after = map(elf, sys.argv[1:3])
assert before['constants'] == after['constants']
checked = []
for wrapper in ['BlockWrapperScalar','BlockWrapper128','BlockWrapper256']:
    name = next(n for n in before['functions'] if wrapper in n)
    assert before['functions'][name] == after['functions'][name], name
    checked.append({'wrapper':wrapper,'symbol':name,**before['functions'][name]})
out = {'method':'ELF64 function bytes, each relocation normalized by offset/type/symbol/addend, and all .rodata/.data/.bss sections. The hh24 switch source is unchanged; branch displacements move with the changed AVX-512 function size.',
       'before_object_sha256':before['sha256'],'final_object_sha256':after['sha256'],
       'unchanged_functions':checked,'constants':before['constants'],
       'changed_functions':[n for n in before['functions'] if before['functions'][n] != after['functions'][n]],
       'final_header_sha256':hashlib.sha256(pathlib.Path('halftime-hash.hpp').read_bytes()).hexdigest(),
       'result':'Width 1, 2 and 4 machine-code functions and their external dependencies are identical; their counted key trials remain applicable. Width 8 is rerun in full.'}
pathlib.Path('certificates/logs/final-object-equivalence.json').write_text(json.dumps(out,indent=2)+'\n')
print(json.dumps(out,indent=2))
