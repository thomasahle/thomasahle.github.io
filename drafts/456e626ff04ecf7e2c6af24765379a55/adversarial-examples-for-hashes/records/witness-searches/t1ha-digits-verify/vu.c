/* vu.c -- independent check of the proposed t1ha2_atonce-64 witness pair (F60) against the
 * ORIGINAL upstream t1ha source (erthink/t1ha 00eb779 = v2.1.4-10), through its public API
 * t1ha2_atonce(data, len, seed).  Nothing of the hash is re-implemented here.
 *
 * Build (inside the upstream clone dir):
 *   gcc -O2 -std=gnu11 -I. -o vu vu.c src/t1ha2.c src/t1ha2_selfcheck.c src/t1ha_selfcheck.c -lpthread -lm
 * Modes:
 *   vu selftest
 *   vu check    m1hex m2hex seedhex...                       (explicit seeds, prints both digests)
 *   vu uniform  m1hex m2hex Lmask Lval log2n threads rngseed (fresh uniform 64-bit seeds, xoshiro256**)
 *   vu cond     m1hex m2hex Lmask Lval log2n threads rngseed (uniform over the seed class, via L)
 * Seed protocol: the 64-bit one-shot API seed, uniform; messages are the fixed byte strings.
 * Class: L = lo(((seed ^ l0) + t) * P1), l0 = lo((len1 + w0(m1)) * P2), t = zero-extended tail of m1;
 *        this is how the upstream 9..16-byte path forms its second product (src/t1ha2.c, t1ha2_tail_ab).
 */
#include "t1ha.h"
#include "src/t1ha_selfcheck.h"
#include <inttypes.h>
#include <math.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define P1 UINT64_C(0x82434FE90EDCEF39)
#define P2 UINT64_C(0xD4F06DB99D67BE4B)

static inline uint64_t rotl(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
static inline uint64_t splitmix64(uint64_t *s) {
    uint64_t z = (*s += UINT64_C(0x9E3779B97F4A7C15));
    z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
    z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
    return z ^ (z >> 31);
}
typedef struct { uint64_t s[4]; } xo;
static void xo_seed(xo *x, uint64_t seed) { uint64_t m = seed; for (int i = 0; i < 4; i++) x->s[i] = splitmix64(&m); }
static inline uint64_t xo_next(xo *x) {
    uint64_t *s = x->s, r = rotl(s[1] * 5, 7) * 9, t = s[1] << 17;
    s[2] ^= s[0]; s[3] ^= s[1]; s[1] ^= s[2]; s[0] ^= s[3]; s[2] ^= t; s[3] = rotl(s[3], 45);
    return r;
}
static int parsehex(const char *h, uint8_t *out) {
    int n = (int)strlen(h) / 2;
    for (int i = 0; i < n; i++) { unsigned v; if (sscanf(h + 2 * i, "%2x", &v) != 1) return -1; out[i] = (uint8_t)v; }
    return n;
}
/* messages live in separately malloc'ed exact-size buffers inside a padded page: a normal caller */
static uint8_t *M1, *M2; static size_t N1, N2;
static uint64_t LMASK, LVAL, L0, T1, P1INV;
static inline uint64_t class_L(uint64_t seed) { return ((seed ^ L0) + T1) * P1; }
static inline uint64_t seed_from_L(uint64_t L) { return ((L * P1INV) - T1) ^ L0; }
static void setup(const char *m1, const char *m2) {
    uint8_t b1[64], b2[64]; int n1 = parsehex(m1, b1), n2 = parsehex(m2, b2);
    if (n1 < 9 || n1 > 16 || n2 < 1 || n2 > 32) { fprintf(stderr, "bad lengths\n"); exit(2); }
    N1 = n1; N2 = n2; M1 = malloc(n1); M2 = malloc(n2); memcpy(M1, b1, n1); memcpy(M2, b2, n2);
    uint64_t w0; memcpy(&w0, M1, 8); L0 = ((uint64_t)n1 + w0) * P2;
    T1 = 0; for (int i = 8; i < n1; i++) T1 |= (uint64_t)M1[i] << (8 * (i - 8));
    uint64_t x = P1; for (int i = 0; i < 6; i++) x *= 2 - P1 * x; P1INV = x;
    if (P1INV * P1 != 1) { fprintf(stderr, "inverse\n"); exit(2); }
}
static int selftest(void) {
    int r = t1ha_selfcheck__t1ha2_atonce();
    printf("upstream t1ha_selfcheck__t1ha2_atonce(): %s\n", r == 0 ? "ok" : "FAIL");
    uint8_t key[256] = {0}, hashes[8 * 256];
    for (int i = 0; i < 256; i++) { uint64_t h = t1ha2_atonce(key, (size_t)i, (uint64_t)(256 - i)); memcpy(hashes + 8 * i, &h, 8); key[i] = (uint8_t)i; }
    uint32_t v = (uint32_t)t1ha2_atonce(hashes, sizeof hashes, 0);
    printf("SMHasher3 verification value %08x (want 8f16c948) %s\n", v, v == 0x8F16C948u ? "ok" : "FAIL");
    return (r == 0 && v == 0x8F16C948u) ? 0 : 1;
}
typedef struct { int id, mode; uint64_t n, coll, inclass, rs; } job;
static void *run(void *p) {
    job *j = p; xo x; xo_seed(&x, j->rs);
    for (uint64_t i = 0; i < j->n; i++) {
        uint64_t r = xo_next(&x), seed = j->mode ? seed_from_L((r & ~LMASK) | LVAL) : r;
        uint64_t h1 = t1ha2_atonce(M1, N1, seed);
        if (h1 == t1ha2_atonce(M2, N2, seed)) {
            uint64_t L = class_L(seed); int in = (L & LMASK) == LVAL; j->coll++; j->inclass += in;
            if (!j->mode) { printf("hit thread=%d seed=%016" PRIx64 " H=%016" PRIx64 " L=%016" PRIx64 " inclass=%d\n", j->id, seed, h1, L, in); fflush(stdout); }
        }
    }
    return NULL;
}
int main(int argc, char **argv) {
    if (argc < 2) { fprintf(stderr, "usage\n"); return 2; }
    if (!strcmp(argv[1], "selftest")) return selftest();
    if (selftest()) return 1;
    setup(argv[2], argv[3]);
    printf("m1 len=%zu m2 len=%zu l0=%016" PRIx64 " t=%016" PRIx64 "\n", N1, N2, L0, T1);
    if (!strcmp(argv[1], "check")) {
        for (int i = 4; i < argc; i++) {
            uint64_t s = strtoull(argv[i], NULL, 16), h1 = t1ha2_atonce(M1, N1, s), h2 = t1ha2_atonce(M2, N2, s);
            printf("seed=%016" PRIx64 " H(m1)=%016" PRIx64 " H(m2)=%016" PRIx64 " %s\n", s, h1, h2, h1 == h2 ? "COLLIDE" : "differ");
        }
        return 0;
    }
    int mode = !strcmp(argv[1], "cond"); if (!mode && strcmp(argv[1], "uniform")) { fprintf(stderr, "mode\n"); return 2; }
    LMASK = strtoull(argv[4], NULL, 16); LVAL = strtoull(argv[5], NULL, 16);
    int lg = atoi(argv[6]), nt = atoi(argv[7]); uint64_t rseed = strtoull(argv[8], NULL, 10);
    if ((LVAL & ~LMASK) || nt < 1 || nt > 64) { fprintf(stderr, "args\n"); return 2; }
    /* class density sanity: 2^-popcount(mask) exactly, since seed -> L is a bijection (P1 odd) */
    { uint64_t s0 = 0x123456789abcdef0ULL; if (seed_from_L(class_L(s0)) != s0) { fprintf(stderr, "bijection\n"); return 2; } }
    pthread_t th[64]; job jb[64]; uint64_t coll = 0, inc = 0, n = 0;
    for (int i = 0; i < nt; i++) { jb[i] = (job){i, mode, (1ULL << lg) / nt, 0, 0, rseed * UINT64_C(0xA24BAED4963EE407) + (uint64_t)i * UINT64_C(0x9FB21C651E98DF25) + 1}; pthread_create(&th[i], NULL, run, &jb[i]); }
    for (int i = 0; i < nt; i++) { pthread_join(th[i], NULL); coll += jb[i].coll; inc += jb[i].inclass; n += jb[i].n; }
    int fixed = __builtin_popcountll(LMASK);
    if (mode) printf("cond: fixed=%d samples=%" PRIu64 " (2^%.2f) collisions=%" PRIu64 " cond-rate=2^%.4f => class contribution 2^%.4f\n", fixed, n, log2((double)n), coll, log2((double)coll / n), -fixed + log2((double)coll / n));
    else printf("uniform: rngseed=%" PRIu64 " samples=%" PRIu64 " (2^%.2f) collisions=%" PRIu64 " inclass=%" PRIu64 " rate=%s2^%.3f\n", rseed, n, log2((double)n), coll, inc, coll ? "" : "<", coll ? log2((double)coll / n) : -log2((double)n));
    return 0;
}
