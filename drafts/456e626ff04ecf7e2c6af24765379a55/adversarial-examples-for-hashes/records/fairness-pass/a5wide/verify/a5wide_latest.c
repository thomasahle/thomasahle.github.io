/* a5wide pair check against upstream a5hash.h (whatever version is in ../a5hash). */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "../a5hash/a5hash.h"
static uint64_t sm(uint64_t *s){uint64_t z=(*s+=0x9e3779b97f4a7c15ULL);z=(z^(z>>30))*0xbf58476d1ce4e5b9ULL;z=(z^(z>>27))*0x94d049bb133111ebULL;return z^(z>>31);}
static void h128(const uint8_t*m,size_t n,uint64_t seed,uint64_t*lo,uint64_t*hi){*hi=0;*lo=a5hash128(m,n,seed,hi);}
int main(int argc,char**argv){
  int lg = argc>1?atoi(argv[1]):26;
  const uint8_t A[17]={0x00,0xdd,0xc7,0xf6,0x5b,0x30,0xce,0x60,0xd6,0,0,0,0,0,0,0,0x00};
  const uint8_t B[17]={0x00,0xdd,0xc7,0xf6,0x5b,0x30,0xce,0x60,0xd6,0,0,0,0,0,0,0,0xff};
  uint8_t C[17]; memcpy(C,A,17); C[15]=0xff; /* control: byte 15 (read by c) changed */
  printf("a5hash.h version %s\n", A5HASH_VER_STR);
  uint64_t lo,hi,lo2,hi2;
  h128(A,17,0x5f642f87d5e23888ULL,&lo,&hi); h128(B,17,0x5f642f87d5e23888ULL,&lo2,&hi2);
  printf("example seed 5f642f87d5e23888: A=%016llx:%016llx B=%016llx:%016llx %s\n",(unsigned long long)hi,(unsigned long long)lo,(unsigned long long)hi2,(unsigned long long)lo2,(lo==lo2&&hi==hi2)?"COLLIDE":"differ");
  const uint64_t special[]={0,~0ULL,0x5555555555555555ULL,0xaaaaaaaaaaaaaaaaULL,1,0x8000000000000000ULL,0xA4093822299F31D0ULL,0xC0AC29B7C97C50DDULL};
  int sp=0; for(unsigned i=0;i<sizeof special/8;i++){h128(A,17,special[i],&lo,&hi);h128(B,17,special[i],&lo2,&hi2);sp+=(lo==lo2&&hi==hi2);}
  printf("structured seeds: %d/%zu collide\n",sp,sizeof special/8);
  uint64_t st=0xA5A5A5A5C0FFEE00ULL; uint64_t N=1ULL<<lg, coll=0,collc=0,coll64=0;
  for(uint64_t i=0;i<N;i++){uint64_t s=sm(&st);
    h128(A,17,s,&lo,&hi);h128(B,17,s,&lo2,&hi2); coll+=(lo==lo2&&hi==hi2);
    h128(C,17,s,&lo2,&hi2); collc+=(lo==lo2&&hi==hi2);
    coll64+=(a5hash(A,17,s)==a5hash(B,17,s)); }
  printf("uniform seeds N=2^%d: pair(A,B) a5hash128 full 128-bit collisions %llu/%llu; control(A,C) %llu/%llu; 64-bit a5hash(A,B) %llu/%llu\n",lg,(unsigned long long)coll,(unsigned long long)N,(unsigned long long)collc,(unsigned long long)N,(unsigned long long)coll64,(unsigned long long)N);
  return (coll==N && sp==(int)(sizeof special/8))?0:1; }
