#define XXH_INLINE_ALL
#include HDR
#include <stdint.h>
#define CAT2(a,b) a##b
#define CAT(a,b) CAT2(a,b)
uint64_t CAT(h_,SUF)(const void*p,size_t n,uint64_t s){return XXH3_64bits_withSeed(p,n,s);}
uint64_t CAT(u_,SUF)(const void*p,size_t n){return XXH3_64bits(p,n);}
void CAT(h128_,SUF)(const void*p,size_t n,uint64_t s,uint64_t*o){XXH128_hash_t h=XXH3_128bits_withSeed(p,n,s);o[0]=h.low64;o[1]=h.high64;}
