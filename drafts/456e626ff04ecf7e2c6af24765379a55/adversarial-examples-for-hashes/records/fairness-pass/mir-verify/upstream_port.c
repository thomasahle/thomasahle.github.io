/* Independent check: call UPSTREAM vnmakarov/mir mir-hash.h (HEAD) directly
 * on the post's pairs, assert recorded outputs, sample 2^lg xoshiro256** seeds
 * (splitmix64 init, seed 1: same protocol as verify/mir/mir_verify.c),
 * and cross-check every output against the post's fmir re-implementation. */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include "mir-hash.h"

static uint64_t load_le(const uint8_t *p, unsigned n) { uint64_t x=0; for(unsigned i=0;i<n;i++) x|=(uint64_t)p[i]<<(8*i); return x; }
static const uint64_t FP1 = 0x65862b62bdf5ef4dULL, FP2 = 0x288eea216831e6a7ULL;
static inline uint64_t fmum(int exact, uint64_t v, uint64_t c) {
    if (exact) { unsigned __int128 r = (unsigned __int128)v * c; return (uint64_t)(r >> 64) + (uint64_t)r; }
    uint64_t v1 = v >> 32, v2 = (uint32_t)v, c1 = c >> 32, c2 = (uint32_t)c, rm = v2 * c1 + v1 * c2;
    return v1 * c1 + (rm >> 32) + v2 * c2 + (rm << 32);
}
static inline uint64_t fkeypart(const uint8_t *v, size_t len) {
    size_t i, start = 0; uint64_t tail = 0;
    if (len >= 4) { tail = (uint64_t)load_le(v,4) << 32; start = 4; }
    for (i = start; i < len; i++) tail = (tail >> 8) | ((uint64_t)v[i] << 56);
    return tail;
}
static uint64_t fmir(int exact, const void *in, size_t olen, uint64_t seed) {
    const uint8_t *v = in; uint64_t r = seed + olen; size_t len = olen;
    for (; len >= 16; len -= 16, v += 16) { r ^= fmum(exact, load_le(v,8), FP1); r ^= fmum(exact, load_le(v + 8,8), FP2); r ^= fmum(exact, r, FP1); }
    if (len >= 8) { r ^= fmum(exact, load_le(v,8), FP1); len -= 8; v += 8; }
    if (len) r ^= fmum(exact, fkeypart(v, len), FP2);
    r ^= fmum(exact, r, FP1); r ^= fmum(exact, r, FP2);
    return r;
}
static uint64_t rs[4];
static uint64_t splitmix(uint64_t *s){ uint64_t z=(*s+=0x9e3779b97f4a7c15ULL); z=(z^(z>>30))*0xbf58476d1ce4e5b9ULL; z=(z^(z>>27))*0x94d049bb133111ebULL; return z^(z>>31);}
static inline uint64_t rotl(uint64_t x,int k){return (x<<k)|(x>>(64-k));}
static uint64_t rng_next(void){ uint64_t r=rotl(rs[1]*5,7)*9, t=rs[1]<<17; rs[2]^=rs[0]; rs[3]^=rs[1]; rs[1]^=rs[2]; rs[0]^=rs[3]; rs[2]^=t; rs[3]=rotl(rs[3],45); return r; }
static void rng_init(uint64_t s){ for(int i=0;i<4;i++) rs[i]=splitmix(&s); }
static int hexs(const char *h, uint8_t *o){ size_t n=strlen(h)/2; for(size_t i=0;i<n;i++){unsigned b; sscanf(h+2*i,"%2x",&b); o[i]=b;} return (int)n; }
typedef struct { const char *m, *mp; uint64_t seed; uint64_t exp_exact, exp_strict; } Case;
int main(int argc,char**argv){
    unsigned lg = argc>1 ? atoi(argv[1]) : 20;
    Case cs[] = {
        {"0000000000000000","0b152b092b2ac03c",0x910a2dec89025cc1ULL,0x5e900c9f273619d2ULL,0x5e900c9f273619d2ULL},
        {"00000000000000005555555555555555","0000000000000000aaaaaaaaaaaaaaaa",0x22118258a9d111a0ULL,0x4d337930c595fdd0ULL,0x66c4631e3e47d736ULL},
    };
    int fail=0;
    /* extra recorded examples */
    uint8_t z8[8]={0}, w8[8]; hexs("0b152b092b2ac03c",w8);
    struct {uint64_t seed, ex, st;} ex8[] = {{0,0x1e30aaaa9235e65aULL,0x1e30aaaa9235e65bULL},{1,0x38d782429014d4bfULL,0x38d782429014d4bfULL},{0xfffffffffffffff8ULL,0,0}};
    for(int i=0;i<3;i++){ uint64_t a=mir_hash(z8,8,ex8[i].seed), b=mir_hash(w8,8,ex8[i].seed), c=mir_hash_strict(z8,8,ex8[i].seed), d=mir_hash_strict(w8,8,ex8[i].seed);
        printf("8B seed %016" PRIx64 ": exact %016" PRIx64 "/%016" PRIx64 " strict %016" PRIx64 "/%016" PRIx64 " expected %016" PRIx64 "/%016" PRIx64 " %s\n", ex8[i].seed,a,b,c,d,ex8[i].ex,ex8[i].st,(a==b&&c==d&&a==ex8[i].ex&&c==ex8[i].st)?"OK":"MISMATCH");
        if(!(a==b&&c==d&&a==ex8[i].ex&&c==ex8[i].st)) fail=1; }
    { uint8_t m[16]; hexs("00000000000000005555555555555555",m); uint64_t a=mir_hash(m,16,0); printf("16B seed 0 exact %016" PRIx64 " expected fd26864d6d50b5b1 %s\n",a,a==0xfd26864d6d50b5b1ULL?"OK":"MISMATCH"); if(a!=0xfd26864d6d50b5b1ULL) fail=1; }
    for(size_t c=0;c<sizeof cs/sizeof *cs;c++){
        uint8_t a[32],b[32]; int na=hexs(cs[c].m,a), nb=hexs(cs[c].mp,b);
        uint64_t he=mir_hash(a,na,cs[c].seed), he2=mir_hash(b,nb,cs[c].seed), hs=mir_hash_strict(a,na,cs[c].seed), hs2=mir_hash_strict(b,nb,cs[c].seed);
        printf("pair %zu (%d B): recorded seed %016" PRIx64 " upstream mir_hash %016" PRIx64 "/%016" PRIx64 " (exp %016" PRIx64 ") mir_hash_strict %016" PRIx64 "/%016" PRIx64 " (exp %016" PRIx64 ")\n",c,na,cs[c].seed,he,he2,cs[c].exp_exact,hs,hs2,cs[c].exp_strict);
        if(he!=he2||hs!=hs2||he!=cs[c].exp_exact||hs!=cs[c].exp_strict) fail=1;
        uint64_t N=1ULL<<lg, ce=0, cs_=0, xfail=0; rng_init(1);
        for(uint64_t i=0;i<N;i++){ uint64_t s=rng_next();
            uint64_t e1=mir_hash(a,na,s), e2=mir_hash(b,nb,s), s1=mir_hash_strict(a,na,s), s2=mir_hash_strict(b,nb,s);
            ce += e1==e2; cs_ += s1==s2;
            if (e1!=fmir(1,a,na,s) || e2!=fmir(1,b,nb,s) || s1!=fmir(0,a,na,s) || s2!=fmir(0,b,nb,s)) xfail++;
        }
        printf("pair %zu: mir_hash collisions %" PRIu64 "/%" PRIu64 "; mir_hash_strict collisions %" PRIu64 "/%" PRIu64 "; fmir cross-check mismatches %" PRIu64 "\n",c,ce,N,cs_,N,xfail);
        if(ce!=N||cs_!=N||xfail) fail=1;
    }
    printf("RESULT: %s\n", fail?"FAIL":"PASS"); return fail;
}
