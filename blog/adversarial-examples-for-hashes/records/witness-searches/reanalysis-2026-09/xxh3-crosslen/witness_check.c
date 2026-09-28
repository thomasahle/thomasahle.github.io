/* Deterministic check of the logged XXH3 v0.8.3 witnesses (no search, no randomness).
 *   1. XXH3-64 cross-length pair (5 B vs 8 B) under an explicit 192-byte secret.
 *   2. XXH3-128 cross-length pair (9 B vs 16 B) under an explicit 192-byte secret.
 *   3. The same two pairs under an explicit withSeed seed.
 *   4. The 2^16-member long-input set (3648 B) under an explicit class secret: every
 *      member has the same XXH3-64 and XXH3-128 output; the secret satisfies
 *      (lo32(K[7]) ^ 0x7fffffff) + hi32(K[7]) = 0xfffffffe.
 *   5. The same set through XXH3_*_withSeed with seed = kSecret[7] - K (K[7] of the derived
 *      secret is kSecret[7] - seed), and through XXH3_*_withSecretandSeed (len > 240 uses the
 *      secret only).
 * Build: cc -O2 -o witness_check witness_check.c   (xxhash.h v0.8.3 beside it, see SOURCES.md)
 */
#define XXH_INLINE_ALL
#include "xxhash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void unhex(const char *h, uint8_t *o, size_t n){ for(size_t i=0;i<n;i++){ unsigned x; sscanf(h+2*i,"%2x",&x); o[i]=(uint8_t)x; } }
static int fails=0;
static void ok(int c,const char *what){ printf("%s %s\n",c?"PASS":"FAIL",what); if(!c) fails++; }

static const char *SEC64 ="083975da0b858e44ded126719bdbb4b04138fac5657195e1d6ba9d970bc3973eb4be66f9c271c6378d7c1c7989a419eeff8156e6935d52d7b4066e81a207b719c2bdb60695a029e2b1f9ee58663d4d926dc891a744136596453d4f4c8cba38236735556a74b5b230ef6b11b6dc07dba024b6eaac9405622a692093e940f7b7f2990b3e5fb8b8a4bbe95bde730d82e724f8e186ec37ba6409d57a73acf00e29d1ae8cb1294fd3fe51fe138225bad81d20af5bc92ff1b71fe145c8c9badf888361";
static const char *SEC128="dae7287b9891ff498ff032063746ae83d16a86a48aa99b1ede3d5c81f35805607220ed05603051b57c696dd19498b392a8316a1e611fe4545b73796e900610e8d3a39b9d1083ec70531adc206c01265a3af822d87a6d3cf4199d738a393fa11c6ebd219b9c8675434b8a50cbb3e51006b731057c36e2b81798eac463b3268953b1e395f206d3873e2f74fc75ea6cec595924ac103016991b3a0b769d232a764ebf174a335900069349756fbfb421b6496cdefb8c314795f7bd5927a027b6090c";
static const char *SECMC ="4a74bed0463010c855c2dc0b688f05eace58aa0bbf246c536c17b7c2f664260aca2b4bdef2940033410a169f771719fa0617401460a987596485818b6385810b68eed8f1ee5f42ddb2404faae88bccc51b03fc683dda077754cbf10ea69a34c2b893f217d9771c37877726d513f5384e70a2b77dab6df4bc4a6156045a4b8f6cacd398b771bdd1af0c582a785a742c1e41cd07c57f0f68b24189308bb18afef120b704ff7c43b24a6f7d21a1f352502816b46f0d32089119b516b1b95e497144";

int main(void){
  uint8_t sec[192], m5[5]={0}, m8[8], m9[9]={0}, m16[16];
  ok(XXH_versionNumber()==803 && XXH3_64bits("",0)==0x2d06800538d394c2ULL, "xxHash v0.8.3, XXH3_64bits(\"\") = 2d06800538d394c2");
  unhex("0fe3fba2f2528c1d",m8,8);
  unhex("00292432025470850029243202543025",m16,16);

  unhex(SEC64,sec,192);
  { uint64_t a=XXH3_64bits_withSecret(m5,5,sec,192), b=XXH3_64bits_withSecret(m8,8,sec,192);
    printf("  XXH3-64 withSecret: H(00x5) = %016llx  H(0fe3fba2f2528c1d) = %016llx\n",(unsigned long long)a,(unsigned long long)b);
    ok(a==b && a==0x79a5d45b5d6567cdULL,"XXH3-64 5 B vs 8 B collide under the logged secret"); }
  unhex(SEC128,sec,192);
  { XXH128_hash_t a=XXH3_128bits_withSecret(m9,9,sec,192), b=XXH3_128bits_withSecret(m16,16,sec,192);
    printf("  XXH3-128 withSecret: H(00x9) = %016llx%016llx  H(m') = %016llx%016llx (high64 low64)\n",
      (unsigned long long)a.high64,(unsigned long long)a.low64,(unsigned long long)b.high64,(unsigned long long)b.low64);
    ok(XXH128_isEqual(a,b) && a.high64==0xedc8fa6d9f82abe8ULL && a.low64==0x5cff7dddfa69543dULL,"XXH3-128 9 B vs 16 B collide under the logged secret"); }
  { uint64_t s=0xc656a656224499e4ULL; uint64_t a=XXH3_64bits_withSeed(m5,5,s), b=XXH3_64bits_withSeed(m8,8,s);
    ok(a==b && a==0x042637874fec2910ULL,"XXH3-64 withSeed 0xc656a656224499e4 collides (042637874fec2910)"); }
  { uint64_t s=0x223bbf0662aa12b5ULL; XXH128_hash_t a=XXH3_128bits_withSeed(m9,9,s), b=XXH3_128bits_withSeed(m16,16,s);
    ok(XXH128_isEqual(a,b) && a.high64==0xef2a458a93a83ca6ULL && a.low64==0xf65ab3c2989a22f9ULL,"XXH3-128 withSeed 0x223bbf0662aa12b5 collides (ef2a458a93a83ca6f65ab3c2989a22f9)"); }

  /* long-input set: 3 full 1024-byte blocks + 576-byte tail = 3648 B; member c (16 bits):
     for block b in 0..3 and toggle t in 0..3, bit 4b+t selects D or ~D, D = 0x000000007fffffff,
     at byte offsets 1024b + 64(7-2t) + 8(2t) and 1024b + 64(6-2t) + 8(2t+1) (LE64); all else 0. */
  unhex(SECMC,sec,192);
  { uint64_t K=XXH_readLE64(sec+56); uint32_t l=(uint32_t)K^0x7fffffffu, h=(uint32_t)(K>>32);
    printf("  class secret K[7] = %016llx\n",(unsigned long long)K);
    ok((uint64_t)l+h==0xfffffffeu,"class condition (lo32(K[7]) ^ 0x7fffffff) + hi32(K[7]) = 0xfffffffe"); }
  { enum { LEN=3648 }; static uint8_t m[LEN]; const uint64_t D=0x7fffffffULL; uint64_t r64=0; XXH128_hash_t r128={0,0}; int all=1;
    for(uint32_t c=0;c<65536;c++){ memset(m,0,LEN);
      for(int b=0;b<4;b++) for(int t=0;t<4;t++){ uint64_t w=((c>>(4*b+t))&1)?~D:D;
        XXH_writeLE64(m+1024*b+64*(7-2*t)+8*(2*t),w); XXH_writeLE64(m+1024*b+64*(6-2*t)+8*(2*t+1),w); }
      uint64_t a=XXH3_64bits_withSecret(m,LEN,sec,192); XXH128_hash_t q=XXH3_128bits_withSecret(m,LEN,sec,192);
      if(c==0){ r64=a; r128=q; } else if(a!=r64 || !XXH128_isEqual(q,r128)) { all=0; break; } }
    printf("  all members: XXH3-64 %016llx  XXH3-128 %016llx%016llx\n",(unsigned long long)r64,(unsigned long long)r128.high64,(unsigned long long)r128.low64);
    ok(all && r64==0xb3c0a2ccc5a6d117ULL && r128.high64==0x3131016698da6968ULL && r128.low64==0xb3c0a2ccc5a6d117ULL,
       "all 65536 members of the 3648-byte set share one XXH3-64 and one XXH3-128 output");
    { uint64_t K=XXH_readLE64(sec+56), seed=XXH_readLE64(XXH3_kSecret+56)-K; uint64_t s64=0; XXH128_hash_t s128={0,0}; int allS=1, allSS=1;
      for(uint32_t c=0;c<65536;c++){ memset(m,0,LEN);
        for(int b=0;b<4;b++) for(int t=0;t<4;t++){ uint64_t w=((c>>(4*b+t))&1)?~D:D;
          XXH_writeLE64(m+1024*b+64*(7-2*t)+8*(2*t),w); XXH_writeLE64(m+1024*b+64*(6-2*t)+8*(2*t+1),w); }
        uint64_t a=XXH3_64bits_withSeed(m,LEN,seed); XXH128_hash_t q=XXH3_128bits_withSeed(m,LEN,seed);
        if(c==0){ s64=a; s128=q; } else if(a!=s64 || !XXH128_isEqual(q,s128)) allS=0;
        if(XXH3_64bits_withSecretandSeed(m,LEN,sec,192,12345)!=r64 || !XXH128_isEqual(XXH3_128bits_withSecretandSeed(m,LEN,sec,192,12345),r128)) allSS=0; }
      printf("  withSeed seed = %016llx: all members XXH3-64 %016llx  XXH3-128 %016llx%016llx\n",(unsigned long long)seed,(unsigned long long)s64,(unsigned long long)s128.high64,(unsigned long long)s128.low64);
      ok(allS,"withSeed: all 65536 members collide (64 and 128) for seed = kSecret[7] - K");
      ok(allSS,"withSecretandSeed (seed 12345): all 65536 members give the withSecret outputs"); }
    /* control: a uniform-looking secret word (flip one bit of K[7]) breaks the class */
    sec[56]^=1; uint64_t a0,a1; memset(m,0,LEN);
    for(int b=0;b<4;b++) for(int t=0;t<4;t++){ XXH_writeLE64(m+1024*b+64*(7-2*t)+8*(2*t),D); XXH_writeLE64(m+1024*b+64*(6-2*t)+8*(2*t+1),D); }
    a0=XXH3_64bits_withSecret(m,LEN,sec,192); XXH_writeLE64(m+64*7,~D); XXH_writeLE64(m+64*6+8,~D); a1=XXH3_64bits_withSecret(m,LEN,sec,192);
    ok(a0!=a1,"control: with bit 0 of K[7] flipped, one toggle changes the output"); }
  printf("%s\n",fails?"SOME CHECKS FAILED":"ALL CHECKS PASSED"); return fails!=0;
}
