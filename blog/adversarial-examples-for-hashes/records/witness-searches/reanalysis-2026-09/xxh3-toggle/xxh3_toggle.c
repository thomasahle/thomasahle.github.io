/*
 * XXH3 long path: position-independence of the shared-word lane-pair toggle.
 * Toggle (j, t): words at stripe j-2t lane 2t and stripe j-2t-1 lane 2t+1 of any 1024-byte block (or of the final
 * partial block if those stripes are accumulated and not re-read by the last stripe); both hold D = 0x7fffffff,
 * the toggle complements both.  Invisible iff (lo32(K[j]) ^ 0x7fffffff) + hi32(K[j]) = 0xfffffffe.
 * mode 0: class secrets (K[j] resampled inside the class), fresh random common content, random valid block and
 *         lane pair per trial: must collide (64 and 128 bits) every time.
 * mode 1: uniformly random secrets, same construction: counts collisions (expected rate 2^-32).
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
enum { SECRET=192, BLOCK=1024 };
static size_t LEN; static int J, MODE;
typedef struct { size_t off1, off2; } pos_t;
static pos_t P[4096]; static int NP;
static void positions(void){
  size_t nb=(LEN-1)/BLOCK, ns=((LEN-1)-nb*BLOCK)/64, last=LEN-64;
  for(size_t b=0;b<=nb;b++) for(int t=0;2*t+1<8;t++){ int a=2*t; int s1=J-a, s2=J-a-1; if(s2<0||s1>15) continue;
    size_t o1=b*BLOCK+64*s1+8*a, o2=b*BLOCK+64*s2+8*(a+1);
    if(b==nb){ if((size_t)s1>=ns) continue; if(o1+8>last||o2+8>last) continue; }
    if(NP<4096){ P[NP].off1=o1; P[NP].off2=o2; NP++; } }
}
static void class_key(rng_t *r,uint8_t *sec){ rng_fill(r,sec,SECRET);
  uint32_t l; do l=(uint32_t)rng_next(r); while(l==0xffffffffu); uint32_t h=0xfffffffeu-l; put64le(sec+8*J,((uint64_t)h<<32)|(l^0x7fffffffu)); }
typedef struct { uint64_t seed,n,hits,bad; } job_t;
static void *worker(void *arg){ job_t *j=arg; rng_t r; rng_seed(&r,j->seed); uint8_t sec[SECRET]; uint8_t *m=malloc(LEN),*m2=malloc(LEN);
  for(uint64_t it=0;it<j->n;it++){ if(MODE==0) class_key(&r,sec); else rng_fill(&r,sec,SECRET);
    rng_fill(&r,m,LEN); pos_t p=P[rng_next(&r)%NP]; put64le(m+p.off1,0x7fffffffu); put64le(m+p.off2,0x7fffffffu);
    memcpy(m2,m,LEN); put64le(m2+p.off1,~(uint64_t)0x7fffffffu); put64le(m2+p.off2,~(uint64_t)0x7fffffffu);
    uint64_t a=XXH3_64bits_withSecret(m,LEN,sec,SECRET), b=XXH3_64bits_withSecret(m2,LEN,sec,SECRET);
    XXH128_hash_t c=XXH3_128bits_withSecret(m,LEN,sec,SECRET), d=XXH3_128bits_withSecret(m2,LEN,sec,SECRET);
    int e64=a==b, e128=c.low64==d.low64&&c.high64==d.high64; j->hits+=e64&&e128; j->bad+=e64!=e128; }
  free(m); free(m2); return NULL; }
int main(int argc,char**argv){
  if(argc<7){ fprintf(stderr,"usage: %s <mode 0 class|1 random> <length> <j> <log2 trials> <rng seed> <threads>\n",argv[0]); return 2; }
  uint32_t v=smh3_verification(f64,8); printf("SMHasher3 XXH3-64 verification %08X expected 1AAEE62C %s\n",v,v==0x1AAEE62C?"PASS":"FAIL"); if(v!=0x1AAEE62C) return 1;
  MODE=atoi(argv[1]); LEN=strtoull(argv[2],0,0); J=atoi(argv[3]); int lg=atoi(argv[4]); uint64_t seed=strtoull(argv[5],0,0); int T=atoi(argv[6]);
  positions(); if(!NP){ puts("no valid position"); return 2; } uint64_t N=UINT64_C(1)<<lg;
  job_t *jb=calloc(T,sizeof(job_t)); pthread_t *th=calloc(T,sizeof(pthread_t)); uint64_t sb=seed;
  for(int t=0;t<T;t++){ jb[t].n=N/T+(t<(int)(N%T)); jb[t].seed=splitmix64(&sb); pthread_create(&th[t],0,worker,&jb[t]); }
  uint64_t h=0,bad=0; for(int t=0;t<T;t++){ pthread_join(th[t],0); h+=jb[t].hits; bad+=jb[t].bad; }
  printf("length %zu B (L = %zu), secret word j = %d, %d valid toggle positions, %s secrets, fresh random content: collided (64 and 128) %llu / 2^%d; 64/128 disagreements %llu\n",
    LEN,(LEN+7)/8,J,NP,MODE?"uniform":"class",(unsigned long long)h,lg,(unsigned long long)bad);
  return 0; }
