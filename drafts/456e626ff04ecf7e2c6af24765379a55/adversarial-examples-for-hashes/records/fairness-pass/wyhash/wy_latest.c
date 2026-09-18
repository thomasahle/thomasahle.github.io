/* Pair checks against the UPSTREAM wyhash.h (latest clone), not the transcription.
 * modes:  check                 -> vectors, recorded seeds, key-free pairs (2^30 random seeds each)
 *         sample <lg> <rngseed> -> pair A over 2^lg xoshiro256** seeds (splitmix init from rngseed)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>
#include "wyhash/wyhash.h"

static uint64_t H(const uint8_t *p, size_t n, uint64_t seed) { return wyhash(p, n, seed, _wyp); }

static uint64_t rng_state[4];
static uint64_t rot(uint64_t x, unsigned n) { return x<<n | x>>(64-n); }
static uint64_t splitmix(uint64_t *s) { uint64_t z=(*s+=UINT64_C(0x9e3779b97f4a7c15)); z=(z^(z>>30))*UINT64_C(0xbf58476d1ce4e5b9); z=(z^(z>>27))*UINT64_C(0x94d049bb133111eb); return z^(z>>31); }
static void rng_init(uint64_t s) { for(int i=0;i<4;i++) rng_state[i]=splitmix(&s); }
static uint64_t rng_next(void) { uint64_t *s=rng_state, r=rot(s[1]*5,7)*9, t=s[1]<<17; s[2]^=s[0]; s[3]^=s[1]; s[1]^=s[2]; s[0]^=s[3]; s[2]^=t; s[3]=rot(s[3],45); return r; }

static size_t decode(const char *s, uint8_t *out) { size_t n=strlen(s)/2; for(size_t i=0;i<n;i++){unsigned x; sscanf(s+2*i,"%2x",&x); out[i]=(uint8_t)x;} return n; }
static const char *A_HEX="9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c";
static const char *B_HEX="642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c";

static uint32_t verification(void) {
    uint8_t key[256]={0}, hashes[256*8], fin[8];
    for(int i=0;i<256;i++){ uint64_t h=H(key,(size_t)i,(uint64_t)(256-i)); for(int j=0;j<8;j++) hashes[i*8+j]=(uint8_t)(h>>(8*j)); key[i]=(uint8_t)i; }
    uint64_t h=H(hashes,256*8,0); for(int j=0;j<8;j++) fin[j]=(uint8_t)(h>>(8*j));
    return (uint32_t)fin[0]|(uint32_t)fin[1]<<8|(uint32_t)fin[2]<<16|(uint32_t)fin[3]<<24;
}
static void hex(const uint8_t *p,size_t n){ for(size_t i=0;i<n;i++) printf("%02x",p[i]); }
static void keyfree(const char *name,const uint8_t *a,size_t na,const uint8_t *b,size_t nb,unsigned lg){
    uint64_t n=UINT64_C(1)<<lg, c=0; rng_init(7);
    uint64_t fixed[3]={0,1,UINT64_MAX};
    printf("\n%s\nM  (%zu B) = ",name,na); hex(a,na); printf("\nM' (%zu B) = ",nb); hex(b,nb); puts("");
    for(int i=0;i<3;i++) printf("seed %016" PRIx64 ": H(M)=%016" PRIx64 " H(M')=%016" PRIx64 "\n",fixed[i],H(a,na,fixed[i]),H(b,nb,fixed[i]));
    for(uint64_t t=0;t<n;t++){ uint64_t s=rng_next(); if(H(a,na,s)==H(b,nb,s)) c++; }
    printf("collisions = %" PRIu64 " / 2^%u random seeds\n",c,lg);
}
int main(int argc,char**argv){
    uint8_t a[64],b[64]; size_t na=decode(A_HEX,a), nb=decode(B_HEX,b);
    if(argc>=2 && !strcmp(argv[1],"sample")){
        unsigned lg=(unsigned)atoi(argv[2]); uint64_t rs=strtoull(argv[3],0,0); uint64_t n=UINT64_C(1)<<lg, c=0; rng_init(rs);
        for(uint64_t t=0;t<n;t++){ uint64_t s=rng_next(); uint64_t ha=H(a,na,s); if(ha==H(b,nb,s)){ c++; printf("hit seed=%016" PRIx64 " out=%016" PRIx64 "\n",s,ha);} }
        printf("RESULT rngseed=%" PRIu64 " collisions=%" PRIu64 " trials=2^%u\n",rs,c,lg); return 0;
    }
    printf("WYHASH_CONDOM=%d\n",WYHASH_CONDOM);
    uint32_t v=verification(); printf("SMHasher3 verification: %08X expected 9DAE7DD3 %s\n",v,v==0x9DAE7DD3?"PASS":"FAIL");
    uint64_t md=H((const uint8_t*)"message digest",14,3); printf("message digest seed 3: %016" PRIx64 " expected 786d1f1df3801df4 %s\n",md,md==UINT64_C(0x786d1f1df3801df4)?"PASS":"FAIL");
    const uint64_t rec[9]={0x131b854bbd1f5b12ull,0x7df3a9721aedda79ull,0xc9ba3955003456f1ull,0xbffd9725feb83e1eull,0xa6f80edc9165ae6aull,0x167084425ff59a01ull,0x46762a8d365e542bull,0xb194e03d9c625ed2ull,0x4e6d29062896ae04ull};
    const uint64_t out[9]={0xa7dd61b404363777ull,0xb6df77843786b309ull,0x32aabeb277b5aa0full,0x622a2a5072b0ad1eull,0xbf48dcbb31c8bdfdull,0xc8c8068dcd21a3c8ull,0x2cd3259e4c5d72ccull,0xcc7f0266af931fd9ull,0x72455f0c25900727ull};
    int ok=1; for(int i=0;i<9;i++){ uint64_t x=H(a,na,rec[i]), y=H(b,nb,rec[i]); int g=(x==y&&x==out[i]); ok&=g; printf("recorded seed %016" PRIx64 ": H(M)=%016" PRIx64 " H(M')=%016" PRIx64 " expected %016" PRIx64 " %s\n",rec[i],x,y,out[i],g?"PASS":"FAIL"); }
    printf("pair A recorded seeds: %s\n",ok?"ALL PASS":"MISMATCH");
    /* key-free pairs from the public default secret _wyp[1] = 0x8bb84b93962eacc9 */
    uint64_t s1=_wyp[1];
    /* 16 B: (len>>3)<<2 = 8, so a = (rd32(p)<<32)|rd32(p+8): bytes 0..3 = LE(hi32 s1), bytes 8..11 = LE(lo32 s1); bytes 4..7 and 12..15 free */
    uint8_t k1[16],k2[16]; uint32_t hi=(uint32_t)(s1>>32), lo=(uint32_t)s1;
    for(int j=0;j<16;j++) k1[j]=k2[j]=(uint8_t)(0x10+j);
    for(int j=0;j<4;j++){ k1[j]=k2[j]=(uint8_t)(hi>>(8*j)); k1[8+j]=k2[8+j]=(uint8_t)(lo>>(8*j)); }
    for(int j=12;j<16;j++) k2[j]=(uint8_t)(0xa0+j);
    keyfree("key-free 16 B: bytes 0..3,8..11 read as secret[1] (annihilates the final product); bytes 12..15 differ",k1,16,k2,16,30);
    /* 12 B: (len>>3)<<2 = 4, a = (rd32(p)<<32)|rd32(p+4) = bytes 0..7; b = rd32(p+8)<<32|rd32(p+4); bytes 8..11 free */
    uint8_t j1[12],j2[12];
    for(int j=0;j<4;j++){ j1[j]=j2[j]=(uint8_t)(hi>>(8*j)); j1[4+j]=j2[4+j]=(uint8_t)(lo>>(8*j)); }
    for(int j=8;j<12;j++){ j1[j]=(uint8_t)(0x10+j); j2[j]=(uint8_t)(0xa0+j); }
    keyfree("key-free 12 B: bytes 0..7 read as secret[1]; bytes 8..11 differ",j1,12,j2,12,30);
    /* 32 B: w2 = s1 (bytes 16..23 LE); differ in w3 (and w0,w1 arbitrary) */
    uint8_t m1[32],m2[32]; memcpy(m1,a,32); memcpy(m2,a,32);
    for(int j=0;j<8;j++){ m1[16+j]=m2[16+j]=(uint8_t)(s1>>(8*j)); }
    for(int j=24;j<32;j++) m2[j]^=0xff;
    keyfree("key-free 32 B: w2 = secret[1]; w3 complemented",m1,32,m2,32,30);
    /* 32 B: w0 = s1 zeroes the block state; differ in w1 */
    uint8_t z1[32],z2[32]; memcpy(z1,a,32); memcpy(z2,a,32);
    for(int j=0;j<8;j++){ z1[j]=z2[j]=(uint8_t)(s1>>(8*j)); }
    for(int j=8;j<16;j++) z2[j]^=0xff;
    keyfree("key-free 32 B: w0 = secret[1] (block state zeroed); w1 complemented",z1,32,z2,32,30);
    return ok?0:1;
}
