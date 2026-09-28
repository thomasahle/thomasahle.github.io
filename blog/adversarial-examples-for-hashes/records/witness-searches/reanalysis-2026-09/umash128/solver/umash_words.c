/*
 * Extract the two per-lane polynomial-hash input words that the UMASH-128
 * medium (9..16 byte) path feeds into each lane's Horner update, for a chosen
 * message, OH/ENH keys and seed.  Uses the reference implementation verbatim
 * (its static inlines) by including umash.c.
 *
 * For a fixed message pair, lane i collides for multiplier f_i iff
 *     f_i^2 * dx_i + f_i * dy_i == 0   (mod 2^64 - 8 = 8p),
 * where (dx_i, dy_i) are the differences of the words printed here.
 * We print xword,yword for lane 0 and lane 1.
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>
#include "umash.c"

int main(int argc, char **argv) {
    /* argv: hex16(message, 32 hex chars) oh0 oh1 lrc0 lrc1 seed nbytes */
    if (argc != 8) { fprintf(stderr, "usage: msg16hex oh0 oh1 lrc0 lrc1 seed nbytes\n"); return 2; }
    uint8_t msg[16] = {0};
    for (int i = 0; i < 16; i++) { unsigned v; sscanf(argv[1] + 2*i, "%2x", &v); msg[i] = (uint8_t)v; }
    uint64_t oh0 = strtoull(argv[2],0,16), oh1 = strtoull(argv[3],0,16);
    uint64_t lrc0 = strtoull(argv[4],0,16), lrc1 = strtoull(argv[5],0,16);
    uint64_t seed = strtoull(argv[6],0,16);
    size_t n = (size_t)strtoull(argv[7],0,10);

    const uint64_t offset = seed ^ n;
    uint64_t lrc[2] = { lrc0, lrc1 };
    union { v128 v; uint64_t u64[2]; } mixed_lrc;
    uint64_t x, y, a, b, enh_hi, enh_lo;

    memcpy(&x, msg, sizeof(x));
    memcpy(&y, msg + n - sizeof(y), sizeof(y));
    a = oh0; b = oh1;
    lrc[0] ^= x ^ a;
    lrc[1] ^= y ^ b;
    mixed_lrc.v = v128_clmul(lrc[0], lrc[1]);
    a += x; b += y;
    mul128(a, b, &enh_hi, &enh_lo);
    enh_hi += offset;
    enh_hi ^= enh_lo;

    uint64_t x0 = enh_lo,                    y0 = enh_hi;
    uint64_t x1 = enh_lo ^ mixed_lrc.u64[0], y1 = enh_hi ^ mixed_lrc.u64[1];
    printf("%016" PRIx64 " %016" PRIx64 " %016" PRIx64 " %016" PRIx64 "\n", x0, y0, x1, y1);
    return 0;
}
