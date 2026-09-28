/* t1ha2_topbit.c -- top-bit differential trail through the t1ha2 32-byte
 * loop (inputs longer than 32 bytes): pairs and 2^r-way multicollisions.
 *
 * Model: seed uniform over 2^64, messages fixed before the seed.
 *
 * Mechanism (block update, s = (a,b,c,d), words w0..w3):
 *   d02 = w0 + rotr(w2 + d, 56);  c13 = w1 + rotr(w3 + c, 19);
 *   d ^= b + rotr(w1, 38);        c ^= a + rotr(w0, 57);
 *   b ^= P6 * (c13 + w2);         a ^= P5 * (d02 + w3);
 * A difference of exactly 2^63 is the same as an XOR and as an additive
 * difference, and survives + and * (odd) unchanged.  Per block choose f:
 *   f = 1: flip w0 bit 56 (0->1, +2^56, rotr 57 moves it to bit 63 of the
 *          c update), flip w1 bit 37 (0->1, +2^37, rotr 38 -> bit 63 of the
 *          d update), and subtract 2^56 from w3 (bit 56 1->0); then
 *          d02 + w3 is unchanged and c13 is unchanged provided
 *          rotr(y - 2^56, 19) = rotr(y, 19) - 2^37 for y = w3 + c, i.e.
 *          y >= 2^56 (probability 1 - 2^-8 over the seed);
 *   always: flip bit 63 of w2 iff d carries a 2^63 difference and bit 63 of
 *          w3 iff c does, so w2 + d and w3 + c see no top-bit difference.
 * With A = B and C = D (always true from a zero start), the state
 * difference evolves as a' = a ^ c, c' = a ^ c ^ f (GF(2), deterministic).
 * f = (1,0,0) returns to zero after 3 blocks, so any subset of the first
 * n-2 blocks of an n-block message may be "switched on": 2^(n-2) messages
 * whose loop states agree whenever the n-2 carry conditions hold for the
 * common base message (the only conditions; they involve the base alone).
 *
 * Usage:
 *   t1ha2_topbit check                       validation only
 *   t1ha2_topbit pair  LOG2SEEDS [RNG]       96-byte pair, uniform seeds
 *   t1ha2_topbit fam   N R LOG2SEEDS [RNG]   N-block family on the first R
 *                                            blocks (k = 2^R, R <= N-2), all
 *                                            members hashed for every seed
 *   t1ha2_topbit pred  N LOG2SEEDS [RNG]     predictor only (base conditions)
 *   t1ha2_topbit print N R                   print the base and member 1
 */
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <math.h>
#include "t1ha2_core.h"

static uint64_t sm_state;
static uint64_t splitmix64(void) {
    uint64_t z = (sm_state += UINT64_C(0x9E3779B97F4A7C15));
    z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
    z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
    return z ^ (z >> 31);
}
static uint64_t xs[4];
static inline uint64_t rotl(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
static uint64_t xoshiro(void) {
    uint64_t r = rotl(xs[1] * 5, 7) * 9, t = xs[1] << 17;
    xs[2] ^= xs[0]; xs[3] ^= xs[1]; xs[1] ^= xs[2]; xs[0] ^= xs[3]; xs[2] ^= t; xs[3] = rotl(xs[3], 45);
    return r;
}
static void rng_seed(uint64_t s) { sm_state = s; for (int i = 0; i < 4; i++) xs[i] = splitmix64(); }

int cmp(const void *x, const void *y);
static void put64(uint8_t *p, uint64_t v) { memcpy(p, &v, 8); }
static uint64_t get64(const uint8_t *p) { uint64_t v; memcpy(&v, p, 8); return v; }

/* Base message: n blocks, deterministic filler with the three bits the f = 1
 * switch needs: w0 bit 56 = 0, w1 bit 37 = 0, w3 bit 56 = 1. */
static void make_base(uint8_t *m, int n) {
    uint64_t s = UINT64_C(0x0123456789abcdef);
    for (int k = 0; k < n; k++) {
        for (int j = 0; j < 4; j++) {
            s = s * UINT64_C(6364136223846793005) + UINT64_C(1442695040888963407);
            uint64_t w = s ^ (s >> 29);
            if (j == 0) w &= ~(UINT64_C(1) << 56);
            if (j == 1) w &= ~(UINT64_C(1) << 37);
            if (j == 3) w |= (UINT64_C(1) << 56);
            put64(m + 32 * k + 8 * j, w);
        }
    }
}
/* Member for switch mask sel (bit k = block k switched on, k < n-2). */
static void make_member(uint8_t *out, const uint8_t *base, int n, uint64_t sel) {
    memcpy(out, base, (size_t)32 * n);
    int a = 0, c = 0; /* state difference bits (a = b, c = d) */
    for (int k = 0; k < n; k++) {
        int f = (int)((sel >> k) & 1);
        uint8_t *blk = out + 32 * k;
        uint64_t w0 = get64(blk), w1 = get64(blk + 8), w2 = get64(blk + 16), w3 = get64(blk + 24);
        if (f) { w0 ^= UINT64_C(1) << 56; w1 ^= UINT64_C(1) << 37; w3 ^= UINT64_C(1) << 56; }
        if (c) { w2 ^= UINT64_C(1) << 63; w3 ^= UINT64_C(1) << 63; }
        put64(blk, w0); put64(blk + 8, w1); put64(blk + 16, w2); put64(blk + 24, w3);
        int na = a ^ c, nc = a ^ c ^ f;
        a = na; c = nc;
    }
    if (a || c) { fprintf(stderr, "member mask %" PRIx64 " does not return to zero\n", sel); exit(2); }
}
/* predictor: base conditions y_k = w3_k + c_k >= 2^56 for the switchable blocks */
static int predict(const uint8_t *base, int n, int r, uint64_t seed) {
    t_state s; uint64_t len = (uint64_t)32 * n;
    s.a = seed; s.b = len; s.c = t_rotr(len, 23) + ~seed; s.d = ~len + t_rotr(seed, 19);
    int ok = 1;
    for (int k = 0; k < n; k++) {
        uint64_t y = get64(base + 32 * k + 24) + s.c;
        if (k < r && y < (UINT64_C(1) << 56)) ok = 0;
        t_update(&s, base + 32 * k);
    }
    return ok;
}
static void hexline(const char *tag, const uint8_t *m, size_t len) {
    printf("%s", tag); for (size_t i = 0; i < len; i++) printf("%02x", m[i]); printf("\n");
}
static void garwood(uint64_t k, double n, double *lo, double *hi) {
    /* normal approximation with continuity for large counts; exact-ish for small via Wilson */
    double p = k / n, z = 1.959963984540054;
    double den = 1 + z * z / n, cen = (p + z * z / (2 * n)) / den;
    double half = z * sqrt(p * (1 - p) / n + z * z / (4 * n * n)) / den;
    *lo = cen - half; *hi = cen + half; if (*lo < 0) *lo = 0; if (*hi > 1) *hi = 1;
}

static int validate(void) {
    uint32_t v64 = t1ha2_verif(0), v128 = t1ha2_verif(1);
    printf("validation: t1ha2_64  SMHasher3 verification 0x%08" PRIX32 " (expected 0x8F16C948) %s\n", v64, v64 == 0x8F16C948u ? "OK" : "FAIL");
    printf("            t1ha2_128 SMHasher3 verification 0x%08" PRIX32 " (expected 0xB44C43A1) %s\n", v128, v128 == 0xB44C43A1u ? "OK" : "FAIL");
    return v64 == 0x8F16C948u && v128 == 0xB44C43A1u;
}

int main(int argc, char **argv) {
    if (!validate()) return 1;
    if (argc < 2 || !strcmp(argv[1], "check")) return 0;
    const char *mode = argv[1];
    if (!strcmp(mode, "print")) {
        int n = atoi(argv[2]), r = atoi(argv[3]);
        uint8_t *b = malloc(32 * n), *m = malloc(32 * n);
        make_base(b, n); make_member(m, b, n, 1);
        hexline("base     = ", b, 32 * n); hexline("member 1 = ", m, 32 * n);
        (void)r; return 0;
    }
    if (!strcmp(mode, "pair")) {
        int lg = argc > 2 ? atoi(argv[2]) : 24; uint64_t rs = argc > 3 ? strtoull(argv[3], 0, 0) : 0xA11CE;
        int n = 3; uint8_t M[96], Mp[96];
        make_base(M, n); make_member(Mp, M, n, 1);
        hexline("M  = ", M, 96); hexline("M' = ", Mp, 96);
        rng_seed(rs);
        uint64_t N = UINT64_C(1) << lg, hit = 0, hit128 = 0, predhit = 0, agree = 0, first = 0; int have = 0;
        for (uint64_t i = 0; i < N; i++) {
            uint64_t sd = xoshiro();
            uint64_t h1 = t1ha2_64(M, 96, sd), h2 = t1ha2_64(Mp, 96, sd);
            uint64_t x1, x2, l1 = t1ha2_128(M, 96, sd, &x1), l2 = t1ha2_128(Mp, 96, sd, &x2);
            int col = h1 == h2, col128 = (l1 == l2 && x1 == x2), pr = predict(M, n, 1, sd);
            hit += col; hit128 += col128; predhit += pr; agree += (pr == col);
            if (col && !have) { first = sd; have = 1; }
        }
        double lo, hi; garwood(hit, (double)N, &lo, &hi);
        printf("pair (96 bytes, L = 12): 64-bit collisions %" PRIu64 "/%" PRIu64 " = %.6f  95%% [%.6f, %.6f]\n", hit, N, (double)hit / N, lo, hi);
        printf("  misses %" PRIu64 " (model 2^-8 = %.1f expected); 128-bit full collisions %" PRIu64 "; predictor agrees on %" PRIu64 "/%" PRIu64 "\n",
               N - hit, N / 256.0, hit128, agree, N);
        printf("  score log2(12/eps) = %.4f bits  (95%%: %.4f .. %.4f)\n", log2(12.0 / ((double)hit / N)), log2(12.0 / hi), log2(12.0 / lo));
        if (have) {
            /* sequence the 128-bit calls before printf, so that x1 and x2
               are set when they are read */
            uint64_t x1, x2, g1 = t1ha2_128(M, 96, first, &x1), g2 = t1ha2_128(Mp, 96, first, &x2);
            printf("  example seed 0x%016" PRIx64 ": t1ha2_64 %016" PRIx64 " %016" PRIx64 "; t1ha2_128 %016" PRIx64 ":%016" PRIx64 " %016" PRIx64 ":%016" PRIx64 "\n",
                   first, t1ha2_64(M, 96, first), t1ha2_64(Mp, 96, first),
                   g1, x1, g2, x2);
        }
        (void)predhit;
        return 0;
    }
    if (!strcmp(mode, "fam") || !strcmp(mode, "pred")) {
        int famm = !strcmp(mode, "fam");
        int n = atoi(argv[2]); int r = famm ? atoi(argv[3]) : n - 2;
        int lg = atoi(argv[famm ? 4 : 3]); uint64_t rs = argc > (famm ? 5 : 4) ? strtoull(argv[famm ? 5 : 4], 0, 0) : 0xF100D;
        if (r > n - 2 || r > 24) { fprintf(stderr, "need R <= N-2 and R <= 24\n"); return 2; }
        size_t len = (size_t)32 * n; uint64_t K = UINT64_C(1) << r;
        uint8_t *base = malloc(len);
        make_base(base, n);
        rng_seed(rs);
        uint64_t N = UINT64_C(1) << lg, allcol = 0, predok = 0, agree = 0;
        if (!famm) {
            for (uint64_t i = 0; i < N; i++) predok += predict(base, n, r, xoshiro());
            double lo, hi; garwood(predok, (double)N, &lo, &hi);
            printf("predictor n=%d r=%d: %" PRIu64 "/%" PRIu64 " = %.5f [%.5f, %.5f]; model (1-2^-8)^%d = %.5f\n",
                   n, r, predok, N, (double)predok / N, lo, hi, r, pow(1 - 1.0 / 256, r));
            return 0;
        }
        uint8_t *msgs = malloc(len * K);
        for (uint64_t s = 0; s < K; s++) make_member(msgs + len * s, base, n, s);
        uint64_t *h = malloc(8 * K);
        /* per-seed largest class */
        uint64_t *cls = malloc(8 * N); uint64_t first = 0; int have = 0; uint64_t firsth = 0;
        for (uint64_t i = 0; i < N; i++) {
            uint64_t sd = xoshiro();
            for (uint64_t s = 0; s < K; s++) h[s] = t1ha2_64(msgs + len * s, len, sd);
            /* largest class: sort copy */
            qsort(h, K, 8, cmp);
            uint64_t best = 1, run = 1;
            for (uint64_t s = 1; s < K; s++) { if (h[s] == h[s - 1]) { run++; if (run > best) best = run; } else run = 1; }
            cls[i] = best;
            int all = best == K, pr = predict(base, n, r, sd);
            allcol += all; predok += pr; agree += (all == pr);
            if (all && !have) { have = 1; first = sd; firsth = h[0]; }
        }
        qsort(cls, N, 8, cmp);
        double lo, hi; garwood(allcol, (double)N, &lo, &hi);
        printf("family: n=%d blocks (%zu bytes, L=%zu), r=%d switchable blocks, k=%" PRIu64 " messages\n", n, len, len / 8, r, K);
        printf("  all k equal (64-bit): %" PRIu64 "/%" PRIu64 " seeds = %.5f  95%% [%.5f, %.5f]; model (1-2^-8)^r = %.5f\n",
               allcol, N, (double)allcol / N, lo, hi, pow(1 - 1.0 / 256, r));
        printf("  predictor (base conditions) agrees with the hash on %" PRIu64 "/%" PRIu64 " seeds\n", agree, N);
        printf("  largest class per seed: min %" PRIu64 ", 1st pct %" PRIu64 ", median %" PRIu64 ", max %" PRIu64 "\n",
               cls[0], cls[N / 100], cls[N / 2], cls[N - 1]);
        if (have) printf("  first all-colliding seed 0x%016" PRIx64 ": all %" PRIu64 " members hash to %016" PRIx64 "\n", first, K, firsth);
        return 0;
    }
    fprintf(stderr, "unknown mode\n");
    return 2;
}
int cmp(const void *x, const void *y) {
    uint64_t a = *(const uint64_t *)x, b = *(const uint64_t *)y;
    return a < b ? -1 : a > b;
}
