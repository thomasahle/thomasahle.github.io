// Usage: cond2 hexmsg1 hexmsg2 Lmask Lval log2n [threads]
// msg1 must have length 9..16.  Samples L uniformly with (L & Lmask)==Lval, derives the seed from msg1
// (u = L*P1^-1, a = u - t1, seed = a ^ lo((len1+w0)*P2)), hashes both messages, counts full collisions.
#include "common.h"
static uint8_t M1[64], M2[64]; static int L1, L2; static uint64_t LMASK, LVAL, LINV, W0, T1, LO;
typedef struct { int id; uint64_t n, coll, ex; } job;
static void *run(void *p) { job *j = p; uint64_t rs = 0x77777ULL * (j->id + 3) ^ 0xFACE;
    for (uint64_t i = 0; i < j->n; i++) {
        uint64_t L = (splitmix64(&rs) & ~LMASK) | LVAL; uint64_t u = L * LINV; uint64_t a = u - T1; uint64_t seed = a ^ LO;
        if (t1ha2_atonce(M1, L1, seed) == t1ha2_atonce(M2, L2, seed)) { j->coll++; if (j->coll == 1) j->ex = seed; } }
    return NULL; }
int main(int argc, char **argv) {
    if (argc < 6) { fprintf(stderr, "usage\n"); return 1; }
    L1 = strlen(argv[1]) / 2; L2 = strlen(argv[2]) / 2;
    for (int i = 0; i < L1; i++) { unsigned a; sscanf(argv[1] + 2 * i, "%2x", &a); M1[i] = a; }
    for (int i = 0; i < L2; i++) { unsigned a; sscanf(argv[2] + 2 * i, "%2x", &a); M2[i] = a; }
    LMASK = hexu(argv[3]); LVAL = hexu(argv[4]); int lg = atoi(argv[5]); int nt = argc > 6 ? atoi(argv[6]) : 3;
    uint64_t x = P1; for (int i = 0; i < 6; i++) x *= 2 - P1 * x; LINV = x;
    W0 = rd64(M1); T1 = tail64(M1 + 8, L1 - 8); LO = ((uint64_t)L1 + W0) * P2;
    pthread_t th[64]; job jb[64]; uint64_t coll = 0, ex = 0, n = 0;
    for (int i = 0; i < nt; i++) { jb[i] = (job){i, (1ULL << lg) / nt, 0, 0}; pthread_create(&th[i], NULL, run, &jb[i]); }
    for (int i = 0; i < nt; i++) { pthread_join(th[i], NULL); coll += jb[i].coll; n += jb[i].n; if (!ex) ex = jb[i].ex; }
    int fixed = __builtin_popcountll(LMASK);
    printf("len1=%d len2=%d fixed=%d samples=2^%d collisions=%llu cond-rate=%s2^%.2f => eps>=2^%.2f example_seed=%016llx\n", L1, L2, fixed, lg,
        (unsigned long long)coll, coll ? "" : "<", coll ? log2((double)coll / n) : -(double)lg, coll ? -fixed + log2((double)coll / n) : -999.0, (unsigned long long)ex);
    return 0; }
