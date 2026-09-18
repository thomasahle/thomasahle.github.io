/* Full-API measurement of the structured pair {(k0,~k1) | tail} vs {(~k0,k1) | tail}, k0,k1 = first two
 * little-endian words of XXH3_kSecret, zero tail, lengths 32 and 128, xoshiro256** seeds (two logical workers). */
#define XXH_INLINE_ALL
#include "xxHash/xxhash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
static uint64_t st[4];
static uint64_t rot(uint64_t x,unsigned n){return x<<n|x>>(64-n);}
static uint64_t sm(uint64_t*s){uint64_t z=(*s+=0x9e3779b97f4a7c15ULL);z=(z^(z>>30))*0xbf58476d1ce4e5b9ULL;z=(z^(z>>27))*0x94d049bb133111ebULL;return z^(z>>31);}
static void init(uint64_t s){for(int i=0;i<4;i++)st[i]=sm(&s);}
static uint64_t nx(void){uint64_t r=rot(st[1]*5,7)*9,t=st[1]<<17;st[2]^=st[0];st[3]^=st[1];st[1]^=st[2];st[0]^=st[3];st[2]^=t;st[3]=rot(st[3],45);return r;}
__attribute__((noinline,noipa)) static uint64_t api(const uint8_t*p,size_t n,uint64_t s){return XXH3_64bits_withSeed(p,n,s);}
int main(int argc,char**argv){
  unsigned lg=argc>1?atoi(argv[1]):20; uint64_t salt=argc>2?strtoull(argv[2],0,0):0x2026091804000477ULL; uint64_t n=1ULL<<lg;
  uint64_t k0=XXH_readLE64(XXH3_kSecret),k1=XXH_readLE64(XXH3_kSecret+8);
  uint8_t a[128]={0},b[128]={0};
  for(int i=0;i<8;i++){a[i]=k0>>(8*i);a[8+i]=(~k1)>>(8*i);b[i]=~a[i];b[8+i]=~a[8+i];}
  printf("XXH_VERSION_NUMBER=%d k0=%016" PRIx64 " k1=%016" PRIx64 "\nM  = ",(int)XXH_VERSION_NUMBER,k0,k1);
  for(int i=0;i<32;i++)printf("%02x",a[i]); printf("\nM2 = "); for(int i=0;i<32;i++)printf("%02x",b[i]); puts("");
  printf("seed 0: H(M)=%016" PRIx64 " H(M2)=%016" PRIx64 "\n",api(a,32,0),api(b,32,0));
  printf("sampling trials=%" PRIu64 " salt=0x%016" PRIx64 "\n",n,salt);
  uint64_t c32=0,c128=0,mis=0,shown=0;
  for(unsigned w=0;w<2;w++){ init(salt*0x9e3779b97f4a7c15ULL+7919ULL*w+1);
    for(uint64_t i=0;i<n/2;i++){uint64_t s=nx();
      uint64_t x=api(a,32,s),y=api(b,32,s),u=api(a,128,s),v=api(b,128,s);
      int h32=(x==y),h128=(u==v); c32+=h32;c128+=h128;mis+=(h32!=h128);
      if(h32&&shown<3){shown++;printf("witness seed=%016" PRIx64 " len32 H=%016" PRIx64 " len128 H=%016" PRIx64 "\n",s,x,u);}
    }}
  printf("summary len=32 L=4 collisions=%" PRIu64 "/%" PRIu64 " log2eps=%.4f score=log2(4N/c)=%.4f\n",c32,n,log2((double)c32/n),log2(4.0*n/c32));
  printf("summary len=128 L=16 collisions=%" PRIu64 "/%" PRIu64 " log2eps=%.4f score=%.4f\n",c128,n,log2((double)c128/n),log2(16.0*n/c128));
  printf("event_mismatch_32_128=%" PRIu64 " %s\n",mis,mis?"FAIL":"PASS"); return mis?1:0; }
