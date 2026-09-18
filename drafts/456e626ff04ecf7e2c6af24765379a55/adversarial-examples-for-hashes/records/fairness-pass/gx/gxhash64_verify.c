/*
 * gxhash64_verify.c -- seed-independent ("key-free") collisions in gxhash / gxhash_64
 *
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2026 Thomas Dybdahl Ahle (this program)
 * Copyright (C) 2025 Frank J. T. Wojcik (SMHasher3 hashes/gxhash.cpp, MIT, from which the
 *   gxhash implementation below was written)
 * Copyright (c) 2023 Olivier Giniaux (ogxd/gxhash, MIT)
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Single file, C11, no dependencies.  The gxhash implementation below was written from the
 * SMHasher3 port hashes/gxhash.cpp (Frank J. T. Wojcik, 2025, MIT; itself ported from
 * ogxd/gxhash commit 55bde47, Olivier Giniaux, MIT).  "gxhash_64" is the low 64 bits (LE)
 * of the 128-bit state; both outputs are reported.
 *
 * What it does
 *   1. Validates the implementation: SMHasher3 verification values 0x48F84240 (gxhash_64)
 *      and 0x64A77B47 (gxhash, 128 bit) via the _ComputedVerifyImpl procedure; aborts on
 *      mismatch.  With a hardware AES path it also cross-checks the round against the
 *      portable table implementation.
 *   2. For each published pair: prints the hex, re-derives m' from m in constant time,
 *      checks compress_all(m) == compress_all(m'), hashes both under N random seeds
 *      (default 2^24; argv[1] = log2 N, argv[2] = RNG master seed) and prints the
 *      collision rate on both outputs, then one explicit colliding seed with both values.
 *
 * Build:  cc -O2 -o gxhash64_verify gxhash64_verify.c -lm          (portable or auto-detected AES)
 *         cc -O2 -maes ...  on x86 for AES-NI;  -DGX_PORTABLE forces the table implementation.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct { uint8_t b[16]; } blk;

/* ---- public constants of gxhash (KEYS[0..2]) ---------------------------------------- */
static const uint8_t GX_KEYS[3][16] = {
    {0x42,0x45,0x78,0xf2,0x21,0x3e,0x9d,0xb0,0xe5,0x22,0xc2,0x89,0x8e,0xc2,0x3b,0xfc},
    {0x79,0xe2,0xfc,0x03,0x9b,0x2e,0x6b,0xcb,0x58,0xdc,0x61,0xb3,0xd9,0x2b,0x13,0x39},
    {0x32,0x2e,0x01,0xd0,0x7d,0x2b,0x9d,0x68,0xb7,0xb1,0x44,0x55,0x2b,0x12,0x8b,0xc7}};

/* ---- AES round primitives, portable table version ----------------------------------- */
static const uint8_t SBOX[256] = {
0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5,0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76,
0xca,0x82,0xc9,0x7d,0xfa,0x59,0x47,0xf0,0xad,0xd4,0xa2,0xaf,0x9c,0xa4,0x72,0xc0,
0xb7,0xfd,0x93,0x26,0x36,0x3f,0xf7,0xcc,0x34,0xa5,0xe5,0xf1,0x71,0xd8,0x31,0x15,
0x04,0xc7,0x23,0xc3,0x18,0x96,0x05,0x9a,0x07,0x12,0x80,0xe2,0xeb,0x27,0xb2,0x75,
0x09,0x83,0x2c,0x1a,0x1b,0x6e,0x5a,0xa0,0x52,0x3b,0xd6,0xb3,0x29,0xe3,0x2f,0x84,
0x53,0xd1,0x00,0xed,0x20,0xfc,0xb1,0x5b,0x6a,0xcb,0xbe,0x39,0x4a,0x4c,0x58,0xcf,
0xd0,0xef,0xaa,0xfb,0x43,0x4d,0x33,0x85,0x45,0xf9,0x02,0x7f,0x50,0x3c,0x9f,0xa8,
0x51,0xa3,0x40,0x8f,0x92,0x9d,0x38,0xf5,0xbc,0xb6,0xda,0x21,0x10,0xff,0xf3,0xd2,
0xcd,0x0c,0x13,0xec,0x5f,0x97,0x44,0x17,0xc4,0xa7,0x7e,0x3d,0x64,0x5d,0x19,0x73,
0x60,0x81,0x4f,0xdc,0x22,0x2a,0x90,0x88,0x46,0xee,0xb8,0x14,0xde,0x5e,0x0b,0xdb,
0xe0,0x32,0x3a,0x0a,0x49,0x06,0x24,0x5c,0xc2,0xd3,0xac,0x62,0x91,0x95,0xe4,0x79,
0xe7,0xc8,0x37,0x6d,0x8d,0xd5,0x4e,0xa9,0x6c,0x56,0xf4,0xea,0x65,0x7a,0xae,0x08,
0xba,0x78,0x25,0x2e,0x1c,0xa6,0xb4,0xc6,0xe8,0xdd,0x74,0x1f,0x4b,0xbd,0x8b,0x8a,
0x70,0x3e,0xb5,0x66,0x48,0x03,0xf6,0x0e,0x61,0x35,0x57,0xb9,0x86,0xc1,0x1d,0x9e,
0xe1,0xf8,0x98,0x11,0x69,0xd9,0x8e,0x94,0x9b,0x1e,0x87,0xe9,0xce,0x55,0x28,0xdf,
0x8c,0xa1,0x89,0x0d,0xbf,0xe6,0x42,0x68,0x41,0x99,0x2d,0x0f,0xb0,0x54,0xbb,0x16};
static uint8_t INV_SBOX[256];
static void init_inv_sbox(void) { for (int i = 0; i < 256; i++) INV_SBOX[SBOX[i]] = (uint8_t)i; }

static inline uint8_t xt(uint8_t x) { return (uint8_t)((x << 1) ^ ((x & 0x80) ? 0x1b : 0)); }
static uint8_t gmul(uint8_t a, uint8_t b) { uint8_t r = 0; while (b) { if (b & 1) r ^= a; a = xt(a); b >>= 1; } return r; }

/* AES state layout: byte i sits in row i%4, column i/4.  ShiftRows shifts row r left by r. */
static void sub_shift(const uint8_t *s, uint8_t *o) {
    for (int c = 0; c < 4; c++) for (int r = 0; r < 4; r++) o[4*c + r] = SBOX[s[4*((c + r) & 3) + r]];
}
static void inv_sub_shift(const uint8_t *o, uint8_t *s) {
    for (int c = 0; c < 4; c++) for (int r = 0; r < 4; r++) s[4*((c + r) & 3) + r] = INV_SBOX[o[4*c + r]];
}
static void mixcol(uint8_t *s) {
    for (int c = 0; c < 4; c++) {
        uint8_t a0 = s[4*c], a1 = s[4*c+1], a2 = s[4*c+2], a3 = s[4*c+3];
        s[4*c]   = xt(a0) ^ xt(a1) ^ a1 ^ a2 ^ a3;
        s[4*c+1] = a0 ^ xt(a1) ^ xt(a2) ^ a2 ^ a3;
        s[4*c+2] = a0 ^ a1 ^ xt(a2) ^ xt(a3) ^ a3;
        s[4*c+3] = xt(a0) ^ a0 ^ a1 ^ a2 ^ xt(a3);
    }
}
static void inv_mixcol(uint8_t *s) {
    for (int c = 0; c < 4; c++) {
        uint8_t a0 = s[4*c], a1 = s[4*c+1], a2 = s[4*c+2], a3 = s[4*c+3];
        s[4*c]   = gmul(a0,14) ^ gmul(a1,11) ^ gmul(a2,13) ^ gmul(a3, 9);
        s[4*c+1] = gmul(a0, 9) ^ gmul(a1,14) ^ gmul(a2,11) ^ gmul(a3,13);
        s[4*c+2] = gmul(a0,13) ^ gmul(a1, 9) ^ gmul(a2,14) ^ gmul(a3,11);
        s[4*c+3] = gmul(a0,11) ^ gmul(a1,13) ^ gmul(a2, 9) ^ gmul(a3,14);
    }
}
static inline blk bxor(blk a, blk b) { for (int i = 0; i < 16; i++) a.b[i] ^= b.b[i]; return a; }
/* aesenc(a,k) = MC(SR(SB(a))) ^ k ;  aesenclast(a,k) = SR(SB(a)) ^ k  (x86 _mm_aesenc semantics) */
static blk p_aesenc(blk a, blk k)     { blk o; sub_shift(a.b, o.b); mixcol(o.b); return bxor(o, k); }
static blk p_aesenclast(blk a, blk k) { blk o; sub_shift(a.b, o.b); return bxor(o, k); }
static blk inv_aesenc(blk o, blk k)   { blk a; o = bxor(o, k); inv_mixcol(o.b); inv_sub_shift(o.b, a.b); return a; }

/* ---- optional hardware AES paths (auto-detected; -DGX_PORTABLE disables) ------------- */
#if !defined(GX_PORTABLE) && defined(__ARM_FEATURE_AES)
#include <arm_neon.h>
static inline blk aesenc(blk a, blk k) {
    uint8x16_t t = vaesmcq_u8(vaeseq_u8(vld1q_u8(a.b), vdupq_n_u8(0)));
    blk o; vst1q_u8(o.b, veorq_u8(t, vld1q_u8(k.b))); return o; }
static inline blk aesenclast(blk a, blk k) {
    uint8x16_t t = vaeseq_u8(vld1q_u8(a.b), vdupq_n_u8(0));
    blk o; vst1q_u8(o.b, veorq_u8(t, vld1q_u8(k.b))); return o; }
#define GX_IMPL "arm-neon-aes"
#elif !defined(GX_PORTABLE) && defined(__AES__)
#include <wmmintrin.h>
static inline blk aesenc(blk a, blk k) { blk o;
    _mm_storeu_si128((__m128i *)o.b, _mm_aesenc_si128(_mm_loadu_si128((const __m128i *)a.b), _mm_loadu_si128((const __m128i *)k.b))); return o; }
static inline blk aesenclast(blk a, blk k) { blk o;
    _mm_storeu_si128((__m128i *)o.b, _mm_aesenclast_si128(_mm_loadu_si128((const __m128i *)a.b), _mm_loadu_si128((const __m128i *)k.b))); return o; }
#define GX_IMPL "x86-aesni"
#else
#define aesenc p_aesenc
#define aesenclast p_aesenclast
#define GX_IMPL "portable"
#endif

/* ---- gxhash (ogxd/gxhash v3 algorithm, as in SMHasher3 hashes/gxhash.cpp) ------------ */
static inline blk ld(const uint8_t *p) { blk o; memcpy(o.b, p, 16); return o; }
static inline blk key(int i) { return ld(GX_KEYS[i]); }
static inline blk zero(void) { blk o; memset(o.b, 0, 16); return o; }
static inline blk add8(blk a, blk b) { for (int i = 0; i < 16; i++) a.b[i] = (uint8_t)(a.b[i] + b.b[i]); return a; }

/* zero-pad to 16 bytes, then add len to every byte */
static blk get_partial(const uint8_t *p, size_t len) {
    blk o = zero(); memcpy(o.b, p, len);
    for (int i = 0; i < 16; i++) o.b[i] = (uint8_t)(o.b[i] + (uint8_t)len);
    return o;
}
static blk compress_8(const uint8_t *p, const uint8_t *end, blk hv, size_t len) {
    blk t1 = zero(), t2 = zero(), lane1 = hv, lane2 = hv;
    while (p < end) {
        blk v0 = ld(p), v1 = ld(p+16), v2 = ld(p+32), v3 = ld(p+48), v4 = ld(p+64), v5 = ld(p+80), v6 = ld(p+96), v7 = ld(p+112);
        p += 128;
        blk a = aesenc(v0, v2), b = aesenc(v1, v3);
        a = aesenc(a, v4); b = aesenc(b, v5); a = aesenc(a, v6); b = aesenc(b, v7);
        t1 = add8(t1, key(0)); t2 = add8(t2, key(1));
        lane1 = aesenclast(aesenc(a, t1), lane1);
        lane2 = aesenclast(aesenc(b, t2), lane2);
    }
    blk lv; uint32_t l = (uint32_t)len;
    for (int i = 0; i < 4; i++) { lv.b[4*i] = (uint8_t)l; lv.b[4*i+1] = (uint8_t)(l >> 8); lv.b[4*i+2] = (uint8_t)(l >> 16); lv.b[4*i+3] = (uint8_t)(l >> 24); }
    return aesenc(add8(lane1, lv), add8(lane2, lv));
}
static blk compress_many(const uint8_t *p, const uint8_t *end, blk hv, size_t len) {
    size_t unroll = ((size_t)(end - p) / 16) / 8; const uint8_t *endp = end - unroll * 128;
    while (p < endp) { hv = aesenc(hv, ld(p)); p += 16; }
    return compress_8(p, end, hv, len);
}
/* The seed-independent part.  Every branch ends in aesenclast(hv, v0) = SR(SB(hv)) ^ v0. */
static blk compress_all(const uint8_t *in, size_t len) {
    const uint8_t *p = in, *end = in + len; size_t extra = len % 16; blk hv;
    if (len == 0) return zero();
    if (len <= 16) return get_partial(p, len);
    if (extra == 0) { hv = ld(p); p += 16; } else { hv = get_partial(p, extra); p += extra; }
    blk v0 = ld(p); p += 16;
    if (len > 32) { v0 = aesenc(v0, ld(p)); p += 16;
        if (len > 48) { v0 = aesenc(v0, ld(p)); p += 16;
            if (len > 64) hv = compress_many(p, end, hv, len); } }
    v0 = aesenc(v0, key(0)); v0 = aesenc(v0, key(1));
    return aesenclast(hv, v0);
}
static blk finalize(blk h) { h = aesenc(h, key(0)); h = aesenc(h, key(1)); return aesenclast(h, key(2)); }
static blk gxhash128(const uint8_t *in, size_t len, uint64_t seed) {
    blk sx; for (int i = 0; i < 8; i++) { sx.b[i] = (uint8_t)(seed >> (8*i)); sx.b[8+i] = sx.b[i]; }
    return finalize(aesenc(compress_all(in, len), sx));
}
static uint64_t low64(blk o) { uint64_t r = 0; for (int i = 7; i >= 0; i--) r = (r << 8) | o.b[i]; return r; }
static uint64_t gxhash64(const uint8_t *in, size_t len, uint64_t seed) { return low64(gxhash128(in, len, seed)); }

/* ---- validation: SMHasher3 lib/Hashinfo.cpp _ComputedVerifyImpl ----------------------- */
static uint32_t smhasher3_verification(int hashbytes) {
    uint8_t key_[256] = {0}, hashes[16*256] = {0};
    for (int i = 0; i < 256; i++) {
        blk h = gxhash128(key_, (size_t)i, (uint64_t)(256 - i));
        memcpy(hashes + i*hashbytes, h.b, (size_t)hashbytes); key_[i] = (uint8_t)i;
    }
    blk t = gxhash128(hashes, (size_t)(256*hashbytes), 0);
    return (uint32_t)t.b[0] | ((uint32_t)t.b[1] << 8) | ((uint32_t)t.b[2] << 16) | ((uint32_t)t.b[3] << 24);
}

/* ---- own RNG: splitmix64 -> xoshiro256** ------------------------------------------- */
static uint64_t splitmix64(uint64_t *s) {
    uint64_t z = (*s += 0x9E3779B97F4A7C15ull);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull; z = (z ^ (z >> 27)) * 0x94D049BB133111EBull; return z ^ (z >> 31);
}
static uint64_t RS[4];
static inline uint64_t rotl(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
static uint64_t rng_next(void) {
    uint64_t r = rotl(RS[1] * 5, 7) * 9, t = RS[1] << 17;
    RS[2] ^= RS[0]; RS[3] ^= RS[1]; RS[1] ^= RS[2]; RS[0] ^= RS[3]; RS[2] ^= t; RS[3] = rotl(RS[3], 45); return r;
}

/* ---- the published pairs ------------------------------------------------------------ */
typedef struct { const char *name, *mech, *m_hex, *m2_hex; uint64_t example_seed, example_hash; } pair_t;
static const pair_t PAIRS[] = {
    {"24-byte same-length pair (L = 3 words), key-free",
     "17..32 bytes: C(m) = SR(SB(hv)) ^ P2(v0) with hv = get_partial(m[0:8]) and v0 = m[8:24];\n"
     "   P2 = two public AES rounds (keys K0, K1) is invertible, so flip byte 0 of hv and set\n"
     "   v0' = P2^-1( P2(v0) ^ SR(SB(hv)) ^ SR(SB(hv')) ).  The seed only enters after C.",
     "000000000000000000000000000000000000000000000000",
     "0100000000000000a803d3a0b6cb85eb1120e4f3a270c9a6", 0xd73a9a3d941e7ec7ull, 0x4c6ff29ce0549cddull},
    {"15-byte vs 16-byte cross-length pair (L = 2 words), key-free",
     "len <= 16: C(m) = zero-pad(m) + len, added to every byte mod 256, so\n"
     "   0^15 -> 0x0f^16 and 0xff^16 -> 0x0f^16: every message shorter than 16 bytes has a 16-byte twin.",
     "000000000000000000000000000000",
     "ffffffffffffffffffffffffffffffff", 0xf556ecbfcbfee3adull, 0x43ec3783791d0fb8ull},
};

static size_t unhex(const char *s, uint8_t *o) {
    size_t n = strlen(s) / 2;
    for (size_t i = 0; i < n; i++) { unsigned v; sscanf(s + 2*i, "%2x", &v); o[i] = (uint8_t)v; }
    return n;
}
static void print_hex(const char *tag, const uint8_t *m, size_t n) {
    printf("  %s (%2zu bytes) = ", tag, n); for (size_t i = 0; i < n; i++) printf("%02x", m[i]); printf("\n");
}
static void print_blk(const uint8_t *b) { for (int i = 0; i < 16; i++) printf("%02x", b[i]); }

/* constant-time re-derivation of m' from m for the two mechanisms above */
static void derive(int idx, const uint8_t *m, size_t l1, size_t l2, uint8_t *out) {
    if (idx == 0) {                                   /* 24 bytes: hv = m[0:8], v0 = m[8:24] */
        memcpy(out, m, l1); out[0] ^= 0x01;
        blk hv = get_partial(m, 8), hv2 = get_partial(out, 8);
        blk w = aesenc(aesenc(ld(m + 8), key(0)), key(1));                 /* P2(v0) */
        w = bxor(w, bxor(p_aesenclast(hv, zero()), p_aesenclast(hv2, zero())));
        blk v = inv_aesenc(inv_aesenc(w, key(1)), key(0));                 /* P2^-1 */
        memcpy(out + 8, v.b, 16);
    } else {                                          /* k < 16 bytes -> 16-byte twin */
        for (size_t i = 0; i < l2; i++) out[i] = (uint8_t)((i < l1 ? m[i] : 0) + (uint8_t)l1 - 16);
    }
}

static int run_pair(int idx, uint64_t N) {
    const pair_t *P = &PAIRS[idx];
    uint8_t m[64], m2[64], d[64]; size_t l1 = unhex(P->m_hex, m), l2 = unhex(P->m2_hex, m2);
    printf("\nPair %d: %s\n   mechanism: %s\n", idx + 1, P->name, P->mech);
    print_hex("m ", m, l1); print_hex("m'", m2, l2);
    derive(idx, m, l1, l2, d);
    int derived_ok = memcmp(d, m2, l2) == 0;
    printf("  re-deriving m' from m (constant time, no search): %s\n", derived_ok ? "matches the published hex" : "MISMATCH");
    blk c1 = compress_all(m, l1), c2 = compress_all(m2, l2);
    int c_ok = memcmp(c1.b, c2.b, 16) == 0;
    printf("  compress_all(m) = "); print_blk(c1.b); printf("\n  compress_all(m')= "); print_blk(c2.b);
    printf("  -> %s\n", c_ok ? "EQUAL (the seed is applied after this point, so every seed collides)" : "DIFFERENT");

    uint64_t col64 = 0, col128 = 0, first_seed = 0, first_non = 0; int have_first = 0, have_non = 0; blk fh1 = zero(), fh2 = zero();
    for (uint64_t i = 0; i < N; i++) {
        uint64_t seed = rng_next();
        blk h1 = gxhash128(m, l1, seed), h2 = gxhash128(m2, l2, seed);
        int eq64 = memcmp(h1.b, h2.b, 8) == 0, eq128 = memcmp(h1.b, h2.b, 16) == 0;
        col64 += (uint64_t)eq64; col128 += (uint64_t)eq128;
        if (eq64 && !have_first) { first_seed = seed; fh1 = h1; fh2 = h2; have_first = 1; }
        if (!eq64 && !have_non) { first_non = seed; have_non = 1; }
    }
    double r64 = (double)col64 / (double)N, r128 = (double)col128 / (double)N;
    printf("  random seeds: N = %llu (2^%.0f)\n", (unsigned long long)N, log2((double)N));
    printf("    gxhash_64 : collisions = %llu, rate = %.6f, log2 rate = %s%.3f\n", (unsigned long long)col64, r64, col64 ? "" : "< ", col64 ? log2(r64) : -log2((double)N));
    printf("    gxhash 128: collisions = %llu, rate = %.6f, log2 rate = %s%.3f\n", (unsigned long long)col128, r128, col128 ? "" : "< ", col128 ? log2(r128) : -log2((double)N));
    if (have_non) printf("    first NON-colliding seed: 0x%016llx\n", (unsigned long long)first_non);
    if (have_first) {
        printf("  first random colliding seed 0x%016llx: gxhash_64(m) = %016llx  gxhash_64(m') = %016llx\n",
               (unsigned long long)first_seed, (unsigned long long)low64(fh1), (unsigned long long)low64(fh2));
    }
    uint64_t es = P->example_seed; blk e1 = gxhash128(m, l1, es), e2 = gxhash128(m2, l2, es);
    printf("  published example seed 0x%016llx:\n    gxhash_64(m)  = %016llx   gxhash(m)  = ", (unsigned long long)es, (unsigned long long)low64(e1)); print_blk(e1.b);
    printf("\n    gxhash_64(m') = %016llx   gxhash(m') = ", (unsigned long long)low64(e2)); print_blk(e2.b);
    int ex_collide = memcmp(e1.b, e2.b, 16) == 0, ex_value = low64(e1) == P->example_hash;
    printf("\n    -> %s; published gxhash_64 value %016llx %s\n", ex_collide ? "COLLIDE (64 and 128 bit)" : "differ",
           (unsigned long long)P->example_hash, ex_value ? "reproduced" : "NOT reproduced");
    return derived_ok && c_ok && col64 == N && col128 == N && ex_collide && ex_value;
}

int main(int argc, char **argv) {
    char *end = NULL;
    int log2n = argc > 1 ? (int)strtol(argv[1], &end, 10) : 24;
    if (argc > 1 && (end == argv[1] || *end)) log2n = -1;                          /* non-numeric: usage */
    uint64_t master = argc > 2 ? strtoull(argv[2], &end, 0) : 0x676878617368ull;   /* "gxhash" */
    if (argc > 2 && (end == argv[2] || *end)) log2n = -1;
    if (log2n < 0 || log2n > 40) { fprintf(stderr, "usage: %s [log2 seeds (0..40), default 24] [rng master seed]\n", argv[0]); return 2; }
    uint64_t N = 1ull << log2n;
    init_inv_sbox();
    { uint64_t s = master; for (int i = 0; i < 4; i++) RS[i] = splitmix64(&s); }

    printf("gxhash / gxhash_64 key-free collision check  (impl = %s)\n", GX_IMPL);
    printf("hash: ogxd/gxhash (v3 algorithm, commit 55bde47) as ported in SMHasher3 hashes/gxhash.cpp\n");

    /* 1. validation */
    uint32_t v64 = smhasher3_verification(8), v128 = smhasher3_verification(16);
    printf("validation: SMHasher3 verification gxhash_64 = 0x%08X (expect 0x48F84240) %s, gxhash = 0x%08X (expect 0x64A77B47) %s\n",
           v64, v64 == 0x48F84240u ? "OK" : "FAIL", v128, v128 == 0x64A77B47u ? "OK" : "FAIL");
    if (v64 != 0x48F84240u || v128 != 0x64A77B47u) { fprintf(stderr, "ABORT: implementation does not reproduce the SMHasher3 verification values\n"); return 1; }
    { uint64_t s = 1; int bad = 0;
      for (int t = 0; t < 4096; t++) {
          blk a, k; for (int i = 0; i < 16; i++) { a.b[i] = (uint8_t)splitmix64(&s); k.b[i] = (uint8_t)(splitmix64(&s) >> 8); }
          blk y = aesenc(a, k), yp = p_aesenc(a, k), z = aesenclast(a, k), zp = p_aesenclast(a, k);
          if (memcmp(y.b, yp.b, 16) || memcmp(z.b, zp.b, 16) || memcmp(inv_aesenc(yp, k).b, a.b, 16)) bad++;
      }
      printf("validation: %s round vs portable table round, and inverse round: %d mismatches / 4096\n", GX_IMPL, bad);
      if (bad) { fprintf(stderr, "ABORT: AES round mismatch\n"); return 1; } }
    { uint8_t m[24] = {0}, m2[24]; unhex(PAIRS[0].m2_hex, m2);
      uint64_t a = gxhash64(m, 24, 0x0123456789abcdefull), b = gxhash64(m2, 24, 0x0123456789abcdefull);
      printf("validation: pair 1 at seed 0x0123456789abcdef: gxhash_64 = %016llx / %016llx (published 69ccb8a053812a7b) %s\n",
             (unsigned long long)a, (unsigned long long)b, (a == 0x69ccb8a053812a7bull && b == a) ? "OK" : "FAIL");
      if (a != 0x69ccb8a053812a7bull || b != a) { fprintf(stderr, "ABORT: published test value not reproduced\n"); return 1; } }

    /* 2. the pairs */
    int ok = 1;
    for (int i = 0; i < 2; i++) ok &= run_pair(i, N);
    printf("\n%s\n", ok ? "ALL CHECKS PASSED: both pairs collide for every sampled seed on both outputs."
                        : "SOME CHECK FAILED");
    return ok ? 0 : 1;
}
