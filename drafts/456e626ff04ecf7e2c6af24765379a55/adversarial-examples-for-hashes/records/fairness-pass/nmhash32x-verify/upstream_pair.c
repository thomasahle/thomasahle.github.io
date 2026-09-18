/* Independent re-check: includes UPSTREAM hash-garage nmhash.h verbatim, computes the
   SMHasher3 verification value for NMHASH32X, checks recorded example seeds, and
   enumerates all 2^32 seeds for the 28-, 32- and 33-byte pairs. */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "nmhash.h"

static int unhex(const char *h, uint8_t *out){ size_t n=strlen(h)/2; for(size_t i=0;i<n;i++){ unsigned v; sscanf(h+2*i,"%2x",&v); out[i]=(uint8_t)v;} return (int)n; }

static uint32_t smhasher3_verif(void){
    uint8_t key[256], hashes[256*4];
    for(int i=0;i<256;i++) key[i]=(uint8_t)i;
    for(int i=0;i<256;i++){ uint32_t h=NMHASH32X(key,(size_t)i,(uint32_t)(256-i)); hashes[4*i]=h&255; hashes[4*i+1]=(h>>8)&255; hashes[4*i+2]=(h>>16)&255; hashes[4*i+3]=h>>24; }
    uint32_t f=NMHASH32X(hashes,sizeof hashes,0);
    return f;
}

typedef struct { const uint8_t *a,*b; size_t n; uint64_t lo,hi; uint64_t count; uint32_t first_bad; int bad; } Job;
static void *worker(void *p){ Job *j=p; uint64_t c=0; for(uint64_t s=j->lo;s<j->hi;s++){ uint32_t ha=NMHASH32X(j->a,j->n,(uint32_t)s), hb=NMHASH32X(j->b,j->n,(uint32_t)s); if(ha==hb) c++; else if(!j->bad){ j->bad=1; j->first_bad=(uint32_t)s; } } j->count=c; return NULL; }

int main(void){
    printf("NMH_VERSION=%d\n", NMH_VERSION);
    uint32_t v=smhasher3_verif();
    printf("SMHasher3 verification NMHASH32X: %08X (expected A8580227) %s\n", v, v==0xA8580227u?"PASS":"FAIL");
    const char *pairs[][2]={
      {"00000000000000000000000000000000000000000000000000000000","08800008088000080000000000000000000000000000088080000880"},
      {"0000000000000000000000000000000000000000000000000000000000000000","0880000808800008000000000000000000000000000000000000088080000880"},
      {"000000000000000000000000000000000000000000000000000000000000000000","088000080880000800000000000000000000088080000880000000000000000000"}};
    uint32_t ex[]={0x0u,0x1u,0xdeadbeefu,0xffffffffu,0xc78b9a30u};
    for(int k=0;k<3;k++){
        uint8_t a[64],b[64]; int n=unhex(pairs[k][0],a); int n2=unhex(pairs[k][1],b); if(n!=n2){printf("len mismatch\n");return 2;}
        printf("pair %d len=%d\n",k,n);
        for(int e=0;e<5;e++) printf("  seed %08x: H(M)=%08x H(M')=%08x\n",ex[e],NMHASH32X(a,n,ex[e]),NMHASH32X(b,n,ex[e]));
        enum {T=8}; pthread_t th[T]; Job jobs[T];
        for(int t=0;t<T;t++){ jobs[t]=(Job){a,b,(size_t)n,(uint64_t)t<<29,(uint64_t)(t+1)<<29,0,0,0}; pthread_create(&th[t],NULL,worker,&jobs[t]); }
        uint64_t tot=0; int bad=0; uint32_t fb=0;
        for(int t=0;t<T;t++){ pthread_join(th[t],NULL); tot+=jobs[t].count; if(jobs[t].bad&&!bad){bad=1;fb=jobs[t].first_bad;} }
        printf("  len=%d collisions=%llu/4294967296%s\n",n,(unsigned long long)tot, bad?" (NON-COLLIDING SEED FOUND)":"");
        if(bad) printf("  first non-colliding seed %08x\n",fb);
    }
    return 0;
}
