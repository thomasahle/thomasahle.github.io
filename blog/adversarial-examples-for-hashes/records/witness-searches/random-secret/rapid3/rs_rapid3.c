/* rs_rapid3.c -- rapidhash v3 fixed-pair collision rates under the RANDOM-SECRET key model.
 *
 * Key model: the 64-bit seed AND all eight 64-bit secret words (the `secret` argument of
 * upstream rapidhash_internal / rapidhashMicro_internal / rapidhashNano_internal) are
 * independent uniform hidden key material (576 bits).  The hash body is the SMHasher3-
 * validated port in rapid3_port.h (verification value 0x1FDC65EE for rapidhash v3).
 *
 * Build:  gcc -O3 -march=native -fopenmp -o rs_rapid3 rs_rapid3.c -lm
 * Modes:
 *   pair    L2N RNGSEED HEX_M HEX_M2 [THREADS] [default]   rate of one fixed pair over 2^L2N keys
 *   sweep   L2N_SHORT L2N_MID L2N_LONG RNGSEED [THREADS]   structured candidates, lengths 1..64 + block boundaries
 *   fold10                                                 exhaustive XOR differentials of the 10-bit fold
 *   climb   NBITS L2S RNGSEED ITERS [THREADS]              hill-climb XOR differentials of the n-bit fold
 *   foldrate NBITS DA DB L2S RNGSEED [THREADS]             P[fold(u,q)=fold(u^DA,q^DB)] over uniform u,q
 *   sweeppair LEN WORD L2N RNGSEED [THREADS]              one sweep candidate (word WORD complemented) at 2^L2N keys
 * Every run prints the counts it saw; probabilities are counts / trials.  No path or host
 * information is embedded.  RNG: xoshiro256** seeded from splitmix64(RNGSEED, thread).
 */
#include "rapid3_port.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h>

typedef unsigned __int128 u128;
static inline uint64_t rotl64(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
typedef struct { uint64_t s[4]; } Rng;
static uint64_t splitmix64(uint64_t *x) {
    uint64_t z = (*x += 0x9E3779B97F4A7C15ull);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBull;
    return z ^ (z >> 31);
}
static void rng_init(Rng *r, uint64_t rngseed, uint64_t stream) {
    uint64_t x = rngseed ^ (stream * 0xD1B54A32D192ED03ull);
    for (int i = 0; i < 4; i++) r->s[i] = splitmix64(&x);
}
static inline uint64_t rng_next(Rng *r) {
    uint64_t *s = r->s;
    uint64_t result = rotl64(s[1] * 5, 7) * 9;
    uint64_t t = s[1] << 17;
    s[2] ^= s[0]; s[3] ^= s[1]; s[1] ^= s[2]; s[0] ^= s[3]; s[2] ^= t; s[3] = rotl64(s[3], 45);
    return result;
}

typedef struct { uint8_t b[512]; size_t len; } Msg;
typedef struct { uint64_t count, trials; int have_ex; uint64_t ex_seed, ex_sec[8], ex_out; } Res;

/* rate of a fixed pair; use_default=1 keeps the shipped secret (seed-only model) */
static Res pair_rate(const Msg *a, const Msg *b, uint64_t N, uint64_t rngseed, int threads, int use_default) {
    Res R; memset(&R, 0, sizeof R); R.trials = N;
    #pragma omp parallel num_threads(threads)
    {
        int t = omp_get_thread_num(), T = omp_get_num_threads();
        Rng r; rng_init(&r, rngseed, (uint64_t)t);
        uint64_t lo = N * (uint64_t)t / T, hi = N * (uint64_t)(t + 1) / T, c = 0;
        int he = 0; uint64_t es = 0, esec[8] = {0}, eo = 0;
        uint64_t sec[8]; memcpy(sec, rapid_secret, sizeof sec);
        for (uint64_t i = lo; i < hi; i++) {
            uint64_t seed = rng_next(&r);
            if (!use_default) for (int j = 0; j < 8; j++) sec[j] = rng_next(&r);
            uint64_t h1 = rapidhash(a->b, a->len, seed, sec);
            uint64_t h2 = rapidhash(b->b, b->len, seed, sec);
            if (h1 == h2) { c++; if (!he) { he = 1; es = seed; memcpy(esec, sec, sizeof esec); eo = h1; } }
        }
        #pragma omp critical
        { R.count += c; if (he && !R.have_ex) { R.have_ex = 1; R.ex_seed = es; memcpy(R.ex_sec, esec, sizeof esec); R.ex_out = eo; } }
    }
    return R;
}

static void print_res(const char *label, const Msg *a, const Msg *b, Res R) {
    size_t L = ((a->len > b->len ? a->len : b->len) + 7) / 8; if (L == 0) L = 1;
    printf("%-28s len %3zu/%3zu L=%zu : %llu / %llu", label, a->len, b->len, L,
           (unsigned long long)R.count, (unsigned long long)R.trials);
    if (R.count) {
        double l2 = log2((double)R.count / (double)R.trials);
        printf("  rate 2^%.3f  cap %.3f bits", l2, log2((double)L) - l2);
    } else printf("  rate < 2^%.1f (floor)  cap > %.2f bits", -log2((double)R.trials), log2((double)L) + log2((double)R.trials));
    printf("\n");
    if (R.have_ex) {
        printf("    example key: seed %016llx secret", (unsigned long long)R.ex_seed);
        for (int j = 0; j < 8; j++) printf(" %016llx", (unsigned long long)R.ex_sec[j]);
        printf(" -> %016llx\n", (unsigned long long)R.ex_out);
    }
    fflush(stdout);
}

static size_t hex2msg(const char *s, Msg *m) {
    size_t n = strlen(s) / 2; memset(m, 0, sizeof *m); m->len = n;
    for (size_t i = 0; i < n; i++) { unsigned v; sscanf(s + 2 * i, "%2x", &v); m->b[i] = (uint8_t)v; }
    return n;
}
static void msg_hex(const Msg *m, char *out) { for (size_t i = 0; i < m->len; i++) sprintf(out + 2 * i, "%02x", m->b[i]); out[2 * m->len] = 0; }

static Msg rnd_msg(size_t len, uint64_t salt) {
    Msg m; memset(&m, 0, sizeof m); m.len = len; Rng r; rng_init(&r, 7, salt);
    for (size_t i = 0; i < len; i++) m.b[i] = (uint8_t)rng_next(&r);
    return m;
}
static void cword(Msg *m, int k) { for (size_t i = 8 * (size_t)k; i < 8 * (size_t)k + 8 && i < m->len; i++) m->b[i] ^= 0xff; }

/* ---- fold primitive ---- */
static inline uint64_t foldn(uint64_t u, uint64_t q, int n) {
    u128 r = (u128)u * q; uint64_t mask = (n == 64) ? ~0ull : ((1ull << n) - 1);
    return (((uint64_t)r) ^ ((uint64_t)(r >> n))) & mask;
}
static uint64_t fold_count(int n, uint64_t dA, uint64_t dB, uint64_t S, uint64_t rngseed, int threads) {
    uint64_t mask = (n == 64) ? ~0ull : ((1ull << n) - 1); uint64_t total = 0;
    #pragma omp parallel num_threads(threads) reduction(+:total)
    {
        int t = omp_get_thread_num(), T = omp_get_num_threads();
        Rng r; rng_init(&r, rngseed, 1000 + (uint64_t)t);
        uint64_t lo = S * (uint64_t)t / T, hi = S * (uint64_t)(t + 1) / T, c = 0;
        for (uint64_t i = lo; i < hi; i++) {
            uint64_t u = rng_next(&r) & mask, q = rng_next(&r) & mask;
            c += foldn(u, q, n) == foldn(u ^ dA, q ^ dB, n);
        }
        total += c;
    }
    return total;
}

typedef struct { uint32_t d, e; uint32_t cnt; } Cand;
static int cand_cmp(const void *x, const void *y) { const Cand *a = x, *b = y; return (a->cnt < b->cnt) - (a->cnt > b->cnt); }

int main(int argc, char **argv) {
    if (argc < 2) { fprintf(stderr, "usage: see header\n"); return 2; }
    const char *mode = argv[1];
    if (!strcmp(mode, "pair")) {
        if (argc < 6) return 2;
        int l2n = atoi(argv[2]); uint64_t rngseed = strtoull(argv[3], 0, 0);
        Msg a, b; hex2msg(argv[4], &a); hex2msg(argv[5], &b);
        int threads = argc > 6 ? atoi(argv[6]) : 8; int def = argc > 7 && !strcmp(argv[7], "default");
        printf("model: %s; rngseed %llu; 2^%d keys; threads %d\n", def ? "seed uniform, DEFAULT secret" : "seed + 8 secret words uniform", (unsigned long long)rngseed, l2n, threads);
        Res R = pair_rate(&a, &b, 1ull << l2n, rngseed, threads, def);
        print_res("pair", &a, &b, R);
        return 0;
    }
    if (!strcmp(mode, "foldrate")) {
        int n = atoi(argv[2]); uint64_t dA = strtoull(argv[3], 0, 16), dB = strtoull(argv[4], 0, 16);
        int l2s = atoi(argv[5]); uint64_t rngseed = strtoull(argv[6], 0, 0); int threads = argc > 7 ? atoi(argv[7]) : 8;
        uint64_t c = fold_count(n, dA, dB, 1ull << l2s, rngseed, threads);
        printf("fold n=%d dA=%016llx dB=%016llx : %llu / 2^%d", n, (unsigned long long)dA, (unsigned long long)dB, (unsigned long long)c, l2s);
        if (c) printf("  P = 2^%.3f", log2((double)c) - l2s); printf("\n");
        return 0;
    }
    if (!strcmp(mode, "fold10")) {
        const int n = 10; const uint32_t M = (1u << n) - 1; const uint32_t ND = 1u << (2 * n);
        Cand *all = malloc(sizeof(Cand) * ND);
        #pragma omp parallel for schedule(dynamic, 64)
        for (uint32_t de = 1; de < ND; de++) {
            uint32_t d = de >> n, e = de & M; uint32_t cnt = 0;
            for (uint32_t u = 0; u <= M; u++) for (uint32_t q = 0; q <= M; q++)
                cnt += foldn(u, q, n) == foldn(u ^ d, q ^ e, n);
            all[de].d = d; all[de].e = e; all[de].cnt = cnt;
        }
        all[0].d = all[0].e = 0; all[0].cnt = 0;
        qsort(all, ND, sizeof(Cand), cand_cmp);
        printf("exhaustive n=10 XOR differentials of the xor-fold; random-function reference 2^-10 = %u / 2^20\n", 1u << 10);
        for (int i = 0; i < 60; i++) printf("%3d  d=%03x e=%03x  cnt=%7u  P=2^%.2f\n", i + 1, all[i].d, all[i].e, all[i].cnt, log2((double)all[i].cnt) - 20);
        free(all); return 0;
    }
    if (!strcmp(mode, "climb")) {
        int n = atoi(argv[2]); int l2s = atoi(argv[3]); uint64_t rngseed = strtoull(argv[4], 0, 0);
        int iters = atoi(argv[5]); int threads = argc > 6 ? atoi(argv[6]) : 8;
        uint64_t mask = (n == 64) ? ~0ull : ((1ull << n) - 1); uint64_t S = 1ull << l2s;
        uint64_t starts[][2] = { {mask, mask}, {mask, 0}, {0, mask}, {mask, mask ^ 1}, {mask ^ 1, mask ^ 1}, {mask >> 1, mask >> 1},
                                 {0x5555555555555555ull & mask, 0x5555555555555555ull & mask}, {0xAAAAAAAAAAAAAAAAull & mask, 0xAAAAAAAAAAAAAAAAull & mask} };
        int ns = sizeof starts / sizeof starts[0];
        Rng r; rng_init(&r, rngseed, 77);
        for (int s = 0; s < ns + 4; s++) {
            uint64_t dA, dB;
            if (s < ns) { dA = starts[s][0]; dB = starts[s][1]; } else { dA = rng_next(&r) & mask; dB = rng_next(&r) & mask; }
            uint64_t best = fold_count(n, dA, dB, S, rngseed + 1, threads);
            printf("start %d: dA=%0*llx dB=%0*llx cnt=%llu P=2^%.3f\n", s, n / 4, (unsigned long long)dA, n / 4, (unsigned long long)dB, (unsigned long long)best, best ? log2((double)best) - l2s : -99.0);
            for (int it = 0; it < iters; it++) {
                int improved = 0;
                for (int bit = 0; bit < 2 * n; bit++) {
                    uint64_t nA = dA, nB = dB;
                    if (bit < n) nA ^= 1ull << bit; else nB ^= 1ull << (bit - n);
                    uint64_t c = fold_count(n, nA, nB, S, rngseed + 2 + it, threads);
                    /* accept only a >3-sigma improvement (Poisson) to avoid chasing noise */
                    if (c > best + 3.0 * sqrt((double)(best + 1)) + 1) {
                        uint64_t c2 = fold_count(n, nA, nB, S, rngseed + 999 + it, threads);
                        if (c2 > best + 2.0 * sqrt((double)(best + 1))) { dA = nA; dB = nB; best = c2; improved = 1;
                            printf("  it %d: move -> dA=%0*llx dB=%0*llx cnt=%llu P=2^%.3f\n", it, n / 4, (unsigned long long)dA, n / 4, (unsigned long long)dB, (unsigned long long)best, log2((double)best) - l2s); fflush(stdout); }
                    }
                }
                if (!improved) { printf("  local optimum after %d sweeps\n", it + 1); break; }
            }
            fflush(stdout);
        }
        return 0;
    }
    if (!strcmp(mode, "sweep")) {
        /* sweep L2N_SHORT L2N_MID L2N_LONG RNGSEED [THREADS]: word complements (single and adjacent
         * pairs) at every length; lengths <= 17 also get bit flips, appended bytes and same-byte
         * adjacent-length pairs; 112/113 and 224/225 get appended bytes.  Keys per candidate by tier. */
        int l2s = atoi(argv[2]), l2m = atoi(argv[3]), l2l = atoi(argv[4]); uint64_t rngseed = strtoull(argv[5], 0, 0); int threads = argc > 6 ? atoi(argv[6]) : 8;
        printf("sweep: random seed + 8 random secret words per key; keys per candidate: 2^%d for len<=40, 2^%d for 41..64, 2^%d beyond; rngseed %llu\n", l2s, l2m, l2l, (unsigned long long)rngseed);
        int lens[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,80,96,112,113,128,224,225,240,336,337};
        int nl = sizeof lens / sizeof lens[0];
        char lab[64];
        for (int li = 0; li < nl; li++) {
            int len = lens[li]; int W = (len + 7) / 8; uint64_t N = 1ull << (len <= 40 ? l2s : (len <= 64 ? l2m : l2l));
            Msg base = rnd_msg(len, (uint64_t)len);
            for (int k = 0; k < W; k++) { Msg b = base; cword(&b, k); snprintf(lab, sizeof lab, "cw%d", k); print_res(lab, &base, &b, pair_rate(&base, &b, N, rngseed, threads, 0)); }
            for (int k = 0; k + 1 < W; k++) { Msg b = base; cword(&b, k); cword(&b, k + 1); snprintf(lab, sizeof lab, "cw%d+%d", k, k + 1); print_res(lab, &base, &b, pair_rate(&base, &b, N, rngseed, threads, 0)); }
            if (len <= 17) {
                { Msg b = base; b.b[0] ^= 1; print_res("bit0", &base, &b, pair_rate(&base, &b, N, rngseed, threads, 0)); }
                { Msg b = base; b.b[len - 1] ^= 0x80; print_res("lastbit", &base, &b, pair_rate(&base, &b, N, rngseed, threads, 0)); }
            }
            if (len <= 17 || len == 112 || len == 224) {
                Msg b = base; b.len = len + 1; b.b[len] = 0; print_res("append00", &base, &b, pair_rate(&base, &b, N, rngseed, threads, 0));
                b.b[len] = base.b[len - 1]; print_res("appenddup", &base, &b, pair_rate(&base, &b, N, rngseed, threads, 0));
            }
            if (len <= 17) { /* same-byte messages of adjacent lengths: identical (a,b) operands on the short path */
                Msg s1, s2; memset(&s1, 0x5a, sizeof s1); memset(&s2, 0x5a, sizeof s2); s1.len = len; s2.len = len + 1;
                print_res("same5a/len+1", &s1, &s2, pair_rate(&s1, &s2, N, rngseed, threads, 0));
                memset(&s1, 0, sizeof s1); memset(&s2, 0, sizeof s2); s1.len = len; s2.len = len + 1;
                print_res("zero/len+1", &s1, &s2, pair_rate(&s1, &s2, N, rngseed, threads, 0));
            }
        }
        return 0;
    }
    if (!strcmp(mode, "sweeppair")) { /* sweeppair LEN WORD L2N RNGSEED [THREADS]: the sweep's base message of LEN bytes with word WORD complemented */
        int len = atoi(argv[2]), k = atoi(argv[3]), l2n = atoi(argv[4]); uint64_t rngseed = strtoull(argv[5], 0, 0); int threads = argc > 6 ? atoi(argv[6]) : 8;
        Msg base = rnd_msg((size_t)len, (uint64_t)len), b = base; cword(&b, k);
        char h1[1100], h2[1100]; msg_hex(&base, h1); msg_hex(&b, h2);
        printf("model: seed + 8 secret words uniform; rngseed %llu; 2^%d keys\nm  = %s\nm' = %s\n", (unsigned long long)rngseed, l2n, h1, h2);
        char lab[32]; snprintf(lab, sizeof lab, "cw%d", k);
        print_res(lab, &base, &b, pair_rate(&base, &b, 1ull << l2n, rngseed, threads, 0));
        return 0;
    }
    fprintf(stderr, "unknown mode\n"); return 2;
}
