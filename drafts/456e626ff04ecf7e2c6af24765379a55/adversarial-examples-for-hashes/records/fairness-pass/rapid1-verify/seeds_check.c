#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include "rapidhash.h"
static size_t dec(const char*h,uint8_t*o){size_t n=strlen(h)/2;for(size_t i=0;i<n;i++){unsigned x;sscanf(h+2*i,"%2x",&x);o[i]=x;}return n;}
int main(void){
  uint8_t A[32],B[32]; dec("9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c",A); dec("642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c",B);
  struct{uint64_t seed,out;} r[12]={{0x3788f2419a81e2d6,0x8f7a71ffebd4a14b},{0x81dd9e7f3f48d8ed,0xc39e0a2ff7d0a541},{0x95460f49e8f81b2b,0x7daddf1d3fc89978},{0xc430bf5d8e43fb94,0x69ebfd3d25766e18},{0xc0111c34a0e81e29,0xedb6fe37c7b58ccc},{0xf1145666200f14c9,0x74b26912215e865c},{0x622301fab58dc671,0x54b9a78c139481f3},{0xb65246f727df0f01,0xce22a3ad58f301f4},{0x0351a2c051a1d050,0xacbb91f915c59f22},{0xfafd76ed2974427d,0x1792b03bf04f3d6c},{0x77f23d6bcf87c93a,0xd35efd3a667083e4},{0xa68df58c0ad5aad9,0xc8631de951251cff}};
  int ok=0; for(int i=0;i<12;i++){uint64_t x=rapidhash_withSeed(A,32,r[i].seed),y=rapidhash_withSeed(B,32,r[i].seed); int p=(x==y&&x==r[i].out); ok+=p; printf("seed %016" PRIx64 ": %016" PRIx64 " %016" PRIx64 " %s\n",r[i].seed,x,y,p?"PASS":"FAIL");}
  uint64_t z0a=rapidhash_withSeed(A,32,0),z0b=rapidhash_withSeed(B,32,0); printf("seed 0: %016" PRIx64 " %016" PRIx64 " (record 5c8ae6952284254e 8623b7cfca76b625)\n",z0a,z0b);
  printf("%d/12 recorded colliding seeds reproduce on upstream v1.0\n",ok); return ok!=12; }
