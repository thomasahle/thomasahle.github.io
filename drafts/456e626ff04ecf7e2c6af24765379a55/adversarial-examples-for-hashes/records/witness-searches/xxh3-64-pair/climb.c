/* Hill-climb over 32-byte complement pairs (block (w0,w1) at secret offset 16*o versus its
 * complement) using the fold-only collision event, common random numbers (fixed salt).
 * Moves: flip one bit of w0, flip one bit of w1, or flip bit i of x:=w0 and set w1 := x - D
 * (D = K[2o]+K[2o+1]+1, the seed-shift family).  A move is accepted when its count beats the
 * current count by more than `z` standard deviations (sqrt of the current count); when no
 * move qualifies the sample size doubles, up to 2^lgmax.
 * usage: climb o w0 w1 [lg=24] [lgmax=28] [z=3] [salt=7]
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include <omp.h>
static const uint8_t kSecret[192] = {
    0xb8, 0xfe, 0x6c, 0x39, 0x23, 0xa4, 0x4b, 0xbe, 0x7c, 0x01, 0x81, 0x2c, 0xf7, 0x21, 0xad, 0x1c,
    0xde, 0xd4, 0x6d, 0xe9, 0x83, 0x90, 0x97, 0xdb, 0x72, 0x40, 0xa4, 0xa4, 0xb7, 0xb3, 0x67, 0x1f,
    0xcb, 0x79, 0xe6, 0x4e, 0xcc, 0xc0, 0xe5, 0x78, 0x82, 0x5a, 0xd0, 0x7d, 0xcc, 0xff, 0x72, 0x21,
    0xb8, 0x08, 0x46, 0x74, 0xf7, 0x43, 0x24, 0x8e, 0xe0, 0x35, 0x90, 0xe6, 0x81, 0x3a, 0x26, 0x4c,
    0x3c, 0x28, 0x52, 0xbb, 0x91, 0xc3, 0x00, 0xcb, 0x88, 0xd0, 0x65, 0x8b, 0x1b, 0x53, 0x2e, 0xa3,
    0x71, 0x64, 0x48, 0x97, 0xa2, 0x0d, 0xf9, 0x4e, 0x38, 0x19, 0xef, 0x46, 0xa9, 0xde, 0xac, 0xd8,
    0xa8, 0xfa, 0x76, 0x3f, 0xe3, 0x9c, 0x34, 0x3f, 0xf9, 0xdc, 0xbb, 0xc7, 0xc7, 0x0b, 0x4f, 0x1d,
    0x8a, 0x51, 0xe0, 0x4b, 0xcd, 0xb4, 0x59, 0x31, 0xc8, 0x9f, 0x7e, 0xc9, 0xd9, 0x78, 0x73, 0x64,
    0xea, 0xc5, 0xac, 0x83, 0x34, 0xd3, 0xeb, 0xc3, 0xc5, 0x81, 0xa0, 0xff, 0xfa, 0x13, 0x63, 0xeb,
    0x17, 0x0d, 0xdd, 0x51, 0xb7, 0xf0, 0xda, 0x49, 0xd3, 0x16, 0x55, 0x26, 0x29, 0xd4, 0x68, 0x9e,
    0x2b, 0x16, 0xbe, 0x58, 0x7d, 0x47, 0xa1, 0xfc, 0x8f, 0xf8, 0xb8, 0xd1, 0x7a, 0xd0, 0x31, 0xce,
    0x45, 0xcb, 0x3a, 0x8f, 0x95, 0x16, 0x04, 0x28, 0xaf, 0xd7, 0xfb, 0xca, 0xbb, 0x4b, 0x40, 0x7e,
};
static inline uint64_t fold(uint64_t a, uint64_t b){unsigned __int128 p=(unsigned __int128)a*b;return (uint64_t)p^(uint64_t)(p>>64);}
static inline uint64_t rotl(uint64_t x,unsigned n){return x<<n|x>>(64-n);}
static uint64_t splitmix(uint64_t *s){uint64_t z=(*s+=0x9e3779b97f4a7c15ULL);z=(z^(z>>30))*0xbf58476d1ce4e5b9ULL;z=(z^(z>>27))*0x94d049bb133111ebULL;return z^(z>>31);}
static uint64_t ka,kb,D;
static uint64_t eval(uint64_t w0,uint64_t w1,unsigned lg,uint64_t salt){
    uint64_t st[4]; uint64_t s0=salt; for(int i=0;i<4;i++)st[i]=splitmix(&s0);
    uint64_t N=1ULL<<lg,c=0;
    for(uint64_t i=0;i<N;i++){
        uint64_t r=rotl(st[1]*5,7)*9,t=st[1]<<17; st[2]^=st[0];st[3]^=st[1];st[1]^=st[2];st[0]^=st[3];st[2]^=t;st[3]=rotl(st[3],45);
        uint64_t A=w0^(ka+r),B=w1^(kb-r); c+=(fold(A,B)==fold(~A,~B));
    }
    return c;
}
int main(int argc,char**argv){
    if(argc<4){fprintf(stderr,"usage: climb o w0 w1 [lg] [lgmax] [z] [salt]\n");return 2;}
    unsigned o=atoi(argv[1]); uint64_t w0=strtoull(argv[2],0,16),w1=strtoull(argv[3],0,16);
    unsigned lg=argc>4?atoi(argv[4]):24, lgmax=argc>5?atoi(argv[5]):28; double z=argc>6?atof(argv[6]):3.0; uint64_t salt=argc>7?strtoull(argv[7],0,0):7;
    uint64_t K[24]; for(int i=0;i<24;i++){uint64_t v=0;for(int j=0;j<8;j++)v|=(uint64_t)kSecret[8*i+j]<<(8*j);K[i]=v;}
    ka=K[2*o];kb=K[2*o+1];D=ka+kb+1;
    uint64_t cur=eval(w0,w1,lg,salt);
    printf("start o=%u w0=%016" PRIx64 " w1=%016" PRIx64 " lg=%u count=%" PRIu64 " log2=%.3f\n",o,w0,w1,lg,cur,log2((double)cur/(1ULL<<lg)));fflush(stdout);
    for(int step=0;step<10000;step++){
        uint64_t nw0[192],nw1[192],nc[192];
        for(int i=0;i<64;i++){nw0[i]=w0^(1ULL<<i);nw1[i]=w1; nw0[64+i]=w0;nw1[64+i]=w1^(1ULL<<i); nw0[128+i]=w0^(1ULL<<i);nw1[128+i]=(w0^(1ULL<<i))-D;}
        #pragma omp parallel for schedule(dynamic)
        for(int i=0;i<192;i++) nc[i]=eval(nw0[i],nw1[i],lg,salt);
        int best=0; for(int i=1;i<192;i++) if(nc[i]>nc[best]) best=i;
        if((double)nc[best]>cur+z*sqrt((double)cur)){
            w0=nw0[best];w1=nw1[best];cur=nc[best];
            printf("step %d move=%s%d lg=%u count=%" PRIu64 " log2=%.3f w0=%016" PRIx64 " w1=%016" PRIx64 "\n",step,best<64?"w0^":best<128?"w1^":"x^",best%64,lg,cur,log2((double)cur/(1ULL<<lg)),w0,w1);fflush(stdout);
        } else if(lg<lgmax){
            lg+=2; cur=eval(w0,w1,lg,salt);
            printf("no move; lg=%u count=%" PRIu64 " log2=%.3f\n",lg,cur,log2((double)cur/(1ULL<<lg)));fflush(stdout);
        } else break;
    }
    printf("final o=%u w0=%016" PRIx64 " w1=%016" PRIx64 " lg=%u count=%" PRIu64 " log2=%.3f\n",o,w0,w1,lg,cur,log2((double)cur/(1ULL<<lg)));
    return 0;
}
