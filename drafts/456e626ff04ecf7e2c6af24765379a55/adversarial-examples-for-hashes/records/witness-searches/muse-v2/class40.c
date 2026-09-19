/* class40.c: long-path exact 2^-32 class at len 40 (L = 5).
 * state[4] = C4 ^ (seed & MASK_A); the tail product wmul(state[4]^t0, state[5]^t1) vanishes when
 * seed & MASK_A == (C4 ^ t0) & MASK_A and (C4 ^ t0) & MASK_B == 0.  Then t1, t2 may change by a
 * common delta (state[5] ^= t1 ^ t2 is unchanged) without changing the output.
 * M = bytes 0..7 zero, t0 = C4, t1 = t2 = t3 = 0;  M2: t1 = t2 = delta.
 * ./class40 <log2N_uniform> <log2N_class> <rngseed> */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "museair2.h"
static void wr64(uint8_t *p, uint64_t x){ memcpy(p, &x, 8); }
int main(int argc, char **argv){
    int lgU = argc > 1 ? atoi(argv[1]) : 30, lgC = argc > 2 ? atoi(argv[2]) : 20; uint64_t rng = argc > 3 ? strtoull(argv[3], 0, 0) : 1;
    uint8_t M[40], M2[40]; memset(M, 0, 40); wr64(M + 8, MA_C[4]); memcpy(M2, M, 40);
    uint64_t delta = 0x0123456789abcdefull; wr64(M2 + 16, delta); wr64(M2 + 24, delta);
    printf("M  = "); for (int i = 0; i < 40; i++) printf("%02x", M[i]); printf("\nM2 = "); for (int i = 0; i < 40; i++) printf("%02x", M2[i]); printf("\n");
    uint64_t cond = MA_C[4] ^ MA_C[4]; (void)cond;   /* class: seed & MASK_A == 0 (since t0 = C4) */
    uint64_t st = rng, hits[2] = {0,0}, N = 1ull << lgU;
    for (uint64_t t = 0; t < N; t++) { uint64_t s = ma_splitmix(&st);
        hits[0] += museair2_hash(M, 40, s, 0) == museair2_hash(M2, 40, s, 0);
        hits[1] += museair2_hash(M, 40, s, 1) == museair2_hash(M2, 40, s, 1); }
    printf("uniform 2^%d seeds: hash %llu, bfast %llu  (expected 2^%d)\n", lgU, (unsigned long long)hits[0], (unsigned long long)hits[1], lgU - 32);
    uint64_t Nc = 1ull << lgC, ch[2] = {0,0}; st = rng ^ 0xff;
    for (uint64_t t = 0; t < Nc; t++) { uint64_t s = ma_splitmix(&st) & MA_MASK_B;   /* class member: odd bits zero */
        ch[0] += museair2_hash(M, 40, s, 0) == museair2_hash(M2, 40, s, 0);
        ch[1] += museair2_hash(M, 40, s, 1) == museair2_hash(M2, 40, s, 1); }
    printf("class-sampled 2^%d seeds (seed & MASK_A == 0): hash %llu, bfast %llu\n", lgC, (unsigned long long)ch[0], (unsigned long long)ch[1]);
    st = rng ^ 0x77; uint64_t s = ma_splitmix(&st) & MA_MASK_B;
    printf("example class seed 0x%016llx: hash(M)=%016llx hash(M2)=%016llx\n", (unsigned long long)s, (unsigned long long)museair2_hash(M, 40, s, 0), (unsigned long long)museair2_hash(M2, 40, s, 0));
    return 0;
}
