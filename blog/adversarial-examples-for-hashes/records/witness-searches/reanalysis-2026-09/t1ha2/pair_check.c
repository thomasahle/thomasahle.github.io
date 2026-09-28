/* pair_check.c -- the 96-byte t1ha2 pair: exact failing-seed interval.
 *
 * Links the upstream t1ha sources unmodified (see SOURCES.md).
 * Build: cc -O2 -std=c11 -I<t1ha> pair_check.c <t1ha>/src/t1ha2.c \
 *          <t1ha>/src/t1ha2_selfcheck.c <t1ha>/src/t1ha_selfcheck.c -lm -o pair_check
 * Run:   ./pair_check [log2 seeds = 24]
 *
 * The pair collides unless y = (w3 + c0) mod 2^64 < 2^56, where w3 is the
 * little-endian word at bytes 24..31 of M and c0 = rotr64(96, 23) + ~seed is the
 * loop state word c entering the first block.  y is a bijection of the seed, so
 * exactly 2^56 of the 2^64 seeds fail: the interval
 *   seed in [w3 + rotr64(96,23) - 2^56, w3 + rotr64(96,23) - 1]   (mod 2^64).
 * The program checks the four seeds at the two edges of that interval, then a
 * uniform sample, and compares every sampled outcome with the condition.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "t1ha.h"

int t1ha_selfcheck__t1ha2_atonce(void);
int t1ha_selfcheck__t1ha2_atonce128(void);

static const char *HM  = "4b2cc652222de32c6eb402b35cdd18cae775ec4962530f8652665a38df9912e3"
                         "1dc78f5ae19a0cc6f585de2f819640042e1f4a635bba50f7fc6de8bf179150d9"
                         "8d19298b36bf110cb6a7a887c622f90d33c44afcfeaeffe3cec888710f1c64b9";
static const char *HMp = "4b2cc652222de32d6eb402b37cdd18cae775ec4962530f8652665a38df9912e2"
                         "1dc78f5ae19a0cc6f585de2f819640042e1f4a635bba5077fc6de8bf17915059"
                         "8d19298b36bf110cb6a7a887c622f90d33c44afcfeaeff63cec888710f1c6439";

static void unhex(const char *h, uint8_t *o) { for (int i = 0; i < 96; i++) { unsigned v; sscanf(h + 2 * i, "%2x", &v); o[i] = (uint8_t)v; } }
static uint64_t rotr64(uint64_t v, unsigned s) { return (v >> s) | (v << (64 - s)); }
static uint64_t sm;
static uint64_t splitmix64(void) {
  uint64_t z = (sm += 0x9e3779b97f4a7c15ULL);
  z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL; z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
  return z ^ (z >> 31);
}

static uint8_t M[96], Mp[96];
static uint64_t w3;
static int fails_pred(uint64_t s) { uint64_t y = w3 + rotr64(96, 23) + ~s; return y < (1ULL << 56); }
static int col64(uint64_t s) { return t1ha2_atonce(M, 96, s) == t1ha2_atonce(Mp, 96, s); }
static int col128(uint64_t s) {
  uint64_t x1, x2, g1 = t1ha2_atonce128(&x1, M, 96, s), g2 = t1ha2_atonce128(&x2, Mp, 96, s);
  return g1 == g2 && x1 == x2;
}

int main(int argc, char **argv) {
  if (t1ha_selfcheck__t1ha2_atonce() || t1ha_selfcheck__t1ha2_atonce128()) { printf("upstream selfcheck FAILED\n"); return 1; }
  printf("upstream selfcheck t1ha2_atonce, t1ha2_atonce128: ok\n");
  unhex(HM, M); unhex(HMp, Mp); memcpy(&w3, M + 24, 8);
  uint64_t hi = w3 + rotr64(96, 23) - 1, lo = hi - ((1ULL << 56) - 1);
  printf("failing seeds: [%016llx, %016llx] (mod 2^64), 2^56 seeds\n", (unsigned long long)lo, (unsigned long long)hi);
  uint64_t edge[6] = { 0, 0x7d4799c23d231d40ULL, lo - 1, lo, hi, hi + 1 };
  const char *name[6] = { "ex. ", "ex. ", "lo-1", "lo  ", "hi  ", "hi+1" };
  int bad = 0;
  for (int e = 0; e < 6; e++) {
    uint64_t s = edge[e], x1, x2;
    uint64_t h1 = t1ha2_atonce(M, 96, s), h2 = t1ha2_atonce(Mp, 96, s);
    uint64_t g1 = t1ha2_atonce128(&x1, M, 96, s), g2 = t1ha2_atonce128(&x2, Mp, 96, s);
    printf("  %s seed %016llx: 64-bit %016llx %016llx  128-bit %016llx:%016llx %016llx:%016llx  %s\n", name[e],
           (unsigned long long)s, (unsigned long long)h1, (unsigned long long)h2, (unsigned long long)g1,
           (unsigned long long)x1, (unsigned long long)g2, (unsigned long long)x2, h1 == h2 ? "collide" : "differ");
    bad |= (h1 == h2) == fails_pred(s);
  }
  int lg = argc > 1 ? atoi(argv[1]) : 24; uint64_t N = 1ULL << lg, c = 0, c128 = 0, mis = 0; sm = 1;
  for (uint64_t t = 0; t < N; t++) {
    uint64_t s = splitmix64(); int a = col64(s), b = col128(s);
    c += a; c128 += b; mis += (a == fails_pred(s)) + (a != b);
  }
  printf("uniform seeds: 64-bit %llu/%llu = %.6f (1 - 2^-8 = %.6f); 128-bit %llu; condition mismatches %llu\n",
         (unsigned long long)c, (unsigned long long)N, (double)c / N, 1 - 1.0 / 256, (unsigned long long)c128,
         (unsigned long long)mis);
  printf("score log2(L/eps), L = 12, eps = 1 - 2^-8: %.4f bits\n", log2(12.0 / (1 - 1.0 / 256)));
  return bad || mis;
}
