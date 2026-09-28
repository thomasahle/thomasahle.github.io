# gxhash 3.5.0: the shortest every-seed flooding family (18 bytes), and any-chunk replacement

## 1. 18 bytes, and why nothing shorter works

For 17..32 bytes, `compress_all` = aesenclast(hv, P2(v0)) with hv = get_partial(first e bytes, e),
e = len - 16, and P2(v) = aesenc(aesenc(v, K0), K1), all public; the seed enters only afterwards,
through a keyed permutation of the 128-bit state. So for a target T and each e-byte prefix,
v0 = P2^-1(T ^ SR(SB(hv))) gives a member: 256^e members at 16 + e bytes, all with state T. The
program builds 256 members of 17 bytes plus 65,536 of 18 bytes (65,792 in total) and checks them.

Minimality among families that share one compressed state: up to 16 bytes a block has at most two
preimages (the 16-byte one and at most one shorter one), and at most 2 + 256 messages of length
<= 17 share a block, so 18 bytes is the shortest maximum length for a 2^16-member family. (Messages
with different states would need the keyed 128-bit permutation to collide for most seeds.)

Examples (memory order): 17 B `0048979328eaf4cfde08239c51a5b2e507`; 18 B `3930e82fb3d3a38495c68625264b99bcdd2f`.

| program | result | log |
|---|---|---|
| `gx_mc.c` on M2 (armv8 AES, cross-checked against a table AES; SMHasher3 verification 64A77B47 / 48F84240) | 65,792 members, 0 state mismatches; all in one 128-bit class for 4096/4096 seeds; published 15/16-byte pair 4096/4096; lengths <= 8: no shared state | `logs/gx_mc_m2_2p12.txt` |

    cc -O2 -std=c11 -march=armv8-a+crypto gx_mc.c -o gx_mc && ./gx_mc 12     # x86-64: -maes

## 2. Long inputs: any 128-byte chunk can be replaced

`compress_all` never sees the seed. Per 128-byte chunk the lanes are a = aesenc(aesenc(aesenc(v0, v2),
v4), v6) and b likewise from v1, v3, v5, v7; for any new v2..v7, solving v0 (and v1) by inverting the
three rounds keeps (a, b) unchanged, so any chunk can be replaced by any of 2^768 others. This is the
mechanism of O. Peters, gxhash issue #83 (2024); these programs only verify it.

| length | 64-bit and 128-bit collisions |
|---|---|
| 256 B, 1000 B, 1 KiB, 64 KiB, 64 KiB + 13 | 65,536/65,536 seeds each (x86-64) |
| 1 MiB, 1 MiB + 7 | 256/256 |
| 2^16 family at 1000 B | all members equal for 1024/1024 seeds |
| M2 rerun, 192 B .. 1 MiB + 7 and the 1000 B family | 256/256 seeds each |

    cc -O2 -std=c11 -maes gx_long.c -o gx_long && ./gx_long 16      # arm64: -march=armv8-a+crypto

Logs: `logs/gx_long_xeon.txt`, `logs/gx_long_m2_2p8.txt`.
