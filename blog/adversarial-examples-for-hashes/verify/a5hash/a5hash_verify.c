/*
 * a5hash_verify.c -- reader-runnable check of the a5hash seed-collision pairs.
 *
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2026 Thomas Dybdahl Ahle
 *
 * The hash implementation below was written from the reference:
 *   a5hash v5.21 -- Copyright (c) 2025 Aleksey Vaneev, MIT, https://github.com/avaneev/a5hash
 *   as registered in SMHasher3 (hashes/a5hash.cpp, Copyright (C) 2021-2025 Frank J. T. Wojcik, MIT).
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 * associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions: The above copyright notice and this
 * permission notice shall be included in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
 * NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Build:  cc -O2 -std=c11 -o a5hash_verify a5hash_verify.c -lm
 * Run:    ./a5hash_verify [log2 N] [rng seed]
 *         (N random seeds per pair, default 2^24; the RNG seed defaults to 0xA5A5A5A5C0FFEE00,
 *          and pair p uses rng seed + its message length so the pairs get distinct streams)
 *
 * What it does, in order:
 *   1. validates the implementation against the SMHasher3 verification values of the exact
 *      variants used ("a5hash" 64-bit: 0xADDE79B3, "a5hash_128": 0x89406B11) and aborts on mismatch;
 *   2. for each published pair: prints the pair, hashes both messages under N uniformly random
 *      seeds (own xoshiro256** / splitmix64), prints collisions/N and the log2 rate;
 *   3. for the weak-seed pairs: enumerates the stated seed class EXACTLY (2-adic lifting over the
 *      seed bits, cross-checked by brute force on the low 24 bits), hashes both messages under
 *      every class member, prints the conditional rate and the class density;
 *   4. prints one explicit colliding seed with both hash values.
 */
#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------------------------------------ */
/* a5hash v5.21, 64-bit ("a5hash") and 128-bit ("a5hash_128"), little-endian canonical.        */
#define V01 UINT64_C(0x5555555555555555)
#define V10 UINT64_C(0xAAAAAAAAAAAAAAAA)
#define K1  UINT64_C(0x243F6A8885A308D3)   /* Seed1 init (mantissa bits of pi)          */
#define K2  UINT64_C(0x452821E638D01377)   /* Seed2 init                                */
#define K3  UINT64_C(0xA4093822299F31D0)   /* Seed3 init (128-bit variant, NOT seeded)  */
#define K4  UINT64_C(0xC0AC29B7C97C50DD)   /* Seed4 init (128-bit variant, NOT seeded)  */

static uint32_t lu32(const uint8_t *p) {
    return (uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24;
}
static uint64_t lu64(const uint8_t *p) { return (uint64_t)lu32(p) | (uint64_t)lu32(p + 4) << 32; }
static uint64_t w64(const uint8_t *p, const uint8_t *q) { return (uint64_t)lu32(p) << 32 | lu32(q); }

static void umul128(uint64_t u, uint64_t v, uint64_t *lo, uint64_t *hi) {
#if defined(__SIZEOF_INT128__) && !defined(A5_PORTABLE_MUL)
    unsigned __int128 r = (unsigned __int128)u * v;
    *lo = (uint64_t)r; *hi = (uint64_t)(r >> 64);
#else /* portable 32x32 schoolbook (forced with -DA5_PORTABLE_MUL) */
    uint64_t u0 = (uint32_t)u, u1 = u >> 32, v0 = (uint32_t)v, v1 = v >> 32;
    uint64_t p00 = u0 * v0, p01 = u0 * v1, p10 = u1 * v0, p11 = u1 * v1;
    uint64_t mid = (p00 >> 32) + (uint32_t)p01 + (uint32_t)p10;
    *lo = (mid << 32) | (uint32_t)p00;
    *hi = p11 + (p01 >> 32) + (p10 >> 32) + (mid >> 32);
#endif
}

static uint64_t a5hash64(const uint8_t *m, size_t len, uint64_t seed) {
    uint64_t v01 = V01, v10 = V10, s1 = K1 ^ len, s2 = K2 ^ len;
    umul128(s2 ^ (seed & v10), s1 ^ (seed & v01), &s1, &s2);           /* (s1_0, s2_0) */
    if (len > 16) {
        v01 ^= s1; v10 ^= s2;
        do {                                                            /* 16-byte blocks */
            umul128((w64(m, m + 4)) ^ s1, (w64(m + 8, m + 12)) ^ s2, &s1, &s2);
            len -= 16; m += 16; s1 += v01; s2 += v10;
        } while (len > 16);
    }
    if (len > 3) {                                                      /* 4..16-byte tail */
        const uint8_t *m4 = m + len - 4; size_t mo = len >> 3;
        s1 ^= w64(m, m4);
        s2 ^= w64(m + mo * 4, m4 - mo * 4);
    } else if (len > 0) {
        s1 ^= m[0];
        if (len > 1) s1 ^= (uint64_t)m[1] << 8;
        if (len > 2) s1 ^= (uint64_t)m[2] << 16;
    }
    umul128(s1, s2, &s1, &s2);
    umul128(s1 ^ v01, s2, &s1, &s2);
    return s1 ^ s2;
}

static void a5hash128(const uint8_t *m, size_t len, uint64_t seed, uint64_t *rl, uint64_t *rh) {
    uint64_t v01 = V01, v10 = V10, s1 = K1 ^ len, s2 = K2 ^ len, s3 = K3, s4 = K4, a, b, c, d;
    umul128(s2 ^ (seed & v10), s1 ^ (seed & v01), &s1, &s2);
    if (len < 17) {
        if (len > 3) {
            const uint8_t *m4 = m + len - 4; size_t mo = len >> 3;
            a = w64(m, m4); b = w64(m + mo * 4, m4 - mo * 4);
        } else {
            a = 0; b = 0;
            if (len > 0) a = m[0];
            if (len > 1) a |= (uint64_t)m[1] << 8;
            if (len > 2) a |= (uint64_t)m[2] << 16;
        }
    } else {
        if (len < 33) {
            a = w64(m, m + 4); b = w64(m + 8, m + 12);
            c = w64(m + len - 16, m + len - 12); d = w64(m + len - 8, m + len - 4);
            umul128(c + s3, d + s4, &s3, &s4);                          /* public constants only */
        } else {
            v01 ^= s1; v10 ^= s2;
            if (len > 64) {
                uint64_t s5 = UINT64_C(0x082EFA98EC4E6C89), s6 = UINT64_C(0x3F84D5B5B5470917);
                uint64_t s7 = UINT64_C(0x13198A2E03707344), s8 = UINT64_C(0xBE5466CF34E90C6C);
                do {
                    uint64_t t1 = s1, t3 = s3, t5 = s5;
                    umul128(lu64(m) + s1, lu64(m + 32) + s2, &s1, &s2);       s1 += v01; s2 += s8;
                    umul128(lu64(m + 8) + s3, lu64(m + 40) + s4, &s3, &s4);   s3 += t1;  s4 += v10;
                    umul128(lu64(m + 16) + s5, lu64(m + 48) + s6, &s5, &s6);
                    umul128(lu64(m + 24) + s7, lu64(m + 56) + s8, &s7, &s8);
                    len -= 64; m += 64; s5 += t3; s6 += v10; s7 += t5; s8 += v10;
                } while (len > 64);
                s1 ^= s5; s2 ^= s6; s3 ^= s7; s4 ^= s8;
            }
            if (len > 32) {
                uint64_t t1 = s1;
                umul128(lu64(m) + s1, lu64(m + 8) + s2, &s1, &s2);    s1 += v01; s2 += s4;
                umul128(lu64(m + 16) + s3, lu64(m + 24) + s4, &s3, &s4);
                len -= 32; m += 32; s3 += t1; s4 += v10;
            }
            a = lu64(m + len - 16); b = lu64(m + len - 8);
            if (len >= 17) { c = lu64(m + len - 32); d = lu64(m + len - 24); umul128(c + s3, d + s4, &s3, &s4); }
        }
        s1 ^= s3; s2 ^= s4;
    }
    umul128(a + s1, b + s2, &s1, &s2);
    umul128(v01 ^ s1, s2, &a, &b);
    *rl = a ^ b;
    umul128(s1 ^ s3, s2 ^ s4, &s3, &s4);
    *rh = s3 ^ s4;
}

/* ------------------------------------------------------------------------------------------ */
/* SMHasher3 verification value (lib/Hashinfo.cpp _ComputedVerifyImpl): hash keys of length     */
/* 0..255, key i = bytes 0..i-1, seed 256-i; concatenate the little-endian outputs; hash that   */
/* buffer with seed 0; take the first 4 bytes little-endian.                                    */
static void put64(uint8_t *p, uint64_t v) { for (int i = 0; i < 8; i++) p[i] = (uint8_t)(v >> (8 * i)); }
static void hash_le(int bits, const uint8_t *m, size_t len, uint64_t seed, uint8_t *out) {
    uint64_t lo, hi;
    if (bits == 64) { put64(out, a5hash64(m, len, seed)); return; }
    a5hash128(m, len, seed, &lo, &hi); put64(out, lo); put64(out + 8, hi);
}
static uint32_t smhasher3_verification(int bits) {
    int hb = bits / 8; uint8_t key[256], buf[256 * 16], out[16];
    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < i; j++) key[j] = (uint8_t)j;
        hash_le(bits, key, (size_t)i, (uint64_t)(256 - i), buf + i * hb);
    }
    hash_le(bits, buf, (size_t)256 * hb, 0, out);
    return lu32(out);
}

/* ------------------------------------------------------------------------------------------ */
/* RNG: splitmix64 -> xoshiro256**                                                              */
static uint64_t splitmix64(uint64_t *x) {
    uint64_t z = (*x += UINT64_C(0x9E3779B97F4A7C15));
    z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
    z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
    return z ^ (z >> 31);
}
static uint64_t rng_s[4];
static uint64_t rotl(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
static uint64_t rng_base = UINT64_C(0xA5A5A5A5C0FFEE00);   /* argv[2] overrides */
static void rng_seed(uint64_t x) { for (int i = 0; i < 4; i++) rng_s[i] = splitmix64(&x); }
static uint64_t rng_next(void) {
    uint64_t r = rotl(rng_s[1] * 5, 7) * 9, t = rng_s[1] << 17;
    rng_s[2] ^= rng_s[0]; rng_s[3] ^= rng_s[1]; rng_s[1] ^= rng_s[2]; rng_s[0] ^= rng_s[3];
    rng_s[2] ^= t; rng_s[3] = rotl(rng_s[3], 45);
    return r;
}

/* ------------------------------------------------------------------------------------------ */
/* Exact enumeration of the weak-seed class { seed : s1_0(seed) == T } for a given length.      */
/* s1_0 = lo64( (K2^len ^ (seed & V10)) * (K1^len ^ (seed & V01)) ).  Bit i of a product        */
/* depends only on bits 0..i of the operands, hence only on seed bits 0..i: fix the seed one    */
/* bit at a time from bit 0 up and prune every prefix whose low i+1 product bits differ from T. */
/* Every seed in the class is found exactly once (2-adic lifting); the level-24 count is        */
/* cross-checked against brute force over all 2^24 low-bit residues.                            */
static uint64_t *cls; static size_t ncls, capcls, level24;
static void lift(uint64_t P1, uint64_t P2, uint64_t T, uint64_t s, int i) {
    if (i == 24) level24++;
    if (i == 64) {
        if (ncls == capcls) { capcls = capcls ? 2 * capcls : 4096; cls = realloc(cls, capcls * sizeof *cls); }
        cls[ncls++] = s; return;
    }
    uint64_t mask = (i == 63) ? ~UINT64_C(0) : (UINT64_C(1) << (i + 1)) - 1;
    for (int b = 0; b < 2; b++) {
        uint64_t t = s | ((uint64_t)b << i);
        if (((((P2 ^ (t & V10)) * (P1 ^ (t & V01))) ^ T) & mask) == 0) lift(P1, P2, T, t, i + 1);
    }
}
static uint64_t s1_init(size_t len, uint64_t seed) {
    uint64_t lo, hi; umul128((K2 ^ len) ^ (seed & V10), (K1 ^ len) ^ (seed & V01), &lo, &hi); return lo;
}

/* ------------------------------------------------------------------------------------------ */
typedef struct {
    const char *title, *mechanism;
    int bits;                         /* 64: "a5hash"; 128: "a5hash_128" (full 128-bit output) */
    size_t len;
    const char *m_hex, *m2_hex;       /* verbatim from the published record */
    int ntargets; uint64_t target[2]; /* weak-seed class: s1_0(seed) in {target[0..ntargets-1]} */
    uint64_t example_seed;
} pair_t;

static const pair_t PAIRS[] = {
    { "pair 1: a5hash (64-bit), len 23 -- weak-seed class, dead first block",
      "seeds with s1_0(seed) == W0 = 0xf01bbe7047b3e000 make the first block's operand W0 ^ s1_0 == 0,\n"
      "  so the 128-bit block product is 0 and the state becomes (val01, val10) whatever W1 (bytes 8..15) is:\n"
      "  the pair differs only in byte 8.  The class was enumerated exhaustively over all 2^64 seeds.",
      64, 23, "70be1bf000e0b34700000000000000000000000000000000",
              "70be1bf000e0b34701000000000000000000000000000000",
      1, { UINT64_C(0xf01bbe7047b3e000), 0 }, UINT64_C(0x016c190d78e85d64) },
    { "pair 2: a5hash (64-bit), len 8 -- weak-seed class, dead tail multiply (both hashes are 0)",
      "seeds with s1_0(seed) == A = 0x1248299ed31fa942 (or A^1) zero the tail operand s1 ^ A, so\n"
      "  umul128(s1, s2) = (0, 0) and the output is 0 whatever the other word B is; the flipped byte 4 gives\n"
      "  s1 = 1 for the other message, whose product then has hi == 0 and also hashes to 0.",
      64, 8, "9e29481242a91fd3", "9e29481243a91fd3",
      2, { UINT64_C(0x1248299ed31fa942), UINT64_C(0x1248299ed31fa943) }, UINT64_C(0x7b9e6e3ec8b1997d) },
    { "pair 3: a5hash_128 (full 128-bit output), len 25 -- key-free, public-constant product",
      "on the 17..32-byte path the last 16 bytes (c, d) enter only through umul128(c + K3, d + K4) with the\n"
      "  PUBLIC constants K3 = 0xA4093822299F31D0, K4 = 0xC0AC29B7C97C50DD (Seed3/Seed4 are never seeded).\n"
      "  The pair sets (c+K3, d+K4) = (1, 1+2^24) vs (1+2^24, 1): equal 128-bit products for every seed.",
      128, 25, "000000000000000000ddc7f65b31ce60d648d6533f24af8337",
               "000000000000000000ddc7f65b31ce60d748d6533f24af8336",
      0, { 0, 0 }, UINT64_C(0xdeadbeefcafef00d) },
};

static size_t unhex(const char *h, uint8_t *out) {
    size_t n = 0;
    for (; h[0] && h[1]; h += 2) { unsigned v; if (sscanf(h, "%2x", &v) != 1) break; out[n++] = (uint8_t)v; }
    return n;
}
static int hashes_equal(const pair_t *p, const uint8_t *m, const uint8_t *m2, uint64_t seed, uint64_t h[4]) {
    if (p->bits == 64) { h[0] = a5hash64(m, p->len, seed); h[2] = a5hash64(m2, p->len, seed); return h[0] == h[2]; }
    a5hash128(m, p->len, seed, &h[0], &h[1]); a5hash128(m2, p->len, seed, &h[2], &h[3]);
    return h[0] == h[2] && h[1] == h[3];
}
static void print_hash(const pair_t *p, const uint64_t *h) {
    if (p->bits == 64) printf("%016" PRIx64, h[0]); else printf("%016" PRIx64 "%016" PRIx64, h[0], h[1]);
}

static void run_pair(const pair_t *p, int log2n) {
    uint8_t m[64], m2[64]; uint64_t h[4];
    size_t n1 = unhex(p->m_hex, m), n2 = unhex(p->m2_hex, m2);
    if (n1 < p->len || n2 < p->len || memcmp(m, m2, p->len) == 0) { fprintf(stderr, "bad pair\n"); exit(2); }
    printf("== %s ==\n", p->title);
    printf("mechanism: %s\n", p->mechanism);
    printf("m  = %s\nm2 = %s\n", p->m_hex, p->m2_hex);
    if (n1 > p->len) printf("(record lists %zu bytes; the hashed length is %zu, the trailing 0x00 is not read)\n", n1, p->len);

    /* uniform random seeds */
    uint64_t N = UINT64_C(1) << log2n, coll = 0, first = 0; int have_first = 0;
    rng_seed(rng_base + p->len);
    for (uint64_t k = 0; k < N; k++) {
        uint64_t seed = rng_next();
        if (hashes_equal(p, m, m2, seed, h)) { if (!have_first) { first = seed; have_first = 1; } coll++; }
    }
    printf("uniform seeds: N = 2^%d, collisions = %" PRIu64 "/%" PRIu64, log2n, coll, N);
    if (coll) printf(", rate = %.6g (2^%.2f)\n", (double)coll / (double)N, log2((double)coll / (double)N));
    else printf(", none observed in 2^%d trials; no population bound\n", log2n);

    /* weak-seed class: exact enumeration */
    double log2eps; size_t L = (p->len + 7) / 8;
    if (p->ntargets) {
        uint64_t P1 = K1 ^ p->len, P2 = K2 ^ p->len, ccoll = 0; size_t total = 0;
        ncls = 0; level24 = 0;
        for (int t = 0; t < p->ntargets; t++) {
            size_t before = ncls, lv = level24; level24 = 0;
            lift(P1, P2, p->target[t], 0, 0);
            uint64_t bf = 0;
            for (uint64_t r = 0; r < (UINT64_C(1) << 24); r++)
                if (((((P2 ^ (r & V10)) * (P1 ^ (r & V01))) ^ p->target[t]) & 0xFFFFFF) == 0) bf++;
            printf("seed class s1_0(seed) == 0x%016" PRIx64 ": %zu seeds (2-adic lift; 24-bit brute-force "
                   "cross-check %" PRIu64 " == %zu %s)\n", p->target[t], ncls - before, bf, level24,
                   bf == level24 ? "OK" : "MISMATCH");
            if (bf != level24) exit(3);
            level24 += lv;
        }
        total = ncls;
        for (size_t k = 0; k < ncls; k++) if (hashes_equal(p, m, m2, cls[k], h)) ccoll++;
        printf("conditional rate: %" PRIu64 "/%zu = %.6f colliding class members\n", ccoll, total, (double)ccoll / (double)total);
        printf("class density:    %zu/2^64 = 2^%.2f\n", total, log2((double)total) - 64.0);
        log2eps = log2((double)ccoll) - 64.0;
        printf("=> collision probability over uniform seeds >= 2^%.2f (this class alone); "
               "L = %zu words -> bits = log2(L/eps) = %.1f\n", log2eps, L, log2((double)L) - log2eps);
        printf("   (2^%d uniform seeds expect %.2g collisions from this class, so 0 above is consistent)\n",
               log2n, (double)N * pow(2.0, log2eps));
    } else {
        printf("seed class: unconditional (every seed collides); L = %zu words -> bits = log2(L/1) = %.1f\n",
               L, log2((double)L));
    }

    /* explicit colliding seed */
    uint64_t ex = p->example_seed; int ok = hashes_equal(p, m, m2, ex, h);
    printf("explicit seed 0x%016" PRIx64 ": h(m) = ", ex); print_hash(p, h);
    printf("  h(m2) = "); print_hash(p, h + 2); printf("  %s", ok ? "COLLIDE" : "DIFFER");
    if (p->ntargets) printf("  (s1_0 = 0x%016" PRIx64 ", class member)", s1_init(p->len, ex));
    printf("\n");
    if (!ok) { fprintf(stderr, "published example seed does not collide\n"); exit(4); }
    if (have_first) printf("first random colliding seed 0x%016" PRIx64 "\n", first);
    if (p->bits == 128) {
        printf("(the 64-bit a5hash of the same pair under this seed differs: %016" PRIx64 " vs %016" PRIx64 ")\n",
               a5hash64(m, p->len, ex), a5hash64(m2, p->len, ex));
    }
    printf("\n");
}

int main(int argc, char **argv) {
    char *end = NULL;
    int log2n = argc > 1 ? (int)strtol(argv[1], &end, 10) : 24;
    if (argc > 1 && (end == argv[1] || *end)) log2n = 0;                     /* non-numeric: usage */
    if (argc > 2) { rng_base = strtoull(argv[2], &end, 0); if (end == argv[2] || *end) log2n = 0; }
    if (log2n < 1 || log2n > 40) { fprintf(stderr, "usage: %s [log2 N (1..40), default 24] [rng seed, default 0xA5A5A5A5C0FFEE00]\n", argv[0]); return 1; }
    printf("a5hash v5.21 (SMHasher3 hashes/a5hash.cpp; https://github.com/avaneev/a5hash) -- seed-collision pairs\n");
    uint32_t v64 = smhasher3_verification(64), v128 = smhasher3_verification(128);
    printf("validation: a5hash      SMHasher3 verification 0x%08" PRIX32 " (expected 0xADDE79B3) %s\n", v64, v64 == 0xADDE79B3 ? "OK" : "FAIL");
    printf("            a5hash_128  SMHasher3 verification 0x%08" PRIX32 " (expected 0x89406B11) %s\n\n", v128, v128 == 0x89406B11 ? "OK" : "FAIL");
    if (v64 != 0xADDE79B3 || v128 != 0x89406B11) { fprintf(stderr, "implementation does not match SMHasher3; aborting\n"); return 1; }
    for (size_t i = 0; i < sizeof PAIRS / sizeof PAIRS[0]; i++) run_pair(&PAIRS[i], log2n);
    free(cls);
    return 0;
}
