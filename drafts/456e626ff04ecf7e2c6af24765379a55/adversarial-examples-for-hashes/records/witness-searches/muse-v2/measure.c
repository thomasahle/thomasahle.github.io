/* measure.c: count seeds (uniform 64-bit, splitmix64 stream) on which a fixed pair collides.
 * ./measure <mhex> <m2hex> <log2N> <rngseed> [threads]
 * Reports all four variants: hash, bfast::hash (seed), hash128, bfast::hash128 (seed_a, seed_b). */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "museair2.h"
static uint8_t M[64], M2[64]; static size_t LEN; static uint64_t N; static uint64_t RNG;
static int hexv(char c){ return c<='9'? c-'0' : (c|32)-'a'+10; }
typedef struct { int id, nth; uint64_t c[4]; uint64_t first_seed; uint64_t first_h; } job;
static void *run(void *p){
    job *J = p; uint64_t st = RNG ^ (0x9E3779B97F4A7C15ull * (uint64_t)(J->id + 1)); ma_splitmix(&st);
    uint64_t per = N / J->nth; int seen = 0;
    for (uint64_t t = 0; t < per; t++) {
        uint64_t s = ma_splitmix(&st), sb = ma_splitmix(&st);
        uint64_t h0 = museair2_hash(M, LEN, s, 0), h1 = museair2_hash(M2, LEN, s, 0);
        if (h0 == h1) { J->c[0]++; if (!seen) { seen = 1; J->first_seed = s; J->first_h = h0; } }
        if (museair2_hash(M, LEN, s, 1) == museair2_hash(M2, LEN, s, 1)) J->c[1]++;
        if (museair2_hash128(M, LEN, s, sb, 0) == museair2_hash128(M2, LEN, s, sb, 0)) J->c[2]++;
        if (museair2_hash128(M, LEN, s, sb, 1) == museair2_hash128(M2, LEN, s, sb, 1)) J->c[3]++;
    }
    return NULL;
}
int main(int argc, char **argv){
    if (argc < 5) { fprintf(stderr, "usage: %s mhex m2hex log2N rngseed [threads]\n", argv[0]); return 2; }
    LEN = strlen(argv[1]) / 2; if (strlen(argv[2]) != strlen(argv[1]) || LEN > 64) { fprintf(stderr, "lengths\n"); return 2; }
    for (size_t i = 0; i < LEN; i++) { M[i] = hexv(argv[1][2*i])*16 + hexv(argv[1][2*i+1]); M2[i] = hexv(argv[2][2*i])*16 + hexv(argv[2][2*i+1]); }
    int lg = atoi(argv[3]); N = 1ull << lg; RNG = strtoull(argv[4], 0, 0); int nth = argc > 5 ? atoi(argv[5]) : 8;
    job J[64]; pthread_t th[64];
    for (int i = 0; i < nth; i++) { J[i].id = i; J[i].nth = nth; J[i].c[0]=J[i].c[1]=J[i].c[2]=J[i].c[3]=0; J[i].first_seed = 0; pthread_create(&th[i], 0, run, &J[i]); }
    uint64_t c[4] = {0,0,0,0}; uint64_t fs = 0, fh = 0;
    for (int i = 0; i < nth; i++) { pthread_join(th[i], 0); for (int v = 0; v < 4; v++) c[v] += J[i].c[v]; if (!fs && J[i].first_seed) { fs = J[i].first_seed; fh = J[i].first_h; } }
    uint64_t tot = (N / nth) * nth;
    const char *names[4] = {"hash", "bfast::hash", "hash128", "bfast::hash128"};
    printf("len %zu, N = %llu (2^%d), rng 0x%llx, %d threads\n", LEN, (unsigned long long)tot, lg, (unsigned long long)RNG, nth);
    for (int v = 0; v < 4; v++) {
        double p = (double)c[v] / tot, lo, hi;
        /* Wilson-free: exact-ish Poisson 95% CI on the count */
        double k = (double)c[v]; lo = k > 0 ? k * pow(1 - 1.0/(9*k) - 1.96/(3*sqrt(k)), 3) : 0; hi = (k + 1) * pow(1 - 1.0/(9*(k+1)) + 1.96/(3*sqrt(k+1)), 3);
        printf("  %-15s %llu / %llu  rate 2^%.3f  [2^%.3f, 2^%.3f]\n", names[v], (unsigned long long)c[v], (unsigned long long)tot,
               c[v] ? log2(p) : -INFINITY, lo > 0 ? log2(lo / tot) : -INFINITY, log2(hi / tot));
    }
    if (fs) printf("  first colliding seed (hash): 0x%016llx -> 0x%016llx\n", (unsigned long long)fs, (unsigned long long)fh);
    return 0;
}
