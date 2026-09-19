// tune.c -- for a fixed (m1, m2, L class), re-randomise the tail word t (keeping dt = ts - t and the
// message lengths) and keep the tails whose conditional collision rate is highest.  Digit signs at
// positions just above the class's fixed low run of L are biased in a t-dependent way, so the best
// t of a family can beat the model's 1 bit per sign.  Selection at 2^lgsmall class samples, then
// the top 16 re-measured at 2^lgbig; the printed best must still be confirmed on fresh seeds.
// Usage: tune hexmsg1 hexmsg2 Lmask Lval ntrials lgsmall lgbig threads rngseed
#include "common.h"
static uint8_t M1[16], M2[16]; static int L1, L2; static uint64_t LMASK, LVAL, LINV, LO, DT, W1, W2, R;
static int NT; static uint64_t RNGSEED;
static uint64_t condrate(uint64_t t, int lg, uint64_t rs, uint64_t *ex) {
    uint64_t ts = t + DT, n = 1ULL << lg, hits = 0; *ex = 0;
    uint8_t m1[16], m2[16]; memcpy(m1, M1, 16); memcpy(m2, M2, 16); memcpy(m1 + 8, &t, L1 - 8); memcpy(m2 + 8, &ts, L2 - 8);
    for (uint64_t i = 0; i < n; i++) {
        uint64_t L = (splitmix64(&rs) & ~LMASK) | LVAL; uint64_t u = L * LINV; uint64_t a = u - t; uint64_t seed = a ^ LO;
        if (t1ha2_atonce(m1, L1, seed) == t1ha2_atonce(m2, L2, seed)) { if (!hits) *ex = seed; hits++; } }
    return hits; }
typedef struct { uint64_t t, hits; } tr;
static tr *TR; static long NTR; static int LGS;
typedef struct { int id; } job;
static void *run(void *p) { job *j = p; uint64_t rs = RNGSEED * 0x9E3779B97F4A7C15ULL + j->id * 7919ULL;
    for (long i = j->id; i < NTR; i += NT) {
        uint64_t t, ts; if (L2 < L1) { ts = splitmix64(&rs) & W2; t = ts - DT; } else { t = splitmix64(&rs) & W1; ts = t + DT; }
        if ((ts & ~W2) || (t & ~W1)) { TR[i].t = 0; TR[i].hits = 0; continue; }
        uint64_t ex; TR[i].t = t; TR[i].hits = condrate(t, LGS, rs ^ (t * 0xD1B54A32D192ED03ULL), &ex); }
    return NULL; }
static int cmp(const void *a, const void *b) { const tr *x = a, *y = b; return x->hits < y->hits ? 1 : x->hits > y->hits ? -1 : 0; }
int main(int argc, char **argv) {
    if (argc < 10) { fprintf(stderr, "usage: tune m1 m2 Lmask Lval ntrials lgsmall lgbig threads rngseed\n"); return 1; }
    L1 = strlen(argv[1]) / 2; L2 = strlen(argv[2]) / 2; memset(M1, 0, 16); memset(M2, 0, 16);
    for (int i = 0; i < L1; i++) { unsigned a; sscanf(argv[1] + 2 * i, "%2x", &a); M1[i] = a; }
    for (int i = 0; i < L2; i++) { unsigned a; sscanf(argv[2] + 2 * i, "%2x", &a); M2[i] = a; }
    LMASK = hexu(argv[3]); LVAL = hexu(argv[4]); NTR = atol(argv[5]); LGS = atoi(argv[6]); int lgb = atoi(argv[7]); NT = atoi(argv[8]); RNGSEED = strtoull(argv[9], NULL, 10);
    uint64_t x = P1; for (int i = 0; i < 6; i++) x *= 2 - P1 * x; LINV = x;
    uint64_t w0 = rd64(M1); LO = ((uint64_t)L1 + w0) * P2;
    uint64_t t0 = tail64(M1 + 8, L1 - 8), ts0 = tail64(M2 + 8, L2 - 8); DT = ts0 - t0;
    W1 = L1 < 16 ? ((1ULL << (8 * (L1 - 8))) - 1) : ~0ULL; W2 = L2 < 16 ? ((1ULL << (8 * (L2 - 8))) - 1) : ~0ULL;
    R = __builtin_ctzll(~LMASK);
    uint64_t ex; uint64_t base = condrate(t0, lgb, 12345, &ex);
    printf("base t=%016llx hits=%llu/2^%d rate=2^%.3f fixed=%d run=%llu\n", (unsigned long long)t0, (unsigned long long)base, lgb, log2((double)base / (1ULL << lgb)), __builtin_popcountll(LMASK), (unsigned long long)R);
    TR = calloc(NTR, sizeof(tr)); pthread_t th[64]; job jb[64];
    for (int i = 0; i < NT; i++) { jb[i].id = i; pthread_create(&th[i], NULL, run, &jb[i]); }
    for (int i = 0; i < NT; i++) pthread_join(th[i], NULL);
    qsort(TR, NTR, sizeof(tr), cmp);
    printf("trials=%ld at 2^%d: best small hits %llu, median %llu\n", NTR, LGS, (unsigned long long)TR[0].hits, (unsigned long long)TR[NTR / 2].hits);
    int top = NTR < 16 ? NTR : 16; tr best = {0, 0};
    for (int i = 0; i < top; i++) { uint64_t h = condrate(TR[i].t, lgb, 777 + i, &ex);
        printf("cand t=%016llx small=%llu big=%llu/2^%d rate=2^%.3f est_eps=2^%.3f exseed=%016llx\n", (unsigned long long)TR[i].t, (unsigned long long)TR[i].hits, (unsigned long long)h, lgb, log2((double)h / (1ULL << lgb)), -__builtin_popcountll(LMASK) + log2((double)h / (1ULL << lgb)), (unsigned long long)ex);
        if (h > best.hits) { best.hits = h; best.t = TR[i].t; } }
    uint64_t ts = best.t + DT; uint8_t m1[16], m2[16]; memcpy(m1, M1, 16); memcpy(m2, M2, 16); memcpy(m1 + 8, &best.t, L1 - 8); memcpy(m2 + 8, &ts, L2 - 8);
    printf("BEST m1="); for (int i = 0; i < L1; i++) printf("%02x", m1[i]); printf(" m2="); for (int i = 0; i < L2; i++) printf("%02x", m2[i]);
    printf(" Lmask=%016llx Lval=%016llx big=%llu/2^%d est_eps=2^%.3f\n", (unsigned long long)LMASK, (unsigned long long)LVAL, (unsigned long long)best.hits, lgb, -__builtin_popcountll(LMASK) + log2((double)best.hits / (1ULL << lgb)));
    return 0; }
