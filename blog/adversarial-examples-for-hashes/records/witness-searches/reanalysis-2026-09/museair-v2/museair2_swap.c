/* museair2_swap.c -- key-free (every-seed) collisions for MuseAir v2
 * (crate 0.6.0) museair::hash and museair::bfast::hash on the >32-byte path.
 *
 * Mechanism.  With the 64-bit seed, state word x is C[x] ^ SA (x even) or
 * C[x] ^ SB (x odd), SA = seed & 0xAA.., SB = seed & 0x55...  The finalize
 * of an 81..96-byte input computes six products P_m = A_m * B_m of operands
 *   A0=S0^r0  B0=S1^r1 | A1=S1^r1^r2 B1=S2^r3 | A2=S2^r3^r4 B2=S3^r5 |
 *   A3=S3^r5^r6 B3=S4^r7 | A4=S4^r7^t0 B4=S5^t1 | A5=S5^t1^t2 B5=S0^r0^t3
 * (r = rest words, t = last-32-byte words) and then uses only
 *   i: s0 - s1 = B5 - A1,  g(P3) + g(P4)
 *   j: s2 - s3 = A2 - A3,  g(P5) + g(P0)
 *   k: s4 - s5 = A4 - A5,  g(P1) + g(P2)          (g(P) = lo ^ hi).
 * Every operand is SA or SB XOR a message-chosen offset, so two operands of
 * the same class can be made equal.  Choose M with
 *   B1 = A2 ^ 2^63, B2 = A1   and   B3 = A4, B4 = A3 ^ 2^63,
 * and M' = M with bit 63 of r3, r6, t1, t2 flipped.  Then in M' products P1
 * and P2 are exchanged, P3 and P4 are exchanged (sums unchanged), A2 and A3
 * both gain 2^63 (A2 - A3 unchanged), and nothing else moves: (i, j, k) are
 * identical for every seed.
 *
 * A shorter variant at length 80 (L = 10) is described at build80().
 *
 * Usage: museair2_swap [LOG2SEEDS=24] [RNGSEED]
 */
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include "museair2_core.h"

static uint64_t sm;
static uint64_t splitmix64(void) {
    uint64_t z = (sm += 0x9E3779B97F4A7C15ull);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull; z = (z ^ (z >> 27)) * 0x94D049BB133111EBull;
    return z ^ (z >> 31);
}
static uint64_t xs[4];
static inline uint64_t rl(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
static uint64_t xoshiro(void) {
    uint64_t r = rl(xs[1] * 5, 7) * 9, t = xs[1] << 17;
    xs[2] ^= xs[0]; xs[3] ^= xs[1]; xs[1] ^= xs[2]; xs[0] ^= xs[3]; xs[2] ^= t; xs[3] = rl(xs[3], 45);
    return r;
}
static void rng_seed(uint64_t s) { sm = s; for (int i = 0; i < 4; i++) xs[i] = splitmix64(); }
static void put(uint8_t *m, int w, uint64_t v) { memcpy(m + 8 * w, &v, 8); }
static uint64_t get(const uint8_t *m, int w) { uint64_t v; memcpy(&v, m + 8 * w, 8); return v; }
static const uint64_t TOP = 0x8000000000000000ull;

/* Build the pair for len 96 (t = words 8..11) or 88 (t = words 7..10, t0 = r7). */
static void build(int len, uint8_t *M, uint8_t *Mp) {
    uint64_t r[8], t[4];
    uint64_t f = 0x243F6A8885A308D3ull; /* arbitrary free words */
    for (int x = 0; x < 8; x++) { f = f * 6364136223846793005ull + 1442695040888963407ull; r[x] = f ^ (f >> 31); }
    for (int x = 0; x < 4; x++) { f = f * 6364136223846793005ull + 1442695040888963407ull; t[x] = f ^ (f >> 31); }
    r[4] = TOP;                                  /* B1 = A2 ^ 2^63 */
    r[5] = MC[1] ^ MC[3] ^ r[1] ^ r[2];          /* B2 = A1 */
    t[0] = 0;                                    /* B3 = A4 */
    if (len == 88) r[7] = 0;                     /* t0 is word 7 at length 88 */
    t[1] = MC[5] ^ MC[3] ^ r[5] ^ r[6] ^ TOP;    /* B4 = A3 ^ 2^63 */
    memset(M, 0, 96);
    for (int x = 0; x < 8; x++) put(M, x, r[x]);
    int t0w = (len == 96) ? 8 : 7;
    for (int x = 0; x < 4; x++) put(M, t0w + x, t[x]);
    if (get(M, 7) != r[7]) { fprintf(stderr, "layout error\n"); exit(2); }
    memcpy(Mp, M, 96);
    int flips[4] = { 3, 6, t0w + 1, t0w + 2 };
    for (int x = 0; x < 4; x++) put(Mp, flips[x], get(Mp, flips[x]) ^ TOP);
}
/* Length 80 (L = 10): rest words r0..r5 = words 0..5 feed P0, P1, P2; the
 * tail words t0..t3 = words 6..9 feed P4, P5; P3 is not computed.  Here the
 * j pair (P0, P5) and the k pair (P1, P2) are exchanged:
 *   M:  A0 = B5 ^ 2^63 (t3 = 2^63), B0 = A5, B1 = A2 ^ 2^63 (r4 = 2^63),
 *       B2 = A1 ^ 2^63;   M' = M with bit 63 of r0, r2, r3, r5 flipped.
 * s0 = B5 and s1 = A1 both gain 2^63 (i unchanged), s2 = A2 and s3 = B2 both
 * gain 2^63 (j unchanged), s4, s5 and P4 are untouched. */
static void build80(uint8_t *M, uint8_t *Mp) {
    uint64_t w[10]; uint64_t f = 0x13198A2E03707344ull;
    for (int x = 0; x < 10; x++) { f = f * 6364136223846793005ull + 1442695040888963407ull; w[x] = f ^ (f >> 31); }
    /* r = w[0..5], t = w[6..9] */
    w[4] = TOP;                                   /* B1 = A2 ^ 2^63 */
    w[9] = TOP;                                   /* A0 = B5 ^ 2^63 */
    w[1] = MC[1] ^ MC[5] ^ w[7] ^ w[8];           /* B0 = A5 */
    w[5] = MC[1] ^ MC[3] ^ w[1] ^ w[2] ^ TOP;     /* B2 = A1 ^ 2^63 */
    for (int x = 0; x < 10; x++) put(M, x, w[x]);
    memcpy(Mp, M, 80);
    int flips[4] = { 0, 2, 3, 5 };
    for (int x = 0; x < 4; x++) put(Mp, flips[x], get(Mp, flips[x]) ^ TOP);
}
static void hexline(const char *tag, const uint8_t *m, int len) {
    printf("%s", tag); for (int i = 0; i < len; i++) printf("%02x", m[i]); printf("\n");
}

int main(int argc, char **argv) {
    static const char *names[8] = { "hash", "hash_folded", "hash128", "hash128_folded",
                                    "bfast::hash", "bfast::hash_folded", "bfast::hash128", "bfast::hash128_folded" };
    static const uint32_t want[8] = { 0x7140CABC, 0x0B8F0243, 0x38028C88, 0xB9CD57B7,
                                      0xA4BFD093, 0xDCCDD53A, 0x81863E77, 0x9BAAAF63 };
    int ok = 1;
    for (int w = 0; w < 8; w++) {
        uint32_t v = museair2_verif(w);
        printf("validation: %-22s 0x%08" PRIX32 " (crate stability_v2 0x%08" PRIX32 ") %s\n", names[w], v, want[w], v == want[w] ? "OK" : "FAIL");
        if (v != want[w]) ok = 0;
    }
    if (!ok) return 1;
    int lg = argc > 1 ? atoi(argv[1]) : 24;
    uint64_t rs = argc > 2 ? strtoull(argv[2], 0, 0) : 0x5EED0055ull;
    int lens[3] = { 80, 88, 96 };
    for (int li = 0; li < 3; li++) {
        int len = lens[li]; uint8_t M[96], Mp[96];
        if (len == 80) build80(M, Mp); else build(len, M, Mp);
        printf("\n== length %d (L = %d words) ==\n", len, (len + 7) / 8);
        hexline("M  = ", M, len); hexline("M' = ", Mp, len);
        rng_seed(rs + (uint64_t)len);
        uint64_t N = 1ull << lg, c64 = 0, cbf = 0, c128 = 0, cbf128 = 0, c128s = 0;
        for (uint64_t n = 0; n < N; n++) {
            uint64_t sd = xoshiro(), sb = xoshiro();
            c64 += museair2_64(M, len, sd, 0) == museair2_64(Mp, len, sd, 0);
            cbf += museair2_64(M, len, sd, 1) == museair2_64(Mp, len, sd, 1);
            if (n < (1ull << 20)) {
                uint64_t a[2], b[2];
                museair2_128(M, len, sd, sb, 0, a); museair2_128(Mp, len, sd, sb, 0, b); c128 += (a[0] == b[0] && a[1] == b[1]);
                museair2_128(M, len, sd, sb, 1, a); museair2_128(Mp, len, sd, sb, 1, b); cbf128 += (a[0] == b[0] && a[1] == b[1]);
                museair2_128(M, len, sd, sd, 0, a); museair2_128(Mp, len, sd, sd, 0, b); c128s += (a[0] == b[0] && a[1] == b[1]);
            }
        }
        uint64_t N20 = N < (1ull << 20) ? N : (1ull << 20);
        printf("hash (64-bit):        %" PRIu64 "/%" PRIu64 " seeds collide\n", c64, N);
        printf("bfast::hash (64-bit): %" PRIu64 "/%" PRIu64 " seeds collide\n", cbf, N);
        printf("hash128 (seed_a, seed_b independent): %" PRIu64 "/%" PRIu64 "; bfast::hash128: %" PRIu64 "/%" PRIu64 "; hash128(seed, seed): %" PRIu64 "/%" PRIu64 "\n",
               c128, N20, cbf128, N20, c128s, N20);
        uint64_t ex[3] = { 0, 0xdeadbeefcafef00dull, 0 };
        rng_seed(rs ^ 0xabcdef); ex[2] = xoshiro();
        for (int e = 0; e < 3; e++)
            printf("seed 0x%016" PRIx64 ": hash %016" PRIx64 " %016" PRIx64 "  bfast %016" PRIx64 " %016" PRIx64 "\n", ex[e],
                   museair2_64(M, len, ex[e], 0), museair2_64(Mp, len, ex[e], 0), museair2_64(M, len, ex[e], 1), museair2_64(Mp, len, ex[e], 1));
    }
    return 0;
}
