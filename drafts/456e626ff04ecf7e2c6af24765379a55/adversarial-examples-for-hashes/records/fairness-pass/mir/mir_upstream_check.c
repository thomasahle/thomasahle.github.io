/* Pair check directly against upstream vnmakarov/mir mir-hash.h (HEAD checkout).
 * Builds mir_hash (relaxed) and mir_hash_strict from the upstream header, asserts
 * the recorded witnesses, then samples 2^lg xoshiro256** seeds per pair/variant
 * (same RNG protocol as verify/mir/mir_verify.c) and also cross-checks against the
 * post's fmir re-implementation for every sampled seed. Copyright (c) 2026 T. D. Ahle, MIT. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include "mir-hash.h"

static uint64_t load_le(const uint8_t *p, unsigned n){uint64_t x=0;for(unsigned i=0;i<n;i++)x|=(uint64_t)p[i]<<(8*i);return x;}
static const uint64_t FP1 = 0x65862b62bdf5ef4dULL, FP2 = 0x288eea216831e6a7ULL;
static inline uint64_t fmum(int exact, uint64_t v, uint64_t c) {
    if (exact) { unsigned __int128 r = (unsigned __int128)v * c; return (uint64_t)(r >> 64) + (uint64_t)r; }
    uint64_t v1 = v >> 32, v2 = (uint32_t)v, c1 = c >> 32, c2 = (uint32_t)c, rm = v2 * c1 + v1 * c2;
    return v1 * c1 + (rm >> 32) + v2 * c2 + (rm << 32);
}
static inline uint64_t fkeypart(const uint8_t *v, size_t len){size_t i,start=0;uint64_t tail=0;if(len>=4){tail=(uint64_t)load_le(v,4)<<32;start=4;}for(i=start;i<len;i++)tail=(tail>>8)|((uint64_t)v[i]<<56);return tail;}
static inline uint64_t fmir(int exact, const void *in, size_t olen, uint64_t seed) {
    const uint8_t *v=(const uint8_t*)in; uint64_t r=seed+olen; size_t len=olen;
    for(;len>=16;len-=16,v+=16){r^=fmum(exact,load_le(v,8),FP1);r^=fmum(exact,load_le(v+8,8),FP2);r^=fmum(exact,r,FP1);}
    if(len>=8){r^=fmum(exact,load_le(v,8),FP1);len-=8;v+=8;}
    if(len)r^=fmum(exact,fkeypart(v,len),FP2);
    r^=fmum(exact,r,FP1);r^=fmum(exact,r,FP2);return r;
}
static uint64_t rng_state[4];
static uint64_t rot(uint64_t x,unsigned n){return x<<n|x>>(64-n);}
static uint64_t splitmix(uint64_t *s){uint64_t z=(*s+=UINT64_C(0x9e3779b97f4a7c15));z=(z^(z>>30))*UINT64_C(0xbf58476d1ce4e5b9);z=(z^(z>>27))*UINT64_C(0x94d049bb133111eb);return z^(z>>31);}
static void rng_init(uint64_t s){for(int i=0;i<4;i++)rng_state[i]=splitmix(&s);}
static uint64_t rng_next(void){uint64_t *s=rng_state,r=rot(s[1]*5,7)*9,t=s[1]<<17;s[2]^=s[0];s[3]^=s[1];s[1]^=s[2];s[0]^=s[3];s[2]^=t;s[3]=rot(s[3],45);return r;}
static size_t decode(const char *s,uint8_t *o){size_t n=strlen(s)/2;for(size_t i=0;i<n;i++){unsigned x;sscanf(s+2*i,"%2x",&x);o[i]=(uint8_t)x;}return n;}
typedef struct{const char*name,*a,*b;uint64_t seed,exp_exact,exp_strict;}Pair;
static const Pair pairs[]={
 {"8-byte p1 pair","0000000000000000","0b152b092b2ac03c",UINT64_C(0x910a2dec89025cc1),UINT64_C(0x5e900c9f273619d2),UINT64_C(0x5e900c9f273619d2)},
 {"16-byte p2 pair","00000000000000005555555555555555","0000000000000000aaaaaaaaaaaaaaaa",UINT64_C(0x22118258a9d111a0),UINT64_C(0x4d337930c595fdd0),UINT64_C(0x66c4631e3e47d736)},
 {"24-byte p1 pair (claimant-only)","000000000000000000000000000000000000000000000000","000000000000000000000000000000000b152b092b2ac03c",UINT64_C(0x22118258a9d111a0),UINT64_C(0xf467d2cc3aebf462),UINT64_C(0x33119ec2835f3a8f)},
};
int main(int argc,char**argv){
    unsigned lg=argc>1?(unsigned)atoi(argv[1]):20; uint64_t rseed=argc>2?strtoull(argv[2],0,0):1; uint64_t n=UINT64_C(1)<<lg;
    /* seed-fixed reference outputs from the records */
    uint8_t z8[8]={0}; printf("seed 0: exact %016" PRIx64 " strict %016" PRIx64 "\n",mir_hash(z8,8,0),mir_hash_strict(z8,8,0));
    printf("seed 1: exact %016" PRIx64 " strict %016" PRIx64 "\n",mir_hash(z8,8,1),mir_hash_strict(z8,8,1));
    printf("seed+len=0: exact %016" PRIx64 " strict %016" PRIx64 "\n",mir_hash(z8,8,UINT64_C(0xfffffffffffffff8)),mir_hash_strict(z8,8,UINT64_C(0xfffffffffffffff8)));
    int fail=0;
    for(size_t i=0;i<sizeof pairs/sizeof*pairs;i++){
        const Pair*p=&pairs[i];uint8_t a[64],b[64];size_t na=decode(p->a,a),nb=decode(p->b,b);
        uint64_t ea=mir_hash(a,na,p->seed),eb=mir_hash(b,nb,p->seed),sa=mir_hash_strict(a,na,p->seed),sb=mir_hash_strict(b,nb,p->seed);
        printf("\n%s\nM = %s\nM' = %s\nrecorded seed %016" PRIx64 ": upstream mir_hash %016" PRIx64 " / %016" PRIx64 " (expected %016" PRIx64 ") ; mir_hash_strict %016" PRIx64 " / %016" PRIx64 " (expected %016" PRIx64 ")\n",p->name,p->a,p->b,p->seed,ea,eb,p->exp_exact,sa,sb,p->exp_strict);
        if(ea!=eb||ea!=p->exp_exact||sa!=sb||sa!=p->exp_strict){puts("RECORDED OUTPUT MISMATCH");fail=1;}
        uint64_t ce=0,cs=0,xm=0; rng_init(rseed);
        for(uint64_t t=0;t<n;t++){uint64_t s=rng_next();
            uint64_t he=mir_hash(a,na,s),he2=mir_hash(b,nb,s),hs=mir_hash_strict(a,na,s),hs2=mir_hash_strict(b,nb,s);
            ce+=he==he2; cs+=hs==hs2;
            if(he!=fmir(1,a,na,s)||he2!=fmir(1,b,nb,s)||hs!=fmir(0,a,na,s)||hs2!=fmir(0,b,nb,s))xm++;
        }
        printf("upstream mir_hash: collisions = %" PRIu64 " / %" PRIu64 "\nupstream mir_hash_strict: collisions = %" PRIu64 " / %" PRIu64 "\nfmir re-implementation mismatches vs upstream over all sampled seeds: %" PRIu64 "\n",ce,n,cs,n,xm);
        if(ce!=n||cs!=n||xm){puts("EVERY-SEED CLAIM OR EQUIVALENCE FAILED");fail=1;}
    }
    puts(fail?"\nRESULT: FAIL":"\nRESULT: PASS");return fail;
}
