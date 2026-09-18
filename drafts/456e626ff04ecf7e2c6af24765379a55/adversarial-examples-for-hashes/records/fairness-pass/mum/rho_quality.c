/* Brent rho on the MUM_QUALITY single-word public term f(w) = _mum(w + p0, p0), then verify an 8-byte pair over 2^20 seeds. */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#define MUM_QUALITY 1
#include "mum_head_595c091.h"
static uint64_t f(uint64_t w){ return _mum(w + _mum_primes[0], _mum_primes[0]); }
static uint64_t s[4];
static uint64_t rotl(uint64_t x,int k){return (x<<k)|(x>>(64-k));}
static uint64_t sm(uint64_t *x){uint64_t z=(*x+=0x9e3779b97f4a7c15ULL);z=(z^(z>>30))*0xbf58476d1ce4e5b9ULL;z=(z^(z>>27))*0x94d049bb133111ebULL;return z^(z>>31);}
static uint64_t nx(void){uint64_t r=rotl(s[1]*5,7)*9,t=s[1]<<17;s[2]^=s[0];s[3]^=s[1];s[1]^=s[2];s[0]^=s[3];s[2]^=t;s[3]=rotl(s[3],45);return r;}
int main(int argc,char**argv){
  uint64_t x0 = argc>1?strtoull(argv[1],0,0):1;
  uint64_t power=1, lam=1, tort=x0, hare=f(x0), steps=1;
  while(tort!=hare){ if(power==lam){tort=hare;power*=2;lam=0;} hare=f(hare); lam++; steps++; }
  tort=hare=x0; for(uint64_t i=0;i<lam;i++) hare=f(hare);
  uint64_t mu=0; while(tort!=hare){ tort=f(tort); hare=f(hare); mu++; }
  /* now tort==hare==x_mu; predecessors: x_{mu-1} and x_{mu+lam-1} */
  if(mu==0){ printf("start on cycle; retry another start\n"); return 2; }
  uint64_t a=x0; for(uint64_t i=0;i+1<mu;i++) a=f(a);
  uint64_t b=a; for(uint64_t i=0;i<lam;i++) b=f(b);
  printf("rho: steps~2^%.1f lam=%llu mu=%llu w=%016llx w'=%016llx f(w)=%016llx f(w')=%016llx %s\n", __builtin_log2((double)(steps+lam+2*mu)),
    (unsigned long long)lam,(unsigned long long)mu,(unsigned long long)a,(unsigned long long)b,(unsigned long long)f(a),(unsigned long long)f(b), (a!=b&&f(a)==f(b))?"OK":"BAD");
  if(a==b||f(a)!=f(b)) return 1;
  uint8_t A[8],B[8]; for(int i=0;i<8;i++){A[i]=a>>(8*i);B[i]=b>>(8*i);}
  printf("M  = "); for(int i=0;i<8;i++)printf("%02x",A[i]); printf("\nM' = "); for(int i=0;i<8;i++)printf("%02x",B[i]); printf("\n");
  printf("seed0: H(M)=%016llx H(M')=%016llx\n",(unsigned long long)mum_hash(A,8,0),(unsigned long long)mum_hash(B,8,0));
  uint64_t rs=1; for(int i=0;i<4;i++) s[i]=sm(&rs);
  uint64_t n=1ULL<<20,c=0; for(uint64_t t=0;t<n;t++){uint64_t sd=nx(); c+= mum_hash(A,8,sd)==mum_hash(B,8,sd);}
  printf("MUM_QUALITY (HEAD 595c091) collisions = %llu / %llu\n",(unsigned long long)c,(unsigned long long)n);
  return c==n?0:1;
}
