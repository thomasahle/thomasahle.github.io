/* Independent driver linked against upstream fasthash.c (ztanml/fast-hash HEAD). */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "fasthash.h"
static uint64_t sm64(uint64_t *s){uint64_t z=(*s+=0x9e3779b97f4a7c15ULL);z=(z^(z>>30))*0xbf58476d1ce4e5b9ULL;z=(z^(z>>27))*0x94d049bb133111ebULL;return z^(z>>31);}
static int unhex(const char*s,unsigned char*o){size_t n=strlen(s)/2;for(size_t i=0;i<n;i++){unsigned v;sscanf(s+2*i,"%2x",&v);o[i]=v;}return n;}
int main(int argc,char**argv){
  int lg=argc>1?atoi(argv[1]):20; uint64_t N=1ULL<<lg;
  const char*P[3][2]={{"00000000000000","5e177be7d1d175f3"},{"00000000000000000000000000000000","f69e1c7bcf4db16ff69e1c7bcf4db16f"},{"00","2cb6512f74662d63"}};
  for(int p=0;p<3;p++){
    unsigned char a[64],b[64]; int na=unhex(P[p][0],a), nb=unhex(P[p][1],b);
    printf("pair %d: %s (%dB) vs %s (%dB)\n",p,P[p][0],na,P[p][1],nb);
    printf("  seed 0x0123456789abcdef: fh64 %016llx / %016llx  fh32(u32 seed) %08x / %08x\n",
      (unsigned long long)fasthash64(a,na,0x0123456789abcdefULL),(unsigned long long)fasthash64(b,nb,0x0123456789abcdefULL),
      fasthash32(a,na,(uint32_t)0x89abcdefu),fasthash32(b,nb,(uint32_t)0x89abcdefu));
    uint64_t c64=0,c32=0,n64=0,n32=0; uint64_t bad64=0,bad32=0; int have64=0,have32=0;
    #pragma omp parallel for reduction(+:c64,c32,n64,n32) schedule(static)
    for(uint64_t t=0;t<N;t++){
      uint64_t s=t*0x9e3779b97f4a7c15ULL+0x1234567ULL; uint64_t seed=sm64(&s);
      if(fasthash64(a,na,seed)==fasthash64(b,nb,seed)) c64++; else { n64++; if(!have64){have64=1;bad64=seed;} }
      uint32_t s32=(uint32_t)seed;
      if(fasthash32(a,na,s32)==fasthash32(b,nb,s32)) c32++; else { n32++; if(!have32){have32=1;bad32=s32;} }
    }
    printf("  fasthash64 uniform 64-bit seed: %llu/%llu collisions, %llu non-collisions%s%016llx\n",(unsigned long long)c64,(unsigned long long)N,(unsigned long long)n64, n64?" first bad seed ":"",(unsigned long long)bad64);
    printf("  fasthash32 uniform 32-bit seed: %llu/%llu collisions, %llu non-collisions%s%016llx\n",(unsigned long long)c32,(unsigned long long)N,(unsigned long long)n32, n32?" first bad seed ":"",(unsigned long long)bad32);
  }
  /* 0-vs-b check: length 0 takes no message step, so it should NOT collide generically */
  unsigned char z[8]={0x5e,0x17,0x7b,0xe7,0xd1,0xd1,0x75,0xf3}; uint64_t c0=0; for(uint64_t t=0;t<1000000;t++){uint64_t s=t;uint64_t seed=sm64(&s); if(fasthash64(z,0,seed)==fasthash64(z,8,seed)) c0++;}
  printf("control: 0B vs 8B(5e17..) collisions %llu/1000000\n",(unsigned long long)c0);
  return 0;
}
