/* Checks the post's nmhash32x pairs against the UPSTREAM hash-garage nmhash.h (HEAD), all 2^32 seeds. */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <omp.h>
#define NMH_VECTOR NMH_SCALAR
#include "hash-garage/nmhash.h"
static size_t dec(const char*s,uint8_t*o){size_t n=strlen(s)/2;for(size_t i=0;i<n;i++){unsigned x;sscanf(s+2*i,"%2x",&x);o[i]=x;}return n;}
static uint32_t verif(void){uint8_t key[256]={0},hs[1024];for(int i=0;i<256;i++){uint32_t h=NMHASH32X(key,i,256-i);memcpy(hs+4*i,&h,4);key[i]=i;}uint32_t f=NMHASH32X(hs,1024,0);return f;}
int main(int argc,char**argv){
  const char*P[][2]={
   {"00000000000000000000000000000000000000000000000000000000","08800008088000080000000000000000000000000000088080000880"},
   {"0000000000000000000000000000000000000000000000000000000000000000","0880000808800008000000000000000000000000000000000000088080000880"},
   {"000000000000000000000000000000000000000000000000000000000000000000","088000080880000800000000000000000000088080000880000000000000000000"}};
  printf("NMH_VERSION=%d verification(SMHasher3 procedure)=%08X expected A8580227\n",NMH_VERSION,verif());
  for(int p=0;p<3;p++){
    uint8_t a[64],b[64];size_t na=dec(P[p][0],a),nb=dec(P[p][1],b);
    uint32_t sd[]={0,1,0xdeadbeef,0xffffffff,0xc78b9a30};
    printf("pair len=%zu:",na);for(int i=0;i<5;i++)printf(" %08x->%08x/%08x",sd[i],NMHASH32X(a,na,sd[i]),NMHASH32X(b,nb,sd[i]));puts("");
    uint64_t coll=0;uint32_t bad=0;int anybad=0;
    #pragma omp parallel for reduction(+:coll) schedule(static)
    for(int64_t s=0;s<(int64_t)1<<32;s++){uint32_t ha=NMHASH32X(a,na,(uint32_t)s),hb=NMHASH32X(b,nb,(uint32_t)s);if(ha==hb)coll++;else{
      #pragma omp critical
      {if(!anybad){anybad=1;bad=(uint32_t)s;}}}}
    printf("len=%zu: collisions=%llu / 4294967296%s",na,(unsigned long long)coll,anybad?" FIRST NON-COLLIDING SEED ":"\n");if(anybad)printf("%08x\n",bad);
  }
  return 0;}
