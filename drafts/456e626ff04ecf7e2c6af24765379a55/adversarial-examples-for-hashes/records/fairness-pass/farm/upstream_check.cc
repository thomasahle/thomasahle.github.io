// Re-check of the post's FarmHash64 pairs against google/farmhash master src/farmhash.cc.
// Links farmhash.cc directly; calls the public API and the farmhashna/uo/xo(/te) namespaces.
#include "farmhash.h"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
namespace farmhashna { uint64_t Hash64(const char*, size_t); uint64_t Hash64WithSeed(const char*, size_t, uint64_t); uint64_t Hash64WithSeeds(const char*, size_t, uint64_t, uint64_t); }
namespace farmhashuo { uint64_t Hash64WithSeed(const char*, size_t, uint64_t); uint64_t Hash64WithSeeds(const char*, size_t, uint64_t, uint64_t); }
namespace farmhashxo { uint64_t Hash64WithSeed(const char*, size_t, uint64_t); uint64_t Hash64WithSeeds(const char*, size_t, uint64_t, uint64_t); }
#ifdef HAVE_TE
namespace farmhashte { uint64_t Hash64WithSeed(const char*, size_t, uint64_t); uint64_t Hash64WithSeeds(const char*, size_t, uint64_t, uint64_t); }
#endif
static uint64_t rng_s[4];
static inline uint64_t rotl64(uint64_t x, int k){ return (x<<k)|(x>>(64-k)); }
static uint64_t splitmix64(uint64_t *st){ uint64_t z=(*st+=0x9e3779b97f4a7c15ULL); z=(z^(z>>30))*0xbf58476d1ce4e5b9ULL; z=(z^(z>>27))*0x94d049bb133111ebULL; return z^(z>>31); }
static void rng_seed(uint64_t s){ for(int i=0;i<4;i++) rng_s[i]=splitmix64(&s); }
static inline uint64_t rng_next(){ uint64_t r=rotl64(rng_s[1]*5,7)*9; uint64_t t=rng_s[1]<<17; rng_s[2]^=rng_s[0]; rng_s[3]^=rng_s[1]; rng_s[1]^=rng_s[2]; rng_s[0]^=rng_s[3]; rng_s[2]^=t; rng_s[3]=rotl64(rng_s[3],45); return r; }
static std::string unhex(const char*h){ std::string o; for(size_t i=0;h[i]&&h[i+1];i+=2){ unsigned b; sscanf(h+i,"%2x",&b); o.push_back((char)b);} return o; }
struct P{ const char*name,*a,*b; };
int main(int argc,char**argv){
  int lg = argc>1 ? atoi(argv[1]) : 26; uint64_t N=1ULL<<lg; uint64_t rs = argc>2? strtoull(argv[2],0,0):1;
  P ps[3]={{"A (8 B rho)","574522e6dce98176","0df3b1f900d36296"},
           {"B (32 B closed form)","726c70206661726d706c61696e683332acc3a05fcc8dbead6d1998c94a300be8","736c70206661726de0d8c2d2dcd06664cdeab4eacd29799b6803df46f137a3cd"},
           {"C (Peters)","6f726c702d6661726d6861736836342d3f56724a404c3779747a776865616161","6f726c702d6661726d6861736836342d703360215351627d666d786865616161"}};
  const uint64_t ex=0x82bdf567d8ebbf4fULL; int bad=0;
  printf("google/farmhash master src/farmhash.cc; N=2^%d seeds per pair per interface (xoshiro256**, seed %llu)\n",lg,(unsigned long long)rs);
  for(auto&p:ps){ std::string a=unhex(p.a), b=unhex(p.b);
    uint64_t ua=farmhashna::Hash64(a.data(),a.size()), ub=farmhashna::Hash64(b.data(),b.size());
    uint64_t pa=farmhash::Hash64(a.data(),a.size()), pb=farmhash::Hash64(b.data(),b.size());
    printf("pair %s: len %zu/%zu  farmhashna::Hash64 %016llx %016llx %s   public farmhash::Hash64 %016llx %016llx %s\n",p.name,a.size(),b.size(),(unsigned long long)ua,(unsigned long long)ub,ua==ub?"EQUAL":"differ",(unsigned long long)pa,(unsigned long long)pb,pa==pb?"EQUAL":"differ");
    uint64_t ha=farmhash::Hash64WithSeed(a.data(),a.size(),ex), hb=farmhash::Hash64WithSeed(b.data(),b.size(),ex);
    printf("  explicit seed %016llx: farmhash::Hash64WithSeed %016llx %016llx %s\n",(unsigned long long)ex,(unsigned long long)ha,(unsigned long long)hb,ha==hb?"COLLIDE":"NO");
    if(ha!=hb) bad++;
    uint64_t c1=0,c2=0,c3=0,c4=0,c5=0,c6=0,c7=0,c8=0;
    rng_seed(rs);
    for(uint64_t i=0;i<N;i++){ uint64_t s=rng_next();
      c1+= farmhash::Hash64WithSeed(a.data(),a.size(),s)==farmhash::Hash64WithSeed(b.data(),b.size(),s);
      c3+= farmhashuo::Hash64WithSeed(a.data(),a.size(),s)==farmhashuo::Hash64WithSeed(b.data(),b.size(),s);
      c5+= farmhashxo::Hash64WithSeed(a.data(),a.size(),s)==farmhashxo::Hash64WithSeed(b.data(),b.size(),s);
#ifdef HAVE_TE
      c7+= farmhashte::Hash64WithSeed(a.data(),a.size(),s)==farmhashte::Hash64WithSeed(b.data(),b.size(),s);
#endif
    }
    rng_seed(rs);
    for(uint64_t i=0;i<N;i++){ uint64_t s0=rng_next(), s1=rng_next();
      c2+= farmhash::Hash64WithSeeds(a.data(),a.size(),s0,s1)==farmhash::Hash64WithSeeds(b.data(),b.size(),s0,s1);
      c4+= farmhashuo::Hash64WithSeeds(a.data(),a.size(),s0,s1)==farmhashuo::Hash64WithSeeds(b.data(),b.size(),s0,s1);
      c6+= farmhashxo::Hash64WithSeeds(a.data(),a.size(),s0,s1)==farmhashxo::Hash64WithSeeds(b.data(),b.size(),s0,s1);
#ifdef HAVE_TE
      c8+= farmhashte::Hash64WithSeeds(a.data(),a.size(),s0,s1)==farmhashte::Hash64WithSeeds(b.data(),b.size(),s0,s1);
#endif
    }
    printf("  farmhash::Hash64WithSeed  %llu/%llu   farmhash::Hash64WithSeeds %llu/%llu\n",(unsigned long long)c1,(unsigned long long)N,(unsigned long long)c2,(unsigned long long)N);
    printf("  farmhashuo::Hash64WithSeed %llu/%llu  farmhashuo::Hash64WithSeeds %llu/%llu\n",(unsigned long long)c3,(unsigned long long)N,(unsigned long long)c4,(unsigned long long)N);
    printf("  farmhashxo::Hash64WithSeed %llu/%llu  farmhashxo::Hash64WithSeeds %llu/%llu\n",(unsigned long long)c5,(unsigned long long)N,(unsigned long long)c6,(unsigned long long)N);
#ifdef HAVE_TE
    printf("  farmhashte::Hash64WithSeed %llu/%llu  farmhashte::Hash64WithSeeds %llu/%llu\n",(unsigned long long)c7,(unsigned long long)N,(unsigned long long)c8,(unsigned long long)N);
    if(c7!=N||c8!=N) bad++;
#endif
    if(c1!=N||c2!=N||c3!=N||c4!=N||c5!=N||c6!=N) bad++;
  }
  printf("result: %s\n", bad? "SOME NON-COLLIDING SEEDS":"all pairs collide for every sampled seed on every interface");
  return bad?1:0;
}
