/* Independent fixed-pair collision counter for XXH3_64bits_withSeed.
 * Build: gcc -O3 -std=c11 -DHDR='"path/xxhash.h"' pair_test.c -o pair_test
 * Run:   ./pair_test <log2 N> <salt hex> <M hex 16B> <M2 hex 16B>
 * Seeds: splitmix64 stream seeded by salt (independent of the post's xoshiro streams).
 * Lengths: 32 B (16-byte prefix + 16 zero bytes) and 128 B (prefix + 112 zero bytes). */
#define XXH_INLINE_ALL
#include HDR
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
static uint64_t sm(uint64_t *s){uint64_t z=(*s+=UINT64_C(0x9e3779b97f4a7c15));z=(z^(z>>30))*UINT64_C(0xbf58476d1ce4e5b9);z=(z^(z>>27))*UINT64_C(0x94d049bb133111eb);return z^(z>>31);}
__attribute__((noinline,noipa)) static uint64_t api(const uint8_t*p,size_t n,uint64_t s){return XXH3_64bits_withSeed(p,n,s);}
static void dec(const char*s,uint8_t*o){for(int i=0;i<16;i++){unsigned x;sscanf(s+2*i,"%2x",&x);o[i]=(uint8_t)x;}}
int main(int c,char**v){
  if(c!=5){fprintf(stderr,"usage\n");return 2;}
  unsigned lg=(unsigned)strtoul(v[1],0,0); uint64_t salt=strtoull(v[2],0,0);
  uint8_t a[128]={0},b[128]={0}; dec(v[3],a); dec(v[4],b);
  printf("XXH_VERSION_NUMBER=%d\n",XXH_VERSION_NUMBER);
  printf("M32=");for(int i=0;i<32;i++)printf("%02x",a[i]);printf("\nM2_32=");for(int i=0;i<32;i++)printf("%02x",b[i]);puts("");
  uint64_t probe[3]={0,1,UINT64_C(0xc8eae1baae13330b)};
  for(int i=0;i<3;i++)printf("seed=%016" PRIx64 " H32(M)=%016" PRIx64 " H32(M2)=%016" PRIx64 " H128(M)=%016" PRIx64 " H128(M2)=%016" PRIx64 "\n",probe[i],api(a,32,probe[i]),api(b,32,probe[i]),api(a,128,probe[i]),api(b,128,probe[i]));
  uint64_t n=UINT64_C(1)<<lg,s=salt,c32=0,c128=0,mis=0,shown=0;
  for(uint64_t i=0;i<n;i++){uint64_t seed=sm(&s);int h32=api(a,32,seed)==api(b,32,seed);int h128=api(a,128,seed)==api(b,128,seed);c32+=h32;c128+=h128;mis+=(h32!=h128);
    if(h32&&shown<5){shown++;printf("example seed=%016" PRIx64 " H32=%016" PRIx64 " H128=%016" PRIx64 "\n",seed,api(a,32,seed),api(a,128,seed));}}
  printf("salt=0x%016" PRIx64 " trials=%" PRIu64 " c32=%" PRIu64 " c128=%" PRIu64 " mismatch=%" PRIu64 "\n",salt,n,c32,c128,mis);
  if(c32)printf("eps32=%.6g log2=%.4f score32=log2(4N/c)=%.4f score128=%.4f\n",(double)c32/n,__builtin_log2((double)c32/n),__builtin_log2(4.0*n/c32),__builtin_log2(16.0*n/c32));
  return 0;}
