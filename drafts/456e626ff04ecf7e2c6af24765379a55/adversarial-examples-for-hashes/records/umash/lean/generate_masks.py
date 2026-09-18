"""Emit a kernel-checked DAG certificate of the signed-support recurrence.
Run on the Xeon. Python supplies witnesses, never a trusted axiom.
"""
from functools import lru_cache
from pathlib import Path

nodes = []
@lru_cache(None)
def supports(w, n):
    if w == 0:
        values = [0] if n == 0 else []
        deps = []
    elif n % 2 == 0:
        a = supports(w-1, n//2)
        values = [2*x for x in a[1]]
        deps = [a[0]]
    else:
        a = supports(w-1, (n-1)//2)
        b = supports(w-1, (n+1)//2)
        values = [2*x+1 for x in a[1]+b[1]]
        deps = [a[0], b[0]]
    name = 'h' + str(len(nodes))
    nodes.append((name, w, n, values, deps))
    return name, values

p = 2**61-1
roots = [supports(64, a*p) for a in range(9)]
values = [x for _, vs in reversed(roots) for x in vs]
def finset(vs):
    return '(' + str(vs) + ' : List ℕ).toFinset'

lines = ['import ProvenHashes.UMASHCertificateSupport', '', 'namespace ProvenHashes.UMASH',
    'set_option maxRecDepth 16384', 'set_option maxHeartbeats 0', '',
    'def maskCertificate : Finset ℕ := ' + finset(values), '',
    'theorem maskSet_eq_certificate : maskSet = maskCertificate := by']
for name, w, n, vs, deps in nodes:
    lines.append(f'  have {name} : signedSupports {w} {n} = {finset(vs)} := by')
    if not w:
        lines.append('    rfl')
        continue
    if n % 2 == 0:
        lhs = f'(signedSupports {w-1} {n//2}).image (2*·)'
    else:
        lhs = f'((signedSupports {w-1} {(n-1)//2}) ∪ (signedSupports {w-1} {(n+1)//2})).image (fun s => 2*s+1)'
    lines.append(f'    change {lhs} = _')
    lines.append('    simp only [' + ', '.join(dict.fromkeys(deps)) +
        ', ← List.toFinset_append, ← list_toFinset_map]')
    lines.append('    rfl')
lines += ['  unfold maskSet masksFor',
    '  have hn : (2^64-1)/p+1 = 9 := by norm_num [p]',
    '  rw [hn]',
    '  norm_num only [p, biUnion_range_step, Finset.range_zero, Finset.biUnion_empty]',
    '  simp only [' + ', '.join(x[0] for x in roots) + ']',
    '  simp only [Finset.union_empty, Finset.empty_union, ← List.toFinset_append]',
    '  rfl', '-- CHECKPOINT', '',
    'theorem maskSet_card : maskSet.card = 852 := by',
    '  rw [maskSet_eq_certificate]', '  decide +kernel', '-- CHECKPOINT', '',
    'end ProvenHashes.UMASH', '']
Path('ProvenHashes/UMASHMaskCertificate.lean').write_text('\n'.join(lines))
print(len(nodes), 'recurrence nodes;', len(values), 'supports;', len(set(values)), 'distinct masks')
