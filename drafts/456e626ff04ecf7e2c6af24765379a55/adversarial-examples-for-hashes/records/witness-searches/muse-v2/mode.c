/* mode.c: model-free mode finder for the short path.  For len and a byte index, for every
 * XOR value x = 1..255 applied to that byte of M = 0^len, sample N seeds and take the modal
 * XOR-difference of the pre-finalizer state (i,j); its count/N is the collision rate of the
 * pair (M, M ^ (D on head, x on byte)).  ./mode <len> <byte> <log2N> <threads> */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "museair2.h"
typedef struct { uint64_t a, b; } pr;
static int cmp(const void *x, const void *y){ const pr *p = x, *q = y; if (p->a != q->a) return p->a < q->a ? -1 : 1; if (p->b != q->b) return p->b < q->b ? -1 : 1; return 0; }
static int LEN, BYTE, LG, NTH; static pthread_mutex_t mu = PTHREAD_MUTEX_INITIALIZER;
static void *run(void *arg){
    int id = (int)(intptr_t)arg; uint64_t N = 1ull << LG; pr *buf = malloc(N * sizeof *buf);
    uint8_t M[64], M2[64]; memset(M, 0, 64);
    for (int x = 1; x < 256; x++) {
        if (x % NTH != id) continue;
        memcpy(M2, M, 64); M2[BYTE] ^= (uint8_t)x;
        uint64_t ss = 0x600d5eedull + (uint64_t)LEN * 4099 + (uint64_t)BYTE * 257 + x;
        for (uint64_t t = 0; t < N; t++) {
            uint64_t s = ma_splitmix(&ss), i0, j0, i1, j1;
            ma2_short_state64(M, LEN, s, &i0, &j0); ma2_short_state64(M2, LEN, s, &i1, &j1);
            buf[t].a = i0 ^ i1; buf[t].b = j0 ^ j1;
        }
        qsort(buf, N, sizeof *buf, cmp);
        uint64_t best = 1, run = 1, second = 0; pr bt = buf[0];
        for (uint64_t t = 1; t < N; t++) { if (!cmp(&buf[t], &buf[t-1])) run++; else { if (run > best) { second = best; best = run; bt = buf[t-1]; } else if (run > second) second = run; run = 1; } }
        if (run > best) { second = best; best = run; bt = buf[N-1]; } else if (run > second) second = run;
        pthread_mutex_lock(&mu);
        printf("len %d byte %d xor %02x  mode_count %llu (second %llu) / 2^%d  Di %016llx Dj %016llx\n", LEN, BYTE, x, (unsigned long long)best, (unsigned long long)second, LG, (unsigned long long)bt.a, (unsigned long long)bt.b);
        fflush(stdout); pthread_mutex_unlock(&mu);
    }
    free(buf); return NULL;
}
int main(int argc, char **argv){
    if (argc < 5) { fprintf(stderr, "usage: %s len byte log2N threads\n", argv[0]); return 2; }
    LEN = atoi(argv[1]); BYTE = atoi(argv[2]); LG = atoi(argv[3]); NTH = atoi(argv[4]);
    pthread_t th[64]; for (int i = 0; i < NTH; i++) pthread_create(&th[i], 0, run, (void*)(intptr_t)i);
    for (int i = 0; i < NTH; i++) pthread_join(th[i], 0); return 0;
}
