/* Pair G check against upstream mum.h (whatever version is included via MUMH). */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include MUMH
static uint64_t s[4];
static uint64_t rotl(uint64_t x,int k){return (x<<k)|(x>>(64-k));}
static uint64_t sm(uint64_t *x){uint64_t z=(*x+=0x9e3779b97f4a7c15ULL);z=(z^(z>>30))*0xbf58476d1ce4e5b9ULL;z=(z^(z>>27))*0x94d049bb133111ebULL;return z^(z>>31);}
static uint64_t nx(void){uint64_t r=rotl(s[1]*5,7)*9,t=s[1]<<17;s[2]^=s[0];s[3]^=s[1];s[1]^=s[2];s[0]^=s[3];s[2]^=t;s[3]=rotl(s[3],45);return r;}
int main(int argc,char**argv){
  unsigned lg=argc>1?atoi(argv[1]):20; uint64_t rs=argc>2?strtoull(argv[2],0,0):1;
  uint8_t a[8]={0x79,0x54,0xa9,0x98,0x71,0x9b,0x89,0xb0}, b[8]={0x2d,0x69,0x16,0x9b,0x25,0x9f,0xeb,0x08};
  uint64_t h0a=mum_hash(a,8,0), h0b=mum_hash(b,8,0);
  printf("%s seed0: H(A)=%016llx H(B)=%016llx %s\n",CFG,(unsigned long long)h0a,(unsigned long long)h0b,h0a==h0b?"EQUAL":"DIFF");
  for(int i=0;i<4;i++) s[i]=sm(&rs);
  uint64_t n=1ULL<<lg, c=0, bad=0;
  for(uint64_t t=0;t<n;t++){uint64_t sd=nx(); if(mum_hash(a,8,sd)==mum_hash(b,8,sd)) c++; else if(bad++<3) printf("  non-colliding seed %016llx\n",(unsigned long long)sd);}
  printf("%s collisions = %llu / %llu\n",CFG,(unsigned long long)c,(unsigned long long)n);
  return c==n?0:1;
}
