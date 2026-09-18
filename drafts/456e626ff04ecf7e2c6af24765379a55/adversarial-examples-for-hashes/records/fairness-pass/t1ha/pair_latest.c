/* Pair check for t1ha2_atonce against the UPSTREAM library (linked from src/t1ha2.c).
   Usage: ./pair_latest <log2 uniform seeds> <log2 class seeds> [rng seed]  */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <omp.h>
#include "t1ha.h"

static inline uint64_t splitmix64(uint64_t *s){ uint64_t z=(*s+=0x9e3779b97f4a7c15ULL); z=(z^(z>>30))*0xbf58476d1ce4e5b9ULL; z=(z^(z>>27))*0x94d049bb133111ebULL; return z^(z>>31);}
static int unhex(const char*h, uint8_t*out){ size_t n=strlen(h)/2; for(size_t i=0;i<n;i++){unsigned b; sscanf(h+2*i,"%2x",&b); out[i]=(uint8_t)b;} return (int)n; }
static uint64_t rd64(const uint8_t*p){uint64_t v=0; for(int i=0;i<8;i++) v|=(uint64_t)p[i]<<(8*i); return v;}
static uint64_t rdz(const uint8_t*p,int n){uint64_t v=0; for(int i=0;i<n;i++) v|=(uint64_t)p[i]<<(8*i); return v;}
#define P1 0x82434fe90edcef39ULL
#define P2 0xd4f06db99d67be4bULL
#define P1INV 0xb605a92816b14f09ULL

typedef struct { const char*name; const char*h1,*h2; uint64_t ex_seed; uint64_t mask, val; } pair_t;
static const pair_t pairs[2]={
 {"A","406b68c281a55e00158ed10a0d96f8ff","d1d86241d24d9f990323e0",0x3c805cc67a687f30ULL,0x7f868a5066451822ULL,0x16808a1062000020ULL},
 {"B","9858cabfcf20f692468be82bd3","95589a09e664a742a4ac26262cc7fdfc",0xd870b802f1050157ULL,0x4d4ce1e6964c5012ULL,0x4c408040000c1012ULL}};

int main(int argc,char**argv){
  int lu = argc>1?atoi(argv[1]):26, lc = argc>2?atoi(argv[2]):24; uint64_t rs = argc>3?strtoull(argv[3],0,0):1;
  printf("t1ha2_atonce from upstream library; threads=%d\n", omp_get_max_threads());
  /* upstream self-check */
  printf("upstream t1ha_selfcheck__t1ha2: %s\n", t1ha_selfcheck__t1ha2()==0?"ok":"FAILED");
  for(int p=0;p<2;p++){
    uint8_t m1[32],m2[32]; int n1=unhex(pairs[p].h1,m1), n2=unhex(pairs[p].h2,m2);
    uint64_t h1=t1ha2_atonce(m1,n1,pairs[p].ex_seed), h2=t1ha2_atonce(m2,n2,pairs[p].ex_seed);
    printf("\npair %s: m1 %d B, m2 %d B; example seed %016llx: H1=%016llx H2=%016llx %s\n",pairs[p].name,n1,n2,(unsigned long long)pairs[p].ex_seed,(unsigned long long)h1,(unsigned long long)h2,h1==h2?"COLLIDE":"DIFFER");
    /* class parameters for m1 */
    uint64_t w0=rd64(m1), t=rdz(m1+8,n1-8);
    unsigned __int128 pr=(unsigned __int128)(w0+(uint64_t)n1)*P2; uint64_t ell=(uint64_t)pr;
    uint64_t mask=pairs[p].mask, val=pairs[p].val; int dens=__builtin_popcountll(mask);
    /* uniform seeds */
    uint64_t NU=1ULL<<lu, NC=1ULL<<lc; uint64_t coll=0, coll_in=0; uint64_t first=0; int have=0;
    #pragma omp parallel reduction(+:coll,coll_in)
    { int tid=omp_get_thread_num(); uint64_t st=rs*0x9E3779B97F4A7C15ULL ^ ((uint64_t)tid<<32) ^ (uint64_t)p; for(int i=0;i<4;i++) splitmix64(&st);
      #pragma omp for schedule(static)
      for(uint64_t i=0;i<NU;i++){ uint64_t s=splitmix64(&st); if(t1ha2_atonce(m1,n1,s)==t1ha2_atonce(m2,n2,s)){ coll++; uint64_t L=((s^ell)+t)*P1; if((L&mask)==val) coll_in++;
          #pragma omp critical
          { if(!have){first=s;have=1;} } } }
    }
    double rate=(double)coll/NU; double lo=0,hi=0; if(coll>0){ /* Poisson exact-ish 95% via chi2 approx: use Garwood */ 
      /* simple: normal approx on sqrt count for k>=10 else Poisson table not needed; print Wald + note */
      double k=coll; lo=(k-1.96*sqrt(k))/NU; hi=(k+1.96*sqrt(k))/NU; }
    printf("  uniform: N=2^%d, collisions=%llu (in-class %llu), rate=%s", lu,(unsigned long long)coll,(unsigned long long)coll_in, coll?"":"0");
    if(coll) printf("2^%.2f, approx 95%% CI [2^%.2f, 2^%.2f]", log2(rate), lo>0?log2(lo):-99, log2(hi));
    else printf(", one-sided 95%% upper bound 2^%.2f", log2(3.0/NU));
    printf("\n  first uniform colliding seed: %016llx\n", have?(unsigned long long)first:0ULL);
    /* class-conditional */
    uint64_t cc=0;
    #pragma omp parallel reduction(+:cc)
    { int tid=omp_get_thread_num(); uint64_t st=(rs+77)*0x9E3779B97F4A7C15ULL ^ ((uint64_t)tid<<32) ^ (uint64_t)p; for(int i=0;i<4;i++) splitmix64(&st);
      #pragma omp for schedule(static)
      for(uint64_t i=0;i<NC;i++){ uint64_t R=splitmix64(&st); uint64_t L=(R&~mask)|val; uint64_t s=(L*P1INV - t)^ell; if(t1ha2_atonce(m1,n1,s)==t1ha2_atonce(m2,n2,s)) cc++; }
    }
    double cr=(double)cc/NC, z=1.96, n=(double)NC, ph=cr; double den=1+z*z/n, cen=(ph+z*z/(2*n))/den, half=z*sqrt(ph*(1-ph)/n+z*z/(4*n*n))/den;
    printf("  class (density 2^-%d): N=2^%d, collisions=%llu, cond rate=2^%.3f, Wilson 95%% [2^%.3f, 2^%.3f]\n", dens, lc,(unsigned long long)cc, log2(cr), log2(cen-half), log2(cen+half));
    printf("  => class contribution 2^%.3f  [2^%.3f, 2^%.3f]\n", log2(cr)-dens, log2(cen-half)-dens, log2(cen+half)-dens);
  }
  return 0;
}
