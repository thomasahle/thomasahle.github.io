/*
 * xxh64_witness.c -- fixed-message collisions and multicollisions in XXH64
 * (classic 64-bit xxHash) under a uniformly random 64-bit seed.
 *
 * XXH64 code: xxHash by Yann Collet (BSD-2-Clause), transcribed from
 * SMHasher3 hashes/xxhash.cpp (XXH64_round, XXH64_mergeRound,
 * XXH64_avalanche, XXH64_finalize, XXH64_impl) with the C++ templates
 * removed; little-endian loads.  SMHasher3 verification value 0x8F8224C4
 * (output in canonical big-endian byte order) is recomputed at startup.
 *
 * Families
 *   A  key-free pair, 16 bytes (short path, len < 32): word difference chosen
 *      so that round(0,w) changes by 2^36; after hash ^= k; rotl 27 the
 *      difference sits in bit 63, survives *P1 + P4, and the next word
 *      (round output changed by 2^63) cancels it.  Every seed.
 *   B  key-free 2^(n-1)-way cube with n tail words (n = 3 at 24 bytes).
 *   C  long path (64 bytes, two stripes): in each lane the stripe-1 word
 *      adds j*2^33 to V = acc + x*P2; rotl 31 moves bits 33..63 to 0..30,
 *      so the rotated value grows by exactly j unless the top 31 bits of V
 *      overflow (probability j/2^31); *P1 turns it into +j*P1 and the
 *      stripe-2 word subtracts j*P1.  J^4 messages over the 4 lanes.
 *
 * Build: cc -std=c11 -O2 -o xxh64_witness xxh64_witness.c
 * Run:   ./xxh64_witness [log2_keys_pairs=24] [log2_keys_family=12] [rng_seed=0x55]
 */
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------------- XXH64 (SMHasher3 transcription) ---------------- */
#define XXH_PRIME64_1  UINT64_C(0x9E3779B185EBCA87)
#define XXH_PRIME64_2  UINT64_C(0xC2B2AE3D27D4EB4F)
#define XXH_PRIME64_3  UINT64_C(0x165667B19E3779F9)
#define XXH_PRIME64_4  UINT64_C(0x85EBCA77C2B2AE63)
#define XXH_PRIME64_5  UINT64_C(0x27D4EB2F165667C5)
static inline uint64_t ROTL64(uint64_t x, int r) { return (x << r) | (x >> (64 - r)); }
static inline uint64_t GET_U64(const uint8_t *p, size_t o) { uint64_t v = 0; for (int i = 7; i >= 0; i--) v = (v << 8) | p[o + i]; return v; }
static inline uint32_t GET_U32(const uint8_t *p, size_t o) { return (uint32_t)p[o] | (uint32_t)p[o+1] << 8 | (uint32_t)p[o+2] << 16 | (uint32_t)p[o+3] << 24; }

static uint64_t XXH64_round( uint64_t acc, uint64_t input ) {
    acc += input * XXH_PRIME64_2;
    acc  = ROTL64(acc, 31);
    acc *= XXH_PRIME64_1;
    return acc;
}
static uint64_t XXH64_mergeRound( uint64_t acc, uint64_t val ) {
    val  = XXH64_round(0, val);
    acc ^= val;
    acc  = acc * XXH_PRIME64_1 + XXH_PRIME64_4;
    return acc;
}
static uint64_t XXH64_avalanche( uint64_t hash ) {
    hash ^= hash >> 33;
    hash *= XXH_PRIME64_2;
    hash ^= hash >> 29;
    hash *= XXH_PRIME64_3;
    hash ^= hash >> 32;
    return hash;
}
static uint64_t XXH64_finalize( uint64_t hash, const uint8_t * ptr, size_t len ) {
    while (len >= 8) {
        uint64_t const k1 = XXH64_round(0, GET_U64(ptr, 0));
        ptr  += 8;
        hash ^= k1;
        hash  = ROTL64(hash, 27) * XXH_PRIME64_1 + XXH_PRIME64_4;
        len  -= 8;
    }
    if (len >= 4) {
        hash ^= (uint64_t)(GET_U32(ptr, 0)) * XXH_PRIME64_1;
        ptr  += 4;
        hash  = ROTL64(hash, 23) * XXH_PRIME64_2 + XXH_PRIME64_3;
        len  -= 4;
    }
    while (len > 0) {
        hash ^= (*ptr++) * XXH_PRIME64_5;
        hash  = ROTL64(hash, 11) * XXH_PRIME64_1;
        --len;
    }
    return XXH64_avalanche(hash);
}
static uint64_t XXH64_impl( const uint8_t * input, size_t len, uint64_t seed ) {
    uint64_t h64;
    if (len >= 32) {
        const uint8_t * const bEnd  = input + len;
        const uint8_t * const limit = bEnd - 31;
        uint64_t v1 = seed + XXH_PRIME64_1 + XXH_PRIME64_2;
        uint64_t v2 = seed + XXH_PRIME64_2;
        uint64_t v3 = seed + 0;
        uint64_t v4 = seed - XXH_PRIME64_1;
        do {
            v1     = XXH64_round(v1, GET_U64(input,  0));
            v2     = XXH64_round(v2, GET_U64(input,  8));
            v3     = XXH64_round(v3, GET_U64(input, 16));
            v4     = XXH64_round(v4, GET_U64(input, 24));
            input += 32;
        } while (input < limit);
        h64 = ROTL64(v1, 1) + ROTL64(v2, 7) + ROTL64(v3, 12) + ROTL64(v4, 18);
        h64 = XXH64_mergeRound(h64, v1);
        h64 = XXH64_mergeRound(h64, v2);
        h64 = XXH64_mergeRound(h64, v3);
        h64 = XXH64_mergeRound(h64, v4);
    } else {
        h64 = seed + XXH_PRIME64_5;
    }
    h64 += (uint64_t)len;
    return XXH64_finalize(h64, input, len & 31);
}

/* SMHasher3 wrapper: canonical big-endian output bytes. */
static void XXH64_bytes(const void *in, size_t len, uint64_t seed, uint8_t out[8]) {
    uint64_t h = XXH64_impl((const uint8_t *)in, len, seed);
    for (int i = 0; i < 8; i++) out[i] = (uint8_t)(h >> (56 - 8 * i));
}
/* lib/Hashinfo.cpp::_ComputedVerifyImpl */
static uint32_t smhasher3_verification(void) {
    uint8_t key[256] = {0}, hashes[8 * 256], total[8];
    for (int i = 0; i < 256; i++) { XXH64_bytes(key, (size_t)i, (uint64_t)(256 - i), hashes + 8 * i); key[i] = (uint8_t)i; }
    XXH64_bytes(hashes, sizeof hashes, 0, total);
    return (uint32_t)total[0] | (uint32_t)total[1] << 8 | (uint32_t)total[2] << 16 | (uint32_t)total[3] << 24;
}

/* ---------------- RNG: splitmix64 -> xoshiro256** ---------------- */
static uint64_t splitmix64(uint64_t *s) { uint64_t z = (*s += UINT64_C(0x9e3779b97f4a7c15)); z = (z ^ (z >> 30)) * UINT64_C(0xbf58476d1ce4e5b9); z = (z ^ (z >> 27)) * UINT64_C(0x94d049bb133111eb); return z ^ (z >> 31); }
static uint64_t rs[4];
static void rng_init(uint64_t seed) { for (int i = 0; i < 4; i++) rs[i] = splitmix64(&seed); }
static uint64_t rng(void) { const uint64_t r = ROTL64(rs[1] * 5, 7) * 9, t = rs[1] << 17; rs[2] ^= rs[0]; rs[3] ^= rs[1]; rs[1] ^= rs[2]; rs[0] ^= rs[3]; rs[2] ^= t; rs[3] = ROTL64(rs[3], 45); return r; }

/* ---------------- helpers ---------------- */
static uint64_t inv64(uint64_t a) { uint64_t x = a; for (int i = 0; i < 6; i++) x *= 2 - a * x; return x; }
static uint64_t IP1, IP2;
/* inverse of k = XXH64_round(0, w) = rotl(w*P2, 31) * P1 */
static uint64_t round0_inv(uint64_t k) { uint64_t t = k * IP1; t = (t >> 31) | (t << 33); return t * IP2; }
static void put64(uint8_t *p, uint64_t v) { for (int i = 0; i < 8; i++) p[i] = (uint8_t)(v >> (8 * i)); }
static void print_msg(const char *name, const uint8_t *m, size_t n) { printf("  %s (%zu bytes) = ", name, n); for (size_t i = 0; i < n; i++) printf("%02x", m[i]); printf("\n"); }
static void wilson(uint64_t k, uint64_t n, double *lo, double *hi) {
    /* exact-ish 95% interval: Wilson score */
    double z = 1.959963984540054, p = (double)k / n, d = 1 + z * z / n, c = p + z * z / (2.0 * n);
    double s = z * __builtin_sqrt(p * (1 - p) / n + z * z / (4.0 * n * n));
    *lo = (c - s) / d; *hi = (c + s) / d;
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

int main(int argc, char **argv) {
    int lg_pairs = argc > 1 ? atoi(argv[1]) : 24;
    int lg_fam   = argc > 2 ? atoi(argv[2]) : 12;
    uint64_t rseed = argc > 3 ? strtoull(argv[3], 0, 0) : 0x55;
    uint32_t v = smhasher3_verification();
    printf("SMHasher3 verification XXH64: 0x%08" PRIX32 " (expected 0x8F8224C4)\n", v);
    if (v != UINT32_C(0x8F8224C4)) { printf("MISMATCH\n"); return 1; }
    IP1 = inv64(XXH_PRIME64_1); IP2 = inv64(XXH_PRIME64_2);
    rng_init(rseed);

    /* ---- A: key-free pair at 16 bytes ---- */
    uint8_t a[16], b[16];
    memcpy(a, "adversarial-xxh6", 16);
    uint64_t w1 = GET_U64(a, 0), w2 = GET_U64(a, 8);
    uint64_t w1b = round0_inv(XXH64_round(0, w1) ^ (UINT64_C(1) << 36));
    uint64_t w2b = round0_inv(XXH64_round(0, w2) ^ (UINT64_C(1) << 63));
    put64(b, w1b); put64(b + 8, w2b);
    printf("\n[A] key-free pair, 16 bytes (L = 2 words)\n");
    print_msg("M1", a, 16); print_msg("M2", b, 16);
    uint64_t N = UINT64_C(1) << lg_pairs, coll = 0, ctrl = 0;
    uint8_t c[16]; memcpy(c, b, 16); c[0] ^= 1;
    for (uint64_t i = 0; i < N; i++) {
        uint64_t s = rng();
        coll += XXH64_impl(a, 16, s) == XXH64_impl(b, 16, s);
        ctrl += XXH64_impl(a, 16, s) == XXH64_impl(c, 16, s);
    }
    printf("  collisions %" PRIu64 " / %" PRIu64 " random seeds; control pair %" PRIu64 "\n", coll, N, ctrl);
    uint64_t s0 = rng();
    printf("  example seed 0x%016" PRIx64 ": %016" PRIx64 " %016" PRIx64 "\n", s0, XXH64_impl(a, 16, s0), XXH64_impl(b, 16, s0));
    printf("  seed 0: %016" PRIx64 " %016" PRIx64 "\n", XXH64_impl(a, 16, 0), XXH64_impl(b, 16, 0));
    int okA = coll == N;

    /* ---- B: key-free 4-way at 24 bytes (3 tail words) ---- */
    printf("\n[B] key-free 4-way cube, 24 bytes (L = 3)\n");
    uint8_t base[24]; memcpy(base, "xxh64 key-free 4way cube", 24);
    uint8_t cube[4][24];
    for (int m = 0; m < 4; m++) {
        int f1 = m & 1, f2 = (m >> 1) & 1;           /* flip bits for words 1 and 2 */
        uint64_t k1 = XXH64_round(0, GET_U64(base, 0)), k2 = XXH64_round(0, GET_U64(base, 8)), k3 = XXH64_round(0, GET_U64(base, 16));
        /* word 1 opens a bit-63 difference (f1); word 2 cancels it and may open a new one (f2); word 3 cancels */
        if (f1) k1 ^= UINT64_C(1) << 36;
        if (f1) k2 ^= UINT64_C(1) << 63;
        if (f2) k2 ^= UINT64_C(1) << 36;
        if (f2) k3 ^= UINT64_C(1) << 63;
        put64(cube[m], round0_inv(k1)); put64(cube[m] + 8, round0_inv(k2)); put64(cube[m] + 16, round0_inv(k3));
        char nm[8]; snprintf(nm, sizeof nm, "C%d", m); print_msg(nm, cube[m], 24);
    }
    uint64_t allB = 0, NB = UINT64_C(1) << 20;
    for (uint64_t i = 0; i < NB; i++) {
        uint64_t s = rng(), h0 = XXH64_impl(cube[0], 24, s); int ok = 1;
        for (int m = 1; m < 4; m++) ok &= XXH64_impl(cube[m], 24, s) == h0;
        allB += ok;
    }
    printf("  all 4 equal for %" PRIu64 " / %" PRIu64 " random seeds\n", allB, NB);
    int okB = allB == NB;

    /* ---- C: long-path family, 64 bytes, J^4 messages ---- */
    const int J = 16; const uint64_t K = (uint64_t)J * J * J * J;
    printf("\n[C] long-path family, 64 bytes (L = 8), J = %d per lane, %" PRIu64 " messages\n", J, K);
    uint8_t fb[64]; memcpy(fb, "XXH64 long-path lane family: 16^4 messages, one class per seed..", 64);
    uint64_t x[4], y[4];
    for (int l = 0; l < 4; l++) { x[l] = GET_U64(fb, 8 * l); y[l] = GET_U64(fb, 32 + 8 * l); }
    uint64_t dx = (UINT64_C(1) << 33) * IP2, dy = XXH_PRIME64_1 * IP2;   /* x += j*dx ; y -= j*dy */
    uint8_t *fam = malloc(K * 64);
    for (uint64_t m = 0; m < K; m++) {
        uint8_t *p = fam + 64 * m;
        for (int l = 0; l < 4; l++) {
            uint64_t j = (m >> (4 * l)) & 15;
            put64(p + 8 * l, x[l] + j * dx); put64(p + 32 + 8 * l, y[l] - j * dy);
        }
    }
    print_msg("F[0]", fam, 64); print_msg("F[1]", fam + 64, 64); print_msg("F[65535]", fam + 64 * (K - 1), 64);
    { uint64_t d = count_distinct(fam, 64, K); printf("  distinct messages: %" PRIu64 " of %" PRIu64 "\n", d, K); if (d != K) { printf("NOT DISTINCT\n"); return 3; } }
    printf("  generator: word l (l=0..3) = x_l + j_l*(2^33*P2^-1), word 4+l = y_l - j_l*(P1*P2^-1), j_l in [0,16)\n");
    /* distinctness */
    uint64_t NF = UINT64_C(1) << lg_fam, allF = 0, minmax = K;
    for (uint64_t t = 0; t < NF; t++) {
        uint64_t s = rng(), h0 = XXH64_impl(fam, 64, s), same = 0;
        for (uint64_t m = 0; m < K; m++) same += XXH64_impl(fam + 64 * m, 64, s) == h0;
        allF += same == K; if (same < minmax) minmax = same;
    }
    double lo, hi; wilson(allF, NF, &lo, &hi);
    printf("  all %" PRIu64 " equal for %" PRIu64 " / %" PRIu64 " random seeds (95%% CI [%.6f, %.6f]); min class of F[0] %" PRIu64 "\n", K, allF, NF, lo, hi, minmax);
    /* exact failure set: union over lanes of { top31(V_l) >= 2^31 - (J-1) }, V_l = init_l(seed) + x_l*P2 */
    const uint64_t init[4] = { XXH_PRIME64_1 + XXH_PRIME64_2, XXH_PRIME64_2, 0, (uint64_t)0 - XXH_PRIME64_1 };
    printf("  exact failure probability <= 4*(J-1)/2^31 = %.3g (union of 4 seed intervals of width (J-1)*2^33)\n", 4.0 * (J - 1) / 2147483648.0);
    /* a seed inside the failure set of lane 0, to show the condition is real */
    uint64_t Vbad = (UINT64_C(0x7FFFFFFF) << 33) | (rng() >> 31);
    uint64_t sbad = Vbad - init[0] - x[0] * XXH_PRIME64_2;
    uint64_t h0 = XXH64_impl(fam, 64, sbad), same = 0;
    for (uint64_t m = 0; m < K; m++) same += XXH64_impl(fam + 64 * m, 64, sbad) == h0;
    printf("  failure-set seed 0x%016" PRIx64 " (lane-0 top31(V) = 2^31-1): class of F[0] has %" PRIu64 " of %" PRIu64 "\n", sbad, same, K);
    uint64_t sx = rng();
    printf("  explicit seed 0x%016" PRIx64 ": XXH64(F[0]) = %016" PRIx64 ", XXH64(F[65535]) = %016" PRIx64 "\n", sx, XXH64_impl(fam, 64, sx), XXH64_impl(fam + 64 * (K - 1), 64, sx));
    int okC = allF == NF;
    free(fam);
    printf("\nsummary: A %s, B %s, C %s\n", okA ? "ok" : "FAIL", okB ? "ok" : "FAIL", okC ? "ok" : "FAIL");
    return (okA && okB && okC) ? 0 : 2;
}
