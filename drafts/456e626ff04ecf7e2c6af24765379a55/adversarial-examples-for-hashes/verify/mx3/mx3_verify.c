/* mx3: adapted for this package from heur2_scratch/verify-mx3/mymx3.h and construct.c.
 * The supplied independent verifier/driver is credited here;
 * adaptations: standalone C11 driver, explicit LE loads, asserted
 * SMHasher3 values and witnesses, single-thread deterministic sampling. */
/*
 * mx3, v1, v2, and v3
 *
 * original author: Jon Maiga, 2020-08-03, jonkagstrom.com, @jonkagstrom
 * license: CC0 license
 */
#include <stdint.h>
#include <stddef.h>
#include <string.h>

static uint64_t load_le(const uint8_t *p, unsigned n) {
    uint64_t x=0; for(unsigned i=0;i<n;i++) x|=(uint64_t)p[i]<<(8*i); return x;
}
/* Independent re-implementation of mx3 v3 (Jon Maiga, CC0) written from the
 * SMHasher3 hashes/mx3.cpp arithmetic, little-endian input. */
#ifndef MYMX3_H
#define MYMX3_H
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#define MX3C UINT64_C(0xbea225f9eb34556d)
static inline uint64_t my_rd64(const uint8_t *p){return load_le(p,8);}
static inline uint64_t my_rd32(const uint8_t *p){return load_le(p,4);}
static inline uint64_t my_rd16(const uint8_t *p){return load_le(p,2);}
static inline uint64_t my_mix(uint64_t x){
  x^=x>>32; x*=MX3C; x^=x>>29; x*=MX3C; x^=x>>32; x*=MX3C; x^=x>>29; return x;
}
/* g(x): the per-word public function; mix_stream(h,x) = (h + g(x)) * C */
static inline uint64_t my_g(uint64_t x){ x*=MX3C; x^=x>>39; x*=MX3C; return x; }
static inline uint64_t my_ms(uint64_t h,uint64_t x){ h+=my_g(x); h*=MX3C; return h; }
static inline uint64_t mymx3(const uint8_t *buf,size_t len,uint64_t seed){
  uint64_t h=my_ms(seed,(uint64_t)len+1);
  while(len>=64){ /* mix_stream_v3 on 4 words is identical to 4 sequential my_ms */
    for(int i=0;i<8;i++) h=my_ms(h,my_rd64(buf+8*i));
    buf+=64; len-=64;
  }
  while(len>=8){ h=my_ms(h,my_rd64(buf)); buf+=8; len-=8; }
  uint64_t v;
  switch(len){
    case 0: return my_mix(h);
    case 1: v=buf[0]; break;
    case 2: v=my_rd16(buf); break;
    case 3: v=my_rd16(buf)|((uint64_t)buf[2]<<16); break;
    case 4: v=my_rd32(buf); break;
    case 5: v=my_rd32(buf)|((uint64_t)buf[4]<<32); break;
    case 6: v=my_rd32(buf)|(my_rd16(buf+4)<<32); break;
    default: v=my_rd32(buf)|(my_rd16(buf+4)<<32)|((uint64_t)buf[6]<<48); break;
  }
  return my_mix(my_ms(h,v));
}
#endif

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

#define HAS_STRUCTURE_CHECKS 1
static Result hash_0(const uint8_t *p, size_t n, uint64_t seed) { return (Result){mymx3(p,n,seed),0}; }
static const Variant variants[] = {
    {"mx3 v3",hash_0,64,64,0,0x7B287B65},
};
static const Pair pairs[] = {
    {"1 vs 8 bytes","00","b6cbe3b809a4265c",0,1,UINT64_C(0x2cb0f69f4abea221),{UINT64_C(0x730d2d8dbe4d729e),UINT64_C(0x0)}},
    {"7 vs 8 bytes","00000000000000","f55d0131fc859d54",0,1,UINT64_C(0x3bb548a553e612ba),{UINT64_C(0xeebfade252d2c35f),UINT64_C(0x0)}},
};
static int structure_checks(void) { return my_g(UINT64_C(0x5c26a409b8e3cbb6))==my_g(0)+MX3C*(my_g(2)-my_g(9)); }

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
