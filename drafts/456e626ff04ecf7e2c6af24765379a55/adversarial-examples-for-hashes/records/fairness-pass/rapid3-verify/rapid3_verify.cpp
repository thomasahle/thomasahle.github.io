// Independent re-check of the blog's rapidhash v3 row against upstream master rapidhash.h.
#include "rapidhash.h"
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <thread>
#include <vector>
#include <atomic>
#include <cmath>
static inline uint64_t rotl64(uint64_t x,int k){return (x<<k)|(x>>(64-k));}
struct Xo{uint64_t s[4];
 uint64_t next(){uint64_t r=rotl64(s[1]*5,7)*9;uint64_t t=s[1]<<17;s[2]^=s[0];s[3]^=s[1];s[1]^=s[2];s[0]^=s[3];s[2]^=t;s[3]=rotl64(s[3],45);return r;}
 void seed(uint64_t x){for(int i=0;i<4;i++){x+=0x9e3779b97f4a7c15ull;uint64_t z=x;z=(z^(z>>30))*0xbf58476d1ce4e5b9ull;z=(z^(z>>27))*0x94d049bb133111ebull;s[i]=z^(z>>31);}}};
static void unhex(const char*h,uint8_t*out,size_t n){for(size_t i=0;i<n;i++){unsigned v;sscanf(h+2*i,"%2x",&v);out[i]=(uint8_t)v;}}
typedef uint64_t (*HF)(const void*,size_t,uint64_t);
static uint64_t H_std(const void*k,size_t n,uint64_t s){return rapidhash_withSeed(k,n,s);}
static uint64_t H_micro(const void*k,size_t n,uint64_t s){return rapidhashMicro_withSeed(k,n,s);}
static uint64_t H_nano(const void*k,size_t n,uint64_t s){return rapidhashNano_withSeed(k,n,s);}
static const char* names[3]={"standard","micro","nano"};
static HF fns[3]={H_std,H_micro,H_nano};
static void put64(uint8_t*p,uint64_t v){for(int i=0;i<8;i++)p[i]=(uint8_t)(v>>(8*i));}

int main(int argc,char**argv){
  int lg=argc>1?atoi(argv[1]):30; int nth=argc>2?atoi(argv[2]):8; uint64_t salt=argc>3?strtoull(argv[3],0,0):0x5eed2026u;
  uint8_t A[32],B[32],D1[48],D2[48];
  unhex("9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c",A,32);
  unhex("642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c",B,32);
  unhex("9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c48c651edae76208e840fc51f1cccbb02",D1,48);
  unhex("642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c48c651edae76208e840fc51f1cccbb02",D2,48);
  printf("build: %s %s\n",
#ifdef RAPIDHASH_PROTECTED
  "PROTECTED",
#else
  "FAST",
#endif
#ifdef RAPIDHASH_COMPACT
  "COMPACT");
#else
  "UNROLLED");
#endif
  // 1. recorded witness
  uint64_t ws=0x3187ae8a8617e034ull;
  for(int v=0;v<3;v++){
    printf("[%s] pair A seed %016llx: %016llx %016llx %s\n",names[v],(unsigned long long)ws,(unsigned long long)fns[v](A,32,ws),(unsigned long long)fns[v](B,32,ws),fns[v](A,32,ws)==fns[v](B,32,ws)?"COLLIDE":"differ");
    printf("[%s] pair D seed %016llx: %016llx %016llx %s\n",names[v],(unsigned long long)ws,(unsigned long long)fns[v](D1,48,ws),(unsigned long long)fns[v](D2,48,ws),fns[v](D1,48,ws)==fns[v](D2,48,ws)?"COLLIDE":"differ");
    for(uint64_t s0=0;s0<2;s0++) printf("[%s] pair A seed %llu: %016llx %016llx | D: %016llx %016llx\n",names[v],(unsigned long long)s0,(unsigned long long)fns[v](A,32,s0),(unsigned long long)fns[v](B,32,s0),(unsigned long long)fns[v](D1,48,s0),(unsigned long long)fns[v](D2,48,s0));
  }
  // 2. every-seed candidates
  const uint64_t S1=0x8bb84b93962eacc9ull,S2=0x4b33a62ed433d4a3ull;
  struct Case{const char*name;size_t len;uint8_t m[48],m2[48];} cs[6];
  auto mk=[&](Case&c,const char*nm,size_t len,uint64_t*w,uint64_t*w2){c.name=nm;c.len=len;for(size_t i=0;i<len/8;i++){put64(c.m+8*i,w[i]);put64(c.m2+8*i,w2[i]);}};
  {uint64_t w[2]={S1,0x0123456789abcdefull},w2[2]={S1,0xfedcba9876543210ull};mk(cs[0],"16B w0=secret[1], w1 differs",16,w,w2);}
  {uint64_t w[4]={1,2,S1,4},w2[4]={5,6,S1,8};mk(cs[1],"32B w2=secret[1], w0,w1,w3 differ",32,w,w2);}
  {uint64_t w[4]={1,2,S1^32,4},w2[4]={5,6,S1^32,8};mk(cs[2],"32B w2=secret[1]^32, w0,w1,w3 differ",32,w,w2);}
  {uint64_t w[4]={S2,2,3,4},w2[4]={S2,6,3,4};mk(cs[3],"32B w0=secret[2], w1 differs",32,w,w2);}
  {uint64_t w[6]={S2,2,3,4,5,6},w2[6]={S2,7,3,4,5,6};mk(cs[4],"48B w0=secret[2], w1 differs",48,w,w2);}
  {uint64_t w[2]={S1,0x0123456789abcdefull},w2[2]={S1,0xfedcba9876543210ull};mk(cs[5],"12B w0=secret[1], bytes 8..11 differ",12,w,w2);}
  int lgE=26;
  for(int v=0;v<3;v++) for(int ci=0;ci<6;ci++){
    Case&c=cs[ci]; uint64_t o0=fns[v](c.m,c.len,0);
    std::atomic<uint64_t> bad{0},nonconst{0};
    std::vector<std::thread> th;
    for(int t=0;t<nth;t++) th.emplace_back([&,t]{Xo x;x.seed(salt*1000003ull+t*7919ull+ci*31ull+v);uint64_t n=(1ull<<lgE)/nth;uint64_t b=0,nc=0;
      for(uint64_t i=0;i<n;i++){uint64_t s=(i&1)?x.next():(t*n+i);uint64_t h1=fns[v](c.m,c.len,s),h2=fns[v](c.m2,c.len,s);if(h1!=h2)b++;if(h1!=o0)nc++;}
      bad+=b;nonconst+=nc;});
    for(auto&t:th)t.join();
    printf("[%s] %s: non-collisions %llu / 2^%d seeds (half sequential, half uniform); output-at-seed-0 %016llx; seeds with different output: %llu\n",names[v],c.name,(unsigned long long)bad.load(),lgE,(unsigned long long)o0,(unsigned long long)nonconst.load());
  }
  // 3. fresh uniform sample for pairs A and D
  std::atomic<uint64_t> cA{0},cD{0};
  std::vector<std::thread> th; uint64_t N=1ull<<lg;
  std::vector<uint64_t> firstA(nth,0);
  for(int t=0;t<nth;t++) th.emplace_back([&,t]{Xo x;x.seed(salt^(0xabcdef0000ull+t));uint64_t n=N/nth,a=0,d=0;
    for(uint64_t i=0;i<n;i++){uint64_t s=x.next();if(H_std(A,32,s)==H_std(B,32,s)){a++;if(!firstA[t])firstA[t]=s;}if(H_std(D1,48,s)==H_std(D2,48,s))d++;}
    cA+=a;cD+=d;});
  for(auto&t:th)t.join();
  double eA=(double)cA.load()/N,eD=(double)cD.load()/N;
  printf("[standard] pair A: %llu / 2^%d collisions, rate 2^%.3f, bits log2(4/eps)=%.3f\n",(unsigned long long)cA.load(),lg,log2(eA),log2(4.0/eA));
  printf("[standard] pair D: %llu / 2^%d collisions, rate 2^%.3f, bits log2(6/eps)=%.3f\n",(unsigned long long)cD.load(),lg,log2(eD),log2(6.0/eD));
  for(int t=0;t<nth;t++) if(firstA[t]) printf("  fresh colliding seed for A: %016llx -> %016llx\n",(unsigned long long)firstA[t],(unsigned long long)H_std(A,32,firstA[t]));
  return 0;
}
