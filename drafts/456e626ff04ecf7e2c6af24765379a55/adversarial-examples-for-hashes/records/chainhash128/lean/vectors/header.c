#include "../../reference/chainhash128.h"
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
int main(void) {
 unsigned model; size_t n; unsigned char keybytes[656];
 while(scanf("%u %zu",&model,&n)==2){
  unsigned count=model?10:41;
  for(unsigned i=0;i<count;i++){
   uint64_t lo,hi; if(scanf("%" SCNu64 " %" SCNu64,&lo,&hi)!=2)return 2;
   chainhash128_store(keybytes+16*i,ch128_make(lo,hi));
  }
  chainhash128_key k=model?chainhash128_key_from_bytes(keybytes):chainhash128_key_from_ideal_bytes(keybytes);
  unsigned char *m=malloc(n+1);
  for(size_t i=0;i<n;i++){unsigned x;if(scanf("%u",&x)!=1)return 3;m[i]=x;}
  ch128_word a=chainhash128_portable(&k,m,n),b=chainhash128(&k,m,n);
  if(!ch128_equal(a,b))return 4;
  printf("%" PRIu64 " %" PRIu64 "\n",a.lo,a.hi);free(m);
 }
 return 0;
}
