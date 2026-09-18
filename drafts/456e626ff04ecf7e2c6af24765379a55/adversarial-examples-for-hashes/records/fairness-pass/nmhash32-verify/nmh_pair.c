/* Independent re-check: enumerate all 2^32 seeds for the recorded nmhash32 pair,
   calling the upstream hash-garage nmhash.h at HEAD directly. */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include "hash-garage/nmhash.h"

static uint8_t M[64], Mp[64];
static const char *MP_HEX = "00004080000000000000000000000000000040800000000000000000000000000828208000000000000000000000000000004080000000000000000000000000";
#define NT 8
typedef struct { uint64_t lo, hi, count; uint64_t first; int have_first; } job;
static void *work(void *a){
    job *j=a; j->count=0; j->have_first=0;
    for(uint64_t s=j->lo;s<j->hi;s++){
        uint32_t h1=NMHASH32(M,64,(uint32_t)s), h2=NMHASH32(Mp,64,(uint32_t)s);
        if(h1==h2){ if(!j->have_first){j->first=s;j->have_first=1;} j->count++; }
    }
    return NULL;
}
int main(int argc,char**argv){
    memset(M,0,64);
    for(int i=0;i<64;i++){ unsigned b; sscanf(MP_HEX+2*i,"%2x",&b); Mp[i]=(uint8_t)b; }
    uint32_t checks[3]={0x00000000u,0xb54cda26u,0xe142fddfu};
    for(int i=0;i<3;i++) printf("seed %08x: H(M)=%08x H(M')=%08x\n",checks[i],NMHASH32(M,64,checks[i]),NMHASH32(Mp,64,checks[i]));
    if(argc>1 && strcmp(argv[1],"exhaustive")==0){
        pthread_t th[NT]; job jb[NT]; uint64_t step=(1ULL<<32)/NT;
        for(int t=0;t<NT;t++){ jb[t].lo=t*step; jb[t].hi=(t+1)*step; pthread_create(&th[t],NULL,work,&jb[t]); }
        uint64_t total=0, first=~0ULL;
        for(int t=0;t<NT;t++){ pthread_join(th[t],NULL); total+=jb[t].count; if(jb[t].have_first && jb[t].first<first) first=jb[t].first; }
        double eps=(double)total/4294967296.0;
        printf("collisions = %llu / 4294967296 = %.9f, log2 = %.6f, bits(L=8) = %.6f\n",(unsigned long long)total,eps,__builtin_log2(eps),3.0-__builtin_log2(eps));
        printf("first colliding seed 0x%08llx -> %08x\n",(unsigned long long)first,NMHASH32(M,64,(uint32_t)first));
    }
    return 0;
}
