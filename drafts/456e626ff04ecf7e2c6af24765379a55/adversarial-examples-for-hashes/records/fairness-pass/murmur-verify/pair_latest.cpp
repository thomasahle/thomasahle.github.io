// Harness: the post's two MurmurHash3_x64_128 pairs against the LATEST aappleby reference source.
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include "MurmurHash3.h"
static int unhex(const char* s, uint8_t* out){int n=0;for(;s[0]&&s[1];s+=2,n++){unsigned v;sscanf(s,"%2x",&v);out[n]=(uint8_t)v;}return n;}
struct P{const char*a;const char*b;};
int main(int argc,char**argv){
  int bits = argc>1?atoi(argv[1]):24;
  P pairs[2]={{"000000000000000000000000000000000000000000000000","60a0fd219e0ef2cd42e098d38ee8728d000000007d4dce82"},
              {"0000000000000000000000000000000000000000000000000000000000000000","60a0fd219e0ef2cd000000000000000060a0fd2121c1234b000000c01f8b7776"}};
  for(int p=0;p<2;p++){
    uint8_t A[64],B[64],C[64]; int la=unhex(pairs[p].a,A), lb=unhex(pairs[p].b,B);
    memcpy(C,B,lb); C[lb-1]^=1;
    uint64_t ha[2],hb[2];
    MurmurHash3_x64_128(A,la,0,ha); MurmurHash3_x64_128(B,lb,0,hb);
    printf("pair %d: len %d/%d  H(m,seed0)=%016llx %016llx  H(m',seed0)=%016llx %016llx\n",p+1,la,lb,(unsigned long long)ha[0],(unsigned long long)ha[1],(unsigned long long)hb[0],(unsigned long long)hb[1]);
    uint64_t N = 1ull<<bits, coll=0, ctrl=0;
    #pragma omp parallel for reduction(+:coll,ctrl) schedule(static)
    for(uint64_t s=0;s<N;s++){
      uint64_t x[2],y[2],z[2];
      MurmurHash3_x64_128(A,la,(uint32_t)s,x); MurmurHash3_x64_128(B,lb,(uint32_t)s,y); MurmurHash3_x64_128(C,lb,(uint32_t)s,z);
      if(x[0]==y[0]&&x[1]==y[1]) coll++;
      if(x[0]==z[0]&&x[1]==z[1]) ctrl++;
    }
    printf("pair %d: seeds 0..2^%d-1: collisions %llu/%llu, control(last bit flipped) %llu/%llu\n",p+1,bits,(unsigned long long)coll,(unsigned long long)N,(unsigned long long)ctrl,(unsigned long long)N);
  }
  return 0;
}
