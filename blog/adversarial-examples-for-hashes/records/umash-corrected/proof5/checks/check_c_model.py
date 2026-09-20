#!/usr/bin/env python3
"""Deterministic C/reference comparisons and exact shuffler structure checks."""
import ctypes as C
import hashlib
import importlib.util
import json
import os
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location('umash_reference', ROOT/'materials/umash-lemma/umash_reference.py')
ref = importlib.util.module_from_spec(spec)
spec.loader.exec_module(ref)
q, p = 1 << 64, (1 << 61)-1


def rank(rows):
    pivots = {}
    for row in rows:
        while row:
            pos = row.bit_length()-1
            if pos not in pivots:
                pivots[pos] = row
                break
            row ^= pivots[pos]
    return len(pivots)


def main():
    assert set(os.sched_getaffinity(0)) <= set(range(40,48))
    libraries = []
    for name in ('scalar', 'optimized'):
        lib = C.CDLL(str(ROOT/f'checks/c_bridge_{name}.so'))
        fn = lib.checked_fingerprint
        fn.argtypes = [C.POINTER(C.c_uint64), C.c_uint64, C.c_uint64, C.c_uint64,
                       C.c_void_p, C.c_size_t, C.POINTER(C.c_uint64)]
        fn.restype = None
        libraries.append(fn)
    sizes = list(range(0,274)) + [287,288,289,495,496,497,511,512,513,767,768,769,
                                  1023,1024,1025,2047,2048,2049,4095,4096,4097,8192]
    profiles = [(2,3,0), (p-1,p-2,q-1), (1,123456789,1 << 63)]
    h = hashlib.sha256()
    checks = 0
    for i,(f0,f1,seed) in enumerate(profiles):
        oh = [int.from_bytes(hashlib.sha256(f'oh-{i}-{j}'.encode()).digest()[:8],'little') for j in range(34)]
        assert len(set(oh)) == 34
        key_words = (C.c_uint64*34)(*oh)
        for size in sizes:
            data = bytes((j*37+(j//17)*11+i*79)%256 for j in range(size))
            buf = C.create_string_buffer(data)
            expected = [ref.umash(ref.UmashKey(f0,oh),seed,data,False),
                        ref.umash(ref.UmashKey(f1,oh),seed,data,True)]
            for fn in libraries:
                out = (C.c_uint64*2)()
                fn(key_words,f0,f1,seed,buf,size,out)
                assert list(out) == expected, (i,size,list(out),expected)
                h.update(bytes(out))
                checks += 1
    # For every possible PH shuffler, its kernel on bit 127 = 0 has size 2.
    shufflers = 0
    for n in range(2,17):
        for position in range(1,n):
            columns = [ref.shuffle(1 << j,position,n) for j in range(127)]
            assert rank(columns) == 126
            assert ref.shuffle(1 << 63,position,n) == 0
            assert all((x & 1) == 0 and ((x >> 64) & 1) == 0 for x in columns)
            shufflers += 1
    # Scaled exhaustive map-fibre checks; never extrapolated as probabilities.
    slices = 0
    for w in range(2,9):
        mask = (1 << w)-1
        for k in range(1,min(15,w)+1):
            for d in range(1,1 << w):
                bins = {}
                for v in range(1 << w):
                    z = ref.gfmul(d,v)
                    lo,hi = z & mask, z >> w
                    def sh(x):
                        return ((x << 1) ^ (0 if k==1 else x << k)) & mask
                    y = sh(lo) | (sh(hi) << w)
                    bins[y] = bins.get(y,0)+1
                assert max(bins.values()) <= 2
                slices += 1
    # The shared-multiplier fibre f=p-1 is an exact identity for every OH key.
    # These full-width examples independently check the source correspondence.
    swap_checks = 0
    x, y = bytes(256)+bytes([1])*256, bytes([1])*256+bytes(256)
    for i in range(8):
        oh = [int.from_bytes(hashlib.sha256(f'swap-{i}-{j}'.encode()).digest()[:8],'little') for j in range(34)]
        seed = (i*0xfedcba9876543211) % q
        for secondary in (False,True):
            key = ref.UmashKey(p-1,oh)
            assert ref.umash(key,seed,x,secondary) == ref.umash(key,seed,y,secondary)
            swap_checks += 1
    result = dict(status='PASS', full_fingerprint_comparisons=checks,
                  outputs_sha256=h.hexdigest(), shufflers_checked=shufflers,
                  scaled_shuffler_slices=slices, shared_multiplier_examples=swap_checks,
                  note='Finite source/algebra checks, not sampled collision-rate evidence.')
    (ROOT/'checks/model_checks.json').write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps(result,indent=2))


if __name__ == '__main__':
    main()
