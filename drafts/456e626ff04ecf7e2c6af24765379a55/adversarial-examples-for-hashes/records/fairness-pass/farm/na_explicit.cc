#include <cstdio>
#include <cstdint>
#include <cstddef>
namespace farmhashna { uint64_t Hash64WithSeed(const char*, size_t, uint64_t); uint64_t Hash64WithSeeds(const char*, size_t, uint64_t, uint64_t); }
int main(){ const char a[8]={0x57,0x45,0x22,(char)0xe6,(char)0xdc,(char)0xe9,(char)0x81,0x76}; const char b[8]={0x0d,(char)0xf3,(char)0xb1,(char)0xf9,0x00,(char)0xd3,0x62,(char)0x96};
 uint64_t s=0x82bdf567d8ebbf4fULL; printf("farmhashna::Hash64WithSeed  %016llx %016llx\n",(unsigned long long)farmhashna::Hash64WithSeed(a,8,s),(unsigned long long)farmhashna::Hash64WithSeed(b,8,s)); }
