/* Pair check for the nmhash32 row against the upstream hash-garage nmhash.h (HEAD).
   Modes: exhaustive [w0 w4]        all 2^32 seeds, 8 threads
          scan K log2N              K random lane-0 word pairs (w0,w4), 2^log2N splitmix-sampled seeds each
          scan0 log2N               w0 = 0, w4 = one or two bits (528 cells), sampled seeds */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include "hash-garage/nmhash.h"

static void put32(uint8_t *p, uint32_t v){ p[0]=v; p[1]=v>>8; p[2]=v>>16; p[3]=v>>24; }
static void build(uint8_t m[64], uint8_t m2[64], uint32_t w0, uint32_t w4){
    memset(m,0,64); memset(m2,0,64);
    put32(m,w0); put32(m+16,w4);
    put32(m2,w0^0x80400000u); put32(m2+16,w4^0x80400000u); put32(m2+32,0x80202808u); put32(m2+48,0x80400000u);
}
static uint64_t sm(uint64_t *x){ uint64_t z=(*x+=0x9e3779b97f4a7c15ull); z=(z^(z>>30))*0xbf58476d1ce4e5b9ull; z=(z^(z>>27))*0x94d049bb133111ebull; return z^(z>>31); }
typedef struct { const uint8_t *m,*m2; uint64_t lo,hi; int sampled; uint64_t cnt; uint32_t first; int hasfirst; uint32_t out; } job;
static void *work(void *a){ job *j=a; j->cnt=0; j->hasfirst=0;
    for(uint64_t s=j->lo;s<j->hi;s++){ uint32_t seed; if(j->sampled){ uint64_t x=s; seed=(uint32_t)sm(&x); } else seed=(uint32_t)s;
        uint32_t h=NMHASH32(j->m,64,seed), h2=NMHASH32(j->m2,64,seed);
        if(h==h2){ if(!j->hasfirst){j->first=seed;j->out=h;j->hasfirst=1;} j->cnt++; } }
    return 0; }
static uint64_t count_par(const uint8_t*m,const uint8_t*m2,uint64_t n,int sampled,int T,uint32_t*first,uint32_t*out){
    pthread_t th[64]; job J[64]; for(int t=0;t<T;t++){J[t].m=m;J[t].m2=m2;J[t].lo=n*t/T;J[t].hi=n*(t+1)/T;J[t].sampled=sampled;pthread_create(&th[t],0,work,&J[t]);}
    uint64_t c=0; int f=0; for(int t=0;t<T;t++){pthread_join(th[t],0); c+=J[t].cnt; if(!f&&J[t].hasfirst){*first=J[t].first;*out=J[t].out;f=1;}} return c; }
static void show(const uint8_t*m,const uint8_t*m2){ printf("M  hex="); for(int i=0;i<64;i++)printf("%02x",m[i]); printf("\nM' hex="); for(int i=0;i<64;i++)printf("%02x",m2[i]); printf("\n"); }
int main(int argc,char**argv){
    uint8_t m[64],m2[64]; build(m,m2,0,0); int T=8;
    const char *mode=argc>1?argv[1]:"exhaustive";
    if(!strcmp(mode,"exhaustive")){
        uint32_t w0=0,w4=0; if(argc>3){ w0=strtoul(argv[2],0,16); w4=strtoul(argv[3],0,16); }
        build(m,m2,w0,w4); printf("lane-0 words w0=%08x w4=%08x; M' = M with word0^80400000 word4^80400000 word8^80202808 word12^80400000\n",w0,w4); show(m,m2);
        uint32_t seeds[3]={0xb54cda26u,0xe142fddfu,0};
        for(int i=0;i<3;i++) printf("seed 0x%08x: H(M)=%08x H(M')=%08x %s\n",seeds[i],NMHASH32(m,64,seeds[i]),NMHASH32(m2,64,seeds[i]),NMHASH32(m,64,seeds[i])==NMHASH32(m2,64,seeds[i])?"COLLIDE":"differ");
        uint32_t f=0,o=0; uint64_t c=count_par(m,m2,1ull<<32,0,T,&f,&o);
        printf("exhaustive: collisions=%llu / 4294967296 rate=%.9f log2=%.6f bits(L=8)=%.6f first_seed=0x%08x out=%08x\n",(unsigned long long)c,c/4294967296.0,log2(c/4294967296.0),log2(8.0)-log2(c/4294967296.0),f,o);
    } else if(!strcmp(mode,"scan")){
        int K=atoi(argv[2]); int lg=atoi(argv[3]); uint64_t n=1ull<<lg; uint64_t x=12345;
        double best=-1,worst=2,sum=0,sq=0; uint32_t bw0=0,bw4=0;
        for(int k=0;k<K;k++){ uint32_t w0=(uint32_t)sm(&x), w4=(uint32_t)sm(&x); if(k==0){w0=0;w4=0;}
            build(m,m2,w0,w4); uint32_t f,o; uint64_t c=count_par(m,m2,n,1,T,&f,&o); double r=(double)c/n; sum+=r; sq+=r*r;
            if(r>best){best=r;bw0=w0;bw4=w4;} if(r<worst)worst=r;
            if(k<3||r>0.2565||r<0.2455) printf("w0=%08x w4=%08x rate=%.6f\n",w0,w4,r); }
        double mean=sum/K; printf("scan K=%d seeds/cell=2^%d (splitmix-sampled): mean=%.6f sd=%.6f min=%.6f max=%.6f at w0=%08x w4=%08x (bits at max=%.4f; binomial sd per cell=%.6f)\n",K,lg,mean,sqrt(sq/K-mean*mean),worst,best,bw0,bw4,log2(8.0/best),sqrt(0.25*0.75/n));
    } else if(!strcmp(mode,"scan0")){ int lg=atoi(argv[2]); uint64_t n=1ull<<lg; int hits=0; double best=-1; uint32_t bw4=0;
        for(int a=0;a<32;a++) for(int b=a;b<32;b++){ uint32_t w4=(1u<<a)|(1u<<b); build(m,m2,0,w4); uint32_t f,o; uint64_t c=count_par(m,m2,n,1,T,&f,&o); double r=(double)c/n;
            if(r>best){best=r;bw4=w4;} if(r>0.2565||r<0.2455){ if(hits<12) printf("w0=00000000 w4=%08x rate=%.6f\n",w4,r); hits++; } }
        printf("scan0 (w0=0, w4 = one or two bits, 528 cells, 2^%d sampled seeds each): %d cells outside [0.2455,0.2565]; max=%.6f at w4=%08x\n",lg,hits,best,bw4); }
    return 0; }
