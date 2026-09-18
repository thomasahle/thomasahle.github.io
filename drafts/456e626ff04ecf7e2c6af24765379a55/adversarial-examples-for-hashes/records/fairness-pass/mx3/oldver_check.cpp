// Does the v3 pair also collide under v1.0.0 / v2.0.0? (expected: no; the seeding differs)
#include <cstdio>
#include <cstdint>
#include <cstring>
namespace v1 {
#include "mx3_v1/mx3.h"
}
namespace v2 {
#include "mx3_v2/mx3.h"
}
alignas(8) static const uint8_t A1[8]={0x00};
alignas(8) static const uint8_t B1[8]={0xb6,0xcb,0xe3,0xb8,0x09,0xa4,0x26,0x5c};
static uint64_t splitmix(uint64_t &s){ uint64_t z=(s+=0x9e3779b97f4a7c15ULL); z=(z^(z>>30))*0xbf58476d1ce4e5b9ULL; z=(z^(z>>27))*0x94d049bb133111ebULL; return z^(z>>31); }
int main(){ uint64_t s=1; int c1=0,c2=0; const int N=1<<20;
  for(int i=0;i<N;i++){ uint64_t seed=splitmix(s);
    if(v1::mx3::hash(A1,1,seed)==v1::mx3::hash(B1,8,seed)) c1++;
    if(v2::mx3::hash(A1,1,seed)==v2::mx3::hash(B1,8,seed)) c2++; }
  printf("v3 pair (1 vs 8 bytes) under v1.0.0: %d/%d collide; under v2.0.0: %d/%d collide\n",c1,N,c2,N); return 0; }
