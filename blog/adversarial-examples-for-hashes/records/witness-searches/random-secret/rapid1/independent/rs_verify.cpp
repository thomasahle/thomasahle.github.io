// Independent verification harness: rapidhash v1.0 under the random-secret key model.
// Uses the upstream header verbatim (rapidhash.h, tag rapidhash_v1.0,
// sha256 f57de5bf8eb6f0b86be9a6b3ea0893b56fd9a27709a903d21b235dbc8dc3b8b5) and its
// rapidhash_internal(key, len, seed, secret) entry point.  Key sampling written from
// scratch: per-thread std::mt19937_64 (seeded through std::seed_seq from rngseed and
// the thread index), four consecutive 64-bit draws per key = seed, secret[0..2].
// Build: g++ -O3 -std=c++17 -march=native -pthread rs_verify.cpp -o rs_verify
// Usage: ./rs_verify selftest
//        ./rs_verify pair <hexM> <hexM'> <log2N> <rngseed> <random|default|odd> <threads>
#include "rapidhash_upstream.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cmath>
#include <random>
#include <string>
#include <thread>
#include <vector>
#include <mutex>

static std::vector<uint8_t> unhex(const char* s){
  size_t n=strlen(s); if(n%2){fprintf(stderr,"odd hex length\n");exit(2);}
  std::vector<uint8_t> v(n/2);
  for(size_t i=0;i<n/2;i++){unsigned b; if(sscanf(s+2*i,"%2x",&b)!=1){fprintf(stderr,"bad hex\n");exit(2);} v[i]=(uint8_t)b;}
  return v;
}
static std::string hex(const std::vector<uint8_t>& v){ std::string s; char b[3]; for(uint8_t x: v){snprintf(b,3,"%02x",x); s+=b;} return s; }

// Poisson CDF P(X<=k; lam), summed directly (k small).
static double pois_cdf(int k,double lam){ double t=exp(-lam), s=t; for(int i=1;i<=k;i++){t*=lam/i; s+=t;} return s; }
// Exact (Garwood) 95% CI for a Poisson mean given an observed count h.
static void garwood(long h,double& lo,double& hi){
  if(h==0) lo=0; else { double a=0,b=h*3+10; for(int it=0;it<200;it++){double m=(a+b)/2; if(pois_cdf((int)h-1,m)>0.975) a=m; else b=m;} lo=(a+b)/2; }
  { double a=0,b=h*3+30; for(int it=0;it<200;it++){double m=(a+b)/2; if(pois_cdf((int)h,m)>0.025) a=m; else b=m;} hi=(a+b)/2; }
}

struct Hit{ uint64_t seed,s0,s1,s2,out; };

int main(int argc,char** argv){
  if(argc<2){fprintf(stderr,"usage\n");return 2;}
  if(!strcmp(argv[1],"selftest")){
    int fails=0;
    // 1. verify-package constant: "message digest", len 14, seed 3, default secret.
    uint64_t h=rapidhash_internal("message digest",14,3,rapid_secret);
    printf("selftest 1: rapidhash_internal(\"message digest\",14,seed=3,default) = %016llx expected 0031cdc21324150f %s\n",(unsigned long long)h,h==0x0031cdc21324150fULL?"PASS":"FAIL"); fails+=h!=0x0031cdc21324150fULL;
    // 2. row witness (default secret): pair A collides at seed 3788f2419a81e2d6 -> 8f7a71ffebd4a14b.
    auto A=unhex("9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c"), B=unhex("642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c");
    uint64_t ha=rapidhash_internal(A.data(),32,0x3788f2419a81e2d6ULL,rapid_secret), hb=rapidhash_internal(B.data(),32,0x3788f2419a81e2d6ULL,rapid_secret);
    printf("selftest 2: row witness seed 3788f2419a81e2d6: %016llx %016llx expected 8f7a71ffebd4a14b %s\n",(unsigned long long)ha,(unsigned long long)hb,(ha==hb&&ha==0x8f7a71ffebd4a14bULL)?"PASS":"FAIL"); fails+=!(ha==hb&&ha==0x8f7a71ffebd4a14bULL);
    // 3. searcher's reported random-secret example for pair A.
    { uint64_t sec[3]={0xd0d81d65fd961dffULL,0xa9132a2f5b5d4f54ULL,0xd72e7f4d4f7270f5ULL}; uint64_t s=0x3879cdfddc782ad3ULL;
      uint64_t x=rapidhash_internal(A.data(),32,s,sec), y=rapidhash_internal(B.data(),32,s,sec);
      printf("selftest 3: pair A example key: %016llx %016llx expected 4329ec0defb7f826 %s\n",(unsigned long long)x,(unsigned long long)y,(x==y&&x==0x4329ec0defb7f826ULL)?"PASS":"FAIL"); fails+=!(x==y&&x==0x4329ec0defb7f826ULL); }
    // 4. searcher's reported random-secret example for pair B24.
    { auto C=unhex("9bd4604137366abec688a63706aa4a2188d35499de169df6"), D=unhex("642b9fbec8c99541c688a63706aa4a2188d35499de169df6");
      uint64_t sec[3]={0xb928485235f145d4ULL,0xe19da6ded88cdbd0ULL,0x64eacf717f4fb9ceULL}; uint64_t s=0x8f934f0d969d8c38ULL;
      uint64_t x=rapidhash_internal(C.data(),24,s,sec), y=rapidhash_internal(D.data(),24,s,sec);
      printf("selftest 4: pair B24 example key: %016llx %016llx expected e69e5ea677632e22 %s\n",(unsigned long long)x,(unsigned long long)y,(x==y&&x==0xe69e5ea677632e22ULL)?"PASS":"FAIL"); fails+=!(x==y&&x==0xe69e5ea677632e22ULL); }
    // 5. public wrapper consistency.
    { uint64_t w=rapidhash_withSeed("message digest",14,3); printf("selftest 5: rapidhash_withSeed matches internal(default secret): %s\n",w==h?"PASS":"FAIL"); fails+=w!=h; }
    printf("selftest: %d failures\n",fails); return fails?1:0;
  }
  if(!strcmp(argv[1],"pair")){
    if(argc<8){fprintf(stderr,"pair <hexM> <hexM'> <log2N> <rngseed> <random|default|odd> <threads>\n");return 2;}
    auto M=unhex(argv[2]), M2=unhex(argv[3]); int lg=atoi(argv[4]); uint64_t rngseed=strtoull(argv[5],0,10); std::string mode=argv[6]; int T=atoi(argv[7]);
    if(M.size()!=M2.size()){fprintf(stderr,"length mismatch\n");return 2;}
    size_t len=M.size(); uint64_t N=1ULL<<lg; uint64_t per=N/T;
    int L=(int)((len+7)/8); if(L<1)L=1; // words of attacker-controlled input (cap = log2 L - log2 rate)
    printf("rs_verify pair: len=%zu M=%s M'=%s N=2^%d rngseed=%llu mode=%s threads=%d rng=mt19937_64\n",len,hex(M).c_str(),hex(M2).c_str(),lg,(unsigned long long)rngseed,mode.c_str(),T);
    fflush(stdout);
    std::vector<long> hits(T,0); std::vector<std::vector<Hit>> ex(T); std::mutex mu;
    std::vector<std::thread> th;
    for(int t=0;t<T;t++) th.emplace_back([&,t](){
      std::seed_seq sq{(uint32_t)rngseed,(uint32_t)(rngseed>>32),(uint32_t)t,0x9e3779b9u,0x7f4a7c15u};
      std::mt19937_64 g(sq);
      long c=0; uint64_t sec[3];
      for(uint64_t i=0;i<per;i++){
        uint64_t seed=g();
        if(mode=="default"){ sec[0]=rapid_secret[0]; sec[1]=rapid_secret[1]; sec[2]=rapid_secret[2]; }
        else { sec[0]=g(); sec[1]=g(); sec[2]=g(); if(mode=="odd"){sec[0]|=1;sec[1]|=1;sec[2]|=1;} }
        uint64_t x=rapidhash_internal(M.data(),len,seed,sec);
        uint64_t y=rapidhash_internal(M2.data(),len,seed,sec);
        if(x==y){ c++; if(ex[t].size()<4) ex[t].push_back({seed,sec[0],sec[1],sec[2],x}); }
      }
      hits[t]=c;
    });
    for(auto& x:th) x.join();
    long h=0; for(int t=0;t<T;t++) h+=hits[t];
    double lo,hi; garwood(h,lo,hi); double Nd=(double)per*T;
    double rate=h/Nd; double l2=h?log2(rate):-INFINITY; double l2lo=lo>0?log2(lo/Nd):-INFINITY, l2hi=log2(hi/Nd);
    printf("result: hits=%ld N=%.0f (2^%d) rate=%.4e log2rate=%.4f [%.4f, %.4f] cap_bits(L=%d)=%.3f [%.3f, %.3f]\n",h,Nd,lg,rate,l2,l2lo,l2hi,L,log2(L)-l2,log2(L)-l2hi,log2(L)-l2lo);
    if(h==0) printf("floor: 0 hits -> rate < %.4e = 2^%.2f (95%%), cap_bits > %.2f\n",hi/Nd,l2hi,log2(L)-l2hi);
    int shown=0; for(int t=0;t<T&&shown<3;t++) for(auto& e:ex[t]){ if(shown>=3)break; printf("example: seed=%016llx secret={%016llx,%016llx,%016llx} out=%016llx\n",(unsigned long long)e.seed,(unsigned long long)e.s0,(unsigned long long)e.s1,(unsigned long long)e.s2,(unsigned long long)e.out); shown++; }
    return 0;
  }
  fprintf(stderr,"unknown mode\n"); return 2;
}
