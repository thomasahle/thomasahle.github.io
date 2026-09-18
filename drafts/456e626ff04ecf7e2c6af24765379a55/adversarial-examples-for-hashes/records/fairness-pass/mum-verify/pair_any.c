#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include MUM_H
static uint64_t s[4];
static inline uint64_t rotl(uint64_t x,int k){return (x<<k)|(x>>(64-k));}
static uint64_t xo(void){uint64_t r=rotl(s[1]*5,7)*9,t=s[1]<<17;s[2]^=s[0];s[3]^=s[1];s[1]^=s[2];s[0]^=s[3];s[2]^=t;s[3]=rotl(s[3],45);return r;}
static uint64_t sm(uint64_t*x){uint64_t z=(*x+=0x9e3779b97f4a7c15ULL);z=(z^(z>>30))*0xbf58476d1ce4e5b9ULL;z=(z^(z>>27))*0x94d049bb133111ebULL;return z^(z>>31);}
static int unhex(const char*h,uint8_t*o){int n=strlen(h)/2;for(int i=0;i<n;i++){unsigned v;sscanf(h+2*i,"%2x",&v);o[i]=v;}return n;}
int main(int argc,char**argv){
  uint8_t A[64],B[64]; int la=unhex(argv[1],A), lb=unhex(argv[2],B); int lg=argc>3?atoi(argv[3]):20;
  uint64_t x=1; for(int i=0;i<4;i++) s[i]=sm(&x);
  uint64_t h0=mum_hash(A,la,0), h1=mum_hash(B,lb,0);
  printf("seed0: %016llx %016llx %s\n",(unsigned long long)h0,(unsigned long long)h1,h0==h1?"EQ":"NE");
  uint64_t n=1ULL<<lg,c=0; for(uint64_t i=0;i<n;i++){uint64_t k=xo(); if(mum_hash(A,la,k)==mum_hash(B,lb,k)) c++;}
  printf("CONFIG=%s collisions %llu/%llu\n",CFG,(unsigned long long)c,(unsigned long long)n); return 0;}
