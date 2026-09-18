// Same pair under v1.0.0 / v2.0.0 mx3.h (checked out separately); expect NOT to collide.
#include <cstdio>
#include <cstdint>
#include <cstring>
#include "mx3.h"
static uint64_t splitmix64(uint64_t &s){ uint64_t z=(s+=0x9E3779B97F4A7C15ull); z=(z^(z>>30))*0xBF58476D1CE4E5B9ull; z=(z^(z>>27))*0x94D049BB133111EBull; return z^(z>>31); }
int main(){ alignas(16) uint8_t A[16]={0}, B[16]={0xb6,0xcb,0xe3,0xb8,0x09,0xa4,0x26,0x5c}; uint64_t s0=42, c=0, N=1<<20;
  for(uint64_t i=0;i<N;i++){ uint64_t s=splitmix64(s0); if(mx3::hash(A,1,s)==mx3::hash(B,8,s)) c++; }
  printf("%s: 1B/8B pair collides %llu/%llu; seed 0 -> %016llx / %016llx\n", VERNAME,(unsigned long long)c,(unsigned long long)N,(unsigned long long)mx3::hash(A,1,0),(unsigned long long)mx3::hash(B,8,0)); }
