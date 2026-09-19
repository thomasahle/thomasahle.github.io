// Usage: direct hexmsg1 hexmsg2 log2n [threads]  -- equal-length messages as hex strings
#include "common.h"
static uint8_t M1[256], M2[256]; static int LEN, LEN2; static int LOG2N;
typedef struct { int id; uint64_t n, coll, ex; } job;
static void *run(void *p) { job *j = p; uint64_t rs = 0x5151ULL * (j->id + 7) ^ 0xBEEF;
    for (uint64_t i = 0; i < j->n; i++) { uint64_t s = splitmix64(&rs);
        if (t1ha2_atonce(M1, LEN, s) == t1ha2_atonce(M2, LEN2, s)) { j->coll++; if (j->coll == 1) j->ex = s; } }
    return NULL; }
int main(int argc, char **argv) {
    LEN = strlen(argv[1]) / 2; LEN2 = strlen(argv[2]) / 2; LOG2N = atoi(argv[3]); int nt = argc > 4 ? atoi(argv[4]) : 3;
    for (int i = 0; i < LEN; i++) { unsigned a; sscanf(argv[1] + 2 * i, "%2x", &a); M1[i] = a; } for (int i = 0; i < LEN2; i++) { unsigned b; sscanf(argv[2] + 2 * i, "%2x", &b); M2[i] = b; }
    pthread_t th[64]; job jb[64]; uint64_t coll = 0, ex = 0;
    for (int i = 0; i < nt; i++) { jb[i] = (job){i, (1ULL << LOG2N) / nt, 0, 0}; pthread_create(&th[i], NULL, run, &jb[i]); }
    for (int i = 0; i < nt; i++) { pthread_join(th[i], NULL); coll += jb[i].coll; if (!ex) ex = jb[i].ex; }
    printf("len1=%d len2=%d samples=2^%d collisions=%llu rate=%s2^%.2f example=%016llx\n", LEN, LEN2, LOG2N, (unsigned long long)coll, coll ? "" : "<", coll ? __builtin_log2((double)coll / (1ULL << LOG2N)) : -LOG2N, (unsigned long long)ex);
    return 0; }
