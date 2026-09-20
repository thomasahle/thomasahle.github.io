#!/usr/bin/env python3
"""Pair files for the independent verification (own RNG seed 91919; independent of the searcher's gen.py).
   row.txt    : the blog row pair F only.
   family.txt : own members of the complement family (32 B, sum classes -1 / 2^63-1, twins 2^62-1 / 2^63+2^62-1,
                block-1 mirror), a random-sum control, a random-pair control, the xxh3-64 memo NAF pair,
                and 64 B / 160 B sum -1 members.
   format: <len> <m_hex> <m'_hex> <name>"""
import random
rnd = random.Random(91919)
M64 = (1 << 64) - 1
def le(w): return w.to_bytes(8, "little")
def words(n): return [rnd.getrandbits(64) for _ in range(n)]
def hexm(ws): return b"".join(le(w) for w in ws).hex()
F_m  = "9bd4604137366abe642b9fbec8c9954188d35499de169df633e0964e8c04600c"
F_m2 = "642b9fbec8c995419bd4604137366abe88d35499de169df633e0964e8c04600c"
open("row.txt", "w").write("# blog row pair F (data.json xxh3-128)\n32 %s %s F_row\n" % (F_m, F_m2))
lines = ["# own complement-family members and controls", "32 %s %s F_row" % (F_m, F_m2)]
def comp_pair(nwords, block, xmask, name):
    ws = words(nwords)
    ws[2*block+1] = (~ws[2*block] ^ xmask) & M64          # w0 + w1 = -1 - xmask  (mod 2^64)
    ws2 = list(ws); ws2[2*block] = ~ws[2*block] & M64; ws2[2*block+1] = ~ws[2*block+1] & M64
    lines.append("%d %s %s %s" % (8*nwords, hexm(ws), hexm(ws2), name))
comp_pair(4, 0, 0, "own_sum_-1_block0")
comp_pair(4, 0, 0, "own_sum_-1_block0_b")
comp_pair(4, 0, 1 << 63, "own_sum_2^63-1_block0")
comp_pair(4, 0, 3 << 62, "own_twin_sum_2^62-1_block0")
comp_pair(4, 0, 1 << 62, "own_twin_sum_2^63+2^62-1_block0")
comp_pair(4, 1, 0, "own_sum_-1_block1")
# random-sum control: complement both words of block 0 without the sum constraint
ws = words(4); ws2 = list(ws); ws2[0] = ~ws[0] & M64; ws2[1] = ~ws[1] & M64
lines.append("32 %s %s own_complement_random_sum_control" % (hexm(ws), hexm(ws2)))
# random pair control
lines.append("32 %s %s own_random_pair_control" % (hexm(words(4)), hexm(words(4))))
# xxh3-64 memo NAF pair (default-secret trick; expected not to transfer)
lines.append("32 0000000000000000000000000000000051151210404400000000008204000105 00000000000000000000000000000000aeeaedefbfbbffffffffff7dfbfffefa xxh3-64_memo_NAF_pair")
comp_pair(8, 0, 0, "own_64B_sum_-1_block0")
comp_pair(20, 0, 0, "own_160B_sum_-1_block0")
open("family.txt", "w").write("\n".join(lines) + "\n")
print(open("family.txt").read())
