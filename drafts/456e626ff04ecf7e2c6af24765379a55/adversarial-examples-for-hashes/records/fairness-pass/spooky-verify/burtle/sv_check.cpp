
// Independent re-check of the SpookyHash V2 pair against Bob Jenkins' reference SpookyV2.cpp.
// Build: g++ -O2 -std=c++11 -o sv_check sv_check.cpp SpookyV2.cpp
// Usage: ./sv_check <log2 N> <mode 0|1> <rng seed>   (mode 0: h1=h2=seed; mode 1: independent h1,h2)
#include "SpookyV2.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cmath>
#include <initializer_list>
static const char *MH  = "9d21a7fafac0e24f428e3c0124de5f8c54c1de8bef8535a636cfd97be5aad365055fb9bc0a26d493c5f21f4acbfade067eba007efbf79f8729cacdc39c584138336a77d8330a62803d3750884eb91b907e091a0ff547078c45198f347078fe21b0fa47014d7d1cd6142861dadfecd7fd0bf71f80b4e9112d36a8d91996608d73c49284098d900d7ef1f9c16471246346ca2277cbf87725a6d861b0db4924da8f5c5307034eb3933fcca647e8404e141afc956d0eb643e7ee9381dabba3c2d09e2dae44a4bda150f889d0704a2463cd7cb2b194ea3ccc9982ceae11e212336b6b3ad81125bd494e3f1b7291e3b0c73a74fa886a41f0fe25055b0f1fc96e8f7c0123002a69c35989a703b718ce8a1bfbddafb3e3";
static const char *MPH = "9d21a7fafac0e24f428e3c0124de5f8c54c1de8bef8535a636cfd97be5aad365055fb9bc0a26d493c5f21f4acbfade067eba007efbf79f8729cacdc39c584138336a77d8330a62803d3750884eb91b907e091a0ff547078c45198f347078fe21b0fa47014d7d1cd6142861dadfecd7fd0bf71f80b4e9112d36a8d91996608d73c49284098d900d7ef1f9c16471246346ca2277cbf87725a6d861b0db4924da8f5c5307034eb3933fcca647e8404e141afc956d0eb643e76e9381dabba3c2d09e2dae44a4bda150f889d0704a2463cdfcb2b194ea3ccc9982ceae11e212336b6b3ad81125bd494e3f1b7291e3b0c73a74fa886a41f0fe25055b0f1fc96e8f7c0123002a69c35989a703b718ce8a1bfb5dafb3c3";
static const char *SH  = "a0ed1dcaeb2e421c17cf8516645510b61c1e80eb014c077fdbb3b3ab867298f2ccfb12c325d96dd09c3de65c247a0257a6d22a9da89710cfb724db191c748806e9d6b0272f125818a2c061b9e406999b788261dcd2487f7064e1bf4d00670a3d09230d92b37de4141a5e5d12e205c37e42c13193ab064833476cb9e5a22bd74c85073313646062ca36b300b4a62fa6797864ca181b97739bf4dd0567f5ce9a24398e0ddb544ee4926c6c033f6397e4790f51641c3d6dc04d5dcfe95ffe3054262ff9e5d8a8f17cce6b3c565acbecc693b77d90b1564fd1ddc71df2c56f2fd52fd6d8c9fb3b84612e5f83397bb5ac932651ce382aa90592ebb81d61c29ac53c2bace0711020c947c1ce55f2cfd79f821e92ba90562ba05eeab1b8f0771534";
static const char *SPH = "a0ed1dcaeb2e421c17cf8516645510b61c1e80eb014c077fdbb3b3ab867298f2ccfb12c325d96dd09c3de65c247a0257a6d22a9da89710cfb724db191c748806e9d6b0272f125818a2c061b9e406999b788261dcd2487f7064e1bf4d00670a3d09230d92b37de4141a5e5d12e205c37e42c13193ab064833476cb9e5a22bd74c85073313646062ca36b300b4a62fa6797864ca181b97739bf4dd0567f5ce9a24398e0ddb544ee4926c6c033f6397e4790f51641c3d6dc04d5dcfe95ffe3054a62ff9e5d8a8f17cce6b3c565acbecc693b77d90b1564fd1ddc71df2c56f2fd52fd6d8c9fb3b84612e5f83397bb5ac932651ce382aa90592ebb81d61c29ac53c2bace0711020c947c1ce55f2cfd79f821e92ba90562ba05e6ab1b8f0771514";
static size_t unhex(const char *s, unsigned char *out){ size_t n=strlen(s)/2; for(size_t i=0;i<n;i++){unsigned v; sscanf(s+2*i,"%2x",&v); out[i]=(unsigned char)v;} return n; }
static uint64_t sm(uint64_t &x){ uint64_t z=(x+=0x9e3779b97f4a7c15ULL); z=(z^(z>>30))*0xbf58476d1ce4e5b9ULL; z=(z^(z>>27))*0x94d049bb133111ebULL; return z^(z>>31); }
// SMHasher verification: keys 0..255 bytes (key[j]=j), seed 256-i, LE outputs concatenated, hashed with seed 0, first 4 bytes LE.
static uint32_t verif64(){ unsigned char key[256], buf[256*8]; for(int i=0;i<256;i++){ for(int j=0;j<i;j++) key[j]=(unsigned char)j; uint64_t h=SpookyHash::Hash64(key,i,(uint64)(256-i)); for(int b=0;b<8;b++) buf[i*8+b]=(unsigned char)(h>>(8*b)); }
  uint64_t f=SpookyHash::Hash64(buf,sizeof buf,0); return (uint32_t)f; }
int main(int argc,char**argv){
  int lg = argc>1?atoi(argv[1]):20; int mode = argc>2?atoi(argv[2]):0; uint64_t rs = argc>3?strtoull(argv[3],0,0):12345;
  printf("SMHasher3 verification SpookyHash2_64 (Hash64 wrapper, h1=h2=seed): 0x%08X (expect 0x972C4BDC)\n", verif64());
  unsigned char m[512],mp[512],s[512],sp[512]; size_t lm=unhex(MH,m), lmp=unhex(MPH,mp), ls=unhex(SH,s), lsp=unhex(SPH,sp);
  printf("275-pair lengths %zu %zu; 286-pair lengths %zu %zu\n", lm,lmp,ls,lsp);
  int nd=0; for(size_t i=0;i<lm;i++) if(m[i]!=mp[i]){ printf("  275 diff byte [%zu] %02x->%02x\n",i,m[i],mp[i]); nd++; }
  for(uint64_t seed: {0ULL,3ULL,4ULL}){ uint64 a1=seed,a2=seed,b1=seed,b2=seed; SpookyHash::Hash128(m,lm,&a1,&a2); SpookyHash::Hash128(mp,lmp,&b1,&b2);
    printf("275 seed %llu: %016llx %016llx | %016llx %016llx %s\n",(unsigned long long)seed,(unsigned long long)a1,(unsigned long long)a2,(unsigned long long)b1,(unsigned long long)b2,(a1==b1&&a2==b2)?"COLLIDE":"differ");
    a1=seed;a2=seed;b1=seed;b2=seed; SpookyHash::Hash128(s,ls,&a1,&a2); SpookyHash::Hash128(sp,lsp,&b1,&b2);
    printf("286 seed %llu: %016llx %016llx | %016llx %016llx %s\n",(unsigned long long)seed,(unsigned long long)a1,(unsigned long long)a2,(unsigned long long)b1,(unsigned long long)b2,(a1==b1&&a2==b2)?"COLLIDE":"differ");
  }
  uint64_t N=1ULL<<lg, c32=0,c64=0,c128=0, d32=0,d64=0,d128=0; uint64_t x=rs;
  for(uint64_t i=0;i<N;i++){ uint64_t s1=sm(x), s2= mode? sm(x): s1;
    uint64 a1=s1,a2=s2,b1=s1,b2=s2; SpookyHash::Hash128(m,lm,&a1,&a2); SpookyHash::Hash128(mp,lmp,&b1,&b2);
    c128 += (a1==b1&&a2==b2); c64 += (a1==b1); c32 += ((uint32_t)a1==(uint32_t)b1);
    a1=s1;a2=s2;b1=s1;b2=s2; SpookyHash::Hash128(s,ls,&a1,&a2); SpookyHash::Hash128(sp,lsp,&b1,&b2);
    d128 += (a1==b1&&a2==b2); d64 += (a1==b1); d32 += ((uint32_t)a1==(uint32_t)b1);
  }
  double r=(double)c64/N, se=sqrt(r*(1-r)/N);
  printf("N=2^%d mode=%d rng=%llu\n275-pair: c32=%llu c64=%llu c128=%llu rate64=%.7f log2=%.5f 95%%CI[%.6f,%.6f] score=log2(35)-log2(rate)=%.4f\n",lg,mode,(unsigned long long)rs,(unsigned long long)c32,(unsigned long long)c64,(unsigned long long)c128,r,log2(r),r-1.96*se,r+1.96*se,log2(35.0)-log2(r));
  double r2=(double)d64/N; printf("286-pair1: c32=%llu c64=%llu c128=%llu rate64=%.7f log2=%.5f score=%.4f\n",(unsigned long long)d32,(unsigned long long)d64,(unsigned long long)d128,r2,log2(r2),log2(36.0)-log2(r2));
  return 0; }
