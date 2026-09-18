// Check the post's 275-byte SpookyHash V2 pair against Bob Jenkins' reference SpookyV2.cpp.
// Seed model 0: Hash64(msg,len,seed) i.e. h1 = h2 = seed (SMHasher3's model). Mode 1: independent (h1,h2).
#include "SpookyV2.h"
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cmath>
static const char *M1H="9d21a7fafac0e24f428e3c0124de5f8c54c1de8bef8535a636cfd97be5aad365055fb9bc0a26d493c5f21f4acbfade067eba007efbf79f8729cacdc39c584138336a77d8330a62803d3750884eb91b907e091a0ff547078c45198f347078fe21b0fa47014d7d1cd6142861dadfecd7fd0bf71f80b4e9112d36a8d91996608d73c49284098d900d7ef1f9c16471246346ca2277cbf87725a6d861b0db4924da8f5c5307034eb3933fcca647e8404e141afc956d0eb643e7ee9381dabba3c2d09e2dae44a4bda150f889d0704a2463cd7cb2b194ea3ccc9982ceae11e212336b6b3ad81125bd494e3f1b7291e3b0c73a74fa886a41f0fe25055b0f1fc96e8f7c0123002a69c35989a703b718ce8a1bfbddafb3e3";
static const char *M2H="9d21a7fafac0e24f428e3c0124de5f8c54c1de8bef8535a636cfd97be5aad365055fb9bc0a26d493c5f21f4acbfade067eba007efbf79f8729cacdc39c584138336a77d8330a62803d3750884eb91b907e091a0ff547078c45198f347078fe21b0fa47014d7d1cd6142861dadfecd7fd0bf71f80b4e9112d36a8d91996608d73c49284098d900d7ef1f9c16471246346ca2277cbf87725a6d861b0db4924da8f5c5307034eb3933fcca647e8404e141afc956d0eb643e76e9381dabba3c2d09e2dae44a4bda150f889d0704a2463cdfcb2b194ea3ccc9982ceae11e212336b6b3ad81125bd494e3f1b7291e3b0c73a74fa886a41f0fe25055b0f1fc96e8f7c0123002a69c35989a703b718ce8a1bfb5dafb3c3";
static size_t unhex(const char*h, uint8_t*o){size_t n=strlen(h)/2;for(size_t i=0;i<n;i++){unsigned b;sscanf(h+2*i,"%2x",&b);o[i]=b;}return n;}
static uint64_t sm(uint64_t&x){uint64_t z=(x+=0x9e3779b97f4a7c15ULL);z=(z^(z>>30))*0xbf58476d1ce4e5b9ULL;z=(z^(z>>27))*0x94d049bb133111ebULL;return z^(z>>31);}
int main(int argc,char**argv){
  int lg=argc>1?atoi(argv[1]):28; int mode=argc>2?atoi(argv[2]):0; uint64_t rs=argc>3?strtoull(argv[3],0,0):1;
  uint8_t a[300],b[300]; size_t la=unhex(M1H,a), lb=unhex(M2H,b);
  printf("lengths %zu %zu, differing bytes:",la,lb); for(size_t i=0;i<la;i++) if(a[i]!=b[i]) printf(" [%zu] %02x->%02x",i,a[i],b[i]); printf("\n");
  // SMHasher3 verification value for SpookyHash2_64 (seed 256-i, LE outputs, then seed 0, first 4 bytes LE)
  { uint8_t key[256], buf[256*8]; for(int i=0;i<256;i++){ for(int j=0;j<i;j++) key[j]=j; uint64_t h=SpookyHash::Hash64(key,i,256-i); for(int j=0;j<8;j++) buf[i*8+j]=(uint8_t)(h>>(8*j)); }
    uint64_t v=SpookyHash::Hash64(buf,256*8,0); printf("SMHasher3 verification SpookyHash2_64: 0x%08X (expect 0x972C4BDC)\n",(unsigned)(v&0xffffffffu)); if((v&0xffffffffu)!=0x972C4BDCu){puts("MISMATCH");return 1;} }
  printf("seed 0: H64(M1)=%016llx H64(M2)=%016llx\n",(unsigned long long)SpookyHash::Hash64(a,la,0),(unsigned long long)SpookyHash::Hash64(b,lb,0));
  { uint64_t h1=0,h2=0,g1=0,g2=0; SpookyHash::Hash128(a,la,&h1,&h2); SpookyHash::Hash128(b,lb,&g1,&g2); printf("seed 0 128-bit: %016llx%016llx / %016llx%016llx\n",(unsigned long long)h1,(unsigned long long)h2,(unsigned long long)g1,(unsigned long long)g2);}
  printf("seed 3: H64(M1)=%016llx H64(M2)=%016llx (expect differ)\n",(unsigned long long)SpookyHash::Hash64(a,la,3),(unsigned long long)SpookyHash::Hash64(b,lb,3));
  uint64_t N=1ULL<<lg, c64=0,c128=0,c32=0; uint64_t x=rs;
  for(uint64_t i=0;i<N;i++){ uint64_t s1=sm(x), s2= mode? sm(x): s1; uint64_t h1=s1,h2=s2,g1=s1,g2=s2; SpookyHash::Hash128(a,la,&h1,&h2); SpookyHash::Hash128(b,lb,&g1,&g2);
    if(h1==g1){c64++; if(h2==g2)c128++;} if((uint32_t)h1==(uint32_t)g1)c32++; }
  double p=(double)c64/N, se=sqrt(p*(1-p)/N);
  printf("N=2^%d mode=%d rngseed=%llu: 32-bit %llu, 64-bit %llu, 128-bit %llu; rate64 %.9f = 2^%.6f; 95%% CI [%.9f, %.9f]\n",lg,mode,(unsigned long long)rs,(unsigned long long)c32,(unsigned long long)c64,(unsigned long long)c128,p,log2(p),p-1.96*se,p+1.96*se);
  return 0;}
