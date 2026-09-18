/* Independent re-check of the a5hash-64 row on upstream a5hash.h (no re-implementation).
 * gcc -O2 -std=c11 -fopenmp -I<a5hash dir> a5_verify2.c -o a5_verify2 -lm
 * Args: A.bin B.bin (the data.json m_hex / m_prime_hex bytes). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <omp.h>
#include "a5hash.h"

#define LEN 6615
#define T   UINT64_C(0x8e5caae000000000)   /* w0 of both messages: lu32(M)<<32 ^ lu32(M+4) */
#define K1  UINT64_C(0x243F6A8885A308D3)
#define K2  UINT64_C(0x452821E638D01377)
#define E   UINT64_C(0x5555555555555555)
#define O   UINT64_C(0xAAAAAAAAAAAAAAAA)

static unsigned char A[LEN], B[LEN];

static uint64_t seed1_init(uint64_t s) {      /* low half of the seed-expansion product */
    unsigned __int128 p = (unsigned __int128)((K2 ^ LEN) ^ (s & O)) * ((K1 ^ LEN) ^ (s & E));
    return (uint64_t)p;
}
static uint32_t smh_verify(void) {            /* SMHasher3 lib/Hashinfo.cpp _ComputedVerifyImpl for a 64-bit hash */
    uint8_t key[256]; uint8_t hashes[8 * 256]; memset(key, 0, 256);
    for (int i = 0; i < 256; i++) {
        uint64_t h = a5hash(key, i, (uint64_t)(256 - i));
        for (int j = 0; j < 8; j++) hashes[i * 8 + j] = (uint8_t)(h >> (8 * j));
        key[i] = (uint8_t)i;
    }
    uint64_t t = a5hash(hashes, 8 * 256, 0);
    return (uint32_t)t;
}
static uint64_t splitmix(uint64_t *x) { uint64_t z = (*x += UINT64_C(0x9E3779B97F4A7C15));
    z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9); z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB); return z ^ (z >> 31); }

/* 2-adic lift: seeds whose Seed1_init == T, bit by bit from bit 0. Leaves at level 64. */
static uint64_t lift_count_to(uint64_t s, int j, int stop) {   /* count prefixes of length `stop` below prefix (s, j) */
    if (j == stop) return 1;
    uint64_t c = 0;
    for (int b = 0; b < 2; b++) {
        uint64_t s2 = s | ((uint64_t)b << j);
        uint64_t mask = (j == 63) ? ~UINT64_C(0) : ((UINT64_C(1) << (j + 1)) - 1);
        if (((seed1_init(s2) ^ T) & mask) == 0) c += lift_count_to(s2, j + 1, stop);
    }
    return c;
}
static void lift_collect(uint64_t s, int j, int stop, uint64_t *out, uint64_t *n) {
    if (j == stop) { out[(*n)++] = s; return; }
    for (int b = 0; b < 2; b++) {
        uint64_t s2 = s | ((uint64_t)b << j);
        uint64_t mask = (j == 63) ? ~UINT64_C(0) : ((UINT64_C(1) << (j + 1)) - 1);
        if (((seed1_init(s2) ^ T) & mask) == 0) lift_collect(s2, j + 1, stop, out, n);
    }
}
/* from a level-`j` prefix, walk to level 64 and hash both messages at each leaf */
static void lift_hash(uint64_t s, int j, uint64_t *leaves, uint64_t *coll, uint64_t *bad_example) {
    if (j == 64) {
        (*leaves)++;
        uint64_t ha = a5hash(A, LEN, s), hb = a5hash(B, LEN, s);
        if (ha == hb) (*coll)++; else if (!*bad_example) *bad_example = s;
        return;
    }
    for (int b = 0; b < 2; b++) {
        uint64_t s2 = s | ((uint64_t)b << j);
        uint64_t mask = (j == 63) ? ~UINT64_C(0) : ((UINT64_C(1) << (j + 1)) - 1);
        if (((seed1_init(s2) ^ T) & mask) == 0) lift_hash(s2, j + 1, leaves, coll, bad_example);
    }
}

int main(int argc, char **argv) {
    printf("a5hash upstream header version %s\n", A5HASH_VER_STR);
    uint32_t v = smh_verify();
    printf("SMHasher3 verification (64-bit): 0x%08X expected 0xADDE79B3 %s\n", v, v == 0xADDE79B3 ? "OK" : "MISMATCH");
    if (v != 0xADDE79B3) return 1;

    /* recipe */
    for (size_t i = 0; i < LEN; i++) A[i] = B[i] = (unsigned char)((7 * i + 3) & 255);
    const unsigned char a16[16] = {0xe0,0xaa,0x5c,0x8e,0,0,0,0,0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11};
    const unsigned char b16[16] = {0xe0,0xaa,0x5c,0x8e,0,0,0,0,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88};
    memcpy(A, a16, 16); memcpy(B, b16, 16);
    if (argc >= 3) {
        unsigned char fa[LEN + 1], fb[LEN + 1]; FILE *f;
        f = fopen(argv[1], "rb"); size_t na = f ? fread(fa, 1, LEN + 1, f) : 0; if (f) fclose(f);
        f = fopen(argv[2], "rb"); size_t nb = f ? fread(fb, 1, LEN + 1, f) : 0; if (f) fclose(f);
        printf("data.json bytes: A %zu B %zu; recipe == data.json: A %s, B %s\n", na, nb,
               (na == LEN && !memcmp(A, fa, LEN)) ? "yes" : "NO", (nb == LEN && !memcmp(B, fb, LEN)) ? "yes" : "NO");
        if (!(na == LEN && nb == LEN && !memcmp(A, fa, LEN) && !memcmp(B, fb, LEN))) return 2;
    }
    uint64_t w0 = (uint64_t)a5hash_lu32(A) << 32 ^ a5hash_lu32(A + 4);
    printf("w0 from upstream reads = 0x%016llx (T = 0x%016llx) %s\n", (unsigned long long)w0, (unsigned long long)T, w0 == T ? "OK" : "MISMATCH");

    uint64_t ex = UINT64_C(0xbdd730a20c451ba4);
    uint64_t ha = a5hash(A, LEN, ex), hb = a5hash(B, LEN, ex);
    printf("example seed 0x%016llx: h(A)=%016llx h(B)=%016llx %s; Seed1_init=%016llx (%s)\n", (unsigned long long)ex,
           (unsigned long long)ha, (unsigned long long)hb, ha == hb ? "COLLIDE" : "DIFFER",
           (unsigned long long)seed1_init(ex), seed1_init(ex) == T ? "in class" : "NOT in class");
    if (ha != hb || ha != UINT64_C(0x9cda28706bf2e550)) return 4;

    /* level-24 cross-check */
    uint64_t brute = 0;
    for (uint64_t s = 0; s < (UINT64_C(1) << 24); s++) if (((seed1_init(s) ^ T) & 0xFFFFFF) == 0) brute++;
    uint64_t lift24 = lift_count_to(0, 0, 24);
    printf("level-24 prefixes: lift %llu, brute force %llu %s\n", (unsigned long long)lift24, (unsigned long long)brute, lift24 == brute ? "OK" : "MISMATCH");
    if (lift24 != brute) return 3;

    /* enumerate to level 24, then parallel DFS + rehash */
    uint64_t *pre = malloc(lift24 * sizeof *pre); uint64_t np = 0; lift_collect(0, 0, 24, pre, &np);
    uint64_t leaves = 0, coll = 0, bad = 0; double t0 = omp_get_wtime();
    #pragma omp parallel
    {
        uint64_t l = 0, c = 0, b = 0;
        #pragma omp for schedule(dynamic, 16)
        for (uint64_t i = 0; i < np; i++) lift_hash(pre[i], 24, &l, &c, &b);
        #pragma omp critical
        { leaves += l; coll += c; if (!bad) bad = b; }
    }
    double dt = omp_get_wtime() - t0;
    printf("class {seed : Seed1_init == T}: %llu seeds = %g * 2^19 (all 64 seed bits lifted)\n", (unsigned long long)leaves, leaves / 524288.0);
    printf("rehash under every class seed: %llu/%llu collide (%.1f s, %d threads)%s\n", (unsigned long long)coll, (unsigned long long)leaves, dt, omp_get_max_threads(),
           coll == leaves ? "" : " -- NOT ALL");
    if (bad) printf("first non-colliding class seed: 0x%016llx\n", (unsigned long long)bad);
    double l2 = log2((double)coll) - 64.0;
    printf("density 2^%.5f; L = ceil(6615/8) = 827 -> bits = log2(827) - (%.5f) = %.4f; exact L=826.875 -> %.4f\n", l2, l2, log2(827.0) - l2, log2(826.875) - l2);

    /* null control */
    uint64_t x = UINT64_C(0x1234567890abcdef), nc = 0;
    for (uint64_t i = 0; i < (UINT64_C(1) << 24); i++) { uint64_t s = splitmix(&x); if (a5hash(A, LEN, s) == a5hash(B, LEN, s)) nc++; }
    printf("null control: %llu/2^24 uniform seeds collide (expected %.2g from the class)\n", (unsigned long long)nc, exp2(24 + l2));
    return coll == leaves ? 0 : 5;
}
