/*
 * verify_rs128.c -- independent random-secret sampler for XXH3-128 (xxHash v0.8.3).
 *
 * Fresh implementation for the verification lane: it does NOT share code with the
 * searcher's rs128.c. The upstream header xxhash.h (tag v0.8.3, fetched from GitHub,
 * sha256 17973c0d...) is included unmodified with XXH_INLINE_ALL.
 *
 * Key model: every trial draws a fresh uniform 192-byte secret (24 uniform 64-bit
 * words, little-endian) and, in mode secretseed, an independent uniform 64-bit seed.
 *   mode secret      : XXH3_128bits_withSecret(msg, len, secret, 192)          (public API, seed 0)
 *   mode secretseed  : XXH3_128bits_internal(msg, len, seed, secret, 192,
 *                                            XXH3_hashLong_128b_withSecret)    (static; seed AND secret)
 *   mode seed        : XXH3_128bits_withSeed(msg, len, seed)                   (default kSecret; control)
 *   mode apiss       : XXH3_128bits_withSecretandSeed(msg, len, secret, 192, seed)
 * A hit = full 128-bit equality (low64 AND high64) of H(m) and H(m') under the same key.
 *
 * RNG: 128-bit Lehmer MCG (multiplier 0xda942042e4dd58b5, high 64 bits out), one stream per
 * thread, state derived by splitmix64 from (rngseed, thread). Different family from the
 * xoshiro256** streams used by the searcher and by the blog's verify packages.
 *
 * usage: verify_rs128 selftest
 *        verify_rs128 <mode> <log2N> <rngseed> <threads> <pairs.txt>
 * pairs.txt lines: <len_bytes> <m_hex> <m'_hex> <name>   ('#' comments)
 */
#define XXH_INLINE_ALL
#include "xxhash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#define MAXPAIRS 64
#define MAXLEN 1024
#define SECRET_BYTES 192
#define SECRET_WORDS (SECRET_BYTES / 8)

typedef struct { int len; char name[64]; unsigned char m[MAXLEN], m2[MAXLEN]; } pair_t;
static pair_t pairs[MAXPAIRS]; static int npairs = 0;

typedef struct { unsigned __int128 s; } rng_t;
static inline uint64_t rng_next(rng_t *r) { r->s *= 0xda942042e4dd58b5ULL; return (uint64_t)(r->s >> 64); }
static uint64_t splitmix64(uint64_t *x) {
    uint64_t z = (*x += 0x9e3779b97f4a7c15ULL);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    return z ^ (z >> 31);
}
static void rng_seed(rng_t *r, uint64_t seed, uint64_t stream) {
    uint64_t x = seed * 0x2545F4914F6CDD1DULL + stream * 0x9E3779B97F4A7C15ULL + 0x1234567ULL;
    uint64_t a = splitmix64(&x), b = splitmix64(&x);
    r->s = ((unsigned __int128)a << 64) | (b | 1ULL);
    for (int i = 0; i < 8; i++) rng_next(r);
}

enum { M_SECRET, M_SECRETSEED, M_SEED, M_APISS };
static int mode = M_SECRET;

static inline XXH128_hash_t H(const unsigned char *m, int len, const unsigned char *secret, uint64_t seed) {
    switch (mode) {
    case M_SECRET:     return XXH3_128bits_withSecret(m, len, secret, SECRET_BYTES);
    case M_SECRETSEED: return XXH3_128bits_internal(m, len, seed, secret, SECRET_BYTES, XXH3_hashLong_128b_withSecret);
    case M_SEED:       return XXH3_128bits_withSeed(m, len, seed);
    default:           return XXH3_128bits_withSecretandSeed(m, len, secret, SECRET_BYTES, seed);
    }
}

typedef struct {
    int tid, nthreads; uint64_t ntrials, rngseed;
    volatile uint64_t done;
    uint64_t hits[MAXPAIRS];
    int have_witness[MAXPAIRS]; uint64_t wit_seed[MAXPAIRS], wit_trial[MAXPAIRS];
    unsigned char wit_secret[MAXPAIRS][SECRET_BYTES]; XXH128_hash_t wit_h[MAXPAIRS];
} worker_t;

static void *worker(void *arg) {
    worker_t *w = (worker_t *)arg;
    rng_t r; rng_seed(&r, w->rngseed, (uint64_t)w->tid);
    unsigned char secret[SECRET_BYTES];
    for (uint64_t t = 0; t < w->ntrials; t++) {
        for (int i = 0; i < SECRET_WORDS; i++) { uint64_t v = rng_next(&r); for (int b = 0; b < 8; b++) secret[8*i+b] = (unsigned char)(v >> (8*b)); }
        uint64_t seed = rng_next(&r);
        for (int p = 0; p < npairs; p++) {
            XXH128_hash_t a = H(pairs[p].m, pairs[p].len, secret, seed);
            XXH128_hash_t b = H(pairs[p].m2, pairs[p].len, secret, seed);
            if (a.low64 == b.low64 && a.high64 == b.high64) {
                w->hits[p]++;
                if (!w->have_witness[p]) { w->have_witness[p] = 1; w->wit_seed[p] = seed; w->wit_trial[p] = t; memcpy(w->wit_secret[p], secret, SECRET_BYTES); w->wit_h[p] = a; }
            }
        }
        if ((t & 0xFFFFF) == 0xFFFFF) w->done = t + 1;
    }
    w->done = w->ntrials;
    return NULL;
}

static int hexval(char c) { if (c >= '0' && c <= '9') return c - '0'; c |= 32; if (c >= 'a' && c <= 'f') return c - 'a' + 10; return -1; }
static int parse_hex(const char *s, unsigned char *out, int len) {
    if ((int)strlen(s) != 2 * len) return 0;
    for (int i = 0; i < len; i++) { int h = hexval(s[2*i]), l = hexval(s[2*i+1]); if (h < 0 || l < 0) return 0; out[i] = (unsigned char)(h * 16 + l); }
    return 1;
}
static void load_pairs(const char *fn) {
    FILE *f = fopen(fn, "r"); if (!f) { perror(fn); exit(2); }
    char line[8192];
    while (fgets(line, sizeof line, f)) {
        if (line[0] == '#' || line[0] == '\n') continue;
        int len; char h1[4096], h2[4096], name[64];
        if (sscanf(line, "%d %4095s %4095s %63s", &len, h1, h2, name) != 4) { fprintf(stderr, "bad line: %s", line); exit(2); }
        if (len < 0 || len > MAXLEN || npairs >= MAXPAIRS) { fprintf(stderr, "len/pairs out of range\n"); exit(2); }
        pair_t *p = &pairs[npairs]; p->len = len; strncpy(p->name, name, 63); p->name[63] = 0;
        if (!parse_hex(h1, p->m, len) || !parse_hex(h2, p->m2, len)) { fprintf(stderr, "bad hex in %s\n", name); exit(2); }
        if (memcmp(p->m, p->m2, len) == 0) { fprintf(stderr, "pair %s: identical messages\n", name); exit(2); }
        npairs++;
    }
    fclose(f);
}

/* SMHasher3-style verification value (hashes/xxhash.cpp, XXH3-128 0.8.3 expects 0x288DAA94). */
static uint32_t smhasher3_verification(void) {
    unsigned char key[256], hashes[256 * 16];
    for (int i = 0; i < 256; i++) {
        key[i] = (unsigned char)i;
        XXH128_hash_t h = XXH3_128bits_withSeed(key, (size_t)i, (uint64_t)(256 - i));
        XXH128_canonical_t c; XXH128_canonicalFromHash(&c, h);
        memcpy(hashes + 16 * i, c.digest, 16);
    }
    XXH128_hash_t h = XXH3_128bits_withSeed(hashes, sizeof hashes, 0);
    XXH128_canonical_t c; XXH128_canonicalFromHash(&c, h);
    return (uint32_t)c.digest[0] | ((uint32_t)c.digest[1] << 8) | ((uint32_t)c.digest[2] << 16) | ((uint32_t)c.digest[3] << 24);
}

static int selftest(void) {
    int fails = 0;
    printf("XXH_versionNumber = %u (expect 803)\n", XXH_versionNumber());
    uint32_t v = smhasher3_verification();
    printf("SMHasher3 verification = %08X (expect 288DAA94) %s\n", v, v == 0x288DAA94u ? "PASS" : "FAIL");
    if (v != 0x288DAA94u) fails++;
    /* Row witness: seed e130d569418d2efe collides pair F with digest 77aee7ca6253510c ff9994fe21ab989a */
    {
        unsigned char m[32], m2[32];
        parse_hex("9bd4604137366abe642b9fbec8c9954188d35499de169df633e0964e8c04600c", m, 32);
        parse_hex("642b9fbec8c995419bd4604137366abe88d35499de169df633e0964e8c04600c", m2, 32);
        XXH128_hash_t a = XXH3_128bits_withSeed(m, 32, 0xe130d569418d2efeULL), b = XXH3_128bits_withSeed(m2, 32, 0xe130d569418d2efeULL);
        int ok = a.low64 == b.low64 && a.high64 == b.high64 && a.high64 == 0x77aee7ca6253510cULL && a.low64 == 0xff9994fe21ab989aULL;
        printf("row witness seed e130d569418d2efe: H(m)=%016llx%016llx H(m')=%016llx%016llx %s\n",
               (unsigned long long)a.high64, (unsigned long long)a.low64, (unsigned long long)b.high64, (unsigned long long)b.low64, ok ? "PASS" : "FAIL");
        if (!ok) fails++;
    }
    /* API dispatch claims: withSecretandSeed == withSeed for len<=240 (secret ignored) and == withSecret for len>240 (seed ignored). */
    {
        rng_t r; rng_seed(&r, 42, 0); unsigned char secret[SECRET_BYTES], msg[512]; int bad_short = 0, bad_long = 0, n = 0;
        for (int it = 0; it < 200; it++) {
            for (int i = 0; i < SECRET_BYTES; i++) secret[i] = (unsigned char)rng_next(&r);
            uint64_t seed = rng_next(&r);
            for (int len = 0; len <= 400; len += (len < 250 ? 1 : 7)) {
                for (int i = 0; i < len; i++) msg[i] = (unsigned char)rng_next(&r);
                XXH128_hash_t x = XXH3_128bits_withSecretandSeed(msg, len, secret, SECRET_BYTES, seed);
                XXH128_hash_t s = XXH3_128bits_withSeed(msg, len, seed);
                XXH128_hash_t c = XXH3_128bits_withSecret(msg, len, secret, SECRET_BYTES);
                XXH128_hash_t in = XXH3_128bits_internal(msg, len, seed, secret, SECRET_BYTES, XXH3_hashLong_128b_withSecret);
                n++;
                if (len <= 240) { if (!(x.low64 == s.low64 && x.high64 == s.high64)) bad_short++; }
                else { if (!(x.low64 == c.low64 && x.high64 == c.high64)) bad_long++; if (!(in.low64 == c.low64 && in.high64 == c.high64)) bad_long++; }
                if (len > 16 && len <= 240 && in.low64 == s.low64 && in.high64 == s.high64) bad_short++; /* internal(seed,secret) must use the custom secret */
            }
        }
        printf("API dispatch: withSecretandSeed==withSeed for len<=240 and ==withSecret (=internal) for len>240: %d checks, short mismatches %d, long mismatches %d %s\n",
               n, bad_short, bad_long, (bad_short == 0 && bad_long == 0) ? "PASS" : "FAIL");
        if (bad_short || bad_long) fails++;
    }
    /* Fold complement event: Pr[fold(A,B) == fold(~A,~B)] for uniform A,B; fold(a,b) = lo(ab)^hi(ab). 2^30 samples. */
    {
        rng_t r; rng_seed(&r, 7, 3); uint64_t N = 1ULL << 30, hits = 0, twin = 0;
        for (uint64_t i = 0; i < N; i++) {
            uint64_t a = rng_next(&r), b = rng_next(&r);
            unsigned __int128 p = (unsigned __int128)a * b, q = (unsigned __int128)(~a) * (~b);
            uint64_t f = (uint64_t)p ^ (uint64_t)(p >> 64), g = (uint64_t)q ^ (uint64_t)(q >> 64);
            if (f == g) hits++; else if ((f ^ g) == 0x8000000000000000ULL) twin++;
        }
        printf("fold complement event: %llu / 2^30 = 2^%.3f  (heuristic (3/4)^64 = 2^-26.56); exact-twin(2^63) %llu\n", (unsigned long long)hits, log2((double)hits / (double)N), (unsigned long long)twin);
    }
    return fails;
}

static double now(void) { struct timespec ts; clock_gettime(CLOCK_MONOTONIC, &ts); return ts.tv_sec + ts.tv_nsec * 1e-9; }

int main(int argc, char **argv) {
    setvbuf(stdout, NULL, _IOLBF, 0);
    if (argc == 2 && strcmp(argv[1], "selftest") == 0) return selftest() ? 1 : 0;
    if (argc != 6) { fprintf(stderr, "usage: %s selftest | <secret|secretseed|seed|apiss> <log2N> <rngseed> <threads> <pairs.txt>\n", argv[0]); return 2; }
    if (!strcmp(argv[1], "secret")) mode = M_SECRET; else if (!strcmp(argv[1], "secretseed")) mode = M_SECRETSEED;
    else if (!strcmp(argv[1], "seed")) mode = M_SEED; else if (!strcmp(argv[1], "apiss")) mode = M_APISS; else { fprintf(stderr, "bad mode\n"); return 2; }
    int log2n = atoi(argv[2]); uint64_t rngseed = strtoull(argv[3], NULL, 0); int T = atoi(argv[4]);
    if (log2n < 0 || log2n > 40 || T < 1 || T > 64) { fprintf(stderr, "bad args\n"); return 2; }
    load_pairs(argv[5]);
    uint32_t v = smhasher3_verification();
    printf("# verify_rs128 mode=%s log2N=%d rngseed=%llu threads=%d pairs=%s (%d pairs); xxhash %u; SMHasher3 verification %08X %s\n",
           argv[1], log2n, (unsigned long long)rngseed, T, argv[5], npairs, XXH_versionNumber(), v, v == 0x288DAA94u ? "PASS" : "FAIL");
    if (v != 0x288DAA94u) return 1;
    uint64_t N = 1ULL << log2n;
    worker_t *ws = calloc((size_t)T, sizeof *ws); pthread_t *th = calloc((size_t)T, sizeof *th);
    for (int t = 0; t < T; t++) { ws[t].tid = t; ws[t].nthreads = T; ws[t].rngseed = rngseed; ws[t].ntrials = N / T + (t < (int)(N % T) ? 1 : 0); pthread_create(&th[t], NULL, worker, &ws[t]); }
    double t0 = now();
    for (;;) {
        sleep(30);
        uint64_t done = 0, alldone = 1; for (int t = 0; t < T; t++) { done += ws[t].done; if (ws[t].done < ws[t].ntrials) alldone = 0; }
        uint64_t h0 = 0; for (int t = 0; t < T; t++) h0 += ws[t].hits[0];
        fprintf(stderr, "progress %.1f%% (%llu trials, %.0f s, pair0 hits %llu)\n", 100.0 * done / N, (unsigned long long)done, now() - t0, (unsigned long long)h0);
        if (alldone) break;
    }
    for (int t = 0; t < T; t++) pthread_join(th[t], NULL);
    double el = now() - t0;
    printf("# elapsed %.0f s, %.2f Mtrials/s\n", el, N / el / 1e6);
    for (int p = 0; p < npairs; p++) {
        uint64_t c = 0; for (int t = 0; t < T; t++) c += ws[t].hits[p];
        double rate = (double)c / (double)N;
        printf("pair %-32s len=%-4d count=%llu N=2^%d rate=%.4e log2rate=%s\n", pairs[p].name, pairs[p].len, (unsigned long long)c, log2n, rate, c ? ({ static char b[32]; snprintf(b, 32, "%.4f", log2(rate)); b; }) : "-inf");
        for (int t = 0; t < T; t++) if (ws[t].have_witness[p]) {
            printf("  witness thread=%d trial=%llu seed=%016llx digest=%016llx%016llx secret=", t, (unsigned long long)ws[t].wit_trial[p], (unsigned long long)ws[t].wit_seed[p], (unsigned long long)ws[t].wit_h[p].high64, (unsigned long long)ws[t].wit_h[p].low64);
            for (int i = 0; i < SECRET_BYTES; i++) printf("%02x", ws[t].wit_secret[p][i]);
            printf("\n"); break;
        }
    }
    return 0;
}
