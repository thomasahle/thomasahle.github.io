/* Independent harness: link the untouched upstream pengyhash.c (tinypeng master 9b70a18e)
 * and check (1) the SMHasher3 verification value, (2) the two recorded pairs collide
 * for fixed seeds and for 2^LG random seeds. Driver: MIT, T. D. Ahle 2026. */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "pengyhash.h"
static uint64_t sm(uint64_t *s){uint64_t z=(*s+=0x9e3779b97f4a7c15ull);z=(z^(z>>30))*0xbf58476d1ce4e5b9ull;z=(z^(z>>27))*0x94d049bb133111ebull;return z^(z>>31);}
static size_t dec(const char*s,uint8_t*o){size_t n=strlen(s)/2;for(size_t i=0;i<n;i++){unsigned x;sscanf(s+2*i,"%2x",&x);o[i]=x;}return n;}
static uint32_t verif(void){uint8_t key[256]={0},hs[2048],fin[8];for(int i=0;i<256;i++){uint64_t h=pengyhash(key,i,256-i);memcpy(hs+8*i,&h,8);key[i]=i;}uint64_t h=pengyhash(hs,2048,0);memcpy(fin,&h,8);return fin[0]|fin[1]<<8|fin[2]<<16|(uint32_t)fin[3]<<24;}
int main(int argc,char**argv){
  int lg=argc>1?atoi(argv[1]):24; uint64_t n=1ull<<lg;
  printf("verification %08X (SMHasher3 expects 861A1254) %s\n",verif(),verif()==0x861A1254?"PASS":"FAIL");
  const char*P[2][2]={{"7d664c02e4863788063367fb37f290ef00000000000000000000000000000000","2bbb99a5513852e1aa89ccb45c8f5b3d00000000000000000000000000000000"},
                      {"c5e359af9bb2c4c857384ca1c89a566e1eb7630b6d21d924c49138e925bd4db6","00"}};
  uint64_t fixed[4]={0x3a34ce6380fc0bc5ull,0,1,0xffffffffffffffffull};
  int bad=0;
  for(int k=0;k<2;k++){uint8_t a[64],b[64];size_t na=dec(P[k][0],a),nb=dec(P[k][1],b);
    for(int i=0;i<4;i++){uint64_t ha=pengyhash(a,na,fixed[i]),hb=pengyhash(b,nb,fixed[i]);
      printf("pair%d seed %016" PRIx64 ": %016" PRIx64 " %016" PRIx64 " %s\n",k,fixed[i],ha,hb,ha==hb?"COLLIDE":"DIFFER");bad+=ha!=hb;}
    uint64_t st=12345,c=0;for(uint64_t t=0;t<n;t++){uint64_t s=sm(&st);c+=pengyhash(a,na,s)==pengyhash(b,nb,s);}
    printf("pair%d random seeds: %" PRIu64 " / %" PRIu64 " collide\n",k,c,n);bad+=c!=n;}
  puts(bad?"RESULT: FAIL":"RESULT: ALL COLLIDE");return bad!=0;}
