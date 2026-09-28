/*
 * xxh32_witness.c -- fixed-message collisions and multicollisions in XXH32
 * (classic 32-bit xxHash) under a uniformly random 32-bit seed.
 *
 * XXH32 code: xxHash by Yann Collet (BSD-2-Clause), transcribed from
 * SMHasher3 hashes/xxhash.cpp (XXH32_round, XXH32_avalanche,
 * XXH32_finalize, XXH32_impl) with the C++ templates removed; little-endian
 * loads.  SMHasher3 verification value 0x6FD78385 (canonical big-endian
 * output) is recomputed at startup.
 *
 * Mechanism (short path, len < 16): each 4-byte word does
 *     h += w*P3;  h = rotl(h,17)*P4.
 * Adding j*2^15 to V = h + w*P3 (w += j*2^15*P3^-1) adds exactly j to
 * rotl(V,17) unless the top 17 bits of V overflow (probability j/2^17), so
 * the state grows by j*P4 and the next word (w' -= j*P4*P3^-1) cancels it.
 *
 *   A  pair, 8 bytes (L = 1), j = 1: collides unless top17(V) = 2^17-1:
 *      exactly 2^32 - 2^15 of the 2^32 seeds (checked exhaustively).
 *   B  J-way family at 8 bytes, J = 2^15: all collide unless
 *      top17(V) >= 2^17-(J-1): p = 1 - (J-1)/2^17 (about 3/4).
 *   C  J^2-way family at 12 bytes (J = 256 -> 65536 messages): word 1 opens
 *      j1, word 2 cancels j1 and opens j2, word 3 cancels j2.
 *   D  long path (32 bytes, 2 stripes), rotl 13: j*2^19 per lane, J = 16,
 *      16^4 = 65536 messages, fails with probability <= 4*15/2^13.
 *
 * Build: cc -std=c11 -O2 -o xxh32_witness xxh32_witness.c -lm
 * Run:   ./xxh32_witness [log2_keys_family=12] [rng_seed=0x32] [--exhaustive]
 */
#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define XXH_PRIME32_1  UINT32_C(0x9E3779B1)
#define XXH_PRIME32_2  UINT32_C(0x85EBCA77)
#define XXH_PRIME32_3  UINT32_C(0xC2B2AE3D)
#define XXH_PRIME32_4  UINT32_C(0x27D4EB2F)
#define XXH_PRIME32_5  UINT32_C(0x165667B1)
static inline uint32_t ROTL32(uint32_t x, int r) { return (x << r) | (x >> (32 - r)); }
static inline uint32_t GET_U32(const uint8_t *p, size_t o) { return (uint32_t)p[o] | (uint32_t)p[o+1] << 8 | (uint32_t)p[o+2] << 16 | (uint32_t)p[o+3] << 24; }

static uint32_t XXH32_avalanche( uint32_t hash ) {
    hash ^= hash >> 15;
    hash *= XXH_PRIME32_2;
    hash ^= hash >> 13;
    hash *= XXH_PRIME32_3;
    hash ^= hash >> 16;
    return hash;
}
static uint32_t XXH32_finalize( uint32_t hash, const uint8_t * ptr, size_t len ) {
    while (len >= 4) {
        hash += GET_U32(ptr, 0) * XXH_PRIME32_3;
        ptr  += 4;
        hash  = ROTL32(hash, 17)       * XXH_PRIME32_4;
        len  -= 4;
    }
    while (len > 0) {
        hash += (*ptr++) * XXH_PRIME32_5;
        hash  = ROTL32(hash, 11) * XXH_PRIME32_1;
        --len;
    }
    return XXH32_avalanche(hash);
}
static uint32_t XXH32_round( uint32_t acc, uint32_t input ) {
    acc += input * XXH_PRIME32_2;
    acc  = ROTL32(acc, 13);
    acc *= XXH_PRIME32_1;
    return acc;
}
static uint32_t XXH32_impl( const uint8_t * input, size_t len, uint32_t seed ) {
    uint32_t h32;
    if (len >= 16) {
        const uint8_t * const bEnd  = input + len;
        const uint8_t * const limit = bEnd - 15;
        uint32_t v1 = seed + XXH_PRIME32_1 + XXH_PRIME32_2;
        uint32_t v2 = seed + XXH_PRIME32_2;
        uint32_t v3 = seed + 0;
        uint32_t v4 = seed - XXH_PRIME32_1;
        do {
            v1     = XXH32_round(v1, GET_U32(input,  0));
            v2     = XXH32_round(v2, GET_U32(input,  4));
            v3     = XXH32_round(v3, GET_U32(input,  8));
            v4     = XXH32_round(v4, GET_U32(input, 12));
            input += 16;
        } while (input < limit);
        h32 = ROTL32(v1, 1) + ROTL32(v2, 7) + ROTL32(v3, 12) + ROTL32(v4, 18);
    } else {
        h32 = seed + XXH_PRIME32_5;
    }
    h32 += (uint32_t)len;
    return XXH32_finalize(h32, input, len & 15);
}
static void XXH32_bytes(const void *in, size_t len, uint32_t seed, uint8_t out[4]) {
    uint32_t h = XXH32_impl((const uint8_t *)in, len, seed);
    for (int i = 0; i < 4; i++) out[i] = (uint8_t)(h >> (24 - 8 * i));
}
static uint32_t smhasher3_verification(void) {
    uint8_t key[256] = {0}, hashes[4 * 256], total[4];
    for (int i = 0; i < 256; i++) { XXH32_bytes(key, (size_t)i, (uint32_t)(256 - i), hashes + 4 * i); key[i] = (uint8_t)i; }
    XXH32_bytes(hashes, sizeof hashes, 0, total);
    return (uint32_t)total[0] | (uint32_t)total[1] << 8 | (uint32_t)total[2] << 16 | (uint32_t)total[3] << 24;
}

static inline uint64_t ROTL64(uint64_t x, int r) { return (x << r) | (x >> (64 - r)); }
static uint64_t splitmix64(uint64_t *s) { uint64_t z = (*s += UINT64_C(0x9e3779b97f4a7c15)); z = (z ^ (z >> 30)) * UINT64_C(0xbf58476d1ce4e5b9); z = (z ^ (z >> 27)) * UINT64_C(0x94d049bb133111eb); return z ^ (z >> 31); }
static uint64_t rs[4];
static void rng_init(uint64_t seed) { for (int i = 0; i < 4; i++) rs[i] = splitmix64(&seed); }
static uint64_t rng(void) { const uint64_t r = ROTL64(rs[1] * 5, 7) * 9, t = rs[1] << 17; rs[2] ^= rs[0]; rs[3] ^= rs[1]; rs[1] ^= rs[2]; rs[0] ^= rs[3]; rs[2] ^= t; rs[3] = ROTL64(rs[3], 45); return r; }
static uint32_t inv32(uint32_t a) { uint32_t x = a; for (int i = 0; i < 5; i++) x *= 2 - a * x; return x; }
static void put32(uint8_t *p, uint32_t v) { for (int i = 0; i < 4; i++) p[i] = (uint8_t)(v >> (8 * i)); }
static void print_msg(const char *name, const uint8_t *m, size_t n) { printf("  %s (%zu bytes) = ", name, n); for (size_t i = 0; i < n; i++) printf("%02x", m[i]); printf("\n"); }
static void wilson(uint64_t k, uint64_t n, double *lo, double *hi) {
    double z = 1.959963984540054, p = (double)k / n, d = 1 + z * z / n, c = p + z * z / (2.0 * n);
    double s = z * sqrt(p * (1 - p) / n + z * z / (4.0 * n * n)); *lo = (c - s) / d; *hi = (c + s) / d;
}

/* distinctness check for a generated family: sort copies, compare neighbours */
static size_t dist_len;
static int dist_cmp(const void *a, const void *b) { return memcmp(a, b, dist_len); }
static uint64_t count_distinct(const uint8_t *fam, size_t len, uint64_t K) {
    uint8_t *c = malloc(K * len); memcpy(c, fam, K * len); dist_len = len;
    qsort(c, K, len, dist_cmp);
    uint64_t d = K ? 1 : 0; for (uint64_t i = 1; i < K; i++) d += memcmp(c + (i - 1) * len, c + i * len, len) != 0;
    free(c); return d;
}

/* p(S) of a family: for nkeys random seeds, count seeds where all K messages share one value,
   and record the size of the class containing message 0 (min / median via histogram). */
static void family_stats(const char *tag, const uint8_t *fam, size_t len, uint64_t K, uint64_t nkeys) {
    { uint64_t d = count_distinct(fam, len, K); printf("  %s: distinct messages %" PRIu64 " of %" PRIu64 "\n", tag, d, K); if (d != K) { printf("NOT DISTINCT\n"); exit(3); } }
    uint64_t all = 0, minc = K; uint64_t *cls = malloc(nkeys * sizeof *cls);
    for (uint64_t t = 0; t < nkeys; t++) {
        uint32_t s = (uint32_t)rng(), h0 = XXH32_impl(fam, len, s); uint64_t same = 0;
        for (uint64_t m = 0; m < K; m++) same += XXH32_impl(fam + len * m, len, s) == h0;
        all += same == K; if (same < minc) minc = same; cls[t] = same;
    }
    double lo, hi; wilson(all, nkeys, &lo, &hi);
    printf("  %s: all %" PRIu64 " equal for %" PRIu64 " / %" PRIu64 " random seeds, p(S) = %.5f, 95%% CI [%.5f, %.5f]; smallest class of message 0: %" PRIu64 "\n",
           tag, K, all, nkeys, (double)all / nkeys, lo, hi, minc);
    free(cls);
}

int main(int argc, char **argv) {
    int lg_fam = argc > 1 ? atoi(argv[1]) : 12;
    uint64_t rseed = argc > 2 ? strtoull(argv[2], 0, 0) : 0x32;
    int exhaustive = argc > 3 && !strcmp(argv[3], "--exhaustive");
    uint32_t v = smhasher3_verification();
    printf("SMHasher3 verification XXH32: 0x%08" PRIX32 " (expected 0x6FD78385)\n", v);
    if (v != UINT32_C(0x6FD78385)) { printf("MISMATCH\n"); return 1; }
    rng_init(rseed);
    const uint32_t IP3 = inv32(XXH_PRIME32_3), IP2 = inv32(XXH_PRIME32_2);
    const uint32_t dA = (UINT32_C(1) << 15) * IP3, cA = XXH_PRIME32_4 * IP3;   /* open j: w += j*dA; close: w -= j*cA */
    int ok = 1;

    /* ---- A: pair at 8 bytes ---- */
    uint8_t a[8], b[8]; memcpy(a, "xxh32-pr", 8);
    put32(b, GET_U32(a, 0) + dA); put32(b + 4, GET_U32(a, 4) - cA);
    printf("\n[A] pair, 8 bytes (L = 1)\n"); print_msg("M1", a, 8); print_msg("M2", b, 8);
    uint64_t NA = UINT64_C(1) << 24, coll = 0;
    for (uint64_t i = 0; i < NA; i++) { uint32_t s = (uint32_t)rng(); coll += XXH32_impl(a, 8, s) == XXH32_impl(b, 8, s); }
    double lo, hi; wilson(NA - coll, NA, &lo, &hi);
    printf("  collisions %" PRIu64 " / %" PRIu64 " random seeds (non-colliding %" PRIu64 ", 95%% CI of failure rate [%.3g, %.3g]; predicted 2^-17 = %.3g)\n",
           coll, NA, NA - coll, lo, hi, 1.0 / 131072);
    /* the predicted failure set: top17(seed + P5 + 8 + w1*P3) == 0x1FFFF */
    uint32_t Vbad = UINT32_C(0xFFFF8000) | ((uint32_t)rng() & 0x7FFF);
    uint32_t sbad = Vbad - XXH_PRIME32_5 - 8 - GET_U32(a, 0) * XXH_PRIME32_3;
    printf("  seed in the predicted failure set 0x%08" PRIx32 ": %08" PRIx32 " vs %08" PRIx32 "\n", sbad, XXH32_impl(a, 8, sbad), XXH32_impl(b, 8, sbad));
    uint32_t sx = (uint32_t)rng();
    printf("  explicit colliding seed 0x%08" PRIx32 ": %08" PRIx32 " %08" PRIx32 "\n", sx, XXH32_impl(a, 8, sx), XXH32_impl(b, 8, sx));
    if (exhaustive) {
        uint64_t c = 0; uint32_t s = 0;
        do { c += XXH32_impl(a, 8, s) == XXH32_impl(b, 8, s); } while (++s != 0);
        printf("  EXHAUSTIVE over all 2^32 seeds: %" PRIu64 " collide, %" PRIu64 " do not (predicted 2^15 = 32768)\n", c, (UINT64_C(1) << 32) - c);
        ok &= c == (UINT64_C(1) << 32) - 32768;
    }

    /* ---- B: J = 2^15 messages at 8 bytes ---- */
    const uint64_t JB = 32768;
    uint8_t *famB = malloc(JB * 8);
    for (uint64_t j = 0; j < JB; j++) { put32(famB + 8 * j, GET_U32(a, 0) + (uint32_t)j * dA); put32(famB + 8 * j + 4, GET_U32(a, 4) - (uint32_t)j * cA); }
    printf("\n[B] 2^15-way family at 8 bytes: word0 = w0 + j*(2^15*P3^-1), word1 = w1 - j*(P4*P3^-1), j < 2^15; predicted p = 1 - (J-1)/2^17 = %.5f\n", 1 - (JB - 1) / 131072.0);
    print_msg("B[1]", famB + 8, 8); print_msg("B[32767]", famB + 8 * (JB - 1), 8);
    family_stats("B", famB, 8, JB, UINT64_C(1) << lg_fam);
    free(famB);

    /* ---- C: 256^2 = 65536 messages at 12 bytes ---- */
    const uint64_t J = 256, KC = J * J;
    uint8_t c0[12]; memcpy(c0, "xxh32 12byte", 12);
    uint8_t *famC = malloc(KC * 12);
    for (uint64_t m = 0; m < KC; m++) {
        uint32_t j1 = (uint32_t)(m & 255), j2 = (uint32_t)(m >> 8);
        uint8_t *p = famC + 12 * m;
        put32(p, GET_U32(c0, 0) + j1 * dA);
        put32(p + 4, GET_U32(c0, 4) - j1 * cA + j2 * dA);
        put32(p + 8, GET_U32(c0, 8) - j2 * cA);
    }
    printf("\n[C] 2^16-way family at 12 bytes: w0 + j1*d, w1 - j1*c + j2*d, w2 - j2*c (d = 2^15*P3^-1, c = P4*P3^-1), j1,j2 < 256; predicted p >= 1 - 2*255/2^17 = %.5f\n", 1 - 510 / 131072.0);
    print_msg("C[0]", famC, 12); print_msg("C[65535]", famC + 12 * (KC - 1), 12);
    family_stats("C", famC, 12, KC, UINT64_C(1) << lg_fam);
    uint32_t s2 = (uint32_t)rng();
    printf("  explicit seed 0x%08" PRIx32 ": XXH32(C[0]) = %08" PRIx32 ", XXH32(C[65535]) = %08" PRIx32 "\n", s2, XXH32_impl(famC, 12, s2), XXH32_impl(famC + 12 * (KC - 1), 12, s2));
    free(famC);

    /* ---- D: long path, 32 bytes, 16^4 messages ---- */
    const uint64_t KD = 65536;
    uint8_t d0[32]; memcpy(d0, "XXH32 long-path lanes, 16^4 msgs", 32);
    const uint32_t dL = (UINT32_C(1) << 19) * IP2, cL = XXH_PRIME32_1 * IP2;
    uint8_t *famD = malloc(KD * 32);
    for (uint64_t m = 0; m < KD; m++) {
        uint8_t *p = famD + 32 * m;
        for (int l = 0; l < 4; l++) {
            uint32_t j = (uint32_t)((m >> (4 * l)) & 15);
            put32(p + 4 * l, GET_U32(d0, 4 * l) + j * dL);
            put32(p + 16 + 4 * l, GET_U32(d0, 16 + 4 * l) - j * cL);
        }
    }
    printf("\n[D] long-path 16^4 family at 32 bytes: lane word += j*(2^19*P2^-1), next stripe -= j*(P1*P2^-1); predicted p >= 1 - 4*15/2^13 = %.5f\n", 1 - 60 / 8192.0);
    print_msg("D[65535]", famD + 32 * (KD - 1), 32);
    family_stats("D", famD, 32, KD, UINT64_C(1) << lg_fam);
    free(famD);
    return ok ? 0 : 2;
}
