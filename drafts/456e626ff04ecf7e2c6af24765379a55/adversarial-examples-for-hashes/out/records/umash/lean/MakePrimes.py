"""Generate a Lucas certificate; only Lean checks the resulting proof."""
from pathlib import Path
import sympy as s

out = ['import Mathlib', '', 'namespace ProvenHashes.Classic',
       'set_option maxRecDepth 8192', 'set_option maxHeartbeats 2000000', '']
seen = set()
def cert(n):
    if n in seen:
        return
    seen.add(n)
    if n < 10000:
        out.append(f'theorem prime_{n} : Nat.Prime {n} := by norm_num')
        return
    fs = s.factorint(n-1)
    for q in fs:
        cert(int(q))
    a = int(s.primitive_root(n))
    out.extend([f'theorem prime_{n} : Nat.Prime {n} := by',
                f'  apply lucas_primality {n} ({a} : ZMod {n})',
                '  · norm_num; reduce_mod_char',
                '  · intro q hq hd'])
    fac = ' * '.join(str(q) if e==1 else f'{q} ^ {e}' for q,e in fs.items())
    pat = 'h'
    for _ in range(len(fs)-1):
        pat = f'({pat} | h)'
    out.extend([f'    have hf : {n} - 1 = {fac} := by norm_num',
                '    rw [hf] at hd',
                '    simp only [hq.dvd_mul] at hd',
                '    rcases hd with ' + pat])
    for q,e in fs.items():
        hd = 'h' if e == 1 else '(hq.dvd_of_dvd_pow h)'
        out.extend([f'    · have he := (Nat.dvd_prime prime_{q}).mp {hd}',
                    '      rcases he with he | he',
                    '      · exact False.elim (hq.ne_one he)',
                    '      · subst q; norm_num; reduce_mod_char; decide'])
cert(2**130-5)
out.extend(['', 'theorem poly1305_prime : Nat.Prime (2 ^ 130 - 5) := by',
            f'  exact prime_{2**130-5}', '',
            '#print axioms poly1305_prime', 'end ProvenHashes.Classic', ''])
Path('ProvenHashes/ClassicPrimes.lean').write_text('\n'.join(out))
