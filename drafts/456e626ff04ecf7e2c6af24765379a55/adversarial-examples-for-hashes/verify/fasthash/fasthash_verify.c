/* fasthash: adapted for this package from heur2_scratch/verify-fasthash/vf.c and mine/vfh.c.
 * The supplied independent verifier/driver is credited here;
 * adaptations: standalone C11 driver, explicit LE loads, asserted
 * SMHasher3 values and witnesses, single-thread deterministic sampling. */
/*
 * fasthash
 * Copyright (C) 2021-2022  Frank J. T. Wojcik
 * Copyright (C) 2012 Zilong Tan (eric.zltan@gmail.com)
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <stdint.h>
#include <stddef.h>
#include <string.h>

static uint64_t load_le(const uint8_t *p, unsigned n) {
    uint64_t x=0; for(unsigned i=0;i<n;i++) x|=(uint64_t)p[i]<<(8*i); return x;
}
static const uint64_t M = 0x880355f21e6d1965ULL;
static const uint64_t MIXC = 0x2127599bf4325c37ULL;

static inline uint64_t mix(uint64_t h){ h ^= h>>23; h *= MIXC; h ^= h>>47; return h; }
static inline uint32_t fold(uint64_t h){ return (uint32_t)(h - (h>>32)); }

static uint64_t fh64(const uint8_t *p, size_t len, uint64_t seed){
    uint64_t h = seed ^ (len * M);
    size_t n = len & ~(size_t)7;
    for (size_t i = 0; i < n; i += 8){ uint64_t v=load_le(p+i,8); h ^= mix(v); h *= M; }
    uint64_t v = 0; size_t r = len & 7;
    for (size_t i = 0; i < r; i++) v |= (uint64_t)p[n+i] << (8*i);
    if (r){ h ^= mix(v); h *= M; }
    return mix(h);
}
static uint32_t fh32(const uint8_t *p, size_t len, uint64_t seed){ return fold(fh64(p,len,seed)); }

// inverse of mix
static uint64_t inv_odd(uint64_t a){ uint64_t x = a; for(int i=0;i<6;i++) x *= 2 - a*x; return x; }
static uint64_t unxs(uint64_t h, int s){ uint64_t r = h; for(int i=0;i<64;i+=s) r = h ^ (r>>s); return r; }
static uint64_t mix_inv(uint64_t h){ h = unxs(h,47); h *= inv_odd(MIXC); h = unxs(h,23); return h; }


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
static Result hash_0(const uint8_t *p, size_t n, uint64_t seed) { return (Result){fh64(p,n,seed),0}; }
static Result hash_1(const uint8_t *p, size_t n, uint64_t seed) { return (Result){fh32(p,n,seed),0}; }
static Result hash_2(const uint8_t *p, size_t n, uint64_t seed) { return (Result){fh32(p,n,(uint32_t)seed),0}; }
static const Variant variants[] = {
    {"fasthash-64",hash_0,64,64,0,0xA16231A7},
    {"fasthash-32 SMHasher3 64-bit seed",hash_1,32,64,0,0xE9481AFC},
    {"fasthash-32 upstream 32-bit seed",hash_2,32,32,0,0xE9481AFC},
};
static const Pair pairs[] = {
    {"7 vs 8 bytes","00000000000000","5e177be7d1d175f3",0,1,UINT64_C(0x0123456789abcdef),{UINT64_C(0x3f624b9140899669),UINT64_C(0x0)}},
    {"7 vs 8 bytes","00000000000000","5e177be7d1d175f3",1,1,UINT64_C(0x0123456789abcdef),{UINT64_C(0x01274ad8),UINT64_C(0x0)}},
    {"7 vs 8 bytes","00000000000000","5e177be7d1d175f3",2,1,UINT64_C(0x0),{UINT64_C(0x33f6e94b),UINT64_C(0x0)}},
    {"16-byte top-bit pair","00000000000000000000000000000000","f69e1c7bcf4db16ff69e1c7bcf4db16f",0,1,UINT64_C(0x0123456789abcdef),{UINT64_C(0x1ac53b33e0d7dd39),UINT64_C(0x0)}},
    {"16-byte top-bit pair","00000000000000000000000000000000","f69e1c7bcf4db16ff69e1c7bcf4db16f",1,1,UINT64_C(0x0123456789abcdef),{UINT64_C(0xc612a206),UINT64_C(0x0)}},
    {"16-byte top-bit pair","00000000000000000000000000000000","f69e1c7bcf4db16ff69e1c7bcf4db16f",2,1,UINT64_C(0x0),{UINT64_C(0xb5f0e529),UINT64_C(0x0)}},
};
static int structure_checks(void) { return mix_inv(7*M ^ 8*M)==UINT64_C(0xf375d1d1e77b175e) && mix(UINT64_C(0x6fb14dcf7b1c9ef6))==(UINT64_C(1)<<63); }

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
