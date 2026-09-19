/* scan.c: bounded empirical single-bit differential scan.
 * For len 1..32: for every bit p of the message, over N seeds, take the XOR difference of the
 * short-path pre-finalizer state (i,j) between M (random fixed) and M^bit; report the modal
 * difference and its count.  A mode with count c means a head-compensated pair collides at
 * rate ~c/N (if the mode's bits lie in head-coverable words, i.e. always for len >= 17).
 * For len 33..64: same for the long-path (i,j,k) before the three final products.
 * ./scan <lenlo> <lenhi> <log2N> <threads> */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "museair2.h"
typedef struct { uint64_t a, b, c; } trip;
static int cmp(const void *x, const void *y){ const trip *p = x, *q = y;
    if (p->a != q->a) return p->a < q->a ? -1 : 1; if (p->b != q->b) return p->b < q->b ? -1 : 1; if (p->c != q->c) return p->c < q->c ? -1 : 1; return 0; }
static int LENLO, LENHI, LG, NTH; static pthread_mutex_t mu = PTHREAD_MUTEX_INITIALIZER;
static void *run(void *arg){
    int id = (int)(intptr_t)arg; uint64_t N = 1ull << LG; trip *buf = malloc(N * sizeof *buf);
    uint8_t M[64], M2[64]; uint64_t rs = 0x1234567 + id;
    int cell = 0;
    for (int len = LENLO; len <= LENHI; len++) for (int p = 0; p < 8*len; p++, cell++) {
        if (cell % NTH != id) continue;
        uint64_t st = 0xabcdef01ull * (uint64_t)(len * 1000 + p + 1);
        for (int i = 0; i < len; i++) M[i] = (uint8_t)ma_splitmix(&st);
        memcpy(M2, M, len); M2[p >> 3] ^= (uint8_t)(1u << (p & 7));
        uint64_t ss = 0x5eed0000ull + (uint64_t)len * 977 + p;
        for (uint64_t t = 0; t < N; t++) {
            uint64_t s = ma_splitmix(&ss);
            if (len <= 32) {
                uint64_t i0, j0, i1, j1; ma2_short_state64(M, len, s, &i0, &j0); ma2_short_state64(M2, len, s, &i1, &j1);
                buf[t].a = i0 ^ i1; buf[t].b = j0 ^ j1; buf[t].c = 0;
            } else {
                uint64_t S[6], i0, j0, k0, i1, j1, k1; const uint8_t *r; size_t rl; uint64_t circ;
                ma2_state_seed64(s, S); r = M; rl = len; circ = MA_C[6]; (void)circ;
                ma2_finalize_pre(S, r, rl, M + len - 32, len, &i0, &j0, &k0);
                ma2_state_seed64(s, S);
                ma2_finalize_pre(S, M2, len, M2 + len - 32, len, &i1, &j1, &k1);
                buf[t].a = i0 ^ i1; buf[t].b = j0 ^ j1; buf[t].c = k0 ^ k1;
            }
        }
        qsort(buf, N, sizeof *buf, cmp);
        uint64_t best = 1, run = 1; trip bt = buf[0]; uint64_t zeros = 0;
        for (uint64_t t = 1; t < N; t++) { if (!cmp(&buf[t], &buf[t-1])) run++; else run = 1; if (run > best) { best = run; bt = buf[t]; } }
        if (buf[0].a == 0 && buf[0].b == 0 && buf[0].c == 0) { uint64_t t = 0; while (t < N && !buf[t].a && !buf[t].b && !buf[t].c) t++; zeros = t; }
        pthread_mutex_lock(&mu);
        printf("len %2d bit %3d  mode_count %llu  zero_count %llu  mode %016llx %016llx %016llx\n", len, p, (unsigned long long)best, (unsigned long long)zeros,
               (unsigned long long)bt.a, (unsigned long long)bt.b, (unsigned long long)bt.c);
        fflush(stdout); pthread_mutex_unlock(&mu);
    }
    free(buf); (void)rs; return NULL;
}
int main(int argc, char **argv){
    if (argc < 5) { fprintf(stderr, "usage: %s lenlo lenhi log2N threads\n", argv[0]); return 2; }
    LENLO = atoi(argv[1]); LENHI = atoi(argv[2]); LG = atoi(argv[3]); NTH = atoi(argv[4]);
    pthread_t th[64]; for (int i = 0; i < NTH; i++) pthread_create(&th[i], 0, run, (void*)(intptr_t)i);
    for (int i = 0; i < NTH; i++) pthread_join(th[i], 0); return 0;
}
