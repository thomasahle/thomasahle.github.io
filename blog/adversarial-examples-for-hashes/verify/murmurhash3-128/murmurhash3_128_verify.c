/*
 * murmurhash3_128_verify.c -- seed-independent (key-free) collisions in
 * MurmurHash3_x64_128, reproducible by anyone with a C compiler.
 *
 * Copyright (c) 2026 Thomas Dybdahl Ahle
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions: the above copyright
 * notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.  THE SOFTWARE IS PROVIDED "AS IS",
 * WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
 *
 * MurmurHash3 was written by Austin Appleby and placed in the public domain.
 * The implementation below is written from his reference MurmurHash3.cpp
 * (https://github.com/aappleby/smhasher, MurmurHash3_x64_128) with the
 * SMHasher3 seeding convention h1 = h2 = (uint32_t)seed, i.e. the SMHasher3
 * hash "MurmurHash3_128" (hashes/murmurhash3.cpp, verification_LE 0x6384BA69).
 *
 * What this program does
 *   1. validates the implementation by recomputing the SMHasher3 verification
 *      value with SMHasher3's exact procedure (lib/Hashinfo.cpp,
 *      _ComputedVerifyImpl) and aborts if it is not 0x6384BA69;
 *   2. prints two published message pairs, hashes both members under N
 *      uniformly random 32-bit seeds (default N = 2^24) and reports the
 *      collision rate, next to a control pair that must not collide;
 *   3. traces the state difference through the compression function so the
 *      mechanism (a difference parked in bit 63) is visible;
 *   4. prints explicit colliding seeds with both hash values, and a 4-way
 *      multicollision built from the two-block pair.
 *
 * Build:  cc -std=c11 -O2 -o murmurhash3_128_verify murmurhash3_128_verify.c -lm
 * Run:    ./murmurhash3_128_verify [log2_seeds] [rng_seed] [--exhaustive]
 *         defaults: log2_seeds = 24, rng_seed = 1.  --exhaustive additionally
 *         enumerates all 2^32 seeds (one to a few minutes per pair,
 *         single-threaded, machine-dependent).
 */
#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------------------- */
/* MurmurHash3_x64_128, SMHasher3 "MurmurHash3_128" variant                   */
/* ------------------------------------------------------------------------- */

static inline uint64_t rotl64(uint64_t x, int r) { return (x << r) | (x >> (64 - r)); }

static inline uint64_t fmix64(uint64_t k) {
    k ^= k >> 33; k *= UINT64_C(0xff51afd7ed558ccd);
    k ^= k >> 33; k *= UINT64_C(0xc4ceb9fe1a85ec53);
    k ^= k >> 33;
    return k;
}

/* Little-endian load, byte by byte (portable to any host endianness). */
static inline uint64_t load64le(const uint8_t *p) {
    uint64_t v = 0;
    for (int i = 7; i >= 0; i--) v = (v << 8) | p[i];
    return v;
}

static inline void store64le(uint8_t *p, uint64_t v) {
    for (int i = 0; i < 8; i++) p[i] = (uint8_t)(v >> (8 * i));
}

static const uint64_t C1 = UINT64_C(0x87c37b91114253d5);
static const uint64_t C2 = UINT64_C(0x4cf5ad432745937f);

/* The two public message bijections: g1 feeds h1, g2 feeds h2. */
static inline uint64_t g1(uint64_t k) { k *= C1; k = rotl64(k, 31); return k * C2; }
static inline uint64_t g2(uint64_t k) { k *= C2; k = rotl64(k, 33); return k * C1; }

/* One 16-byte block of the compression function. */
static inline void mm3_block(uint64_t *h1, uint64_t *h2, uint64_t k1, uint64_t k2) {
    *h1 ^= g1(k1); *h1 = rotl64(*h1, 27); *h1 += *h2; *h1 = *h1 * 5 + 0x52dce729;
    *h2 ^= g2(k2); *h2 = rotl64(*h2, 31); *h2 += *h1; *h2 = *h2 * 5 + 0x38495ab5;
}

static void murmurhash3_x64_128(const uint8_t *data, size_t len, uint32_t seed, uint8_t out[16]) {
    const size_t nblocks = len / 16;
    uint64_t h1 = seed, h2 = seed;

    for (size_t i = 0; i < nblocks; i++)
        mm3_block(&h1, &h2, load64le(data + 16 * i), load64le(data + 16 * i + 8));

    const uint8_t *tail = data + nblocks * 16;
    const size_t r = len & 15;
    uint64_t k1 = 0, k2 = 0;
    if (r > 8) {                       /* bytes 8..r-1 of the tail feed h2 */
        for (size_t j = 8; j < r; j++) k2 ^= (uint64_t)tail[j] << (8 * (j - 8));
        h2 ^= g2(k2);
    }
    if (r > 0) {                       /* bytes 0..min(r,8)-1 feed h1 */
        size_t m = r < 8 ? r : 8;
        for (size_t j = 0; j < m; j++) k1 ^= (uint64_t)tail[j] << (8 * j);
        h1 ^= g1(k1);
    }

    h1 ^= (uint32_t)len; h2 ^= (uint32_t)len;
    h1 += h2; h2 += h1;
    h1 = fmix64(h1); h2 = fmix64(h2);
    h1 += h2; h2 += h1;
    store64le(out, h1); store64le(out + 8, h2);
}

/* ------------------------------------------------------------------------- */
/* Validation: SMHasher3's verification value, computed its way              */
/* ------------------------------------------------------------------------- */

/* lib/Hashinfo.cpp::_ComputedVerifyImpl: hash the keys {}, {0}, {0,1}, ...,
 * {0..254} (key i has bytes 0..i-1 = 0..i-1) with seed 256-i, concatenate the
 * 256 outputs, hash that with seed 0, and read the first 4 bytes little-endian. */
static uint32_t smhasher3_verification(void) {
    uint8_t key[256] = {0}, hashes[16 * 256], total[16];
    for (int i = 0; i < 256; i++) {
        murmurhash3_x64_128(key, (size_t)i, (uint32_t)(256 - i), hashes + 16 * i);
        key[i] = (uint8_t)i;
    }
    murmurhash3_x64_128(hashes, sizeof hashes, 0, total);
    return (uint32_t)total[0] | (uint32_t)total[1] << 8 | (uint32_t)total[2] << 16 | (uint32_t)total[3] << 24;
}

/* ------------------------------------------------------------------------- */
/* Own RNG: splitmix64 seeding xoshiro256**                                  */
/* ------------------------------------------------------------------------- */

static uint64_t splitmix64(uint64_t *s) {
    uint64_t z = (*s += UINT64_C(0x9e3779b97f4a7c15));
    z = (z ^ (z >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
    z = (z ^ (z >> 27)) * UINT64_C(0x94d049bb133111eb);
    return z ^ (z >> 31);
}

typedef struct { uint64_t s[4]; } rng_t;

static void rng_init(rng_t *r, uint64_t seed) { for (int i = 0; i < 4; i++) r->s[i] = splitmix64(&seed); }

static uint64_t rng_next(rng_t *r) {
    uint64_t *s = r->s;
    const uint64_t result = rotl64(s[1] * 5, 7) * 9, t = s[1] << 17;
    s[2] ^= s[0]; s[3] ^= s[1]; s[1] ^= s[2]; s[0] ^= s[3]; s[2] ^= t; s[3] = rotl64(s[3], 45);
    return result;
}

/* The SMHasher3 variant truncates its seed to 32 bits, so a uniformly random
 * seed is a uniformly random 32-bit value. */
static inline uint32_t rng_seed32(rng_t *r) { return (uint32_t)(rng_next(r) >> 32); }

/* ------------------------------------------------------------------------- */
/* The published pairs                                                        */
/* ------------------------------------------------------------------------- */

typedef struct {
    const char *title, *m_hex, *m2_hex, *mechanism;
} pair_t;

static const pair_t PAIRS[] = {
    { "24-byte pair (one block + 8-byte tail): confirmed worst pair, L = 3 words, bits = log2(3/1) = 1.585",
      "000000000000000000000000000000000000000000000000",
      "60a0fd219e0ef2cd42e098d38ee8728d000000007d4dce82",
      "block 1: g1(k1') = g1(k1) ^ 2^36 and g2(k2') = g2(k2) ^ 2^32 give state difference (2^63, 0) "
      "deterministically (rotl 27 / rotl 31 move the bits to the top; += and *5+c preserve 2^63); "
      "8-byte tail k3 with g1(k3') = g1(k3) ^ 2^63 cancels h1; no k2 tail so h2 untouched" },
    { "32-byte pair (two blocks): the 2012-style two-block universal collision, L = 4 words, bits = 2",
      "0000000000000000000000000000000000000000000000000000000000000000",
      "60a0fd219e0ef2cd000000000000000060a0fd2121c1234b000000c01f8b7776",
      "block 1: g1 diff 2^36 -> (2^63, 2^63); block 2: g1 diff 2^63|2^36, g2 diff 2^63 -> (0, 0); "
      "n such block pairs give 2^n messages colliding for every seed (hash-flooding multicollision)" },
};
#define NPAIRS (sizeof PAIRS / sizeof PAIRS[0])

static size_t unhex(const char *s, uint8_t *out) {
    size_t n = strlen(s) / 2;
    for (size_t i = 0; i < n; i++) {
        unsigned b;
        if (sscanf(s + 2 * i, "%2x", &b) != 1) { fprintf(stderr, "bad hex literal\n"); abort(); }
        out[i] = (uint8_t)b;
    }
    return n;
}

static void print_hex(const char *label, const uint8_t *p, size_t n) {
    printf("%s", label);
    for (size_t i = 0; i < n; i++) printf("%02x", p[i]);
    printf("\n");
}

static void print_log2(uint64_t coll, uint64_t n) {
    if (coll == 0) printf("unestimated (zero observed in 2^%.0f trials)", log2((double)n));
    else printf("2^%.4f", log2((double)coll / (double)n));
}

/* ------------------------------------------------------------------------- */
/* Mechanism trace: XOR difference of the state after every step             */
/* ------------------------------------------------------------------------- */

static void trace_pair(const uint8_t *a, const uint8_t *b, size_t len, uint32_t seed) {
    uint64_t h1 = seed, h2 = seed, h1p = seed, h2p = seed;
    printf("  trace at seed 0x%08" PRIx32 " (state difference h1^h1', h2^h2'):\n", seed);
    for (size_t i = 0; i < len / 16; i++) {
        uint64_t k1 = load64le(a + 16 * i), k2 = load64le(a + 16 * i + 8);
        uint64_t k1p = load64le(b + 16 * i), k2p = load64le(b + 16 * i + 8);
        printf("    block %zu: g1 diff %016" PRIx64 "  g2 diff %016" PRIx64, i + 1, g1(k1) ^ g1(k1p), g2(k2) ^ g2(k2p));
        mm3_block(&h1, &h2, k1, k2); mm3_block(&h1p, &h2p, k1p, k2p);
        printf("  -> state diff (%016" PRIx64 ", %016" PRIx64 ")\n", h1 ^ h1p, h2 ^ h2p);
    }
    if ((len & 15) > 8) {   /* this tracer only handles k1-only tails */
        printf("    tail   : %zu bytes, not traced (the tracer handles tails of at most 8 bytes)\n", len & 15);
    } else if (len & 15) {
        uint64_t k1 = load64le(a + (len & ~(size_t)15)), k1p = load64le(b + (len & ~(size_t)15));
        h1 ^= g1(k1); h1p ^= g1(k1p);
        printf("    tail   : g1 diff %016" PRIx64 "                            -> state diff (%016" PRIx64 ", %016" PRIx64 ")\n",
               g1(k1) ^ g1(k1p), h1 ^ h1p, h2 ^ h2p);
    }
    printf("    same length, same state -> same finalization -> same hash\n");
}

/* ------------------------------------------------------------------------- */
/* Sampling                                                                  */
/* ------------------------------------------------------------------------- */

typedef struct { uint64_t coll; int have; uint32_t seed; } tally_t;

static inline void tally(tally_t *t, const uint8_t *a, const uint8_t *b, size_t len, uint32_t seed) {
    uint8_t ha[16], hb[16];
    murmurhash3_x64_128(a, len, seed, ha);
    murmurhash3_x64_128(b, len, seed, hb);
    if (memcmp(ha, hb, 16) == 0) { t->coll++; if (!t->have) { t->have = 1; t->seed = seed; } }
}

static void show_seed(const char *label, const uint8_t *a, const uint8_t *b, size_t len, uint32_t seed) {
    uint8_t ha[16], hb[16];
    murmurhash3_x64_128(a, len, seed, ha);
    murmurhash3_x64_128(b, len, seed, hb);
    printf("  %s seed 0x%08" PRIx32 ":\n", label, seed);
    print_hex("    H(m)  = ", ha, 16);
    print_hex("    H(m') = ", hb, 16);
    printf("    %s\n", memcmp(ha, hb, 16) == 0 ? "equal" : "DIFFERENT");
}

int main(int argc, char **argv) {
    int log2n = 24, exhaustive = 0;
    uint64_t rng_seed = 1;
    int npos = 0;
    for (int i = 1; i < argc; i++) {
        char *end = NULL;
        if (strcmp(argv[i], "--exhaustive") == 0) exhaustive = 1;
        else if (npos == 0) { log2n = (int)strtol(argv[i], &end, 10); npos++; }
        else if (npos == 1) { rng_seed = strtoull(argv[i], &end, 0); npos++; }
        else end = argv[i];
        if (end && (end == argv[i] || *end)) {   /* non-numeric, misspelled flag, or too many */
            fprintf(stderr, "bad argument '%s'\nusage: %s [log2_seeds] [rng_seed] [--exhaustive]\n", argv[i], argv[0]);
            return 2;
        }
    }
    if (log2n < 0 || log2n > 40) { fprintf(stderr, "log2_seeds out of range\n"); return 2; }
    const uint64_t N = UINT64_C(1) << log2n;

    /* 1. validate the implementation */
    uint32_t v = smhasher3_verification();
    printf("MurmurHash3_x64_128 (SMHasher3 \"MurmurHash3_128\", seed truncated to 32 bits)\n");
    printf("SMHasher3 verification value: computed 0x%08" PRIX32 ", expected 0x6384BA69 -> %s\n",
           v, v == 0x6384BA69u ? "OK" : "MISMATCH");
    if (v != 0x6384BA69u) { fprintf(stderr, "implementation does not match the reference; aborting\n"); abort(); }

    printf("Sampling N = 2^%d uniformly random 32-bit seeds (xoshiro256**, rng seed %" PRIu64 ")\n", log2n, rng_seed);

    uint8_t a[64], b[64], ctrl[64];
    for (size_t p = 0; p < NPAIRS; p++) {
        const pair_t *P = &PAIRS[p];
        size_t la = unhex(P->m_hex, a), lb = unhex(P->m2_hex, b);
        if (la != lb) { fprintf(stderr, "pair %zu: length mismatch\n", p); return 1; }
        memcpy(ctrl, b, lb); ctrl[lb - 1] ^= 1;    /* control: m' with its last bit flipped */

        printf("\n== Pair %zu: %s ==\n", p + 1, P->title);
        printf("  length %zu bytes each, condition on the seed: none (key-free)\n", la);
        print_hex("  m  = ", a, la);
        print_hex("  m' = ", b, lb);
        printf("  mechanism: %s\n", P->mechanism);
        trace_pair(a, b, la, 0x00000000u);
        trace_pair(a, b, la, 0x9e3779b9u);

        rng_t rng; rng_init(&rng, rng_seed + p);
        tally_t t = {0}, tc = {0};
        for (uint64_t i = 0; i < N; i++) {
            uint32_t s = rng_seed32(&rng);
            tally(&t, a, b, la, s);
            tally(&tc, a, ctrl, la, s);
        }
        printf("  random seeds : collisions/N = %" PRIu64 "/%" PRIu64 " = %.6f, rate ", t.coll, N, (double)t.coll / (double)N);
        print_log2(t.coll, N); printf("\n");
        printf("  control pair : collisions/N = %" PRIu64 "/%" PRIu64 ", rate ", tc.coll, N);
        print_log2(tc.coll, N); printf("  (m' with last bit flipped)\n");

        show_seed("published example", a, b, la, 0x00000000u);
        if (t.have) show_seed("first sampled colliding", a, b, la, t.seed);

        if (exhaustive) {
            tally_t te = {0};
            for (uint64_t s = 0; s < (UINT64_C(1) << 32); s++) tally(&te, a, b, la, (uint32_t)s);
            printf("  exhaustive   : collisions = %" PRIu64 " of 4294967296 seeds (2^32), rate ", te.coll);
            print_log2(te.coll, UINT64_C(1) << 32); printf("\n");
        }
    }

    /* 4-way multicollision: after the two-block pair the states agree, so any
     * concatenation of the two 32-byte blocks (AA, AB, BA, BB) collides too. */
    {
        size_t l = unhex(PAIRS[1].m_hex, a); unhex(PAIRS[1].m2_hex, b);
        uint8_t msg[4][64];
        for (int i = 0; i < 4; i++) { memcpy(msg[i], (i & 2) ? b : a, l); memcpy(msg[i] + l, (i & 1) ? b : a, l); }
        uint64_t n = N < (UINT64_C(1) << 20) ? N : (UINT64_C(1) << 20), all4 = 0;
        rng_t rng; rng_init(&rng, rng_seed + 99);
        uint32_t ex = 0; uint8_t exh[4][16];
        for (uint64_t i = 0; i < n; i++) {
            uint32_t s = rng_seed32(&rng); uint8_t h[4][16]; int same = 1;
            for (int k = 0; k < 4; k++) { murmurhash3_x64_128(msg[k], 2 * l, s, h[k]); if (k && memcmp(h[0], h[k], 16)) same = 0; }
            if (same) { if (all4 == 0) { ex = s; memcpy(exh, h, sizeof exh); } all4++; }
        }
        printf("\n== Multicollision: the four 64-byte messages {m,m'} x {m,m'} of pair 2 ==\n");
        printf("  seeds where all four hashes agree: %" PRIu64 "/%" PRIu64 "\n", all4, n);
        if (all4) {
            printf("  example seed 0x%08" PRIx32 ":\n", ex);
            for (int k = 0; k < 4; k++) { printf("    H(%c%c) = ", (k & 2) ? 'B' : 'A', (k & 1) ? 'B' : 'A'); print_hex("", exh[k], 16); }
        }
    }
    printf("\nDone.\n");
    return 0;
}
