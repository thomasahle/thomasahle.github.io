/* SMHasher3 _ComputedVerifyImpl procedure applied to upstream hash-garage nmhash.h at HEAD. */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "hash-garage/nmhash.h"
int main(void){
    uint8_t key[256], hashes[256*4];
    for(int i=0;i<256;i++){ key[i]=(uint8_t)i; uint32_t h=NMHASH32(key,(size_t)i,(uint32_t)(256-i)); memcpy(hashes+4*i,&h,4); }
    uint32_t v=NMHASH32(hashes,256*4,0);
    printf("NMHASH32 verification LE = 0x%08X (expected 0x12A30553) %s\n",v,v==0x12A30553u?"PASS":"FAIL");
    uint32_t vx; for(int i=0;i<256;i++){ uint32_t h=NMHASH32X(key,(size_t)i,(uint32_t)(256-i)); memcpy(hashes+4*i,&h,4); }
    vx=NMHASH32X(hashes,256*4,0);
    printf("NMHASH32X verification LE = 0x%08X (expected 0xA8580227) %s\n",vx,vx==0xA8580227u?"PASS":"FAIL");
    printf("NMH_VECTOR = %d (0=scalar,1=sse2,2=avx2,3=avx512)\n", NMH_VECTOR);
    return 0;
}
