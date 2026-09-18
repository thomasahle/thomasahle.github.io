/* Independent re-check of the a5wide pair against the upstream a5hash.h in ../a5hash. */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "a5hash/a5hash.h"
static uint64_t sm64(uint64_t*s){uint64_t z=(*s+=0x9E3779B97F4A7C15ULL);z=(z^(z>>30))*0xBF58476D1CE4E5B9ULL;z=(z^(z>>27))*0x94D049BB133111EBULL;return z^(z>>31);}
static void h128(const uint8_t*m,size_t n,uint64_t seed,uint64_t*lo,uint64_t*hi){uint64_t r=0;*lo=a5hash128(m,n,seed,&r);*hi=r;}
int main(int argc,char**argv){
  int k=argc>1?atoi(argv[1]):24; uint64_t rng=argc>2?strtoull(argv[2],0,0):0x1234567887654321ULL;
  const uint8_t A[17]={0x00,0xdd,0xc7,0xf6,0x5b,0x30,0xce,0x60,0xd6,0,0,0,0,0,0,0,0x00};
  uint8_t B[17]; memcpy(B,A,17); B[16]=0xff;
  uint8_t C[17]; memcpy(C,A,17); C[15]=0xff; /* control: byte 15 changed */
  printf("a5hash.h version %s\n",A5HASH_VER_STR);
  /* algebraic check: c = lu32(M+1)<<32 | lu32(M+5) */
  uint32_t l1,l5; memcpy(&l1,A+1,4); memcpy(&l5,A+5,4);
  uint64_t c=((uint64_t)l1<<32)|l5; printf("c=%016llx  c+Seed3=%016llx\n",(unsigned long long)c,(unsigned long long)(c+0xA4093822299F31D0ULL));
  uint64_t lo,hi,lo2,hi2; h128(A,17,0x5f642f87d5e23888ULL,&lo,&hi); h128(B,17,0x5f642f87d5e23888ULL,&lo2,&hi2);
  printf("example seed 5f642f87d5e23888: A=%016llx:%016llx B=%016llx:%016llx %s\n",(unsigned long long)hi,(unsigned long long)lo,(unsigned long long)hi2,(unsigned long long)lo2,(lo==lo2&&hi==hi2)?"COLLIDE":"DIFFER");
  uint64_t structured[8]={0,1,~0ULL,0x8000000000000000ULL,0x5555555555555555ULL,0xaaaaaaaaaaaaaaaaULL,0xA4093822299F31D0ULL,0x243f6a8885a308d3ULL}; int sc=0;
  for(int i=0;i<8;i++){h128(A,17,structured[i],&lo,&hi);h128(B,17,structured[i],&lo2,&hi2);sc+=(lo==lo2&&hi==hi2);}
  printf("structured seeds %d/8 collide\n",sc);
  uint64_t N=1ULL<<k, col=0, ctl=0, c64=0;
  for(uint64_t i=0;i<N;i++){uint64_t s=sm64(&rng);
    h128(A,17,s,&lo,&hi);h128(B,17,s,&lo2,&hi2); col+=(lo==lo2&&hi==hi2);
    uint64_t lo3,hi3; h128(C,17,s,&lo3,&hi3); ctl+=(lo==lo3&&hi==hi3);
    c64+=(a5hash(A,17,s)==a5hash(B,17,s)); }
  printf("uniform seeds N=2^%d: a5hash128 full 128-bit collisions %llu/%llu; control (byte 15 changed) %llu/%llu; 64-bit a5hash(A,B) %llu/%llu\n",k,(unsigned long long)col,(unsigned long long)N,(unsigned long long)ctl,(unsigned long long)N,(unsigned long long)c64,(unsigned long long)N);
  return col==N?0:1; }
