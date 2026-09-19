// Empirical heaviness of S2_init = hi64(X*Y) (the second initial state word) at a given length:
// 2^N uniform seeds, values radix-sorted, maximal multiplicity reported. Resolves Pr >~ 2^-(N-3).
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include "a5.h"
static uint64_t rng(uint64_t *s){ uint64_t z=(*s+=0x9E3779B97F4A7C15ull); z=(z^(z>>30))*0xBF58476D1CE4E5B9ull; z=(z^(z>>27))*0x94D049BB133111EBull; return z^(z>>31); }
static uint64_t *A,*B; static uint64_t N, LEN; static int NT=8, WHICH=2;
typedef struct{ uint64_t i0,i1; uint64_t rs; } job_t;
static void *fill(void *a){ job_t *j=a; for(uint64_t i=j->i0;i<j->i1;i++){ uint64_t s1,s2; a5_init(LEN,rng(&j->rs),&s1,&s2); A[i]=WHICH==2?s2:s1; } return 0; }
static int cmp(const void *x,const void *y){ uint64_t a=*(const uint64_t*)x,b=*(const uint64_t*)y; return a<b?-1:a>b; }
int main(int argc,char**argv){ LEN=strtoull(argv[1],0,0); int n=atoi(argv[2]); if(argc>3) NT=atoi(argv[3]); if(argc>4) WHICH=atoi(argv[4]); N=1ull<<n;
  A=malloc(N*8); B=malloc(N*8); pthread_t th[64]; job_t j[64];
  for(int t=0;t<NT;t++){ j[t].i0=N*t/NT; j[t].i1=N*(t+1)/NT; j[t].rs=0x5EED0000+t*7919+LEN; pthread_create(&th[t],0,fill,&j[t]); } for(int t=0;t<NT;t++) pthread_join(th[t],0);
  // LSD radix sort, 16 bits x 4
  for(int pass=0;pass<4;pass++){ static uint64_t cnt[65536]; for(int i=0;i<65536;i++)cnt[i]=0; int sh=16*pass;
    for(uint64_t i=0;i<N;i++) cnt[(A[i]>>sh)&0xffff]++; uint64_t acc=0; for(int i=0;i<65536;i++){ uint64_t c=cnt[i]; cnt[i]=acc; acc+=c; }
    for(uint64_t i=0;i<N;i++) B[cnt[(A[i]>>sh)&0xffff]++]=A[i]; uint64_t *t=A;A=B;B=t; }
  uint64_t best=1,run=1,bv=A[0]; uint64_t ndup=0; for(uint64_t i=1;i<N;i++){ if(A[i]==A[i-1]){ run++; ndup++; if(run>best){best=run;bv=A[i];} } else run=1; }
  printf("len %llu S%d_init: 2^%d seeds, max multiplicity %llu (value 0x%016llx), duplicate pairs %llu; a value with Pr=2^-%d would show ~%g hits\n",(unsigned long long)LEN,WHICH,n,(unsigned long long)best,(unsigned long long)bv,(unsigned long long)ndup,n-3,8.0);
  return 0; }
