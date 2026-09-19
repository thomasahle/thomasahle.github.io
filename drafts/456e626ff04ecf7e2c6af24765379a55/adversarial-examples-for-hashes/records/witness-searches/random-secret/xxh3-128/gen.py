#!/usr/bin/env python3
"""Generate the pair files and fold-mask files for rs128 (deterministic, seed 20260919)."""
import random, struct, os
R = random.Random(20260919)
M64 = (1 << 64) - 1
out = os.path.dirname(os.path.abspath(__file__))

def words_to_hex(ws): return b''.join(struct.pack('<Q', w) for w in ws).hex()
def rnd64(): return R.getrandbits(64)

lines_family = []
# Row pair F (blog data.json, pair_id F, 32 bytes)
F = ("9bd4604137366abe642b9fbec8c9954188d35499de169df633e0964e8c04600c",
     "642b9fbec8c995419bd4604137366abe88d35499de169df633e0964e8c04600c")
lines_family.append(f"32 {F[0]} {F[1]} F_row")
open(f"{out}/F.txt", "w").write(f"# blog row pair F\n32 {F[0]} {F[1]} F_row\n")

def comp_pair(sumval, block, label, n=32):
    """Complement 16-byte block `block` (0 or 1 for the two mix16B blocks of a 32-byte
    message); words in that block are chosen so w_a + w_b = sumval mod 2^64."""
    ws = [rnd64() for _ in range(n // 8)]
    a, b = (0, 1) if block == 0 else (2, 3)
    ws[b] = (sumval - ws[a]) & M64
    ws2 = list(ws); ws2[a] ^= M64; ws2[b] ^= M64
    return f"{n} {words_to_hex(ws)} {words_to_hex(ws2)} {label}"

lines_family.append(comp_pair(M64, 0, "G_sum_-1_block0"))
lines_family.append(comp_pair(M64, 1, "G2_sum_-1_block1"))
lines_family.append(comp_pair((1 << 63) - 1, 0, "H_sum_2^63-1_block0"))
lines_family.append(comp_pair((1 << 62) - 1, 0, "T1_twin_sum_2^62-1_block0"))
lines_family.append(comp_pair((1 << 63) + (1 << 62) - 1, 0, "T2_twin_sum_2^63+2^62-1_block0"))
lines_family.append(comp_pair(rnd64() | 2, 0, "C_sum_random_control"))
# xxh3-64 memo pair (default-secret NAF construction) -- control: should not transfer
lines_family.append("32 " + "00" * 16 + "51151210404400000000008204000105 " + "00" * 16 + "aeeaedefbfbbffffffffff7dfbfffefa xxh3-64_memo_NAF_pair")
# 160-byte complement (129..240 path), block 0, sum -1
ws = [rnd64() for _ in range(20)]; ws[1] = (M64 - ws[0]) & M64
ws2 = list(ws); ws2[0] ^= M64; ws2[1] ^= M64
lines_family.append(f"160 {words_to_hex(ws)} {words_to_hex(ws2)} M160_sum_-1_block0")
# 64-byte complement (33..64 path, two mix32B), block 0 (input+0), sum -1
ws = [rnd64() for _ in range(8)]; ws[1] = (M64 - ws[0]) & M64
ws2 = list(ws); ws2[0] ^= M64; ws2[1] ^= M64
lines_family.append(f"64 {words_to_hex(ws)} {words_to_hex(ws2)} M64_sum_-1_block0")
open(f"{out}/family.txt", "w").write("# complement-family pairs under the random-secret model\n" + "\n".join(lines_family) + "\n")

# Long path (248 bytes > XXH3_MIDSIZE_MAX): stripe swap and top-bit flips
lines_long = []
for lane in (0, 3):
    ws = [rnd64() for _ in range(31)]
    ws[8 + lane] = ws[lane] ^ M64          # stripe 1 lane = ~ (stripe 0 lane)
    ws2 = list(ws); ws2[lane], ws2[8 + lane] = ws[8 + lane], ws[lane]
    lines_long.append(f"248 {words_to_hex(ws)} {words_to_hex(ws2)} L248_swap_comp_lane{lane}_stripes01")
ws = [rnd64() for _ in range(31)]
ws2 = list(ws); ws2[0] ^= 1 << 63; ws2[8] ^= 1 << 63
lines_long.append(f"248 {words_to_hex(ws)} {words_to_hex(ws2)} L248_topbit_lane0_stripes01")
# swap with a random (non-complement) difference: w and w^D swapped between stripes
ws = [rnd64() for _ in range(31)]; D = rnd64(); ws[8] = ws[0] ^ D
ws2 = list(ws); ws2[0], ws2[8] = ws[8], ws[0]
lines_long.append(f"248 {words_to_hex(ws)} {words_to_hex(ws2)} L248_swap_randomdiff_lane0")
# swap across a block boundary is impossible below 1024+64 bytes; skip.
open(f"{out}/long.txt", "w").write("# long-path (>240 B) structural pairs\n" + "\n".join(lines_long) + "\n")

# Controls: random pairs at many lengths + single-bit flips at 32 and 16 bytes
lines_ctrl = []
for n in (1, 2, 3, 4, 5, 8, 9, 12, 16, 17, 24, 31, 32, 33, 48, 63, 64, 65, 96, 128, 129, 160, 192, 240, 241, 248, 256):
    a = bytes(R.getrandbits(8) for _ in range(n)); b = bytes(R.getrandbits(8) for _ in range(n))
    lines_ctrl.append(f"{n} {a.hex()} {b.hex()} R{n}_random")
for n in (32, 16, 8):
    base = bytes(R.getrandbits(8) for _ in range(n))
    for bit in range(8 * n):
        b = bytearray(base); b[bit // 8] ^= 1 << (bit % 8)
        lines_ctrl.append(f"{n} {base.hex()} {bytes(b).hex()} B{n}_bit{bit}")
# all 2-bit flips inside the first word at 32 bytes (sum-preserving ones need bit i set / cleared; here random base)
open(f"{out}/controls.txt", "w").write("# controls: random and low-weight pairs\n" + "\n".join(lines_ctrl) + "\n")

# Fold masks
mk = []
mk.append(f"{M64:016x} {M64:016x} allones")
for k in range(64):
    mk.append(f"{M64 ^ (1<<k):016x} {M64:016x} allones_clr_d0_b{k}")
    mk.append(f"{M64:016x} {M64 ^ (1<<k):016x} allones_clr_d1_b{k}")
    mk.append(f"{M64 ^ (1<<k):016x} {M64 ^ (1<<k):016x} allones_clr_both_b{k}")
for k in range(1, 64):
    mk.append(f"{(1<<k)-1:016x} {(1<<k)-1:016x} low{k}_both")
    mk.append(f"{(M64 << k) & M64:016x} {(M64 << k) & M64:016x} high{64-k}_both")
for k in range(64):
    mk.append(f"{1<<k:016x} {1<<k:016x} bit{k}_both")
for k in range(63):
    mk.append(f"{1<<k:016x} {3<<k:016x} bit{k}_d0_3bit{k}_d1")
for d1 in (1, 3, 5, 7, 0xff, 1 << 32, 1 << 63):
    mk.append(f"{M64:016x} {d1:016x} allones_d0_{d1:x}_d1")
for lab, d in (("alt5", 0x5555555555555555), ("altA", 0xAAAAAAAAAAAAAAAA), ("nib0F", 0x0F0F0F0F0F0F0F0F),
               ("byteFF00", 0xFF00FF00FF00FF00), ("lo32", 0xFFFFFFFF), ("hi32", 0xFFFFFFFF00000000)):
    mk.append(f"{d:016x} {d:016x} {lab}_both")
for i in range(100):
    d = rnd64(); mk.append(f"{d:016x} {d:016x} rand_both_{i}")
for i in range(100):
    w = R.randint(1, 4); d = 0
    for _ in range(w): d |= 1 << R.randrange(64)
    mk.append(f"{d:016x} {d:016x} lowwt{w}_both_{i}")
for i in range(100):
    d0 = rnd64(); d1 = rnd64(); mk.append(f"{d0:016x} {d1:016x} rand_diff_{i}")
open(f"{out}/masks1.txt", "w").write("# fold XOR-differential scan\n" + "\n".join(mk) + "\n")
# distance-2 neighbours of all-ones: 500 random pairs of cleared bits over the 128 positions
mk2 = []
seen = set()
while len(mk2) < 500:
    i, j = sorted(R.sample(range(128), 2))
    if (i, j) in seen: continue
    seen.add((i, j))
    d0, d1 = M64, M64
    for p in (i, j):
        if p < 64: d0 ^= 1 << p
        else: d1 ^= 1 << (p - 64)
    mk2.append(f"{d0:016x} {d1:016x} allones_clr2_{i}_{j}")
open(f"{out}/masks2.txt", "w").write("# distance-2 neighbours of all-ones\n" + "\n".join(mk2) + "\n")
print("families", len(lines_family), "long", len(lines_long), "controls", len(lines_ctrl), "masks1", len(mk), "masks2", len(mk2))
