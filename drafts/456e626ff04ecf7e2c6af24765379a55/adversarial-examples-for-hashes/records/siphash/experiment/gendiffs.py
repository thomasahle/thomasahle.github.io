#!/usr/bin/env python3
"""Generate difference lists. usage: gendiffs.py <W> <class: single|double|struct|all>"""
import sys, itertools
W = int(sys.argv[1]); cls = sys.argv[2]
nb = 64*W
def out(bits_or_words, name):
    if isinstance(bits_or_words, tuple): d0, d1 = bits_or_words
    else:
        d0 = d1 = 0
        for b in bits_or_words:
            if b < 64: d0 |= 1 << b
            else: d1 |= 1 << (b-64)
    print(f"{d0:016x} {d1:016x} {name}")
if cls in ("single", "all"):
    for b in range(nb): out([b], f"bit{b}")
if cls in ("double", "all"):
    for a, b in itertools.combinations(range(nb), 2): out([a, b], f"bits{a}_{b}")
if cls in ("struct", "all"):
    M = (1<<64)-1
    words = [(M, 0), (0xff, 0), (0xff00000000000000, 0), (0x00ff00ff00ff00ff, 0), (0xff00ff00ff00ff00, 0),
             (0x0f0f0f0f0f0f0f0f, 0), (0x8080808080808080, 0), (0x0101010101010101, 0), (0xffffffff, 0), (0xffffffff00000000, 0),
             (0xaaaaaaaaaaaaaaaa, 0), (0x5555555555555555, 0), (0x8000000000000001, 0)]
    names = ["complement0", "lowbyte", "highbyte", "byteswap_even", "byteswap_odd", "nibble", "bytemsb", "bytelsb", "low32", "high32", "alt_a", "alt_5", "wrap"]
    # carry-chain patterns: 0x1, 0x3, 0x7, ... (x+1 vs x differences with carries) and 0xffff.. runs
    for L in range(2, 65, 3): words.append(((1<<L)-1, 0)); names.append(f"run{L}")
    for L in range(2, 65, 3): words.append((((1<<L)-1) << (64-L), 0)); names.append(f"toprun{L}")
    for L in [4, 8, 16, 32]: words.append((((1<<L)-1) << 24, 0)); names.append(f"midrun{L}")
    for d, n in zip(words, names): out(d, n)
    if W == 2:
        for d, n in zip(words, names): out((d[1], d[0]), n + "_w1")
        out((M, M), "complement_both"); out((1, 1), "bit0_both"); out((1<<63, 1<<63), "bit63_both")
        out((0x0123456789abcdef, 0xfedcba9876543210), "rand_both")
