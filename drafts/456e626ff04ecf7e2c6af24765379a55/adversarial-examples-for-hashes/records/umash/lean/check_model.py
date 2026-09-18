"""Differential smoke check against the exact supplied Python reference.
Not a probability proof. Run only on the Xeon under the lane build wrapper.
"""
from pathlib import Path
import importlib.util
import json
import subprocess

spec = importlib.util.spec_from_file_location('umash_reference', 'sources/umash_reference.py')
ref = importlib.util.module_from_spec(spec)
spec.loader.exec_module(ref)
result = subprocess.run(['lake', 'env', 'lean', '--run', 'CheckModel.lean'],
                        text=True, capture_output=True, check=True)
records = []
for line in result.stdout.splitlines():
    profile, n, h0, h1 = map(int, line.split(','))
    q, p = 2**64, 2**61-1
    if profile == 0:
        m = bytes((13*i+7)%256 for i in range(n))
        keys, seed, multipliers = list(range(34)), 42, (1234567, 9876543)
    elif profile == 1:
        m = bytes([255])*n
        keys, seed, multipliers = [q-1-i for i in range(34)], q-1, (2, p-1)
    else:
        m = bytes(0 if i%3 == 0 else 255 for i in range(n))
        keys = [(i*0x9E3779B97F4A7C15)%q for i in range(34)]
        seed, multipliers = 0xDEADBEEF12345678, (p-1, p-2)
    expected = [ref.umash(ref.UmashKey(f, keys), seed, m, secondary)
                for f, secondary in zip(multipliers, (False, True))]
    assert [h0,h1] == expected, (profile, n, h0, h1, expected)
    records.append({'profile': profile, 'bytes': n, 'primary': h0, 'secondary': h1})
Path('ModelVerification.json').write_text(json.dumps({'result':'PASS',
    'reference':'sources/umash_reference.py', 'cases':records}, indent=2)+'\n')
print('PASS:', len(records), 'cases (20 lengths x 3 profiles), both compressors; 120 outputs')
