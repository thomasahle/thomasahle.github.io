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
/* rapidhash v1.0: Copyright (c) 2024 Nicolas De Carli, MIT (terms above).
 * Based on wyhash by Wang Yi.
 * Transcribed from supplied harness/hashes.h; default public secret, no seed fixup. */
#include <stdint.h>
#include <stddef.h>
typedef unsigned __int128 u128;
/* Explicit little-endian reads; rd32 returns 64 bits for the <<32 expressions. */
static inline uint64_t rd32(const uint8_t *p) {
    return (uint64_t)p[0] | (uint64_t)p[1]<<8 | (uint64_t)p[2]<<16 | (uint64_t)p[3]<<24;
}
static inline uint64_t rd64(const uint8_t *p) { return rd32(p) | rd32(p+4)<<32; }
static const uint64_t RAPID_SECRET_DEFAULT[3] = {0x2d358dccaa6c78a5ull, 0x8bb84b93962eacc9ull, 0x4b33a62ed433d4a3ull};
static inline void rapid_mum(uint64_t* A, uint64_t* B) { u128 r = (u128)*A * *B; *A = (uint64_t)r; *B = (uint64_t)(r >> 64); }
static inline uint64_t rapid_mix(uint64_t A, uint64_t B) { rapid_mum(&A, &B); return A ^ B; }
static inline uint64_t rapid_readSmall(const uint8_t* p, size_t k) { return (((uint64_t)p[0]) << 56) | (((uint64_t)p[k >> 1]) << 32) | p[k - 1]; }

static inline uint64_t rapidhash_ref(const void* key, size_t len, uint64_t seed, const uint64_t* secret) {
    const uint8_t* p = (const uint8_t*)key;
    seed ^= rapid_mix(seed ^ secret[0], secret[1]) ^ len;
    uint64_t a, b;
    if (len <= 16) {
        if (len >= 4) {
            const uint8_t* plast = p + len - 4;
            a = (rd32(p) << 32) | rd32(plast);
            const uint64_t delta = ((len & 24) >> (len >> 3));
            b = ((rd32(p + delta) << 32) | rd32(plast - delta));
        } else if (len > 0) { a = rapid_readSmall(p, len); b = 0; }
        else a = b = 0;
    } else {
        size_t i = len;
        if (i > 48) {
            uint64_t see1 = seed, see2 = seed;
            while (i >= 96) {
                seed = rapid_mix(rd64(p) ^ secret[0], rd64(p + 8) ^ seed);
                see1 = rapid_mix(rd64(p + 16) ^ secret[1], rd64(p + 24) ^ see1);
                see2 = rapid_mix(rd64(p + 32) ^ secret[2], rd64(p + 40) ^ see2);
                seed = rapid_mix(rd64(p + 48) ^ secret[0], rd64(p + 56) ^ seed);
                see1 = rapid_mix(rd64(p + 64) ^ secret[1], rd64(p + 72) ^ see1);
                see2 = rapid_mix(rd64(p + 80) ^ secret[2], rd64(p + 88) ^ see2);
                p += 96; i -= 96;
            }
            if (i >= 48) {
                seed = rapid_mix(rd64(p) ^ secret[0], rd64(p + 8) ^ seed);
                see1 = rapid_mix(rd64(p + 16) ^ secret[1], rd64(p + 24) ^ see1);
                see2 = rapid_mix(rd64(p + 32) ^ secret[2], rd64(p + 40) ^ see2);
                p += 48; i -= 48;
            }
            seed ^= see1 ^ see2;
        }
        if (i > 16) {
            seed = rapid_mix(rd64(p) ^ secret[2], rd64(p + 8) ^ seed ^ secret[1]);
            if (i > 32) seed = rapid_mix(rd64(p + 16) ^ secret[2], rd64(p + 24) ^ seed);
        }
        a = rd64(p + i - 16); b = rd64(p + i - 8);
    }
    a ^= secret[1]; b ^= seed; rapid_mum(&a, &b);
    return rapid_mix(a ^ secret[0] ^ len, b ^ secret[1]);
}

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

/* Key models.  "default": one uniform 64-bit API seed per trial with the shipped public secret
 * words (the historical experiment).  "random-secret": one uniform 64-bit seed and 3
 * independent uniform 64-bit secret words per trial, passed to the hash's secret parameter; this is
 * the strongest key model the API supports and the model the article scores. */
#define NSECRET 3
static const uint64_t *active_secret = RAPID_SECRET_DEFAULT;
static Result hash_0(const uint8_t *p,size_t n,uint64_t seed) { return (Result){rapidhash_ref(p,n,seed,active_secret),0}; }
static const Variant variants[] = { {"rapidhash v1.0",hash_0,64,64,0,0} };
static const Pair pairs[] = {
    {"paper pair A","9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c","642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c",0,0,UINT64_C(0x3788f2419a81e2d6),{UINT64_C(0x8f7a71ffebd4a14b),UINT64_C(0x0)}},
};
typedef struct { const char *name; int pair; uint64_t seed, secret[NSECRET]; Result expected; } KeyWitness;
static const uint64_t *const default_secret = RAPID_SECRET_DEFAULT;
/* Random-secret witness from the 2026-09-19 measurement (records/witness-searches/random-secret/rapid1/):
 * seed and three secret words, both messages of pair A hash to 4329ec0defb7f826. */
static const KeyWitness key_witnesses[] = {
    {"pair A",0,UINT64_C(0x3879cdfddc782ad3),{UINT64_C(0xd0d81d65fd961dff),UINT64_C(0xa9132a2f5b5d4f54),UINT64_C(0xd72e7f4d4f7270f5)},{UINT64_C(0x4329ec0defb7f826),UINT64_C(0x0)}},
};
static int additional_validation(void) {
    uint64_t got=hash_0((const uint8_t *)"message digest",14,3).lo;
    const uint64_t expected=UINT64_C(0x0031cdc21324150f);
    printf("harness message digest, seed 3: %016" PRIx64 " expected %016" PRIx64 " %s\n",got,expected,got==expected?"PASS":"FAIL");
    return got==expected;
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
int main(int argc, char **argv) {
    if(argc>4) { fprintf(stderr,"usage: %s [log2 N (0..40), default 20] [rng seed, default 1] [default|random-secret]\n",argv[0]); return 2; }
    unsigned lg=argc>1?(unsigned)argument(argv[1],40):20;
    uint64_t rseed=argc>2?argument(argv[2],UINT64_MAX):1;
    int random_secret=0;
    if(argc>3) {
        if(!strcmp(argv[3],"random-secret")) random_secret=1;
        else if(strcmp(argv[3],"default")) { fputs("key model must be default or random-secret\n",stderr); return 2; }
    }
    uint64_t n=UINT64_C(1)<<lg;
    for(size_t i=0;i<sizeof(variants)/sizeof(*variants);i++) {
        if (!variants[i].verification) continue; /* Validated by a harness vector below. */
        uint32_t got=verification(&variants[i]);
        printf("SMHasher3 %s: %08" PRIX32 " expected %08" PRIX32 " %s\n",variants[i].name,got,variants[i].verification,got==variants[i].verification?"PASS":"FAIL");
        if(got!=variants[i].verification) return 1;
    }
    if (!additional_validation()) return 1;
    printf("key model: %s\n",random_secret?"uniform 64-bit seed and three uniform 64-bit secret words per trial (256 bits), rapidhash_internal(key,len,seed,secret)":"uniform 64-bit API seed; shipped public secret words (default)");
    for(size_t i=0;i<sizeof(pairs)/sizeof(*pairs);i++) {
        const Pair *p=&pairs[i]; const Variant *v=&variants[p->variant];
        uint8_t a[512],b[512]; size_t na=decode(p->a,a),nb=decode(p->b,b);
        if(na==nb && !memcmp(a,b,na)) return 1;
        printf("\n%s / %s\nM (%zu B) = %s\nM' (%zu B) = %s\n",p->name,v->name,na,p->a,nb,p->b);
        Result ha=v->hash(a,na,p->seed), hb=v->hash(b,nb,p->seed);
        printf("recorded colliding seed %016" PRIx64 ": H(M)=",p->seed); print_result(ha,v->bits);
        printf(" H(M')="); print_result(hb,v->bits); puts("");
        if(!equal(ha,hb) || !equal(ha,p->expected)) { fputs("recorded output mismatch\n",stderr); return 1; }
        if(random_secret) for(size_t k=0;k<sizeof(key_witnesses)/sizeof(*key_witnesses);k++) {
            const KeyWitness *w=&key_witnesses[k]; if(w->pair!=(int)i) continue;
            active_secret=w->secret;
            ha=v->hash(a,na,w->seed); hb=v->hash(b,nb,w->seed);
            active_secret=default_secret;
            printf("recorded colliding key (random-secret model) seed %016" PRIx64 " secret",w->seed);
            for(int j=0;j<NSECRET;j++) printf("%c%016" PRIx64,j?',':' ',w->secret[j]);
            printf(": H(M)="); print_result(ha,v->bits); printf(" H(M')="); print_result(hb,v->bits); puts("");
            if(!equal(ha,hb) || !equal(ha,w->expected)) { fputs("recorded random-secret output mismatch\n",stderr); return 1; }
        }
        uint64_t count=0, first_seed=0, first_secret[NSECRET]={0}, words[NSECRET]; Result first={0,0};
        rng_init(rseed); /* Same stream per pair, deliberately correlated. */
        for(uint64_t t=0;t<n;t++) {
            uint64_t seed=rng_next();
            if(v->seed_bits==32) seed=(uint32_t)seed;
            if(random_secret) { for(int j=0;j<NSECRET;j++) words[j]=rng_next(); active_secret=words; }
            ha=v->hash(a,na,seed); hb=v->hash(b,nb,seed);
            if(equal(ha,hb)) {
                if(!count) { first_seed=seed; first=ha; memcpy(first_secret,words,sizeof first_secret); }
                count++;
            } else if(p->every_seed) {
                fprintf(stderr,"non-colliding seed %016" PRIx64 " violates every-seed claim\n",seed); return 1;
            }
        }
        active_secret=default_secret;
        double rate=(double)count/(double)n;
        printf("collisions = %" PRIu64 " / %" PRIu64 "; rate = %.12g",count,n,rate);
        if(count) printf("; log2(rate) = %.6f; sampled score = %.6f",log2(rate),log2((double)(((na>nb?na:nb)+7)/8))-log2(rate));
        else printf("; log2(rate) = -inf (zero hits; no population-rate estimate)");
        puts("");
        if(count) {
            printf("first sampled colliding seed %016" PRIx64,first_seed);
            if(random_secret) { printf(" secret"); for(int j=0;j<NSECRET;j++) printf("%c%016" PRIx64,j?',':' ',first_secret[j]); }
            printf(": H(M)="); print_result(first,v->bits); printf(" H(M')="); print_result(first,v->bits); puts("");
        }
        else puts("no sampled collision; the recorded witness above was checked separately");
    }
    return 0;
}
