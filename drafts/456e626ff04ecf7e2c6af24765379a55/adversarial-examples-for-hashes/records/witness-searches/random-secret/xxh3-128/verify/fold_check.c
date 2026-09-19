/* fold_check.c -- Monte Carlo of the hash-independent event behind pair F under a uniform secret:
 * Pr[fold(A,B) == fold(~A,~B)] for independent uniform 64-bit A,B, fold(a,b) = lo64(a*b) ^ hi64(a*b).
 * Also counts the 2^63 twin (fold values differing exactly in the top bit).
 * usage: fold_check <log2N> <rngseed> <threads>;  build: cc -O2 -pthread -o fold_check fold_check.c -lm */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <math.h>
typedef struct { unsigned __int128 s; uint64_t n, hits, twin; } w_t;
static uint64_t sm(uint64_t *x){ uint64_t z=(*x+=0x9e3779b97f4a7c15ULL); z=(z^(z>>30))*0xbf58476d1ce4e5b9ULL; z=(z^(z>>27))*0x94d049bb133111ebULL; return z^(z>>31); }
static void *run(void *a){ w_t *w=a; for(uint64_t i=0;i<w->n;i++){ w->s*=0xda942042e4dd58b5ULL; uint64_t A=(uint64_t)(w->s>>64); w->s*=0xda942042e4dd58b5ULL; uint64_t B=(uint64_t)(w->s>>64);
  unsigned __int128 p=(unsigned __int128)A*B, q=(unsigned __int128)(~A)*(~B); uint64_t f=(uint64_t)p^(uint64_t)(p>>64), g=(uint64_t)q^(uint64_t)(q>>64);
  if(f==g) w->hits++; else if((f^g)==0x8000000000000000ULL) w->twin++; } return NULL; }
int main(int argc,char**argv){ int l=atoi(argv[1]); uint64_t seed=strtoull(argv[2],0,0); int T=atoi(argv[3]); uint64_t N=1ULL<<l;
  w_t w[64]; pthread_t th[64]; for(int t=0;t<T;t++){ uint64_t x=seed*7919+t*104729+11; uint64_t a=sm(&x),b=sm(&x); w[t].s=((unsigned __int128)a<<64)|(b|1); w[t].n=N/T; w[t].hits=w[t].twin=0; pthread_create(&th[t],0,run,&w[t]); }
  uint64_t h=0,tw=0; for(int t=0;t<T;t++){ pthread_join(th[t],0); h+=w[t].hits; tw+=w[t].twin; }
  printf("fold complement event: hits=%llu twin=%llu N=2^%d rate=2^%.4f ((3/4)^64 = 2^-26.56)\n",(unsigned long long)h,(unsigned long long)tw,l,log2((double)h/(double)N)); return 0; }
