#!/usr/bin/env python3
"""Generate candidate-pair batch files for wyrs (wyhash final v4.3, random-secret model).
Base messages come from random.Random(20260919); every pair is written out in full hex, so the
batch files themselves are the reproduction record."""
import random
R = random.Random(20260919)
M = (1 << 64) - 1
def base(n): return bytes(R.getrandbits(8) for _ in range(n))
def words(b): return [int.from_bytes(b[i:i+8], 'little') for i in range(0, len(b), 8)]
def xor_word(b, i, d):
    w = bytearray(b); v = int.from_bytes(w[8*i:8*i+8], 'little') ^ d; w[8*i:8*i+8] = v.to_bytes(8, 'little'); return bytes(w)
def xor_bytes(b, lo, hi, v=0xff):
    w = bytearray(b)
    for i in range(lo, hi): w[i] ^= v
    return bytes(w)
def line(tag, a, b, lg=None): return f"{tag} {a.hex()} {b.hex()}" + (f" {lg}" if lg else "")

A32 = bytes.fromhex("9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c")
A32p = bytes.fromhex("642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c")

# ---- structural candidates (2^30 each) ----
S = []
S.append(line("A32_row_pair", A32, A32p))
b = base(32); S.append(line("A32_fresh_base_w0M_w1M", b, xor_word(xor_word(b, 0, M), 1, M)))
b = base(32); S.append(line("len32_w0M_only", b, xor_word(b, 0, M)))
b = base(32); S.append(line("len32_w1M_only", b, xor_word(b, 1, M)))
b = base(24); S.append(line("len24_w0M", b, xor_word(b, 0, M)))
b = base(24); S.append(line("len24_w0M_b2", b, xor_word(b, 0, M)))
b = base(24); S.append(line("len24_w0M_zero_base", bytes(24), xor_word(bytes(24), 0, M)))
b = base(23); S.append(line("len23_bytes0-6M", b, xor_bytes(b, 0, 7)))
b = base(20); S.append(line("len20_bytes0-3M", b, xor_bytes(b, 0, 4)))
b = base(17); S.append(line("len17_byte0M", b, xor_bytes(b, 0, 1)))
b = base(24); S.append(line("len24_w0_lo32M", b, xor_word(b, 0, (1 << 32) - 1)))
b = base(24); S.append(line("len24_w0_hi32M", b, xor_word(b, 0, M ^ ((1 << 32) - 1))))
b = base(24); S.append(line("len24_w0_topbit", b, xor_word(b, 0, 1 << 63)))
b = base(24); S.append(line("len24_w0_altA", b, xor_word(b, 0, 0x5555555555555555)))
b = base(24); S.append(line("len24_w0_alt5", b, xor_word(b, 0, 0xAAAAAAAAAAAAAAAA)))
b = base(24); S.append(line("len24_w0_M_minus2", b, xor_word(b, 0, M ^ 2)))
b = base(24); S.append(line("len24_w0_M_minus1", b, xor_word(b, 0, M ^ 1)))
b = base(24); S.append(line("len24_w0M_w2M", b, xor_word(xor_word(b, 0, M), 2, M)))  # tail also complemented
b = base(24); S.append(line("len24_w1M_w2M", b, xor_word(xor_word(b, 1, M), 2, M)))  # tail mum (a,b) both complemented
b = base(48); S.append(line("len48_lane1_w0M_w1M", b, xor_word(xor_word(b, 0, M), 1, M)))
b = base(48); S.append(line("len48_lane2_w2M_w3M", b, xor_word(xor_word(b, 2, M), 3, M)))
b = base(64); S.append(line("len64_w0M_w1M", b, xor_word(xor_word(b, 0, M), 1, M)))
b = base(40); S.append(line("len40_w0M_w1M", b, xor_word(xor_word(b, 0, M), 1, M)))
b = base(40); S.append(line("len40_w2M_only", b, xor_word(b, 2, M)))
b = base(16); S.append(line("len16_allM", b, xor_bytes(b, 0, 16)))
b = base(16); S.append(line("len16_aM", b, xor_bytes(xor_bytes(b, 0, 4), 8, 12)))
b = base(16); S.append(line("len16_bM", b, xor_bytes(xor_bytes(b, 4, 8), 12, 16)))
b = base(8);  S.append(line("len8_allM", b, xor_bytes(b, 0, 8)))
b = base(12); S.append(line("len12_allM", b, xor_bytes(b, 0, 12)))
b = base(4);  S.append(line("len4_allM", b, xor_bytes(b, 0, 4)))
b = base(3);  S.append(line("len3_allM", b, xor_bytes(b, 0, 3)))
b = base(16); S.append(line("len16_random_pair", b, base(16)))
b = base(32); S.append(line("len32_random_pair", b, base(32)))
for l1, l2 in [(16, 15), (8, 7), (4, 3), (1, 0), (12, 11), (16, 9), (2, 1), (16, 8), (10, 5), (7, 0)]:
    S.append(line(f"xlen_zero_{l1}_vs_{l2}", bytes(l1), bytes(l2)))
open("batch_struct.txt", "w").write("\n".join(S) + "\n")

# ---- cross-length zero messages, all pairs of lengths 0..16 (same (a,b)=(0,0); only len differs) ----
X = [line(f"xlen_zero_{l1}_vs_{l2}", bytes(l1), bytes(l2)) for l1 in range(17) for l2 in range(l1)]
open("batch_xlen.txt", "w").write("\n".join(X) + "\n")

# ---- (delta, 0) site at len 24 (L=3): structured deltas ----
b24 = base(24)
D = []
for k in range(1, 65):
    lowk = (1 << k) - 1
    D.append(line(f"site24_low{k}", b24, xor_word(b24, 0, lowk)))
    if k < 64: D.append(line(f"site24_high{64-k}", b24, xor_word(b24, 0, M ^ lowk)))
for k in range(0, 64, 4): D.append(line(f"site24_bit{k}", b24, xor_word(b24, 0, 1 << k)))
for k in range(1, 64): D.append(line(f"site24_Mflip{k}", b24, xor_word(b24, 0, M ^ (1 << k))))  # single-bit neighbours of all-ones
open("batch_site24.txt", "w").write("\n".join(D) + "\n")

# ---- (delta, eps) site at len 32 (L=4): structured pairs ----
b32 = base(32)
P = []
pats = {"M": M, "low32": (1 << 32) - 1, "high32": M ^ ((1 << 32) - 1), "low56": (1 << 56) - 1, "low8": 0xff, "top": 1 << 63, "altA": 0x5555555555555555, "alt5": 0xAAAAAAAAAAAAAAAA, "Mm1": M ^ 1, "Mm2": M ^ 2}
for na, da in pats.items():
    for nb, db in pats.items():
        P.append(line(f"site32_{na}_{nb}", b32, xor_word(xor_word(b32, 0, da), 1, db)))
for k in range(0, 64): P.append(line(f"site32_M_Mflip{k}", b32, xor_word(xor_word(b32, 0, M), 1, M ^ (1 << k))))
open("batch_site32.txt", "w").write("\n".join(P) + "\n")

# ---- length sweep 1..64: whole-message complement, each aligned word complement, random pair (2^29);
#      single-byte complements (2^27) ----
W, Y = [], []
for n in range(1, 65):
    b = base(n)
    W.append(line(f"sweep{n}_allM", b, xor_bytes(b, 0, n)))
    W.append(line(f"sweep{n}_random", b, base(n)))
    for i in range(n // 8): W.append(line(f"sweep{n}_w{i}M", b, xor_word(b, i, M)))
    for i in range(n): Y.append(line(f"sweep{n}_byte{i}M", b, xor_bytes(b, i, i + 1)))
open("batch_sweep_w.txt", "w").write("\n".join(W) + "\n")
open("batch_sweep_b.txt", "w").write("\n".join(Y) + "\n")
print(len(S), len(X), len(D), len(P), len(W), len(Y))
