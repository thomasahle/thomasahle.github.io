# gxhash-64: seed-independent collisions

**Hash.** gxhash by Olivier Giniaux, <https://github.com/ogxd/gxhash> (Rust crate, v3
algorithm; the SMHasher3 port `hashes/gxhash.cpp` was made from ogxd/gxhash commit
`55bde47` by Frank J. T. Wojcik, 2025-08-26, MIT).  SMHasher3 registers two variants:
`gxhash` (128-bit) and `gxhash_64` (the low 64 bits, little-endian).  The program
implements the hash from that source (portable table AES; ARMv8 AES or x86 AES-NI are
picked up automatically when the compiler enables them) and reports both outputs.

**What the pairs exploit.** gxhash computes `state = compress_all(m)` from the message
alone, and only then mixes in the seed: `gxhash(m, seed) = finalize(aesenc(state,
seed‖seed))`.  Everything after `compress_all` is a bijection of the 128-bit state for
every seed, so two messages with equal `compress_all` collide for *every* seed, on both
the 64- and the 128-bit output.  `compress_all` is easy to collide because each branch
ends by XORing two independently processed message parts: for 17–32 bytes it is
`SR(SB(hv)) ^ P2(v0)` with `hv` built from the leading bytes and `P2` two *public* AES
rounds of the trailing 16 bytes, so flipping a byte of `hv` is cancelled by setting
`v0' = P2^-1(P2(v0) ^ SR(SB(hv)) ^ SR(SB(hv')))` — one inverse AES round pair, no search
(pair 1).  For messages of at most 16 bytes `compress_all` is just zero-padding plus the
length added to every byte, so a k-byte message and the 16-byte message
`(pad(m)[i] + k - 16) mod 256` are identical after compression (pair 2, a cross-length pair).
The same weakness was reported publicly in ogxd/gxhash issue #83 (2024) with a 192-byte pair.

## Pairs

| # | mechanism | m | m' | rate over random seeds |
|---|-----------|---|----|------------------------|
| 1 | 24 bytes, hv/v0 cancellation through the public rounds | `000000000000000000000000000000000000000000000000` | `0100000000000000a803d3a0b6cb85eb1120e4f3a270c9a6` | 1 (2^0), all seeds |
| 2 | 15 vs 16 bytes, length-padding twin | `000000000000000000000000000000` | `ffffffffffffffffffffffffffffffff` | 1 (2^0), all seeds |

Explicit colliding seeds (from the program): pair 1, seed `0xd73a9a3d941e7ec7` →
`gxhash_64 = 4c6ff29ce0549cdd` for both (and seed `0x0123456789abcdef` →
`69ccb8a053812a7b`, the published test value); pair 2, seed `0xf556ecbfcbfee3ad` →
`43ec3783791d0fb8` for both.

## Build and run

    cc -O2 -o gxhash64_verify gxhash64_verify.c -lm     # auto-detects ARMv8 AES; add -maes on x86 for AES-NI
    ./gxhash64_verify            # 2^24 random seeds (default), about 1 s with hardware AES, 4 s portable
    ./gxhash64_verify 26         # 2^26 seeds
    ./gxhash64_verify 24 0x1234  # different RNG master seed

`-DGX_PORTABLE` forces the table implementation (`gxhash64_verify_portable` below).
The program aborts unless it reproduces the SMHasher3 verification values
`0x48F84240` (gxhash_64) and `0x64A77B47` (gxhash) by the `_ComputedVerifyImpl`
procedure (keys of length 0..255 with seed 256−i, concatenated outputs hashed with seed
0, first 4 bytes LE), cross-checks any hardware AES round against the table round, checks
the published value `69ccb8a053812a7b`, and asserts that the two example seeds reproduce
their published `gxhash_64` values (`4c6ff29ce0549cdd`, `43ec3783791d0fb8`), not merely
that the two messages collide there.  Exit status 0 only if every check passes; a
non-numeric argument is rejected with status 2.

## Expected output

Apple M2 Pro, Apple clang 17, `cc -O2`, default arguments (2^24 seeds, 0.8 s):

```
gxhash / gxhash_64 key-free collision check  (impl = arm-neon-aes)
hash: ogxd/gxhash (v3 algorithm, commit 55bde47) as ported in SMHasher3 hashes/gxhash.cpp
validation: SMHasher3 verification gxhash_64 = 0x48F84240 (expect 0x48F84240) OK, gxhash = 0x64A77B47 (expect 0x64A77B47) OK
validation: arm-neon-aes round vs portable table round, and inverse round: 0 mismatches / 4096
validation: pair 1 at seed 0x0123456789abcdef: gxhash_64 = 69ccb8a053812a7b / 69ccb8a053812a7b (published 69ccb8a053812a7b) OK

Pair 1: 24-byte same-length pair (L = 3 words), key-free
   mechanism: 17..32 bytes: C(m) = SR(SB(hv)) ^ P2(v0) with hv = get_partial(m[0:8]) and v0 = m[8:24];
   P2 = two public AES rounds (keys K0, K1) is invertible, so flip byte 0 of hv and set
   v0' = P2^-1( P2(v0) ^ SR(SB(hv)) ^ SR(SB(hv')) ).  The seed only enters after C.
  m  (24 bytes) = 000000000000000000000000000000000000000000000000
  m' (24 bytes) = 0100000000000000a803d3a0b6cb85eb1120e4f3a270c9a6
  re-deriving m' from m (constant time, no search): matches the published hex
  compress_all(m) = 953a6ffc8610b87f7f40c81e7dea7eaf
  compress_all(m')= 953a6ffc8610b87f7f40c81e7dea7eaf  -> EQUAL (the seed is applied after this point, so every seed collides)
  random seeds: N = 16777216 (2^24)
    gxhash_64 : collisions = 16777216, rate = 1.000000, log2 rate = 0.000
    gxhash 128: collisions = 16777216, rate = 1.000000, log2 rate = 0.000
  first random colliding seed 0xf1f5fbabc606b373: gxhash_64(m) = 2104e5d47c6801a1  gxhash_64(m') = 2104e5d47c6801a1
  published example seed 0xd73a9a3d941e7ec7:
    gxhash_64(m)  = 4c6ff29ce0549cdd   gxhash(m)  = dd9c54e09cf26f4c390150562e7344f4
    gxhash_64(m') = 4c6ff29ce0549cdd   gxhash(m') = dd9c54e09cf26f4c390150562e7344f4
    -> COLLIDE (64 and 128 bit); published gxhash_64 value 4c6ff29ce0549cdd reproduced

Pair 2: 15-byte vs 16-byte cross-length pair (L = 2 words), key-free
   mechanism: len <= 16: C(m) = zero-pad(m) + len, added to every byte mod 256, so
   0^15 -> 0x0f^16 and 0xff^16 -> 0x0f^16: every message shorter than 16 bytes has a 16-byte twin.
  m  (15 bytes) = 000000000000000000000000000000
  m' (16 bytes) = ffffffffffffffffffffffffffffffff
  re-deriving m' from m (constant time, no search): matches the published hex
  compress_all(m) = 0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f
  compress_all(m')= 0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f  -> EQUAL (the seed is applied after this point, so every seed collides)
  random seeds: N = 16777216 (2^24)
    gxhash_64 : collisions = 16777216, rate = 1.000000, log2 rate = 0.000
    gxhash 128: collisions = 16777216, rate = 1.000000, log2 rate = 0.000
  first random colliding seed 0xafd374b1146d3f23: gxhash_64(m) = 26732c2036b199ea  gxhash_64(m') = 26732c2036b199ea
  published example seed 0xf556ecbfcbfee3ad:
    gxhash_64(m)  = 43ec3783791d0fb8   gxhash(m)  = b80f1d798337ec43c5cdc136b3c57d0d
    gxhash_64(m') = 43ec3783791d0fb8   gxhash(m') = b80f1d798337ec43c5cdc136b3c57d0d
    -> COLLIDE (64 and 128 bit); published gxhash_64 value 43ec3783791d0fb8 reproduced

ALL CHECKS PASSED: both pairs collide for every sampled seed on both outputs.
```

The portable build (`-DGX_PORTABLE`) prints the same numbers (`impl = portable`, 4.3 s),
and `./gxhash64_verify 26` gives 67108864/67108864 collisions on both outputs for both
pairs (2.6 s).  Raw logs of these three runs: `run_2e24_neon.txt`, `run_2e24_portable.txt`,
`run_2e26_neon.txt`.  The rate is exactly 1 by the argument above; the sampling only
confirms that the implementation is the real gxhash.

## Files

- `gxhash64_verify.c` — single-file C11 program (MIT, full text in the header).  The gxhash
  implementation was written from the SMHasher3 source, so the header also reproduces the
  upstream MIT copyright lines, Frank J. T. Wojcik (2025) and Olivier Giniaux (2023).
- `README.md` — this file.
- `run_*.txt` — the runs quoted above, exactly as the program printed them.

## Upstream Rust check

`rust_check_head.rs` and `rust_check_head.txt` record the upstream crate check at 55bde47 (3.5.0). Create a Cargo binary with `gxhash = "=3.5.0"`, use the supplied Rust file as src/main.rs, and build with `RUSTFLAGS="-C target-feature=+aes,+sse2"` on x86. The Xeon AES-NI and portable reruns are recorded in `run_2e26_xeon_aesni.txt` and `run_2e24_xeon_portable.txt`; their trailing timing blocks are retained.
