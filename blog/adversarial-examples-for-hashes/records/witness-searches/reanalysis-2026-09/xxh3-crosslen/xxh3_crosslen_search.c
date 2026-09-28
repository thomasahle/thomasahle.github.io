/*
 * XXH3 cross-length pairs under a uniformly random 192-byte secret (XXH3_*_withSecret).
 *
 * P64 (XXH3-64, lengths 5 and 8, L = 1):
 *   len 4..8 path: keyed = input64 ^ bitflip, bitflip = S[8..16) ^ S[16..24) (uniform);
 *   rrmxmx: h = L(keyed) (L(x) = x ^ rotl49 ^ rotl24, GF(2)-linear, invertible), A = C*h,
 *   B = A ^ ((A>>35) + len), then a bijection.  Messages of different lengths collide iff
 *   A' = A ^ (t+len) ^ (t+len') with t = A>>35, i.e. iff the XOR difference Y = h ^ h' of
 *   the two L-images equals a fixed target.  The target maximising Pr_h[...] was found by
 *   heavy-hitter search; the pair is m = 00^5, m' = the 8 bytes whose input64 = L^{-1}(Y).
 * P128 (XXH3-128, lengths 9 and 16, L = 2):
 *   len 9..16 path: m128 = (wlo ^ whi ^ Kl) * P1, low += (len-1)<<54, high += g(whi ^ Kh),
 *   then a bijection of (low, high).  The length term is cancelled by an additive difference
 *   of Z = wlo^whi^Kl that is a multiple of 2^54, and the resulting high-half difference by
 *   an additive difference in g(whi ^ Kh); both are XOR->add differentials of uniform words.
 */
#define XXH_INLINE_ALL
#define XXH_STATIC_LINKING_ONLY
#include "xxhash.h"   /* xxHash v0.8.3, unmodified; see SOURCES.md */

/* ===== begin common.h (verbatim) ===== */
/* common harness pieces: splitmix64 -> xoshiro256** RNG, hex helpers, Garwood interval */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
typedef struct { uint64_t s[4]; } rng_t;
static uint64_t splitmix64(uint64_t *x){uint64_t z=(*x+=UINT64_C(0x9e3779b97f4a7c15));z=(z^(z>>30))*UINT64_C(0xbf58476d1ce4e5b9);z=(z^(z>>27))*UINT64_C(0x94d049bb133111eb);return z^(z>>31);}
static void rng_seed(rng_t *r,uint64_t seed){for(int i=0;i<4;i++) r->s[i]=splitmix64(&seed);}
static inline uint64_t rotl64_(uint64_t x,int k){return (x<<k)|(x>>(64-k));}
static inline uint64_t rng_next(rng_t *r){uint64_t *s=r->s,res=rotl64_(s[1]*5,7)*9,t=s[1]<<17;s[2]^=s[0];s[3]^=s[1];s[1]^=s[2];s[0]^=s[3];s[2]^=t;s[3]=rotl64_(s[3],45);return res;}
static void rng_fill(rng_t *r,uint8_t *p,size_t n){for(size_t i=0;i<n;i+=8){uint64_t v=rng_next(r);size_t k=n-i<8?n-i:8;memcpy(p+i,&v,k);}}
static void hexprint(const uint8_t *p,size_t n){for(size_t i=0;i<n;i++) printf("%02x",p[i]);}
static void put64le(uint8_t *p,uint64_t v){for(int i=0;i<8;i++) p[i]=(uint8_t)(v>>(8*i));}
static void put32le(uint8_t *p,uint32_t v){for(int i=0;i<4;i++) p[i]=(uint8_t)(v>>(8*i));}
/* exact central 95% Poisson (Garwood) interval via bisection on the regularized gamma */
static double pois_cdf(unsigned k,double mu){ if(mu<=0) return 1; double s=0; for(unsigned i=0;i<=k;i++) s+=exp(-mu+i*log(mu)-lgamma(i+1.0)); return s>1?1:s; }
static void garwood(unsigned k,double *lo,double *hi){
  double a=0,b=k+50+10*sqrt(k+1.0);
  if(k==0) *lo=0; else { double L=0,H=b; for(int it=0;it<200;it++){double m=(L+H)/2; if(1-pois_cdf(k-1,m)<0.025) L=m; else H=m;} *lo=L; }
  { double L=a,H=b; for(int it=0;it<200;it++){double m=(L+H)/2; if(pois_cdf(k,m)>0.025) L=m; else H=m;} *hi=L; }
}

/* ===== end common.h ===== */

/* ===== begin smh3.h (verbatim) ===== */
/* SMHasher3 verification value: hash keys {0},{0,1},...,{0..254} of length i with seed 256-i,
   concatenate the outputs, hash that with seed 0, take the first 4 bytes little-endian.
   'canonical' = output bytes big-endian (XXH3 registrations), else little-endian. */
typedef void (*smh3_fn)(const uint8_t *,size_t,uint64_t,uint8_t *out); /* writes bits/8 bytes */
static uint32_t smh3_verification(smh3_fn f,int bytes){
  uint8_t key[256]={0},hashes[256*16],fin[16];
  for(int i=0;i<256;i++){ f(key,(size_t)i,(uint64_t)(256-i),hashes+i*bytes); key[i]=(uint8_t)i; }
  f(hashes,(size_t)256*bytes,0,fin);
  return (uint32_t)fin[0]|(uint32_t)fin[1]<<8|(uint32_t)fin[2]<<16|(uint32_t)fin[3]<<24;
}
static void enc_be(uint8_t *o,uint64_t v){for(int i=0;i<8;i++) o[i]=(uint8_t)(v>>(56-8*i));}
static void enc_le(uint8_t *o,uint64_t v){for(int i=0;i<8;i++) o[i]=(uint8_t)(v>>(8*i));}

/* ===== end smh3.h ===== */

static void f64(const uint8_t *p,size_t n,uint64_t s,uint8_t *o){ enc_be(o,XXH3_64bits_withSeed(p,n,s)); }
static void f128(const uint8_t *p,size_t n,uint64_t s,uint8_t *o){ XXH128_hash_t h=XXH3_128bits_withSeed(p,n,s); enc_be(o,h.high64); enc_be(o+8,h.low64); }

static uint64_t Lmap(uint64_t x,int r1,int r2){ return x ^ rotl64_(x,r1) ^ rotl64_(x,r2); }
/* L = 1 + x^49 + x^24 in GF(2)[x]/(x^64+1); L^64 = 1, so L^-1 = prod_{k<6} L^(2^k) */
static uint64_t Linv(uint64_t y){ for(int k=0;k<6;k++){ int a=(49<<k)&63,b=(24<<k)&63; y = y ^ (a?rotl64_(y,a):y) ^ (b?rotl64_(y,b):y); } return y; }

typedef struct { const char *name; uint8_t m[64], m2[64]; size_t n, n2; int bits; } pair_t;
static pair_t P[3];
enum { SECRET=192 };
typedef struct { int pi, api; uint64_t seed, n; uint64_t hits; int have; uint8_t sec[SECRET]; uint64_t kseed; } job_t;

static int collide(const pair_t *p,int api,const uint8_t *sec,uint64_t sd,uint64_t out[4]){
  if(p->bits==64){
    uint64_t a,b;
    if(api==0){ a=XXH3_64bits_withSecret(p->m,p->n,sec,SECRET); b=XXH3_64bits_withSecret(p->m2,p->n2,sec,SECRET);} 
    else { a=XXH3_64bits_withSeed(p->m,p->n,sd); b=XXH3_64bits_withSeed(p->m2,p->n2,sd);} 
    out[0]=a; out[1]=b; return a==b;
  } else {
    XXH128_hash_t a,b;
    if(api==0){ a=XXH3_128bits_withSecret(p->m,p->n,sec,SECRET); b=XXH3_128bits_withSecret(p->m2,p->n2,sec,SECRET);} 
    else { a=XXH3_128bits_withSeed(p->m,p->n,sd); b=XXH3_128bits_withSeed(p->m2,p->n2,sd);} 
    out[0]=a.high64; out[1]=a.low64; out[2]=b.high64; out[3]=b.low64; return a.low64==b.low64 && a.high64==b.high64;
  }
}
static void *worker(void *arg){
  job_t *j=arg; rng_t r; rng_seed(&r,j->seed); uint8_t sec[SECRET]; uint64_t o[4];
  for(uint64_t i=0;i<j->n;i++){
    uint64_t sd=0;
    if(j->api==0) rng_fill(&r,sec,SECRET); else sd=rng_next(&r);
    if(collide(&P[j->pi],j->api,sec,sd,o)){ j->hits++; if(!j->have){ j->have=1; memcpy(j->sec,sec,SECRET); j->kseed=sd; } }
  }
  return NULL;
}
int main(int argc,char **argv){
  if(argc<4){ fprintf(stderr,"usage: %s <pair 0=P64 1=P128> <log2 keys> <rng seed> [threads=8] [api 0=withSecret 1=withSeed]\n",argv[0]); return 2; }
  int pi=atoi(argv[1]); int lg=atoi(argv[2]); uint64_t seed=strtoull(argv[3],0,0); int T=argc>4?atoi(argv[4]):8; int api=argc>5?atoi(argv[5]):0;
  uint32_t v64=smh3_verification(f64,8), v128=smh3_verification(f128,16);
  printf("SMHasher3 XXH3-64  verification %08X expected 1AAEE62C %s\n",v64,v64==0x1AAEE62C?"PASS":"FAIL");
  printf("SMHasher3 XXH3-128 verification %08X expected 288DAA94 %s\n",v128,v128==0x288DAA94?"PASS":"FAIL");
  if(v64!=0x1AAEE62C||v128!=0x288DAA94||XXH_versionNumber()!=803) return 1;
  /* P64: m = 5 zero bytes (input64 = 0), m' = 8 bytes with input64 = Linv(Y) */
  { uint64_t Y=UINT64_C(0x08032aaa29309209), d=Linv(Y);
    if(Lmap(d,49,24)!=Y){ puts("Linv self-check FAIL"); return 1; }
    pair_t *p=&P[0]; p->name="XXH3-64 len5 vs len8"; p->bits=64; memset(p->m,0,16); p->n=5; p->n2=8;
    put32le(p->m2,(uint32_t)(d>>32)); put32le(p->m2+4,(uint32_t)d); }
  /* P128: m = 9 zero bytes, m' = 16 bytes (two LE words) */
  { pair_t *p=&P[1]; p->name="XXH3-128 len9 vs len16"; p->bits=128; memset(p->m,0,16); p->n=9; p->n2=16;
    put64le(p->m2,UINT64_C(0x8570540232242900)); put64le(p->m2+8,UINT64_C(0x2530540232242900)); }
  if(pi==2){ if(argc<9){fprintf(stderr,"pair 2 needs: <api> <m hex> <m' hex> <bits 64|128>\n");return 2;}
    pair_t *q=&P[2]; q->name="published pair (hex)"; q->bits=atoi(argv[8]);
    const char *a=argv[6],*b=argv[7]; q->n=strlen(a)/2; q->n2=strlen(b)/2;
    for(size_t i=0;i<q->n;i++){unsigned x;sscanf(a+2*i,"%2x",&x);q->m[i]=(uint8_t)x;} for(size_t i=0;i<q->n2;i++){unsigned x;sscanf(b+2*i,"%2x",&x);q->m2[i]=(uint8_t)x;} }
  pair_t *p=&P[pi];
  printf("pair %s\n  m  (%zu B) = ",p->name,p->n); hexprint(p->m,p->n); printf("\n  m' (%zu B) = ",p->n2); hexprint(p->m2,p->n2);
  printf("\n  L = %zu words; api = %s; keys = 2^%d; rng seed = 0x%llx; threads = %d\n",(p->n2+7)/8, api?"withSeed (default secret, uniform 64-bit seed)":"withSecret (uniform 192-byte secret)",lg,(unsigned long long)seed,T);
  uint64_t sbase=seed; /* thread t uses the (t+1)-th splitmix64 output of the run seed */
  job_t *J=calloc(T,sizeof(job_t)); pthread_t *th=calloc(T,sizeof(pthread_t)); uint64_t N=UINT64_C(1)<<lg;
  for(int t=0;t<T;t++){ J[t].pi=pi; J[t].api=api; J[t].n=N/T+(t<(int)(N%T)); J[t].seed=splitmix64(&sbase); pthread_create(&th[t],0,worker,&J[t]); }
  uint64_t hits=0; int w=-1; for(int t=0;t<T;t++){ pthread_join(th[t],0); hits+=J[t].hits; if(w<0&&J[t].have) w=t; }
  double lo,hi; garwood((unsigned)hits,&lo,&hi); double L=(double)(((p->n>p->n2?p->n:p->n2)+7)/8);
  printf("collisions %llu / 2^%d = 2^%.3f  95%% [2^%.3f, 2^%.3f]\n",(unsigned long long)hits,lg,log2((double)hits/N),log2(lo/N),log2(hi/N));
  printf("score log2(L/eps) = %.3f  95%% [%.3f, %.3f]\n",log2(L*N/hits),log2(L*N/hi),hits?log2(L*N/lo):INFINITY);
  if(w>=0){ uint64_t o[4]; collide(p,api,J[w].sec,J[w].kseed,o);
    if(api==0){ printf("explicit colliding secret (192 B) = "); hexprint(J[w].sec,SECRET); puts(""); } else printf("explicit colliding seed = 0x%016llx\n",(unsigned long long)J[w].kseed);
    if(p->bits==64) printf("H(m) = %016llx  H(m') = %016llx\n",(unsigned long long)o[0],(unsigned long long)o[1]);
    else printf("H(m) = %016llx%016llx  H(m') = %016llx%016llx (high64 low64)\n",(unsigned long long)o[0],(unsigned long long)o[1],(unsigned long long)o[2],(unsigned long long)o[3]); }
  return 0;
}
