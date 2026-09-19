// direct_class.c -- fresh uniform-seed collision count for a fixed t1ha2_atonce pair (9..16 bytes),
// reporting for every hit whether the seed lies in the stated L class.
// Usage: direct_class hexmsg1 hexmsg2 Lmask Lval log2n threads rngseed
// Seeds: splitmix64 streams seeded by (rngseed, thread id); every seed is printed on a hit.
#include "common.h"
static uint8_t M1[64], M2[64]; static int L1, L2; static uint64_t LMASK, LVAL, W0, T1, LO;
typedef struct { int id; uint64_t n, coll, inclass, rs; uint64_t ex[64]; } job;
static void *run(void *p) { job *j = p; uint64_t rs = j->rs;
    for (uint64_t i = 0; i < j->n; i++) { uint64_t s = splitmix64(&rs);
        if (t1ha2_atonce(M1, L1, s) == t1ha2_atonce(M2, L2, s)) {
            uint64_t a = s ^ LO, u = a + T1, L = u * P1;
            int in = (L & LMASK) == LVAL; if (in) j->inclass++;
            if (j->coll < 64) j->ex[j->coll] = s; j->coll++;
            printf("hit thread=%d seed=%016llx L=%016llx inclass=%d\n", j->id, (unsigned long long)s, (unsigned long long)L, in); fflush(stdout);
        } }
    return NULL; }
int main(int argc, char **argv) {
    if (argc < 8) { fprintf(stderr, "usage: direct_class m1 m2 Lmask Lval log2n threads rngseed\n"); return 1; }
    L1 = strlen(argv[1]) / 2; L2 = strlen(argv[2]) / 2;
    for (int i = 0; i < L1; i++) { unsigned a; sscanf(argv[1] + 2 * i, "%2x", &a); M1[i] = a; }
    for (int i = 0; i < L2; i++) { unsigned a; sscanf(argv[2] + 2 * i, "%2x", &a); M2[i] = a; }
    LMASK = hexu(argv[3]); LVAL = hexu(argv[4]); int lg = atoi(argv[5]); int nt = atoi(argv[6]); uint64_t rseed = strtoull(argv[7], NULL, 10);
    W0 = rd64(M1); T1 = tail64(M1 + 8, L1 - 8); LO = ((uint64_t)L1 + W0) * P2;
    // selftest
    { uint8_t key[256] = { 0 }, hashes[8 * 256];
      for (int i = 0; i < 256; i++) { uint64_t h = t1ha2_atonce(key, (size_t)i, (uint64_t)(256 - i)); memcpy(hashes + 8 * i, &h, 8); key[i] = (uint8_t)i; }
      uint32_t v = (uint32_t)t1ha2_atonce(hashes, sizeof hashes, 0);
      printf("selftest SMHasher3 verification %08x (want 8f16c948) %s\n", v, v == 0x8F16C948u ? "ok" : "FAIL"); if (v != 0x8F16C948u) return 1; }
    pthread_t th[64]; job jb[64]; uint64_t coll = 0, inc = 0, n = 0;
    for (int i = 0; i < nt; i++) { jb[i] = (job){0}; jb[i].id = i; jb[i].n = (1ULL << lg) / nt; jb[i].rs = rseed * 0x9E3779B97F4A7C15ULL + (uint64_t)i * 0xD1B54A32D192ED03ULL; pthread_create(&th[i], NULL, run, &jb[i]); }
    for (int i = 0; i < nt; i++) { pthread_join(th[i], NULL); coll += jb[i].coll; inc += jb[i].inclass; n += jb[i].n; }
    double rate = coll ? (double)coll / n : 0;
    // 95% CI for a Poisson count (Garwood exact would need chi2; use the Wilson-score-free normal approx on log scale for large counts, exact-ish bounds via 1.96*sqrt)
    double lo = coll > 0 ? (coll - 1.96 * sqrt((double)coll)) / n : 0, hi = (coll + 1.96 * sqrt((double)coll) + 3.84 / 2) / n;
    printf("len1=%d len2=%d rngseed=%llu samples=%llu (2^%.2f) collisions=%llu inclass=%llu rate=%s2^%.2f approx95=[2^%.2f, 2^%.2f]\n", L1, L2,
        (unsigned long long)rseed, (unsigned long long)n, log2((double)n), (unsigned long long)coll, (unsigned long long)inc,
        coll ? "" : "<", coll ? log2(rate) : -log2((double)n), lo > 0 ? log2(lo) : -99.0, log2(hi));
    return 0; }
