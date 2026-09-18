// rapid3_check: re-check the blog's rapidhash v3 witness against UPSTREAM master rapidhash.h,
// then test the shipped-constant annihilation pairs (key-free under a fixed public secret).
// Build: g++ -O2 -std=c++17 -pthread -I<clone> rapid3_check.cpp -o rapid3_check
// Run:   ./rapid3_check <log2N for the fold pair> <log2 seeds for annihilation checks>
#include "rapidhash.h"
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <string>
#include <thread>
#include <vector>
#include <atomic>
#include <mutex>

static const char* A32  = "9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c";
static const char* B32  = "642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c";
static const char* A48  = "9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c48c651edae76208e840fc51f1cccbb02";
static const char* B48  = "642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c48c651edae76208e840fc51f1cccbb02";

static std::vector<uint8_t> unhex(const char* s){ std::vector<uint8_t> v; for(size_t i=0;s[i]&&s[i+1];i+=2){ unsigned b; sscanf(s+i,"%2x",&b); v.push_back((uint8_t)b);} return v; }
static void put64(uint8_t* p, uint64_t x){ for(int i=0;i<8;i++) p[i]=(uint8_t)(x>>(8*i)); }

static inline uint64_t splitmix64(uint64_t& x){ uint64_t z=(x+=0x9e3779b97f4a7c15ull); z=(z^(z>>30))*0xbf58476d1ce4e5b9ull; z=(z^(z>>27))*0x94d049bb133111ebull; return z^(z>>31); }
struct Xo { uint64_t s[4]; static uint64_t rotl(uint64_t x,int k){return (x<<k)|(x>>(64-k));}
  void seed(uint64_t v){ for(int i=0;i<4;i++) s[i]=splitmix64(v); }
  uint64_t next(){ uint64_t r=rotl(s[1]*5,7)*9; uint64_t t=s[1]<<17; s[2]^=s[0]; s[3]^=s[1]; s[1]^=s[2]; s[0]^=s[3]; s[2]^=t; s[3]=rotl(s[3],45); return r; } };

typedef uint64_t (*HF)(const void*, size_t, uint64_t);
struct Var { const char* name; HF f; };
static Var vars[3] = { {"rapidhash v3", rapidhash_withSeed}, {"rapidhash v3 micro", rapidhashMicro_withSeed}, {"rapidhash v3 nano", rapidhashNano_withSeed} };

// exact central Poisson 95% CI for a count k (Garwood), via chi-square quantiles computed by bisection on the regularized gamma
static double gammainc_lower(double a, double x){ // regularized P(a,x)
  if(x<=0) return 0; if(x<a+1){ double sum=1.0/a, term=1.0/a; for(int n=1;n<10000;n++){ term*=x/(a+n); sum+=term; if(term<sum*1e-15) break;} return sum*exp(-x+a*log(x)-lgamma(a)); }
  double b=x+1-a, c=1e300, d=1/b, h=d; for(int i=1;i<10000;i++){ double an=-i*(i-a); b+=2; d=an*d+b; if(fabs(d)<1e-300) d=1e-300; c=b+an/c; if(fabs(c)<1e-300) c=1e-300; d=1/d; double del=d*c; h*=del; if(fabs(del-1)<1e-15) break;} return 1-exp(-x+a*log(x)-lgamma(a))*h; }
static double poisson_ci(int k, bool upper){ // lower: chi2(0.025,2k)/2 ; upper: chi2(0.975,2k+2)/2
  double a = upper ? (k+1) : k; double target = upper ? 0.975 : 0.025; if(!upper && k==0) return 0;
  double lo=0, hi=std::max(10.0, 5.0*a+50); for(int it=0;it<200;it++){ double mid=(lo+hi)/2; if(gammainc_lower(a,mid)<target) lo=mid; else hi=mid; } return (lo+hi)/2; }

int main(int argc, char** argv){
  int lgN = argc>1 ? atoi(argv[1]) : 30; int lgS = argc>2 ? atoi(argv[2]) : 26; int T = 8;
  auto a32=unhex(A32), b32=unhex(B32), a48=unhex(A48), b48=unhex(B48);
  printf("== upstream master rapidhash.h, default mode (RAPIDHASH_FAST/unprotected, %s)\n",
#ifdef RAPIDHASH_COMPACT
  "RAPIDHASH_COMPACT"
#else
  "RAPIDHASH_UNROLLED"
#endif
  );
  // 1. recorded witness and fixed-seed outputs (records/paper_rows.json rapid3_32 / rapid3_48)
  int bad=0;
  for(auto& v: vars){
    uint64_t hA=v.f(a32.data(),32,0x3187ae8a8617e034ull), hB=v.f(b32.data(),32,0x3187ae8a8617e034ull);
    printf("%s 32B seed 3187ae8a8617e034: %016llx %016llx %s (record a7ee6375a78a86f0)\n",v.name,(unsigned long long)hA,(unsigned long long)hB,hA==hB?"COLLIDE":"differ"); bad += !(hA==hB && hA==0xa7ee6375a78a86f0ull);
    uint64_t dA=v.f(a48.data(),48,0x3187ae8a8617e034ull), dB=v.f(b48.data(),48,0x3187ae8a8617e034ull);
    printf("%s 48B seed 3187ae8a8617e034: %016llx %016llx %s (record b52b5b5759f1d08a)\n",v.name,(unsigned long long)dA,(unsigned long long)dB,dA==dB?"COLLIDE":"differ"); bad += !(dA==dB && dA==0xb52b5b5759f1d08aull);
    printf("%s 32B seed 0: %016llx %016llx (record 0d20ed65346f1683 c5edbf6f73ebfe93)\n",v.name,(unsigned long long)v.f(a32.data(),32,0),(unsigned long long)v.f(b32.data(),32,0));
    bad += !(v.f(a32.data(),32,0)==0x0d20ed65346f1683ull && v.f(b32.data(),32,0)==0xc5edbf6f73ebfe93ull);
    printf("%s 32B seed 1: %016llx %016llx (record bdbb8ff6a4b55840 df632f0ff8d94704)\n",v.name,(unsigned long long)v.f(a32.data(),32,1),(unsigned long long)v.f(b32.data(),32,1));
    bad += !(v.f(a32.data(),32,1)==0xbdbb8ff6a4b55840ull && v.f(b32.data(),32,1)==0xdf632f0ff8d94704ull);
    printf("%s 48B seed 0/1: %016llx %016llx / %016llx %016llx (record 7db1189e1a7b8262 f55876e5398b384e / ee70e342e0da67d9 f833b72f2285b326)\n",v.name,
      (unsigned long long)v.f(a48.data(),48,0),(unsigned long long)v.f(b48.data(),48,0),(unsigned long long)v.f(a48.data(),48,1),(unsigned long long)v.f(b48.data(),48,1));
    bad += !(v.f(a48.data(),48,0)==0x7db1189e1a7b8262ull && v.f(b48.data(),48,0)==0xf55876e5398b384eull && v.f(a48.data(),48,1)==0xee70e342e0da67d9ull && v.f(b48.data(),48,1)==0xf833b72f2285b326ull);
  }
  printf("record mismatches: %d\n\n", bad);

  // 2. sampled rate of the recorded fold pair (32 B and 48 B, standard) over uniform 64-bit seeds; T threads, xoshiro256** per thread
  for(int which=0; which<2; which++){
    const uint8_t* ma = which? a48.data(): a32.data(); const uint8_t* mb = which? b48.data(): b32.data(); size_t len = which?48:32; int L = which?6:4;
    uint64_t N = 1ull<<lgN; std::atomic<uint64_t> hits{0}; std::vector<std::thread> th; std::mutex mu; std::vector<uint64_t> ex;
    for(int t=0;t<T;t++) th.emplace_back([&,t]{ Xo r; r.seed(0x5eedull*1000003ull + 20260918ull + (uint64_t)t*0x9e3779b97f4a7c15ull); uint64_t n=N/T, h=0;
      for(uint64_t i=0;i<n;i++){ uint64_t s=r.next(); if(rapidhash_withSeed(ma,len,s)==rapidhash_withSeed(mb,len,s)){ h++; std::lock_guard<std::mutex> g(mu); if(ex.size()<3) ex.push_back(s);} } hits+=h; });
    for(auto& x: th) x.join();
    uint64_t k=hits.load(); double p=(double)k/N; double lo=poisson_ci((int)k,false)/N, hi=poisson_ci((int)k,true)/N;
    printf("fold pair %zuB: %llu / 2^%d collisions; rate 2^%.3f; bits = log2(%d) - log2(rate) = %.3f; exact Poisson 95%% CI on bits [%.2f, %.2f]\n", len, (unsigned long long)k, lgN, log2(p), L, log2(L)-log2(p), log2(L)-log2(hi), k? log2(L)-log2(lo): INFINITY);
    for(auto s: ex) printf("  example colliding seed %016llx -> %016llx\n",(unsigned long long)s,(unsigned long long)rapidhash_withSeed(ma,len,s));
  }
  printf("\n");

  // 3. annihilation pairs with the shipped constants (key-free: the zeroed operand kills the product for every seed)
  struct Case { const char* what; size_t len; std::vector<uint8_t> m, m2; };
  std::vector<Case> cases;
  { // 16 B: w0 = secret[1] -> final a = 0 -> output = mix(secret[7], secret[1]^16) for all seeds and all w1
    std::vector<uint8_t> m(16), m2(16); put64(&m[0],0x8bb84b93962eacc9ull); put64(&m[8],0x0123456789abcdefull); m2=m; put64(&m2[8],0xfedcba9876543210ull);
    cases.push_back({"16B: w0 = secret[1] (final product zeroed), w1 differs; L=2",16,m,m2}); }
  { // 32 B: w2 = secret[1] ^ 32 -> final a = 0; w0,w1,w3 arbitrary
    std::vector<uint8_t> m(32), m2(32); memcpy(m.data(),a32.data(),32); put64(&m[16],0x8bb84b93962eacc9ull^32ull); m2=m; memcpy(m2.data(),b32.data(),16); put64(&m2[24],~0ull);
    cases.push_back({"32B: w2 = secret[1]^32 (final product zeroed), w0,w1,w3 differ; L=4",32,m,m2}); }
  { // 32 B: w0 = secret[2] -> first fold zero -> state 0, w1 forgotten
    std::vector<uint8_t> m(32), m2(32); memcpy(m.data(),a32.data(),32); put64(&m[0],0x4b33a62ed433d4a3ull); m2=m; put64(&m2[8],0xdeadbeefcafef00dull);
    cases.push_back({"32B: w0 = secret[2] (first fold zeroed), w1 differs; L=4",32,m,m2}); }
  { // 48 B: same w0 = secret[2] trick, w1 differs
    std::vector<uint8_t> m(48), m2(48); memcpy(m.data(),a48.data(),48); put64(&m[0],0x4b33a62ed433d4a3ull); m2=m; put64(&m2[8],0xdeadbeefcafef00dull);
    cases.push_back({"48B: w0 = secret[2] (first fold zeroed), w1 differs; L=6",48,m,m2}); }
  for(auto& c: cases){
    for(auto& v: vars){
      uint64_t S=1ull<<lgS; std::atomic<uint64_t> miss{0}; std::vector<std::thread> th;
      for(int t=0;t<T;t++) th.emplace_back([&,t]{ Xo r; r.seed(777ull+t); uint64_t n=S/T, mm=0;
        for(uint64_t i=0;i<n;i++){ uint64_t s = (i&1)? r.next() : (uint64_t)t*n+i; // half sequential seeds, half uniform seeds
          if(v.f(c.m.data(),c.len,s)!=v.f(c.m2.data(),c.len,s)) mm++; } miss+=mm; });
      for(auto& x: th) x.join();
      printf("%s | %s: %llu non-collisions in 2^%d seeds (2^%d sequential + 2^%d uniform) -> %s; h(seed 0)=%016llx h(seed 2^63)=%016llx\n", v.name, c.what, (unsigned long long)miss.load(), lgS, lgS-1, lgS-1, miss.load()?"NOT key-free":"collides for every tested seed",
        (unsigned long long)v.f(c.m.data(),c.len,0),(unsigned long long)v.f(c.m.data(),c.len,1ull<<63));
    }
  }
  // 4. 256-way multicollision at 16 B with w0 = secret[1]: all outputs equal for a few seeds
  { uint64_t seeds[3]={0,0x3187ae8a8617e034ull,~0ull}; for(auto s: seeds){ std::vector<uint8_t> m(16); put64(&m[0],0x8bb84b93962eacc9ull); uint64_t h0=0; int eq=0; for(int x=0;x<256;x++){ put64(&m[8],(uint64_t)x*0x9e3779b97f4a7c15ull); uint64_t h=rapidhash_withSeed(m.data(),16,s); if(x==0) h0=h; eq += (h==h0);} printf("16B w0=secret[1] family, seed %016llx: %d/256 messages share output %016llx\n",(unsigned long long)s,eq,(unsigned long long)h0);} }
  return bad?1:0;
}
