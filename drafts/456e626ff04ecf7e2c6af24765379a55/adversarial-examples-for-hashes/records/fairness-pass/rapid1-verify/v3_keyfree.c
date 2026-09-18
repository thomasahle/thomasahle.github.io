#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include "rapidhash.h"
int main(void){ uint64_t s1=0x8bb84b93962eacc9ull; uint8_t M[24]={0},M2[24]={0}; memcpy(M+8,&s1,8); memcpy(M2+8,&s1,8); M2[0]=1;
  uint64_t st=0x1234567ull; int c=0,n=1<<20; uint64_t out=rapidhash_withSeed(M,24,0); int constant=1;
  for(int t=0;t<n;t++){ st^=st<<13; st^=st>>7; st^=st<<17; uint64_t x=rapidhash_withSeed(M,24,st),y=rapidhash_withSeed(M2,24,st); c+=(x==y); constant&=(x==out);}
  printf("master (v3) w[1]=secret[1] pair at 24 B: %d / 2^20 collide, constant %s (%016" PRIx64 ")\n",c,constant?"yes":"no",out); return 0; }
