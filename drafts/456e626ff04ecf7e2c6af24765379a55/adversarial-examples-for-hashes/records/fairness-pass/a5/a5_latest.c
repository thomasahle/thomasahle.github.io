/* a5_latest.c -- re-check the post's a5hash-64 6615-byte pair against the LATEST upstream a5hash.h
 * (whatever ../a5hash/a5hash.h is at build time; the version string is printed).
 * 1. reproduces SMHasher3's verification value for the 64-bit hash (0xADDE79B3 for v5.21);
 * 2. checks the example seed / output from the post;
 * 3. enumerates EXACTLY, by 2-adic lifting over all 64 seed bits, the class {seed : Seed1_init == W0}
 *    (no assumption about which bits are free), cross-checks the level-24 count against brute force;
 * 4. hashes both messages under EVERY class seed with the upstream function (OpenMP);
 * 5. 2^24 uniform random seeds as a null control.
 * Thomas Dybdahl Ahle, 2026, MIT.  Build: gcc -O2 -fopenmp -I../a5hash a5_latest.c -o a5_latest -lm
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>
#include <omp.h>
#include "a5hash.h"

#define LEN 6615
#define V01 UINT64_C(0x5555555555555555)
#define V10 UINT64_C(0xAAAAAAAAAAAAAAAA)
#define K1  UINT64_C(0x243F6A8885A308D3)
#define K2  UINT64_C(0x452821E638D01377)

static uint32_t lu32(const uint8_t *p) { return (uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24; }
static void put64(uint8_t *p, uint64_t v) { for (int i = 0; i < 8; i++) p[i] = (uint8_t)(v >> (8 * i)); }

static uint32_t smhasher3_verification64(void) {
    uint8_t key[256], buf[256 * 8], out[8];
    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < i; j++) key[j] = (uint8_t)j;
        put64(buf + i * 8, a5hash(key, (size_t)i, (uint64_t)(256 - i)));
    }
    put64(out, a5hash(buf, 256 * 8, 0));
    return lu32(out);
}

static uint64_t *cls; static size_t ncls, capcls, level24;
static void lift(uint64_t P1, uint64_t P2, uint64_t T, uint64_t s, int i) {
    if (i == 24) level24++;
    if (i == 64) {
        if (ncls == capcls) { capcls = capcls ? 2 * capcls : 1 << 20; cls = realloc(cls, capcls * sizeof *cls); if (!cls) { perror("realloc"); exit(9); } }
        cls[ncls++] = s; return;
    }
    uint64_t mask = (i == 63) ? ~UINT64_C(0) : (UINT64_C(1) << (i + 1)) - 1;
    for (int b = 0; b < 2; b++) {
        uint64_t t = s | ((uint64_t)b << i);
        if (((((P2 ^ (t & V10)) * (P1 ^ (t & V01))) ^ T) & mask) == 0) lift(P1, P2, T, t, i + 1);
    }
}

static uint64_t splitmix64(uint64_t *x) { uint64_t z = (*x += UINT64_C(0x9E3779B97F4A7C15)); z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9); z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB); return z ^ (z >> 31); }

static size_t unhex(const char *h, uint8_t *out) { size_t n = strlen(h) / 2; for (size_t i = 0; i < n; i++) { unsigned v; sscanf(h + 2 * i, "%2x", &v); out[i] = (uint8_t)v; } return n; }

int main(void) {
    printf("a5hash upstream header version: %s\n", A5HASH_VER_STR);
    uint32_t v = smhasher3_verification64();
    printf("SMHasher3 verification (a5hash 64-bit): 0x%08X (v5.21 registered 0xADDE79B3) %s\n", v, v == 0xADDE79B3 ? "OK" : "MISMATCH");
    if (v != 0xADDE79B3) return 1;

    static uint8_t A[LEN], B[LEN];
    for (size_t i = 0; i < LEN; i++) A[i] = B[i] = (uint8_t)((7 * i + 3) & 255);
    unhex("e0aa5c8e000000008877665544332211", A);
    unhex("e0aa5c8e000000001122334455667788", B);
    uint64_t ex = UINT64_C(0xbdd730a20c451ba4);
    uint64_t ha = a5hash(A, LEN, ex), hb = a5hash(B, LEN, ex);
    printf("example seed 0x%016" PRIx64 ": h(A)=%016" PRIx64 " h(B)=%016" PRIx64 " %s (post says 9cda28706bf2e550)\n", ex, ha, hb, ha == hb ? "COLLIDE" : "DIFFER");
    if (ha != hb || ha != UINT64_C(0x9cda28706bf2e550)) return 4;

    uint64_t W0 = (uint64_t)lu32(A) << 32 | lu32(A + 4);
    uint64_t P1 = K1 ^ LEN, P2 = K2 ^ LEN;
    printf("target Seed1_init == W0 = 0x%016" PRIx64 "\n", W0);
    double t0 = omp_get_wtime();
    lift(P1, P2, W0, 0, 0);
    printf("class size (2-adic lift over all 64 seed bits): %zu = %.6f * 2^%d  (%.2f s)\n", ncls, ncls / 524288.0, 19, omp_get_wtime() - t0);
    printf("  118 * 2^19 = %d; match: %s\n", 118 * 524288, ncls == (size_t)118 * 524288 ? "YES" : "NO");
    size_t bf = 0;
    for (uint64_t r = 0; r < (1u << 24); r++) if (((((P2 ^ (r & V10)) * (P1 ^ (r & V01))) ^ W0) & 0xFFFFFF) == 0) bf++;
    printf("  level-24 cross-check: lift %zu vs brute force %zu %s\n", level24, bf, level24 == bf ? "OK" : "FAIL");
    if (level24 != bf) return 3;

    /* hash both messages under every class seed with the upstream function */
    t0 = omp_get_wtime();
    size_t coll = 0, inclass = 0;
    #pragma omp parallel for reduction(+:coll,inclass) schedule(static)
    for (size_t k = 0; k < ncls; k++) {
        uint64_t s = cls[k];
        uint64_t lo = (P2 ^ (s & V10)) * (P1 ^ (s & V01));
        inclass += (lo == W0);
        coll += (a5hash(A, LEN, s) == a5hash(B, LEN, s));
    }
    printf("class rehash: %zu/%zu collide (Seed1_init == W0 re-verified for %zu)  (%.1f s, %d threads)\n", coll, ncls, inclass, omp_get_wtime() - t0, omp_get_max_threads());
    double eps_log2 = log2((double)ncls) - 64.0;
    double L = (double)LEN / 8.0, Lw = ceil(L);
    printf("class density 2^%.5f; L = %.1f words (ceil %d) -> bits = log2(L/eps) = %.4f (ceil L) / %.4f (exact L)\n", eps_log2, L, (int)Lw, log2(Lw) - eps_log2, log2(L) - eps_log2);

    /* random control */
    t0 = omp_get_wtime();
    size_t rc = 0; const size_t N = (size_t)1 << 24;
    #pragma omp parallel reduction(+:rc)
    {
        uint64_t x = UINT64_C(0xA5A5A5A5C0FFEE00) + 1000003u * (uint64_t)omp_get_thread_num();
        #pragma omp for schedule(static)
        for (size_t k = 0; k < N; k++) { uint64_t s = splitmix64(&x); rc += (a5hash(A, LEN, s) == a5hash(B, LEN, s)); }
    }
    printf("uniform random seeds: %zu/%zu collisions (expected %.2e from the class)  (%.1f s)\n", rc, N, N * exp2(eps_log2), omp_get_wtime() - t0);
    return (coll == ncls) ? 0 : 5;
}
