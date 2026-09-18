/* Independent check of the t1ha2-64 fixed pairs against the UPSTREAM library
 * (erthink/t1ha HEAD, src/t1ha2.c linked as-is).  Usage: pair_check U C R
 *   U = log2(uniform seeds), C = log2(class seeds), R = rng seed.            */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <inttypes.h>
#include <omp.h>
#include "t1ha.h"

extern int t1ha_selfcheck__t1ha2(void);

#define P1 UINT64_C(0x82434FE90EDCEF39)
#define P2 UINT64_C(0xD4F06DB99D67BE4B)

typedef struct { const char *name, *m1, *m2; uint64_t lmask, lval, ex_seed, ex_hash; } pair_t;
static const pair_t PAIRS[] = {
  {"A 16/11", "406b68c281a55e00158ed10a0d96f8ff", "d1d86241d24d9f990323e0",
   UINT64_C(0x7f868a5066451822), UINT64_C(0x16808a1062000020),
   UINT64_C(0x3c805cc67a687f30), UINT64_C(0xf2a1196d24fddaab)},
  {"B 13/16", "9858cabfcf20f692468be82bd3", "95589a09e664a742a4ac26262cc7fdfc",
   UINT64_C(0x4d4ce1e6964c5012), UINT64_C(0x4c408040000c1012),
   UINT64_C(0xd870b802f1050157), UINT64_C(0x63cf67ba079a2997)},
};

static size_t unhex(const char *s, uint8_t *o){size_t n=strlen(s)/2;for(size_t i=0;i<n;i++){unsigned v;sscanf(s+2*i,"%2x",&v);o[i]=(uint8_t)v;}return n;}
static uint64_t splitmix64(uint64_t z){z+=UINT64_C(0x9E3779B97F4A7C15);z=(z^(z>>30))*UINT64_C(0xBF58476D1CE4E5B9);z=(z^(z>>27))*UINT64_C(0x94D049BB133111EB);return z^(z>>31);}
static uint64_t rd64(const uint8_t *p){uint64_t v=0;for(int i=7;i>=0;i--)v=(v<<8)|p[i];return v;}
static uint64_t tailz(const uint8_t *p,size_t n){uint64_t v=0;for(size_t i=n;i-->0;)v=(v<<8)|p[i];return v;}
static int popc(uint64_t x){int c=0;for(;x;x&=x-1)c++;return c;}
static double l2(uint64_t h,uint64_t n){return h?log2((double)h/(double)n):-INFINITY;}
static void wilson(uint64_t h,uint64_t n,double *lo,double *hi){double z=1.96,p=(double)h/n,d=1+z*z/n,c=p+z*z/(2*n),r=z*sqrt(p*(1-p)/n+z*z/(4.0*n*n));*lo=(c-r)/d;*hi=(c+r)/d;}

int main(int argc,char**argv){
  int U=argc>1?atoi(argv[1]):24, C=argc>2?atoi(argv[2]):24; uint64_t R=argc>3?strtoull(argv[3],0,10):1;
  printf("upstream selfcheck t1ha2: %s\n", t1ha_selfcheck__t1ha2()==0?"ok":"FAIL");
  printf("threads: %d  uniform 2^%d  class 2^%d  rng %" PRIu64 "\n", omp_get_max_threads(),U,C,R);
  uint64_t p1inv=P1; for(int i=0;i<6;i++)p1inv*=2-P1*p1inv; if(p1inv*P1!=1){puts("inv fail");return 1;}
  for(size_t pi=0;pi<2;pi++){
    const pair_t*p=&PAIRS[pi]; uint8_t m1[16],m2[16]; size_t n1=unhex(p->m1,m1), n2=unhex(p->m2,m2);
    unsigned __int128 pr=(unsigned __int128)((uint64_t)n1+rd64(m1))*P2; uint64_t l0=(uint64_t)pr, t=tailz(m1+8,n1-8);
    #define LOF(seed) (((seed)^l0)+t)*P1
    #define SEEDOF(L) ((((L)*p1inv)-t)^l0)
    printf("\npair %s: m1 %zu B, m2 %zu B, class bits %d\n", p->name,n1,n2,popc(p->lmask));
    uint64_t h1=t1ha2_atonce(m1,n1,p->ex_seed), h2=t1ha2_atonce(m2,n2,p->ex_seed);
    printf("  example seed %016" PRIx64 ": H(m1)=%016" PRIx64 " H(m2)=%016" PRIx64 " %s (expected %016" PRIx64 ") in class: %s\n",
      p->ex_seed,h1,h2,h1==h2?"COLLIDE":"DIFFER",p->ex_hash,((LOF(p->ex_seed))&p->lmask)==p->lval?"yes":"no");
    /* sanity: seed -> L -> seed round trip */
    if(SEEDOF(LOF(p->ex_seed))!=p->ex_seed){puts("  L map not invertible?!");return 1;}
    /* uniform seeds */
    uint64_t N=UINT64_C(1)<<U, hits=0, inclass=0, firsts[8]; int nf=0;
    double t0=omp_get_wtime();
    #pragma omp parallel for schedule(static) reduction(+:hits,inclass)
    for(uint64_t i=0;i<N;i++){ uint64_t s=splitmix64((R<<48)^(pi<<40)^i);
      if(t1ha2_atonce(m1,n1,s)==t1ha2_atonce(m2,n2,s)){ hits++; if(((LOF(s))&p->lmask)==p->lval)inclass++;
        #pragma omp critical
        { if(nf<8)firsts[nf++]=s; } } }
    double lo,hi; wilson(hits,N,&lo,&hi);
    printf("  uniform: N=2^%d hits=%" PRIu64 " (in class %" PRIu64 ") rate 2^%.2f  Wilson95 [2^%.2f, 2^%.2f]  expected@2^-30: %.1f  (%.0fs)\n",
      U,hits,inclass,l2(hits,N),log2(lo),log2(hi),(double)N/pow(2,30),omp_get_wtime()-t0);
    for(int k=0;k<nf;k++)printf("    hit seed %016" PRIx64 "\n",firsts[k]);
    /* class-conditional */
    uint64_t M=UINT64_C(1)<<C, ch=0; t0=omp_get_wtime();
    #pragma omp parallel for schedule(static) reduction(+:ch)
    for(uint64_t i=0;i<M;i++){ uint64_t L=(splitmix64((R<<48)^(pi<<40)^(UINT64_C(1)<<47)^i)&~p->lmask)|p->lval; uint64_t s=SEEDOF(L);
      ch+= t1ha2_atonce(m1,n1,s)==t1ha2_atonce(m2,n2,s); }
    wilson(ch,M,&lo,&hi);
    printf("  class: N=2^%d hits=%" PRIu64 " cond rate 2^%.3f [2^%.3f, 2^%.3f] => contribution 2^%.3f [2^%.3f, 2^%.3f]  (%.0fs)\n",
      C,ch,l2(ch,M),log2(lo),log2(hi),l2(ch,M)-popc(p->lmask),log2(lo)-popc(p->lmask),log2(hi)-popc(p->lmask),omp_get_wtime()-t0);
  }
  return 0;
}
