// Checks the post's mx3 v3 pairs against the UPSTREAM header (jonmaiga/mx3 master = v3.0.0).
#include "mx3/mx3.h"
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <omp.h>
static const uint64_t C = 0xbea225f9eb34556dULL;
static uint64_t inv64(uint64_t a){ uint64_t x=a; for(int i=0;i<6;i++) x*=2-a*x; return x; }
static uint64_t g(uint64_t x){ x*=C; x^=x>>39; return x*C; }
static uint64_t ginv(uint64_t y){ uint64_t Ci=inv64(C); y*=Ci; y^=y>>39; return y*Ci; }
static uint64_t splitmix(uint64_t &s){ uint64_t z=(s+=0x9e3779b97f4a7c15ULL); z=(z^(z>>30))*0xbf58476d1ce4e5b9ULL; z=(z^(z>>27))*0x94d049bb133111ebULL; return z^(z>>31); }
struct Pair { const char* name; const uint8_t* a; size_t na; const uint8_t* b; size_t nb; uint64_t seed; uint64_t expect; };
alignas(8) static const uint8_t A1[8]={0x00};
alignas(8) static const uint8_t B1[8]={0xb6,0xcb,0xe3,0xb8,0x09,0xa4,0x26,0x5c};
alignas(8) static const uint8_t A7[8]={0,0,0,0,0,0,0,0};
alignas(8) static const uint8_t B7[8]={0xf5,0x5d,0x01,0x31,0xfc,0x85,0x9d,0x54};
alignas(8) static const uint8_t A16[16]={0};
alignas(8) static const uint8_t B16[16]={0x01,0,0,0,0,0,0,0,0x02,0x34,0xf9,0x50,0xe6,0xe4,0x0a,0x19};
int main(int argc,char**argv){
  unsigned lg = argc>1?atoi(argv[1]):30;
  uint64_t N = 1ULL<<lg;
  // sanity of ginv
  for(int i=0;i<1000;i++){ uint64_t s=i*0x9e37ULL+7; uint64_t x=splitmix(s); if(ginv(g(x))!=x){puts("ginv FAIL");return 1;} }
  printf("C^-1 = %016llx; g^-1(g(0)+C*(g(2)-g(9))) = %016llx (expect 5c26a409b8e3cbb6)\n",(unsigned long long)inv64(C),(unsigned long long)ginv(g(0)+C*(g(2)-g(9))));
  Pair pairs[]={
    {"1 vs 8 bytes",A1,1,B1,8,0x2cb0f69f4abea221ULL,0x730d2d8dbe4d729eULL},
    {"7 vs 8 bytes",A7,7,B7,8,0x3bb548a553e612baULL,0xeebfade252d2c35fULL},
    {"16 vs 16 bytes (claimant-only family member)",A16,16,B16,16,0,0},
  };
  int rc=0;
  for(auto&p:pairs){
    uint64_t ha=mx3::hash(p.a,p.na,p.seed), hb=mx3::hash(p.b,p.nb,p.seed);
    printf("\n%s: recorded seed %016llx H(M)=%016llx H(M')=%016llx %s\n",p.name,(unsigned long long)p.seed,(unsigned long long)ha,(unsigned long long)hb,
      ha==hb && (p.expect==0||ha==p.expect) ? "OK":"MISMATCH");
    if(!(ha==hb && (p.expect==0||ha==p.expect))) rc=1;
    // structured seeds
    uint64_t structured[80]; int ns=0;
    structured[ns++]=0; structured[ns++]=1; structured[ns++]=~0ULL; structured[ns++]=C; structured[ns++]=inv64(C);
    structured[ns++]=p.na; structured[ns++]=p.nb; structured[ns++]=p.na+1; structured[ns++]=p.nb+1;
    for(int k=0;k<64;k++) structured[ns++]=1ULL<<k;
    int bad=0; for(int i=0;i<ns;i++){ if(mx3::hash(p.a,p.na,structured[i])!=mx3::hash(p.b,p.nb,structured[i])){bad++; printf("  structured seed %016llx does NOT collide\n",(unsigned long long)structured[i]);} }
    printf("  structured seeds: %d/%d collide\n",ns-bad,ns); if(bad) rc=1;
    // random seeds, N total, per-thread splitmix streams
    uint64_t total=0, coll=0; int T=omp_get_max_threads();
    #pragma omp parallel reduction(+:total,coll)
    { int t=omp_get_thread_num(); uint64_t s=0x1234567ULL*(t+1)+lg; uint64_t per=N/omp_get_num_threads();
      for(uint64_t i=0;i<per;i++){ uint64_t seed=splitmix(s); total++; if(mx3::hash(p.a,p.na,seed)==mx3::hash(p.b,p.nb,seed)) coll++; } }
    printf("  random seeds (%d threads): %llu / %llu collide\n",T,(unsigned long long)coll,(unsigned long long)total); if(coll!=total) rc=1;
  }
  // every 1..7-byte tail has an 8-byte partner: w = g^-1(g(tail) + C*(g(t+1)-g(9)))
  printf("\npartner construction for every tail length 1..7 (256 one-byte messages exhaustive; 4096 random tails for 2..7), 64 random seeds each\n");
  for(int t=1;t<=7;t++){ int cnt=0, ok=0; uint64_t s=99+t;
    int trials = t==1?256:4096;
    for(int i=0;i<trials;i++){ alignas(8) uint8_t a[8]={0}; uint64_t v = t==1? (uint64_t)i : (splitmix(s) & ((1ULL<<(8*t))-1)); memcpy(a,&v,8);
      uint64_t w=ginv(g(v)+C*(g(t+1)-g(9))); alignas(8) uint8_t b[8]; memcpy(b,&w,8);
      cnt++; int good=1; for(int k=0;k<64;k++){ uint64_t seed=splitmix(s); if(mx3::hash(a,t,seed)!=mx3::hash(b,8,seed)) good=0; } ok+=good; }
    printf("  tail %d bytes: %d/%d messages have a colliding 8-byte partner for all 64 seeds\n",t,ok,cnt); if(ok!=cnt) rc=1; }
  puts(rc?"\nRESULT: FAIL":"\nRESULT: ALL CHECKS PASS"); return rc;
}
