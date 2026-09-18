// Independent re-check of the FarmHash64 key-free pairs against google/farmhash master,
// including the .cc directly so the internal namespaces (farmhashna/uo/xo/te) are reachable.
#include "farmhash.cc"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cinttypes>

static uint64_t s_[4];
static inline uint64_t rotl(uint64_t x, int k){ return (x<<k)|(x>>(64-k)); }
static uint64_t next(void){ uint64_t r = rotl(s_[1]*5,7)*9; uint64_t t=s_[1]<<17; s_[2]^=s_[0]; s_[3]^=s_[1]; s_[1]^=s_[2]; s_[0]^=s_[3]; s_[2]^=t; s_[3]=rotl(s_[3],45); return r; }
static void seed_rng(uint64_t x){ for(int i=0;i<4;i++){ x+=0x9e3779b97f4a7c15ULL; uint64_t z=x; z=(z^(z>>30))*0xbf58476d1ce4e5b9ULL; z=(z^(z>>27))*0x94d049bb133111ebULL; s_[i]=z^(z>>31);} }

static int unhex(const char* h, char* out){ size_t n=strlen(h)/2; for(size_t i=0;i<n;i++){ unsigned v; sscanf(h+2*i,"%2x",&v); out[i]=(char)v;} return (int)n; }

struct Pair { const char* name; const char* a; const char* b; };
static Pair pairs[] = {
  {"A (8 B, rho)", "574522e6dce98176", "0df3b1f900d36296"},
  {"B (32 B, closed form)", "726c70206661726d706c61696e683332acc3a05fcc8dbead6d1998c94a300be8", "736c70206661726de0d8c2d2dcd06664cdeab4eacd29799b6803df46f137a3cd"},
  {"C (Peters)", "6f726c702d6661726d6861736836342d3f56724a404c3779747a776865616161", "6f726c702d6661726d6861736836342d703360215351627d666d786865616161"},
};

typedef uint64_t (*F1)(const char*, size_t, uint64_t);
typedef uint64_t (*F2)(const char*, size_t, uint64_t, uint64_t);
struct Iface { const char* name; F1 f1; F2 f2; };

int main(int argc, char** argv){
  int lg = argc>1 ? atoi(argv[1]) : 24;
  uint64_t N = 1ULL<<lg;
  Iface ifs[] = {
    {"farmhash::(public)", farmhash::Hash64WithSeed, farmhash::Hash64WithSeeds},
    {"farmhashna", farmhashna::Hash64WithSeed, farmhashna::Hash64WithSeeds},
    {"farmhashuo", farmhashuo::Hash64WithSeed, farmhashuo::Hash64WithSeeds},
    {"farmhashxo", farmhashxo::Hash64WithSeed, farmhashxo::Hash64WithSeeds},
#if can_use_sse41 && x86_64
    {"farmhashte", farmhashte::Hash64WithSeed, farmhashte::Hash64WithSeeds},
#endif
  };
  int nif = sizeof(ifs)/sizeof(ifs[0]);
  printf("upstream_check: google/farmhash src/farmhash.cc included directly; NDEBUG %s; can_use_sse41=%d; N=2^%d seeds per interface\n",
#ifdef NDEBUG
   "defined",
#else
   "NOT defined (public wrapper applies DebugTweak)",
#endif
   (int)can_use_sse41, lg);
  printf("Peters vector: farmhashna::Hash64(\"orlp-farmhash64-?VrJ@L7ytzwheaaa\") = %" PRIu64 " (published 1337)\n",
         farmhashna::Hash64("orlp-farmhash64-?VrJ@L7ytzwheaaa", 32));
  int bad = 0;
  for (Pair& p : pairs) {
    char a[64], b[64]; int la = unhex(p.a, a), lb = unhex(p.b, b);
    uint64_t ha = farmhashna::Hash64(a, la), hb = farmhashna::Hash64(b, lb);
    printf("\npair %s: unseeded farmhashna::Hash64 %016" PRIx64 " / %016" PRIx64 " %s\n", p.name, ha, hb, ha==hb?"EQUAL":"DIFFER");
    const uint64_t es = 0x82bdf567d8ebbf4fULL;
    for (int i=0;i<nif;i++){
      uint64_t xa = ifs[i].f1(a,la,es), xb = ifs[i].f1(b,lb,es);
      printf("  %-20s explicit seed %016" PRIx64 ": %016" PRIx64 " / %016" PRIx64 " %s\n", ifs[i].name, es, xa, xb, xa==xb?"COLLIDE":"DIFFER");
      seed_rng(1);
      uint64_t c1=0,c2=0;
      for (uint64_t k=0;k<N;k++){
        uint64_t s0=next(), s1=next();
        if (ifs[i].f1(a,la,s0)==ifs[i].f1(b,lb,s0)) c1++;
        if (ifs[i].f2(a,la,s0,s1)==ifs[i].f2(b,lb,s0,s1)) c2++;
      }
      printf("  %-20s Hash64WithSeed %" PRIu64 "/%" PRIu64 "   Hash64WithSeeds %" PRIu64 "/%" PRIu64 "%s\n", ifs[i].name, c1, N, c2, N, (c1==N&&c2==N)?"":"   <-- NON-COLLIDING SEED FOUND");
      if (c1!=N||c2!=N||xa!=xb) bad++;
    }
  }
  printf("\nRESULT: %s\n", bad?"FAIL":"all pairs collide on every sampled seed on every interface");
  return bad?1:0;
}
