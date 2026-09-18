/*
 * t1ha2_64_verify.c -- fixed-pair, random-seed collision check for t1ha2-64
 *
 * Copyright (c) 2026 Thomas Dybdahl Ahle
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
 * The hash below is a from-scratch re-implementation of t1ha2_atonce (64-bit
 * output, 64-bit seed), written from the SMHasher3 source hashes/t1ha.cpp
 * (variant t1ha2_64 = t1ha2<MODE_LE_NATIVE,false>, verification_LE 0x8F16C948).
 * Three things are copied from the t1ha project's self-check rather than
 * rewritten: the 81-entry known-answer table t1ha_refval_2atonce, the 64-byte
 * test pattern t1ha_test_pattern, and the probe schedule of t1ha_selfcheck().
 * Those carry the t1ha license, reproduced here:
 *
 *   Copyright (c) 2016-2020 Positive Technologies, https://www.ptsecurity.com,
 *   Fast Positive Hash.
 *   Portions Copyright (c) 2010-2020 Leonid Yuriev <leo@yuriev.ru>,
 *   The 1Hippeus project (t1h).
 *   Portions Copyright (c) 2022 Frank J. T. Wojcik
 *
 *   This software is provided 'as-is', without any express or implied
 *   warranty. In no event will the authors be held liable for any damages
 *   arising from the use of this software.
 *
 *   Permission is granted to anyone to use this software for any purpose,
 *   including commercial applications, and to alter it and redistribute it
 *   freely, subject to the following restrictions:
 *
 *   1. The origin of this software must not be misrepresented; you must not
 *      claim that you wrote the original software. If you use this software
 *      in a product, an acknowledgement in the product documentation would be
 *      appreciated but is not required.
 *   2. Altered source versions must be plainly marked as such, and must not be
 *      misrepresented as being the original software.
 *   3. This notice may not be removed or altered from any source distribution.
 *
 * (https://github.com/erthink/t1ha, frozen v2.1 as carried by SMHasher3.)
 *
 * Build:  cc -O2 -std=c11 -o t1ha2_64_verify t1ha2_64_verify.c -lm
 * Run:    ./t1ha2_64_verify [log2 seeds, default 24] [rng seed, default 1]
 *         (a first argument above 63 is taken as the seed count itself)
 *
 * Needs a little-endian host and a compiler with unsigned __int128 (gcc/clang).
 */
#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------------------ */
/* t1ha2_atonce, 64-bit                                                      */
/* ------------------------------------------------------------------------ */
#define P0 UINT64_C(0xEC99BF0D8372CAAB)
#define P1 UINT64_C(0x82434FE90EDCEF39)
#define P2 UINT64_C(0xD4F06DB99D67BE4B)
#define P3 UINT64_C(0xBD9CACC22C6E9571)
#define P4 UINT64_C(0x9C06FAF4D023E3AB)
#define P5 UINT64_C(0xC060724A8424F345)
#define P6 UINT64_C(0xCB5AF53AE3AAAC31)

static inline uint64_t rotr64(uint64_t v, unsigned n) { return (v >> n) | (v << (64 - n)); }
static inline uint64_t rd64(const uint8_t *p) { uint64_t v; memcpy(&v, p, 8); return v; }
/* last 1..8 bytes of the input, little-endian, zero-extended (upstream reads
 * past the end of the buffer instead; the values are identical) */
static inline uint64_t tail64(const uint8_t *p, size_t len) {
    size_t n = len & 7 ? len & 7 : 8; uint64_t v = 0; memcpy(&v, p, n); return v;
}
static inline void mul128(uint64_t a, uint64_t b, uint64_t *lo, uint64_t *hi) {
    unsigned __int128 r = (unsigned __int128)a * b; *lo = (uint64_t)r; *hi = (uint64_t)(r >> 64);
}
static inline uint64_t mux64(uint64_t v, uint64_t prime) {
    uint64_t l, h; mul128(v, prime, &l, &h); return l ^ h;
}
static inline void mixup64(uint64_t *a, uint64_t *b, uint64_t v, uint64_t prime) {
    uint64_t l, h; mul128(*b + v, prime, &l, &h); *a ^= l; *b += h;
}
static inline uint64_t final64(uint64_t a, uint64_t b) {
    uint64_t x = (a + rotr64(b, 41)) * P0;
    uint64_t y = (rotr64(a, 23) + b) * P6;
    return mux64(x ^ y, P5);
}
static uint64_t t1ha2_64(const void *data, size_t len, uint64_t seed) {
    const uint8_t *v = (const uint8_t *)data;
    uint64_t a = seed, b = (uint64_t)len;
    size_t n = len;
    if (n > 32) {
        uint64_t c = rotr64(b, 23) + ~a, d = ~b + rotr64(a, 19);
        const uint8_t *detent = v + n - 31;
        do {
            uint64_t w0 = rd64(v), w1 = rd64(v + 8), w2 = rd64(v + 16), w3 = rd64(v + 24);
            v += 32;
            uint64_t d02 = w0 + rotr64(w2 + d, 56);
            uint64_t c13 = w1 + rotr64(w3 + c, 19);
            d ^= b + rotr64(w1, 38);
            c ^= a + rotr64(w0, 57);
            b ^= P6 * (c13 + w2);
            a ^= P5 * (d02 + w3);
        } while (v < detent);
        a ^= P6 * (c + rotr64(d, 23));
        b ^= P5 * (rotr64(c, 19) + d);
        n &= 31;
    }
    if (n > 24) { mixup64(&a, &b, rd64(v), P4); v += 8; }
    if (n > 16) { mixup64(&b, &a, rd64(v), P3); v += 8; }
    if (n >  8) { mixup64(&a, &b, rd64(v), P2); v += 8; }
    if (n >  0) { mixup64(&b, &a, tail64(v, n), P1); }
    return final64(a, b);
}

/* ------------------------------------------------------------------------ */
/* Validation: upstream known-answer table + SMHasher3 verification value    */
/* ------------------------------------------------------------------------ */
static const uint8_t kat_pattern[64] = {
    0, 1, 2, 3, 4, 5, 6, 7, 0xFF, 0x7F, 0x3F, 0x1F, 0xF, 8, 16, 32, 64, 0x80, 0xFE, 0xFC,
    0xF8, 0xF0, 0xE0, 0xC0, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x55, 0xAA, 11, 17, 19, 23,
    29, 37, 42, 43, 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x' };
static const uint64_t kat_ref[81] = { 0,
    0x772C7311BE32FF42, 0x444753D23F207E03, 0x71F6DF5DA3B4F532, 0x555859635365F660,
    0xE98808F1CD39C626, 0x2EB18FAF2163BB09, 0x7B9DD892C8019C87, 0xE2B1431C4DA4D15A,
    0x1984E718A5477F70, 0x08DD17B266484F79, 0x4C83A05D766AD550, 0x92DCEBB131D1907D,
    0xD67BC6FC881B8549, 0xF6A9886555FBF66B, 0x6E31616D7F33E25E, 0x36E31B7426E3049D,
    0x4F8E4FAF46A13F5F, 0x03EB0CB3253F819F, 0x636A7769905770D2, 0x3ADF3781D16D1148,
    0x92D19CB1818BC9C2, 0x283E68F4D459C533, 0xFA83A8A88DECAA04, 0x8C6F00368EAC538C,
    0x7B66B0CF3797B322, 0x5131E122FDABA3FF, 0x6E59FF515C08C7A9, 0xBA2C5269B2C377B0,
    0xA9D24FD368FE8A2B, 0x22DB13D32E33E891, 0x7B97DFC804B876E5, 0xC598BDFCD0E834F9,
    0xB256163D3687F5A7, 0x66D7A73C6AEF50B3, 0x25A7201C85D9E2A3, 0x911573EDA15299AA,
    0x5C0062B669E18E4C, 0x17734ADE08D54E28, 0xFFF036E33883F43B, 0xFE0756E7777DF11E,
    0x37972472D023F129, 0x6CFCE201B55C7F57, 0xE019D1D89F02B3E1, 0xAE5CC580FA1BB7E6,
    0x295695FB7E59FC3A, 0x76B6C820A40DD35E, 0xB1680A1768462B17, 0x2FB6AF279137DADA,
    0x28FB6B4366C78535, 0xEC278E53924541B1, 0x164F8AAB8A2A28B5, 0xB6C330AEAC4578AD,
    0x7F6F371070085084, 0x94DEAD60C0F448D3, 0x99737AC232C559EF, 0x6F54A6F9CA8EDD57,
    0x979B01E926BFCE0C, 0xF7D20BC85439C5B4, 0x64EDB27CD8087C12, 0x11488DE5F79C0BE2,
    0x25541DDD1680B5A4, 0x8B633D33BE9D1973, 0x404A3113ACF7F6C6, 0xC59DBDEF8550CD56,
    0x039D23C68F4F992C, 0x5BBB48E4BDD6FD86, 0x41E312248780DF5A, 0xD34791CE75D4E94F,
    0xED523E5D04DCDCFF, 0x7A6BCE0B6182D879, 0x21FB37483CAC28D8, 0x19A1B66E8DA878AD,
    0x6F804C5295B09ABE, 0x2A4BE5014115BA81, 0xA678ECC5FC924BE0, 0x50F7A54A99A36F59,
    0x0FD7E63A39A66452, 0x5AB1B213DD29C4E4, 0xF3ED80D9DF6534C5, 0xC736B12EF90615FD };

static int kat_probe(int *k, const void *p, size_t n, uint64_t seed) {
    uint64_t h = t1ha2_64(p, n, seed), r = kat_ref[*k];
    if (h != r) fprintf(stderr, "KAT %d: got %016" PRIx64 " want %016" PRIx64 "\n", *k, h, r);
    (*k)++;
    return h != r;
}
static void validate(void) {
    int k = 0, bad = 0;            /* same probe order as upstream t1ha_selfcheck */
    static const uint8_t zero8[8] = { 0 };
    uint8_t longpat[512];
    uint64_t seed;
    bad |= kat_probe(&k, zero8, 0, 0);
    bad |= kat_probe(&k, zero8, 0, ~UINT64_C(0));
    bad |= kat_probe(&k, kat_pattern, 64, 0);
    seed = 1;
    for (int i = 1; i < 64; i++) { bad |= kat_probe(&k, kat_pattern, i, seed); seed <<= 1; }
    seed = ~UINT64_C(0);
    for (int i = 1; i <= 7; i++) { seed <<= 1; bad |= kat_probe(&k, kat_pattern + i, 64 - i, seed); }
    for (int i = 0; i < 512; i++) longpat[i] = (uint8_t)i;
    for (int i = 0; i <= 7; i++) bad |= kat_probe(&k, longpat + i, 128 + i * 17, seed);
    /* SMHasher3 HashInfo::_ComputedVerifyImpl: keys {}, {0}, {0,1}, ... (len 0..255)
     * with seed 256-len, outputs concatenated little-endian, rehashed with seed 0,
     * first four bytes little-endian */
    uint8_t key[256] = { 0 }, hashes[8 * 256];
    for (int i = 0; i < 256; i++) {
        uint64_t h = t1ha2_64(key, (size_t)i, (uint64_t)(256 - i));
        for (int j = 0; j < 8; j++) hashes[8 * i + j] = (uint8_t)(h >> (8 * j));
        key[i] = (uint8_t)i;
    }
    uint32_t verif = (uint32_t)t1ha2_64(hashes, sizeof hashes, 0);
    printf("validation: upstream KAT %d/%d ok; SMHasher3 verification 0x%08" PRIX32 " (want 0x8F16C948) %s\n",
           k - bad, k, verif, verif == 0x8F16C948u ? "ok" : "FAIL");
    if (bad || verif != 0x8F16C948u) { fprintf(stderr, "validation failed, aborting\n"); exit(1); }
}

/* ------------------------------------------------------------------------ */
/* RNG: splitmix64 seeding xoshiro256**                                      */
/* ------------------------------------------------------------------------ */
static uint64_t splitmix64(uint64_t *s) {
    uint64_t z = (*s += UINT64_C(0x9E3779B97F4A7C15));
    z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
    z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
    return z ^ (z >> 31);
}
typedef struct { uint64_t s[4]; } rng_t;
static void rng_init(rng_t *r, uint64_t seed) { for (int i = 0; i < 4; i++) r->s[i] = splitmix64(&seed); }
static inline uint64_t rng_next(rng_t *r) {
    uint64_t *s = r->s, res = rotr64(s[1] * 5, 64 - 7) * 9, t = s[1] << 17;
    s[2] ^= s[0]; s[3] ^= s[1]; s[1] ^= s[2]; s[0] ^= s[3]; s[2] ^= t; s[3] = rotr64(s[3], 64 - 45);
    return res;
}

/* ------------------------------------------------------------------------ */
/* The published pairs                                                       */
/* ------------------------------------------------------------------------ */
typedef struct {
    const char *name, *m1, *m2, *mechanism;
    uint64_t lmask, lval;      /* seed class: (L & lmask) == lval, L defined below */
    uint64_t example_seed;     /* one published colliding seed */
    double claimed_eps_log2;   /* recorded class contribution, log2 */
} pair_t;

static const pair_t PAIRS[] = {
    { "A (worst confirmed pair)",
      "406b68c281a55e00158ed10a0d96f8ff", "d1d86241d24d9f990323e0",
      "16-byte vs 11-byte messages: a signed-digit difference in word 0 (public\n"
      "    (len+w0)*P2 shift) is cancelled by the tail word's carry pattern through P1;\n"
      "    only 26 bits of L and 4 carry signs depend on the seed",
      UINT64_C(0x7f868a5066451822), UINT64_C(0x16808a1062000020),
      UINT64_C(0x3c805cc67a687f30), -30.0 },
    { "B (same mechanism, second digit route)",
      "9858cabfcf20f692468be82bd3", "95589a09e664a742a4ac26262cc7fdfc",
      "13-byte vs 16-byte messages: 4-digit route +2^56-2^46-2^20+2^50 on the\n"
      "    word-0 product, tail difference chosen so the P1 carries cancel it;\n"
      "    27 bits of L and 4 carry signs fixed",
      UINT64_C(0x4d4ce1e6964c5012), UINT64_C(0x4c408040000c1012),
      UINT64_C(0xd870b802f1050157), -31.01 },
};

static size_t unhex(const char *s, uint8_t *out) {
    size_t n = strlen(s) / 2;
    for (size_t i = 0; i < n; i++) { unsigned v; sscanf(s + 2 * i, "%2x", &v); out[i] = (uint8_t)v; }
    return n;
}
static double log2_rate(uint64_t hits, uint64_t n) { return hits ? log2((double)hits / (double)n) : -INFINITY; }
static int popcount64(uint64_t x) { int c = 0; for (; x; x &= x - 1) c++; return c; }

/* For a 9..16-byte message m1 = (w0, t): after mixup64(&a,&b,w0,P2) the state
 * word a equals seed ^ l0 with l0 = lo((len + w0) * P2) public; the tail step
 * computes L = lo(((seed ^ l0) + t) * P1).  L is a bijection of the seed, so
 * sampling L uniformly on {L & lmask == lval} and inverting samples the seed
 * uniformly on a class of density exactly 2^-popcount(lmask). */
typedef struct { uint64_t l0, t, p1inv; } lmap_t;
static lmap_t lmap_init(const uint8_t *m, size_t len) {
    lmap_t L; uint64_t lo, hi, x = P1;
    if (len < 9 || len > 16) { fprintf(stderr, "class map needs a 9..16-byte m1\n"); exit(1); }
    mul128((uint64_t)len + rd64(m), P2, &lo, &hi);
    L.l0 = lo; L.t = tail64(m + 8, len - 8);
    for (int i = 0; i < 6; i++) x *= 2 - P1 * x;   /* Newton: inverse of P1 mod 2^64 */
    if (x * P1 != 1) { fprintf(stderr, "P1 inverse failed\n"); exit(1); }
    L.p1inv = x;
    return L;
}
static inline uint64_t lmap_L(const lmap_t *L, uint64_t seed) { return ((seed ^ L->l0) + L->t) * P1; }
static inline uint64_t lmap_seed(const lmap_t *L, uint64_t Lv) { return (Lv * L->p1inv - L->t) ^ L->l0; }

static void run_pair(const pair_t *p, uint64_t N, uint64_t rngseed) {
    uint8_t m1[64], m2[64];
    size_t n1 = unhex(p->m1, m1), n2 = unhex(p->m2, m2);
    rng_t r; rng_init(&r, rngseed);
    printf("\npair %s\n  m1 (%2zu bytes) = %s\n  m2 (%2zu bytes) = %s\n  mechanism: %s\n",
           p->name, n1, p->m1, n2, p->m2, p->mechanism);

    lmap_t L = lmap_init(m1, n1);

    /* (a) uniform random seeds; every hit is also tested for membership in the
     *     seed class of (b), since the class rate below is only a lower bound on
     *     the uniform rate if collisions can occur outside the class too */
    uint64_t hits = 0, first = 0, hits_in_class = 0;
    for (uint64_t i = 0; i < N; i++) {
        uint64_t s = rng_next(&r);
        if (t1ha2_64(m1, n1, s) == t1ha2_64(m2, n2, s)) {
            if (!hits) first = s;
            hits++;
            hits_in_class += (lmap_L(&L, s) & p->lmask) == p->lval;
        }
    }
    printf("  uniform seeds: N = 2^%.2f (%" PRIu64 "): %" PRIu64 " collisions, rate ",
           log2((double)N), N, hits);
    if (hits) printf("2^%.2f", log2_rate(hits, N)); else printf("unestimated (zero observed in 2^%.2f trials)", log2((double)N));
    printf("  (expected at the published 2^%.2f: %.3f)\n", p->claimed_eps_log2,
           (double)N * pow(2.0, p->claimed_eps_log2));
    if (hits) printf("  first uniform colliding seed: %016" PRIx64 "  (uniform hits inside the seed class below: %" PRIu64 "/%" PRIu64 ")\n",
                     first, hits_in_class, hits);

    /* (b) the weak-seed class */
    int kbits = popcount64(p->lmask);
    uint64_t chits = 0, cfirst = 0;
    for (uint64_t i = 0; i < N; i++) {
        uint64_t s = lmap_seed(&L, (rng_next(&r) & ~p->lmask) | p->lval);
        if (t1ha2_64(m1, n1, s) == t1ha2_64(m2, n2, s)) { if (!chits) cfirst = s; chits++; }
    }
    double crate = log2_rate(chits, N);
    printf("  seed class: (L & %016" PRIx64 ") == %016" PRIx64 ", L = lo(((seed ^ l0) + t) * P1),"
           " density 2^-%d\n", p->lmask, p->lval, kbits);
    printf("  conditional: N = 2^%.2f seeds in the class: %" PRIu64 " collisions, rate 2^%.2f\n",
           log2((double)N), chits, crate);
    printf("  => estimated class contribution = 2^-%d * 2^%.2f = 2^%.2f  (published 2^%.2f)\n",
           kbits, crate, crate - kbits, p->claimed_eps_log2);
    if (chits) printf("  first class colliding seed: %016" PRIx64 "\n", cfirst);

    /* (c) the explicit published seed */
    uint64_t s = p->example_seed, h1 = t1ha2_64(m1, n1, s), h2 = t1ha2_64(m2, n2, s);
    uint64_t Ls = lmap_L(&L, s);
    printf("  explicit seed %016" PRIx64 ": H(m1) = %016" PRIx64 "  H(m2) = %016" PRIx64 "  %s"
           " (L = %016" PRIx64 ", in class: %s)\n", s, h1, h2, h1 == h2 ? "COLLIDE" : "DIFFER",
           Ls, (Ls & p->lmask) == p->lval ? "yes" : "no");
    if (h1 != h2) { fprintf(stderr, "published seed does not collide\n"); exit(1); }
}

int main(int argc, char **argv) {
    uint64_t N = UINT64_C(1) << 24, rngseed = 1;
    char *end = NULL;
    if (argc > 1) { uint64_t v = strtoull(argv[1], &end, 0); N = v <= 63 ? UINT64_C(1) << v : v;
                    if (end == argv[1] || *end) { fprintf(stderr, "usage: %s [log2 seeds (or a count above 63)] [rng seed]\n", argv[0]); return 1; } }
    if (argc > 2) { rngseed = strtoull(argv[2], &end, 0);
                    if (end == argv[2] || *end) { fprintf(stderr, "usage: %s [log2 seeds (or a count above 63)] [rng seed]\n", argv[0]); return 1; } }
    if (N == 0) { fprintf(stderr, "need at least one seed\n"); return 1; }
    { uint16_t e = 1; if (*(uint8_t *)&e != 1) { fprintf(stderr, "little-endian host required\n"); return 1; } }
    printf("t1ha2-64 (t1ha2_atonce, 64-bit output, 64-bit seed): fixed message pairs under random seeds\n");
    validate();
    printf("rng: xoshiro256** seeded by splitmix64(%" PRIu64 " + pair index)\n", rngseed);
    for (size_t i = 0; i < sizeof PAIRS / sizeof PAIRS[0]; i++) run_pair(&PAIRS[i], N, rngseed + i);
    return 0;
}
