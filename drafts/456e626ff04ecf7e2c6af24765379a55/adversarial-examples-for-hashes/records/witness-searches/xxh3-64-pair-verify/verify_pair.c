/* Independent verifier for a fixed XXH3-64 pair under a uniform 64-bit API seed.
 * Links the UPSTREAM xxHash v0.8.3 library (xxhash.c compiled separately; no
 * XXH_INLINE_ALL, no port).  Sampler: splitmix64(salt ^ worker-tag) -> xoshiro256**,
 * one independent stream per worker, N/W full-width 64-bit seeds each.
 * usage: verify_pair LOG2N SALT_HEX WORKERS M_HEX MPRIME_HEX [WITNESS_SEED_HEX EXPECTED_H_HEX]
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include "xxhash.h"

static uint64_t splitmix64(uint64_t *s){uint64_t z=(*s+=0x9e3779b97f4a7c15ULL);z=(z^(z>>30))*0xbf58476d1ce4e5b9ULL;z=(z^(z>>27))*0x94d049bb133111ebULL;return z^(z>>31);}
static inline uint64_t rotl(uint64_t x,int k){return (x<<k)|(x>>(64-k));}
typedef struct{uint64_t s[4];} xo;
static uint64_t xo_next(xo*r){uint64_t*s=r->s;uint64_t res=rotl(s[1]*5,7)*9;uint64_t t=s[1]<<17;s[2]^=s[0];s[3]^=s[1];s[1]^=s[2];s[0]^=s[3];s[2]^=t;s[3]=rotl(s[3],45);return res;}

static unsigned char M[64],MP[64]; static size_t LEN;
typedef struct{int id;uint64_t n,salt;uint64_t hits;uint64_t first_seed[8],first_h[8];int nfirst;} job;
static void* work(void*p){job*j=p;uint64_t sm=j->salt^(0x6a09e667f3bcc908ULL*(uint64_t)(j->id+1));xo r;for(int i=0;i<4;i++)r.s[i]=splitmix64(&sm);
 uint64_t hits=0;for(uint64_t i=0;i<j->n;i++){uint64_t seed=xo_next(&r);XXH64_hash_t a=XXH3_64bits_withSeed(M,LEN,seed);XXH64_hash_t b=XXH3_64bits_withSeed(MP,LEN,seed);if(a==b){if(j->nfirst<8){j->first_seed[j->nfirst]=seed;j->first_h[j->nfirst]=a;j->nfirst++;}hits++;}}
 j->hits=hits;return 0;}
static int hex2bin(const char*h,unsigned char*o){size_t l=strlen(h);if(l%2)return -1;for(size_t i=0;i<l/2;i++){unsigned v;if(sscanf(h+2*i,"%2x",&v)!=1)return -1;o[i]=(unsigned char)v;}return (int)(l/2);}

/* upstream cli/xsum_sanity_check.c vectors for XXH3_64bits_withSeed */
static void fill(unsigned char*b,size_t n){uint64_t g=2654435761ULL;for(size_t i=0;i<n;i++){b[i]=(unsigned char)(g>>56);g*=0x9e3779b185ebca8dULL;}}
static int sanity(void){struct{size_t len;uint64_t seed,exp;}v[]={{0,0,0x2D06800538D394C2ULL},{0,0x9e3779b185ebca8dULL,0xA8A6B918B2F0364AULL},{1,0,0xC44BDFF4074EECDBULL},{1,0x9e3779b185ebca8dULL,0x032BE332DD766EF8ULL},{6,0,0x27B56A84CD2D7325ULL},{6,0x9e3779b185ebca8dULL,0x84589C116AB59AB9ULL},{12,0,0xA713DAF0DFBB77E7ULL},{12,0x9e3779b185ebca8dULL,0xE7303E1B2336DE0EULL},{24,0,0xA3FE70BF9D3510EBULL},{24,0x9e3779b185ebca8dULL,0x850E80FC35BDD690ULL},{48,0,0x397DA259ECBA1F11ULL},{48,0x9e3779b185ebca8dULL,0xADC2CBAA44ACC616ULL},{80,0,0xBCDEFBBB2C47C90AULL},{80,0x9e3779b185ebca8dULL,0xC6DD0CB699532E73ULL},{195,0,0xCD94217EE362EC3AULL},{195,0x9e3779b185ebca8dULL,0xBA68003D370CB3D9ULL},{403,0,0xCDEB804D65C6DEA4ULL},{403,0x9e3779b185ebca8dULL,0x6259F6ECFD6443FDULL},{512,0,0x617E49599013CB6BULL},{512,0x9e3779b185ebca8dULL,0x3CE457DE14C27708ULL},{2048,0,0xDD59E2C3A5F038E0ULL},{2048,0x9e3779b185ebca8dULL,0x66F81670669ABABCULL},{2240,0,0x6E73A90539CF2948ULL},{2240,0x9e3779b185ebca8dULL,0x757BA8487D1B5247ULL},{2367,0,0xCB37AEB9E5D361EDULL},{2367,0x9e3779b185ebca8dULL,0xD2DB3415B942B42AULL}};
 unsigned char b[4096];fill(b,sizeof b);int bad=0;for(size_t i=0;i<sizeof v/sizeof v[0];i++){uint64_t h=XXH3_64bits_withSeed(b,v[i].len,v[i].seed);printf("sanity len=%zu seed=%016llx: %016llx expected %016llx %s\n",v[i].len,(unsigned long long)v[i].seed,(unsigned long long)h,(unsigned long long)v[i].exp,h==v[i].exp?"PASS":"FAIL");if(h!=v[i].exp)bad++;}return bad;}

int main(int argc,char**argv){if(argc<6){fprintf(stderr,"usage\n");return 2;}
 int lg=atoi(argv[1]);uint64_t salt=strtoull(argv[2],0,16);int W=atoi(argv[3]);
 int l1=hex2bin(argv[4],M),l2=hex2bin(argv[5],MP);if(l1<=0||l1!=l2){fprintf(stderr,"bad pair\n");return 2;}LEN=(size_t)l1;
 printf("xxHash library version %u (XXH_VERSION_NUMBER %u)\n",XXH_versionNumber(),XXH_VERSION_NUMBER);
 if(XXH_versionNumber()!=803){fprintf(stderr,"not 0.8.3\n");return 1;}
 if(sanity()){fprintf(stderr,"sanity FAIL\n");return 1;}
 printf("M  = %s\nM' = %s\nlen = %zu\n",argv[4],argv[5],LEN);
 if(argc>=8){uint64_t ws=strtoull(argv[6],0,16),eh=strtoull(argv[7],0,16);uint64_t a=XXH3_64bits_withSeed(M,LEN,ws),b=XXH3_64bits_withSeed(MP,LEN,ws);printf("witness seed %016llx: H(M)=%016llx H(M')=%016llx expected %016llx %s\n",(unsigned long long)ws,(unsigned long long)a,(unsigned long long)b,(unsigned long long)eh,(a==b&&a==eh)?"PASS":"FAIL");if(!(a==b&&a==eh))return 1;}
 {uint64_t a=XXH3_64bits_withSeed(M,LEN,0),b=XXH3_64bits_withSeed(MP,LEN,0);printf("seed 0: H(M)=%016llx H(M')=%016llx %s\n",(unsigned long long)a,(unsigned long long)b,a==b?"COLLIDE":"differ");}
 uint64_t N=1ULL<<lg;job*J=calloc(W,sizeof*J);pthread_t*T=malloc(W*sizeof*T);
 for(int w=0;w<W;w++){J[w].id=w;J[w].n=N/W+(w<(int)(N%W)?1:0);J[w].salt=salt;pthread_create(&T[w],0,work,&J[w]);}
 uint64_t hits=0;for(int w=0;w<W;w++){pthread_join(T[w],0);hits+=J[w].hits;printf("worker %d: %llu / %llu\n",w,(unsigned long long)J[w].hits,(unsigned long long)J[w].n);for(int k=0;k<J[w].nfirst&&k<2;k++)printf("  colliding seed %016llx H=%016llx\n",(unsigned long long)J[w].first_seed[k],(unsigned long long)J[w].first_h[k]);}
 double eps=(double)hits/(double)N;printf("salt=%016llx N=2^%d workers=%d collisions = %llu / %llu; rate = %.6e; log2(rate) = %.4f; cap bits (L=%zu) = %.4f\n",(unsigned long long)salt,lg,W,(unsigned long long)hits,(unsigned long long)N,eps,log2(eps),LEN/8,log2((double)(LEN/8)/eps));
 return 0;}
