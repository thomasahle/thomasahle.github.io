/* xxh3_verify.c - independent verification of XXH3-64 (xxHash v0.8.3) fixed-pair
 * collision rates under the random-secret key model.  Written from scratch against the
 * upstream header (xxhash.h v0.8.3 beside this file, sha256 17973c0d...); it shares no
 * code with the search harness xxh3_rs.c: different RNG family (per-thread SplitMix64
 * streams, not xoshiro256**), own fold64, own key layout, and every witness is re-hashed
 * through the public API in the main thread before it is printed.
 *
 * Build: cc -O3 -std=c11 -pthread -march=native xxh3_verify.c -lm -o xxh3_verify
 *
 * Key models (MODE):
 *   secret  XXH3_64bits_withSecret(m, len, S, 192)            S = 192 uniform bytes (seed is 0 by API definition)
 *   both    XXH3_64bits_internal(m, len, seed, S, 192)        seed uniform AND S uniform (no public one-shot API
 *                                                             exposes this for len <= 240; see 'ss')
 *   seed    XXH3_64bits_withSeed(m, len, seed)                default kSecret, uniform 64-bit seed
 *   ss      XXH3_64bits_withSecretandSeed(m, len, S, 192, seed)   upstream ignores S when len <= 240
 *   stream  XXH3_64bits_reset_withSecretandSeed + update + digest  (streaming form of 'ss')
 *
 * Commands:
 *   pair MODE LOG2N SALT THREADS MHEX M2HEX     count keys for which H(m) == H(m')
 *   fold LOG2N SALT THREADS DHEX EHEX           P[fold64(x,y) == fold64(x^d, y^e)], x,y uniform
 *
 * Every trial draws a fresh key (seed word + 24 secret words); the pair is fixed before any key is drawn.
 */
#define XXH_INLINE_ALL
#include "xxhash.h"
#include <inttypes.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ---- RNG: SplitMix64 stream per thread (Steele/Lea/Flood 2014), seeded through a second mixer ---- */
static inline uint64_t sm64(uint64_t *s) {
    uint64_t z = (*s += 0x9E3779B97F4A7C15ULL);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    return z ^ (z >> 31);
}
static inline uint64_t fmix64(uint64_t k) {              /* MurmurHash3 finalizer */
    k ^= k >> 33; k *= 0xFF51AFD7ED558CCDULL; k ^= k >> 33; k *= 0xC4CEB9FE1A85EC53ULL; k ^= k >> 33; return k;
}
static uint64_t stream_seed(uint64_t salt, unsigned tid) {
    return fmix64(salt * 0xD1B54A32D192ED03ULL + 0x8CB92BA72F3D8DD7ULL) ^ fmix64(((uint64_t)tid << 32) | 0x5EEDu);
}

/* ---- key material ---- */
#define SECRET_BYTES 192
#define SECRET_WORDS (SECRET_BYTES / 8)
typedef struct { uint64_t seed; uint8_t secret[SECRET_BYTES]; } key_t_;
static inline void draw_key(uint64_t *rs, key_t_ *k, int need_secret) {
    k->seed = sm64(rs);
    if (need_secret) for (int i = 0; i < SECRET_WORDS; i++) { uint64_t w = sm64(rs); memcpy(k->secret + 8 * i, &w, 8); }
}

enum { M_SECRET, M_BOTH, M_SEED, M_SS, M_STREAM };
static int parse_mode(const char *s) {
    static const char *names[] = { "secret", "both", "seed", "ss", "stream" };
    for (int i = 0; i < 5; i++) if (!strcmp(s, names[i])) return i;
    fprintf(stderr, "unknown mode '%s'\n", s); exit(2);
}
static uint64_t hash_stream(const uint8_t *m, size_t len, const key_t_ *k) {
    XXH3_state_t st; XXH3_INITSTATE(&st);
    if (XXH3_64bits_reset_withSecretandSeed(&st, k->secret, SECRET_BYTES, k->seed) != XXH_OK) { fprintf(stderr, "reset failed\n"); exit(3); }
    /* feed in two chunks to exercise the buffer path */
    size_t a = len / 3;
    XXH3_64bits_update(&st, m, a); XXH3_64bits_update(&st, m + a, len - a);
    return XXH3_64bits_digest(&st);
}
static inline uint64_t hash_mode(int mode, const uint8_t *m, size_t len, const key_t_ *k) {
    switch (mode) {
    case M_SECRET: return XXH3_64bits_withSecret(m, len, k->secret, SECRET_BYTES);
    case M_BOTH:   return XXH3_64bits_internal(m, len, k->seed, k->secret, SECRET_BYTES, XXH3_hashLong_64b_withSecret);
    case M_SEED:   return XXH3_64bits_withSeed(m, len, k->seed);
    case M_SS:     return XXH3_64bits_withSecretandSeed(m, len, k->secret, SECRET_BYTES, k->seed);
    default:       return hash_stream(m, len, k);
    }
}

/* ---- hex ---- */
static size_t unhex(const char *h, uint8_t *out, size_t cap) {
    size_t n = strlen(h); if (n & 1) { fprintf(stderr, "odd-length hex\n"); exit(2); }
    if (n / 2 > cap) { fprintf(stderr, "hex too long\n"); exit(2); }
    for (size_t i = 0; i < n / 2; i++) { unsigned v; if (sscanf(h + 2 * i, "%2x", &v) != 1) { fprintf(stderr, "bad hex\n"); exit(2); } out[i] = (uint8_t)v; }
    return n / 2;
}
static void hex(const uint8_t *b, size_t n) { for (size_t i = 0; i < n; i++) printf("%02x", b[i]); }

/* ---- pair ---- */
#define MAXLEN 4096
typedef struct {
    int mode; unsigned tid; uint64_t n, salt; const uint8_t *m, *m2; size_t len, len2;
    uint64_t hits; int have_w; key_t_ w; uint64_t w_out;
} pjob;
static void *pair_worker(void *p) {
    pjob *j = p; uint64_t rs = stream_seed(j->salt, j->tid); key_t_ k; uint64_t hits = 0;
    int need_secret = j->mode != M_SEED;
    for (uint64_t i = 0; i < j->n; i++) {
        draw_key(&rs, &k, need_secret);
        uint64_t a = hash_mode(j->mode, j->m, j->len, &k), b = hash_mode(j->mode, j->m2, j->len2, &k);
        if (a == b) { hits++; if (!j->have_w) { j->have_w = 1; j->w = k; j->w_out = a; } }
    }
    j->hits = hits; return NULL;
}
static int cmd_pair(int argc, char **argv) {
    if (argc != 8) { fprintf(stderr, "pair MODE LOG2N SALT THREADS MHEX M2HEX\n"); return 2; }
    int mode = parse_mode(argv[2]); int log2n = atoi(argv[3]); uint64_t salt = strtoull(argv[4], 0, 0); int T = atoi(argv[5]);
    static uint8_t m[MAXLEN], m2[MAXLEN]; size_t len = unhex(argv[6], m, MAXLEN), len2 = unhex(argv[7], m2, MAXLEN);
    if (T < 1 || T > 256 || log2n < 0 || log2n > 62) { fprintf(stderr, "bad THREADS/LOG2N\n"); return 2; }
    uint64_t N = 1ULL << log2n;
    pjob *J = calloc((size_t)T, sizeof *J); pthread_t *th = calloc((size_t)T, sizeof *th);
    struct timespec t0, t1; clock_gettime(CLOCK_MONOTONIC, &t0);
    for (int t = 0; t < T; t++) {
        J[t] = (pjob){ .mode = mode, .tid = (unsigned)t, .n = N / (uint64_t)T + ((uint64_t)t < N % (uint64_t)T), .salt = salt,
                       .m = m, .m2 = m2, .len = len, .len2 = len2 };
        pthread_create(&th[t], 0, pair_worker, &J[t]);
    }
    uint64_t hits = 0; int wt = -1;
    for (int t = 0; t < T; t++) { pthread_join(th[t], 0); hits += J[t].hits; if (wt < 0 && J[t].have_w) wt = t; }
    clock_gettime(CLOCK_MONOTONIC, &t1);
    double secs = (double)(t1.tv_sec - t0.tv_sec) + 1e-9 * (double)(t1.tv_nsec - t0.tv_nsec);
    printf("xxh3_verify pair mode=%s len=%zu/%zu keys=2^%d (%" PRIu64 ") salt=%" PRIu64 " threads=%d wall=%.0fs\n",
           argv[2], len, len2, log2n, N, salt, T, secs);
    printf("m  = "); hex(m, len); printf("\nm' = "); hex(m2, len2); printf("\n");
    printf("per-thread hits:"); for (int t = 0; t < T; t++) printf(" %" PRIu64, J[t].hits); printf("\n");
    printf("hits = %" PRIu64 " / 2^%d", hits, log2n);
    if (hits) printf("  rate = 2^%.4f\n", __builtin_log2((double)hits / (double)N));
    else printf("  rate = 0  (95%% upper bound 2^%.2f)\n", __builtin_log2(-__builtin_log(0.05) / (double)N));
    if (wt >= 0) {
        const key_t_ *w = &J[wt].w;
        /* re-check the witness through the public API in this thread */
        uint64_t a = hash_mode(mode, m, len, w), b = hash_mode(mode, m2, len2, w);
        printf("first witness (thread %d): H(m)=%016" PRIx64 " H(m')=%016" PRIx64 " recheck=%s\n", wt, a, b,
               (a == b && a == J[wt].w_out) ? "ok" : "MISMATCH");
        printf("  seed   = %016" PRIx64 "%s\n", w->seed, mode == M_SECRET ? " (unused: withSecret has no seed)" : (mode == M_SS || mode == M_STREAM) ? " (the only key at len<=240)" : "");
        if (mode != M_SEED) { printf("  secret = "); hex(w->secret, SECRET_BYTES); printf("%s\n", (mode == M_SS || mode == M_STREAM) ? "  (ignored at len<=240)" : ""); }
    }
    free(J); free(th); return 0;
}

/* ---- fold differential ---- */
static inline uint64_t my_fold64(uint64_t a, uint64_t b) { unsigned __int128 p = (unsigned __int128)a * b; return (uint64_t)p ^ (uint64_t)(p >> 64); }
typedef struct { unsigned tid; uint64_t n, salt, d, e, hits; } fjob;
static void *fold_worker(void *p) {
    fjob *j = p; uint64_t rs = stream_seed(j->salt, j->tid), h = 0;
    for (uint64_t i = 0; i < j->n; i++) { uint64_t x = sm64(&rs), y = sm64(&rs); h += my_fold64(x, y) == my_fold64(x ^ j->d, y ^ j->e); }
    j->hits = h; return NULL;
}
static int cmd_fold(int argc, char **argv) {
    if (argc != 7) { fprintf(stderr, "fold LOG2N SALT THREADS DHEX EHEX\n"); return 2; }
    int log2n = atoi(argv[2]); uint64_t salt = strtoull(argv[3], 0, 0); int T = atoi(argv[4]);
    uint64_t d = strtoull(argv[5], 0, 16), e = strtoull(argv[6], 0, 16), N = 1ULL << log2n;
    fjob *J = calloc((size_t)T, sizeof *J); pthread_t *th = calloc((size_t)T, sizeof *th);
    for (int t = 0; t < T; t++) { J[t] = (fjob){ .tid = (unsigned)t, .n = N / (uint64_t)T + ((uint64_t)t < N % (uint64_t)T), .salt = salt, .d = d, .e = e }; pthread_create(&th[t], 0, fold_worker, &J[t]); }
    uint64_t h = 0; for (int t = 0; t < T; t++) { pthread_join(th[t], 0); h += J[t].hits; }
    printf("xxh3_verify fold d=%016" PRIx64 " e=%016" PRIx64 " samples=2^%d salt=%" PRIu64 " hits=%" PRIu64, d, e, log2n, salt, h);
    if (h) printf(" rate=2^%.4f\n", __builtin_log2((double)h / (double)N)); else printf(" rate=0\n");
    free(J); free(th); return 0;
}

int main(int argc, char **argv) {
    if (argc < 2) { fprintf(stderr, "usage: xxh3_verify pair|fold ...\n"); return 2; }
    if (!strcmp(argv[1], "pair")) return cmd_pair(argc, argv);
    if (!strcmp(argv[1], "fold")) return cmd_fold(argc, argv);
    fprintf(stderr, "unknown command\n"); return 2;
}
