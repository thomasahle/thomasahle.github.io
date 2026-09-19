// Reproduce the SMHasher3 verification procedure (lib/Hashinfo.cpp _ComputedVerifyImpl):
// keys {}, {0}, {0,1}, ... {0..254} with seed 256-i, then hash the concatenated outputs with seed 0;
// first 4 bytes LE = verification value.
#include <stdio.h>
#include <stdlib.h>
#include "a5.h"
static void put64(uint8_t *p, uint64_t v){ memcpy(p,&v,8); }
int main(void){
    uint8_t key[256]; uint8_t hashes[16*256]; memset(key,0,256);
    // a5hash 64
    for (int i=0;i<256;i++){ put64(hashes+8*i, a5hash64(key,i,256-i)); key[i]=(uint8_t)i; }
    uint64_t t = a5hash64(hashes, 8*256, 0);
    printf("a5hash      computed 0x%08X expected 0xADDE79B3 %s\n", (uint32_t)t, (uint32_t)t==0xADDE79B3?"OK":"FAIL");
    // a5hash_128 (16-byte output: lo then hi)
    memset(key,0,256);
    for (int i=0;i<256;i++){ uint64_t h; uint64_t l = a5hash128(key,i,256-i,&h); put64(hashes+16*i,l); put64(hashes+16*i+8,h); key[i]=(uint8_t)i; }
    { uint64_t h; uint64_t l = a5hash128(hashes,16*256,0,&h);
      printf("a5hash_128  computed 0x%08X expected 0x89406B11 %s\n", (uint32_t)l, (uint32_t)l==0x89406B11?"OK":"FAIL"); }
    // a5hash_128__64 (truncated, rh==NULL)
    memset(key,0,256);
    for (int i=0;i<256;i++){ put64(hashes+8*i, a5hash128(key,i,256-i,NULL)); key[i]=(uint8_t)i; }
    t = a5hash128(hashes, 8*256, 0, NULL);
    printf("a5hash_128_64 computed 0x%08X expected 0x14AD402C %s\n", (uint32_t)t, (uint32_t)t==0x14AD402C?"OK":"FAIL");
    return 0;
}
