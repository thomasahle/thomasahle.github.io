"""Construct a fixed pair colliding at seed 0; run on the Xeon only.

This is an implementation check, not part of the Lean proof's trusted base.
The two inputs are fixed before the subsequent random-seed experiment.
"""
import json
import subprocess

MASK = (1 << 64) - 1
def expand(seed, count):
    out = b''
    for _ in range(count):
        seed = (seed + 0x9e3779b97f4a7c15) & MASK
        z = ((seed ^ (seed >> 30)) * 0xbf58476d1ce4e5b9) & MASK
        z = ((z ^ (z >> 27)) * 0x94d049bb133111eb) & MASK
        out += (z ^ (z >> 31)).to_bytes(8, 'little')
    return out

def mul(x, y):
    z = 0
    v = x
    for i in range(128):
        if y & (1 << (127-i)):
            z ^= v
        v = (v >> 1) ^ (0xe1000000000000000000000000000000 if v & 1 else 0)
    return z

def power(x, n):
    result = 1 << 127  # GCM's leftmost bit is the multiplicative identity.
    while n:
        if n & 1:
            result = mul(result, x)
        x = mul(x, x)
        n >>= 1
    return result

key = expand(0, 2)
hbytes = subprocess.check_output(['openssl', 'enc', '-aes-128-ecb', '-K', key.hex(),
                                 '-nosalt', '-nopad'], input=bytes(16))
h = int.from_bytes(hbytes, 'big')
assert h != 0
hinv = power(h, (1 << 128) - 2)
assert mul(h, hinv) == 1 << 127
length_block = 128 << 64
a = mul(length_block, hinv)
message = a.to_bytes(16, 'big')
assert mul(mul(a, h) ^ length_block, h) == 0

def gmac(m):
    return subprocess.check_output(['openssl', 'mac', '-macopt', 'cipher:AES-128-GCM',
        '-macopt', 'hexkey:' + key.hex(), '-macopt', 'hexiv:' + '00'*12, 'GMAC'],
        input=m).decode().strip().lower()

t0, t1 = gmac(b''), gmac(message)
assert t0 == t1
print(json.dumps({'seed': 0, 'key_hex': key.hex(), 'H_hex': hbytes.hex(),
                  'message_0_hex': '', 'message_1_hex': message.hex(),
                  'tag_0_hex': t0, 'tag_1_hex': t1,
                  'openssl': subprocess.check_output(['openssl', 'version']).decode().strip(),
                  'result': 'PASS'}, indent=2))
