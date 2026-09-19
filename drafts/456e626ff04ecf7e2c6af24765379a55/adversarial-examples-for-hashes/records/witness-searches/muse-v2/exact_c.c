/* exact_c.c: does the collision indicator of the len-19 pair depend only on seed mod 2^k?
 * For 2^22 random seeds s: compare coll(s) with coll(s ^ h) where h is random with zero low k bits.
 * Prints disagreements for k = 16, 20, 24, 28, 32 and the collision count. */
#include <stdio.h>
#include <stdlib.h>
#include "museair2.h"
static int hexv(char c){ return c<='9'? c-'0' : (c|32)-'a'+10; }
int main(int argc, char **argv){
    const char *a = argc > 1 ? argv[1] : "00000000000000000000000000000000000000", *b = argc > 2 ? argv[2] : "48898050201582401100000000000000000015";
    size_t len = strlen(a)/2; uint8_t M[64], M2[64];
    for (size_t i = 0; i < len; i++) { M[i] = hexv(a[2*i])*16+hexv(a[2*i+1]); M2[i] = hexv(b[2*i])*16+hexv(b[2*i+1]); }
    int ks[5] = {16, 20, 24, 28, 32}; uint64_t dis[5] = {0}, coll = 0, N = 1ull << 22, st = 0x777;
    for (uint64_t t = 0; t < N; t++) {
        uint64_t s = ma_splitmix(&st), h = ma_splitmix(&st);
        int c0 = museair2_hash(M, len, s, 0) == museair2_hash(M2, len, s, 0); coll += c0;
        for (int i = 0; i < 5; i++) { uint64_t s2 = s ^ (h & ~((1ull << ks[i]) - 1)); int c1 = museair2_hash(M, len, s2, 0) == museair2_hash(M2, len, s2, 0); dis[i] += c0 != c1; }
    }
    printf("len %zu: collisions %llu / 2^22\n", len, (unsigned long long)coll);
    for (int i = 0; i < 5; i++) printf("  flip random bits >= %d: indicator disagreements %llu\n", ks[i], (unsigned long long)dis[i]);
    /* if it depends only on low bits, enumerate the class exactly */
    return 0;
}
