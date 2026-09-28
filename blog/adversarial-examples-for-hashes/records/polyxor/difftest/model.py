"""Independent executable model of the Lean definitions of PolyXOR128.

Transcribed from proofs/PolyXORCompress.lean and proofs/PolyXORFull.lean
(polyxor commit 3123eb6), NOT from src/reference.rs.  The Lean theorem is
stated for every field F with |F| = 2^128, every F4-module structure on F
and every additive map iota injective on degree < 128.  To compare with the
shipped code this model fixes the one instantiation the implementation uses:

  * F = 128-bit strings with XOR as addition and POLYVAL's Montgomery
    product a*b*x^-128 mod P(x), P = x^128 + x^127 + x^126 + x^121 + 1
    (bit i = coefficient of x^i).  This is a field isomorphic to GF(2^128)
    via a -> a * x^-128 (checked in self_check below).
  * iota = the identity on polynomials of degree < 128 (bit layout).
  * w acts on the 64 GF(4) elements stored as 32-bit subwords
    [lo0, hi0, lo1, hi1] (element = lo + hi*w): (lo, hi) -> (hi, lo + hi).

Everything below is written from the math; no code is shared with Rust.
"""

MASK64 = (1 << 64) - 1
MASK128 = (1 << 128) - 1
P = (1 << 128) | (1 << 127) | (1 << 126) | (1 << 121) | 1


def pmul(a, b):
    """Carry-less (GF(2)[x]) product of two non-negative ints."""
    r = 0
    while b:
        low = b & -b
        r ^= a << (low.bit_length() - 1)
        b ^= low
    return r


def pmod(a, m=P):
    dm = m.bit_length()
    while a.bit_length() >= dm:
        a ^= m << (a.bit_length() - dm)
    return a


def pinv(a, m=P):
    """Inverse of a modulo m in GF(2)[x] by extended Euclid."""
    r0, r1, s0, s1 = m, a, 0, 1
    while r1:
        q = 0
        r = r0
        while r and r.bit_length() >= r1.bit_length():
            sh = r.bit_length() - r1.bit_length()
            q ^= 1 << sh
            r ^= r1 << sh
        r0, r1 = r1, r
        s0, s1 = s1, s0 ^ pmul(q, s1)
    assert r0 == 1
    return pmod(s0, m)


XINV128 = pinv(pmod(1 << 128))


def fmul(a, b):
    """Field product of F: Montgomery product a*b*x^-128 mod P."""
    return pmod(pmul(pmod(pmul(a, b)), XINV128))


def clmul(x, y):
    """Lean `clmul`: polynomial product of two words, unreduced (deg < 127)."""
    assert 0 <= x <= MASK64 and 0 <= y <= MASK64
    return pmul(x, y)


def iota(p):
    """Lean `iota` on degree < 128 polynomials: the bit layout."""
    assert p.bit_length() <= 128
    return p


def w_smul(x):
    """w * x for the F4-module structure: per element (lo, hi) -> (hi, lo+hi)."""
    out = 0
    for half in (0, 64):
        lo = (x >> half) & 0xFFFFFFFF
        hi = (x >> (half + 32)) & 0xFFFFFFFF
        out |= hi << half
        out |= (lo ^ hi) << (half + 32)
    return out


def compress_axu128(a, b, c, d):
    """Lean `compress_axu128`: outputs (h0, h1)."""
    e = (a[0] ^ b[0] ^ c[0] ^ d[0], a[1] ^ b[1] ^ c[1] ^ d[1])
    ha, hb, hc, hd, he = (clmul(*a), clmul(*b), clmul(*c), clmul(*d), clmul(*e))
    h0 = iota(ha ^ hb ^ hc ^ hd)
    h1 = iota(hb) ^ w_smul(iota(hc)) ^ w_smul(w_smul(iota(hd))) ^ iota(he)
    return h0, h1


def word_at(m, n):
    """Lean `wordAt`: little-endian 64-bit word n, zero past the end."""
    v = 0
    for k in range(8):
        i = 8 * n + k
        if i < len(m):
            v |= m[i] << (8 * k)
    return v


def len_poly(n):
    """Lean `lenPoly`: the low 64 bits of n."""
    return n & MASK64


def num_blocks(m):
    return (len(m) + 127) // 128


def num_chunks(m):
    return (num_blocks(m) + 31) // 32


def hash_block(x):
    """Lean `hashBlock` on the 16 key-XORed words x."""
    ev = compress_axu128((x[0], x[12]), (x[2], x[14]), (x[8], x[4]), (x[10], x[6]))
    od = compress_axu128((x[1], x[13]), (x[3], x[15]), (x[9], x[5]), (x[11], x[7]))
    return ev[0] ^ od[0], ev[1] ^ od[1]


def chunk_digest(block, m, c):
    h0 = h1 = 0
    nb = num_blocks(m)
    for b in range(32):
        if 32 * c + b < nb:
            x = [word_at(m, 16 * (32 * c + b) + i) ^ block[b][i] for i in range(16)]
            d0, d1 = hash_block(x)
            h0 ^= d0
            h1 ^= d1
    return h0, h1


def polyxor128(key, m):
    """Lean `polyxor128` (= finalize_raw)."""
    block, z, u, y = key
    acc = z
    for c in range(num_chunks(m)):
        d0, d1 = chunk_digest(block, m, c)
        acc = fmul(acc ^ u, d1 ^ y) ^ d0
    return fmul(acc ^ u, iota(len_poly(len(m))) ^ y)


def key_from_entropy(ent):
    """The crate's from_entropy layout (4160 bytes) -> (Key, avalanche_mul)."""
    assert len(ent) >= 4160
    le = lambda bs: int.from_bytes(bs, "little")
    z, u, y, av = (le(ent[16 * i:16 * i + 16]) for i in range(4))
    block = [[le(ent[64 + 128 * b + 8 * i:64 + 128 * b + 8 * i + 8]) for i in range(16)]
             for b in range(32)]
    return (block, z, u, y), max(av, 1)


def avalanche(raw, av_mul, tweak=0):
    """finalize_avalanche_with_tweak, written from its specification."""
    M = 0xE9846AF9B1A615D
    lo = ((raw & MASK64) + tweak) & MASK64
    hi = raw >> 64
    rotl32 = lambda v: ((v << 32) | (v >> 32)) & MASK64
    lo ^= lo >> 32; lo = (lo * M) & MASK64
    hi ^= hi >> 32; hi = (hi * M) & MASK64
    lo ^= rotl32(hi)
    hi = (hi + lo) & MASK64
    lo ^= lo >> 32; lo = (lo * M) & MASK64
    hi ^= hi >> 32; hi = (hi * M) & MASK64
    return fmul((lo << 64) | hi, av_mul)


def self_check():
    import random
    rng = random.Random(1)
    # RFC 8452 Appendix A POLYVAL vector: POLYVAL(H, X1, X2) = dot(dot(X1,H)^X2, H).
    le = lambda s: int.from_bytes(bytes.fromhex(s), "little")
    h = le("25629347589242761d31f826ba4b757b")
    x1 = le("4f4f95668c83dfb6401762bb2d01a262")
    x2 = le("d1a24ddd2721d006bbe45f20d3c9f362")
    want = le("f7a3b47b846119fae5b7866cf5e5b77e")
    assert fmul(fmul(x1, h) ^ x2, h) == want, "RFC 8452 POLYVAL vector"
    one = pmod(1 << 128)  # identity of the Montgomery field
    for _ in range(200):
        a, b, c = (rng.getrandbits(128) for _ in range(3))
        assert fmul(a, b) == fmul(b, a)
        assert fmul(fmul(a, b), c) == fmul(a, fmul(b, c))
        assert fmul(a, b ^ c) == fmul(a, b) ^ fmul(a, c)
        assert fmul(a, one) == a
        # w^2 = w + 1 on the module, and w is additive.
        assert w_smul(w_smul(a)) == w_smul(a) ^ a
        assert w_smul(a ^ b) == w_smul(a) ^ w_smul(b)
    return True


if __name__ == "__main__":
    print("self_check", self_check())
