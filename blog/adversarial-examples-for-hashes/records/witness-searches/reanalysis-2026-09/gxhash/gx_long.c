/*
 * gxhash implementation section, taken from the post's verify/gxhash-64/gxhash64_verify.c (written from SMHasher3 hashes/gxhash.cpp)
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

/* ===================== longA driver (own code) =====================
 * gxhash v3 long inputs: the compress_8 bulk loop.  Per 128-byte chunk (v0..v7),
 *   a = aesenc(aesenc(aesenc(v0, v2), v4), v6),  b = aesenc(aesenc(aesenc(v1, v3), v5), v7)
 * depend on the chunk alone (no state, no seed) and the chunk enters the lanes only through (a, b).
 * So v0 = E^-1(E^-1(E^-1(a ^ v6) ^ v4) ^ v2) (E = MixColumns.ShiftRows.SubBytes) gives, for ANY
 * (v2, v4, v6), a chunk with the same a; likewise v1 for b.  Any chunk of the compress_8 region can be
 * replaced, whatever the content around it, and compress_all (hence both outputs for every seed) is
 * unchanged.  This is the mechanism of O. Peters, ogxd/gxhash#83 (2024).
 * Usage: ./gx_long [log2 seeds] [rng seed] [log2 family size] */
static uint64_t RS[4], SMS;
static uint64_t sm64(void) { uint64_t z = (SMS += 0x9E3779B97F4A7C15ull); z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull; z = (z ^ (z >> 27)) * 0x94D049BB133111EBull; return z ^ (z >> 31); }
static inline uint64_t rotl_(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
static uint64_t rng64(void) { uint64_t r = rotl_(RS[1] * 5, 7) * 9, t = RS[1] << 17; RS[2] ^= RS[0]; RS[3] ^= RS[1]; RS[1] ^= RS[2]; RS[0] ^= RS[3]; RS[2] ^= t; RS[3] = rotl_(RS[3], 45); return r; }
static void rng_init(uint64_t s) { SMS = s; for (int i = 0; i < 4; i++) RS[i] = sm64(); }
static void fill(uint8_t *p, size_t n) { for (size_t i = 0; i < n; i++) p[i] = (uint8_t)rng64(); }
static void hex(const uint8_t *p, size_t n) { for (size_t i = 0; i < n; i++) printf("%02x", p[i]); }
/* start of the compress_8 region and number of 128-byte chunks, from compress_all/compress_many */
static void region(size_t len, size_t *start, size_t *nchunks) {
    size_t extra = len % 16, e = extra ? extra : 16;
    size_t p = e + 16 + 16 + 16;                 /* hv, then the three v0 blocks (len > 64) */
    size_t rem = len - p, unroll = (rem / 16) / 8;
    *nchunks = unroll; *start = len - unroll * 128;
}
/* replace chunk at c (128 bytes) by one with v2, v3 (and v4..v7) new and v0, v1 solved */
static void twin_chunk(uint8_t *c, const uint8_t newv[6][16]) {
    blk v[8]; for (int i = 0; i < 8; i++) v[i] = ld(c + 16 * i);
    blk a = p_aesenc(p_aesenc(p_aesenc(v[0], v[2]), v[4]), v[6]);
    blk b = p_aesenc(p_aesenc(p_aesenc(v[1], v[3]), v[5]), v[7]);
    for (int i = 0; i < 6; i++) memcpy(v[2 + i].b, newv[i], 16);
    blk v0 = inv_aesenc(inv_aesenc(inv_aesenc(a, v[6]), v[4]), v[2]);
    blk v1 = inv_aesenc(inv_aesenc(inv_aesenc(b, v[7]), v[5]), v[3]);
    memcpy(c, v0.b, 16); memcpy(c + 16, v1.b, 16);
    for (int i = 2; i < 8; i++) memcpy(c + 16 * i, v[i].b, 16);
}
static int eqb(blk x, blk y) { return !memcmp(x.b, y.b, 16); }
int main(int argc, char **argv) {
    unsigned logN = argc > 1 ? (unsigned)atoi(argv[1]) : 12; uint64_t rs = argc > 2 ? strtoull(argv[2], 0, 0) : 0x4c6f6e6742ull;
    unsigned logK = argc > 3 ? (unsigned)atoi(argv[3]) : 16;
    int fail = 0;
    init_inv_sbox(); rng_init(rs);
    uint32_t v64 = smhasher3_verification(8), v128 = smhasher3_verification(16);
    printf("impl %s; SMHasher3 verification gxhash_64 %08X (expect 48F84240) gxhash %08X (expect 64A77B47) %s\n", GX_IMPL, v64, v128,
           (v64 == 0x48F84240u && v128 == 0x64A77B47u) ? "OK" : "MISMATCH");
    if (v64 != 0x48F84240u || v128 != 0x64A77B47u) return 1;
    { int bad = 0; for (int t = 0; t < 4096; t++) { blk x, k; fill(x.b, 16); fill(k.b, 16);
        blk h = aesenc(x, k), p = p_aesenc(x, k); if (!eqb(h, p) || !eqb(inv_aesenc(p, k), x)) bad++; }
      printf("round cross-check (hardware/table/inverse): %d mismatches / 4096\n", bad); if (bad) return 1; }
    const size_t lens[] = { 192, 256, 1000, 1024, 65536, 65536 + 13, 1048576, 1048576 + 7 };
    for (size_t li = 0; li < sizeof lens / sizeof lens[0]; li++) {
        size_t n = lens[li], st, nc; region(n, &st, &nc);
        uint8_t *A = malloc(n), *B = malloc(n); fill(A, n); memcpy(B, A, n);
        size_t ci = nc > 1 ? rng64() % nc : 0, off = st + 128 * ci;
        uint8_t nv[6][16]; fill(&nv[0][0], 96); twin_chunk(B + off, nv);
        blk ca = compress_all(A, n), cb = compress_all(B, n);
        uint64_t N = 1ull << (n > 100000 ? (logN > 8 ? 8 : logN) : logN), h64 = 0, h128 = 0, ex = 0;
        for (uint64_t t = 0; t < N; t++) { uint64_t sd = rng64(); blk ha = gxhash128(A, n, sd), hb = gxhash128(B, n, sd);
            if (low64(ha) == low64(hb)) h64++; if (eqb(ha, hb)) { h128++; if (h128 == 1) ex = sd; } }
        size_t ndiff = 0; for (size_t i = 0; i < n; i++) ndiff += A[i] != B[i];
        printf("len %8zu (L=%6zu words): compress_8 region %zu..%zu (%zu chunks), replaced chunk %zu at byte %zu (%zu bytes differ);"
               " compress_all equal: %s; seeds: 64-bit %llu/%llu, 128-bit %llu/%llu",
               n, (n + 7) / 8, st, n - 1, nc, ci, off, ndiff, eqb(ca, cb) ? "yes" : "NO", (unsigned long long)h64, (unsigned long long)N,
               (unsigned long long)h128, (unsigned long long)N);
        if (h128) { blk h = gxhash128(A, n, ex); printf("  e.g. seed %016llx -> gxhash ", (unsigned long long)ex); hex(h.b, 16); }
        printf("\n");
        if (n == 256) { printf("  m  = "); hex(A, n); printf("\n  m' = "); hex(B, n); printf("\n"); }
        if (!eqb(ca, cb) || h128 != N) fail = 1;
        free(A); free(B);
    }
    /* multicollision family: 2^logK members at 1000 bytes (not a multiple of 16), random content;
       members differ in one chunk (v2 = member index, v0 solved) -- and, to show slots compose, the
       second half of the members also replace a second chunk */
    {
        size_t n = 1000, st, nc; region(n, &st, &nc); size_t K = (size_t)1 << logK;
        uint8_t *base = malloc(n); fill(base, n);
        uint8_t *F = malloc(K * n); uint8_t rest[5][16]; fill(&rest[0][0], 80);
        for (size_t i = 0; i < K; i++) {
            uint8_t *m = F + i * n; memcpy(m, base, n);
            uint8_t nv[6][16]; memset(nv[0], 0, 16); memcpy(nv[0], &i, sizeof i); memcpy(nv[1], rest, 80);
            twin_chunk(m + st + 128 * 1, nv);
            if (i & 1) { uint8_t w[6][16]; memset(w, 0x5a, sizeof w); twin_chunk(m + st + 128 * 4, w); }
        }
        size_t distinct = 1; /* members differ in v2 of chunk 1 by construction; check pairwise with a sort-free test */
        for (size_t i = 1; i < K; i++) distinct += memcmp(F + i * n, F + (i - 1) * n, n) != 0;
        blk c0 = compress_all(F, n); size_t same = 0; for (size_t i = 0; i < K; i++) same += eqb(compress_all(F + i * n, n), c0);
        uint64_t N = 1ull << (logN > 10 ? 10 : logN), all = 0, ex = 0; blk exh;
        for (uint64_t t = 0; t < N; t++) { uint64_t sd = rng64(); blk h0 = gxhash128(F, n, sd); size_t ok = 1;
            for (size_t i = 1; i < K; i++) ok += eqb(gxhash128(F + i * n, n, sd), h0);
            if (ok == K) { all++; if (all == 1) { ex = sd; exh = h0; } } }
        printf("family: %zu members of %zu bytes (compress_8 chunks at %zu + 128j, members = chunk 1 re-solved with v2 = index,"
               " odd members also re-solve chunk 4); adjacent members distinct: %zu/%zu; compress_all equal: %zu/%zu;"
               " seeds with ALL members equal on 128 bits: %llu/%llu\n", K, n, st, distinct, K, same, K, (unsigned long long)all, (unsigned long long)N);
        if (all) { printf("  e.g. seed %016llx -> ", (unsigned long long)ex); hex(exh.b, 16); printf("\n  member[0] chunk 1 = "); hex(F + st + 128, 128);
                   printf("\n  member[%zu] chunk 1 = ", K - 1); hex(F + (K - 1) * n + st + 128, 128); printf("\n"); }
        if (all != N || same != K) fail = 1;
    }
    printf(fail ? "RESULT: FAIL\n" : "RESULT: PASS\n"); return fail;
}
