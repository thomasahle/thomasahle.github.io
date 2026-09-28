"""Generate differential-test cases: entropy, message, split points."""
import random, sys
rng = random.Random(int(sys.argv[1]) if len(sys.argv) > 1 else 2026)
def ent(kind):
    if kind == 'zero': return bytes(4160)
    if kind == 'ones': return b'\xff' * 4160
    return rng.randbytes(4160)
def msg(n, kind):
    if kind == 'zero': return bytes(n)
    if kind == 'ones': return b'\xff' * n
    return rng.randbytes(n)
lengths = list(range(0, 641))
for k in range(1, 5):
    for d in (-257, -129, -128, -127, -9, -8, -7, -1, 0, 1, 7, 8, 9, 127, 128, 129, 257):
        lengths.append(4096 * k + d)
lengths += [rng.randrange(0, 20000) for _ in range(120)]
cases = []
for i, n in enumerate(lengths):
    ek = ['rand', 'rand', 'rand', 'zero', 'ones'][i % 5]
    mk = ['rand', 'rand', 'zero', 'ones'][i % 4]
    m = msg(n, mk)
    cuts = sorted(set(rng.randrange(0, n + 1) for _ in range(rng.randrange(0, 5)))) if n else []
    if n > 4096 and i % 3 == 0: cuts = sorted(set(cuts + [4096 * rng.randrange(1, n // 4096 + 1)]))
    if n > 4096 and i % 3 == 1: cuts = sorted(set(cuts + [4095, 4097]) & set(range(n + 1)))
    cases.append((ent(ek), m, cuts))
for e, m, c in cases:
    print(e.hex(), m.hex() or '-', ','.join(map(str, c)) or '-')
