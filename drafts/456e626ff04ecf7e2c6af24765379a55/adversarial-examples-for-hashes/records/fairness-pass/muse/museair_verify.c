/*
 * museair_verify.c -- MuseAir v0.3: a key-free (seed-independent) collision pair.
 *
 * Copyright (c) 2026 Thomas Dybdahl Ahle.  MIT License.
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
 * to whom the Software is furnished to do so, subject to the following conditions: the above
 * copyright notice and this permission notice shall be included in all copies or substantial
 * portions of the Software.  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * The MuseAir implementation below was written from the SMHasher3 reference
 * hashes/museair.cpp ("MuseAir v0.3, by K--Aethiax", released under CC0 1.0;
 * upstream https://github.com/eternal-io/museair).  It reproduces all four
 * SMHasher3 variants (MuseAir, MuseAir__bfast, MuseAir_128, MuseAir_128__bfast)
 * and is validated at startup against their SMHasher3 verification values.
 *
 * Build:  cc -O2 -std=c11 -o museair_verify museair_verify.c -lm
 * Run:    ./museair_verify [log2 N = 24] [rng seed = 0x243F6A8885A308D3]
 *
 * What it shows: for 17 <= len <= 32 the bytes 16..len-1 of a message enter
 * MuseAir only through a public, seed-free function P(u,v) that is XORed onto
 * the same two 64-bit words (i,j) as the first 16 bytes.  So for ANY two tails
 * a 16-byte head fix-up gives two messages with identical (i,j) for EVERY seed:
 * a collision with probability 1, in all four variants.
 */
#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/* MuseAir v0.3 (from SMHasher3 hashes/museair.cpp)                    */
/* ------------------------------------------------------------------ */

/* "AiryAi(0) mantissas calculated by Y-Cruncher" */
static const uint64_t K[7] = {
    UINT64_C(0x5ae31e589c56e17a), UINT64_C(0x96d7bb04e64f6da9),
    UINT64_C(0x7ab1006b26f9eb64), UINT64_C(0x21233394220b8457),
    UINT64_C(0x047cb9557c9f3b43), UINT64_C(0xd24f2590c0bcee28),
    UINT64_C(0x33ea8f71bb6016d8),
};

static void mul128(uint64_t a, uint64_t b, uint64_t *lo, uint64_t *hi) {
#ifdef __SIZEOF_INT128__
    unsigned __int128 r = (unsigned __int128)a * b;
    *lo = (uint64_t)r;
    *hi = (uint64_t)(r >> 64);
#else
    uint64_t a0 = (uint32_t)a, a1 = a >> 32, b0 = (uint32_t)b, b1 = b >> 32;
    uint64_t p00 = a0 * b0, p01 = a0 * b1, p10 = a1 * b0, p11 = a1 * b1;
    uint64_t mid = (p00 >> 32) + (uint32_t)p01 + (uint32_t)p10;
    *lo = (mid << 32) | (uint32_t)p00;
    *hi = p11 + (p01 >> 32) + (p10 >> 32) + (mid >> 32);
#endif
}

static uint64_t rotl64(uint64_t x, int r) { r &= 63; return (x << r) | (x >> ((64 - r) & 63)); }
static uint64_t rotr64(uint64_t x, int r) { r &= 63; return (x >> r) | (x << ((64 - r) & 63)); }

/* Little-endian loads/stores, byte-wise so the program is endian-independent. */
static uint32_t ld32(const uint8_t *p) {
    return (uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24;
}
static uint64_t ld64(const uint8_t *p) { return (uint64_t)ld32(p) | (uint64_t)ld32(p + 4) << 32; }
static void st32(uint8_t *p, uint32_t v) { for (int k = 0; k < 4; k++) p[k] = (uint8_t)(v >> (8 * k)); }
static void st64(uint8_t *p, uint64_t v) { st32(p, (uint32_t)v); st32(p + 4, (uint32_t)(v >> 32)); }

static void read_short(const uint8_t *b, size_t len, uint64_t *i, uint64_t *j) {
    if (len >= 4) {
        int off = (int)((len & 24) >> (len >> 3)); /* len >= 8 ? 4 : 0 */
        *i = ((uint64_t)ld32(b) << 32) | ld32(b + len - 4);
        *j = ((uint64_t)ld32(b + off) << 32) | ld32(b + len - 4 - off);
    } else if (len > 0) {
        *i = ((uint64_t)b[0] << 48) | ((uint64_t)b[len >> 1] << 24) | (uint64_t)b[len - 1];
        *j = 0;
    } else {
        *i = 0;
        *j = 0;
    }
}

static void mumix(int bfast, uint64_t *p, uint64_t *q, uint64_t ip, uint64_t iq) {
    if (!bfast) {
        uint64_t lo, hi;
        *p ^= ip;
        *q ^= iq;
        mul128(*p, *q, &lo, &hi);
        *p ^= lo;
        *q ^= hi;
    } else {
        mul128(*p ^ ip, *q ^ iq, p, q);
    }
}

static void hash_short(const uint8_t *b, size_t len, uint64_t seed, int bfast, int b128,
                       uint64_t *out_lo, uint64_t *out_hi) {
    uint64_t lo0, lo1, lo2, hi0, hi1, hi2, i, j;
    mul128(seed ^ K[0], (uint64_t)len ^ K[1], &lo2, &hi2);
    read_short(b, len <= 16 ? len : 16, &i, &j);
    i ^= (uint64_t)len ^ lo2;
    j ^= seed ^ hi2;
    if (len > 16) { /* the tail meets no secret: P(u,v) is a public function */
        uint64_t u, v;
        read_short(b + 16, len - 16, &u, &v);
        mul128(K[2], K[3] ^ u, &lo0, &hi0);
        mul128(K[4], K[5] ^ v, &lo1, &hi1);
        i ^= lo0 ^ hi1;
        j ^= lo1 ^ hi0;
    }
    if (b128) {
        mul128(i, j, &lo0, &hi0);
        mul128(i ^ K[2], j ^ K[3], &lo1, &hi1);
        i = lo0 ^ hi1;
        j = lo1 ^ hi0;
        mul128(i, j, &lo0, &hi0);
        mul128(i ^ K[4], j ^ K[5], &lo1, &hi1);
        *out_lo = lo0 ^ hi1; *out_hi = lo1 ^ hi0;
    } else {
        mul128(i ^ K[2], j ^ K[3], &lo2, &hi2);
        if (!bfast) { i ^= lo2; j ^= hi2; } else { i = lo2; j = hi2; }
        mul128(i ^ K[4], j ^ K[5], &lo2, &hi2);
        *out_lo = bfast ? (lo2 ^ hi2) : (i ^ j ^ lo2 ^ hi2);
        *out_hi = 0;
    }
}

static void hash_loong(const uint8_t *p, size_t len, uint64_t seed, int bfast, int b128,
                       uint64_t *out_lo, uint64_t *out_hi) {
    size_t q = len;
    uint64_t lo[6], hi[6], i, j, k;
    uint64_t s[6] = { K[0] + seed, K[1] - seed, K[2] ^ seed, K[3] + seed, K[4] - seed, K[5] ^ seed };
    lo[5] = K[6];
    if (q >= 96) {
        do { /* round t: words 2t,2t+1 into state[t],state[t+1 mod 6]; chained by lo[t-1] */
            for (int t = 0; t < 6; t++) {
                int a = t, c = (t + 1) % 6;
                s[a] ^= ld64(p + 16 * t);
                s[c] ^= ld64(p + 16 * t + 8);
                mul128(s[a], s[c], &lo[t], &hi[t]);
                if (!bfast) s[a] += lo[(t + 5) % 6] ^ hi[t];
                else        s[a]  = lo[(t + 5) % 6] ^ hi[t];
            }
            p += 96; q -= 96;
        } while (q >= 96);
        s[0] ^= lo[5];
    }
    if (q >= 48) {
        mumix(bfast, &s[0], &s[1], ld64(p), ld64(p + 8));
        mumix(bfast, &s[2], &s[3], ld64(p + 16), ld64(p + 24));
        mumix(bfast, &s[4], &s[5], ld64(p + 32), ld64(p + 40));
        p += 48; q -= 48;
    }
    if (q >= 16) {
        mumix(bfast, &s[0], &s[3], ld64(p), ld64(p + 8));
        if (q >= 32) mumix(bfast, &s[1], &s[4], ld64(p + 16), ld64(p + 24));
    }
    mumix(bfast, &s[2], &s[5], ld64(p + q - 16), ld64(p + q - 8));
    /* epilogue */
    i = rotl64(s[0] - s[1], (int)(len & 63));
    j = rotr64(s[2] - s[3], (int)(len & 63));
    k = (s[4] - s[5]) ^ (uint64_t)len;
    mul128(i, j, &lo[0], &hi[0]);
    mul128(j, k, &lo[1], &hi[1]);
    mul128(k, i, &lo[2], &hi[2]);
    i = lo[0] ^ hi[2]; j = lo[1] ^ hi[0]; k = lo[2] ^ hi[1];
    mul128(i, j, &lo[0], &hi[0]);
    mul128(j, k, &lo[1], &hi[1]);
    mul128(k, i, &lo[2], &hi[2]);
    if (b128) { *out_lo = lo[0] ^ lo[1] ^ hi[2]; *out_hi = hi[0] ^ hi[1] ^ lo[2]; }
    else      { *out_lo = (lo[0] ^ hi[2]) + (lo[1] ^ hi[0]) + (lo[2] ^ hi[1]); *out_hi = 0; }
}

/* Writes 8 bytes (64-bit variants) or 16 bytes (128-bit variants), little-endian. */
static void museair(const uint8_t *in, size_t len, uint64_t seed, int bfast, int b128, uint8_t *out) {
    uint64_t lo, hi;
    if (len <= 32) hash_short(in, len, seed, bfast, b128, &lo, &hi);
    else           hash_loong(in, len, seed, bfast, b128, &lo, &hi);
    st64(out, lo);
    if (b128) st64(out + 8, hi);
}

/* ------------------------------------------------------------------ */
/* Variants and SMHasher3 verification                                 */
/* ------------------------------------------------------------------ */

typedef struct { const char *name; int bfast, b128; uint32_t verification; } variant_t;
static const variant_t VARIANTS[4] = {
    { "MuseAir",            0, 0, 0xF89F1683u },
    { "MuseAir__bfast",     1, 0, 0xC61BEE56u },
    { "MuseAir_128",        0, 1, 0xD3DFE238u },
    { "MuseAir_128__bfast", 1, 1, 0x27939BF1u },
};
static int hashbytes(const variant_t *v) { return v->b128 ? 16 : 8; }

/* SMHasher3 lib/Hashinfo.cpp _ComputedVerifyImpl: hash keys {}, {0}, {0,1}, ...,
 * {0..254} with seed 256-i, hash the concatenated outputs with seed 0, take
 * the first 4 bytes little-endian. */
static uint32_t smhasher3_verification(const variant_t *v) {
    int hb = hashbytes(v);
    uint8_t key[256], hashes[16 * 256], total[16];
    memset(key, 0, sizeof key);
    memset(hashes, 0, sizeof hashes);
    for (int i = 0; i < 256; i++) {
        museair(key, (size_t)i, (uint64_t)(256 - i), v->bfast, v->b128, hashes + i * hb);
        key[i] = (uint8_t)i;
    }
    museair(hashes, (size_t)hb * 256, 0, v->bfast, v->b128, total);
    return (uint32_t)total[0] | (uint32_t)total[1] << 8 | (uint32_t)total[2] << 16 | (uint32_t)total[3] << 24;
}

/* ------------------------------------------------------------------ */
/* The published pairs                                                 */
/* ------------------------------------------------------------------ */

typedef struct {
    const char *label;
    int len;
    const char *m_hex, *m2_hex;   /* copied exactly from the record */
    uint64_t example_seed;
    const char *example_hash;     /* MuseAir (64-bit, plain) output at example_seed */
} pair_t;

static const pair_t PAIRS[] = {
    { "17-byte pair (L = ceil(17/8) = 3 words, score cap <= 1.59 bits)", 17,
      "0000000000000000000000000000000000",
      "8079763bb19a00001a9a1100642d3a3f01",
      UINT64_C(0x2cb0f69f4abea221), "d4ed417ecc529ae4" },
    { "24-byte pair (L = 3 words, score cap <= 1.59 bits)", 24,
      "000000000000000000000000000000000000000000000000",
      "7cd5c18245c15e8ef47bfef8b79181a80101010101010101",
      UINT64_C(0x2cb0f69f4abea221), "b7eb6f6095f1eeb9" },
};
#define NPAIRS ((int)(sizeof PAIRS / sizeof PAIRS[0]))

static int unhex(const char *s, uint8_t *out) {
    int n = 0;
    for (; s[0] && s[1]; s += 2, n++) {
        unsigned v;
        if (sscanf(s, "%2x", &v) != 1) { fprintf(stderr, "bad hex\n"); exit(2); }
        out[n] = (uint8_t)v;
    }
    return n;
}
static void print_hex(const uint8_t *p, int n) { for (int k = 0; k < n; k++) printf("%02x", p[k]); }

/* Mechanism: the public tail contribution P(u,v) for 16 < len <= 32. */
static void tail_contrib(const uint8_t *m, int len, uint64_t *pi, uint64_t *pj) {
    uint64_t u, v, lo0, hi0, lo1, hi1;
    read_short(m + 16, (size_t)(len - 16), &u, &v);
    mul128(K[2], K[3] ^ u, &lo0, &hi0);
    mul128(K[4], K[5] ^ v, &lo1, &hi1);
    *pi = lo0 ^ hi1; *pj = lo1 ^ hi0;
}

/* Given M and a different tail, build the partner M2 = (fixed head | new tail)
 * whose (i, j) equals M's for every seed.  read_short on 16 bytes reads
 * i = u32@0<<32 | u32@12 and j = u32@4<<32 | u32@8, a bijection of the head. */
static void derive_partner(const uint8_t *m, int len, const uint8_t *tail2, uint8_t *m2) {
    uint64_t i, j, pi, pj, qi, qj;
    memcpy(m2, m, (size_t)len);
    memcpy(m2 + 16, tail2, (size_t)(len - 16));
    read_short(m, 16, &i, &j);
    tail_contrib(m, len, &pi, &pj);
    tail_contrib(m2, len, &qi, &qj);
    i ^= pi ^ qi; j ^= pj ^ qj;
    st32(m2 + 0, (uint32_t)(i >> 32)); st32(m2 + 12, (uint32_t)i);
    st32(m2 + 4, (uint32_t)(j >> 32)); st32(m2 + 8, (uint32_t)j);
}

/* ------------------------------------------------------------------ */
/* RNG: splitmix64 -> xoshiro256**                                      */
/* ------------------------------------------------------------------ */

static uint64_t splitmix64(uint64_t *s) {
    uint64_t z = (*s += UINT64_C(0x9E3779B97F4A7C15));
    z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
    z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
    return z ^ (z >> 31);
}
typedef struct { uint64_t s[4]; } rng_t;
static void rng_init(rng_t *g, uint64_t seed) { for (int k = 0; k < 4; k++) g->s[k] = splitmix64(&seed); }
static uint64_t rng_next(rng_t *g) {
    uint64_t *s = g->s, r = rotl64(s[1] * 5, 7) * 9, t = s[1] << 17;
    s[2] ^= s[0]; s[3] ^= s[1]; s[1] ^= s[2]; s[0] ^= s[3]; s[2] ^= t; s[3] = rotl64(s[3], 45);
    return r;
}

/* ------------------------------------------------------------------ */

static void print_rate(uint64_t coll, uint64_t n, int log2n) {
    printf("%" PRIu64 " / %" PRIu64 " = %.6f", coll, n, (double)coll / (double)n);
    if (coll == 0) printf("  (none observed in 2^%d trials; no population bound)\n", log2n);
    else           printf("  (log2 rate = %.4f)\n", log2((double)coll / (double)n));
}

int main(int argc, char **argv) {
    char *end = NULL;
    int log2n = argc > 1 ? (int)strtol(argv[1], &end, 10) : 24;
    if (argc > 1 && (end == argv[1] || *end)) { fprintf(stderr, "usage: %s [log2 N = 24] [rng seed]\n", argv[0]); return 2; }
    uint64_t rng_seed = argc > 2 ? strtoull(argv[2], &end, 0) : UINT64_C(0x243F6A8885A308D3);
    if (argc > 2 && (end == argv[2] || *end)) { fprintf(stderr, "usage: %s [log2 N = 24] [rng seed]\n", argv[0]); return 2; }
    if (log2n < 0 || log2n > 40) { fprintf(stderr, "log2 N must be in 0..40\n"); return 2; }
    uint64_t n = (uint64_t)1 << log2n;

    /* 1. Validation against SMHasher3 (abort on failure). */
    printf("== MuseAir v0.3 (SMHasher3 hashes/museair.cpp): verification values ==\n");
    for (int v = 0; v < 4; v++) {
        uint32_t got = smhasher3_verification(&VARIANTS[v]);
        printf("  %-20s computed 0x%08" PRIX32 " expected 0x%08" PRIX32 " %s\n", VARIANTS[v].name, got,
               VARIANTS[v].verification, got == VARIANTS[v].verification ? "PASS" : "FAIL");
        if (got != VARIANTS[v].verification) { fflush(stdout); fprintf(stderr, "validation failed; exiting with status 1\n"); return 1; }
    }

    uint8_t m[NPAIRS][32] = {{0}}, m2[NPAIRS][32] = {{0}};   /* zero-filled: the control copies 32 bytes */
    for (int p = 0; p < NPAIRS; p++) {
        const pair_t *P = &PAIRS[p];
        int l1 = unhex(P->m_hex, m[p]), l2 = unhex(P->m2_hex, m2[p]);
        if (l1 != P->len || l2 != P->len || memcmp(m[p], m2[p], (size_t)l1) == 0) {
            fprintf(stderr, "pair %d malformed\n", p); return 2;
        }
        /* 2. The pair and the mechanism check. */
        printf("\n== %s ==\n", P->label);
        printf("  M  = "); print_hex(m[p], P->len); printf("\n");
        printf("  M2 = "); print_hex(m2[p], P->len); printf("\n");
        uint8_t derived[32];
        derive_partner(m[p], P->len, m2[p] + 16, derived);
        printf("  head of M2 derived from the public tail function P(u,v): ");
        print_hex(derived, 16);
        printf("  -> %s\n", memcmp(derived, m2[p], (size_t)P->len) == 0 ? "matches M2" : "DOES NOT MATCH");
    }

    /* 3. Uniformly random seeds. */
    printf("\n== %" PRIu64 " (2^%d) uniformly random 64-bit seeds (xoshiro256**, seed 0x%016" PRIx64 ") ==\n",
           n, log2n, rng_seed);
    uint64_t coll[NPAIRS][4] = { { 0 } };
    uint64_t first_seed = 0;
    rng_t g;
    rng_init(&g, rng_seed);
    for (uint64_t t = 0; t < n; t++) {
        uint64_t seed = rng_next(&g);
        if (t == 0) first_seed = seed;
        for (int p = 0; p < NPAIRS; p++)
            for (int v = 0; v < 4; v++) {
                uint8_t h1[16], h2[16];
                museair(m[p], (size_t)PAIRS[p].len, seed, VARIANTS[v].bfast, VARIANTS[v].b128, h1);
                museair(m2[p], (size_t)PAIRS[p].len, seed, VARIANTS[v].bfast, VARIANTS[v].b128, h2);
                coll[p][v] += memcmp(h1, h2, (size_t)hashbytes(&VARIANTS[v])) == 0;
            }
    }
    for (int p = 0; p < NPAIRS; p++) {
        printf("  %d-byte pair:\n", PAIRS[p].len);
        for (int v = 0; v < 4; v++) {
            printf("    %-20s collisions ", VARIANTS[v].name);
            print_rate(coll[p][v], n, log2n);
        }
    }

    /* 4. Explicit colliding seeds with both hash values. */
    printf("\n== explicit colliding seeds ==\n");
    for (int p = 0; p < NPAIRS; p++) {
        const pair_t *P = &PAIRS[p];
        uint64_t seeds[2] = { P->example_seed, first_seed };
        for (int s = 0; s < 2; s++) {
            printf("  %d-byte pair, seed 0x%016" PRIx64 "%s:\n", P->len, seeds[s],
                   s == 0 ? " (published example)" : " (first seed of the sweep)");
            for (int v = 0; v < 4; v++) {
                uint8_t h1[16], h2[16];
                int hb = hashbytes(&VARIANTS[v]);
                museair(m[p], (size_t)P->len, seeds[s], VARIANTS[v].bfast, VARIANTS[v].b128, h1);
                museair(m2[p], (size_t)P->len, seeds[s], VARIANTS[v].bfast, VARIANTS[v].b128, h2);
                printf("    %-20s H(M) = ", VARIANTS[v].name); print_hex(h1, hb);
                printf("  H(M2) = "); print_hex(h2, hb);
                printf("  %s\n", memcmp(h1, h2, (size_t)hb) == 0 ? "EQUAL" : "differ");
                if (s == 0 && v == 0) { /* bytes above are LE; the record quotes the u64 */
                    char hex[17];
                    snprintf(hex, sizeof hex, "%016" PRIx64, ld64(h1));
                    printf("      as u64 0x%s, published 0x%s: %s\n", hex, P->example_hash,
                           strcmp(hex, P->example_hash) == 0 ? "reproduced" : "MISMATCH");
                }
            }
        }
    }

    /* 5. Control: the same tail change WITHOUT the head fix-up does not collide. */
    uint64_t ctrl_n = n >> 8 ? n >> 8 : 1, ctrl = 0;
    uint8_t m3[32];
    memcpy(m3, m[0], 32);
    memcpy(m3 + 16, m2[0] + 16, (size_t)(PAIRS[0].len - 16));
    rng_init(&g, rng_seed ^ 1);
    for (uint64_t t = 0; t < ctrl_n; t++) {
        uint64_t seed = rng_next(&g);
        uint8_t h1[8], h3[8];
        museair(m[0], (size_t)PAIRS[0].len, seed, 0, 0, h1);
        museair(m3, (size_t)PAIRS[0].len, seed, 0, 0, h3);
        ctrl += memcmp(h1, h3, 8) == 0;
    }
    printf("\n== control: M vs (head of M | tail of M2), 17 bytes, MuseAir ==\n  collisions ");
    print_rate(ctrl, ctrl_n, log2n - 8 > 0 ? log2n - 8 : 0);
    return 0;
}
