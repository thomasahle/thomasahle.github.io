/* Is the long-path lane-pair toggle available for the DEFAULT secret (unseeded XXH3)?
 *
 * A toggle for lanes (2t, 2t+1) puts one word D at (lane 2t, stripe j-2t) and
 * (lane 2t+1, stripe j-2t-1), which both read secret word K = K[j], and complements
 * both.  Each of acc[2t], acc[2t+1] changes by
 *     dg + dD,  dg = (M32-l)(M32-h) - l*h = M32^2 - M32*(l+h),  dD = ~D - D = -1 - 2D,
 * with l = lo32(D^K), h = hi32(D^K), M32 = 2^32-1.  The toggle is invisible iff
 *     2D = M32^2 - M32*s - 1  (mod 2^64),  s = l + h in [0, 2^33-2].
 * The right side is even iff s is even; each even s gives two D (bit 63 free); keep
 * those whose own l + h equals s.  2^33 candidates per secret word.
 *
 * usage: default_secret_toggle <j> [K in hex]   (default K = LE64(kSecret + 8j))
 * Build: cc -O2 -o default_secret_toggle default_secret_toggle.c  (xxhash.h v0.8.3 beside it)
 */
#define XXH_INLINE_ALL
#include "xxhash.h"
#include <stdio.h>
#include <stdlib.h>
int main(int argc,char**argv){
  if(argc<2){ fprintf(stderr,"usage: %s <j in 1..21> [K hex]\n",argv[0]); return 2; }
  int j=atoi(argv[1]); if(argc<3 && (j<0||j>23)){ fprintf(stderr,"j out of range\n"); return 2; }
  uint64_t K = argc>2 ? strtoull(argv[2],0,16) : XXH_readLE64(XXH3_kSecret+8*j);
  const uint64_t M=0xffffffffULL; uint64_t found=0;
  for(uint64_t s=0;s<=0x1fffffffeULL;s+=2){ uint64_t D0=(M*M - M*s - 1)>>1;
    for(int t=0;t<2;t++){ uint64_t D=D0^((uint64_t)t<<63), x=D^K;
      if((uint64_t)(uint32_t)x+(x>>32)==s){ found++; if(found<=16) printf("j=%d D=%016llx\n",j,(unsigned long long)D); } } }
  printf("j=%d K=%016llx solutions=%llu\n",j,(unsigned long long)K,(unsigned long long)found); return 0; }
