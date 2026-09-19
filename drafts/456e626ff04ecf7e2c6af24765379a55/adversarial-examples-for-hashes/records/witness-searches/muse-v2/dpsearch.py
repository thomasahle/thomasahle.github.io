#!/usr/bin/env python3
"""Carry-DP search for MuseAir v2 short-path (len 17..32) constant-addition differentials.

Flipping the bit set delta of tail word u (or v) changes X = C4^seed^u (resp. C6^seed^v) by
m = sum_{k in delta} (+/-)2^k, sign per seed bit, so the 128-bit product Y = X*C5 (resp. X*C7)
changes by exactly m*C.  For |m| fixed (prob 2^-(|delta|-1) over the seed bits), the XOR
difference Y ^ (Y +/- m*C) equals a pattern D with probability 2^-w(D) where w is the number of
carry mismatches (uniform-Y model; add and subtract give the same distribution).  The head
(bytes 0..15) is XORed in bijectively, so head' = head ^ D gives a colliding pair with
probability ~ 2^-((|delta|-1) + w).  Bits = log2(L) + that.  L = ceil(len/8).
Output: ranked candidates with the explicit D and the minimal length at which delta is
'word-only' (does not touch the other tail word through read_short's overlapping windows).
"""
import sys, json, math
C5 = 0xd24f2590c0bcee28; C7 = 0xb5d2697595d0a01f
M128 = (1 << 128) - 1

def naf(m):
    """non-adjacent form digits: list of (pos, sign)"""
    d = []; k = 0
    while m:
        if m & 1:
            s = 2 - (m & 3)          # 1 -> +1, 3 -> -1
            d.append((k, s)); m -= s
        m >>= 1; k += 1
    return d

def dp(delta):
    """min carry-mismatch weight and the XOR pattern D for Y ^ (Y + delta), Y uniform 128-bit."""
    INF = 10**9
    cost = [0, INF]; par = [[-1, -1] for _ in range(129)]
    for i in range(128):
        di = (delta >> i) & 1
        nc = [INF, INF]
        for c in (0, 1):
            if cost[c] >= INF: continue
            if di == c:
                if cost[c] < nc[di]: nc[di] = cost[c]; par[i+1][di] = c
            else:
                add = 0 if i == 127 else 1
                for n in (0, 1):
                    if cost[c] + add < nc[n]: nc[n] = cost[c] + add; par[i+1][n] = c
        cost = nc
    end = 0 if cost[0] <= cost[1] else 1
    w = cost[end]
    # backtrack carries
    c = end; carries = [0]*129; carries[128] = end
    for i in range(128, 0, -1):
        c = par[i][c]; carries[i-1] = c
    D = 0
    for i in range(128):
        if ((delta >> i) & 1) ^ carries[i]: D |= 1 << i
    return w, D

def avail(word, pos, tl):
    """is bit pos of tail word `word` ('u'/'v') read ONLY into that word at tail length tl (1..16)?"""
    byte = pos >> 3
    if tl >= 8:
        if pos > 63: return False
        return (byte < tl - 8) if word == 'u' else (byte >= 16 - tl)
    if tl >= 4:
        if pos > 31: return False
        return (byte < tl - 4) if word == 'u' else (byte >= 8 - tl)
    if tl == 3:
        return (pos < 8 or 48 <= pos < 56) if word == 'u' else (pos < 8)
    if tl == 2:
        return (48 <= pos < 56) if word == 'u' else False
    return False

def main():
    maxbits = int(sys.argv[1]) if len(sys.argv) > 1 else 20   # m < 2^maxbits
    maxnaf = int(sys.argv[2]) if len(sys.argv) > 2 else 3
    ms = [m for m in range(1, 1 << maxbits, 2) if len(naf(m)) <= maxnaf]
    print(f"# multipliers: {len(ms)} odd m < 2^{maxbits} with NAF weight <= {maxnaf}", file=sys.stderr)
    res = []
    for word, C in (('u', C5), ('v', C7)):
        for m in ms:
            digits = naf(m); nm = len(digits)
            for k in range(64):
                delta = (m * C << k) & M128
                if delta == 0: continue
                w, D = dp(delta)
                positions = [k + p for p, s in digits]
                if max(positions) > 63: continue
                cost = (nm - 1) + w
                # minimal tail length where all positions are word-only
                best = None
                for tl in range(1, 17):
                    if all(avail(word, p, tl) for p in positions):
                        L = (16 + tl + 7) // 8
                        bits = math.log2(L) + cost
                        if best is None or bits < best[0]: best = (bits, tl, L)
                if best is None: continue
                res.append((best[0], word, m, k, w, nm, best[1] + 16, best[2], D))
    res.sort()
    print(f"# {len(res)} candidates; top 40 by bits = log2(L) + (naf(m)-1) + w")
    print("bits\tword\tm\tk\tw\tnaf(m)\tlen\tL\tD_lo\tD_hi\tpositions")
    for bits, word, m, k, w, nm, ln, L, D in res[:40]:
        positions = [k + p for p, s in naf(m)]
        print(f"{bits:.3f}\t{word}\t{m}\t{k}\t{w}\t{nm}\t{ln}\t{L}\t{D & ((1<<64)-1):016x}\t{D >> 64:016x}\t{positions}")
    # also best single-bit (m=1) per word at each length class
    print("\n# best m=1 (single-bit) per word and L")
    for word in ('u', 'v'):
        for L in (3, 4):
            cand = [r for r in res if r[1] == word and r[2] == 1 and r[7] == L]
            if cand:
                bits, _, m, k, w, nm, ln, _, D = cand[0]
                print(f"{word} L={L}: bits {bits:.3f} k={k} w={w} len={ln} D_lo={D & ((1<<64)-1):016x} D_hi={D >> 64:016x}")
    json.dump([dict(bits=r[0], word=r[1], m=r[2], k=r[3], w=r[4], naf=r[5], len=r[6], L=r[7], D_lo=r[8] & ((1<<64)-1), D_hi=r[8] >> 64) for r in res[:200]], open('dp_top.json', 'w'), indent=1)

if __name__ == '__main__':
    main()
