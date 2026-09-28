"""Compare harness output to the Lean-derived model. usage: compare.py cases out label"""
import sys
from model import key_from_entropy, polyxor128, avalanche
cases = open(sys.argv[1]).read().split('\n')
outs = open(sys.argv[2]).read().split('\n')
n = bad = 0
lens = []
for c, o in zip(cases, outs):
    if not c: continue
    e, m, _ = c.split(' ')
    e = bytes.fromhex(e); m = b'' if m == '-' else bytes.fromhex(m)
    key, av = key_from_entropy(e)
    want = polyxor128(key, m)
    got = [int(x, 16) for x in o.split(' ')]
    exp = [want, avalanche(want, av), want, want, want]
    n += 1; lens.append(len(m))
    if got != exp:
        bad += 1
        print('MISMATCH len', len(m), [hex(x) for x in got], [hex(x) for x in exp])
print(f'{sys.argv[3]}: {n} cases, {bad} mismatches, lengths {min(lens)}..{max(lens)}, total bytes {sum(lens)}')
sys.exit(1 if bad else 0)
