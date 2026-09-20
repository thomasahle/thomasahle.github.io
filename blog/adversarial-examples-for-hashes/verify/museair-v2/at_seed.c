/* at_seed.c: print all four variants for M and M2 at an explicit seed (128-bit: seed_a = seed, seed_b = seed_b arg).
 * ./at_seed <mhex> <m2hex> <seed> [seed_b] */
#include <stdio.h>
#include <stdlib.h>
#include "museair2.h"
static int hexv(char c){ return c<='9'? c-'0' : (c|32)-'a'+10; }
static void p128(ma_u128 x){ printf("%016llx%016llx", (unsigned long long)(x>>64), (unsigned long long)x); }
int main(int argc, char **argv){
    if (argc < 4) return 2; size_t len = strlen(argv[1])/2; uint8_t M[64], M2[64];
    for (size_t i = 0; i < len; i++) { M[i] = hexv(argv[1][2*i])*16+hexv(argv[1][2*i+1]); M2[i] = hexv(argv[2][2*i])*16+hexv(argv[2][2*i+1]); }
    uint64_t s = strtoull(argv[3], 0, 0), sb = argc > 4 ? strtoull(argv[4], 0, 0) : 0;
    printf("len %zu seed 0x%016llx (seed_b 0x%016llx for the 128-bit variants)\n", len, (unsigned long long)s, (unsigned long long)sb);
    uint64_t a = museair2_hash(M, len, s, 0), b = museair2_hash(M2, len, s, 0); printf("  hash            %016llx %016llx %s\n", (unsigned long long)a, (unsigned long long)b, a==b?"EQUAL":"differ");
    a = museair2_hash(M, len, s, 1); b = museair2_hash(M2, len, s, 1); printf("  bfast::hash     %016llx %016llx %s\n", (unsigned long long)a, (unsigned long long)b, a==b?"EQUAL":"differ");
    ma_u128 x = museair2_hash128(M, len, s, sb, 0), y = museair2_hash128(M2, len, s, sb, 0); printf("  hash128         "); p128(x); printf(" "); p128(y); printf(" %s\n", x==y?"EQUAL":"differ");
    x = museair2_hash128(M, len, s, sb, 1); y = museair2_hash128(M2, len, s, sb, 1); printf("  bfast::hash128  "); p128(x); printf(" "); p128(y); printf(" %s\n", x==y?"EQUAL":"differ");
    return 0;
}
