/*
 * Build a real umash_params with chosen multipliers f0,f1 and OH/ENH keys,
 * then print umash_fprint(seed, M1) and umash_fprint(seed, M2) and whether
 * the full 128-bit fingerprints collide.  Uses the reference umash.c verbatim,
 * including umash_params_prepare (which derives f^2 and enforces the accepted
 * multiplier set), so this is a genuine collision of the shipped function.
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>
#include "umash.h"

int main(int argc, char **argv) {
    /* argv: m1hex m2hex nbytes f0 f1 oh0 oh1 lrc0 lrc1 seed */
    if (argc != 11) { fprintf(stderr, "usage: m1hex m2hex nbytes f0 f1 oh0 oh1 lrc0 lrc1 seed\n"); return 2; }
    uint8_t m1[16] = {0}, m2[16] = {0};
    for (int i = 0; i < 16; i++) { unsigned v; sscanf(argv[1]+2*i,"%2x",&v); m1[i]=(uint8_t)v; }
    for (int i = 0; i < 16; i++) { unsigned v; sscanf(argv[2]+2*i,"%2x",&v); m2[i]=(uint8_t)v; }
    size_t n = (size_t)strtoull(argv[3],0,10);
    uint64_t f0 = strtoull(argv[4],0,16), f1 = strtoull(argv[5],0,16);
    uint64_t oh0 = strtoull(argv[6],0,16), oh1 = strtoull(argv[7],0,16);
    uint64_t lrc0 = strtoull(argv[8],0,16), lrc1 = strtoull(argv[9],0,16);
    uint64_t seed = strtoull(argv[10],0,16);

    struct umash_params params;
    memset(&params, 0, sizeof(params));
    params.poly[0][1] = f0;
    params.poly[1][1] = f1;
    /* spare-entropy slots (unused because our keys are all valid/distinct) */
    params.poly[0][0] = 0x1111111111111111ULL;
    params.poly[1][0] = 0x2222222222222222ULL;
    params.oh[0] = oh0; params.oh[1] = oh1;
    /* distinct filler for the remaining OH words so dedup never fires */
    for (int i = 2; i < UMASH_OH_PARAM_COUNT; i++) params.oh[i] = 0x100 + i;
    params.oh[UMASH_OH_PARAM_COUNT]     = lrc0;
    params.oh[UMASH_OH_PARAM_COUNT + 1] = lrc1;

    if (!umash_params_prepare(&params)) { fprintf(stderr, "prepare rejected\n"); return 3; }
    /* Confirm prepare kept our multipliers unchanged. */
    if (params.poly[0][1] != f0 || params.poly[1][1] != f1) {
        fprintf(stderr, "prepare altered multipliers f0=%016"PRIx64" f1=%016"PRIx64"\n",
                params.poly[0][1], params.poly[1][1]); return 4;
    }
    if (params.oh[0] != oh0 || params.oh[1] != oh1 ||
        params.oh[UMASH_OH_PARAM_COUNT] != lrc0 || params.oh[UMASH_OH_PARAM_COUNT+1] != lrc1) {
        fprintf(stderr, "prepare altered oh/lrc keys\n"); return 5;
    }

    struct umash_fp a = umash_fprint(&params, seed, m1, n);
    struct umash_fp b = umash_fprint(&params, seed, m2, n);
    printf("M1 fp = %016"PRIx64" %016"PRIx64"\n", a.hash[0], a.hash[1]);
    printf("M2 fp = %016"PRIx64" %016"PRIx64"\n", b.hash[0], b.hash[1]);
    printf("lane0 %s  lane1 %s  full128 %s\n",
           a.hash[0]==b.hash[0]?"COLLIDE":"differ",
           a.hash[1]==b.hash[1]?"COLLIDE":"differ",
           (a.hash[0]==b.hash[0]&&a.hash[1]==b.hash[1])?"COLLIDE":"differ");
    return (a.hash[0]==b.hash[0]&&a.hash[1]==b.hash[1]) ? 0 : 1;
}
