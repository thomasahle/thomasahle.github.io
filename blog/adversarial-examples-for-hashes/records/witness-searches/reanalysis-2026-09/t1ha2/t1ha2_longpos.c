/*
 * t1ha2_longpos.c -- position independence of the t1ha2 top-bit loop trail
 * (3 consecutive 32-byte blocks) inside long messages with random content.
 *
 * Links the upstream t1ha sources unmodified (erthink/t1ha v2.1.4-10-g00eb779:
 * src/t1ha2.c, t1ha_bits.h, t1ha_selfcheck*.c); checks the upstream self-test
 * t1ha_selfcheck__t1ha2_atonce / _atonce128 and the SMHasher3 verification value
 * 0x8F16C948 of t1ha2_atonce; exit 1 on mismatch.
 *
 * Trail (units of 2^63 for the state differences), blocks b, b+1, b+2 of the loop
 * (block b at byte offset 32*b; the loop processes every full 32-byte block):
 *   block b  : M has w0 bit 56 = 0 and w1 bit 37 = 0.  M' = M with w0 ^= 2^56,
 *              w1 ^= 2^37, w3 -= 2^56.  d02 + w3 and c13 are unchanged provided
 *              y = w3 + c >= 2^56 (c = state entering block b); c and d gain 2^63.
 *   block b+1: w2 ^= 2^63, w3 ^= 2^63  -> state difference (1,1,1,1)
 *   block b+2: w2 ^= 2^63, w3 ^= 2^63  -> state difference 0.
 * Only condition: (w3 + c) mod 2^64 >= 2^56, i.e. 1 - 2^-8 for uniform c.
 *
 * Modes:
 *   rand  LEN N          fresh random message, random block b, random seed per trial
 *   fixed LEN NKEYS      one message/position, NKEYS seeds; prints the window
 *   cube  LEN R NKEYS    R disjoint windows (blocks 3q..3q+2) -> 2^R messages
 * Env: THREADS, RNGSEED.
 * Build: cc -std=c11 -O2 -pthread -Iupstream/t1ha t1ha2_longpos.c upstream/t1ha/src/t1ha2.c
 *        upstream/t1ha/src/t1ha2_selfcheck.c upstream/t1ha/src/t1ha_selfcheck.c -lm -o t1ha2_longpos
 */
#include <inttypes.h>
#include <math.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "t1ha.h"

int t1ha_selfcheck__t1ha2_atonce(void);
int t1ha_selfcheck__t1ha2_atonce128(void);

typedef struct { uint64_t s[4]; } rng_t;
static inline uint64_t rotl64(uint64_t x, int r) { return (x << r) | (x >> (64 - r)); }
static inline uint64_t rotr64(uint64_t x, int r) { return (x >> r) | (x << (64 - r)); }
static uint64_t splitmix64(uint64_t *s) { uint64_t z = (*s += UINT64_C(0x9e3779b97f4a7c15)); z = (z ^ (z >> 30)) * UINT64_C(0xbf58476d1ce4e5b9); z = (z ^ (z >> 27)) * UINT64_C(0x94d049bb133111eb); return z ^ (z >> 31); }
static void rng_init(rng_t *r, uint64_t seed) { for (int i = 0; i < 4; i++) r->s[i] = splitmix64(&seed); }
static uint64_t rng(rng_t *r) { uint64_t *s = r->s; const uint64_t res = rotl64(s[1] * 5, 7) * 9, t = s[1] << 17; s[2] ^= s[0]; s[3] ^= s[1]; s[1] ^= s[2]; s[0] ^= s[3]; s[2] ^= t; s[3] = rotl64(s[3], 45); return res; }
static void rng_fill(rng_t *r, uint8_t *p, size_t n) { size_t i = 0; for (; i + 8 <= n; i += 8) { uint64_t v = rng(r); memcpy(p + i, &v, 8); } if (i < n) { uint64_t v = rng(r); memcpy(p + i, &v, n - i); } }
static inline uint64_t G(const uint8_t *p, size_t o) { uint64_t v; memcpy(&v, p + o, 8); return v; }
static inline void S(uint8_t *p, size_t o, uint64_t v) { memcpy(p + o, &v, 8); }

static const uint64_t P5 = UINT64_C(0xC060724A8424F345), P6 = UINT64_C(0xCB5AF53AE3AAAC31);
static const uint64_t TOP = UINT64_C(1) << 63;

/* make M satisfy the base bit requirements for a window at block b */
static void prep_base(uint8_t *m, size_t b) {
    size_t o = 32 * b;
    S(m, o + 0, G(m, o + 0) & ~(UINT64_C(1) << 56));
    S(m, o + 8, G(m, o + 8) & ~(UINT64_C(1) << 37));
}
static void apply_window(uint8_t *m, size_t b) {
    size_t o = 32 * b;
    S(m, o + 0, G(m, o + 0) ^ (UINT64_C(1) << 56));
    S(m, o + 8, G(m, o + 8) ^ (UINT64_C(1) << 37));
    S(m, o + 24, G(m, o + 24) - (UINT64_C(1) << 56));
    for (int k = 1; k <= 2; k++) { S(m, o + 32 * k + 16, G(m, o + 32 * k + 16) ^ TOP); S(m, o + 32 * k + 24, G(m, o + 32 * k + 24) ^ TOP); }
}
/* own transcription of the loop state (for the model only): value of c entering block b */
static int predict_fail_many(const uint8_t *m, size_t len, uint64_t seed, const size_t *bs, int nb) {
    uint64_t a = seed, bb = len, c = rotr64(len, 23) + ~seed, d = ~(uint64_t)len + rotr64(seed, 19);
    size_t maxb = 0; for (int k = 0; k < nb; k++) if (bs[k] > maxb) maxb = bs[k];
    int fail = 0;
    for (size_t t = 0; t <= maxb; t++) {
        uint64_t w0 = G(m, 32 * t), w1 = G(m, 32 * t + 8), w2 = G(m, 32 * t + 16), w3 = G(m, 32 * t + 24);
        for (int k = 0; k < nb; k++) if (bs[k] == t) fail |= (uint64_t)(w3 + c) < (UINT64_C(1) << 56);
        uint64_t d02 = w0 + rotr64(w2 + d, 56), c13 = w1 + rotr64(w3 + c, 19);
        d ^= bb + rotr64(w1, 38); c ^= a + rotr64(w0, 57);
        bb ^= P6 * (c13 + w2); a ^= P5 * (d02 + w3);
    }
    return fail;
}

static void wilson(uint64_t k, uint64_t n, double *lo, double *hi) {
    double z = 1.959963984540054, p = (double)k / n, d = 1 + z * z / n, c = p + z * z / (2.0 * n);
    double s = z * sqrt(p * (1 - p) / n + z * z / (4.0 * n * n));
    *lo = (c - s) / d; *hi = (c + s) / d;
}

static int NT = 1; static uint64_t RNGSEED = UINT64_C(0x7431686132);
typedef struct { int tid, mode; size_t len; uint64_t n; const uint8_t *fm, *fm2; size_t fb;
                 uint64_t coll, coll128, pfail, mism, done, ex_seed, ex_h; int have_ex; } job_t;

static void *worker(void *arg) {
    job_t *J = arg; rng_t R; rng_init(&R, RNGSEED ^ ((uint64_t)(J->tid + 1) << 32) ^ J->len ^ ((uint64_t)J->mode << 60));
    size_t len = J->len, nblk = len / 32;
    uint8_t *m = malloc(len), *m2 = malloc(len);
    for (uint64_t i = 0; i < J->n; i++) {
        const uint8_t *a = m, *b = m2; size_t blk; uint64_t seed;
        if (J->mode == 0) {
            rng_fill(&R, m, len); blk = rng(&R) % (nblk - 2);
            prep_base(m, blk); memcpy(m2, m, len); apply_window(m2, blk); seed = rng(&R);
        } else { a = J->fm; b = J->fm2; blk = J->fb; seed = rng(&R); }
        uint64_t h1 = t1ha2_atonce(a, len, seed), h2 = t1ha2_atonce(b, len, seed);
        uint64_t x1, x2, l1 = t1ha2_atonce128(&x1, a, len, seed), l2 = t1ha2_atonce128(&x2, b, len, seed);
        int c = h1 == h2; int f = predict_fail_many(a, len, seed, &blk, 1);
        J->coll += c; J->coll128 += (l1 == l2 && x1 == x2); J->pfail += f; J->mism += (c == f); J->done++;
        if (c && !J->have_ex) { J->have_ex = 1; J->ex_seed = seed; J->ex_h = h1; }
    }
    free(m); free(m2); return NULL;
}
static void run(job_t *P, uint64_t n) {
    pthread_t th[256]; job_t J[256];
    for (int t = 0; t < NT; t++) { J[t] = *P; J[t].tid = t; J[t].n = n / NT + ((uint64_t)t < n % NT); pthread_create(&th[t], 0, worker, &J[t]); }
    for (int t = 0; t < NT; t++) { pthread_join(th[t], 0); P->coll += J[t].coll; P->coll128 += J[t].coll128; P->pfail += J[t].pfail; P->mism += J[t].mism; P->done += J[t].done;
        if (J[t].have_ex && !P->have_ex) { P->have_ex = 1; P->ex_seed = J[t].ex_seed; P->ex_h = J[t].ex_h; } }
}
static void report(const char *what, job_t *P) {
    double lo, hi; wilson(P->coll, P->done, &lo, &hi);
    uint64_t f = P->done - P->coll; double flo, fhi; wilson(f, P->done, &flo, &fhi);
    printf("%s len=%zu: t1ha2_atonce collisions %" PRIu64 "/%" PRIu64 " = %.6f [95%% %.6f, %.6f]; failures %" PRIu64 " = 2^%.3f [2^%.3f, 2^%.3f] (model 2^-8); atonce128 collisions %" PRIu64 "; model-predicted failures %" PRIu64 "; model/outcome disagreements %" PRIu64 "\n",
           what, P->len, P->coll, P->done, (double)P->coll / P->done, lo, hi, f, log2((double)f / P->done), log2(flo), log2(fhi), P->coll128, P->pfail, P->mism);
    if (P->have_ex) printf("  example colliding seed %016" PRIx64 " -> both %016" PRIx64 "\n", P->ex_seed, P->ex_h);
    fflush(stdout);
}

static uint32_t smhasher_verif(void) {
    uint8_t key[256], table[256 * 8];
    for (int i = 0; i < 256; i++) { key[i] = (uint8_t)i; uint64_t h = t1ha2_atonce(key, (size_t)i, (uint64_t)(256 - i)); memcpy(table + 8 * i, &h, 8); }
    uint64_t h = t1ha2_atonce(table, sizeof table, 0);
    return (uint32_t)h;
}

int main(int argc, char **argv) {
    if (getenv("THREADS")) NT = atoi(getenv("THREADS"));
    if (getenv("RNGSEED")) RNGSEED = strtoull(getenv("RNGSEED"), 0, 0);
    int s1 = t1ha_selfcheck__t1ha2_atonce(), s2 = t1ha_selfcheck__t1ha2_atonce128();
    uint32_t v = smhasher_verif();
    printf("upstream selfcheck atonce=%d atonce128=%d (0 = pass); SMHasher3 verification %08" PRIX32 " (expected 8F16C948)\n", s1, s2, v);
    if (s1 || s2 || v != 0x8F16C948u) return 1;
    if (argc < 3) return 0;
    job_t P; memset(&P, 0, sizeof P); P.len = strtoull(argv[2], 0, 0);
    if (!strcmp(argv[1], "rand")) { P.mode = 0; run(&P, strtoull(argv[3], 0, 0)); report("rand", &P); }
    else if (!strcmp(argv[1], "fixed") || !strcmp(argv[1], "cube")) {
        rng_t R; rng_init(&R, RNGSEED ^ 0xF1ED); size_t len = P.len, nblk = len / 32;
        uint8_t *m = malloc(len), *m2 = malloc(len); rng_fill(&R, m, len);
        if (!strcmp(argv[1], "fixed")) {
            size_t b = nblk / 2 + rng(&R) % (nblk / 2 - 2);
            prep_base(m, b); memcpy(m2, m, len); apply_window(m2, b);
            printf("fixed len=%zu: message = %zu bytes from xoshiro256** seeded by splitmix64(0x%" PRIx64 ") with bits 56 of w0 and 37 of w1 of block %zu cleared; window blocks %zu..%zu (byte offset %zu)\n",
                   len, len, RNGSEED ^ 0xF1ED, b, b, b + 2, 32 * b);
            for (size_t o = 32 * b; o < 32 * b + 96; o += 8) printf("  offset %7zu: %016" PRIx64 " -> %016" PRIx64 "%s\n", o, G(m, o), G(m2, o), G(m, o) != G(m2, o) ? "  *" : "");
            P.mode = 1; P.fm = m; P.fm2 = m2; P.fb = b; run(&P, strtoull(argv[3], 0, 0)); report("fixed", &P);
        } else {
            int r = atoi(argv[3]); uint64_t nk = strtoull(argv[4], 0, 0);
            size_t nwin = nblk / 3; size_t *bs = malloc(sizeof(size_t) * r);
            for (int k = 0; k < r; k++) { size_t q; int dup; do { q = rng(&R) % nwin; dup = 0; for (int j = 0; j < k; j++) dup |= bs[j] == 3 * q; } while (dup); bs[k] = 3 * q; prep_base(m, bs[k]); }
            printf("cube len=%zu r=%d windows at blocks:", len, r); for (int k = 0; k < r; k++) printf(" %zu", bs[k]); printf("\n");
            uint64_t M = UINT64_C(1) << r, ok = 0, pok = 0, mism = 0, minc = UINT64_MAX; uint64_t *hv = malloc(8 * M); uint8_t *msg = malloc(len);
            for (uint64_t key = 0; key < nk; key++) {
                uint64_t seed = rng(&R);
                for (uint64_t mb = 0; mb < M; mb++) { memcpy(msg, m, len); for (int k = 0; k < r; k++) if (mb >> k & 1) apply_window(msg, bs[k]); hv[mb] = t1ha2_atonce(msg, len, seed); }
                uint64_t eq = 0; for (uint64_t mb = 0; mb < M; mb++) eq += hv[mb] == hv[0];
                int o = eq == M; ok += o; int p = !predict_fail_many(m, len, seed, bs, r); pok += p; mism += o != p; if (eq < minc) minc = eq;
                if (key == 0) printf("  key %016" PRIx64 ": %" PRIu64 "/%" PRIu64 " members equal member 0 (%016" PRIx64 ")\n", seed, eq, M, hv[0]);
            }
            double lo, hi; wilson(ok, nk, &lo, &hi);
            printf("cube len=%zu r=%d: whole %" PRIu64 "-set collides for %" PRIu64 "/%" PRIu64 " keys = %.4f [95%% %.4f, %.4f] (model (1-2^-8)^r = %.4f); model predicts %" PRIu64 ", disagreements %" PRIu64 "; smallest class of member 0: %" PRIu64 "\n",
                   len, r, M, ok, nk, (double)ok / nk, lo, hi, pow(1 - 1.0 / 256, r), pok, mism, minc);
        }
    }
    return 0;
}
