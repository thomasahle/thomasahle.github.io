/* Pair check linked against upstream ztanml/fast-hash fasthash.c at HEAD. */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <omp.h>
#include "fasthash.h"
static uint64_t splitmix(uint64_t *s){uint64_t z=(*s+=0x9e3779b97f4a7c15ULL);z=(z^(z>>30))*0xbf58476d1ce4e5b9ULL;z=(z^(z>>27))*0x94d049bb133111ebULL;return z^(z>>31);}
static size_t hx(const char*s,uint8_t*o){size_t n=strlen(s)/2;for(size_t i=0;i<n;i++){unsigned x;sscanf(s+2*i,"%2x",&x);o[i]=x;}return n;}
int main(int argc,char**argv){
  unsigned lg=argc>1?atoi(argv[1]):30; uint64_t N=1ULL<<lg;
  const char*P[3][2]={{"00000000000000","5e177be7d1d175f3"},{"00000000000000000000000000000000","f69e1c7bcf4db16ff69e1c7bcf4db16f"},{"00","2cb6512f74662d63"}};
  uint8_t a[16],b[16]; 
  for(int p=0;p<3;p++){
    size_t na=hx(P[p][0],a),nb=hx(P[p][1],b);
    printf("pair %d: M(%zu)=%s M'(%zu)=%s\n",p,na,P[p][0],nb,P[p][1]);
    printf("  seed 0123456789abcdef: h64 %016" PRIx64 " %016" PRIx64 " ; h32(seed32=89abcdef) %08x %08x\n",
      fasthash64(a,na,0x0123456789abcdefULL),fasthash64(b,nb,0x0123456789abcdefULL),fasthash32(a,na,0x89abcdef),fasthash32(b,nb,0x89abcdef));
    uint64_t c64=0,c32=0,n64=0,n32=0;
    #pragma omp parallel reduction(+:c64,c32,n64,n32)
    { int T=omp_get_num_threads(),t=omp_get_thread_num(); uint64_t s=0x1234567ULL+t*0x9e37ULL;
      for(uint64_t i=t;i<N;i+=T){ uint64_t seed=splitmix(&s);
        if(fasthash64(a,na,seed)==fasthash64(b,nb,seed)) c64++; else n64++;
        if(fasthash32(a,na,(uint32_t)seed)==fasthash32(b,nb,(uint32_t)seed)) c32++; else n32++; } }
    printf("  fasthash64 (64-bit seed): collisions %" PRIu64 " / %" PRIu64 " (non-collisions %" PRIu64 ")\n",c64,N,n64);
    printf("  fasthash32 (32-bit seed): collisions %" PRIu64 " / %" PRIu64 " (non-collisions %" PRIu64 ")\n",c32,N,n32);
  }
  return 0;
}
