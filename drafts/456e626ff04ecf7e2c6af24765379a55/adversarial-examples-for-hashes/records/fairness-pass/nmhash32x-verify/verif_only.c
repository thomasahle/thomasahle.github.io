#include <stdio.h>
#include <stdint.h>
#include "nmhash.h"
int main(void){
    uint8_t key[256], hashes[256*4];
    for(int i=0;i<256;i++) key[i]=(uint8_t)i;
    for(int i=0;i<256;i++){ uint32_t h=NMHASH32X(key,(size_t)i,(uint32_t)(256-i)); hashes[4*i]=h&255; hashes[4*i+1]=(h>>8)&255; hashes[4*i+2]=(h>>16)&255; hashes[4*i+3]=h>>24; }
    uint32_t f=NMHASH32X(hashes,sizeof hashes,0);
    printf("NMH_VECTOR=%d verification=%08X %s\n", (int)NMH_VECTOR, f, f==0xA8580227u?"PASS":"FAIL");
    return 0;
}
