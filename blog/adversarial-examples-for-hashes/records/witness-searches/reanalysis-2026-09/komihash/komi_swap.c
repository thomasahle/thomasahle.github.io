/* komi_swap.c -- komihash 5.34 (64-bit, uniform 64-bit UseSeed):
 *  (1) re-measure the published 64-byte lane-tie pair;
 *  (2) key-free pair: tie BOTH lane pairs of the 64-byte loop block, so the
 *      lane low halves cancel pairwise and the folded state has S1 == S5 = X
 *      for every seed; the epilogue then multiplies (m0 ^ X) * (m1 ^ X),
 *      which is symmetric, so exchanging m0 and m1 gives the same hash for
 *      every seed.  Length 73 (9-byte epilogue: m0 = bytes 0..7, m1 = 0x100 |
 *      byte 8) and length 80 (16-byte epilogue: one HASH16 on two words).
 *
 * Usage: komi_swap pub LOG2SEEDS [RNG] | swap LOG2SEEDS [RNG]
 */
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <math.h>
#include "komihash_core.h"

static uint64_t sm;
static uint64_t splitmix64(void) {
    uint64_t z = (sm += 0x9E3779B97F4A7C15ull);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull; z = (z ^ (z >> 27)) * 0x94D049BB133111EBull;
    return z ^ (z >> 31);
}
static uint64_t xs[4];
static inline uint64_t rl(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
static uint64_t xoshiro(void) {
    uint64_t r = rl(xs[1] * 5, 7) * 9, t = xs[1] << 17;
    xs[2] ^= xs[0]; xs[3] ^= xs[1]; xs[1] ^= xs[2]; xs[0] ^= xs[3]; xs[2] ^= t; xs[3] = rl(xs[3], 45);
    return r;
}
static void rng_seed(uint64_t s) { sm = s; for (int i = 0; i < 4; i++) xs[i] = splitmix64(); }
static int unhex(const char *h, uint8_t *out) {
    int n = 0; for (; h[0] && h[1]; h += 2) { unsigned v; sscanf(h, "%2x", &v); out[n++] = (uint8_t)v; } return n;
}
static void put(uint8_t *m, int off, uint64_t v) { for (int i = 0; i < 8; i++) m[off + i] = (uint8_t)(v >> (8 * i)); }
static void hexline(const char *tag, const uint8_t *m, int len) {
    printf("%s", tag); for (int i = 0; i < len; i++) printf("%02x", m[i]); printf("\n");
}
static void wilson(uint64_t k, double n, double *lo, double *hi) {
    double p = k / n, z = 1.959963984540054, den = 1 + z * z / n, c = (p + z * z / (2 * n)) / den;
    double h = z * sqrt(p * (1 - p) / n + z * z / (4 * n * n)) / den; *lo = c - h; *hi = c + h;
}
/* 64-byte block with both lane pairs tied (lanes 0/1 and 2/3) */
static void tied_block(uint8_t *m, uint64_t w0, uint64_t w2, uint64_t w4, uint64_t w6) {
    put(m, 0, w0);  put(m, 8, w0 ^ KI[2]);
    put(m, 16, w2); put(m, 24, w2 ^ KI[3] ^ KI[4]);
    put(m, 32, w4); put(m, 40, w4 ^ KI[6]);
    put(m, 48, w6); put(m, 56, w6 ^ KI[7] ^ KI[8]);
}

int main(int argc, char **argv) {
    uint32_t v = komihash_verif();
    printf("validation: komihash SMHasher3 verification 0x%08" PRIX32 " (expected 0x8157FF6D) %s\n", v, v == 0x8157FF6Du ? "OK" : "FAIL");
    if (v != 0x8157FF6Du) return 1;
    if (argc < 3) return 0;
    int lg = atoi(argv[2]); uint64_t rs = argc > 3 ? strtoull(argv[3], 0, 0) : 0xC0DE5EEDull;
    uint64_t N = 1ull << lg;
    if (!strcmp(argv[1], "pub")) {
        uint8_t A[64], B[64];
        unhex("0000000000000000447370032e8a191311111111111111112222222222222222"
              "f0f9dda4c1c0a15e9cf534900ea6f5e033333333333333334444444444444444", A);
        unhex("0100000000000000457370032e8a191311111111111111112222222222222222"
              "f0f9dda4c1c0a15e9cf534900ea6f5e033333333333333334444444444444444", B);
        rng_seed(rs); uint64_t c = 0;
        for (uint64_t n = 0; n < N; n++) { uint64_t s = xoshiro(); c += komihash64(A, 64, s) == komihash64(B, 64, s); }
        double lo, hi; wilson(c, (double)N, &lo, &hi);
        printf("published 64-byte pair: %" PRIu64 "/%" PRIu64 " = %.5f  95%% [%.5f, %.5f]; bits log2(8/eps) = %.4f\n",
               c, N, (double)c / N, lo, hi, log2(8.0 / ((double)c / N)));
        printf("  example seed 0x27d1f77dc2a01269: %016" PRIx64 " %016" PRIx64 "\n",
               komihash64(A, 64, 0x27d1f77dc2a01269ull), komihash64(B, 64, 0x27d1f77dc2a01269ull));
        return 0;
    }
    if (!strcmp(argv[1], "swap")) {
        /* length 73: tail 9 bytes [y, 01, 00 x 6, z] vs [z, 01, 00 x 6, y] */
        uint8_t A[80], B[80];
        tied_block(A, 0x0123456789abcdefull, 0xfedcba9876543210ull, 0x5ea1c0c1a4ddf9f0ull, 0x1111111111111111ull);
        memcpy(B, A, 64);
        memset(A + 64, 0, 16); memset(B + 64, 0, 16);
        A[64] = 0x5a; A[65] = 0x01; A[72] = 0xc3;
        B[64] = 0xc3; B[65] = 0x01; B[72] = 0x5a;
        rng_seed(rs); uint64_t c73 = 0, c80 = 0;
        uint8_t C[80], D[80]; memcpy(C, A, 64); memcpy(D, A, 64);
        put(C, 64, 0x0f1e2d3c4b5a6978ull); put(C, 72, 0x8877665544332211ull);
        put(D, 64, 0x8877665544332211ull); put(D, 72, 0x0f1e2d3c4b5a6978ull);
        for (uint64_t n = 0; n < N; n++) {
            uint64_t s = xoshiro();
            c73 += komihash64(A, 73, s) == komihash64(B, 73, s);
            c80 += komihash64(C, 80, s) == komihash64(D, 80, s);
        }
        printf("key-free pair, length 73 (L = 10):\n"); hexline("  M  = ", A, 73); hexline("  M' = ", B, 73);
        printf("  collisions %" PRIu64 "/%" PRIu64 "; bits log2(10/1) = %.4f\n", c73, N, log2(10.0));
        printf("key-free pair, length 80 (L = 10):\n"); hexline("  M  = ", C, 80); hexline("  M' = ", D, 80);
        printf("  collisions %" PRIu64 "/%" PRIu64 "\n", c80, N);
        uint64_t ex[2] = { 0, 0x27d1f77dc2a01269ull };
        for (int e = 0; e < 2; e++)
            printf("  seed 0x%016" PRIx64 ": len73 %016" PRIx64 " %016" PRIx64 "; len80 %016" PRIx64 " %016" PRIx64 "\n", ex[e],
                   komihash64(A, 73, ex[e]), komihash64(B, 73, ex[e]), komihash64(C, 80, ex[e]), komihash64(D, 80, ex[e]));
        return 0;
    }
    return 2;
}
