/*
 * Reproduction driver and C transcription: Copyright (c) 2026 Thomas Dybdahl Ahle.
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
/* ref/xxhash.h 0.8.3 embedded verbatim, including Yann Collet's BSD-2-Clause notice. */
#define XXH_INLINE_ALL
#define XXH_INLINE_ALL
#include "xxHash/xxhash.h"
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

static Result hash_0(const uint8_t *p,size_t n,uint64_t seed) { return (Result){XXH3_64bits_withSeed(p,n,seed),0}; }
static const Variant variants[] = { {"XXH3-64 0.8.3",hash_0,64,64,1,0x1AAEE62C} };
static const Pair pairs[] = {
    {"paper pair A","9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c","642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c",0,0,UINT64_C(0xcd362ec99e1e6259),{UINT64_C(0xbf1f613669d6c8ae),UINT64_C(0x0)}},
    {"base-1143 pair","8912a3da9fc464368202a1be238b7d11a56186f8dc03bd1f78bfbd2ba3386ced7093d3639fc8f076abe3f9a757e5c84290720e6340b2e7820a16a5633b9b0344773006232e3abc7d4727eafba248b46cf2a6516698382bd2fa890528924cd1b618df4aa87d1af4bcec13f92bf8a1163e657fb0885deddf709a0252fbe3ae169d","76ed5c25603b9bc97dfd5e41dc7482eea56186f8dc03bd1f78bfbd2ba3386ced7093d3639fc8f076abe3f9a757e5c84290720e6340b2e7820a16a5633b9b0344773006232e3abc7d4727eafba248b46cf2a6516698382bd2fa890528924cd1b618df4aa87d1af4bcec13f92bf8a1163e657fb0885deddf709a0252fbe3ae169d",0,0,UINT64_C(0xc8eae1baae13330b),{UINT64_C(0x448e3716c8effb94),UINT64_C(0x0)}},
};
/* Official cli/xsum_sanity_check.c vectors as recorded in the local
 * tools/bench/adversarial/selftest.cpp. PRIME64 here is 11400714785074694797. */
static int additional_validation(void) {
    uint8_t buf[256]; uint64_t byteGen=UINT64_C(2654435761);
    for (size_t i=0;i<sizeof(buf);i++) { buf[i]=(uint8_t)(byteGen>>56); byteGen*=UINT64_C(11400714785074694797); }
    const struct { size_t len; uint64_t seed, expected; } vs[] = {
        {12, 0, 0xA713DAF0DFBB77E7ULL}, {12, 11400714785074694797ULL, 0xE7303E1B2336DE0EULL},
        {24, 0, 0xA3FE70BF9D3510EBULL}, {24, 11400714785074694797ULL, 0x850E80FC35BDD690ULL},
        {48, 0, 0x397DA259ECBA1F11ULL}, {48, 11400714785074694797ULL, 0xADC2CBAA44ACC616ULL},
        {80, 0, 0xBCDEFBBB2C47C90AULL}, {80, 11400714785074694797ULL, 0xC6DD0CB699532E73ULL},
        {195, 0, 0xCD94217EE362EC3AULL}, {195, 11400714785074694797ULL, 0xBA68003D370CB3D9ULL},
    };
    for (size_t i=0;i<sizeof(vs)/sizeof(*vs);i++) {
        uint64_t got=XXH3_64bits_withSeed(buf,vs[i].len,vs[i].seed);
        printf("XXH3 sanity len=%zu seed=%016" PRIx64 ": %016" PRIx64 " expected %016" PRIx64 " %s\n",vs[i].len,vs[i].seed,got,vs[i].expected,got==vs[i].expected?"PASS":"FAIL");
        if(got!=vs[i].expected) return 0;
    }
    return XXH_VERSION_NUMBER==804;
}

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
/* Length-normalisation check. All six hashes go through the complete API.
 * Two logical xoshiro streams, processed sequentially, reproduce the historical
 * worker convention without requiring threads or changing the sample by machine.
 * Build: cc -O3 -std=c11 xxh3_64_32B_check.c -lm -o xxh3_64_32B_check
 * Run: ./xxh3_64_32B_check [log2 N (1..40), default 20] [stream salt]
 * Default salt: 0x2026091801000477. Every hit is printed; no input files needed.
 */
#if defined(__GNUC__) && !defined(__clang__)
__attribute__((noinline,noipa))
#elif defined(__clang__)
__attribute__((noinline))
#endif
static uint64_t full_api(const uint8_t *p, size_t n, uint64_t seed) {
    return XXH3_64bits_withSeed(p,n,seed);
}
static void print_hex(const uint8_t *p, size_t n) {
    for(size_t i=0;i<n;i++) printf("%02x",p[i]);
}
int main(int argc, char **argv) {
    if(argc>3) { fprintf(stderr,"usage: %s [log2 N (1..40), default 20] [stream salt]\n",argv[0]); return 2; }
    unsigned lg=argc>1?(unsigned)argument(argv[1],40):20;
    if(lg<1) { fputs("log2 N must be at least 1\n",stderr); return 2; }
    uint64_t salt=argc>2?argument(argv[2],UINT64_MAX):UINT64_C(0x2026091801000477);
    uint64_t n=UINT64_C(1)<<lg;
    uint32_t got=verification(&variants[0]);
    printf("SMHasher3 %s: %08" PRIX32 " expected %08" PRIX32 " %s\n",variants[0].name,got,variants[0].verification,got==variants[0].verification?"PASS":"FAIL");
    if(got!=variants[0].verification || !additional_validation()) return 1;
    /* Retain the package's literal historical witness assertions. */
    for(size_t j=0;j<sizeof(pairs)/sizeof(*pairs);j++) {
        uint8_t a[512],b[512]; size_t na=decode(pairs[j].a,a),nb=decode(pairs[j].b,b);
        Result ha=hash_0(a,na,pairs[j].seed),hb=hash_0(b,nb,pairs[j].seed);
        if(!equal(ha,hb) || !equal(ha,pairs[j].expected)) return 1;
        printf("recorded %s seed=%016" PRIx64 " hash=",pairs[j].name,pairs[j].seed);
        print_result(ha,64); puts(" PASS");
    }
    uint8_t a[3][512]={{0}},b[3][512]={{0}};
    const size_t lens[3]={16,32,128};
    if(decode(pairs[1].a,a[2])!=128 || decode(pairs[1].b,b[2])!=128) return 1;
    for(int j=0;j<2;j++) { memcpy(a[j],a[2],16); memcpy(b[j],b[2],16); }
    for(int j=0;j<3;j++) {
        printf("M len=%zu: ",lens[j]); print_hex(a[j],lens[j]); puts("");
        printf("Mprime len=%zu: ",lens[j]); print_hex(b[j],lens[j]); puts("");
        uint64_t ha=full_api(a[j],lens[j],pairs[1].seed),hb=full_api(b[j],lens[j],pairs[1].seed);
        printf("witness len=%zu seed=%016" PRIx64 " a=%016" PRIx64 " b=%016" PRIx64 " collision=%d\n",lens[j],pairs[1].seed,ha,hb,ha==hb);
        if(j && ha!=hb) return 1;
    }
    printf("sampling trials=%" PRIu64 " salt=0x%016" PRIx64 " logical_streams=2 execution_threads=1\n",n,salt);
    uint64_t count[3]={0},mismatch=0,fold_mismatch=0,intersection=0;
    for(unsigned worker=0;worker<2;worker++) {
        uint64_t init=salt*UINT64_C(0x9e3779b97f4a7c15)+UINT64_C(7919)*worker+1;
        printf("stream worker=%u init=0x%016" PRIx64 " trials=%" PRIu64 "\n",worker,init,n/2);
        rng_init(init);
        for(uint64_t i=0;i<n/2;i++) {
            uint64_t seed=rng_next(),ha[3],hb[3]; int hit[3];
            for(int j=0;j<3;j++) {
                ha[j]=full_api(a[j],lens[j],seed); hb[j]=full_api(b[j],lens[j],seed);
                hit[j]=(ha[j]==hb[j]); count[j]+=(unsigned)hit[j];
            }
            int fold=(XXH3_mix16B(a[0],XXH3_kSecret,seed)==XXH3_mix16B(b[0],XXH3_kSecret,seed));
            mismatch+=(hit[1]!=hit[2]); fold_mismatch+=(hit[1]!=fold); intersection+=(hit[0] && hit[1]);
            if(hit[0] || hit[1] || hit[2]) {
                printf("hit worker=%u index=%" PRIu64 " seed=%016" PRIx64,worker,(uint64_t)worker*(n/2)+i,seed);
                for(int j=0;j<3;j++) printf(" len%zu=%d a%zu=%016" PRIx64 " b%zu=%016" PRIx64,lens[j],hit[j],lens[j],ha[j],lens[j],hb[j]);
                puts("");
            }
        }
    }
    for(int j=0;j<3;j++) {
        printf("summary len=%zu L=%zu collisions=%" PRIu64 " trials=%" PRIu64 " epsilon=%" PRIu64 "/%" PRIu64,lens[j],lens[j]/8,count[j],n,count[j],n);
        if(count[j]) printf(" score=log2(%zu*%" PRIu64 "/%" PRIu64 ") approximate_score=%.17g",lens[j]/8,n,count[j],log2((double)(lens[j]/8)*n/count[j]));
        else printf(" score=unestimated_zero_hits");
        puts("");
    }
    printf("checks event_mismatch_32_128=%" PRIu64 " event_mismatch_32_fold=%" PRIu64 " intersection_16_32=%" PRIu64 " %s\n",mismatch,fold_mismatch,intersection,(mismatch||fold_mismatch)?"FAIL":"PASS");
    return (mismatch||fold_mismatch)?1:0;
}
