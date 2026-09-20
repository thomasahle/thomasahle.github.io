/*
 * rs128.c -- XXH3-128 (xxHash 0.8.3) fixed-pair collision rates under the
 * RANDOM-SECRET key model: every one of the 192 secret bytes
 * (XXH3_SECRET_DEFAULT_SIZE) is fresh uniform key material per trial, and
 * optionally the 64-bit seed as well.
 *
 * Build:  cc -O2 -std=gnu11 -pthread -o rs128 rs128.c
 * (xxhash.h v0.8.3, sha256 17973c0d..., must sit next to this file; it is
 *  included verbatim with XXH_INLINE_ALL so the static internal entry point
 *  XXH3_128bits_internal is reachable for the seed+secret variant.)
 *
 * Modes
 *   pair <log2N> <rngseed> <threads> <api> <pairs-file>
 *        api = secret      : XXH3_128bits_withSecret(m, len, secret, 192)      (seed = 0)
 *              secretseed  : XXH3_128bits_internal(m, len, seed, secret, 192,
 *                            XXH3_hashLong_128b_withSecret)  -- seed AND secret random
 *              seed        : XXH3_128bits_withSeed(m, len, seed)  (default public secret;
 *                            the blog row's current model, for control runs)
 *        pairs-file lines: <len> <hexM> <hexM2> <label>   (len bytes each; '#' comments)
 *        Every pair sees the same N keys (deliberately correlated; use one pair per
 *        file for an independent confirmation).  Full 128-bit equality is counted.
 *   fold <log2N> <rngseed> <threads> <masks-file>
 *        masks-file lines: <d0-hex> <d1-hex> <label>.  Samples A,B uniform and counts
 *        fold(A,B) == fold(A^d0, B^d1)  ("eq") and  == fold(...) ^ 2^63 ("twin"),
 *        fold = XXH3_mul128_fold64.  This is exactly the per-block differential that
 *        decides a 17..128-byte collision once the secret words are uniform.
 *
 * Every counter is deterministic given (log2N, rngseed, threads): thread t seeds a
 * xoshiro256** generator from splitmix64(rngseed * 0x9E3779B97F4A7C15 + t + 1).
 */
#define XXH_INLINE_ALL
#define XXH_STATIC_LINKING_ONLY
#include "xxhash.h"

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define SECRET_BYTES 192
#define MAXP 4096
#define MAXLEN 1024

/* ---- RNG ---- */
static inline uint64_t splitmix64(uint64_t *s) {
    uint64_t z = (*s += 0x9E3779B97F4A7C15ULL);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    return z ^ (z >> 31);
}
typedef struct { uint64_t s[4]; } xo_t;
static inline uint64_t rotl(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
static inline uint64_t xo_next(xo_t *g) {
    uint64_t *s = g->s;
    uint64_t const r = rotl(s[1] * 5, 7) * 9;
    uint64_t const t = s[1] << 17;
    s[2] ^= s[0]; s[3] ^= s[1]; s[1] ^= s[2]; s[0] ^= s[3];
    s[2] ^= t; s[3] = rotl(s[3], 45);
    return r;
}
static void xo_seed(xo_t *g, uint64_t rngseed, int t) {
    uint64_t sm = rngseed * 0x9E3779B97F4A7C15ULL + (uint64_t)t + 1;
    for (int i = 0; i < 4; i++) g->s[i] = splitmix64(&sm);
}

/* ---- pairs ---- */
typedef struct {
    size_t len;
    uint8_t m[MAXLEN], m2[MAXLEN];
    char label[128];
} pair_t;
static pair_t pairs[MAXP];
static int npairs;

static int hex2bin(const char *h, uint8_t *out, size_t want) {
    size_t n = strlen(h);
    if (n != 2 * want) return -1;
    for (size_t i = 0; i < want; i++) {
        unsigned v;
        if (sscanf(h + 2 * i, "%2x", &v) != 1) return -1;
        out[i] = (uint8_t)v;
    }
    return 0;
}
static void load_pairs(const char *fn) {
    FILE *f = fopen(fn, "r");
    if (!f) { perror(fn); exit(1); }
    char line[8192];
    while (fgets(line, sizeof line, f)) {
        if (line[0] == '#' || line[0] == '\n') continue;
        char h1[4096], h2[4096], lab[128] = "";
        unsigned long len;
        int k = sscanf(line, "%lu %4095s %4095s %127s", &len, h1, h2, lab);
        if (k < 3) { fprintf(stderr, "bad line: %s", line); exit(1); }
        if (len > MAXLEN) { fprintf(stderr, "len too big\n"); exit(1); }
        pair_t *p = &pairs[npairs];
        p->len = len;
        if (hex2bin(h1, p->m, len) || hex2bin(h2, p->m2, len)) {
            fprintf(stderr, "bad hex (len %lu): %s", len, line); exit(1);
        }
        strncpy(p->label, lab, 127);
        npairs++;
    }
    fclose(f);
}

enum { API_SECRET, API_SECRETSEED, API_SEED };
static int api_mode;
static uint64_t g_N;       /* trials per thread */
static uint64_t g_rngseed;

typedef struct {
    int t;
    uint64_t count[MAXP];
    int have_wit[MAXP];
    uint8_t wit_secret[MAXP][SECRET_BYTES];
    uint64_t wit_seed[MAXP];
    XXH128_hash_t wit_h[MAXP];
} pw_t;

static inline XXH128_hash_t H(const uint8_t *m, size_t len, const uint8_t *secret, uint64_t seed) {
    switch (api_mode) {
    case API_SECRET:     return XXH3_128bits_withSecret(m, len, secret, SECRET_BYTES);
    case API_SECRETSEED: return XXH3_128bits_internal(m, len, seed, secret, SECRET_BYTES, XXH3_hashLong_128b_withSecret);
    default:             return XXH3_128bits_withSeed(m, len, seed);
    }
}

static void *pair_worker(void *arg) {
    pw_t *w = (pw_t *)arg;
    xo_t g; xo_seed(&g, g_rngseed, w->t);
    uint64_t secret_w[SECRET_BYTES / 8];
    uint8_t *secret = (uint8_t *)secret_w;
    for (uint64_t i = 0; i < g_N; i++) {
        for (int k = 0; k < SECRET_BYTES / 8; k++) secret_w[k] = xo_next(&g);   /* stored little-endian on x86 */
        uint64_t seed = xo_next(&g);
        for (int p = 0; p < npairs; p++) {
            XXH128_hash_t a = H(pairs[p].m, pairs[p].len, secret, seed);
            XXH128_hash_t b = H(pairs[p].m2, pairs[p].len, secret, seed);
            if (a.low64 == b.low64 && a.high64 == b.high64) {
                w->count[p]++;
                if (!w->have_wit[p]) {
                    w->have_wit[p] = 1;
                    memcpy(w->wit_secret[p], secret, SECRET_BYTES);
                    w->wit_seed[p] = seed; w->wit_h[p] = a;
                }
            }
        }
    }
    return NULL;
}

static void run_pairs(int log2N, uint64_t rngseed, int threads, const char *api, const char *fn) {
    if (!strcmp(api, "secret")) api_mode = API_SECRET;
    else if (!strcmp(api, "secretseed")) api_mode = API_SECRETSEED;
    else if (!strcmp(api, "seed")) api_mode = API_SEED;
    else { fprintf(stderr, "api must be secret|secretseed|seed\n"); exit(1); }
    load_pairs(fn);
    uint64_t N = 1ULL << log2N;
    g_N = N / (uint64_t)threads; g_rngseed = rngseed;
    uint64_t Ntot = g_N * (uint64_t)threads;
    printf("mode=pair api=%s log2N=%d N=%llu rngseed=%llu threads=%d pairs=%d secret_bytes=%d xxhash=%d.%d.%d\n",
           api, log2N, (unsigned long long)Ntot, (unsigned long long)rngseed, threads, npairs, SECRET_BYTES,
           XXH_VERSION_MAJOR, XXH_VERSION_MINOR, XXH_VERSION_RELEASE);
    fflush(stdout);
    pw_t *ws = calloc((size_t)threads, sizeof(pw_t));
    pthread_t *th = calloc((size_t)threads, sizeof(pthread_t));
    for (int t = 0; t < threads; t++) { ws[t].t = t; pthread_create(&th[t], NULL, pair_worker, &ws[t]); }
    for (int t = 0; t < threads; t++) pthread_join(th[t], NULL);
    for (int p = 0; p < npairs; p++) {
        uint64_t c = 0; int wt = -1;
        for (int t = 0; t < threads; t++) { c += ws[t].count[p]; if (wt < 0 && ws[t].have_wit[p]) wt = t; }
        double rate = (double)c / (double)Ntot;
        printf("PAIR %s len=%zu count=%llu N=%llu rate=%.6e log2rate=%.4f\n", pairs[p].label, pairs[p].len,
               (unsigned long long)c, (unsigned long long)Ntot, rate, c ? log2(rate) : -INFINITY);
        if (wt >= 0) {
            printf("  witness seed=%016llx h=%016llx%016llx secret=", (unsigned long long)ws[wt].wit_seed[p],
                   (unsigned long long)ws[wt].wit_h[p].high64, (unsigned long long)ws[wt].wit_h[p].low64);
            for (int k = 0; k < SECRET_BYTES; k++) printf("%02x", ws[wt].wit_secret[p][k]);
            printf("\n");
        }
    }
    fflush(stdout);
}

/* ---- fold differential ---- */
typedef struct { uint64_t d0, d1; char label[128]; } mask_t;
static mask_t masks[65536];
static int nmasks;
typedef struct { int t; uint64_t d0, d1; uint64_t eq, twin; } fw_t;

static void *fold_worker(void *arg) {
    fw_t *w = (fw_t *)arg;
    xo_t g; xo_seed(&g, g_rngseed, w->t);
    uint64_t eq = 0, twin = 0, d0 = w->d0, d1 = w->d1;
    for (uint64_t i = 0; i < g_N; i++) {
        uint64_t a = xo_next(&g), b = xo_next(&g);
        uint64_t x = XXH3_mul128_fold64(a, b) ^ XXH3_mul128_fold64(a ^ d0, b ^ d1);
        eq += (x == 0);
        twin += (x == (1ULL << 63));
    }
    w->eq = eq; w->twin = twin;
    return NULL;
}

static void run_fold(int log2N, uint64_t rngseed, int threads, const char *fn) {
    FILE *f = fopen(fn, "r");
    if (!f) { perror(fn); exit(1); }
    char line[1024];
    while (fgets(line, sizeof line, f)) {
        if (line[0] == '#' || line[0] == '\n') continue;
        mask_t *m = &masks[nmasks];
        unsigned long long a, b;
        m->label[0] = 0;
        if (sscanf(line, "%llx %llx %127s", &a, &b, m->label) < 2) { fprintf(stderr, "bad mask line %s", line); exit(1); }
        m->d0 = a; m->d1 = b; nmasks++;
    }
    fclose(f);
    uint64_t N = 1ULL << log2N;
    g_N = N / (uint64_t)threads; g_rngseed = rngseed;
    uint64_t Ntot = g_N * (uint64_t)threads;
    printf("mode=fold log2N=%d N=%llu rngseed=%llu threads=%d masks=%d\n", log2N, (unsigned long long)Ntot,
           (unsigned long long)rngseed, threads, nmasks);
    fflush(stdout);
    fw_t *ws = calloc((size_t)threads, sizeof(fw_t));
    pthread_t *th = calloc((size_t)threads, sizeof(pthread_t));
    for (int mi = 0; mi < nmasks; mi++) {
        for (int t = 0; t < threads; t++) { ws[t].t = t; ws[t].d0 = masks[mi].d0; ws[t].d1 = masks[mi].d1; pthread_create(&th[t], NULL, fold_worker, &ws[t]); }
        uint64_t eq = 0, twin = 0;
        for (int t = 0; t < threads; t++) { pthread_join(th[t], NULL); eq += ws[t].eq; twin += ws[t].twin; }
        printf("MASK %016llx %016llx %s eq=%llu twin=%llu N=%llu log2eq=%.4f log2twin=%.4f\n",
               (unsigned long long)masks[mi].d0, (unsigned long long)masks[mi].d1, masks[mi].label,
               (unsigned long long)eq, (unsigned long long)twin, (unsigned long long)Ntot,
               eq ? log2((double)eq / (double)Ntot) : -INFINITY, twin ? log2((double)twin / (double)Ntot) : -INFINITY);
        fflush(stdout);
    }
}

static void selftest(void) {
    /* SMHasher3-style verification of the embedded XXH3-128 (expected 0x288DAA94, see verify/xxh3-128). */
    uint8_t key[256], hashes[256 * 16];
    for (int i = 0; i < 256; i++) key[i] = (uint8_t)i;
    for (int i = 0; i < 256; i++) {
        XXH128_hash_t h = XXH3_128bits_withSeed(key, (size_t)i, (uint64_t)(256 - i));
        XXH128_canonical_t c; XXH128_canonicalFromHash(&c, h);
        memcpy(hashes + 16 * i, c.digest, 16);
    }
    XXH128_hash_t h = XXH3_128bits_withSeed(hashes, sizeof hashes, 0);
    XXH128_canonical_t c; XXH128_canonicalFromHash(&c, h);
    uint32_t v = (uint32_t)c.digest[0] | ((uint32_t)c.digest[1] << 8) | ((uint32_t)c.digest[2] << 16) | ((uint32_t)c.digest[3] << 24);
    printf("selftest XXH3-128 verification=%08X expected=288DAA94 %s\n", v, v == 0x288DAA94u ? "PASS" : "FAIL");
    if (v != 0x288DAA94u) exit(2);
}

int main(int argc, char **argv) {
    if (argc < 2) { fprintf(stderr, "usage: rs128 pair|fold ...\n"); return 1; }
    selftest();
    if (!strcmp(argv[1], "pair") && argc == 7) run_pairs(atoi(argv[2]), strtoull(argv[3], 0, 10), atoi(argv[4]), argv[5], argv[6]);
    else if (!strcmp(argv[1], "fold") && argc == 6) run_fold(atoi(argv[2]), strtoull(argv[3], 0, 10), atoi(argv[4]), argv[5]);
    else { fprintf(stderr, "bad args\n"); return 1; }
    return 0;
}
