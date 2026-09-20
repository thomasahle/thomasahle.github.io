/* xxh3_rs.c - XXH3-64 (xxHash v0.8.3) fixed-pair collision rates under the
 * random-secret key model, plus a structured candidate scan and a search over
 * the differential of the 64-bit multiply-fold that keys every 16-byte block.
 *
 * Build:  cc -O3 -std=c11 -pthread xxh3_rs.c -lm -o xxh3_rs    (xxhash.h v0.8.3 beside it)
 *
 * Key models (argument MODE):
 *   secret : XXH3_64bits_withSecret(m, len, S, 192)             S = 192 uniform bytes, seed fixed 0
 *   both   : XXH3_64bits_internal(m, len, seed, S, 192)         seed uniform 64-bit AND S uniform
 *            (the hypothetical model of the task; no public one-shot API exposes it for len <= 240,
 *             see mode ss)
 *   seed   : XXH3_64bits_withSeed(m, len, seed)                 default kSecret (the page's old model)
 *   ss     : XXH3_64bits_withSecretandSeed(m, len, S, 192, seed) - for len <= 240 upstream ignores S
 *
 * Sub-commands:
 *   pair MODE LOG2N SALT THREADS MHEX M2HEX      collision count of one fixed pair
 *   scan MODE LOG2N SALT THREADS LMIN LMAX [LOG2N_CHEAP]
 *                                               structured candidates at every length in [LMIN,LMAX]
 *   fold LOG2N SALT THREADS DELTA EPS [...]      P[fold(x,y)=fold(x^d,y^e)] for uniform x,y
 *   foldclimb LOG2N SALT THREADS ROUNDS          hill-climb over (d,e) from the all-ones seeds
 *
 * Every trial draws fresh key material; the pair is fixed before the key is drawn.
 * RNG: one xoshiro256** stream per thread, seeded by SplitMix64(SALT * 0x9E3779B97F4A7C15 + tid + 1).
 */
#define XXH_INLINE_ALL
#include "xxhash.h"
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------------- RNG ---------------- */
typedef struct { uint64_t s[4]; } rng_t;
static uint64_t splitmix(uint64_t *x) {
    uint64_t z = (*x += 0x9E3779B97F4A7C15ull);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBull;
    return z ^ (z >> 31);
}
static void rng_seed(rng_t *r, uint64_t salt, uint64_t tid) {
    uint64_t x = salt * 0x9E3779B97F4A7C15ull + tid + 1;
    for (int i = 0; i < 4; i++) r->s[i] = splitmix(&x);
}
static inline uint64_t rotl64(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
static inline uint64_t rng_next(rng_t *r) {
    uint64_t *s = r->s;
    uint64_t res = rotl64(s[1] * 5, 7) * 9, t = s[1] << 17;
    s[2] ^= s[0]; s[3] ^= s[1]; s[1] ^= s[2]; s[0] ^= s[3]; s[2] ^= t; s[3] = rotl64(s[3], 45);
    return res;
}

/* ---------------- key models ---------------- */
#define SECRET_BYTES 192
enum { MODE_SECRET = 0, MODE_BOTH = 1, MODE_SEED = 2, MODE_SS = 3 };
static int parse_mode(const char *s) {
    if (!strcmp(s, "secret")) return MODE_SECRET;
    if (!strcmp(s, "both")) return MODE_BOTH;
    if (!strcmp(s, "seed")) return MODE_SEED;
    if (!strcmp(s, "ss")) return MODE_SS;
    fprintf(stderr, "unknown mode %s\n", s); exit(2);
}
typedef struct { uint64_t seed; uint64_t secret[SECRET_BYTES / 8]; } key_t_;
static inline void draw_key(rng_t *r, int mode, key_t_ *k) {
    k->seed = rng_next(r);
    if (mode != MODE_SEED) for (int i = 0; i < SECRET_BYTES / 8; i++) k->secret[i] = rng_next(r);
}
static inline uint64_t H(int mode, const key_t_ *k, const uint8_t *m, size_t len) {
    switch (mode) {
    case MODE_SECRET: return XXH3_64bits_withSecret(m, len, k->secret, SECRET_BYTES);
    case MODE_BOTH:   return XXH3_64bits_internal(m, len, (XXH64_hash_t)k->seed, k->secret, SECRET_BYTES,
                                                  XXH3_hashLong_64b_withSecret);
    case MODE_SEED:   return XXH3_64bits_withSeed(m, len, (XXH64_hash_t)k->seed);
    default:          return XXH3_64bits_withSecretandSeed(m, len, k->secret, SECRET_BYTES, (XXH64_hash_t)k->seed);
    }
}

/* ---------------- hex ---------------- */
static int hexval(int c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}
static size_t from_hex(const char *h, uint8_t *out, size_t cap) {
    size_t n = strlen(h);
    if (n % 2) { fprintf(stderr, "odd hex\n"); exit(2); }
    if (n / 2 > cap) { fprintf(stderr, "hex too long\n"); exit(2); }
    for (size_t i = 0; i < n / 2; i++) out[i] = (uint8_t)(hexval(h[2 * i]) * 16 + hexval(h[2 * i + 1]));
    return n / 2;
}
static void print_hex(const uint8_t *b, size_t n) { for (size_t i = 0; i < n; i++) printf("%02x", b[i]); }

/* ---------------- pair ---------------- */
#define MAXLEN 2048
typedef struct {
    int mode, tid; uint64_t n, salt;
    const uint8_t *m, *m2; size_t len, len2;
    uint64_t hits; int have_witness; key_t_ witness; uint64_t witness_out;
} pair_job;
static void *pair_thread(void *arg) {
    pair_job *j = (pair_job *)arg;
    rng_t r; rng_seed(&r, j->salt, (uint64_t)j->tid);
    key_t_ k; uint64_t hits = 0;
    for (uint64_t i = 0; i < j->n; i++) {
        draw_key(&r, j->mode, &k);
        uint64_t a = H(j->mode, &k, j->m, j->len);
        uint64_t b = H(j->mode, &k, j->m2, j->len2);
        if (a == b) { hits++; if (!j->have_witness) { j->have_witness = 1; j->witness = k; j->witness_out = a; } }
    }
    j->hits = hits;
    return NULL;
}
static void print_key(int mode, const key_t_ *k) {
    printf("  seed = %016llx", (unsigned long long)k->seed);
    if (mode == MODE_SECRET || mode == MODE_SS) printf(" (mode %s: seed %s)", mode == MODE_SECRET ? "secret" : "ss",
                                                       mode == MODE_SECRET ? "unused, API uses 0" : "used only if len > 240");
    printf("\n");
    if (mode != MODE_SEED) {
        printf("  secret (192 bytes, little-endian words) = ");
        for (int i = 0; i < SECRET_BYTES / 8; i++) { uint64_t w = k->secret[i]; for (int b = 0; b < 8; b++) printf("%02x", (unsigned)((w >> (8 * b)) & 0xff)); }
        printf("\n");
    }
}
static int cmd_pair(int argc, char **argv) {
    if (argc < 8) { fprintf(stderr, "pair MODE LOG2N SALT THREADS MHEX M2HEX\n"); return 2; }
    int mode = parse_mode(argv[2]); int log2n = atoi(argv[3]); uint64_t salt = strtoull(argv[4], 0, 0); int T = atoi(argv[5]);
    static uint8_t m[MAXLEN], m2[MAXLEN];
    size_t len = from_hex(argv[6], m, MAXLEN), len2 = from_hex(argv[7], m2, MAXLEN);
    uint64_t N = 1ull << log2n;
    pair_job *jobs = calloc((size_t)T, sizeof *jobs); pthread_t *th = calloc((size_t)T, sizeof *th);
    for (int t = 0; t < T; t++) {
        jobs[t] = (pair_job){ .mode = mode, .tid = t, .n = N / (uint64_t)T + (t < (int)(N % (uint64_t)T)), .salt = salt,
                              .m = m, .m2 = m2, .len = len, .len2 = len2 };
        pthread_create(&th[t], 0, pair_thread, &jobs[t]);
    }
    uint64_t hits = 0; int wt = -1;
    for (int t = 0; t < T; t++) { pthread_join(th[t], 0); hits += jobs[t].hits; if (wt < 0 && jobs[t].have_witness) wt = t; }
    printf("pair mode=%s len=%zu/%zu keys=2^%d salt=%llu threads=%d\n", argv[2], len, len2, log2n, (unsigned long long)salt, T);
    printf("m  = "); print_hex(m, len); printf("\nm' = "); print_hex(m2, len2); printf("\n");
    printf("hits = %llu / %llu", (unsigned long long)hits, (unsigned long long)N);
    if (hits) printf("  rate = 2^%.4f", __builtin_log2((double)hits / (double)N)); else printf("  rate = 0 (95%% upper bound 2^%.2f)", __builtin_log2(3.0 / (double)N));
    printf("\n");
    if (wt >= 0) { printf("first witness (thread %d): output %016llx\n", wt, (unsigned long long)jobs[wt].witness_out); print_key(mode, &jobs[wt].witness); }
    return 0;
}

/* ---------------- scan ---------------- */
typedef struct { size_t len2; uint8_t m2[MAXLEN]; char desc[96]; uint64_t hits; int cheap; } cand;
static int ncand; static cand *cands;
static void add_cand(const uint8_t *m2, size_t len2, int cheap, const char *desc) {
    cands = realloc(cands, sizeof(cand) * (size_t)(ncand + 1));
    cand *c = &cands[ncand++]; memset(c, 0, sizeof *c);
    c->len2 = len2; memcpy(c->m2, m2, len2); c->cheap = cheap; snprintf(c->desc, sizeof c->desc, "%s", desc);
}
static void xor_word(uint8_t *m, size_t off, uint64_t mask) { for (int b = 0; b < 8; b++) m[off + b] ^= (uint8_t)(mask >> (8 * b)); }
static void build_cands(const uint8_t *m, size_t L) {
    static uint8_t t[MAXLEN]; char d[96];
    int big = L > 64;
    /* byte complements (cheap) */
    if (!big) for (size_t i = 0; i < L; i++) { memcpy(t, m, L); t[i] ^= 0xff; snprintf(d, sizeof d, "byte %zu ^ ff", i); add_cand(t, L, 1, d); }
    /* word masks at aligned offsets and tail-aligned offsets */
    static const uint64_t masks[] = { 0xffffffffffffffffull, 0x5555555555555555ull, 0xaaaaaaaaaaaaaaaaull, 0x8000000000000000ull, 1ull, 0xfffffffffffffffdull, 0x7fffffffffffffffull };
    static const char *mnames[] = { "M", "A", "~A", "T", "1", "M-2", "M-T" };
    if (L >= 8) {
        size_t offs[600]; int no = 0;
        for (size_t o = 0; o + 8 <= L; o += 8) offs[no++] = o;
        for (size_t o = L - 8; ; o -= 8) { int dup = 0; for (int i = 0; i < no; i++) if (offs[i] == o) dup = 1; if (!dup && no < 590) offs[no++] = o; if (o < 8) break; }
        for (int i = 0; i < no; i++) for (int mi = 0; mi < 7; mi++) {
            if (big && mi > 0) continue;
            memcpy(t, m, L); xor_word(t, offs[i], masks[mi]); snprintf(d, sizeof d, "word@%zu ^ %s", offs[i], mnames[mi]); add_cand(t, L, mi >= 3, d);
        }
        /* two adjacent words (a 16-byte block) complemented; block at aligned and tail-aligned offsets */
        if (L >= 16) for (int i = 0; i < no; i++) { size_t o = offs[i]; if (o + 16 > L) continue; if (o % 8) continue;
            memcpy(t, m, L); xor_word(t, o, ~0ull); xor_word(t, o + 8, ~0ull); snprintf(d, sizeof d, "block@%zu ^ (M,M)", o); add_cand(t, L, 0, d);
            memcpy(t, m, L); xor_word(t, o, ~0ull); xor_word(t, o + 8, 0xfffffffffffffffdull); snprintf(d, sizeof d, "block@%zu ^ (M,M-2)", o); add_cand(t, L, 1, d);
            memcpy(t, m, L); xor_word(t, o, 0x5555555555555555ull); xor_word(t, o + 8, 0x5555555555555555ull); snprintf(d, sizeof d, "block@%zu ^ (A,A)", o); add_cand(t, L, 1, d);
        }
        if (L >= 16) { size_t o = L - 16; if (o % 8) { memcpy(t, m, L); xor_word(t, o, ~0ull); xor_word(t, o + 8, ~0ull); snprintf(d, sizeof d, "block@%zu ^ (M,M)", o); add_cand(t, L, 0, d); } }
    }
    if (L >= 4 && L <= 8) { /* the 4..8 path reads 32-bit words at 0 and L-4 */
        memcpy(t, m, L); for (int b = 0; b < 4; b++) t[b] ^= 0xff; add_cand(t, L, 0, "dword@0 ^ M");
        memcpy(t, m, L); for (int b = 0; b < 4; b++) t[L - 4 + b] ^= 0xff; add_cand(t, L, 0, "dword@L-4 ^ M");
        memcpy(t, m, L); t[3] ^= 0x80; add_cand(t, L, 0, "dword@0 top bit");
        memcpy(t, m, L); t[L - 1] ^= 0x80; add_cand(t, L, 0, "dword@L-4 top bit");
    }
    /* full complement */
    memcpy(t, m, L); for (size_t i = 0; i < L; i++) t[i] ^= 0xff; add_cand(t, L, 0, "all ^ ff");
    if (!big) {
        /* random masks: 8 full-random, 8 weight-2, 8 weight-3 (deterministic generator) */
        uint64_t g = 0x2026091900001000ull + L;
        for (int i = 0; i < 8; i++) { memcpy(t, m, L); for (size_t b = 0; b < L; b++) t[b] ^= (uint8_t)splitmix(&g); snprintf(d, sizeof d, "random mask #%d", i); add_cand(t, L, 1, d); }
        for (int w = 2; w <= 3; w++) for (int i = 0; i < 8; i++) {
            memcpy(t, m, L); for (int k = 0; k < w; k++) { uint64_t p = splitmix(&g) % (8 * L); t[p / 8] ^= (uint8_t)(1u << (p % 8)); }
            snprintf(d, sizeof d, "random weight-%d #%d", w, i); add_cand(t, L, 1, d);
        }
        /* cross-length neighbours */
        memcpy(t, m, L); t[L] = 0x00; snprintf(d, sizeof d, "len+1 append 00"); add_cand(t, L + 1, 1, d);
        memcpy(t, m, L); t[L] = 0xff; snprintf(d, sizeof d, "len+1 append ff"); add_cand(t, L + 1, 1, d);
        memcpy(t, m, L); t[L] = m[L - 1]; snprintf(d, sizeof d, "len+1 repeat last"); add_cand(t, L + 1, 1, d);
        if (L >= 2) { memcpy(t, m, L - 1); snprintf(d, sizeof d, "len-1 truncate"); add_cand(t, L - 1, 1, d); }
        if (L >= 2) { memcpy(t + 1, m, L - 1); t[0] = 0x00; snprintf(d, sizeof d, "len+1 prepend 00"); add_cand(t, L + 1, 1, d); }
    }
}
typedef struct { int mode, tid; uint64_t n, n_cheap, salt; const uint8_t *m; size_t len; uint64_t *hits; } scan_job;
static void *scan_thread(void *arg) {
    scan_job *j = (scan_job *)arg;
    rng_t r; rng_seed(&r, j->salt, (uint64_t)j->tid);
    key_t_ k;
    for (uint64_t i = 0; i < j->n; i++) {
        draw_key(&r, j->mode, &k);
        uint64_t a = H(j->mode, &k, j->m, j->len);
        int cheap_on = i < j->n_cheap;
        for (int c = 0; c < ncand; c++) {
            if (cands[c].cheap && !cheap_on) continue;
            if (H(j->mode, &k, cands[c].m2, cands[c].len2) == a) j->hits[c]++;
        }
    }
    return NULL;
}
static int cmd_scan(int argc, char **argv) {
    if (argc < 8) { fprintf(stderr, "scan MODE LOG2N SALT THREADS LMIN LMAX [LOG2N_CHEAP]\n"); return 2; }
    int mode = parse_mode(argv[2]); int log2n = atoi(argv[3]); uint64_t salt = strtoull(argv[4], 0, 0); int T = atoi(argv[5]);
    size_t lmin = (size_t)atoi(argv[6]), lmax = (size_t)atoi(argv[7]); int log2c = argc > 8 ? atoi(argv[8]) : log2n;
    printf("scan mode=%s keys=2^%d (cheap candidates 2^%d) salt=%llu threads=%d lengths %zu..%zu\n", argv[2], log2n, log2c, (unsigned long long)salt, T, lmin, lmax);
    printf("base message m(L): bytes from SplitMix64(0x2026091900000000 + L); candidates m' listed per length; hits over keys drawn per length\n");
    for (size_t L = lmin; L <= lmax; L++) {
        static uint8_t m[MAXLEN]; uint64_t g = 0x2026091900000000ull + L; for (size_t i = 0; i < L; i++) m[i] = (uint8_t)splitmix(&g);
        ncand = 0; build_cands(m, L);
        uint64_t N = 1ull << log2n, Nc = 1ull << log2c;
        scan_job *jobs = calloc((size_t)T, sizeof *jobs); pthread_t *th = calloc((size_t)T, sizeof *th);
        for (int t = 0; t < T; t++) {
            jobs[t] = (scan_job){ .mode = mode, .tid = t, .n = N / (uint64_t)T, .n_cheap = Nc / (uint64_t)T, .salt = salt * 1000003ull + L, .m = m, .len = L, .hits = calloc((size_t)ncand, 8) };
            pthread_create(&th[t], 0, scan_thread, &jobs[t]);
        }
        for (int t = 0; t < T; t++) pthread_join(th[t], 0);
        printf("\n== L=%zu m=", L); print_hex(m, L); printf("  candidates=%d\n", ncand);
        for (int c = 0; c < ncand; c++) {
            uint64_t h = 0; for (int t = 0; t < T; t++) h += jobs[t].hits[c];
            uint64_t n = cands[c].cheap ? Nc : N;
            if (h) printf("L=%zu %-28s len'=%zu hits=%llu/2^%d rate=2^%.2f  m'=", L, cands[c].desc, cands[c].len2, (unsigned long long)h, cands[c].cheap ? log2c : log2n, __builtin_log2((double)h / (double)n));
            else   printf("L=%zu %-28s len'=%zu hits=0/2^%d", L, cands[c].desc, cands[c].len2, cands[c].cheap ? log2c : log2n);
            if (h) print_hex(cands[c].m2, cands[c].len2);
            printf("\n");
        }
        for (int t = 0; t < T; t++) free(jobs[t].hits); free(jobs); free(th);
        fflush(stdout);
    }
    return 0;
}

/* ---------------- fold differential ---------------- */
static inline uint64_t fold64(uint64_t a, uint64_t b) { __uint128_t p = (__uint128_t)a * b; return (uint64_t)p ^ (uint64_t)(p >> 64); }
typedef struct { int tid; uint64_t n, salt, d, e, hits; } fold_job;
static void *fold_thread(void *arg) {
    fold_job *j = (fold_job *)arg; rng_t r; rng_seed(&r, j->salt, (uint64_t)j->tid); uint64_t h = 0;
    for (uint64_t i = 0; i < j->n; i++) { uint64_t x = rng_next(&r), y = rng_next(&r); h += fold64(x, y) == fold64(x ^ j->d, y ^ j->e); }
    j->hits = h; return NULL;
}
static uint64_t fold_rate(uint64_t d, uint64_t e, int log2n, uint64_t salt, int T) {
    fold_job *jobs = calloc((size_t)T, sizeof *jobs); pthread_t *th = calloc((size_t)T, sizeof *th); uint64_t N = 1ull << log2n;
    for (int t = 0; t < T; t++) { jobs[t] = (fold_job){ .tid = t, .n = N / (uint64_t)T, .salt = salt, .d = d, .e = e }; pthread_create(&th[t], 0, fold_thread, &jobs[t]); }
    uint64_t h = 0; for (int t = 0; t < T; t++) { pthread_join(th[t], 0); h += jobs[t].hits; }
    free(jobs); free(th); return h;
}
static void report_fold(uint64_t d, uint64_t e, uint64_t h, int log2n, const char *tag) {
    if (h) printf("fold d=%016llx e=%016llx hits=%llu/2^%d rate=2^%.3f %s\n", (unsigned long long)d, (unsigned long long)e, (unsigned long long)h, log2n, __builtin_log2((double)h / (double)(1ull << log2n)), tag);
    else printf("fold d=%016llx e=%016llx hits=0/2^%d %s\n", (unsigned long long)d, (unsigned long long)e, log2n, tag);
    fflush(stdout);
}
static int cmd_fold(int argc, char **argv) {
    if (argc < 7) { fprintf(stderr, "fold LOG2N SALT THREADS D E [D E ...]\n"); return 2; }
    int log2n = atoi(argv[2]); uint64_t salt = strtoull(argv[3], 0, 0); int T = atoi(argv[4]);
    for (int i = 5; i + 1 < argc; i += 2) { uint64_t d = strtoull(argv[i], 0, 16), e = strtoull(argv[i + 1], 0, 16); report_fold(d, e, fold_rate(d, e, log2n, salt, T), log2n, ""); }
    return 0;
}
static int cmd_foldclimb(int argc, char **argv) {
    if (argc < 6) { fprintf(stderr, "foldclimb LOG2N SALT THREADS ROUNDS\n"); return 2; }
    int log2n = atoi(argv[2]); uint64_t salt = strtoull(argv[3], 0, 0); int T = atoi(argv[4]); int rounds = atoi(argv[5]);
    const uint64_t M = ~0ull;
    uint64_t seeds[][2] = { { M, M }, { M, 0 }, { M, M - 2 }, { M - 2, M - 2 }, { M, 0x5555555555555555ull }, { 0xaaaaaaaaaaaaaaaaull, 0xaaaaaaaaaaaaaaaaull }, { M, 0x8000000000000000ull }, { M, 1 } };
    int nseeds = (int)(sizeof seeds / sizeof seeds[0]);
    printf("foldclimb screening=2^%d salt=%llu threads=%d rounds=%d\n", log2n, (unsigned long long)salt, T, rounds);
    uint64_t bestd = M, beste = M, besth = 0; uint64_t iter = 0;
    for (int s = 0; s < nseeds; s++) { uint64_t h = fold_rate(seeds[s][0], seeds[s][1], log2n, salt + iter++, T); report_fold(seeds[s][0], seeds[s][1], h, log2n, "seed"); if (h > besth) { besth = h; bestd = seeds[s][0]; beste = seeds[s][1]; } }
    for (int rnd = 0; rnd < rounds; rnd++) {
        uint64_t nd = bestd, ne = beste, nh = besth; int improved = 0;
        for (int bit = 0; bit < 128; bit++) {
            uint64_t d = bestd ^ (bit < 64 ? 1ull << bit : 0), e = beste ^ (bit >= 64 ? 1ull << (bit - 64) : 0);
            uint64_t h = fold_rate(d, e, log2n, salt + iter++, T);
            char tag[64]; snprintf(tag, sizeof tag, "round %d flip %d", rnd, bit); report_fold(d, e, h, log2n, tag);
            if (h > nh) { nh = h; nd = d; ne = e; improved = 1; }
        }
        if (!improved) { printf("round %d: no single-bit neighbour beats the incumbent (%llu hits)\n", rnd, (unsigned long long)besth); break; }
        /* re-measure the challenger and the incumbent on fresh samples before accepting (avoid winner's curse) */
        uint64_t h1 = fold_rate(nd, ne, log2n + 2, salt + iter++, T), h0 = fold_rate(bestd, beste, log2n + 2, salt + iter++, T);
        report_fold(nd, ne, h1, log2n + 2, "challenger re-measured"); report_fold(bestd, beste, h0, log2n + 2, "incumbent re-measured");
        if (h1 > h0 + 3 * __builtin_sqrt((double)(h0 + h1))) { bestd = nd; beste = ne; besth = nh; printf("round %d: accept challenger\n", rnd); }
        else { printf("round %d: challenger not significantly better (z<3); stop\n", rnd); break; }
    }
    printf("best d=%016llx e=%016llx\n", (unsigned long long)bestd, (unsigned long long)beste);
    return 0;
}

int main(int argc, char **argv) {
    if (argc < 2) { fprintf(stderr, "usage: pair|scan|fold|foldclimb ...\n"); return 2; }
    if (!strcmp(argv[1], "pair")) return cmd_pair(argc, argv);
    if (!strcmp(argv[1], "scan")) return cmd_scan(argc, argv);
    if (!strcmp(argv[1], "fold")) return cmd_fold(argc, argv);
    if (!strcmp(argv[1], "foldclimb")) return cmd_foldclimb(argc, argv);
    fprintf(stderr, "unknown command\n"); return 2;
}
