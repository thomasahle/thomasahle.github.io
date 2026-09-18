#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "k527.h"
static uint64_t xs=0x9E3779B97F4A7C15ull; static uint64_t rnd(void){xs^=xs<<13;xs^=xs>>7;xs^=xs<<17;return xs;}
uint64_t h534(const void*,size_t,uint64_t);
static int unhex(const char*s,unsigned char*o){size_t n=strlen(s)/2;for(size_t i=0;i<n;i++){unsigned v;sscanf(s+2*i,"%2x",&v);o[i]=v;}return (int)n;}
int main(void){unsigned char buf[600];long mism=0,N=200000;
 for(long t=0;t<N;t++){size_t n=rnd()%600;for(size_t i=0;i<n;i++)buf[i]=rnd();uint64_t s=(t&1)?rnd():(rnd()&0xffff);
  if(komihash(buf,n,s)!=h534(buf,n,s))mism++;}
 printf("5.27 (a602815) vs 5.34 (HEAD): %ld random inputs len 0..599, mismatches %ld\n",N,mism);
 unsigned char A[64],B[64];unhex("0000000000000000447370032e8a191311111111111111112222222222222222f0f9dda4c1c0a15e9cf534900ea6f5e033333333333333334444444444444444",A);
 unhex("0100000000000000457370032e8a191311111111111111112222222222222222f0f9dda4c1c0a15e9cf534900ea6f5e033333333333333334444444444444444",B);
 uint64_t sd=0x27d1f77dc2a01269ull;printf("5.27: h(A)=%016llx h(B)=%016llx\n",(unsigned long long)komihash(A,64,sd),(unsigned long long)komihash(B,64,sd));
 printf("5.34: h(A)=%016llx h(B)=%016llx\n",(unsigned long long)h534(A,64,sd),(unsigned long long)h534(B,64,sd));
 long c=0,M=1<<22;for(long t=0;t<M;t++){uint64_t s=rnd();if(komihash(A,64,s)==komihash(B,64,s))c++;}
 printf("5.27 header, pair rate over 2^22 seeds: %ld/%ld = %.6f\n",c,M,(double)c/M);return 0;}
