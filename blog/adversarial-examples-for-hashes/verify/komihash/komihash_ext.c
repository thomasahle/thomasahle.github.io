/*
 * komihash_ext.c -- extended measurements of the lane-tie collision family of
 * komihash 5.34 (64..127-byte messages, one pass of the 8-lane loop).
 *
 * Build:  cc -O2 -std=c11 -o komihash_ext komihash_ext.c -lm
 * Usage:  ./komihash_ext <experiment> [log2 seeds = 20] [bases = 200] [rng seed hex = c0ffee]
 *
 * Experiments:
 *   arb      tie on lanes 1,2 only; free words random; m4 random vs published
 *   free     per-seed outcome does not depend on m2,m3,m6,m7 (identity check)
 *   s5bias   bit bias of S5 after the init round; E[v]/2^64 for v = m4 ^ S5
 *   tie34    second tie on lanes 3,4; flip bit j (j = 0 and j = 2)
 *   multi    both ties; the 4 messages {none, A, B, AB}; 4-way rate
 *   all8     all four lanes tied to lane 1; 8 even flip sets; largest-subset histogram
 *   low3     lanes 1,2 tied; vary the low 3 bits of m0 (8 messages); histogram
 *   len      the 4-message set at lengths 64, 96, 127, 128
 *   control  the same flip without the tie, or with half of it (expect no collisions)
 *
 * Message layout: eight 64-bit little-endian words m0..m7 at byte offsets
 * 0, 8, ..., 56.  In the loop, lane i (1..4) multiplies (m_{i-1} ^ Seed_i) by
 * (m_{i+3} ^ Seed_{i+4}); Seed2..4 = Seed1 ^ IVAL2..4, Seed6..8 = Seed5 ^ IVAL6..8.
 */
#include <inttypes.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "komihash.h"

/* The IVAL constants are #undef'd at the end of komihash.h; restate them. */
#define IVAL2 UINT64_C(0x13198A2E03707344)
#define IVAL3 UINT64_C(0xA4093822299F31D0)
#define IVAL4 UINT64_C(0x082EFA98EC4E6C89)
#define IVAL6 UINT64_C(0xBE5466CF34E90C6C)
#define IVAL7 UINT64_C(0xC0AC29B7C97C50DD)
#define IVAL8 UINT64_C(0x3F84D5B5B5470917)

/* Published pair-1 words (blog verify/ directory), little-endian. */
static const uint64_t PUB_M0 = 0, PUB_M2 = UINT64_C(0x1111111111111111);
static const uint64_t PUB_M4 = UINT64_C(0x5ea1c0c1a4ddf9f0);

/* ---- RNG: splitmix64 seeding xoshiro256** (public domain) ---- */
static uint64_t splitmix64(uint64_t *x) {
    uint64_t z = (*x += UINT64_C(0x9E3779B97F4A7C15));
    z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
    z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
    return z ^ (z >> 31);
}
typedef struct { uint64_t s[4]; } rng_t;
static void rng_init(rng_t *r, uint64_t seed) { for (int k = 0; k < 4; k++) r->s[k] = splitmix64(&seed); }
static inline uint64_t rotl64(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
static inline uint64_t rng_next(rng_t *r) {
    uint64_t *s = r->s, res = rotl64(s[1] * 5, 7) * 9, t = s[1] << 17;
    s[2] ^= s[0]; s[3] ^= s[1]; s[1] ^= s[2]; s[0] ^= s[3]; s[2] ^= t; s[3] = rotl64(s[3], 45);
    return res;
}

/* ---- helpers ---- */
static void die(const char *what) { fprintf(stderr, "FATAL: %s\n", what); exit(1); }
static void st64le(uint8_t *p, uint64_t v) { for (int i = 0; i < 8; i++) p[i] = (uint8_t)(v >> (8 * i)); }
static void build(uint8_t *msg, const uint64_t w[8]) { for (int i = 0; i < 8; i++) st64le(msg + 8 * i, w[i]); }
static uint64_t mulhi64(uint64_t a, uint64_t b) {
    uint64_t a0 = (uint32_t)a, a1 = a >> 32, b0 = (uint32_t)b, b1 = b >> 32;
    uint64_t p00 = a0 * b0, p01 = a0 * b1, p10 = a1 * b0, p11 = a1 * b1;
    uint64_t mid = (p00 >> 32) + (uint32_t)p01 + (uint32_t)p10;
    return p11 + (p01 >> 32) + (p10 >> 32) + (mid >> 32);
}
static int cmp_u64(const void *a, const void *b) {
    uint64_t x = *(const uint64_t *)a, y = *(const uint64_t *)b; return x < y ? -1 : x > y;
}
static int cmp_dbl(const void *a, const void *b) {
    double x = *(const double *)a, y = *(const double *)b; return x < y ? -1 : x > y;
}
/* largest number of equal values among n outputs */
static int largest_class(uint64_t *h, int n) {
    qsort(h, (size_t)n, sizeof *h, cmp_u64);
    int best = 1, run = 1;
    for (int i = 1; i < n; i++) { run = h[i] == h[i - 1] ? run + 1 : 1; if (run > best) best = run; }
    return best;
}
static void print_dist(const char *label, double *v, int n) {
    qsort(v, (size_t)n, sizeof *v, cmp_dbl);
    double mean = 0; for (int i = 0; i < n; i++) mean += v[i]; mean /= n;
    printf("  %s over %d bases: min %.4f  median %.4f  mean %.4f  max %.4f\n", label, n, v[0], v[n / 2], mean, v[n - 1]);
}
static void print_words(const char *label, const uint64_t w[8]) {
    printf("  %s:", label);
    for (int i = 0; i < 8; i++) printf(" %016" PRIx64, w[i]);
    printf("\n");
}

/* Ties.  tie12: lanes 1 and 2 multiply identical operands for every seed.
 * tie34: lanes 3 and 4 likewise.  tieall: all four lanes equal lane 1. */
static void tie12(uint64_t w[8]) { w[1] = w[0] ^ IVAL2; w[5] = w[4] ^ IVAL6; }
static void tie34(uint64_t w[8]) { w[3] = w[2] ^ (IVAL3 ^ IVAL4); w[7] = w[6] ^ (IVAL7 ^ IVAL8); }
static void tieall(uint64_t w[8]) {
    w[1] = w[0] ^ IVAL2; w[2] = w[0] ^ IVAL3; w[3] = w[0] ^ IVAL4;
    w[5] = w[4] ^ IVAL6; w[6] = w[4] ^ IVAL7; w[7] = w[4] ^ IVAL8;
}
/* flip bit j of words a and b */
static void flip(uint64_t w[8], int a, int b, int j) { w[a] ^= UINT64_C(1) << j; w[b] ^= UINT64_C(1) << j; }

/* ---- experiment 1: arbitrary free words, lanes 1,2 tied, flip bit 0 of m0,m1 ---- */
static void run_arb(uint64_t N, int bases, uint64_t rngseed, int pub_m4) {
    printf("\n[arb] lanes 1,2 tied (m1 = m0^IVAL2, m5 = m4^IVAL6); m0,m2,m3,m6,m7 uniform; m4 %s\n",
           pub_m4 ? "= published 0x5ea1c0c1a4ddf9f0" : "uniform");
    printf("  pair = flip bit 0 of m0 and m1; %d bases x 2^%.0f seeds (same seed stream for every base)\n",
           bases, log2((double)N));
    double *rate = malloc(sizeof(double) * (size_t)bases), *nocarry = malloc(sizeof(double) * (size_t)bases);
    double *given_carry = malloc(sizeof(double) * (size_t)bases);
    rng_t rb; rng_init(&rb, rngseed ^ UINT64_C(0xa1));
    double worst = 2, best = -1; uint64_t worst_w[8] = {0}, best_w[8] = {0};
    for (int b = 0; b < bases; b++) {
        uint64_t w[8]; for (int i = 0; i < 8; i++) w[i] = rng_next(&rb);
        if (pub_m4) w[4] = PUB_M4;
        tie12(w);
        uint64_t w2[8]; memcpy(w2, w, sizeof w); flip(w2, 0, 1, 0);
        uint8_t A[64], B[64]; build(A, w); build(B, w2);
        rng_t r; rng_init(&r, rngseed);
        uint64_t coll = 0, nc = 0, coll_nc = 0;
        for (uint64_t i = 0; i < N; i++) {
            uint64_t seed = rng_next(&r), s1, s5;
            komihash_set_preseed_inner(&s1, &s5, seed);
            int eq = mulhi64(s1 ^ w[0], s5 ^ w[4]) == mulhi64(s1 ^ w2[0], s5 ^ w[4]);
            int c = komihash(A, 64, seed) == komihash(B, 64, seed);
            coll += (uint64_t)c; nc += (uint64_t)eq; coll_nc += (uint64_t)(c & eq);
            if (!c && eq) die("collision predicted (no carry) but not observed");
        }
        rate[b] = (double)coll / (double)N; nocarry[b] = (double)nc / (double)N;
        given_carry[b] = (double)(coll - coll_nc) / (double)(N - nc);
        if (rate[b] < worst) { worst = rate[b]; memcpy(worst_w, w, sizeof w); }
        if (rate[b] > best) { best = rate[b]; memcpy(best_w, w, sizeof w); }
    }
    print_dist("collision rate", rate, bases);
    print_dist("P(no carry into the high half)", nocarry, bases);
    print_dist("P(collision | carry) (predicted 3/4)", given_carry, bases);
    printf("  every no-carry seed collided (checked per seed)\n");
    print_words("worst base (m0..m7)", worst_w);
    print_words("best base (m0..m7)", best_w);
    free(rate); free(nocarry); free(given_carry);
}

/* ---- identity check: m2,m3,m6,m7 do not change the per-seed outcome ---- */
static void run_free(uint64_t N, int bases, uint64_t rngseed) {
    printf("\n[free] published m0,m4 with lanes 1,2 tied; %d random fills of m2,m3,m6,m7; 2^%.0f seeds\n",
           bases, log2((double)N));
    uint8_t *ref = malloc(N);
    rng_t rb; rng_init(&rb, rngseed ^ UINT64_C(0xf2));
    uint64_t mism = 0, coll0 = 0;
    for (int b = 0; b < bases; b++) {
        uint64_t w[8]; for (int i = 0; i < 8; i++) w[i] = rng_next(&rb);
        w[0] = PUB_M0; w[4] = PUB_M4; tie12(w);
        uint64_t w2[8]; memcpy(w2, w, sizeof w); flip(w2, 0, 1, 0);
        uint8_t A[64], B[64]; build(A, w); build(B, w2);
        rng_t r; rng_init(&r, rngseed);
        for (uint64_t i = 0; i < N; i++) {
            uint64_t seed = rng_next(&r);
            uint8_t c = komihash(A, 64, seed) == komihash(B, 64, seed);
            if (b == 0) { ref[i] = c; coll0 += c; } else mism += (uint64_t)(c != ref[i]);
        }
    }
    printf("  fill 0: %" PRIu64 "/%" PRIu64 " collisions; per-seed outcome differs from fill 0 in %" PRIu64
           " of %" PRIu64 " (fill,seed) cells\n", coll0, N, mism, (uint64_t)(bases - 1) * N);
    free(ref);
}

/* ---- S5 bias after the init round ---- */
static void run_s5bias(uint64_t N, uint64_t rngseed) {
    printf("\n[s5bias] S5 after the init round, 2^%.0f uniform seeds\n", log2((double)N));
    uint64_t top[256] = {0}; double ev_pub = 0, ev_rand = 0; uint64_t bit[64] = {0};
    rng_t r; rng_init(&r, rngseed);
    uint64_t rand_m4 = splitmix64(&(uint64_t){rngseed ^ 0x55});
    for (uint64_t i = 0; i < N; i++) {
        uint64_t s1, s5; komihash_set_preseed_inner(&s1, &s5, rng_next(&r));
        top[s5 >> 56]++;
        for (int k = 0; k < 64; k++) bit[k] += (s5 >> k) & 1;
        ev_pub += ldexp((double)(s5 ^ PUB_M4), -64); ev_rand += ldexp((double)(s5 ^ rand_m4), -64);
    }
    int mode = 0; for (int k = 1; k < 256; k++) if (top[k] > top[mode]) mode = k;
    printf("  most common top byte of S5: 0x%02x with frequency %.4f (uniform would be %.4f); published m4 top byte 0x%02x\n",
           mode, (double)top[mode] / (double)N, 1.0 / 256, (unsigned)(PUB_M4 >> 56));
    printf("  P(bit k of S5 = 1) for k = 63..56:");
    for (int k = 63; k >= 56; k--) printf(" %.3f", (double)bit[k] / (double)N);
    printf("\n  E[(m4 ^ S5)/2^64]: published m4 %.4f; random m4 0x%016" PRIx64 " %.4f  (P(no carry) ~ 1 - E[v/2^64])\n",
           ev_pub / (double)N, rand_m4, ev_rand / (double)N);
}

/* ---- experiment 2: second tie on lanes 3,4; flip bit j of m2 and m3 ---- */
static void run_tie34(uint64_t N, int bases, uint64_t rngseed, int j, int tuned_m6) {
    printf("\n[tie34] lanes 3,4 tied (m3 = m2^(IVAL3^IVAL4), m7 = m6^(IVAL7^IVAL8)) on top of the lanes-1,2 tie;\n"
           "  pair = flip bit %d of m2 and m3; m0,m2,m4 uniform, m6 %s; %d bases x 2^%.0f seeds\n",
           j, tuned_m6 ? "= published m4 ^ IVAL7 (tuned)" : "uniform", bases, log2((double)N));
    printf("  IVAL3^IVAL4 = 0x%016" PRIx64 "  IVAL7^IVAL8 = 0x%016" PRIx64 " (bit %d: %d and %d)\n",
           IVAL3 ^ IVAL4, IVAL7 ^ IVAL8, j, (int)((IVAL3 ^ IVAL4) >> j & 1), (int)((IVAL7 ^ IVAL8) >> j & 1));
    double *rate = malloc(sizeof(double) * (size_t)bases), *nocarry = malloc(sizeof(double) * (size_t)bases);
    double *given_carry = malloc(sizeof(double) * (size_t)bases);
    rng_t rb; rng_init(&rb, rngseed ^ UINT64_C(0x34));
    for (int b = 0; b < bases; b++) {
        uint64_t w[8]; for (int i = 0; i < 8; i++) w[i] = rng_next(&rb);
        if (tuned_m6) w[6] = PUB_M4 ^ IVAL7;
        tie12(w); tie34(w);
        uint64_t w2[8]; memcpy(w2, w, sizeof w); flip(w2, 2, 3, j);
        uint8_t A[64], B[64]; build(A, w); build(B, w2);
        rng_t r; rng_init(&r, rngseed);
        uint64_t coll = 0, nc = 0, coll_nc = 0;
        for (uint64_t i = 0; i < N; i++) {
            uint64_t seed = rng_next(&r), s1, s5;
            komihash_set_preseed_inner(&s1, &s5, seed);
            /* lane 3 operands: (m2 ^ IVAL3 ^ S1) * (m6 ^ IVAL7 ^ S5) */
            int eq = mulhi64(s1 ^ IVAL3 ^ w[2], s5 ^ IVAL7 ^ w[6]) == mulhi64(s1 ^ IVAL3 ^ w2[2], s5 ^ IVAL7 ^ w[6]);
            int c = komihash(A, 64, seed) == komihash(B, 64, seed);
            coll += (uint64_t)c; nc += (uint64_t)eq; coll_nc += (uint64_t)(c & eq);
            if (!c && eq) die("collision predicted (high half unchanged) but not observed");
        }
        rate[b] = (double)coll / (double)N; nocarry[b] = (double)nc / (double)N;
        given_carry[b] = N == nc ? 0 : (double)(coll - coll_nc) / (double)(N - nc);
    }
    print_dist("collision rate", rate, bases);
    print_dist("P(lane-3 high half unchanged)", nocarry, bases);
    print_dist("P(collision | high half changed)", given_carry, bases);
    free(rate); free(nocarry); free(given_carry);
}

/* ---- experiment 3a: both ties, 4 messages {none, A, B, AB} ---- */
static void best_base(uint64_t w[8]) {
    w[0] = PUB_M0; w[2] = PUB_M2; w[4] = PUB_M4; w[6] = PUB_M4 ^ IVAL7; tie12(w); tie34(w);
}
static void four_set(const uint64_t w[8], uint8_t M[4][128], size_t len, uint8_t fill) {
    for (int k = 0; k < 4; k++) {
        uint64_t v[8]; memcpy(v, w, 8 * sizeof(uint64_t));
        if (k & 1) flip(v, 0, 1, 0);
        if (k & 2) flip(v, 2, 3, 0);
        build(M[k], v); memset(M[k] + 64, fill, 64);
        (void)len;
    }
}
static void multi_one(const uint64_t w[8], uint64_t N, uint64_t rngseed, size_t len, const char *label) {
    uint8_t M[4][128]; four_set(w, M, len, 0x99);
    rng_t r; rng_init(&r, rngseed);
    uint64_t cA = 0, cB = 0, c4 = 0, hist[5] = {0};
    for (uint64_t i = 0; i < N; i++) {
        uint64_t seed = rng_next(&r), h[4];
        for (int k = 0; k < 4; k++) h[k] = komihash(M[k], len, seed);
        cA += h[0] == h[1]; cB += h[0] == h[2];
        c4 += h[0] == h[1] && h[0] == h[2] && h[0] == h[3];
        hist[largest_class(h, 4)]++;
    }
    printf("  %s len %3zu: A-pair %.4f  B-pair %.4f  product %.4f  4-way %" PRIu64 "/%" PRIu64 " = %.4f  largest class {1:%.4f 2:%.4f 3:%.4f 4:%.4f}\n",
           label, len, (double)cA / (double)N, (double)cB / (double)N, (double)cA / (double)N * (double)cB / (double)N,
           c4, N, (double)c4 / (double)N,
           (double)hist[1] / (double)N, (double)hist[2] / (double)N, (double)hist[3] / (double)N, (double)hist[4] / (double)N);
}
static void run_multi(uint64_t N, int bases, uint64_t rngseed) {
    printf("\n[multi] both ties; messages {none, A = flip bit 0 of m0,m1; B = flip bit 0 of m2,m3; AB}; 2^%.0f seeds\n",
           log2((double)N));
    uint64_t w[8]; best_base(w); print_words("best base (m0..m7)", w);
    uint8_t M[4][128]; four_set(w, M, 64, 0x99);
    for (int k = 0; k < 4; k++) { printf("  msg %d = ", k); for (int i = 0; i < 64; i++) printf("%02x", M[k][i]); printf("\n"); }
    multi_one(w, N, rngseed, 64, "best base");
    printf("  %d random bases (all free words uniform, both ties), 2^%.0f seeds each:\n", bases, log2((double)N));
    double *r4 = malloc(sizeof(double) * (size_t)bases);
    rng_t rb; rng_init(&rb, rngseed ^ UINT64_C(0x44));
    for (int b = 0; b < bases; b++) {
        uint64_t v[8]; for (int i = 0; i < 8; i++) v[i] = rng_next(&rb);
        tie12(v); tie34(v);
        uint8_t Q[4][128]; four_set(v, Q, 64, 0x99);
        rng_t r; rng_init(&r, rngseed);
        uint64_t c4 = 0;
        for (uint64_t i = 0; i < N; i++) {
            uint64_t seed = rng_next(&r), h[4];
            for (int k = 0; k < 4; k++) h[k] = komihash(Q[k], 64, seed);
            c4 += h[0] == h[1] && h[0] == h[2] && h[0] == h[3];
        }
        r4[b] = (double)c4 / (double)N;
    }
    print_dist("4-way collision rate", r4, bases);
    free(r4);
}

/* ---- experiment 3b: all four lanes tied to lane 1; the 8 even flip sets ---- */
static void eight_set(const uint64_t w[8], uint8_t M[8][64]) {
    /* flip sets of even size over lanes {1,2,3,4}: {}, {12}, {34}, {13}, {24}, {14}, {23}, {1234} */
    static const int sets[8] = { 0x0, 0x3, 0xC, 0x5, 0xA, 0x9, 0x6, 0xF };
    for (int k = 0; k < 8; k++) {
        uint64_t v[8]; memcpy(v, w, 8 * sizeof(uint64_t));
        for (int lane = 0; lane < 4; lane++) if (sets[k] >> lane & 1) v[lane] ^= 1;
        build(M[k], v);
    }
}
static void all8_one(const uint64_t w[8], uint64_t N, uint64_t rngseed, const char *label, int verbose) {
    uint8_t M[8][64]; eight_set(w, M);
    if (verbose) for (int k = 0; k < 8; k++) { printf("  msg %d = ", k); for (int i = 0; i < 64; i++) printf("%02x", M[k][i]); printf("\n"); }
    rng_t r; rng_init(&r, rngseed);
    uint64_t hist[9] = {0}, pair[8] = {0}, nc = 0, c8 = 0, c8_nc = 0, odd_coll = 0;
    for (uint64_t i = 0; i < N; i++) {
        uint64_t seed = rng_next(&r), h[8], s1, s5;
        for (int k = 0; k < 8; k++) h[k] = komihash(M[k], 64, seed);
        komihash_set_preseed_inner(&s1, &s5, seed);
        int eq = mulhi64(s1 ^ w[0], s5 ^ w[4]) == mulhi64(s1 ^ w[0] ^ 1, s5 ^ w[4]);
        nc += (uint64_t)eq;
        for (int k = 1; k < 8; k++) pair[k] += h[k] == h[0];
        int all = 1; for (int k = 1; k < 8; k++) all &= h[k] == h[0];
        c8 += (uint64_t)all; c8_nc += (uint64_t)(all & eq);
        if (eq && !all) die("no carry but not an 8-way collision");
        /* control: a single-lane flip (odd set) must not collide */
        uint64_t v[8]; memcpy(v, w, sizeof v); v[0] ^= 1; uint8_t O[64]; build(O, v);
        odd_coll += komihash(O, 64, seed) == h[0];
        hist[largest_class(h, 8)]++;
    }
    printf("  %s: 8-way %" PRIu64 "/%" PRIu64 " = %.4f; P(no carry) = %.4f; 8-way given no carry = %" PRIu64 "/%" PRIu64
           "; 8-way given carry = %" PRIu64 "/%" PRIu64 " = %.4f\n", label, c8, N, (double)c8 / (double)N, (double)nc / (double)N,
           c8_nc, nc, c8 - c8_nc, N - nc, (double)(c8 - c8_nc) / (double)(N - nc));
    printf("    pair rates vs msg 0: {12} %.4f {34} %.4f {13} %.4f {24} %.4f {14} %.4f {23} %.4f {1234} %.4f; single-lane flip (control) %.6f\n",
           (double)pair[1] / (double)N, (double)pair[2] / (double)N, (double)pair[3] / (double)N, (double)pair[4] / (double)N,
           (double)pair[5] / (double)N, (double)pair[6] / (double)N, (double)pair[7] / (double)N, (double)odd_coll / (double)N);
    printf("    largest same-output class among the 8:");
    for (int k = 1; k <= 8; k++) if (hist[k]) printf(" %d:%.4f", k, (double)hist[k] / (double)N);
    printf("\n");
}
static void run_all8(uint64_t N, int bases, uint64_t rngseed) {
    printf("\n[all8] all lanes tied to lane 1 (m1..m3 = m0 ^ IVAL2..4, m5..m7 = m4 ^ IVAL6..8); messages = bit-0 flips of\n"
           "  an even-size set of lanes: {}, {1,2}, {3,4}, {1,3}, {2,4}, {1,4}, {2,3}, {1,2,3,4}; 2^%.0f seeds\n", log2((double)N));
    uint64_t w[8] = {0}; w[0] = PUB_M0; w[4] = PUB_M4; tieall(w);
    print_words("best base (m0..m7)", w);
    all8_one(w, N, rngseed, "best base (published m0, m4)", 1);
    rng_t rb; rng_init(&rb, rngseed ^ UINT64_C(0x88));
    for (int b = 0; b < bases; b++) {
        uint64_t v[8] = {0}; v[0] = rng_next(&rb); v[4] = rng_next(&rb); tieall(v);
        char label[64]; snprintf(label, sizeof label, "random base %d (m0 %016" PRIx64 " m4 %016" PRIx64 ")", b, v[0], v[4]);
        all8_one(v, N, rngseed, label, 0);
    }
}

/* ---- experiment 3c: vary the low 3 bits of m0 (lanes 1,2 tied) ---- */
static void low3_one(const uint64_t w[8], uint64_t N, uint64_t rngseed, const char *label) {
    uint8_t M[8][64];
    for (int t = 0; t < 8; t++) { uint64_t v[8]; memcpy(v, w, 8 * sizeof(uint64_t)); v[0] = (w[0] & ~UINT64_C(7)) | (uint64_t)t; tie12(v); build(M[t], v); }
    rng_t r; rng_init(&r, rngseed);
    uint64_t hist[9] = {0}, allhi = 0, c8 = 0;
    for (uint64_t i = 0; i < N; i++) {
        uint64_t seed = rng_next(&r), h[8], s1, s5;
        for (int t = 0; t < 8; t++) h[t] = komihash(M[t], 64, seed);
        komihash_set_preseed_inner(&s1, &s5, seed);
        uint64_t u0 = (s1 ^ w[0]) & ~UINT64_C(7), v = s5 ^ w[4], h0 = mulhi64(u0, v);
        int same = 1; for (int t = 1; t < 8; t++) same &= mulhi64(u0 | (uint64_t)t, v) == h0;
        allhi += (uint64_t)same;
        int lc = largest_class(h, 8); hist[lc]++; c8 += lc == 8;
        if (same && lc != 8) die("all eight high halves equal but not an 8-way collision");
    }
    printf("  %s: largest same-output class among the 8:", label);
    for (int k = 1; k <= 8; k++) if (hist[k]) printf(" %d:%.4f", k, (double)hist[k] / (double)N);
    printf("\n    8-way %" PRIu64 "/%" PRIu64 " = %.4f; P(all eight lane-1 high halves equal) = %.4f\n", c8, N, (double)c8 / (double)N, (double)allhi / (double)N);
}
static void run_low3(uint64_t N, int bases, uint64_t rngseed) {
    printf("\n[low3] lanes 1,2 tied; 8 messages = m0 with low 3 bits set to 0..7 (m1 re-tied); 2^%.0f seeds\n", log2((double)N));
    uint64_t w[8]; best_base(w);
    low3_one(w, N, rngseed, "best base (published m0, m4)");
    rng_t rb; rng_init(&rb, rngseed ^ UINT64_C(0x33));
    for (int b = 0; b < bases; b++) {
        uint64_t v[8]; for (int i = 0; i < 8; i++) v[i] = rng_next(&rb); tie12(v); tie34(v);
        char label[80]; snprintf(label, sizeof label, "random base %d (m0 %016" PRIx64 " m4 %016" PRIx64 ")", b, v[0], v[4]);
        low3_one(v, N, rngseed, label);
    }
}


/* ---- control: the same bit flips without the tie (or with half of it) ---- */
static void run_control(uint64_t N, int bases, uint64_t rngseed) {
    printf("\n[control] flip bit 0 of m0 and m1 on messages WITHOUT the tie (all 8 words uniform), and with only half of it; %d bases x 2^%.0f seeds\n",
           bases, log2((double)N));
    rng_t rb; rng_init(&rb, rngseed ^ UINT64_C(0xc0));
    uint64_t c_none = 0, c_half1 = 0, c_half5 = 0, c_full = 0;
    for (int b = 0; b < bases; b++) {
        uint64_t w[8]; for (int i = 0; i < 8; i++) w[i] = rng_next(&rb);
        uint64_t h1[8], h5[8], full[8];
        memcpy(h1, w, sizeof w); h1[1] = h1[0] ^ IVAL2;          /* m1 tied, m5 free */
        memcpy(h5, w, sizeof w); h5[5] = h5[4] ^ IVAL6;          /* m5 tied, m1 free */
        memcpy(full, w, sizeof w); tie12(full);
        uint64_t *sets[4] = { w, h1, h5, full }; uint64_t *cnt[4] = { &c_none, &c_half1, &c_half5, &c_full };
        for (int k = 0; k < 4; k++) {
            uint64_t w2[8]; memcpy(w2, sets[k], sizeof w2); flip(w2, 0, 1, 0);
            uint8_t A[64], B[64]; build(A, sets[k]); build(B, w2);
            rng_t r; rng_init(&r, rngseed);
            for (uint64_t i = 0; i < N; i++) { uint64_t seed = rng_next(&r); *cnt[k] += komihash(A, 64, seed) == komihash(B, 64, seed); }
        }
    }
    uint64_t tot = (uint64_t)bases * N;
    printf("  no tie: %" PRIu64 "/%" PRIu64 "; m1 tied only: %" PRIu64 "/%" PRIu64 "; m5 tied only: %" PRIu64 "/%" PRIu64 "; full tie: %" PRIu64 "/%" PRIu64 " = %.4f\n",
           c_none, tot, c_half1, tot, c_half5, tot, c_full, tot, (double)c_full / (double)tot);
}

/* ---- experiment 4: length scope for the 4-message set ---- */
static void run_len(uint64_t N, uint64_t rngseed) {
    printf("\n[len] the 4-message set of [multi] (best base) as the first block of longer messages (suffix bytes 0x99); 2^%.0f seeds\n",
           log2((double)N));
    uint64_t w[8]; best_base(w);
    static const size_t lens[5] = { 63, 64, 96, 127, 128 };
    for (int k = 0; k < 5; k++) multi_one(w, N, rngseed ^ (uint64_t)lens[k], lens[k], "best base");
}

int main(int argc, char **argv) {
    if (argc < 2) die("usage: komihash_ext <arb|free|s5bias|tie34|multi|all8|low3|len|control> [log2 seeds=20] [bases=200] [rng seed hex=c0ffee]");
    char *end = NULL;
    long lg = argc > 2 ? strtol(argv[2], &end, 10) : 20;
    if (argc > 2 && (end == argv[2] || *end)) die("log2 seeds must be decimal");
    if (lg < 1 || lg > 40) die("log2 seeds must be in 1..40");
    long bases = argc > 3 ? strtol(argv[3], &end, 10) : 200;
    if (argc > 3 && (end == argv[3] || *end)) die("bases must be decimal");
    if (bases < 1 || bases > 100000) die("bases out of range");
    uint64_t rngseed = argc > 4 ? strtoull(argv[4], &end, 16) : UINT64_C(0xc0ffee);
    if (argc > 4 && (end == argv[4] || *end)) die("rng seed must be hex");
    uint64_t N = UINT64_C(1) << lg;
    const char *e = argv[1];
    printf("komihash 5.34 (reference header, SHA-256 1adfc1bc...f44d89), 64-bit output; collision = full 64-bit equality; rng seed 0x%" PRIx64 "\n", rngseed);
    /* sanity: one upstream vector */
    uint8_t bulk[256]; for (int i = 0; i < 256; i++) bulk[i] = (uint8_t)i;
    if (komihash(bulk, 64, UINT64_C(0x0123456789abcdef)) != UINT64_C(0x765490569ccd77f2)) die("upstream test vector mismatch");
    if (!strcmp(e, "arb")) { run_arb(N, (int)bases, rngseed, 0); run_arb(N, (int)bases, rngseed, 1); }
    else if (!strcmp(e, "free")) run_free(N, (int)bases, rngseed);
    else if (!strcmp(e, "s5bias")) run_s5bias(N, rngseed);
    else if (!strcmp(e, "tie34")) { run_tie34(N, (int)bases, rngseed, 0, 0); run_tie34(N, (int)bases, rngseed, 0, 1); run_tie34(N, (int)bases, rngseed, 2, 0); run_tie34(N, (int)bases, rngseed, 2, 1); }
    else if (!strcmp(e, "multi")) run_multi(N, (int)bases, rngseed);
    else if (!strcmp(e, "all8")) run_all8(N, (int)bases, rngseed);
    else if (!strcmp(e, "low3")) run_low3(N, (int)bases, rngseed);
    else if (!strcmp(e, "len")) run_len(N, rngseed);
    else if (!strcmp(e, "control")) run_control(N, (int)bases, rngseed);
    else die("unknown experiment");
    return 0;
}
