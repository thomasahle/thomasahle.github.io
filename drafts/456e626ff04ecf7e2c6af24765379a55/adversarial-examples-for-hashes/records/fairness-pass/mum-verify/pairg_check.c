/* Independent check of pair G against upstream mum.h (whatever MUM_H points at). */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include MUM_H
static uint64_t s[4];
static inline uint64_t rotl(uint64_t x,int k){return (x<<k)|(x>>(64-k));}
static uint64_t xo(void){uint64_t r=rotl(s[1]*5,7)*9,t=s[1]<<17;s[2]^=s[0];s[3]^=s[1];s[1]^=s[2];s[0]^=s[3];s[2]^=t;s[3]=rotl(s[3],45);return r;}
static uint64_t sm(uint64_t*x){uint64_t z=(*x+=0x9e3779b97f4a7c15ULL);z=(z^(z>>30))*0xbf58476d1ce4e5b9ULL;z=(z^(z>>27))*0x94d049bb133111ebULL;return z^(z>>31);}
int main(int argc,char**argv){
  int lg=argc>1?atoi(argv[1]):20; uint64_t sd=argc>2?strtoull(argv[2],0,0):1;
  const uint8_t A[8]={0x79,0x54,0xa9,0x98,0x71,0x9b,0x89,0xb0};
  const uint8_t B[8]={0x2d,0x69,0x16,0x9b,0x25,0x9f,0xeb,0x08};
  uint64_t x=sd; for(int i=0;i<4;i++) s[i]=sm(&x);
  uint64_t h0=mum_hash(A,8,0), h1=mum_hash(B,8,0);
  printf("seed0: %016llx %016llx %s\n",(unsigned long long)h0,(unsigned long long)h1,h0==h1?"EQ":"NE");
  uint64_t n=1ULL<<lg,c=0;
  for(uint64_t i=0;i<n;i++){uint64_t k=xo(); if(mum_hash(A,8,k)==mum_hash(B,8,k)) c++;}
  printf("CONFIG=%s unroll=%d collisions %llu/%llu\n",CFG,(int)_MUM_UNROLL_FACTOR,(unsigned long long)c,(unsigned long long)n);
  return 0;
}
