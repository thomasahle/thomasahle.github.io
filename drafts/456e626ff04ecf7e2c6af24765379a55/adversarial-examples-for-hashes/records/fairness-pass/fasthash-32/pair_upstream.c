/* Pair check linked against the UPSTREAM ztanml/fast-hash fasthash.c (unmodified).
 * 1) recorded seeds -> outputs; 2) every-seed check: fasthash32 (uint32_t seed API)
 * exhaustively over all 2^32 seeds; fasthash64 over 2^30 splitmix64 seeds (and the
 * 32-bit fold of the same 64-bit value, i.e. the SMHasher3 fasthash-32 model). */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <inttypes.h>
#include "fasthash.h"
static const uint8_t A[7]={0};
static const uint8_t B[8]={0x5e,0x17,0x7b,0xe7,0xd1,0xd1,0x75,0xf3};
static const uint8_t E1[16]={0};
static const uint8_t E2[16]={0xf6,0x9e,0x1c,0x7b,0xcf,0x4d,0xb1,0x6f,0xf6,0x9e,0x1c,0x7b,0xcf,0x4d,0xb1,0x6f};
#define NT 8
typedef struct { int id; uint64_t coll32, coll64, coll32of64, coll16_64, coll16_32, bad; } W;
static uint64_t sm(uint64_t z){ z=(z^(z>>30))*0xbf58476d1ce4e5b9ULL; z=(z^(z>>27))*0x94d049bb133111ebULL; return z^(z>>31);}
static void* work(void* p){ W* w=p;
  /* exhaustive 32-bit seeds, strided */
  for(uint64_t s=w->id; s<(1ULL<<32); s+=NT){
    uint32_t x=fasthash32(A,7,(uint32_t)s), y=fasthash32(B,8,(uint32_t)s);
    if(x==y) w->coll32++; else w->bad++;
  }
  /* 2^30 splitmix64 64-bit seeds */
  for(uint64_t i=w->id; i<(1ULL<<30); i+=NT){
    uint64_t s=sm((i+1)*0x9e3779b97f4a7c15ULL);
    uint64_t x=fasthash64(A,7,s), y=fasthash64(B,8,s);
    if(x==y) w->coll64++; else w->bad++;
    if((uint32_t)(x-(x>>32))==(uint32_t)(y-(y>>32))) w->coll32of64++;
    uint64_t u=fasthash64(E1,16,s), v=fasthash64(E2,16,s);
    if(u==v) w->coll16_64++; else w->bad++;
    if((uint32_t)(u-(u>>32))==(uint32_t)(v-(v>>32))) w->coll16_32++;
  }
  return 0; }
int main(void){
  uint64_t seeds[3]={0x0123456789abcdefULL,0xc05a677850dc981aULL,0};
  for(int i=0;i<3;i++){ uint64_t s=seeds[i];
    printf("seed %016" PRIx64 ": fasthash64(M)=%016" PRIx64 " fasthash64(M')=%016" PRIx64 " | fasthash32(uint32 seed %08x): %08x %08x | 16B pair: %016" PRIx64 " %016" PRIx64 "\n",
      s,fasthash64(A,7,s),fasthash64(B,8,s),(uint32_t)s,fasthash32(A,7,(uint32_t)s),fasthash32(B,8,(uint32_t)s),fasthash64(E1,16,s),fasthash64(E2,16,s)); }
  pthread_t t[NT]; W w[NT]; memset(w,0,sizeof w);
  for(int i=0;i<NT;i++){ w[i].id=i; pthread_create(&t[i],0,work,&w[i]); }
  W tot={0}; for(int i=0;i<NT;i++){ pthread_join(t[i],0); tot.coll32+=w[i].coll32; tot.coll64+=w[i].coll64; tot.coll32of64+=w[i].coll32of64; tot.coll16_64+=w[i].coll16_64; tot.coll16_32+=w[i].coll16_32; tot.bad+=w[i].bad; }
  printf("fasthash32 (upstream uint32_t seed), 7B vs 8B: %" PRIu64 " / %" PRIu64 " collisions (EXHAUSTIVE over all 2^32 seeds)\n",tot.coll32,1ULL<<32);
  printf("fasthash64, 7B vs 8B: %" PRIu64 " / %" PRIu64 " collisions (2^30 splitmix64 seeds); 32-bit fold of same: %" PRIu64 "\n",tot.coll64,1ULL<<30,tot.coll32of64);
  printf("fasthash64, 16B top-bit pair: %" PRIu64 " / %" PRIu64 "; 32-bit fold: %" PRIu64 "\n",tot.coll16_64,1ULL<<30,tot.coll16_32);
  printf("non-colliding events: %" PRIu64 "\n",tot.bad);
  return tot.bad?1:0; }
