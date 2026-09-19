#!/usr/bin/env python3
"""Build explicit message pairs from DP candidates (word, m, k, len, D).  M = 0^len.
M2 = head ^ D (i = bytes 0..8 LE, j = bytes 8..16 LE) with tail word bits delta flipped.
u -> (lo0 to i, hi0 to j): D_i = D_lo, D_j = D_hi.   v -> (lo1 to j, hi1 to i): D_i = D_hi, D_j = D_lo.
Tail bit -> message byte via read_short(tail) for tail length tl = len-16."""
import sys, json
def naf(m):
    d=[]; k=0
    while m:
        if m&1: s=2-(m&3); d.append(k); m-=s
        m>>=1; k+=1
    return d
def tail_byte(word, pos, tl):
    """message byte index holding bit pos of tail word `word` (must be word-only)."""
    b = pos >> 3
    if tl >= 8:  return 16 + (b if word == 'u' else tl - 8 + b)
    if tl >= 4:  return 16 + (b if word == 'u' else tl - 4 + b)
    if tl == 3:  return 16 + ((0 if pos >= 48 else 2) if word == 'u' else 1)
    if tl == 2:  return 16 + 0
    raise ValueError
def build(word, m, k, ln, D_lo, D_hi):
    tl = ln - 16
    M = bytearray(ln); M2 = bytearray(ln)
    Di, Dj = (D_lo, D_hi) if word == 'u' else (D_hi, D_lo)
    M2[0:8] = Di.to_bytes(8, 'little'); M2[8:16] = Dj.to_bytes(8, 'little')
    for p in naf(m):
        pos = k + p; byte = tail_byte(word, pos, tl); M2[byte] ^= 1 << (pos & 7)
    return M.hex(), M2.hex()
if __name__ == '__main__':
    cands = json.load(open(sys.argv[1]))
    sel = [(r['word'], r['m'], r['k'], r['len']) for r in cands]
    want = [('u',21,59,32), ('v',33,58,25), ('u',21,0,19), ('u',21,48,18), ('v',33,0,19), ('u',1,63,32), ('u',1,0,19)]
    for w in want:
        r = next(r for r in cands if (r['word'], r['m'], r['k'], r['len']) == w)
        M, M2 = build(r['word'], r['m'], r['k'], r['len'], r['D_lo'], r['D_hi'])
        print(f"{r['word']} m={r['m']} k={r['k']} len={r['len']} L={r['L']} pred_bits={r['bits']:.3f} w={r['w']}\n  M ={M}\n  M2={M2}")
