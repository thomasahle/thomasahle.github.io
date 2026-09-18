#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "pengyhash/pengyhash.h"
static uint64_t sm(uint64_t *s){uint64_t z=(*s+=0x9e3779b97f4a7c15ULL);z=(z^(z>>30))*0xbf58476d1ce4e5b9ULL;z=(z^(z>>27))*0x94d049bb133111ebULL;return z^(z>>31);}
static void hex(const char*h,uint8_t*o,size_t n){for(size_t i=0;i<n;i++){unsigned x;sscanf(h+2*i,"%2x",&x);o[i]=x;}}
int main(){
  uint8_t key[256]={0},hashes[2048],fin[8];
  for(int i=0;i<256;i++){uint64_t h=pengyhash(key,i,256-i);memcpy(hashes+8*i,&h,8);key[i]=i;}
  uint64_t h=pengyhash(hashes,2048,0);memcpy(fin,&h,8);
  printf("upstream verification = %08X (SMHasher3 sequenced expects 861A1254)\n",fin[0]|fin[1]<<8|fin[2]<<16|fin[3]<<24);
  uint8_t a[32],b[32],c[32],d[1]={0};
  hex("7d664c02e4863788063367fb37f290ef00000000000000000000000000000000",a,32);
  hex("2bbb99a5513852e1aa89ccb45c8f5b3d00000000000000000000000000000000",b,32);
  hex("c5e359af9bb2c4c857384ca1c89a566e1eb7630b6d21d924c49138e925bd4db6",c,32);
  uint64_t fixed[]={0x3a34ce6380fc0bc5ULL,0,1,~0ULL};
  for(int i=0;i<4;i++) printf("seed %016llx: pair1 %016llx %016llx  pair2 %016llx %016llx\n",(unsigned long long)fixed[i],(unsigned long long)pengyhash(a,32,fixed[i]),(unsigned long long)pengyhash(b,32,fixed[i]),(unsigned long long)pengyhash(c,32,fixed[i]),(unsigned long long)pengyhash(d,1,fixed[i]));
  uint64_t s=7,n=1ULL<<24,c1=0,c2=0;
  for(uint64_t t=0;t<n;t++){uint64_t k=sm(&s);c1+=pengyhash(a,32,k)==pengyhash(b,32,k);c2+=pengyhash(c,32,k)==pengyhash(d,1,k);}
  printf("random seeds: pair1 %llu/%llu  pair2 %llu/%llu\n",(unsigned long long)c1,(unsigned long long)n,(unsigned long long)c2,(unsigned long long)n);
  return 0;}
