/* Independent check of the rapid1 row against the UPSTREAM v1.0 header (tag rapidhash_v1.0).
 * usage: upstream_check <log2 N> <rng seed> [mode]
 * mode 0 (default): reference vector, recorded seed, then sample N random seeds on pair A.
 * mode 1: key-free pair (w[len-16] = secret[1]) for N seeds at 24 bytes, plus random (seed,M,M') at 17/24/32 B.
 * mode 2: differential test upstream vs transcription (rapidhash_v1_verify.c's rapidhash_ref) on N random inputs. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include "rapidhash.h"          /* upstream, from -I */

typedef unsigned __int128 u128;
static inline uint64_t rd32(const uint8_t *p){return (uint64_t)p[0]|(uint64_t)p[1]<<8|(uint64_t)p[2]<<16|(uint64_t)p[3]<<24;}
static inline uint64_t rd64(const uint8_t *p){return rd32(p)|rd32(p+4)<<32;}
static const uint64_t SEC[3]={0x2d358dccaa6c78a5ull,0x8bb84b93962eacc9ull,0x4b33a62ed433d4a3ull};
static inline void t_mum(uint64_t*A,uint64_t*B){u128 r=(u128)*A**B;*A=(uint64_t)r;*B=(uint64_t)(r>>64);}
static inline uint64_t t_mix(uint64_t A,uint64_t B){t_mum(&A,&B);return A^B;}
static inline uint64_t t_small(const uint8_t*p,size_t k){return (((uint64_t)p[0])<<56)|(((uint64_t)p[k>>1])<<32)|p[k-1];}
/* transcription copied verbatim from verify/rapidhash-v1/rapidhash_v1_verify.c */
static uint64_t transcription(const void*key,size_t len,uint64_t seed,const uint64_t*secret){
    const uint8_t*p=(const uint8_t*)key; seed^=t_mix(seed^secret[0],secret[1])^len; uint64_t a,b;
    if(len<=16){ if(len>=4){const uint8_t*plast=p+len-4; a=(rd32(p)<<32)|rd32(plast); const uint64_t delta=((len&24)>>(len>>3)); b=((rd32(p+delta)<<32)|rd32(plast-delta));}
        else if(len>0){a=t_small(p,len);b=0;} else a=b=0; }
    else { size_t i=len;
        if(i>48){ uint64_t see1=seed,see2=seed;
            while(i>=96){ seed=t_mix(rd64(p)^secret[0],rd64(p+8)^seed); see1=t_mix(rd64(p+16)^secret[1],rd64(p+24)^see1); see2=t_mix(rd64(p+32)^secret[2],rd64(p+40)^see2);
                seed=t_mix(rd64(p+48)^secret[0],rd64(p+56)^seed); see1=t_mix(rd64(p+64)^secret[1],rd64(p+72)^see1); see2=t_mix(rd64(p+80)^secret[2],rd64(p+88)^see2); p+=96;i-=96;}
            if(i>=48){ seed=t_mix(rd64(p)^secret[0],rd64(p+8)^seed); see1=t_mix(rd64(p+16)^secret[1],rd64(p+24)^see1); see2=t_mix(rd64(p+32)^secret[2],rd64(p+40)^see2); p+=48;i-=48;}
            seed^=see1^see2; }
        if(i>16){ seed=t_mix(rd64(p)^secret[2],rd64(p+8)^seed^secret[1]); if(i>32) seed=t_mix(rd64(p+16)^secret[2],rd64(p+24)^seed);}
        a=rd64(p+i-16); b=rd64(p+i-8); }
    a^=secret[1]; b^=seed; t_mum(&a,&b); return t_mix(a^secret[0]^len,b^secret[1]);
}

static uint64_t s[4];
static uint64_t rot(uint64_t x,unsigned n){return x<<n|x>>(64-n);}
static uint64_t splitmix(uint64_t*z){uint64_t r=(*z+=0x9e3779b97f4a7c15ull);r=(r^(r>>30))*0xbf58476d1ce4e5b9ull;r=(r^(r>>27))*0x94d049bb133111ebull;return r^(r>>31);}
static void rng_init(uint64_t z){for(int i=0;i<4;i++)s[i]=splitmix(&z);}
static uint64_t rng(void){uint64_t r=rot(s[1]*5,7)*9,t=s[1]<<17;s[2]^=s[0];s[3]^=s[1];s[1]^=s[2];s[0]^=s[3];s[2]^=t;s[3]=rot(s[3],45);return r;}
static size_t dec(const char*h,uint8_t*o){size_t n=strlen(h)/2;for(size_t i=0;i<n;i++){unsigned x;sscanf(h+2*i,"%2x",&x);o[i]=x;}return n;}

int main(int argc,char**argv){
    unsigned lg=argc>1?atoi(argv[1]):20; uint64_t rs=argc>2?strtoull(argv[2],0,0):1; int mode=argc>3?atoi(argv[3]):0; uint64_t N=1ull<<lg;
    uint64_t v=rapidhash_withSeed("message digest",14,3);
    printf("upstream v1.0 'message digest' seed 3: %016" PRIx64 " expected 0031cdc21324150f %s\n",v,v==0x0031cdc21324150full?"PASS":"FAIL");
    uint8_t A[64],B[64]; size_t na=dec("9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c",A), nb=dec("642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c",B);
    rng_init(rs);
    if(mode==0){
        uint64_t ha=rapidhash_withSeed(A,na,0x3788f2419a81e2d6ull), hb=rapidhash_withSeed(B,nb,0x3788f2419a81e2d6ull);
        printf("recorded seed 3788f2419a81e2d6: H(A)=%016" PRIx64 " H(B)=%016" PRIx64 " expected 8f7a71ffebd4a14b %s\n",ha,hb,(ha==hb&&ha==0x8f7a71ffebd4a14bull)?"PASS":"FAIL");
        uint64_t c=0; for(uint64_t t=0;t<N;t++){uint64_t sd=rng(); if(rapidhash_withSeed(A,na,sd)==rapidhash_withSeed(B,nb,sd)) c++;}
        printf("pair A upstream: %" PRIu64 " / 2^%u collisions (rng seed %" PRIu64 ")\n",c,lg,rs);
    } else if(mode==1){
        /* key-free: 24-byte messages, w[1] = secret[1] (offset len-16 = 8), w[0] differs */
        uint8_t M[24]={0},M2[24]={0}; for(int i=0;i<8;i++){M[8+i]=M2[8+i]=(uint8_t)(SEC[1]>>(8*i));} M2[0]=1;
        uint64_t c=0,out=rapidhash_withSeed(M,24,0); int constant=1;
        for(uint64_t t=0;t<N;t++){uint64_t sd=rng(); uint64_t x=rapidhash_withSeed(M,24,sd),y=rapidhash_withSeed(M2,24,sd); if(x==y)c++; if(x!=out)constant=0;}
        printf("key-free literal pair (24 B, w[1]=secret[1]): %" PRIu64 " / 2^%u collide; output %016" PRIx64 " constant over all seeds: %s\n",c,lg,out,constant?"yes":"no");
        printf("expected constant rapid_mix(secret[0]^24, secret[1]) = %016" PRIx64 "\n",t_mix(SEC[0]^24,SEC[1]));
        size_t lens[3]={17,24,32};
        for(int k=0;k<3;k++){ size_t L=lens[k]; uint64_t cc=0;
            for(uint64_t t=0;t<65536;t++){ uint8_t X[64],Y[64]; for(size_t i=0;i<L;i+=8){uint64_t r=rng();memcpy(X+i,&r,L-i<8?L-i:8);} for(size_t i=0;i<L;i+=8){uint64_t r=rng();memcpy(Y+i,&r,L-i<8?L-i:8);}
                memcpy(X+L-16,&SEC[1],8); memcpy(Y+L-16,&SEC[1],8); uint64_t sd=rng(); if(rapidhash_withSeed(X,L,sd)==rapidhash_withSeed(Y,L,sd)) cc++; }
            printf("random (seed,M,M') with w[len-16]=secret[1], len %zu: %" PRIu64 " / 65536 collide\n",L,cc); }
    } else {
        uint64_t bad=0; for(uint64_t t=0;t<N;t++){ size_t L=rng()%200; uint8_t X[256]; for(size_t i=0;i<L;i+=8){uint64_t r=rng();memcpy(X+i,&r,L-i<8?L-i:8);} uint64_t sd=rng();
            if(rapidhash_withSeed(X,L,sd)!=transcription(X,L,sd,SEC)) { if(bad<3) printf("MISMATCH len %zu\n",L); bad++; } }
        printf("differential upstream vs transcription, lengths 0..199, %" PRIu64 " inputs: %" PRIu64 " mismatches\n",N,bad);
    }
    return 0;
}
