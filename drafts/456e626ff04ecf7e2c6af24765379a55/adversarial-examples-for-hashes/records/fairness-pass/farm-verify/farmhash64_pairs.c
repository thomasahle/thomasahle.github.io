/*
 * farmhash64_pairs.c -- seed-independent collisions for FarmHash64 (NA variant).
 *
 * Copyright (c) 2026 Thomas Dybdahl Ahle (this program)
 * Copyright (c) 2014 Google, Inc. (FarmHash v1.1 by Geoff Pike, MIT,
 *   https://github.com/google/farmhash)
 * Copyright (C) 2021-2022 Frank J. T. Wojcik (SMHasher3 hashes/farmhash.cpp, MIT,
 *   https://gitlab.com/fwojcik/smhasher3, from which the farmhashna code below
 *   is transcribed)
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
 * The hash below is FarmHash v1.1, namespace farmhashna, in the form SMHasher3
 * registers as FarmHash_64__NA (verification_LE = 0xEBC4A679).  The
 * little-endian code path is used regardless of host endianness (bytes are
 * assembled explicitly).
 *
 * Single file, C11, no dependencies:   cc -O2 -std=c11 -o farmhash64_pairs farmhash64_pairs.c
 * Usage:  ./farmhash64_pairs [log2 N = 24] [rng seed = 1]
 */
#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------------------- */
/* FarmHash NA (64-bit), from SMHasher3 hashes/farmhash.cpp                   */
/* ------------------------------------------------------------------------- */
static const uint64_t k0 = UINT64_C(0xc3a5c85c97cb3127);
static const uint64_t k1 = UINT64_C(0xb492b66fbe98f273);
static const uint64_t k2 = UINT64_C(0x9ae16a3b2f90404f);

static inline uint64_t rotr64(uint64_t x, unsigned r) { return (x >> r) | (x << (64 - r)); }
static inline uint64_t fetch64(const uint8_t *p) {
    uint64_t v = 0;
    for (int i = 7; i >= 0; i--) v = (v << 8) | p[i];
    return v;
}
static inline uint32_t fetch32(const uint8_t *p) {
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}
static inline uint64_t shift_mix(uint64_t v) { return v ^ (v >> 47); }

/* Hash128to64 / HashLen16(u, v): the unkeyed 128->64 combine (kMul fixed). */
static inline uint64_t hash_len16(uint64_t u, uint64_t v) {
    const uint64_t kMul = UINT64_C(0x9ddfea08eb382d69);
    uint64_t a = (u ^ v) * kMul;
    a ^= a >> 47;
    uint64_t b = (v ^ a) * kMul;
    b ^= b >> 47;
    b *= kMul;
    return b;
}
/* HashLen16(u, v, mul) */
static inline uint64_t hash_len16_mul(uint64_t u, uint64_t v, uint64_t mul) {
    uint64_t a = (u ^ v) * mul;
    a ^= a >> 47;
    uint64_t b = (v ^ a) * mul;
    b ^= b >> 47;
    b *= mul;
    return b;
}

typedef struct { uint64_t first, second; } pair64;

/* WeakHashLen32WithSeeds */
static inline pair64 weak_len32(uint64_t w, uint64_t x, uint64_t y, uint64_t z, uint64_t a, uint64_t b) {
    a += w;
    b = rotr64(b + a + z, 21);
    uint64_t c = a;
    a += x;
    a += y;
    b += rotr64(a, 44);
    pair64 r = { a + z, b + c };
    return r;
}
static inline pair64 weak_len32_s(const uint8_t *s, uint64_t a, uint64_t b) {
    return weak_len32(fetch64(s), fetch64(s + 8), fetch64(s + 16), fetch64(s + 24), a, b);
}

static uint64_t hash_len0to16(const uint8_t *s, size_t len) {
    if (len >= 8) {
        uint64_t mul = k2 + len * 2;
        uint64_t a = fetch64(s) + k2;
        uint64_t b = fetch64(s + len - 8);
        uint64_t c = rotr64(b, 37) * mul + a;
        uint64_t d = (rotr64(a, 25) + b) * mul;
        return hash_len16_mul(c, d, mul);
    }
    if (len >= 4) {
        uint64_t mul = k2 + len * 2;
        uint64_t a = fetch32(s);
        return hash_len16_mul(len + (a << 3), fetch32(s + len - 4), mul);
    }
    if (len > 0) {
        uint8_t a = s[0], b = s[len >> 1], c = s[len - 1];
        uint32_t y = (uint32_t)a + ((uint32_t)b << 8);
        uint32_t z = (uint32_t)len + ((uint32_t)c << 2);
        return shift_mix(y * k2 ^ z * k0) * k2;
    }
    return k2;
}

static uint64_t hash_len17to32(const uint8_t *s, size_t len) {
    uint64_t mul = k2 + len * 2;
    uint64_t a = fetch64(s) * k1;
    uint64_t b = fetch64(s + 8);
    uint64_t c = fetch64(s + len - 8) * mul;
    uint64_t d = fetch64(s + len - 16) * k2;
    return hash_len16_mul(rotr64(a + b, 43) + rotr64(c, 30) + d, a + rotr64(b + k2, 18) + c, mul);
}

static uint64_t hash_len33to64(const uint8_t *s, size_t len) {
    uint64_t mul = k2 + len * 2;
    uint64_t a = fetch64(s) * k2;
    uint64_t b = fetch64(s + 8);
    uint64_t c = fetch64(s + len - 8) * mul;
    uint64_t d = fetch64(s + len - 16) * k2;
    uint64_t y = rotr64(a + b, 43) + rotr64(c, 30) + d;
    uint64_t z = hash_len16_mul(y, a + rotr64(b + k2, 18) + c, mul);
    uint64_t e = fetch64(s + 16) * mul;
    uint64_t f = fetch64(s + 24);
    uint64_t g = (y + fetch64(s + len - 32)) * mul;
    uint64_t h = (z + fetch64(s + len - 24)) * mul;
    return hash_len16_mul(rotr64(e + f, 43) + rotr64(g, 30) + h, e + rotr64(f + a, 18) + g, mul);
}

/* farmhashna::Hash64 -- the public, unseeded function. */
static uint64_t farm_hash64(const uint8_t *s, size_t len) {
    const uint64_t seed = 81;
    if (len <= 32) return len <= 16 ? hash_len0to16(s, len) : hash_len17to32(s, len);
    if (len <= 64) return hash_len33to64(s, len);

    uint64_t x = seed;
    uint64_t y = seed * k1 + 113;
    uint64_t z = shift_mix(y * k2 + 113) * k2;
    pair64 v = { 0, 0 }, w = { 0, 0 };
    x = x * k2 + fetch64(s);

    const uint8_t *end = s + ((len - 1) / 64) * 64;
    const uint8_t *last64 = end + ((len - 1) & 63) - 63;
    do {
        x = rotr64(x + y + v.first + fetch64(s + 8), 37) * k1;
        y = rotr64(y + v.second + fetch64(s + 48), 42) * k1;
        x ^= w.second;
        y += v.first + fetch64(s + 40);
        z = rotr64(z + w.first, 33) * k1;
        v = weak_len32_s(s, v.second * k1, x + w.first);
        w = weak_len32_s(s + 32, z + w.second, y + fetch64(s + 16));
        { uint64_t t = z; z = x; x = t; }
        s += 64;
    } while (s != end);
    uint64_t mul = k1 + ((z & 0xff) << 1);
    s = last64;
    w.first += ((len - 1) & 63);
    v.first += w.first;
    w.first += v.first;
    x = rotr64(x + y + v.first + fetch64(s + 8), 37) * mul;
    y = rotr64(y + v.second + fetch64(s + 48), 42) * mul;
    x ^= w.second * 9;
    y += v.first * 9 + fetch64(s + 40);
    z = rotr64(z + w.first, 33) * mul;
    v = weak_len32_s(s, v.second * mul, x + w.first);
    w = weak_len32_s(s + 32, z + w.second, y + fetch64(s + 16));
    { uint64_t t = z; z = x; x = t; }
    return hash_len16_mul(hash_len16_mul(v.first, w.first, mul) + shift_mix(y) * k0 + z,
                          hash_len16_mul(v.second, w.second, mul) + x, mul);
}

/* farmhashna::Hash64WithSeeds / Hash64WithSeed: the seed enters ONLY here. */
static inline uint64_t farm_hash64_with_seeds(const uint8_t *s, size_t len, uint64_t seed0, uint64_t seed1) {
    return hash_len16(farm_hash64(s, len) - seed0, seed1);
}
static inline uint64_t farm_hash64_with_seed(const uint8_t *s, size_t len, uint64_t seed) {
    return farm_hash64_with_seeds(s, len, k2, seed);
}

/* SMHasher3 hashfn FarmHashNA<false>: 64-bit result stored little-endian. */
static void smhasher3_farmhash_na(const void *in, size_t len, uint64_t seed, uint8_t *out) {
    uint64_t h = farm_hash64_with_seed((const uint8_t *)in, len, seed);
    for (int i = 0; i < 8; i++) out[i] = (uint8_t)(h >> (8 * i));
}

/* ------------------------------------------------------------------------- */
/* Validation: SMHasher3 lib/Hashinfo.cpp HashInfo::_ComputedVerifyImpl       */
/* ------------------------------------------------------------------------- */
static uint32_t smhasher3_verification(void) {
    uint8_t key[256], hashes[8 * 256], total[8];
    memset(key, 0, sizeof key);
    memset(hashes, 0, sizeof hashes);
    for (int i = 0; i < 256; i++) {           /* key i = {0,1,..,i-1}, seed 256-i */
        smhasher3_farmhash_na(key, (size_t)i, (uint64_t)(256 - i), &hashes[8 * i]);
        key[i] = (uint8_t)i;
    }
    smhasher3_farmhash_na(hashes, sizeof hashes, 0, total);   /* the 2048-byte array, seed 0 */
    return (uint32_t)total[0] | ((uint32_t)total[1] << 8) | ((uint32_t)total[2] << 16) | ((uint32_t)total[3] << 24);
}

/* ------------------------------------------------------------------------- */
/* RNG: xoshiro256** seeded from splitmix64 (own code, no libc rand)          */
/* ------------------------------------------------------------------------- */
static uint64_t rng_s[4];
static uint64_t splitmix64(uint64_t *st) {
    uint64_t z = (*st += UINT64_C(0x9e3779b97f4a7c15));
    z = (z ^ (z >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
    z = (z ^ (z >> 27)) * UINT64_C(0x94d049bb133111eb);
    return z ^ (z >> 31);
}
static void rng_seed(uint64_t seed) { for (int i = 0; i < 4; i++) rng_s[i] = splitmix64(&seed); }
static inline uint64_t rotl64(uint64_t x, unsigned r) { return (x << r) | (x >> (64 - r)); }
static inline uint64_t rng_next(void) {
    const uint64_t result = rotl64(rng_s[1] * 5, 7) * 9;
    const uint64_t t = rng_s[1] << 17;
    rng_s[2] ^= rng_s[0]; rng_s[3] ^= rng_s[1]; rng_s[1] ^= rng_s[2]; rng_s[0] ^= rng_s[3];
    rng_s[2] ^= t; rng_s[3] = rotl64(rng_s[3], 45);
    return result;
}

/* ------------------------------------------------------------------------- */
/* The published pairs (hex = message bytes in memory order)                  */
/* ------------------------------------------------------------------------- */
typedef struct {
    const char *label, *m1_hex, *m2_hex, *provenance;
} pair_rec;

static const pair_rec PAIRS[] = {
    { "A", "574522e6dce98176", "0df3b1f900d36296",
      "8-byte pair: collision of the public unseeded Hash64 found by a distinguished-point rho\n"
      "      walk (~2^31.8 evaluations); verifier-confirmed at 2^30 seeds, rate exactly 1" },
    { "B", "726c70206661726d706c61696e683332acc3a05fcc8dbead6d1998c94a300be8",
           "736c70206661726de0d8c2d2dcd06664cdeab4eacd29799b6803df46f137a3cd",
      "32-byte pair: closed form, NO search -- HashLen17to32 is solved for words w2,w3 so both\n"
      "      messages land in one HashLen16 collision class; verifier-confirmed at 2^30 seeds" },
    { "C", "6f726c702d6661726d6861736836342d3f56724a404c3779747a776865616161",
           "6f726c702d6661726d6861736836342d703360215351627d666d786865616161",
      "Orson Peters' published strings 'orlp-farmhash64-?VrJ@L7ytzwheaaa' and\n"
      "      'orlp-farmhash64-p3`!SQb}fmxheaaa' (orlp.net/blog/breaking-hash-functions): Hash64 == 1337" },
};
static const uint64_t EXPLICIT_SEED = UINT64_C(0x82bdf567d8ebbf4f);   /* the record's example seed */

static size_t unhex(const char *h, uint8_t *out, size_t cap) {
    size_t n = strlen(h) / 2;
    if (n > cap) { fprintf(stderr, "hex too long\n"); exit(2); }
    for (size_t i = 0; i < n; i++) {
        unsigned b;
        if (sscanf(h + 2 * i, "%2x", &b) != 1) { fprintf(stderr, "bad hex\n"); exit(2); }
        out[i] = (uint8_t)b;
    }
    return n;
}

static void print_rate(const char *what, uint64_t coll, uint64_t n) {
    printf("  %-32s collisions = %" PRIu64 " / %" PRIu64 "   rate = %.6g   log2 rate = ",
           what, coll, n, (double)coll / (double)n);
    if (coll == 0) printf("-inf\n");
    else printf("%.4f\n", log2((double)coll) - log2((double)n));
}

int main(int argc, char **argv) {
    char *end = NULL;
    long lgl = argc > 1 ? strtol(argv[1], &end, 10) : 24;
    if (argc > 1 && (end == argv[1] || *end)) { fprintf(stderr, "usage: %s [log2 N = 24] [rng seed = 1]\n", argv[0]); return 2; }
    uint64_t rng_seed_val = argc > 2 ? strtoull(argv[2], &end, 0) : 1;
    if (argc > 2 && (end == argv[2] || *end)) { fprintf(stderr, "usage: %s [log2 N = 24] [rng seed = 1]\n", argv[0]); return 2; }
    if (lgl < 0 || lgl > 40) { fprintf(stderr, "log2 N must be in 0..40\n"); return 2; }
    int lg = (int)lgl;
    const uint64_t N = (uint64_t)1 << lg;

    printf("farmhash-64: FarmHash v1.1 NA, farmhashna::Hash64WithSeed (SMHasher3 FarmHash_64__NA)\n");

    /* 1. Validate against SMHasher3's verification value; abort on mismatch. */
    uint32_t ver = smhasher3_verification();
    printf("validation: SMHasher3 verification_LE = 0x%08" PRIX32 " (expected 0xEBC4A679) %s\n",
           ver, ver == 0xEBC4A679u ? "OK" : "MISMATCH");
    if (ver != 0xEBC4A679u) { fprintf(stderr, "ABORT: implementation does not match SMHasher3 FarmHash_64__NA\n"); return 1; }
    /* Independent published vector: Peters' strings hash to 1337 under the unseeded Hash64. */
    const char *pv = "orlp-farmhash64-?VrJ@L7ytzwheaaa";
    uint64_t pvh = farm_hash64((const uint8_t *)pv, 32);
    printf("validation: unseeded Hash64(\"%s\") = %" PRIu64 " (published: 1337) %s\n", pv, pvh, pvh == 1337 ? "OK" : "MISMATCH");
    if (pvh != 1337) { fprintf(stderr, "ABORT: published test vector mismatch\n"); return 1; }

    printf("\nmechanism: Hash64WithSeed(m, seed) = HashLen16(Hash64(m) - k2, seed) and\n"
           "           Hash64WithSeeds(m, s0, s1) = HashLen16(Hash64(m) - s0, s1).  For a fixed second\n"
           "           argument HashLen16(u, v) is a bijection of u (xor with v, multiply by an odd\n"
           "           constant, the involution u ^= u >> 47), so the seed never separates two messages\n"
           "           with equal unseeded Hash64: every such pair collides for EVERY seed (rate 1, key-free).\n"
           "conditional on: none (no weak-seed class; the class is all 2^64 seeds, density 1)\n");
    printf("\nseeds: N = 2^%d = %" PRIu64 " uniformly random 64-bit seeds per pair (xoshiro256**, seed %" PRIu64 ")\n",
           lg, N, rng_seed_val);

    int bad = 0;   /* set if any pair fails to collide for every sampled seed */
    for (size_t p = 0; p < sizeof PAIRS / sizeof PAIRS[0]; p++) {
        const pair_rec *r = &PAIRS[p];
        uint8_t m1[64], m2[64];
        size_t n1 = unhex(r->m1_hex, m1, sizeof m1), n2 = unhex(r->m2_hex, m2, sizeof m2);
        printf("\npair %s: %s\n", r->label, r->provenance);
        printf("  m1 = %s  (%zu bytes)\n  m2 = %s  (%zu bytes)\n", r->m1_hex, n1, r->m2_hex, n2);
        if (n1 == n2 && memcmp(m1, m2, n1) == 0) { printf("  messages are identical -- not a pair\n"); return 1; }
        uint64_t u1 = farm_hash64(m1, n1), u2 = farm_hash64(m2, n2);
        printf("  unseeded Hash64: m1 -> %016" PRIx64 "   m2 -> %016" PRIx64 "   %s\n", u1, u2, u1 == u2 ? "(equal)" : "(differ)");

        /* 64-bit seed interface (the SMHasher3 variant). */
        rng_seed(rng_seed_val);
        uint64_t coll = 0, first_coll = 0, first_non = 0;
        int have_coll = 0, have_non = 0;
        for (uint64_t i = 0; i < N; i++) {
            uint64_t sd = rng_next();
            if (farm_hash64_with_seed(m1, n1, sd) == farm_hash64_with_seed(m2, n2, sd)) {
                coll++;
                if (!have_coll) { have_coll = 1; first_coll = sd; }
            } else if (!have_non) { have_non = 1; first_non = sd; }
        }
        print_rate("Hash64WithSeed(seed):", coll, N);

        /* 128-bit seed interface (seed0, seed1), same N. */
        uint64_t coll2 = 0;
        for (uint64_t i = 0; i < N; i++) {
            uint64_t s0 = rng_next(), s1 = rng_next();
            coll2 += farm_hash64_with_seeds(m1, n1, s0, s1) == farm_hash64_with_seeds(m2, n2, s0, s1);
        }
        print_rate("Hash64WithSeeds(seed0, seed1):", coll2, N);

        /* 4. Explicit colliding seeds with both hash values. */
        uint64_t e1 = farm_hash64_with_seed(m1, n1, EXPLICIT_SEED), e2 = farm_hash64_with_seed(m2, n2, EXPLICIT_SEED);
        printf("  explicit seed %016" PRIx64 ":   H(m1) = %016" PRIx64 "   H(m2) = %016" PRIx64 "   %s\n",
               EXPLICIT_SEED, e1, e2, e1 == e2 ? "COLLIDE" : "differ");
        if (u1 != u2 || coll != N || coll2 != N || e1 != e2 || have_non) bad = 1;
        if (have_coll)
            printf("  first sampled colliding seed %016" PRIx64 ":   H = %016" PRIx64 " for both\n",
                   first_coll, farm_hash64_with_seed(m1, n1, first_coll));
        if (have_non)
            printf("  first NON-colliding seed %016" PRIx64 " (unexpected)\n", first_non);
        else
            printf("  non-colliding seeds found: 0\n");
    }
    if (bad) fprintf(stderr, "FAILED: a pair did not collide for every sampled seed\n");
    return bad;
}
