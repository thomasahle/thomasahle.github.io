"""Xeon-only evidence: compare the scalar ideal model with SMHasher3 clhash.cpp.

The C++ hashing routines are copied verbatim from the recorded source snapshot.
Only SMHasher headers/registration are replaced by a small input/output harness.
This check is not part of the Lean proof or a C++ correspondence theorem.
"""
from pathlib import Path
import hashlib
import random
import struct
import subprocess

root = Path(__file__).resolve().parent
source_path = root.parent / 'reference' / 'clhash.cpp'
source = source_path.read_text()
start = source.index('// Keys for scalar xorshift128.')
end = source.index('//------------------------------------------------------------\ntemplate <bool bswap>\nstatic void CLHash(')
body = source[start:end]
header = r'''
#include <immintrin.h>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cassert>
#include <cstdio>
#include <vector>
using seed_t = uint64_t;
static inline __m128i mm_bswap64(__m128i x) {
  return _mm_shuffle_epi8(x, _mm_setr_epi8(7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8));
}
template<bool swap> static uint64_t GET_U64(const uint8_t* p, size_t off) {
  uint64_t x; memcpy(&x, p + off, 8);
  return swap ? __builtin_bswap64(x) : x;
}
'''
main = r'''
int main() {
  uint64_t n;
  while (fread(&n, 8, 1, stdin) == 1) {
    if (n > (1ULL << 24)) return 2;
    alignas(16) uint64_t key[133];
    if (fread(key, 8, 133, stdin) != 133) return 3;
    std::vector<uint8_t> data(n + 32, 0);
    if (fread(data.data(), 1, n, stdin) != n) return 4;
    uint64_t result = clhash<false, false>(key, data.data(), n);
    if (fwrite(&result, 8, 1, stdout) != 1) return 5;
  }
}
'''
outdir = root / 'vectors'
outdir.mkdir(exist_ok=True)
cpp = outdir / 'clhash_harness.cpp'
cpp.write_text(header + body + main)
binary = outdir / 'clhash_harness'
subprocess.run(['g++', '-O2', '-std=c++17', '-mpclmul', '-mssse3', str(cpp), '-o', str(binary)], check=True)

MASK = (1 << 64) - 1
P64 = (1 << 64) | 27
PLAZY = (1 << 128) | 6

def mul(a, b):
    p = 0
    while b:
        if b & 1:
            p ^= a
        a <<= 1
        b >>= 1
    return p

def mod(p, d):
    while p.bit_length() >= d.bit_length():
        p ^= d << (p.bit_length() - d.bit_length())
    return p

def raw_block(data, key):
    result = 0
    for pos in range(0, len(data), 16):
        i = pos // 8
        a = int.from_bytes(data[pos:pos + 8], 'little')
        b = int.from_bytes(data[pos + 8:pos + 16], 'little')
        result ^= mul(a ^ key[i], b ^ key[i + 1])
    return result

def model(data, key):
    if len(data) <= 1024:
        core = raw_block(data, key)
    else:
        kpoly = (key[128] | key[129] << 64) & ((1 << 126) - 1)
        acc = 0
        for pos in range(0, len(data), 1024):
            acc = mod(mul(acc, kpoly), PLAZY) ^ raw_block(data[pos:pos + 1024], key)
        core = mul((acc & MASK) ^ key[130], (acc >> 64) ^ key[131])
    return mod(core ^ mul(key[132], len(data)), P64)

rng = random.Random(20260918)
lengths = sorted(set(range(41)) | set(range(1008, 1041)) | set(range(2032, 2065)) |
                 {127, 128, 129, 255, 256, 257, 511, 512, 513, 4095, 4096, 4097, 16384})
cases = []
for n in lengths:
    for mode in range(4):
        key = [rng.getrandbits(64) for _ in range(133)]
        if mode == 0:
            key[128] = key[129] = 0
        elif mode == 1:
            key[128], key[129] = 1, 0
        elif mode == 2:
            key[128] = key[129] = MASK
        data = bytes(n) if mode == 0 else rng.randbytes(n)
        cases.append((data, key))
for _ in range(128):
    n = rng.randrange(1, 32769)
    cases.append((rng.randbytes(n), [rng.getrandbits(64) for _ in range(133)]))
request = b''.join(struct.pack('<Q133Q', len(data), *key) + data for data, key in cases)
actual = subprocess.run([str(binary)], input=request, stdout=subprocess.PIPE, check=True).stdout
assert len(actual) == 8 * len(cases)
rows = ['case,bytes,output_hex']
for i, (data, key) in enumerate(cases):
    got = struct.unpack_from('<Q', actual, 8 * i)[0]
    want = model(data, key)
    assert got == want, (i, len(data), hex(got), hex(want))
    rows.append(f'{i},{len(data)},{got:016x}')
(outdir / 'results.csv').write_text('\n'.join(rows) + '\n')
report = (f'PASS: {len(cases)} vectors; bytes 0 through {max(len(m) for m, _ in cases)}.\n'
          'Includes empty input, partial words, block boundaries, and polynomial keys 0, 1, and all ones.\n'
          'C++ variant: clhash<false, false>; full keys supplied directly, no seed expansion.\n'
          f'Source SHA256: {hashlib.sha256(source_path.read_bytes()).hexdigest()}\n'
          f'Harness SHA256: {hashlib.sha256(cpp.read_bytes()).hexdigest()}\n'
          f'Input corpus SHA256: {hashlib.sha256(request).hexdigest()}\n'
          f'Results SHA256: {hashlib.sha256((outdir / "results.csv").read_bytes()).hexdigest()}\n')
(outdir / 'REPORT.txt').write_text(report)
print(report, end='')
