/* Independent re-check of the fasthash fixed pairs, linked against the
 * unmodified upstream ztanml/fast-hash fasthash.c. No transcription.
 * (a) exhaustive over all 2^32 seeds of upstream fasthash32(uint32_t seed)
 * (b) 2^30 splitmix64 seeds of fasthash64 and of the SMHasher3-style fold
 *     h - (h>>32) (which is what SMHasher3 'fasthash-32' computes with a
 *     64-bit seed, since fasthash.cpp passes seed through to the 64-bit core). */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <inttypes.h>
#include "fasthash.h"

static const uint8_t A7[7] = {0};
static const uint8_t B8[8] = {0x5e,0x17,0x7b,0xe7,0xd1,0xd1,0x75,0xf3};
static const uint8_t A16[16] = {0};
static const uint8_t B16[16] = {0xf6,0x9e,0x1c,0x7b,0xcf,0x4d,0xb1,0x6f,0xf6,0x9e,0x1c,0x7b,0xcf,0x4d,0xb1,0x6f};
static inline uint32_t fold(uint64_t h){ return (uint32_t)(h-(h>>32)); }
static uint64_t splitmix(uint64_t x){ x+=0x9e3779b97f4a7c15ULL; x=(x^(x>>30))*0xbf58476d1ce4e5b9ULL; x=(x^(x>>27))*0x94d049bb133111ebULL; return x^(x>>31); }

#define NT 8
typedef struct { int id; uint64_t c32a, c32b, n32, c64a, c64b, f32a, f32b, n64, bad; uint64_t badseed; } W;
static void *work(void *p){
    W *w=p;
    /* (a) exhaustive 32-bit API */
    for(uint64_t s=w->id; s<(1ULL<<32); s+=NT){
        uint32_t sd=(uint32_t)s;
        w->n32++;
        if(fasthash32(A7,7,sd)==fasthash32(B8,8,sd)) w->c32a++; else { w->bad++; w->badseed=s; }
        if(fasthash32(A16,16,sd)==fasthash32(B16,16,sd)) w->c32b++; else { w->bad++; w->badseed=s; }
    }
    /* (b) 2^30 splitmix64 seeds, 64-bit API and fold */
    for(uint64_t i=w->id; i<(1ULL<<30); i+=NT){
        uint64_t sd=splitmix(i*0x9e3779b97f4a7c15ULL+12345);
        w->n64++;
        uint64_t x=fasthash64(A7,7,sd), y=fasthash64(B8,8,sd);
        if(x==y) w->c64a++; else { w->bad++; w->badseed=sd; }
        if(fold(x)==fold(y)) w->f32a++;
        x=fasthash64(A16,16,sd); y=fasthash64(B16,16,sd);
        if(x==y) w->c64b++; else { w->bad++; w->badseed=sd; }
        if(fold(x)==fold(y)) w->f32b++;
    }
    return 0;
}
int main(void){
    uint64_t seeds[3]={0x0123456789abcdefULL,0xc05a677850dc981aULL,0};
    for(int i=0;i<3;i++){
        uint64_t s=seeds[i];
        printf("seed %016" PRIx64 ": fh64(7B)=%016" PRIx64 " fh64(8B)=%016" PRIx64 " fold=%08x/%08x | fh64(16B)=%016" PRIx64 "/%016" PRIx64 " fold=%08x/%08x | upstream fh32(seed trunc %08x)=%08x/%08x 16B %08x/%08x\n",
            s, fasthash64(A7,7,s), fasthash64(B8,8,s), fold(fasthash64(A7,7,s)), fold(fasthash64(B8,8,s)),
            fasthash64(A16,16,s), fasthash64(B16,16,s), fold(fasthash64(A16,16,s)), fold(fasthash64(B16,16,s)),
            (uint32_t)s, fasthash32(A7,7,(uint32_t)s), fasthash32(B8,8,(uint32_t)s), fasthash32(A16,16,(uint32_t)s), fasthash32(B16,16,(uint32_t)s));
    }
    /* sanity: pair is not degenerate and the 32-bit API is not seed-independent */
    printf("fh32(7B, seed 0)=%08x fh32(7B, seed 1)=%08x (should differ)\n", fasthash32(A7,7,0), fasthash32(A7,7,1));
    printf("fh64(7B, seed 0)=%016" PRIx64 " fh64(00, 1B, seed 0)=%016" PRIx64 " (control: 1B vs 7B zero should differ)\n", fasthash64(A7,7,0), fasthash64(A7,1,0));
    pthread_t t[NT]; W w[NT]; memset(w,0,sizeof w);
    for(int i=0;i<NT;i++){ w[i].id=i; pthread_create(&t[i],0,work,&w[i]); }
    W T; memset(&T,0,sizeof T);
    for(int i=0;i<NT;i++){ pthread_join(t[i],0); T.n32+=w[i].n32; T.c32a+=w[i].c32a; T.c32b+=w[i].c32b; T.n64+=w[i].n64; T.c64a+=w[i].c64a; T.c64b+=w[i].c64b; T.f32a+=w[i].f32a; T.f32b+=w[i].f32b; T.bad+=w[i].bad; if(w[i].bad) T.badseed=w[i].badseed; }
    printf("upstream fasthash32 EXHAUSTIVE: 7B vs 8B %" PRIu64 "/%" PRIu64 "; 16B pair %" PRIu64 "/%" PRIu64 "\n", T.c32a, T.n32, T.c32b, T.n32);
    printf("fasthash64 splitmix 2^30: 7B vs 8B %" PRIu64 "/%" PRIu64 " (fold %" PRIu64 "); 16B pair %" PRIu64 "/%" PRIu64 " (fold %" PRIu64 ")\n", T.c64a, T.n64, T.f32a, T.c64b, T.n64, T.f32b);
    printf("non-colliding events: %" PRIu64 " (last seed %016" PRIx64 ")\n", T.bad, T.badseed);
    return T.bad?1:0;
}
