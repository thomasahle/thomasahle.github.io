// Check the post's CityHash64 pair A (and pair B) against the UPSTREAM google/cityhash src/city.cc.
#include "city.h"
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <cinttypes>
#include <cstdlib>
static uint64_t sm(uint64_t *st){uint64_t z=(*st+=0x9e3779b97f4a7c15ULL);z=(z^(z>>30))*0xbf58476d1ce4e5b9ULL;z=(z^(z>>27))*0x94d049bb133111ebULL;return z^(z>>31);}
static size_t unhex(const char*h,unsigned char*o){size_t n=strlen(h)/2;for(size_t i=0;i<n;i++){unsigned b;sscanf(h+2*i,"%2x",&b);o[i]=(unsigned char)b;}return n;}
static uint32_t smh3_verif(){unsigned char key[256],hashes[8*256];memset(key,0,256);
 for(int i=0;i<256;i++){uint64 h=CityHash64WithSeed((const char*)key,i,256-i);for(int j=0;j<8;j++)hashes[8*i+j]=(unsigned char)(h>>(8*j));key[i]=(unsigned char)i;}
 return (uint32_t)CityHash64WithSeed((const char*)hashes,2048,0);}
int main(int argc,char**argv){
 uint64_t N=argc>1?strtoull(argv[1],0,0):(1ULL<<24);
 printf("upstream city.cc: SMHasher3 verification value = 0x%08X (SMHasher3 expects 0x5FABC5C5)\n",smh3_verif());
 const char* pairs[2][2]={{"a01109025ea76be1","020bd424b04ae555"},
  {"436974794861736836342d6f6b21212183454502d40dfa393030303030303030","436974794861736836342d6f6b21212183ff1a7c2ab06b6a3030303030303031"}};
 int ok=1;
 for(int p=0;p<2;p++){unsigned char m1[64],m2[64];size_t l=unhex(pairs[p][0],m1);unhex(pairs[p][1],m2);
  uint64 u1=CityHash64((const char*)m1,l),u2=CityHash64((const char*)m2,l);
  printf("pair %c (%zu B): unseeded %016" PRIx64 " %016" PRIx64 " %s\n",'A'+p,l,(uint64_t)u1,(uint64_t)u2,u1==u2?"EQUAL":"DIFFER");
  uint64_t fixed[3]={0,1,p==0?0x6637c1ce6357a2c8ULL:0xcbd18ebcd1f9b00dULL};
  for(int i=0;i<3;i++){uint64 a=CityHash64WithSeed((const char*)m1,l,fixed[i]),b=CityHash64WithSeed((const char*)m2,l,fixed[i]);
   printf("  seed %016" PRIx64 ": %016" PRIx64 " %016" PRIx64 " %s\n",fixed[i],(uint64_t)a,(uint64_t)b,a==b?"COLLIDE":"differ");if(a!=b)ok=0;}
  uint64_t st=0x5eed5eed5eed5eedULL,hits=0,hits2=0;
  for(uint64_t i=0;i<N;i++){uint64_t s=sm(&st);if(CityHash64WithSeed((const char*)m1,l,s)==CityHash64WithSeed((const char*)m2,l,s))hits++;
   uint64_t s0=sm(&st),s1=sm(&st);if(CityHash64WithSeeds((const char*)m1,l,s0,s1)==CityHash64WithSeeds((const char*)m2,l,s0,s1))hits2++;}
  printf("  random 64-bit seeds: N=%" PRIu64 " collisions=%" PRIu64 "; random (seed0,seed1) WithSeeds: N=%" PRIu64 " collisions=%" PRIu64 "\n",N,hits,N,hits2);
  if(hits!=N||hits2!=N)ok=0;}
 // control
 {unsigned char m1[8],m3[8];unhex("a01109025ea76be1",m1);memcpy(m3,m1,8);m3[7]^=1;uint64_t st=1,hits=0;for(uint64_t i=0;i<N;i++){uint64_t s=sm(&st);if(CityHash64WithSeed((const char*)m1,8,s)==CityHash64WithSeed((const char*)m3,8,s))hits++;}
  printf("control (last bit flipped): N=%" PRIu64 " collisions=%" PRIu64 "\n",N,hits);if(hits)ok=0;}
 printf("result: %s\n",ok?"ALL CHECKS PASSED":"FAILED");return ok?0:1;}
