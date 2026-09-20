/* nmhash32: adapted for this package from experiment/verify-nmhash32/own_x28/own_nmhash.h; NMHASH32 trail also independently checked in verify-nmhash32/nm.h.
 * The supplied independent verifier/driver is credited here;
 * adaptations: standalone C11 driver, explicit LE loads, asserted
 * SMHasher3 values and witnesses, single-thread deterministic sampling. */
/*
 * nmhash
 * Copyright (C) 2021-2023  Frank J. T. Wojcik
 * Copyright (C) 2023       jason
 * Copyright (c) 2021, James Z.M. Gao
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided
 * with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdint.h>
#include <stddef.h>
#include <string.h>

static uint64_t load_le(const uint8_t *p, unsigned n) {
    uint64_t x=0; for(unsigned i=0;i<n;i++) x|=(uint64_t)p[i]<<(8*i); return x;
}
/* Independent scalar transcription of nmhash32 / nmhash32x v2 (James Z.M. Gao),
   written from smhasher3 hashes/nmhash.cpp for the verify-nmhash32 reproduction. */
#include <stdint.h>
#include <stddef.h>
#include <string.h>

static inline uint32_t rd32(const uint8_t *p, size_t o){ return (uint32_t)load_le(p+o,4); } /* LE host */
static inline uint32_t rd16(const uint8_t *p, size_t o){ return (uint32_t)load_le(p+o,2); }
static inline uint32_t rotl(uint32_t x,int r){ return (x<<r)|(x>>(32-r)); }
static inline uint32_t mul16(uint32_t a,uint32_t b){
    uint32_t lo=(uint16_t)((uint32_t)(uint16_t)a*(uint16_t)b), hi=(uint16_t)((uint32_t)(uint16_t)(a>>16)*(uint16_t)(b>>16));
    return (hi<<16)+lo;
}
#define P1 0x9E3779B1u
#define P2 0x85EBCA77u
#define P3 0xC2B2AE3Du
#define P4 0x27D4EB2Fu
#define M1 0xF0D9649Bu
#define M2 0x29A7935Du
#define M3 0x55D35831u
static const uint32_t ACC_INIT[32]={
0xB8FE6C39,0x23A44BBE,0x7C01812C,0xF721AD1C,0xDED46DE9,0x839097DB,0x7240A4A4,0xB7B3671F,
0xCB79E64E,0xCCC0E578,0x825AD07D,0xCCFF7221,0xB8084674,0xF743248E,0xE03590E6,0x813A264C,
0x3C2852BB,0x91C300CB,0x88D0658B,0x1B532EA3,0x71644897,0xA20DF94E,0x3819EF46,0xA9DEACD8,
0xA8FA763F,0xE39C343F,0xF9DCBBC7,0xC70B4F1D,0x8A51E04B,0xCDB45931,0xC89F7EC9,0xD9787364};

/* ---- shared long path (>=256 bytes) ---- */
static void long_round(uint32_t *X,uint32_t *Y,const uint8_t *p){
    int i;
    for(i=0;i<32;i++) X[i]^=rd32(p,i*4);
    for(i=0;i<32;i++) Y[i]^=rd32(p,i*4+128);
    for(i=0;i<32;i++) X[i]+=Y[i];
    for(i=0;i<32;i++) Y[i]^=X[i]>>1;
    for(i=0;i<32;i++) X[i]=mul16(X[i],M1);
    for(i=0;i<32;i++) X[i]^=(X[i]<<5)^(X[i]>>13);
    for(i=0;i<32;i++) X[i]=mul16(X[i],M2);
    for(i=0;i<32;i++) X[i]^=Y[i];
    for(i=0;i<32;i++) X[i]^=(X[i]<<11)^(X[i]>>9);
    for(i=0;i<32;i++) X[i]=mul16(X[i],M3);
    for(i=0;i<32;i++) X[i]^=(X[i]>>10)^(X[i]>>20);
}
static uint32_t nmh_long(const uint8_t *p,size_t len,uint32_t seed){
    uint32_t X[32],Y[32],sum=0; size_t i,nb=(len-1)/256;
    for(i=0;i<32;i++){X[i]=ACC_INIT[i];Y[i]=seed;}
    for(i=0;i<nb;i++) long_round(X,Y,p+i*256);
    long_round(X,Y,p+len-256);
    for(i=0;i<32;i++){X[i]^=ACC_INIT[i]; sum+=X[i];}
    sum+=(uint32_t)(len>>16>>16);
    return sum^(uint32_t)len;
}

/* ---- nmhash32 ---- */
static uint32_t nmh_0to8(uint32_t x,uint32_t seed2){
    x^=(x>>12)^(x>>6); x=mul16(x,0x776BF593u);
    x^=(x<<11)^(x>>19); x=mul16(x,0x3FB39C65u);
    x^=seed2; x^=(x>>15)^(x>>9); x=mul16(x,0xE9139917u);
    x^=(x<<16)^(x>>11); return x;
}
static uint32_t nmh_9to255(const uint8_t *p,size_t len,uint32_t seed,int gt32){
    uint32_t x[4]={P1,P2,P3,P4},y[4]; uint32_t sl=seed+(uint32_t)len; int j;
    for(j=0;j<4;j++) y[j]=sl;
    if(gt32){
        size_t r=(len-1)/32,i;
        for(i=0;i<r;i++){
            for(j=0;j<4;j++) x[j]^=rd32(p,i*32+j*4);
            for(j=0;j<4;j++) y[j]^=rd32(p,i*32+j*4+16);
            for(j=0;j<4;j++) x[j]+=y[j];
            for(j=0;j<4;j++) x[j]=mul16(x[j],M1);
            for(j=0;j<4;j++) x[j]^=(x[j]<<5)^(x[j]>>13);
            for(j=0;j<4;j++) x[j]=mul16(x[j],M2);
            for(j=0;j<4;j++) x[j]^=y[j];
            for(j=0;j<4;j++) x[j]^=(x[j]<<11)^(x[j]>>9);
            for(j=0;j<4;j++) x[j]=mul16(x[j],M3);
            for(j=0;j<4;j++) x[j]^=(x[j]>>10)^(x[j]>>20);
        }
        for(j=0;j<4;j++) x[j]^=rd32(p,len-32+j*4);
        for(j=0;j<4;j++) y[j]^=rd32(p,len-16+j*4);
    } else {
        size_t h=(len>>4)<<3;
        x[0]^=rd32(p,0); x[1]^=rd32(p,h); x[2]^=rd32(p,len-8); x[3]^=rd32(p,len-8-h);
        y[0]^=rd32(p,4); y[1]^=rd32(p,h+4); y[2]^=rd32(p,len-4); y[3]^=rd32(p,len-4-h);
    }
    for(j=0;j<4;j++) x[j]+=y[j];
    for(j=0;j<4;j++) y[j]^=(y[j]<<17)^(y[j]>>6);
    for(j=0;j<4;j++) x[j]=mul16(x[j],M1);
    for(j=0;j<4;j++) x[j]^=(x[j]<<5)^(x[j]>>13);
    for(j=0;j<4;j++) x[j]=mul16(x[j],M2);
    for(j=0;j<4;j++) x[j]^=y[j];
    for(j=0;j<4;j++) x[j]^=(x[j]<<11)^(x[j]>>9);
    for(j=0;j<4;j++) x[j]=mul16(x[j],M3);
    for(j=0;j<4;j++) x[j]^=(x[j]>>10)^(x[j]>>20);
    x[0]^=P1;x[1]^=P2;x[2]^=P3;x[3]^=P4;
    x[0]+=x[1]+x[2]+x[3];
    x[0]^=sl+(sl>>5); x[0]=mul16(x[0],M3); x[0]^=(x[0]>>10)^(x[0]>>20);
    return x[0];
}
static uint32_t nmh_aval(uint32_t x){
    x^=(x>>8)^(x>>21); x=mul16(x,0xCCE5196Du); x^=(x<<12)^(x>>7); x=mul16(x,0x464BE229u);
    return x^(x>>8)^(x>>21);
}
static uint32_t nmhash32(const void *in,size_t len,uint32_t seed){
    const uint8_t *p=in;
    if(len<=32){
        if(len>8) return nmh_9to255(p,len,seed,0);
        if(len>4){
            uint32_t x=rd32(p,0), y=rd32(p,len-4)^(P4+2+seed);
            x+=y; x^=x<<(len+7); return nmh_0to8(x,rotl(y,5));
        }
        uint32_t data;
        switch(len){
        case 0: seed+=P2; data=0; break;
        case 1: seed+=P2+(1u<<24)+(1<<1); data=p[0]; break;
        case 2: seed+=P2+(2u<<24)+(2<<1); data=rd16(p,0); break;
        case 3: seed+=P2+(3u<<24)+(3<<1); data=rd16(p,0)|((uint32_t)p[2]<<16); break;
        default: seed+=P3; data=rd32(p,0); break;
        }
        return nmh_0to8(data+seed,rotl(seed,5));
    }
    if(len<256) return nmh_9to255(p,len,seed,1);
    return nmh_aval(nmh_long(p,len,seed));
}

/* ---- nmhash32x ---- */
static uint32_t nmhx_0to4(uint32_t x,uint32_t seed){
    x^=seed; x*=0xBDAB1EA9u; x+=rotl(seed,31); x^=x>>18; x*=0xA7896A1Bu; x^=x>>12; x*=0x83796A2Du; x^=x>>16; return x;
}
static uint32_t nmhx_5to8(const uint8_t *p,size_t len,uint32_t seed){
    uint32_t x=rd32(p,0)^P3, y=rd32(p,len-4)^seed;
    x+=y; x^=x>>len; x*=0x11049A7Du; x^=x>>23; x*=0xBCCCDC7Bu; x^=rotl(y,3); x^=x>>12; x*=0x065E9DADu; x^=x>>12; return x;
}
static uint32_t nmhx_9to255(const uint8_t *p,size_t len,uint32_t seed){
    uint32_t x=P3,y=seed,a=P4,b=seed; size_t i,r=(len-1)/16;
    for(i=0;i<r;i++){
        x^=rd32(p,i*16+0); y^=rd32(p,i*16+4);
        x^=y; x*=0x11049A7Du; x^=x>>23; x*=0xBCCCDC7Bu; y=rotl(y,4); x^=y; x^=x>>12; x*=0x065E9DADu; x^=x>>12;
        a^=rd32(p,i*16+8); b^=rd32(p,i*16+12);
        a^=b; a*=0x11049A7Du; a^=a>>23; a*=0xBCCCDC7Bu; b=rotl(b,3); a^=b; a^=a>>12; a*=0x065E9DADu; a^=a>>12;
    }
    if(((uint8_t)len-1)&8){
        if(((uint8_t)len-1)&4){
            a^=rd32(p,r*16+0); b^=rd32(p,r*16+4);
            a^=b; a*=0x11049A7Du; a^=a>>23; a*=0xBCCCDC7Bu; a^=rotl(b,4); a^=a>>12; a*=0x065E9DADu;
        } else {
            a^=rd32(p,r*16)+b; a^=a>>16; a*=0xA52FB2CDu; a^=a>>15; a*=0x551E4D49u;
        }
        x^=rd32(p,len-8); y^=rd32(p,len-4);
        x^=y; x*=0x11049A7Du; x^=x>>23; x*=0xBCCCDC7Bu; x^=rotl(y,3); x^=x>>12; x*=0x065E9DADu;
    } else {
        if(((uint8_t)len-1)&4){
            a^=rd32(p,r*16)+b; a^=a>>16; a*=0xA52FB2CDu; a^=a>>15; a*=0x551E4D49u;
        }
        x^=rd32(p,len-4)+y; x^=x>>16; x*=0xA52FB2CDu; x^=x>>15; x*=0x551E4D49u;
    }
    x^=(uint32_t)len; x^=rotl(a,27); x^=x>>14; x*=0x141CC535u; return x;
}
static uint32_t nmhx_aval(uint32_t x){
    x^=x>>15; x*=0xD168AAADu; x^=x>>15; x*=0xAF723597u; x^=x>>15; return x;
}
static uint32_t nmhash32x(const void *in,size_t len,uint32_t seed){
    const uint8_t *p=in;
    if(len<=8){
        if(len>4) return nmhx_5to8(p,len,seed);
        uint32_t data;
        switch(len){
        case 0: seed+=P2; data=0; break;
        case 1: seed+=P2+(1u<<24)+(1<<1); data=p[0]; break;
        case 2: seed+=P2+(2u<<24)+(2<<1); data=rd16(p,0); break;
        case 3: seed+=P2+(3u<<24)+(3<<1); data=rd16(p,0)|((uint32_t)p[2]<<16); break;
        default: seed+=P1; data=rd32(p,0); break;
        }
        return nmhx_0to4(data,seed);
    }
    if(len<256) return nmhx_9to255(p,len,seed);
    return nmhx_aval(nmh_long(p,len,seed));
}

/* Reproduction driver: Copyright (c) 2026 Thomas Dybdahl Ahle, MIT.
 * The implementation's separate attribution/license is retained above.
 * One process, one thread; no dependencies outside this file. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <errno.h>
#include <math.h>

typedef struct { uint64_t lo, hi; } Result;
typedef Result (*Hash)(const uint8_t *, size_t, uint64_t);
typedef struct {
    const char *name; Hash hash; int bits, seed_bits, canonical;
    uint32_t verification;
} Variant;
typedef struct {
    const char *name, *a, *b;
    int variant, every_seed;
    uint64_t seed;
    Result expected;
} Pair;

#define HAS_STRUCTURE_CHECKS 0
static Result hash_0(const uint8_t *p, size_t n, uint64_t seed) { return (Result){nmhash32(p,n,(uint32_t)seed),0}; }
static const Variant variants[] = {
    {"nmhash32 v2",hash_0,32,32,0,0x12A30553},
};
static const Pair pairs[] = {
    {"selected pair","00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000","00004080000000000000000000000000000040800000000000000000000000000828208000000000000000000000000000004080000000000000000000000000",0,0,UINT64_C(0xb54cda26),{UINT64_C(0xe00f99e0),UINT64_C(0x0)}},
};
static int structure_checks(void) { return 1; }

static uint64_t rng_state[4];
static uint64_t rng_rot(uint64_t x, unsigned n) { return x<<n | x>>(64-n); }
static uint64_t splitmix(uint64_t *s) {
    uint64_t z=(*s+=UINT64_C(0x9e3779b97f4a7c15));
    z=(z^(z>>30))*UINT64_C(0xbf58476d1ce4e5b9);
    z=(z^(z>>27))*UINT64_C(0x94d049bb133111eb);
    return z^(z>>31);
}
static void rng_init(uint64_t s) { for(int i=0;i<4;i++) rng_state[i]=splitmix(&s); }
static uint64_t rng_next(void) {
    uint64_t *s=rng_state, r=rng_rot(s[1]*5,7)*9, t=s[1]<<17;
    s[2]^=s[0]; s[3]^=s[1]; s[1]^=s[2]; s[0]^=s[3]; s[2]^=t; s[3]=rng_rot(s[3],45);
    return r;
}
static int equal(Result a, Result b) { return a.lo==b.lo && a.hi==b.hi; }
static void print_result(Result h, int bits) {
    if(bits==128) printf("%016" PRIx64, h.hi);
    if(bits==32) printf("%08" PRIx32, (uint32_t)h.lo);
    else printf("%016" PRIx64, h.lo);
}
static void encode(uint8_t *p, Result h, const Variant *v) {
    int n=v->bits/8;
    for(int i=0;i<n;i++) {
        /* XXH3's SMHasher3 wrapper emits canonical big-endian output,
         * high half first at 128 bits; other registrations emit LE. */
        int j=v->canonical ? n-1-i : i;
        p[i]=(uint8_t)((j<8?h.lo:h.hi) >> (8*(j%8)));
    }
}
static uint32_t verification(const Variant *v) {
    uint8_t key[256]={0}, hashes[4096], final[16];
    int bytes=v->bits/8;
    for(int i=0;i<256;i++) {
        encode(hashes+i*bytes,v->hash(key,(size_t)i,(uint64_t)(256-i)),v);
        key[i]=(uint8_t)i;
    }
    encode(final,v->hash(hashes,(size_t)256*bytes,0),v);
    return (uint32_t)final[0] | (uint32_t)final[1]<<8 | (uint32_t)final[2]<<16 | (uint32_t)final[3]<<24;
}
static size_t decode(const char *s, uint8_t *out) {
    size_t n=strlen(s)/2;
    if(strlen(s)%2 || n>512) { fputs("invalid built-in pair\n",stderr); exit(1); }
    for(size_t i=0;i<n;i++) {
        unsigned x;
        if(sscanf(s+2*i,"%2x",&x)!=1) exit(1);
        out[i]=(uint8_t)x;
    }
    return n;
}
static uint64_t argument(const char *s, uint64_t max) {
    char *end;
    if(!*s || *s=='-' || *s=='+' || *s==' ' || *s=='\t') goto bad;
    errno=0;
    uint64_t x=strtoull(s,&end,0);
    if(errno || *end || x>max) goto bad;
    return x;
bad: fputs("invalid argument\n",stderr); exit(2);
}
int main(int argc, char **argv) {
    if(argc>3) { fprintf(stderr,"usage: %s [log2 N (0..40), default 20] [rng seed, default 1]\n",argv[0]); return 2; }
    unsigned lg=argc>1?(unsigned)argument(argv[1],40):20;
    uint64_t rseed=argc>2?argument(argv[2],UINT64_MAX):1;
    uint64_t n=UINT64_C(1)<<lg;
    for(size_t i=0;i<sizeof(variants)/sizeof(*variants);i++) {
        uint32_t got=verification(&variants[i]);
        printf("SMHasher3 %s: %08" PRIX32 " expected %08" PRIX32 " %s\n",variants[i].name,got,variants[i].verification,got==variants[i].verification?"PASS":"FAIL");
        if(got!=variants[i].verification) return 1;
    }
    if(!structure_checks()) { fputs("structural assertion failed\n",stderr); return 1; }
    if(HAS_STRUCTURE_CHECKS) puts("structural checks PASS");
    else puts("SMHasher3 checks complete; recorded output assertions follow");
    for(size_t i=0;i<sizeof(pairs)/sizeof(*pairs);i++) {
        const Pair *p=&pairs[i]; const Variant *v=&variants[p->variant];
        uint8_t a[512],b[512]; size_t na=decode(p->a,a),nb=decode(p->b,b);
        if(na==nb && !memcmp(a,b,na)) return 1;
        printf("\n%s / %s\nM (%zu B) = %s\nM' (%zu B) = %s\n",p->name,v->name,na,p->a,nb,p->b);
        Result ha=v->hash(a,na,p->seed), hb=v->hash(b,nb,p->seed);
        printf("recorded colliding seed %016" PRIx64 ": H(M)=",p->seed); print_result(ha,v->bits);
        printf(" H(M')="); print_result(hb,v->bits); puts("");
        if(!equal(ha,hb) || !equal(ha,p->expected)) { fputs("recorded output mismatch\n",stderr); return 1; }
        uint64_t count=0, first_seed=0; Result first={0,0};
        rng_init(rseed); /* Same stream per pair, deliberately correlated. */
        for(uint64_t t=0;t<n;t++) {
            uint64_t seed=rng_next();
            if(v->seed_bits==32) seed=(uint32_t)seed;
            ha=v->hash(a,na,seed); hb=v->hash(b,nb,seed);
            if(equal(ha,hb)) {
                if(!count) { first_seed=seed; first=ha; }
                count++;
            } else if(p->every_seed) {
                fprintf(stderr,"non-colliding seed %016" PRIx64 " violates every-seed claim\n",seed); return 1;
            }
        }
        double rate=(double)count/(double)n;
        printf("collisions = %" PRIu64 " / %" PRIu64 "; rate = %.12g",count,n,rate);
        if(count) printf("; log2(rate) = %.6f; sampled score = %.6f",log2(rate),log2((double)(((na>nb?na:nb)+7)/8))-log2(rate));
        else printf("; no rate/score estimate from zero hits (resolution 1/N)");
        puts("");
        if(count) { printf("first sampled colliding seed %016" PRIx64 ": H(M)=H(M')=",first_seed); print_result(first,v->bits); puts(""); }
        else puts("no sampled collision; the recorded witness above was checked separately");
    }
    return 0;
}
