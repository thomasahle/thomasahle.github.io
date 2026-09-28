/*
 * murmurhash2a_witness.c -- seed-independent (key-free) collisions and
 * multicollisions in MurmurHash2A (Austin Appleby, public domain).  Code
 * transcribed from SMHasher3 hashes/murmurhash2.cpp (MurmurHash2A_32,
 * verification 0x7FBD4396), little-endian loads; recomputed at startup.
 *
 * Each 4-byte block runs mmix(h,k): k = k*m; k ^= k>>24; k = k*m; h = h*m;
 * h ^= k.  As in MurmurHash2, an XOR difference in the top bit of k survives
 * the next block's h*=m (m odd), so flipping the top bit of block i's mixed
 * word opens a difference that flipping block i+1's top bit cancels.  With n
 * body blocks (all a multiple of 4 bytes so the tail mmix(h,t) is difference-
 * free and mmix(h,len) is identical), the 2^(n-1) messages with an even number
 * of top-bit-flipped blocks all collide under EVERY seed.
 *
 * Build: cc -std=c11 -O2 -o murmurhash2a_witness murmurhash2a_witness.c
 * Run:   ./murmurhash2a_witness [log2_keys=12] [rng_seed=0x2a] [--exhaustive]
 */
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline uint32_t GET_U32(const uint8_t *p, size_t o){return (uint32_t)p[o]|(uint32_t)p[o+1]<<8|(uint32_t)p[o+2]<<16|(uint32_t)p[o+3]<<24;}
#define mmix(h, k) { k *= m; k ^= k >> r; k *= m; h *= m; h ^= k; }

static uint32_t MurmurHash2A_32(const void *in, const size_t olen, const uint32_t seed){
    const uint32_t m = 0x5bd1e995; const uint32_t r = 24;
    size_t len = olen; uint32_t len32 = olen; uint32_t h = seed;
    const uint8_t *data = (const uint8_t*)in;
    while (len >= 4){ uint32_t k = GET_U32(data,0); mmix(h,k); data += 4; len -= 4; }
    uint32_t t = 0;
    switch (len){ case 3: t ^= data[2]<<16; /*FALLTHROUGH*/ case 2: t ^= data[1]<<8; /*FALLTHROUGH*/ case 1: t ^= data[0]; }
    mmix(h,t); mmix(h,len32);
    h ^= h >> 13; h *= m; h ^= h >> 15;
    return h;
}
static uint32_t verify(void){
    uint8_t key[256]={0}, hashes[4*256], total[4];
    for(int i=0;i<256;i++){ uint32_t h=MurmurHash2A_32(key,(size_t)i,(uint32_t)(256-i)); for(int b=0;b<4;b++) hashes[4*i+b]=(uint8_t)(h>>(8*b)); key[i]=(uint8_t)i; }
    uint32_t h=MurmurHash2A_32(hashes,sizeof hashes,0); for(int b=0;b<4;b++) total[b]=(uint8_t)(h>>(8*b));
    return (uint32_t)total[0]|(uint32_t)total[1]<<8|(uint32_t)total[2]<<16|(uint32_t)total[3]<<24;
}
static inline uint64_t ROTL64(uint64_t x,int r){return (x<<r)|(x>>(64-r));}
static uint64_t splitmix64(uint64_t*s){uint64_t z=(*s+=0x9e3779b97f4a7c15ULL);z=(z^(z>>30))*0xbf58476d1ce4e5b9ULL;z=(z^(z>>27))*0x94d049bb133111ebULL;return z^(z>>31);}
static uint64_t rs[4]; static void rng_init(uint64_t s){for(int i=0;i<4;i++) rs[i]=splitmix64(&s);}
static uint64_t rng(void){const uint64_t r=ROTL64(rs[1]*5,7)*9,t=rs[1]<<17;rs[2]^=rs[0];rs[3]^=rs[1];rs[1]^=rs[2];rs[0]^=rs[3];rs[2]^=t;rs[3]=ROTL64(rs[3],45);return r;}
static uint32_t inv32(uint32_t a){uint32_t x=a;for(int i=0;i<5;i++)x*=2-a*x;return x;}
static void put32(uint8_t*p,uint32_t v){for(int i=0;i<4;i++)p[i]=(uint8_t)(v>>(8*i));}
static void print_msg(const char*n,const uint8_t*m,size_t k){printf("  %s (%zu bytes) = ",n,k);for(size_t i=0;i<k;i++)printf("%02x",m[i]);printf("\n");}
static const uint32_t M=0x5bd1e995;
static uint32_t mix(uint32_t k){k*=M;k^=k>>24;k*=M;return k;}
static uint32_t unmix(uint32_t k){k*=inv32(M);k^=k>>24;return k*inv32(M);}
static void cube(uint8_t*out,const uint8_t*base,int n,uint64_t idx){
    int prev=0;
    for(int i=0;i<n;i++){ int open = i<n-1 ? (int)((idx>>i)&1):0; uint32_t k=mix(GET_U32(base,4*i))^((uint32_t)(open^prev)<<31); put32(out+4*i,unmix(k)); prev=open; }
}
/* distinctness check for a generated family: sort copies, compare neighbours */
static size_t dist_len;
static int dist_cmp(const void *a, const void *b) { return memcmp(a, b, dist_len); }
static uint64_t count_distinct(const uint8_t *fam, size_t len, uint64_t K) {
    uint8_t *c = malloc(K * len); memcpy(c, fam, K * len); dist_len = len;
    qsort(c, K, len, dist_cmp);
    uint64_t d = K ? 1 : 0; for (uint64_t i = 1; i < K; i++) d += memcmp(c + (i - 1) * len, c + i * len, len) != 0;
    free(c); return d;
}

int main(int argc,char**argv){
    int lg=argc>1?atoi(argv[1]):12; uint64_t rseed=argc>2?strtoull(argv[2],0,0):0x2a; int ex=argc>3&&!strcmp(argv[3],"--exhaustive");
    uint32_t v=verify(); printf("SMHasher3 verification MurmurHash2a: 0x%08" PRIX32 " (expected 0x7FBD4396)\n",v);
    if(v!=UINT32_C(0x7FBD4396)){printf("MISMATCH\n");return 1;}
    rng_init(rseed); int ok=1; uint64_t NK=UINT64_C(1)<<lg;
    uint8_t base[8]; memcpy(base,"mmh2a-8b",8);
    uint8_t a[8],b[8]; cube(a,base,2,0); cube(b,base,2,1);
    printf("\n[pair] 8 bytes (L = 1), key-free\n"); print_msg("M1",a,8); print_msg("M2",b,8);
    uint64_t c=0; for(uint64_t t=0;t<(UINT64_C(1)<<24);t++){uint32_t s=(uint32_t)rng(); c+=MurmurHash2A_32(a,8,s)==MurmurHash2A_32(b,8,s);}
    printf("  collisions %" PRIu64 " / %" PRIu64 " random 32-bit seeds\n",c,UINT64_C(1)<<24); ok&=c==(UINT64_C(1)<<24);
    if(ex){uint64_t e=0;uint32_t s=0;do{e+=MurmurHash2A_32(a,8,s)==MurmurHash2A_32(b,8,s);}while(++s);printf("  EXHAUSTIVE: %" PRIu64 " of 2^32 seeds collide\n",e);ok&=e==(UINT64_C(1)<<32);}
    { const int n=17; const uint64_t K=UINT64_C(1)<<(n-1); uint8_t bb[68]; memcpy(bb,"MurmurHash2A key-free 2^16-way multicollision -- 68 bytes total..",64); memcpy(bb+64,"tail",4);
      uint8_t*fam=malloc(K*68); for(uint64_t i=0;i<K;i++) cube(fam+68*i,bb,n,i);
      { uint64_t d=count_distinct(fam,68,K); printf("  (next cube) distinct messages: %" PRIu64 " of %" PRIu64 "\n",d,K); ok&=d==K; }
      printf("\n[cube] 2^16 messages of 68 bytes (L = 9), key-free; generator cube(base,17,idx)\n");
      print_msg("X[65535]",fam+68*(K-1),68);
      uint64_t all=0; for(uint64_t t=0;t<NK;t++){uint32_t s=(uint32_t)rng(),h0=MurmurHash2A_32(fam,68,s);uint64_t same=0;for(uint64_t i=0;i<K;i++)same+=MurmurHash2A_32(fam+68*i,68,s)==h0;all+=same==K;}
      printf("  all 65536 equal for %" PRIu64 " / %" PRIu64 " random seeds\n",all,NK); ok&=all==NK; free(fam); }
    printf("\nsummary: %s\n",ok?"key-free claims hold":"FAIL"); return ok?0:2;
}
