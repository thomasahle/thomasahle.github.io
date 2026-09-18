// Independent driver: includes the upstream mx3.h verbatim (jonmaiga/mx3, HEAD = v3.0.0)
// and checks the three recorded pairs over structured seeds + 2^N random seeds.
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <vector>
#include "mx3/mx3.h"

static uint64_t splitmix64(uint64_t &s){ uint64_t z=(s+=0x9E3779B97F4A7C15ull); z=(z^(z>>30))*0xBF58476D1CE4E5B9ull; z=(z^(z>>27))*0x94D049BB133111EBull; return z^(z>>31); }
static int unhex(const char*h, uint8_t*out){ size_t n=strlen(h)/2; for(size_t i=0;i<n;i++){unsigned v; sscanf(h+2*i,"%2x",&v); out[i]=(uint8_t)v;} return (int)n; }

struct Pair { const char* name; const char* a; const char* b; uint64_t exseed; uint64_t exout; };
static const Pair pairs[] = {
  {"1B vs 8B", "00", "b6cbe3b809a4265c", 0x2cb0f69f4abea221ull, 0x730d2d8dbe4d729eull},
  {"7B vs 8B", "00000000000000", "f55d0131fc859d54", 0x3bb548a553e612baull, 0xeebfade252d2c35full},
  {"16B vs 16B", "00000000000000000000000000000000", "01000000000000000234f950e6e40a19", 0x0433ebf21b5f33f3ull, 0xa2c7b06bd25d7036ull},
};

static uint64_t g(uint64_t x){ x*=mx3::C; x^=x>>39; return x*mx3::C; }
static uint64_t modinv64(uint64_t a){ uint64_t x=a; for(int i=0;i<6;i++) x*=2-a*x; return x; }
static uint64_t ginv(uint64_t y){ uint64_t Ci=modinv64(mx3::C); uint64_t z=y*Ci; uint64_t u=z^(z>>39); return u*Ci; }

int main(int argc,char**argv){
  int lg = argc>1? atoi(argv[1]):20;
  uint64_t base = argc>2? strtoull(argv[2],0,0): 0x5DEECE66DA3C0F11ull;
  int fails=0;
  // 1. algebra: partner of byte 0 and of 7 zero bytes and of (0,0)
  uint64_t w1 = ginv(g(0) + mx3::C*(g(2)-g(9)));
  uint64_t w7 = ginv(g(0) + mx3::C*(g(8)-g(9)));
  uint64_t w16 = ginv(mx3::C*(g(0)-g(1)));
  printf("g^-1 partners: 1B->%016llx (expect 5c26a409b8e3cbb6)  7B->%016llx (expect 549d85fc31015df5)  16B second word->%016llx (expect 190ae4e650f93402)\n",(unsigned long long)w1,(unsigned long long)w7,(unsigned long long)w16);
  if(w1!=0x5c26a409b8e3cbb6ull||w7!=0x549d85fc31015df5ull||w16!=0x190ae4e650f93402ull){puts("ALGEBRA MISMATCH");fails++;}
  // 2. recorded examples and structured seeds
  alignas(16) uint8_t A[64], B[64];
  std::vector<uint64_t> structured={0,1,2,3,7,8,9,16,17,0xffffffffffffffffull,0x8000000000000000ull,mx3::C,modinv64(mx3::C),0xdeadbeefcafef00dull,0x0123456789abcdefull};
  for(int k=0;k<64;k++) structured.push_back(1ull<<k);
  for(const Pair&p:pairs){
    memset(A,0,64);memset(B,0,64);
    int la=unhex(p.a,A), lb=unhex(p.b,B);
    uint64_t ha=mx3::hash(A,la,p.exseed), hb=mx3::hash(B,lb,p.exseed);
    printf("%s: seed %016llx -> %016llx / %016llx (recorded %016llx) %s\n",p.name,(unsigned long long)p.exseed,(unsigned long long)ha,(unsigned long long)hb,(unsigned long long)p.exout,(ha==hb&&ha==p.exout)?"OK":"FAIL");
    if(!(ha==hb&&ha==p.exout)) fails++;
    int sbad=0; for(uint64_t s:structured) if(mx3::hash(A,la,s)!=mx3::hash(B,lb,s)) sbad++;
    printf("  structured seeds: %zu tried, %d non-colliding\n",structured.size(),sbad); fails+=sbad;
    uint64_t N=1ull<<lg, coll=0;
    #pragma omp parallel reduction(+:coll)
    {
      alignas(16) uint8_t a2[64], b2[64]; memcpy(a2,A,64); memcpy(b2,B,64);
      #pragma omp for schedule(static)
      for(long long i=0;i<(long long)N;i++){ uint64_t st=base+(uint64_t)i*0x9E3779B97F4A7C15ull; uint64_t s=splitmix64(st); if(mx3::hash(a2,la,s)==mx3::hash(b2,lb,s)) coll++; }
    }
    printf("  random seeds: %llu/%llu collide\n",(unsigned long long)coll,(unsigned long long)N); if(coll!=N) fails++;
  }
  // 3. every one-byte message has an 8-byte partner; every 2..7-byte tail too (random tails)
  { uint64_t s0=base^0xabcdefull; int bad=0, tot=0;
    for(int b=0;b<256;b++){ uint64_t w=ginv(g((uint64_t)b)+mx3::C*(g(2)-g(9))); memset(A,0,64);memset(B,0,64); A[0]=(uint8_t)b; memcpy(B,&w,8);
      for(int t=0;t<16;t++){ uint64_t s=splitmix64(s0); tot++; if(mx3::hash(A,1,s)!=mx3::hash(B,8,s)) bad++; } }
    for(int len=2;len<=7;len++) for(int r=0;r<1024;r++){ memset(A,0,64);memset(B,0,64); uint64_t tail=splitmix64(s0)&((1ull<<(8*len))-1); memcpy(A,&tail,8); uint64_t w=ginv(g(tail)+mx3::C*(g(len+1)-g(9))); memcpy(B,&w,8);
      for(int t=0;t<16;t++){ uint64_t s=splitmix64(s0); tot++; if(mx3::hash(A,len,s)!=mx3::hash(B,8,s)) bad++; } }
    printf("partner construction: %d checks, %d failures\n",tot,bad); fails+=bad; }
  printf(fails? "RESULT: %d FAILURES\n":"RESULT: ALL CHECKS PASS\n",fails);
  return fails?1:0;
}
