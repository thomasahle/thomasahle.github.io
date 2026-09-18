/* Bounded screen: Pair-F family for XXH3-128 at 32 B. M = (w0, ~w0, t2, t3), M' = (~w0, w0, t2, t3).
 * For each random base w0, count seeds (xoshiro256** stream) where XXH3_128bits_withSeed(M) == (M').
 * Usage: scan base_start n_bases log2_seeds stream_salt [w0_hex ...]  (explicit w0 list overrides random bases) */
#define XXH_INLINE_ALL
#include "upstream/xxhash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
static uint64_t st[4];
static uint64_t rot(uint64_t x,unsigned n){return x<<n|x>>(64-n);}
static uint64_t splitmix(uint64_t *s){uint64_t z=(*s+=UINT64_C(0x9e3779b97f4a7c15));z=(z^(z>>30))*UINT64_C(0xbf58476d1ce4e5b9);z=(z^(z>>27))*UINT64_C(0x94d049bb133111eb);return z^(z>>31);}
static void rinit(uint64_t s){for(int i=0;i<4;i++)st[i]=splitmix(&s);}
static uint64_t rnext(void){uint64_t r=rot(st[1]*5,7)*9,t=st[1]<<17;st[2]^=st[0];st[3]^=st[1];st[1]^=st[2];st[0]^=st[3];st[2]^=t;st[3]=rot(st[3],45);return r;}
static void put64(uint8_t*p,uint64_t v){for(int i=0;i<8;i++)p[i]=(uint8_t)(v>>(8*i));}
int main(int argc,char**argv){
  if(argc<5){fprintf(stderr,"usage\n");return 2;}
  uint64_t base_start=strtoull(argv[1],0,0); unsigned nb=(unsigned)strtoul(argv[2],0,0); unsigned lg=(unsigned)strtoul(argv[3],0,0); uint64_t salt=strtoull(argv[4],0,0);
  uint64_t t2=UINT64_C(0xf69d16de9954d388), t3=UINT64_C(0x0c60048c4e96e033); /* tail of recorded pair F, LE words */
  unsigned nexplicit=argc-5; if(nexplicit) nb=nexplicit;
  uint64_t n=UINT64_C(1)<<lg;
  for(unsigned i=0;i<nb;i++){
    uint64_t w0; uint64_t bid=base_start+i;
    if(nexplicit) w0=strtoull(argv[5+i],0,16); else { uint64_t s=bid*UINT64_C(0x9e3779b97f4a7c15)+12345; w0=splitmix(&s); }
    uint8_t a[32],b[32]; put64(a,w0);put64(a+8,~w0);put64(a+16,t2);put64(a+24,t3);
    put64(b,~w0);put64(b+8,w0);put64(b+16,t2);put64(b+24,t3);
    rinit(salt^bid); uint64_t cnt=0, first=0;
    for(uint64_t t=0;t<n;t++){uint64_t seed=rnext(); XXH128_hash_t ha=XXH3_128bits_withSeed(a,32,seed), hb=XXH3_128bits_withSeed(b,32,seed); if(ha.low64==hb.low64&&ha.high64==hb.high64){if(!cnt)first=seed;cnt++;}}
    printf("base %" PRIu64 " w0=%016" PRIx64 " lg=%u count=%" PRIu64 " first_seed=%016" PRIx64 "\n",bid,w0,lg,cnt,first); fflush(stdout);
  }
  return 0;
}
