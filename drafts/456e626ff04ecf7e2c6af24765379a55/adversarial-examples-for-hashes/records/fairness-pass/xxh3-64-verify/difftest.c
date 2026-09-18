/* Differential test: does XXH3_64bits_withSeed (and XXH3_64bits, XXH3_128bits_withSeed) agree between two headers?
 * Each header is compiled into its own TU with a namespace prefix via XXH_NAMESPACE. */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
uint64_t h_a(const void*,size_t,uint64_t); uint64_t h_b(const void*,size_t,uint64_t);
uint64_t u_a(const void*,size_t); uint64_t u_b(const void*,size_t);
void h128_a(const void*,size_t,uint64_t,uint64_t*); void h128_b(const void*,size_t,uint64_t,uint64_t*);
static uint64_t sm(uint64_t *s){uint64_t z=(*s+=UINT64_C(0x9e3779b97f4a7c15));z=(z^(z>>30))*UINT64_C(0xbf58476d1ce4e5b9);z=(z^(z>>27))*UINT64_C(0x94d049bb133111eb);return z^(z>>31);}
int main(void){uint64_t s=42,bad=0,n=0;uint8_t buf[4096];
 for(size_t len=0;len<=1200;len++){for(int r=0;r<2000;r++){for(size_t i=0;i<len;i+=8){uint64_t w=sm(&s);memcpy(buf+i,&w,(len-i)<8?(len-i):8);}
  uint64_t seed=(r&1)?sm(&s):(r&2?0:sm(&s)&0xffff);uint64_t x[2],y[2];h128_a(buf,len,seed,x);h128_b(buf,len,seed,y);
  if(h_a(buf,len,seed)!=h_b(buf,len,seed)||u_a(buf,len)!=u_b(buf,len)||x[0]!=y[0]||x[1]!=y[1])bad++;n++;}}
 printf("differential: %llu cases, %llu mismatches\n",(unsigned long long)n,(unsigned long long)bad);return bad!=0;}
