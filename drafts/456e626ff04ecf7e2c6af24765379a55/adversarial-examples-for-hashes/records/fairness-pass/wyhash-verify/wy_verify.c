/* Independent re-check of the wyhash blog row against the UPSTREAM header. */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include "wyhash/wyhash.h"   /* fresh clone of github.com/wangyi-fudan/wyhash */

static uint64_t H(const uint8_t*p,size_t n,uint64_t seed){ return wyhash(p,n,seed,_wyp); }

/* xoshiro256** seeded from splitmix64 */
static uint64_t s[4];
static uint64_t splitmix(uint64_t*x){ uint64_t z=(*x+=0x9e3779b97f4a7c15ull); z=(z^(z>>30))*0xbf58476d1ce4e5b9ull; z=(z^(z>>27))*0x94d049bb133111ebull; return z^(z>>31);}
static void rng_init(uint64_t seed){ for(int i=0;i<4;i++) s[i]=splitmix(&seed); }
static inline uint64_t rotl(uint64_t x,int k){ return (x<<k)|(x>>(64-k)); }
static inline uint64_t rng(void){ uint64_t r=rotl(s[1]*5,7)*9, t=s[1]<<17; s[2]^=s[0]; s[3]^=s[1]; s[1]^=s[2]; s[0]^=s[3]; s[2]^=t; s[3]=rotl(s[3],45); return r; }

static size_t unhex(const char*h,uint8_t*o){ size_t n=strlen(h)/2; for(size_t i=0;i<n;i++){ unsigned b; sscanf(h+2*i,"%2x",&b); o[i]=(uint8_t)b;} return n; }

static const char*A ="9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c";
static const char*A2="642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c";
static const struct{uint64_t seed,out;} rec[9]={
{UINT64_C(0x131b854bbd1f5b12),UINT64_C(0xa7dd61b404363777)},
{UINT64_C(0x7df3a9721aedda79),UINT64_C(0xb6df77843786b309)},
{UINT64_C(0xc9ba3955003456f1),UINT64_C(0x32aabeb277b5aa0f)},
{UINT64_C(0xbffd9725feb83e1e),UINT64_C(0x622a2a5072b0ad1e)},
{UINT64_C(0xa6f80edc9165ae6a),UINT64_C(0xbf48dcbb31c8bdfd)},
{UINT64_C(0x167084425ff59a01),UINT64_C(0xc8c8068dcd21a3c8)},
{UINT64_C(0x46762a8d365e542b),UINT64_C(0x2cd3259e4c5d72cc)},
{UINT64_C(0xb194e03d9c625ed2),UINT64_C(0xcc7f0266af931fd9)},
{UINT64_C(0x4e6d29062896ae04),UINT64_C(0x72455f0c25900727)}};
static const char*KF[3][2]={
 {"934bb88bc9ac2e9618191a1b","934bb88bc9ac2e96a8a9aaab"},
 {"934bb88b14151617c9ac2e961c1d1e1f","934bb88b14151617c9ac2e96acadaeaf"},
 {"9bd4604137366abec688a63706aa4a21c9ac2e96934bb88b33e0964e8c04600c","9bd4604137366abec688a63706aa4a21c9ac2e96934bb88bcc1f69b173fb9ff3"}};

int main(int argc,char**argv){
  if(argc<2){fputs("usage: check | sample <rngseed> <lg> | keyfree <lg>\n",stderr);return 2;}
  if(!strcmp(argv[1],"check")){
    printf("WYHASH_CONDOM=%d WYHASH_32BIT_MUM=%d\n",WYHASH_CONDOM,WYHASH_32BIT_MUM);
    printf("_wyp = %016" PRIx64 " %016" PRIx64 " %016" PRIx64 " %016" PRIx64 "\n",_wyp[0],_wyp[1],_wyp[2],_wyp[3]);
    /* SMHasher3 verification */
    uint8_t key[256], hashes[256*8], fin[8]; for(int i=0;i<256;i++){ key[i]=(uint8_t)i; uint64_t h=H(key,(size_t)i,(uint64_t)(256-i)); for(int k=0;k<8;k++) hashes[i*8+k]=(uint8_t)(h>>(8*k)); }
    uint64_t f=H(hashes,sizeof hashes,0); for(int k=0;k<8;k++) fin[k]=(uint8_t)(f>>(8*k));
    uint32_t v=fin[0]|(fin[1]<<8)|(fin[2]<<16)|((uint32_t)fin[3]<<24);
    printf("SMHasher3 verification: %08" PRIX32 " expected 9DAE7DD3 %s\n",v,v==0x9DAE7DD3?"PASS":"FAIL");
    uint64_t md=H((const uint8_t*)"message digest",14,3);
    printf("\"message digest\" seed 3: %016" PRIx64 " expected 786d1f1df3801df4 %s\n",md,md==UINT64_C(0x786d1f1df3801df4)?"PASS":"FAIL");
    uint8_t a[64],b[64]; size_t na=unhex(A,a), nb=unhex(A2,b); int ok=1;
    for(int i=0;i<9;i++){ uint64_t x=H(a,na,rec[i].seed), y=H(b,nb,rec[i].seed); int p=(x==y&&x==rec[i].out); ok&=p;
      printf("pair A seed %016" PRIx64 ": H(M)=%016" PRIx64 " H(M')=%016" PRIx64 " rec=%016" PRIx64 " %s\n",rec[i].seed,x,y,rec[i].out,p?"PASS":"FAIL"); }
    for(uint64_t sd=0;sd<2;sd++){ printf("pair A seed %" PRIu64 ": %016" PRIx64 " %016" PRIx64 "\n",sd,H(a,na,sd),H(b,nb,sd)); }
    uint64_t fixed[3]={0,1,UINT64_MAX};
    for(int i=0;i<3;i++){ na=unhex(KF[i][0],a); nb=unhex(KF[i][1],b);
      for(int j=0;j<3;j++){ uint64_t x=H(a,na,fixed[j]), y=H(b,nb,fixed[j]); ok&=(x==y);
        printf("keyfree pair %zuB seed %016" PRIx64 ": %016" PRIx64 " %016" PRIx64 " %s\n",na,fixed[j],x,y,x==y?"PASS":"FAIL"); } }
    puts(ok?"ALL PASS":"SOME FAIL"); return ok?0:1;
  }
  if(!strcmp(argv[1],"sample")&&argc==4){
    uint64_t rs=strtoull(argv[2],0,0); unsigned lg=(unsigned)atoi(argv[3]); uint64_t n=UINT64_C(1)<<lg;
    uint8_t a[64],b[64]; size_t na=unhex(A,a), nb=unhex(A2,b); rng_init(rs); uint64_t c=0;
    for(uint64_t t=0;t<n;t++){ uint64_t sd=rng(); if(H(a,na,sd)==H(b,nb,sd)){ c++; if(c<=3) printf("hit seed %016" PRIx64 "\n",sd);} }
    printf("RESULT rngseed=%" PRIu64 " collisions=%" PRIu64 " trials=2^%u\n",rs,c,lg); return 0;
  }
  if(!strcmp(argv[1],"keyfree")&&argc==3){
    unsigned lg=(unsigned)atoi(argv[2]); uint64_t n=UINT64_C(1)<<lg; int ok=1;
    for(int i=0;i<3;i++){ uint8_t a[64],b[64]; size_t na=unhex(KF[i][0],a), nb=unhex(KF[i][1],b); rng_init(777+i); uint64_t bad=0, first=H(a,na,0); int constant=1;
      for(uint64_t t=0;t<n;t++){ uint64_t sd=rng(); uint64_t x=H(a,na,sd), y=H(b,nb,sd); if(x!=y) bad++; if(x!=first) constant=0; }
      printf("KEYFREE %zuB: noncolliding=%" PRIu64 " / 2^%u random seeds; output constant across seeds: %s (H=%016" PRIx64 ")\n",na,bad,lg,constant?"yes":"no",first); ok&=(bad==0); }
    return ok?0:1;
  }
  return 2;
}
