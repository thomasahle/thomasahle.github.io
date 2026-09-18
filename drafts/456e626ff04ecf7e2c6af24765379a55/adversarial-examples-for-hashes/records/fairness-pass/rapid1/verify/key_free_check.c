/* Every-seed check of the secret[1] annihilation pair against upstream rapidhash.h (tag rapidhash_v1.0). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include "rapidhash.h"
static uint64_t st[4];
static uint64_t rotl(uint64_t x,int k){return (x<<k)|(x>>(64-k));}
static uint64_t sm(uint64_t*s){uint64_t z=(*s+=0x9e3779b97f4a7c15ull);z=(z^(z>>30))*0xbf58476d1ce4e5b9ull;z=(z^(z>>27))*0x94d049bb133111ebull;return z^(z>>31);}
static void rng_init(uint64_t s){for(int i=0;i<4;i++)st[i]=sm(&s);}
static uint64_t next(void){uint64_t r=rotl(st[1]*5,7)*9,t=st[1]<<17;st[2]^=st[0];st[3]^=st[1];st[1]^=st[2];st[0]^=st[3];st[2]^=t;st[3]=rotl(st[3],45);return r;}
static void hexdec(const char*s,uint8_t*o){for(size_t i=0;i<strlen(s)/2;i++){unsigned x;sscanf(s+2*i,"%2x",&x);o[i]=x;}}
int main(int argc,char**argv){
  unsigned lg=argc>1?atoi(argv[1]):20;
  const char* P[2][2]={
   {"0000000000000000c9ac2e96934bb88b0000000000000000","0100000000000000c9ac2e96934bb88b0000000000000000"},            /* 24 B, L=3, differ in w0 */
   {"9bd4604137366abec9ac2e96934bb88b88d35499de169df633e0964e8c04600c","642b9fbec8c9954100000000000000000000000000000000"} /* 32 B vs 24 B: cross length, w2 vs w1 = secret[1] */
  };
  for(int k=0;k<2;k++){
    uint8_t a[64],b[64]; size_t na=strlen(P[k][0])/2, nb=strlen(P[k][1])/2; hexdec(P[k][0],a); hexdec(P[k][1],b);
    uint64_t seeds[3]={0,1,0x3788f2419a81e2d6ull};
    for(int i=0;i<3;i++) printf("pair %d seed %016" PRIx64 ": H(M)=%016" PRIx64 " H(M')=%016" PRIx64 "\n",k,seeds[i],rapidhash_withSeed(a,na,seeds[i]),rapidhash_withSeed(b,nb,seeds[i]));
    rng_init(1); uint64_t n=1ull<<lg, c=0; for(uint64_t t=0;t<n;t++){uint64_t s=next(); c+= rapidhash_withSeed(a,na,s)==rapidhash_withSeed(b,nb,s);}
    printf("pair %d (%zu B / %zu B): %" PRIu64 " / 2^%u random seeds collide\n",k,na,nb,c,lg);
  }
  return 0;
}
