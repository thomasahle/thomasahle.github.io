/* UMASH-128 (umash_fprint): an explicit full 128-bit collision of the shipped C function.
 *
 * Mechanism.  On the 9..16-byte path each fingerprint lane i outputs
 * finalize(f_i^2 * x_i + f_i * y_i mod 8p), 8p = 2^64 - 8, p = 2^61 - 1, where (x_i, y_i) are lane i's
 * compressed words and f_i is lane i's secret multiplier.  For a fixed pair lane i collides iff
 * f_i^2 dx_i + f_i dy_i = 0 (mod 8p): one nonzero root mod p plus a mod-8 residue condition.  The two
 * multipliers are independent, so eps = (n0/M)(n1/M) with n_i the root counts and M = p - 2 accepted
 * values; the paper envelope's r^2 term is exactly this product, at degree 2*ceil(L/32) per lane.
 * Here the roots were solved for one 16-byte pair and one set of OH/ENH words; the two roots are then
 * used as the multipliers.  umash_params_prepare accepts the key unchanged, so this is a collision of
 * the shipped function under a valid key (it witnesses the mechanism; it is not a rate measurement).
 *
 * Build (upstream umash 9709e11, see SOURCES.md):
 *   x86-64: cc -O2 -std=c11 -mpclmul -o umash128_collision umash128_collision.c umash.c
 *   arm64:  cc -O2 -std=c11 -march=armv8-a+crypto -o umash128_collision umash128_collision.c umash.c
 */
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "umash.h"

static int hex16(const char *s, uint8_t out[16]) {
    for (int i = 0; i < 16; i++) { unsigned v; if (sscanf(s + 2 * i, "%2x", &v) != 1) return -1; out[i] = (uint8_t)v; }
    return 0;
}

static int build(struct umash_params *p, uint64_t f0, uint64_t f1) {
    memset(p, 0, sizeof *p);
    p->poly[0][1] = f0;                       /* lane-0 multiplier */
    p->poly[1][1] = f1;                       /* lane-1 multiplier */
    p->poly[0][0] = 0x1111111111111111ULL;    /* spare entropy for prepare's repair path (unused here) */
    p->poly[1][0] = 0x2222222222222222ULL;
    p->oh[0] = 0x135499bc53154baeULL;         /* OH words read on the 16-byte path */
    p->oh[1] = 0xe2ddc31c759c3ee4ULL;
    for (int i = 2; i < UMASH_OH_PARAM_COUNT; i++) p->oh[i] = 0x100 + (uint64_t)i;   /* distinct filler */
    p->oh[UMASH_OH_PARAM_COUNT] = 0x46eb3a10ff22baf9ULL;       /* fingerprint (second-lane) words */
    p->oh[UMASH_OH_PARAM_COUNT + 1] = 0x8c9aaa012ab2a6b7ULL;
    if (!umash_params_prepare(p)) return -1;
    if (p->poly[0][1] != f0 || p->poly[1][1] != f1) return -2;   /* prepare must keep the multipliers */
    return 0;
}

int main(void) {
    /* 1. upstream README vector: example.c parameters, seed 42 */
    static const char secret[32] = "hello example.c";
    struct umash_params ex;
    umash_params_derive(&ex, 0, secret);
    const char *in = "the quick brown fox";
    struct umash_fp v = umash_fprint(&ex, 42, in, strlen(in));
    int vok = v.hash[0] == 0x398c5bb5cc113d03ULL && v.hash[1] == 0x3a52693519575abaULL;
    printf("upstream README vector: %016" PRIx64 ", %016" PRIx64 " (expected 398c5bb5cc113d03, 3a52693519575aba) %s\n",
           v.hash[0], v.hash[1], vok ? "OK" : "MISMATCH");
    if (!vok) return 1;

    /* 2. the witness */
    uint8_t m1[16], m2[16];
    hex16("074a1dda8ebce2aab0e27d1d87698812", m1);
    hex16("074a1dda8ebce2aab0e27dd487698812", m2);   /* byte 6: 1d -> d4 */
    const uint64_t f0 = 0x0d12f0abd78e00b5ULL, f1 = 0x0b0c691ad7f5dc18ULL, seed = 0xd11bd6324600d9bdULL;
    struct umash_params p;
    int rc = build(&p, f0, f1);
    if (rc) { printf("umash_params_prepare rejected or altered the key (%d)\n", rc); return 1; }
    struct umash_fp a = umash_fprint(&p, seed, m1, 16), b = umash_fprint(&p, seed, m2, 16);
    int coll = a.hash[0] == b.hash[0] && a.hash[1] == b.hash[1];
    printf("umash_fprint(M1) = %016" PRIx64 " %016" PRIx64 "\n", a.hash[0], a.hash[1]);
    printf("umash_fprint(M2) = %016" PRIx64 " %016" PRIx64 "\n", b.hash[0], b.hash[1]);
    printf("full 128-bit collision: %s\n", coll ? "YES" : "no");

    /* 3. controls: move one multiplier off its root; that lane separates, the other still collides */
    struct umash_params q;
    if (build(&q, f0 + 1, f1) == 0) {
        struct umash_fp c = umash_fprint(&q, seed, m1, 16), d = umash_fprint(&q, seed, m2, 16);
        printf("control f0+1: lane0 %s, lane1 %s\n", c.hash[0] == d.hash[0] ? "collide" : "differ", c.hash[1] == d.hash[1] ? "collide" : "differ");
    }
    if (build(&q, f0, f1 + 1) == 0) {
        struct umash_fp c = umash_fprint(&q, seed, m1, 16), d = umash_fprint(&q, seed, m2, 16);
        printf("control f1+1: lane0 %s, lane1 %s\n", c.hash[0] == d.hash[0] ? "collide" : "differ", c.hash[1] == d.hash[1] ? "collide" : "differ");
    }
    return coll ? 0 : 1;
}
