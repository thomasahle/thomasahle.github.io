/* rapid1 fairness pass: pair check against the UPSTREAM rapidhash.h (tag rapidhash_v1.0), not the transcription.
 * usage: ./upstream_check <log2 N> <rng seed>
 * 1. reference vector "message digest", len 14, seed 3 (harness selftest literal)
 * 2. recorded pair A at the recorded seed
 * 3. N uniformly random 64-bit seeds (xoshiro256**, own stream): collision count of pair A
 * 4. key-free check: 2^16 random (seed, 32-byte M, M') with w2 = rapid_secret[1] (issue #10 / #25 pattern), also 17- and 24-byte
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>
#include "rapidhash.h"
static uint64_t st[4];
static uint64_t rotl(uint64_t x,int k){return (x<<k)|(x>>(64-k));}
static uint64_t sm(uint64_t*s){uint64_t z=(*s+=0x9e3779b97f4a7c15ull);z=(z^(z>>30))*0xbf58476d1ce4e5b9ull;z=(z^(z>>27))*0x94d049bb133111ebull;return z^(z>>31);}
static void rng_init(uint64_t s){for(int i=0;i<4;i++)st[i]=sm(&s);}
static uint64_t next(void){uint64_t r=rotl(st[1]*5,7)*9,t=st[1]<<17;st[2]^=st[0];st[3]^=st[1];st[1]^=st[2];st[0]^=st[3];st[2]^=t;st[3]=rotl(st[3],45);return r;}
static void hexdec(const char*s,uint8_t*o){for(size_t i=0;i<strlen(s)/2;i++){unsigned x;sscanf(s+2*i,"%2x",&x);o[i]=x;}}
int main(int argc,char**argv){
  unsigned lg=argc>1?atoi(argv[1]):20; uint64_t rs=argc>2?strtoull(argv[2],0,0):1;
  uint64_t v=rapidhash_withSeed("message digest",14,3);
  printf("ref vector: %016" PRIx64 " expected 0031cdc21324150f %s\n",v,v==0x0031cdc21324150full?"PASS":"FAIL");
  uint8_t A[32],B[32];
  hexdec("9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c",A);
  hexdec("642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c",B);
  uint64_t ha=rapidhash_withSeed(A,32,0x3788f2419a81e2d6ull), hb=rapidhash_withSeed(B,32,0x3788f2419a81e2d6ull);
  printf("recorded seed 3788f2419a81e2d6: H(A)=%016" PRIx64 " H(B)=%016" PRIx64 " expected 8f7a71ffebd4a14b %s\n",ha,hb,(ha==hb&&ha==0x8f7a71ffebd4a14bull)?"PASS":"FAIL");
  /* the 12 recorded colliding seeds */
  const uint64_t rec[12]={0x3788f2419a81e2d6ull,0x81dd9e7f3f48d8edull,0x95460f49e8f81b2bull,0xc430bf5d8e43fb94ull,0xc0111c34a0e81e29ull,0xf1145666200f14c9ull,0x622301fab58dc671ull,0xb65246f727df0f01ull,0x0351a2c051a1d050ull,0xfafd76ed2974427dull,0x77f23d6bcf87c93aull,0xa68df58c0ad5aad9ull};
  int ok=0; for(int i=0;i<12;i++) ok+= rapidhash_withSeed(A,32,rec[i])==rapidhash_withSeed(B,32,rec[i]);
  printf("recorded colliding seeds reproduced: %d/12\n",ok);
  uint64_t n=1ull<<lg, c=0; rng_init(rs);
  for(uint64_t t=0;t<n;t++){uint64_t s=next(); c+= rapidhash_withSeed(A,32,s)==rapidhash_withSeed(B,32,s);}
  printf("pair A sample: rng seed %" PRIu64 ": collisions = %" PRIu64 " / 2^%u", rs,c,lg);
  if(c) printf("; log2 rate = %.4f; cap = %.4f bits", log2((double)c/n), 2-log2((double)c/n));
  puts("");
  /* key-free annihilation: w at offset len-16 equals secret[1] */
  size_t lens[3]={32,24,17};
  for(int li=0;li<3;li++){ size_t L=lens[li]; uint64_t hits=0, tot=1ull<<16; uint8_t M[64],N[64];
    for(uint64_t t=0;t<tot;t++){ for(size_t i=0;i<L;i+=8){uint64_t x=next(),y=next();memcpy(M+i,&x,(L-i<8)?L-i:8);memcpy(N+i,&y,(L-i<8)?L-i:8);}
      uint64_t s1=rapid_secret[1]; memcpy(M+L-16,&s1,8); memcpy(N+L-16,&s1,8); uint64_t sd=next();
      hits+= rapidhash_withSeed(M,L,sd)==rapidhash_withSeed(N,L,sd); }
    printf("key-free w[len-16]=secret[1], len %zu (L=%zu): %" PRIu64 " / %" PRIu64 " random (seed,M,M') collide\n",L,(L+7)/8,hits,tot);
  }
  return 0;
}
