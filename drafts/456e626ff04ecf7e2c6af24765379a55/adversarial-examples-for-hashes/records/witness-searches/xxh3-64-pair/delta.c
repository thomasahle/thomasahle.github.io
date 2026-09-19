/* P[fold(A,~b)==fold(~A,b)] for A uniform and b = A ^ delta, per single-bit delta (and control). */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
static inline uint64_t fold(uint64_t a, uint64_t b){unsigned __int128 p=(unsigned __int128)a*b;return (uint64_t)p^(uint64_t)(p>>64);}
static uint64_t st=0x9e3779b97f4a7c15ULL;
static inline uint64_t sm(void){uint64_t z=(st+=0x9e3779b97f4a7c15ULL);z=(z^(z>>30))*0xbf58476d1ce4e5b9ULL;z=(z^(z>>27))*0x94d049bb133111ebULL;return z^(z>>31);}
int main(int argc,char**argv){int lg=argc>1?atoi(argv[1]):24; uint64_t N=1ULL<<lg;
 for(int i=-1;i<64;i++){ uint64_t d=i<0?0:(1ULL<<i), c=0, c2=0;
   for(uint64_t k=0;k<N;k++){uint64_t A=sm(); uint64_t b=i<0?sm():A^d; c+=(fold(A,~b)==fold(~A,b)); if(i>=0){uint64_t b2=A^d^(d<<1); c2+=(fold(A,~b2)==fold(~A,b2));}}
   printf("delta=%s%d count=%llu log2=%.3f  adjacent-pair(i,i+1) count=%llu log2=%.3f\n", i<0?"random ":"2^", i<0?0:i,(unsigned long long)c, c?log2((double)c/N):-99.0,(unsigned long long)c2,c2?log2((double)c2/N):-99.0); fflush(stdout);}
 return 0;}
