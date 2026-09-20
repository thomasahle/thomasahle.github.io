/* Selected MuseAir algorithm-v2 pair; implementation and original sampler in adjacent files. */
#define main measure_main
#include "measure.c"
#undef main
int main(int argc, char **argv) {
    char *args[] = {argv[0], "0000000000000000000000000000000000000000000000000000000000000000", "00000000000000404a048402a910048a00000000000000a80000000000000000", argc>1?argv[1]:"24", argc>2?argv[2]:"1", "1", NULL};
    uint8_t a[32]={0}, b[32]={0};
    for(int i=0;i<32;i++) b[i]=hexv(args[2][2*i])*16+hexv(args[2][2*i+1]);
    uint64_t s=UINT64_C(0x040963434fe5e368), h=UINT64_C(0xef46cda19c0bbc67);
    if(museair2_hash(a,32,s,0)!=h || museair2_hash(b,32,s,0)!=h || museair2_hash(a,32,0,0)==museair2_hash(b,32,0,0)) return 1;
    puts("explicit seed 040963434fe5e368 -> ef46cda19c0bbc67 PASS; seed 0 differs");
    return measure_main(6,args);
}
