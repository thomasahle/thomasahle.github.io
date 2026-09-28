/* nmhash32 v2: lane-tied trail T2 (D = 0x04008040) multicollision check.
 * Independent of the search program: builds the families from the description only and
 * hashes with the upstream header nmhash.h (gzm55/hash-garage e022156ca8, see SOURCES.md).
 *
 *   cc -O2 -std=c11 -pthread -o nm_check nm_check.c
 *   ./nm_check witness            # prints the 16 members of the 64-byte family, seed 0xffffffbf
 *   ./nm_check exh64 [threads]    # all 2^32 seeds: 16-way 64 B family (guess 7) vs the 3-bit predictor
 *   ./nm_check perkey N [threads] # fixed 65,536-message set at 512 B, largest class for N random seeds
 *   ./nm_check fam16 N [threads]  # 2^16 lane-subset family at 512 B (guess 5), N uniform and N class seeds
 *
 * Construction (little-endian 32-bit words, lane j):
 *   64 B  (33..255-byte path, P = NMH_PRIME32_1..4):
 *     x_j = P_j ^ A ^ [on]D, y_j = B ^ [on]D, x'_j = [on]C, y'_j = [on]D  at byte offsets 4j, 16+4j, 32+4j, 48+4j
 *   512 B (long path, NMH_ACC_INIT[j], 32 lanes): same at offsets 4j, 128+4j, 256+4j, 384+4j
 *   D = 0x04008040, C = 0x00008020, B = 0.  A = A(g) sets bits 6, 15, 26 to the complement of the
 *   guess g of those bits of y (y = seed + len on the 64 B path, y = seed on the long path), so that
 *   bits 26, 15, 6 of x ^ y are all 1 exactly when the guess is right.  [on] = lane j is in the subset.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "nmhash.h"

#define D 0x04008040u
#define C 0x00008020u
static const uint32_t P4[4] = {NMH_PRIME32_1, NMH_PRIME32_2, NMH_PRIME32_3, NMH_PRIME32_4};

static void put32(uint8_t *m, size_t o, uint32_t v) { m[o] = v; m[o+1] = v >> 8; m[o+2] = v >> 16; m[o+3] = v >> 24; }
static uint32_t H(const uint8_t *m, size_t n, uint32_t s) { return NMHASH32(m, n, s); }
static uint32_t A_of(unsigned g) {
    uint32_t A = 0;
    if (!(g & 1)) A |= 1u << 6;
    if (!(g & 2)) A |= 1u << 15;
    if (!(g & 4)) A |= 1u << 26;
    return A;
}
static unsigned bits3(uint32_t y) { return ((y >> 6) & 1) | (((y >> 15) & 1) << 1) | (((y >> 26) & 1) << 2); }

static void build64(uint8_t m[64], unsigned mask, unsigned g) {
    uint32_t A = A_of(g);
    for (int j = 0; j < 4; j++) {
        int on = (mask >> j) & 1;
        put32(m, 4*j, P4[j] ^ A ^ (on ? D : 0));
        put32(m, 16 + 4*j, on ? D : 0);
        put32(m, 32 + 4*j, on ? C : 0);
        put32(m, 48 + 4*j, on ? D : 0);
    }
}
static void build512(uint8_t m[512], uint32_t mask, unsigned g) {
    uint32_t A = A_of(g);
    for (int j = 0; j < 32; j++) {
        int on = (mask >> j) & 1;
        put32(m, 4*j, NMH_ACC_INIT[j] ^ A ^ (on ? D : 0));
        put32(m, 128 + 4*j, on ? D : 0);
        put32(m, 256 + 4*j, on ? C : 0);
        put32(m, 384 + 4*j, on ? D : 0);
    }
}

/* SMHasher3 verification: keys {}, {0}, {0,1}, ... with seed 256 - i, then hash of the hashes. */
static uint32_t verification(void) {
    uint8_t key[256] = {0}, hs[1024];
    for (uint32_t i = 0; i < 256; i++) { put32(hs, 4*i, H(key, i, 256 - i)); key[i] = (uint8_t)i; }
    return H(hs, sizeof hs, 0);
}

static uint64_t rng_state = 0x243f6a8885a308d3ull;
static uint64_t splitmix(uint64_t *x) { uint64_t z = (*x += 0x9e3779b97f4a7c15ull); z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ull; z = (z ^ (z >> 27)) * 0x94d049bb133111ebull; return z ^ (z >> 31); }

/* ---- exh64 ---- */
static uint8_t F64[16][64];
static int NT = 8;
typedef struct { int t; uint64_t all, pred, all_not_pred, pred_not_all; } exh_arg;
static void *exh_worker(void *p) {
    exh_arg *a = p;
    for (uint64_t s = (uint64_t)a->t; s < (1ull << 32); s += NT) {
        uint32_t seed = (uint32_t)s, h0 = H(F64[0], 64, seed);
        int ok = 1;
        for (int k = 1; k < 16 && ok; k++) ok = H(F64[k], 64, seed) == h0;
        int pr = bits3(seed + 64u) == 7;
        a->all += ok; a->pred += pr; a->all_not_pred += ok && !pr; a->pred_not_all += pr && !ok;
    }
    return 0;
}

/* ---- perkey ---- */
static uint8_t (*SET)[512];
static long NSEEDS;
static uint64_t RS;
typedef struct { int t; long minc, maxc, done; } pk_arg;
static int cmpu(const void *x, const void *y) { uint32_t a = *(const uint32_t *)x, b = *(const uint32_t *)y; return a < b ? -1 : a > b; }
static void *pk_worker(void *p) {
    pk_arg *a = p; uint64_t st = RS + 7919ull * (uint64_t)a->t;
    uint32_t *h = malloc(65536 * sizeof *h);
    a->minc = 1L << 30; a->maxc = 0;
    for (long i = a->t; i < NSEEDS; i += NT) {
        uint32_t s = (uint32_t)splitmix(&st);
        for (int j = 0; j < 65536; j++) h[j] = H(SET[j], 512, s);
        qsort(h, 65536, sizeof *h, cmpu);
        long best = 1, run = 1;
        for (int j = 1; j < 65536; j++) { run = h[j] == h[j-1] ? run + 1 : 1; if (run > best) best = run; }
        if (best < a->minc) a->minc = best;
        if (best > a->maxc) a->maxc = best;
        a->done++;
    }
    free(h);
    return 0;
}

/* ---- fam16 ---- */
static uint8_t (*FAM)[512];
typedef struct { int t, cls; long hit; } fam_arg;
static void *fam_worker(void *p) {
    fam_arg *a = p; uint64_t st = RS + 104729ull * (uint64_t)a->t + (uint64_t)a->cls;
    for (long i = a->t; i < NSEEDS; i += NT) {
        uint32_t s;
        do s = (uint32_t)splitmix(&st); while (a->cls && bits3(s) != 5);
        uint32_t h0 = H(FAM[0], 512, s); int ok = 1;
        for (int j = 1; j < 65536 && ok; j++) ok = H(FAM[j], 512, s) == h0;
        a->hit += ok;
    }
    return 0;
}

int main(int argc, char **argv) {
    uint32_t v = verification();
    printf("SMHasher3 verification of upstream nmhash.h NMHASH32: %08X (expected 12A30553) %s\n", v, v == 0x12A30553u ? "OK" : "MISMATCH");
    if (v != 0x12A30553u) return 1;
    if (argc < 2) { fprintf(stderr, "usage: see header comment\n"); return 2; }
    const char *md = argv[1];
    if (!strcmp(md, "witness")) {
        for (unsigned k = 0; k < 16; k++) build64(F64[k], k, 7);
        for (unsigned k = 0; k < 16; k++) {
            printf("member %2u = ", k); for (int i = 0; i < 64; i++) printf("%02x", F64[k][i]);
            printf("  H(.,0xffffffbf) = %08x\n", H(F64[k], 64, 0xffffffbfu));
        }
        printf("seed 0 (not in the class): member 0 -> %08x, member 1 -> %08x\n", H(F64[0], 64, 0), H(F64[1], 64, 0));
        return 0;
    }
    if (!strcmp(md, "exh64")) {
        if (argc > 2) NT = atoi(argv[2]);
        for (unsigned k = 0; k < 16; k++) build64(F64[k], k, 7);
        pthread_t th[256]; exh_arg a[256]; memset(a, 0, sizeof a);
        for (int t = 0; t < NT; t++) { a[t].t = t; pthread_create(&th[t], 0, exh_worker, &a[t]); }
        uint64_t all = 0, pred = 0, anp = 0, pna = 0;
        for (int t = 0; t < NT; t++) { pthread_join(th[t], 0); all += a[t].all; pred += a[t].pred; anp += a[t].all_not_pred; pna += a[t].pred_not_all; }
        printf("exh64 T2 guess 7: 16-way 64-byte family collides for %llu / 2^32 seeds; predictor (bits 26,15,6 of seed+64 all 1) %llu; family-without-predictor %llu; predictor-without-family %llu\n",
               (unsigned long long)all, (unsigned long long)pred, (unsigned long long)anp, (unsigned long long)pna);
        return 0;
    }
    if (!strcmp(md, "perkey")) {
        NSEEDS = argc > 2 ? atol(argv[2]) : 256; if (argc > 3) NT = atoi(argv[3]); RS = 20260928;
        SET = malloc(65536 * 512ull);
        for (unsigned g = 0; g < 8; g++) for (uint32_t j = 0; j < 8192; j++) build512(SET[g * 8192 + j], j, g);
        /* members are distinct: A(g) differs per g in lane 0's x word bits 6/15/26; masks differ per j */
        pthread_t th[256]; pk_arg a[256]; memset(a, 0, sizeof a);
        for (int t = 0; t < NT; t++) { a[t].t = t; pthread_create(&th[t], 0, pk_worker, &a[t]); }
        long mn = 1L << 30, mx = 0, done = 0;
        for (int t = 0; t < NT; t++) { pthread_join(th[t], 0); if (a[t].minc < mn) mn = a[t].minc; if (a[t].maxc > mx) mx = a[t].maxc; done += a[t].done; }
        printf("perkey: fixed set of 65536 distinct 512-byte messages (8 guesses x 2^13 lane subsets); largest full-output class over %ld uniform seeds: min %ld, max %ld\n", done, mn, mx);
        return 0;
    }
    if (!strcmp(md, "fam16")) {
        NSEEDS = argc > 2 ? atol(argv[2]) : 256; if (argc > 3) NT = atoi(argv[3]); RS = 424242;
        FAM = malloc(65536 * 512ull);
        for (uint32_t j = 0; j < 65536; j++) build512(FAM[j], j, 5);
        for (int cls = 0; cls < 2; cls++) {
            pthread_t th[256]; fam_arg a[256]; memset(a, 0, sizeof a);
            for (int t = 0; t < NT; t++) { a[t].t = t; a[t].cls = cls; pthread_create(&th[t], 0, fam_worker, &a[t]); }
            long hit = 0; for (int t = 0; t < NT; t++) { pthread_join(th[t], 0); hit += a[t].hit; }
            printf("fam16 T2 guess 5, 2^16 members at 512 B, %s seeds: whole family collides for %ld / %ld\n", cls ? "class (bits 26,15,6 of seed = 1,0,1)" : "uniform", hit, NSEEDS);
        }
        return 0;
    }
    fprintf(stderr, "unknown mode\n"); return 2;
}
