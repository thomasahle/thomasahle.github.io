/* Include the frozen reference unchanged; -Itests supplies c/highwayhash.h. */
#include "../../materials/highwayhash.c"
#include <stdio.h>
#include <inttypes.h>

static const uint64_t keys[][4] = {
  {0,0,0,0}, {1,2,3,4},
  {UINT64_C(0xdbe6d5d58afad71e), UINT64_C(0xa0142b42de197939),
   UINT64_C(0x5bd2b2861106bd66), UINT64_C(0xb6c304527caad524)},
  {UINT64_C(0xdbe6d5d58afad71f), UINT64_C(0xa0142b42de197939),
   UINT64_C(0x5bd2b2861106bd66), UINT64_C(0xb6c304527caad524)}
};
static const uint64_t packets[3][12] = {
  {UINT64_C(0x24192a2a01b331d1),0,0,0,UINT64_C(0x24192a2ab4b332d1),0,0,0,0,0,0,0},
  {UINT64_C(0x24192a2a01b332d1),0,0,0,UINT64_C(0x24192a2ab3b330d1),0,0,0,256,0,0,0},
  {UINT64_C(0x0706050403020100),UINT64_C(0x0f0e0d0c0b0a0908),
   UINT64_C(0x1716151413121110),UINT64_C(0x1f1e1d1c1b1a1918)}
};
int main(void) {
  for (unsigned k=0;k<4;k++) for (unsigned m=0;m<3;m++) {
    uint8_t bytes[96]={0};
    unsigned size=m==2?32:96;
    for(unsigned j=0;j<size/8;j++) for(unsigned b=0;b<8;b++)
      bytes[8*j+b]=(uint8_t)(packets[m][j]>>(8*b));
    HighwayHashState s;
    ProcessAll(bytes,size,keys[k],&s);
    uint64_t out[23];
    for(unsigned i=0;i<4;i++) {
      out[i]=s.v0[i];out[4+i]=s.v1[i];out[8+i]=s.mul0[i];out[12+i]=s.mul1[i];
    }
    out[16]=HighwayHash64(bytes,size,keys[k]);
    HighwayHash128(bytes,size,keys[k],out+17);
    HighwayHash256(bytes,size,keys[k],out+19);
    putchar('[');
    for(unsigned i=0;i<23;i++) printf("%s%"PRIu64,i?", ":"",out[i]);
    puts("]");
  }
}
