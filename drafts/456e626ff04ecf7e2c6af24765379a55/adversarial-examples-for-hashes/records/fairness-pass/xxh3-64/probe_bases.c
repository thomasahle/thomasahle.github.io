/* Cheap structural probe: first-fold collision rate of complement-both pairs for a few
 * structured bases (w0,w1) vs base-1143 and random bases, over 2^26 xoshiro seeds. */
#define XXH_INLINE_ALL
#include "xxHash/xxhash.h"
#include <stdio.h>
#include <inttypes.h>
static uint64_t st[4];
static uint64_t rot(uint64_t x,unsigned n){return x<<n|x>>(64-n);}
static uint64_t sm(uint64_t*s){uint64_t z=(*s+=0x9e3779b97f4a7c15ULL);z=(z^(z>>30))*0xbf58476d1ce4e5b9ULL;z=(z^(z>>27))*0x94d049bb133111ebULL;return z^(z>>31);}
static void init(uint64_t s){for(int i=0;i<4;i++)st[i]=sm(&s);}
static uint64_t nx(void){uint64_t r=rot(st[1]*5,7)*9,t=st[1]<<17;st[2]^=st[0];st[3]^=st[1];st[1]^=st[2];st[0]^=st[3];st[2]^=t;st[3]=rot(st[3],45);return r;}
int main(void){
  struct{const char*n;uint64_t w0,w1;} B[]={
    {"base-1143 (control)",0x3664c49fdaa31289ULL,0x117d8b23bea10282ULL},
    {"(0,0)",0,0},{"(1,1)",1,1},{"(2^63,2^63)",1ULL<<63,1ULL<<63},{"(~0,0)",~0ULL,0},
    {"(k0,k1)",0xbe4ba423396cfeb8ULL,0x1cad21f72c81017cULL},{"(k0,~k1)",0xbe4ba423396cfeb8ULL,~0x1cad21f72c81017cULL},
    {"(k1,k0)",0x1cad21f72c81017cULL,0xbe4ba423396cfeb8ULL},{"(0,~k0-k1... 0)",0,0xdaf8c61a65ee0034ULL},
    {"random#1",0x9e3779b97f4a7c15ULL,0x243f6a8885a308d3ULL},{"random#2",0x13198a2e03707344ULL,0xa4093822299f31d0ULL},
    {"random#3",0x082efa98ec4e6c89ULL,0x452821e638d01377ULL},{"random#4",0xbe5466cf34e90c6cULL,0xc0ac29b7c97c50ddULL}};
  const uint64_t N=1ULL<<26;
  for(unsigned k=0;k<sizeof B/sizeof*B;k++){
    uint8_t a[16],b[16];
    for(int i=0;i<8;i++){a[i]=B[k].w0>>(8*i);a[8+i]=B[k].w1>>(8*i);b[i]=~a[i];b[8+i]=~a[8+i];}
    init(0x2026091899000477ULL); uint64_t c=0;
    for(uint64_t i=0;i<N;i++){uint64_t s=nx(); c+=(XXH3_mix16B(a,XXH3_kSecret,s)==XXH3_mix16B(b,XXH3_kSecret,s));}
    printf("%-22s hits=%" PRIu64 "/2^26 log2rate=%s%.2f\n",B[k].n,c,c?"":"<",c?__builtin_log2((double)c/N):-26.0);
  }
}
