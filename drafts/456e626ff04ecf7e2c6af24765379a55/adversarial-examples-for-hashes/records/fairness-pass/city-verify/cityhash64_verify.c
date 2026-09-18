/*
 * cityhash64_verify.c -- key-free collisions for Google CityHash64WithSeed (v1.1.1)
 *
 * Copyright (c) 2026 Thomas Dybdahl Ahle (this program)
 * Copyright (c) 2011 Google, Inc. (CityHash v1.1.1, src/city.cc: the hash functions below
 *   are a function-by-function transcription of that file, https://github.com/google/cityhash)
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
 * The hash below is a from-scratch C11 transcription of CityHash64 / CityHash64WithSeeds /
 * CityHash64WithSeed from CityHash v1.1.1 (Geoff Pike and Jyrki Alakuijala, Google, 2011,
 * MIT; https://github.com/google/cityhash, src/city.cc) in the exact form SMHasher3 tests
 * as "CityHash_64" (hashes/cityhash.cpp: 64-bit seed, native little-endian fetches).
 *
 * What the program does:
 *   1. computes the SMHasher3 verification value and aborts unless it is 0x5FABC5C5;
 *   2. checks the structural fact behind the attack: the seed enters only through
 *      HashLen16(CityHash64(m) - k2, seed), which is a bijection of its first argument for
 *      every seed (explicit inverse below), so equal unseeded hashes collide for EVERY seed;
 *   3. for two published message pairs, hashes both under N uniformly random seeds
 *      (default N = 2^24; argv[1] = N or "2^k") and under N random (seed0, seed1) pairs of
 *      CityHash64WithSeeds, and prints collisions/N and log2 of the rate;
 *   4. prints explicit colliding seeds with both hash values.
 *
 * Build:  cc -O2 -std=c11 -o cityhash64_verify cityhash64_verify.c -lm
 * Run:    ./cityhash64_verify [N | 2^k] [rng-seed]      (default N = 2^24)
 */
#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* CityHash64 v1.1.1, written from the reference source.  Fetches are little-endian. ----- */
static const uint64_t k0   = UINT64_C(0xc3a5c85c97cb3127);
static const uint64_t k1   = UINT64_C(0xb492b66fbe98f273);
static const uint64_t k2   = UINT64_C(0x9ae16a3b2f90404f);
static const uint64_t kMul = UINT64_C(0x9ddfea08eb382d69);   /* Hash128to64 multiplier */

static inline uint64_t fetch64(const uint8_t *p) {
    uint64_t v = 0;
    for (int i = 7; i >= 0; i--) v = (v << 8) | p[i];
    return v;
}
static inline uint32_t fetch32(const uint8_t *p) {
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}
static inline uint64_t rotr64(uint64_t x, int r) { return (x >> r) | (x << (64 - r)); }
static inline uint64_t bswap64(uint64_t x) {
    x = ((x & UINT64_C(0x00ff00ff00ff00ff)) << 8) | ((x >> 8) & UINT64_C(0x00ff00ff00ff00ff));
    x = ((x & UINT64_C(0x0000ffff0000ffff)) << 16) | ((x >> 16) & UINT64_C(0x0000ffff0000ffff));
    return (x << 32) | (x >> 32);
}
static inline uint64_t shift_mix(uint64_t v) { return v ^ (v >> 47); }

/* HashLen16(u, v, mul): the Murmur-inspired 128->64 combine.  For fixed v it is a
 * bijection of u: xor with v, multiply by odd mul, x ^= x >> 47 are all invertible. */
static inline uint64_t hash_len16_mul(uint64_t u, uint64_t v, uint64_t mul) {
    uint64_t a = (u ^ v) * mul;
    a ^= a >> 47;
    uint64_t b = (v ^ a) * mul;
    b ^= b >> 47;
    return b * mul;
}
static inline uint64_t hash_len16(uint64_t u, uint64_t v) { return hash_len16_mul(u, v, kMul); }

typedef struct { uint64_t first, second; } u64pair;

static inline u64pair weak_hash_len32(uint64_t w, uint64_t x, uint64_t y, uint64_t z,
                                      uint64_t a, uint64_t b) {
    a += w;
    uint64_t c = a;
    b = rotr64(b + a + z, 21);
    a += x;
    a += y;
    b += rotr64(a, 44);
    return (u64pair){ a + z, b + c };
}
static inline u64pair weak_hash_len32_s(const uint8_t *s, uint64_t a, uint64_t b) {
    return weak_hash_len32(fetch64(s), fetch64(s + 8), fetch64(s + 16), fetch64(s + 24), a, b);
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
    uint64_t c = fetch64(s + len - 24);
    uint64_t d = fetch64(s + len - 32);
    uint64_t e = fetch64(s + 16) * k2;
    uint64_t f = fetch64(s + 24) * 9;
    uint64_t g = fetch64(s + len - 8);
    uint64_t h = fetch64(s + len - 16) * mul;
    uint64_t u = rotr64(a + g, 43) + (rotr64(b, 30) + c) * 9;
    uint64_t v = ((a + g) ^ d) + f + 1;
    uint64_t w = bswap64((u + v) * mul) + h;
    uint64_t x = rotr64(e + f, 42) + c;
    uint64_t y = (bswap64((v + w) * mul) + g) * mul;
    uint64_t z = e + f + c;
    a = bswap64((x + z) * mul + y) + b;
    b = shift_mix((z + a) * mul + d + h) * mul;
    return b + x;
}

static uint64_t cityhash64(const uint8_t *s, size_t len) {
    if (len <= 32) return len <= 16 ? hash_len0to16(s, len) : hash_len17to32(s, len);
    if (len <= 64) return hash_len33to64(s, len);
    /* > 64 bytes: hash the tail first, then 64-byte chunks with 56 bytes of state. */
    uint64_t x = fetch64(s + len - 40);
    uint64_t y = fetch64(s + len - 16) + fetch64(s + len - 56);
    uint64_t z = hash_len16(fetch64(s + len - 48) + len, fetch64(s + len - 24));
    u64pair v = weak_hash_len32_s(s + len - 64, len, z);
    u64pair w = weak_hash_len32_s(s + len - 32, y + k1, x);
    x = x * k1 + fetch64(s);
    len = (len - 1) & ~(size_t)63;
    do {
        x = rotr64(x + y + v.first + fetch64(s + 8), 37) * k1;
        y = rotr64(y + v.second + fetch64(s + 48), 42) * k1;
        x ^= w.second;
        y += v.first + fetch64(s + 40);
        z = rotr64(z + w.first, 33) * k1;
        v = weak_hash_len32_s(s, v.second * k1, x + w.first);
        w = weak_hash_len32_s(s + 32, z + w.second, y + fetch64(s + 16));
        uint64_t t = z; z = x; x = t;
        s += 64;
        len -= 64;
    } while (len != 0);
    return hash_len16(hash_len16(v.first, w.first) + shift_mix(y) * k1 + z,
                      hash_len16(v.second, w.second) + x);
}

static inline uint64_t cityhash64_with_seeds(const uint8_t *s, size_t len, uint64_t seed0, uint64_t seed1) {
    return hash_len16(cityhash64(s, len) - seed0, seed1);
}
static inline uint64_t cityhash64_with_seed(const uint8_t *s, size_t len, uint64_t seed) {
    return cityhash64_with_seeds(s, len, k2, seed);
}

/* SMHasher3 verification value (lib/Hashinfo.cpp, _ComputedVerifyImpl). ---------------- */
static uint32_t smhasher3_verification(void) {
    uint8_t key[256], hashes[8 * 256];
    memset(key, 0, sizeof key);
    for (int i = 0; i < 256; i++) {               /* key i = bytes 0..i-1, seed 256-i */
        uint64_t h = cityhash64_with_seed(key, (size_t)i, (uint64_t)(256 - i));
        for (int j = 0; j < 8; j++) hashes[8 * i + j] = (uint8_t)(h >> (8 * j));
        key[i] = (uint8_t)i;
    }
    uint64_t total = cityhash64_with_seed(hashes, sizeof hashes, 0);
    return (uint32_t)total;                       /* first 4 bytes, little-endian */
}

/* RNG: splitmix64 seeds xoshiro256** (Blackman & Vigna, public domain algorithms). ------ */
static uint64_t splitmix64(uint64_t *state) {
    uint64_t z = (*state += UINT64_C(0x9e3779b97f4a7c15));
    z = (z ^ (z >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
    z = (z ^ (z >> 27)) * UINT64_C(0x94d049bb133111eb);
    return z ^ (z >> 31);
}
typedef struct { uint64_t s[4]; } xoshiro256;
static void xoshiro_seed(xoshiro256 *r, uint64_t seed) { for (int i = 0; i < 4; i++) r->s[i] = splitmix64(&seed); }
static inline uint64_t rotl64(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
static inline uint64_t xoshiro_next(xoshiro256 *r) {
    uint64_t *s = r->s, result = rotl64(s[1] * 5, 7) * 9, t = s[1] << 17;
    s[2] ^= s[0]; s[3] ^= s[1]; s[1] ^= s[2]; s[0] ^= s[3]; s[2] ^= t; s[3] = rotl64(s[3], 45);
    return result;
}

/* The mechanism, made explicit: inverse of the seed combine and of HashLen17to32. ------- */
static uint64_t inv_odd64(uint64_t a) {           /* a^-1 mod 2^64, Newton iteration */
    uint64_t x = a;
    for (int i = 0; i < 6; i++) x *= 2 - a * x;
    return x;
}
static inline uint64_t unshift47(uint64_t x) { return x ^ (x >> 47); }   /* self-inverse */

/* u such that hash_len16_mul(u, v, mul) == h.  Exists and is unique for every (h, v). */
static uint64_t hash_len16_inverse(uint64_t h, uint64_t v, uint64_t mul) {
    uint64_t imul = inv_odd64(mul);
    uint64_t a = (unshift47(h * imul) * imul) ^ v;       /* a after its shift-mix */
    return (unshift47(a) * imul) ^ v;                    /* u */
}
/* Given a seeded output and the seed, recover the unseeded CityHash64 of the message. */
static uint64_t strip_seed(uint64_t seeded, uint64_t seed) {
    return hash_len16_inverse(seeded, seed, kMul) + k2;
}
/* 32-byte preimage: for any target T and any words m0, m1, m3 there is exactly one m2
 * with CityHash64(m0|m1|m2|m3) == T (HashLen17to32 with len = 32, mul = k2 + 64). */
static uint64_t solve_word2(uint64_t m0, uint64_t m1, uint64_t m3, uint64_t target) {
    uint64_t mul = k2 + 64;
    uint64_t a = m0 * k1, b = m1, c = m3 * mul;
    uint64_t v = a + rotr64(b + k2, 18) + c;
    uint64_t u = hash_len16_inverse(target, v, mul);
    uint64_t d = u - rotr64(a + b, 43) - rotr64(c, 30);    /* d = m2 * k2 */
    return d * inv_odd64(k2);
}

/* Published pairs (hex copied exactly from the analysis record). ------------------------ */
typedef struct {
    const char *label, *m1_hex, *m2_hex, *mechanism;
    uint64_t example_seed, example_hash;             /* published colliding seed + value */
    uint64_t unseeded;                               /* published CityHash64(m1) == CityHash64(m2) */
} published_pair;

static const published_pair PAIRS[] = {
    { "A: 8-byte pair (L = 1 word), found by Brent rho on the public map m -> CityHash64(m)",
      "a01109025ea76be1", "020bd424b04ae555",
      "unseeded CityHash64(m1) == CityHash64(m2); the seed enters only through the bijection "
      "u -> HashLen16(u - k2, seed), so the pair collides for every 64-bit seed and every "
      "(seed0, seed1) of CityHash64WithSeeds",
      UINT64_C(0x6637c1ce6357a2c8), UINT64_C(0xd4d44b0c5f8bae0a), UINT64_C(0x794ce2bbd3dc7242) },
    { "B: 32-byte pair (L = 4 words), analytic preimage of HashLen17to32 with target 0x1337",
      "436974794861736836342d6f6b21212183454502d40dfa393030303030303030",
      "436974794861736836342d6f6b21212183ff1a7c2ab06b6a3030303030303031",
      "choose words m0, m1, m3 freely, invert HashLen16(u, v, k2 + 64) and solve m2 = "
      "(u - rotr(m0*k1 + m1, 43) - rotr(m3*mul, 30)) * k2^-1: 2^192 messages of length 32 "
      "share any chosen unseeded value, hence collide for every seed",
      UINT64_C(0xcbd18ebcd1f9b00d), UINT64_C(0x9f176442ecd010a8), UINT64_C(0x0000000000001337) },
};

static size_t unhex(const char *hex, uint8_t *out, size_t cap) {
    size_t n = strlen(hex) / 2;
    if (n > cap) { fprintf(stderr, "hex too long\n"); exit(2); }
    for (size_t i = 0; i < n; i++) {
        unsigned b;
        if (sscanf(hex + 2 * i, "%2x", &b) != 1) { fprintf(stderr, "bad hex\n"); exit(2); }
        out[i] = (uint8_t)b;
    }
    return n;
}
static void print_hex(const uint8_t *p, size_t n) { for (size_t i = 0; i < n; i++) printf("%02x", p[i]); }

static void print_rate(const char *what, uint64_t hits, uint64_t n) {
    printf("  %s: N = %" PRIu64 " (2^%.2f), collisions = %" PRIu64 ", rate = %.6g, log2 rate = ",
           what, n, log2((double)n), hits, (double)hits / (double)n);
    if (hits) printf("%.4f\n", log2((double)hits / (double)n));
    else      printf("-inf (< %.2f)\n", -log2((double)n));
}

static int check_pair(const published_pair *p, uint64_t n, xoshiro256 *rng) {
    int ok = 1;
    uint8_t m1[64], m2[64];
    size_t len = unhex(p->m1_hex, m1, sizeof m1), len2 = unhex(p->m2_hex, m2, sizeof m2);
    if (len != len2 || memcmp(m1, m2, len) == 0) { printf("  pair malformed\n"); return 0; }

    printf("\npair %s\n  m1 = ", p->label); print_hex(m1, len); printf("  (%zu bytes)\n  m2 = ", len);
    print_hex(m2, len); printf("\n");
    printf("  mechanism: %s\n", p->mechanism);
    printf("  seed class: all seeds (unconditional, key-free); class density = 1\n");

    uint64_t h1 = cityhash64(m1, len), h2 = cityhash64(m2, len);
    printf("  unseeded CityHash64: m1 -> %016" PRIx64 ", m2 -> %016" PRIx64 " %s (published %016" PRIx64 ")\n",
           h1, h2, h1 == h2 ? "EQUAL" : "DIFFER", p->unseeded);
    if (h1 != h2 || h1 != p->unseeded) ok = 0;
    /* N uniformly random 64-bit seeds through CityHash64WithSeed. */
    uint64_t hits = 0, first_seed = 0, first_hash = 0;
    int have_first = 0;
    for (uint64_t i = 0; i < n; i++) {
        uint64_t seed = xoshiro_next(rng);
        uint64_t a = cityhash64_with_seed(m1, len, seed), b = cityhash64_with_seed(m2, len, seed);
        if (a == b) { hits++; if (!have_first) { have_first = 1; first_seed = seed; first_hash = a; } }
    }
    print_rate("random 64-bit seeds", hits, n);
    if (hits != n) ok = 0;

    /* N uniformly random (seed0, seed1) pairs through CityHash64WithSeeds. */
    uint64_t hits2 = 0;
    for (uint64_t i = 0; i < n; i++) {
        uint64_t s0 = xoshiro_next(rng), s1 = xoshiro_next(rng);
        if (cityhash64_with_seeds(m1, len, s0, s1) == cityhash64_with_seeds(m2, len, s0, s1)) hits2++;
    }
    print_rate("random 128-bit (seed0, seed1), CityHash64WithSeeds", hits2, n);
    if (hits2 != n) ok = 0;

    /* Explicit colliding seeds: the published one (checked against the published value),
     * seed 0, seed 1 and the first sampled one. */
    uint64_t e1 = cityhash64_with_seed(m1, len, p->example_seed), e2 = cityhash64_with_seed(m2, len, p->example_seed);
    printf("  published seed 0x%016" PRIx64 ": h(m1) = %016" PRIx64 ", h(m2) = %016" PRIx64 " %s (published %016" PRIx64 ")\n",
           p->example_seed, e1, e2, e1 == e2 ? "COLLIDE" : "differ", p->example_hash);
    if (e1 != e2 || e1 != p->example_hash) ok = 0;
    for (uint64_t seed = 0; seed <= 1; seed++) {
        uint64_t a = cityhash64_with_seed(m1, len, seed), b = cityhash64_with_seed(m2, len, seed);
        printf("  seed 0x%016" PRIx64 ": h(m1) = %016" PRIx64 ", h(m2) = %016" PRIx64 " %s\n",
               seed, a, b, a == b ? "COLLIDE" : "differ");
        if (a != b) ok = 0;
    }
    if (have_first)
        printf("  first sampled seed 0x%016" PRIx64 ": h(m1) = h(m2) = %016" PRIx64 "; strip_seed -> %016" PRIx64 "\n",
               first_seed, first_hash, strip_seed(first_hash, first_seed));
    return ok;
}

static uint64_t parse_count(const char *s) {
    char *end = NULL;
    if (s[0] == '2' && s[1] == '^') {
        unsigned long k = strtoul(s + 2, &end, 10);
        if (end == s + 2 || *end || k > 63) { fprintf(stderr, "bad count '%s': use N or 2^k with 0 <= k <= 63\n", s); exit(2); }
        return UINT64_C(1) << k;
    }
    uint64_t v = strtoull(s, &end, 0);
    if (end == s || *end) { fprintf(stderr, "bad count '%s': use N or 2^k\n", s); exit(2); }
    return v;
}

int main(int argc, char **argv) {
    uint64_t n = argc > 1 ? parse_count(argv[1]) : (UINT64_C(1) << 24);
    char *end = NULL;
    uint64_t rng_seed = argc > 2 ? strtoull(argv[2], &end, 0) : UINT64_C(0x5eed5eed5eed5eed);
    if (argc > 2 && (end == argv[2] || *end)) { fprintf(stderr, "bad rng seed '%s'\n", argv[2]); return 2; }
    if (n == 0) { fprintf(stderr, "N must be positive\n"); return 2; }
    int ok = 1;
    printf("cityhash-64: Google CityHash64WithSeed, CityHash v1.1.1 (SMHasher3 variant CityHash_64)\n");
    /* 1. validation */
    uint32_t v = smhasher3_verification();
    printf("SMHasher3 verification value: 0x%08" PRIX32 " (expected 0x5FABC5C5) %s\n", v,
           v == UINT32_C(0x5FABC5C5) ? "OK" : "MISMATCH");
    if (v != UINT32_C(0x5FABC5C5)) { fprintf(stderr, "abort: hash implementation does not match SMHasher3\n"); return 1; }

    /* 2. the bijection that makes every seed useless: check the explicit inverse */
    xoshiro256 rng;
    xoshiro_seed(&rng, rng_seed);
    uint64_t fails = 0, trials = UINT64_C(1) << 20;
    for (uint64_t i = 0; i < trials; i++) {
        uint64_t u = xoshiro_next(&rng), seed = xoshiro_next(&rng);
        if (hash_len16_inverse(hash_len16(u, seed), seed, kMul) != u) fails++;
    }
    printf("seed combine HashLen16(u, seed) inverted on 2^20 random (u, seed): %" PRIu64 " failures\n", fails);
    if (fails) ok = 0;

    /* 3.-4. the pairs */
    for (size_t i = 0; i < sizeof PAIRS / sizeof PAIRS[0]; i++)
        if (!check_pair(&PAIRS[i], n, &rng)) ok = 0;

    /* Pair B is reproducible from its recipe: prefix "CityHash64-ok!!!" gives m0, m1;
     * m3 is the ASCII counter "00000000" / "00000001"; m2 is solved for target 0x1337. */
    {
        uint8_t m[32], want[32];
        memcpy(m, "CityHash64-ok!!!", 16);
        uint64_t m0 = fetch64(m), m1 = fetch64(m + 8);
        int same = 1;
        for (int i = 0; i < 2; i++) {
            char ctr[9]; snprintf(ctr, sizeof ctr, "%08d", i);
            memcpy(m + 24, ctr, 8);
            uint64_t m2 = solve_word2(m0, m1, fetch64(m + 24), UINT64_C(0x1337));
            for (int j = 0; j < 8; j++) m[16 + j] = (uint8_t)(m2 >> (8 * j));
            unhex(i == 0 ? PAIRS[1].m1_hex : PAIRS[1].m2_hex, want, sizeof want);
            if (memcmp(m, want, 32) != 0 || cityhash64(m, 32) != UINT64_C(0x1337)) same = 0;
        }
        printf("\npair B rebuilt from its recipe (solve_word2 with target 0x1337): %s\n",
               same ? "matches the published hex, both hash to 0000000000001337" : "MISMATCH");
        if (!same) ok = 0;
    }

    /* Negative control: m1 of pair A against m1 with one bit flipped must not collide. */
    {
        uint8_t m1[8], m3[8];
        unhex(PAIRS[0].m1_hex, m1, 8); memcpy(m3, m1, 8); m3[7] ^= 1;
        uint64_t hits = 0;
        for (uint64_t i = 0; i < n; i++) {
            uint64_t seed = xoshiro_next(&rng);
            if (cityhash64_with_seed(m1, 8, seed) == cityhash64_with_seed(m3, 8, seed)) hits++;
        }
        printf("\ncontrol: m1 of pair A vs m1 with the last bit flipped (");
        print_hex(m3, 8); printf(")\n");
        print_rate("random 64-bit seeds", hits, n);
        if (hits) ok = 0;
    }

    printf("\nresult: %s\n", ok ? "ALL CHECKS PASSED" : "SOME CHECK FAILED");
    return ok ? 0 : 1;
}
