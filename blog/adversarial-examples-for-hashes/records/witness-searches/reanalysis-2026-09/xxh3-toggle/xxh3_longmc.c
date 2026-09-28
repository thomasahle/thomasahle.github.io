/*
 * XXH3 (64 and 128) long-input weak-key multicollision under a uniformly random 192-byte secret
 * (XXH3_64bits_withSecret / XXH3_128bits_withSecret).
 *
 * Long path (len > 240): for stripe n (64 B) of every block and lane i (0..7),
 *     x = D[n][i] ^ K[n+i]          (K[j] = LE64 of secret bytes 8j..8j+7)
 *     acc[i^1] += D[n][i]           (raw, unkeyed)
 *     acc[i]   += lo32(x) * hi32(x)
 * and a bijective scramble after each 16-stripe block.  Block contributions are additive, so
 * whether two block contents give the same accumulators depends only on the block contents and
 * the secret, never on the running state: the SAME condition decides every block.
 *
 * Toggle t (t = 0..3, lanes a = 2t, b = 2t+1, fixed secret word K = K[j], j = 7):
 *     position (lane a, stripe j-a) and (lane b, stripe j-b) both use K; both hold the word D;
 *     the toggle complements both words.  acc[a] changes by dg + dD (product of the first
 *     position, raw word of the second), acc[b] by the same dg + dD, where
 *     dD = ~D - D = M - 2D  and  dg = M32 * (M32 - l - h),  l = lo32(D^K), h = hi32(D^K).
 * With D = 0x000000007fffffff the toggle is invisible iff  l + h = 0xfffffffe, i.e.
 *     (lo32(K) ^ 0x7fffffff) + hi32(K) = 0xfffffffe          (2^32 - 1 of 2^64 values of K).
 * All 4 toggles of every block (and of the final partial block) are governed by that single
 * condition, so a message with B full blocks plus a 576-byte tail gives a 2^(4(B+1))-way set
 * that collides in full (64-bit and 128-bit outputs) for every secret in the class, density
 * (2^32-1)/2^64 = 2^-32.0000000003.
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
enum { SECRET=192, J=7, BLOCK=1024 };
static const uint64_t Dw=UINT64_C(0x000000007fffffff);
static int B;           /* full blocks */
static size_t LEN;      /* 1024*B + 576 */
/* member number c (4(B+1) bits) -> message */
static void build(uint8_t *m,uint64_t c){
  memset(m,0,LEN);
  for(int blk=0;blk<=B;blk++) for(int t=0;t<4;t++){
    int bit=(c>>(4*blk+t))&1; uint64_t w=bit?~Dw:Dw;
    for(int k=0;k<2;k++){ int lane=2*t+k, stripe=J-lane; put64le(m+(size_t)blk*BLOCK+(size_t)stripe*64+(size_t)lane*8,w); }
  }
}
static int in_class(const uint8_t *sec){ uint64_t K=0; for(int i=0;i<8;i++) K|=(uint64_t)sec[8*J+i]<<(8*i);
  uint32_t l=(uint32_t)K^0x7fffffffu, h=(uint32_t)(K>>32); return (uint64_t)l+h==0xfffffffeu; }
static void class_key(rng_t *r,uint8_t *sec){ rng_fill(r,sec,SECRET);
  uint32_t l; do l=(uint32_t)rng_next(r); while(l==0xffffffffu); uint32_t h=0xfffffffeu-l; uint64_t K=((uint64_t)h<<32)|(l^0x7fffffffu); put64le(sec+8*J,K); }
static int cmpu(const void*a,const void*b){uint64_t x=*(const uint64_t*)a,y=*(const uint64_t*)b;return x<y?-1:x>y;}
static uint64_t maxclass(uint64_t *v,size_t n){ qsort(v,n,8,cmpu); uint64_t best=1,run=1; for(size_t i=1;i<n;i++){ run=(v[i]==v[i-1])?run+1:1; if(run>best) best=run;} return best; }

typedef struct { int mode; uint64_t seed, n, hits, hits_in_class, bad; int have; uint8_t sec[SECRET]; uint64_t *mc; } job_t;
static uint8_t *M0,*M1;
static void *worker(void *arg){
  job_t *j=arg; rng_t r; rng_seed(&r,j->seed); uint8_t sec[SECRET]; size_t N=(size_t)1<<(4*(B+1));
  uint8_t *m=malloc(LEN); uint64_t *h64=malloc(N*8), *hh=malloc(N*8), *hl=malloc(N*8), *lo16=malloc(N*8), *hi16=malloc(N*8);
  for(uint64_t it=0;it<j->n;it++){
    if(j->mode==0){ /* random secrets, single toggle pair (member 0 vs member 1) */
      rng_fill(&r,sec,SECRET);
      if(XXH3_64bits_withSecret(M0,LEN,sec,SECRET)==XXH3_64bits_withSecret(M1,LEN,sec,SECRET)){
        j->hits++; int ic=in_class(sec); j->hits_in_class+=ic;
        XXH128_hash_t a=XXH3_128bits_withSecret(M0,LEN,sec,SECRET), b=XXH3_128bits_withSecret(M1,LEN,sec,SECRET);
        if(!(a.low64==b.low64&&a.high64==b.high64)) j->bad++;
        if(!j->have&&ic){ j->have=1; memcpy(j->sec,sec,SECRET);} }
    } else if(j->mode==1){ /* class secrets: whole set must collide, 64 and 128 */
      class_key(&r,sec); int ok=1; uint64_t r64=0; XXH128_hash_t r128={0,0};
      for(size_t c=0;c<N;c++){ build(m,c); uint64_t a=XXH3_64bits_withSecret(m,LEN,sec,SECRET); XXH128_hash_t b=XXH3_128bits_withSecret(m,LEN,sec,SECRET);
        if(c==0){r64=a;r128=b;} else if(a!=r64||b.low64!=r128.low64||b.high64!=r128.high64){ok=0;break;} }
      if(ok){ j->hits++; if(!j->have){j->have=1;memcpy(j->sec,sec,SECRET);} } else j->bad++;
    } else { /* random secrets: per-key largest class of the fixed set (full 64, full 128, low16, high16) */
      rng_fill(&r,sec,SECRET);
      for(size_t c=0;c<N;c++){ build(m,c); uint64_t a=XXH3_64bits_withSecret(m,LEN,sec,SECRET); XXH128_hash_t b=XXH3_128bits_withSecret(m,LEN,sec,SECRET);
        h64[c]=a; hh[c]=b.high64^(b.low64*UINT64_C(0x9e3779b97f4a7c15)); lo16[c]=a&0xffff; hi16[c]=a>>48; (void)hl; }
      uint64_t *o=j->mc+5*it; o[0]=maxclass(h64,N); o[1]=maxclass(hh,N); o[2]=maxclass(lo16,N); o[3]=maxclass(hi16,N); o[4]=in_class(sec);
    }
  }
  free(m);free(h64);free(hh);free(hl);free(lo16);free(hi16); return NULL;
}
int main(int argc,char **argv){
  if(argc<5){ fprintf(stderr,"usage: %s <mode 0=random-secret pair rate,1=class keys full set,2=random keys largest class> <B full blocks> <log2 keys | keys> <rng seed> [threads=8]\n",argv[0]); return 2; }
  int mode=atoi(argv[1]); B=atoi(argv[2]); int lg=atoi(argv[3]); uint64_t seed=strtoull(argv[4],0,0); int T=argc>5?atoi(argv[5]):8;
  LEN=(size_t)BLOCK*B+576;
  uint32_t v64=smh3_verification(f64,8), v128=smh3_verification(f128,16);
  printf("SMHasher3 XXH3-64  verification %08X expected 1AAEE62C %s\n",v64,v64==0x1AAEE62C?"PASS":"FAIL");
  printf("SMHasher3 XXH3-128 verification %08X expected 288DAA94 %s\n",v128,v128==0x288DAA94?"PASS":"FAIL");
  if(v64!=0x1AAEE62C||v128!=0x288DAA94||XXH_versionNumber()!=803) return 1;
  M0=malloc(LEN); M1=malloc(LEN); build(M0,0); build(M1,1);
  printf("set: length %zu B (L = %zu words), %d full blocks + 576-byte tail, k = 2^%d members; member c: for block b<=%d, toggle t<4,\n"
         "  bit 4b+t of c selects D=%016llx or ~D at offsets 1024b + 64(7-2t) + 8(2t) and 1024b + 64(6-2t) + 8(2t+1); all other bytes 0\n",
         LEN,(LEN+7)/8,B,4*(B+1),B,(unsigned long long)Dw);
  uint64_t N = mode==2 ? (uint64_t)lg : (UINT64_C(1)<<lg);
  uint64_t sbase=seed; /* thread t uses the (t+1)-th splitmix64 output of the run seed */
  job_t *jb=calloc(T,sizeof(job_t)); pthread_t *th=calloc(T,sizeof(pthread_t)); uint64_t *mc=calloc(N*5,8); uint64_t off=0;
  for(int t=0;t<T;t++){ jb[t].mode=mode; jb[t].n=N/T+(t<(int)(N%T)); jb[t].seed=splitmix64(&sbase); jb[t].mc=mc+5*off; off+=jb[t].n; pthread_create(&th[t],0,worker,&jb[t]); }
  uint64_t hits=0,hic=0,bad=0; int w=-1; for(int t=0;t<T;t++){ pthread_join(th[t],0); hits+=jb[t].hits; hic+=jb[t].hits_in_class; bad+=jb[t].bad; if(w<0&&jb[t].have) w=t; }
  if(mode==0){ double lo,hi; garwood((unsigned)hits,&lo,&hi);
    printf("random secrets 2^%d: member0==member1 (64-bit) %llu times = 2^%.3f 95%% [2^%.3f, 2^%.3f]; in class %llu; 128-bit disagreements %llu\n",lg,(unsigned long long)hits,log2((double)hits/N),log2(lo/N),log2(hi/N),(unsigned long long)hic,(unsigned long long)bad);
    printf("class density (2^32-1)/2^64 = 2^%.6f\n",log2((pow(2,32)-1)/pow(2,64)));
  } else if(mode==1){ printf("class secrets 2^%d: whole 2^%d-set collided (64 and 128) for %llu, failed for %llu\n",lg,4*(B+1),(unsigned long long)hits,(unsigned long long)bad); }
  else { /* summary stats */
    for(int q=0;q<4;q++){ uint64_t *v=malloc(N*8); for(uint64_t i=0;i<N;i++) v[i]=mc[5*i+q]; qsort(v,N,8,cmpu);
      const char *nm[4]={"full 64-bit","full 128-bit","low 16 bits of 64","high 16 bits of 64"};
      printf("largest class over %llu random secrets, %s: min %llu, 1%% %llu, median %llu, max %llu\n",(unsigned long long)N,nm[q],(unsigned long long)v[0],(unsigned long long)v[N/100],(unsigned long long)v[N/2],(unsigned long long)v[N-1]); free(v);} }
  if(w>=0){ printf("explicit class secret (192 B) = "); hexprint(jb[w].sec,SECRET); puts("");
    printf("  member 0: XXH3-64 %016llx; member %llu: XXH3-64 %016llx\n",(unsigned long long)XXH3_64bits_withSecret(M0,LEN,jb[w].sec,SECRET),(unsigned long long)((UINT64_C(1)<<(4*(B+1)))-1),
      (unsigned long long)({uint8_t*m=malloc(LEN);build(m,(UINT64_C(1)<<(4*(B+1)))-1);uint64_t x=XXH3_64bits_withSecret(m,LEN,jb[w].sec,SECRET);free(m);x;}));
    XXH128_hash_t a=XXH3_128bits_withSecret(M0,LEN,jb[w].sec,SECRET); printf("  member 0: XXH3-128 %016llx%016llx\n",(unsigned long long)a.high64,(unsigned long long)a.low64); }
  return 0;
}
