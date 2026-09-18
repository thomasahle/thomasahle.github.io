// Pair check against the LATEST upstream aappleby/smhasher src/MurmurHash3.cpp
// (reference API: uint32_t seed, h1 = h2 = seed).  Enumerates all 2^32 seeds.
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "MurmurHash3.h"
static size_t unhex(const char *s, uint8_t *o){size_t n=strlen(s)/2;for(size_t i=0;i<n;i++){unsigned b;sscanf(s+2*i,"%2x",&b);o[i]=(uint8_t)b;}return n;}
int main(int argc,char**argv){
  const char* pairs[][2]={
   {"000000000000000000000000000000000000000000000000","60a0fd219e0ef2cd42e098d38ee8728d000000007d4dce82"},
   {"0000000000000000000000000000000000000000000000000000000000000000","60a0fd219e0ef2cd000000000000000060a0fd2121c1234b000000c01f8b7776"}};
  int log2n = argc>1 ? atoi(argv[1]) : 32;
  uint64_t N = 1ULL<<log2n;
  for(int p=0;p<2;p++){
    uint8_t a[64],b[64],c[64]; size_t la=unhex(pairs[p][0],a), lb=unhex(pairs[p][1],b);
    memcpy(c,b,lb); c[lb-1]^=1;
    uint64_t coll=0, ctrl=0;
    #pragma omp parallel for reduction(+:coll,ctrl) schedule(static)
    for(uint64_t s=0;s<N;s++){
      uint64_t ha[2],hb[2],hc[2];
      MurmurHash3_x64_128(a,(int)la,(uint32_t)s,ha);
      MurmurHash3_x64_128(b,(int)lb,(uint32_t)s,hb);
      MurmurHash3_x64_128(c,(int)lb,(uint32_t)s,hc);
      if(ha[0]==hb[0]&&ha[1]==hb[1]) coll++;
      if(ha[0]==hc[0]&&ha[1]==hc[1]) ctrl++;
    }
    uint64_t h0[2]; MurmurHash3_x64_128(a,(int)la,0u,h0);
    printf("pair %d (%zu bytes): collisions %llu / %llu seeds; control (last bit flipped) %llu / %llu; H(m,seed0) = %016llx %016llx\n",
      p+1,la,(unsigned long long)coll,(unsigned long long)N,(unsigned long long)ctrl,(unsigned long long)N,
      (unsigned long long)h0[0],(unsigned long long)h0[1]);
  }
  return 0;
}
