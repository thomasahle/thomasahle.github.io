/* komihash 5.34: grid (both lane pairs tied, shared multiplier, low 8 bits of word0 and word2 varied)
 * times the 16-byte tail swap, 2^17 messages of 80 bytes. Includes upstream komihash.h unmodified.
 * Usage: ./komihash_grid_swap <log2 seeds> <rng seed> */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "komihash.h"
static uint64_t sm_state, xs[4];
static uint64_t splitmix64(void) { uint64_t z = (sm_state += 0x9e3779b97f4a7c15ULL); z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL; z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL; return z ^ (z >> 31); }
static uint64_t rotl(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
static uint64_t xoshiro(void) { uint64_t r = rotl(xs[1] * 5, 7) * 9, t = xs[1] << 17; xs[2] ^= xs[0]; xs[3] ^= xs[1]; xs[1] ^= xs[2]; xs[0] ^= xs[3]; xs[2] ^= t; xs[3] = rotl(xs[3], 45); return r; }
static void put64(uint8_t *p, uint64_t v) { memcpy(p, &v, 8); }
static int cmp(const void *a, const void *b) { uint64_t x = *(const uint64_t *)a, y = *(const uint64_t *)b; return x < y ? -1 : x > y; }
#define IV2 0x13198A2E03707344ULL
#define IV3 0xA4093822299F31D0ULL
#define IV4 0x082EFA98EC4E6C89ULL
#define IV6 0xBE5466CF34E90C6CULL
#define IV7 0xC0AC29B7C97C50DDULL
#define IV8 0x3F84D5B5B5470917ULL
int main(int argc, char **argv) {
  uint8_t key[256], table[2048];
  for (int i = 0; i < 256; i++) { key[i] = (uint8_t)i; uint64_t h = komihash(key, (size_t)i, (uint64_t)(256 - i)); memcpy(table + 8 * i, &h, 8); }
  if ((uint32_t)komihash(table, 2048, 0) != 0x8157FF6Du) { fprintf(stderr, "verification mismatch\n"); return 1; }
  int lg = atoi(argv[1]); sm_state = strtoull(argv[2], 0, 0); for (int i = 0; i < 4; i++) xs[i] = splitmix64();
  uint64_t w0 = xoshiro() & ~0xFFULL, w2 = xoshiro() & ~0xFFULL, w4 = xoshiro(), m8 = xoshiro(), m9 = xoshiro();
  const int K = 1 << 17;
  uint8_t (*msg)[80] = malloc((size_t)K * 80);
  for (int idx = 0; idx < K; idx++) {
    uint64_t a = w0 | (idx & 0xFF), c = w2 | ((idx >> 8) & 0xFF); int sw = idx >> 16;
    uint8_t *b = msg[idx];
    put64(b, a); put64(b + 8, a ^ IV2); put64(b + 16, c); put64(b + 24, c ^ IV3 ^ IV4);
    put64(b + 32, w4); put64(b + 40, w4 ^ IV6); put64(b + 48, w4 ^ IV7); put64(b + 56, w4 ^ IV8);
    put64(b + 64, sw ? m9 : m8); put64(b + 72, sw ? m8 : m9);
  }
  uint64_t n = 1ULL << lg, all17 = 0, all16 = 0, swapok = 0, hist[20] = {0};
  uint64_t *h = malloc((size_t)K * 8);
  for (uint64_t t = 0; t < n; t++) {
    uint64_t s = xoshiro();
    for (int i = 0; i < K; i++) h[i] = komihash(msg[i], 80, s);
    int so = 1; for (int i = 0; i < K / 2; i++) so &= h[i] == h[i + K / 2];
    swapok += so;
    int g = 1; for (int i = 1; i < K / 2; i++) g &= h[i] == h[0];
    all16 += g; all17 += g && so;
    qsort(h, K, 8, cmp);
    uint64_t best = 1, run = 1; for (int i = 1; i < K; i++) { run = h[i] == h[i - 1] ? run + 1 : 1; if (run > best) best = run; }
    int lb = 0; while ((1ULL << (lb + 1)) <= best) lb++; hist[lb]++;
  }
  printf("seeds %llu: swap half equals unswapped half for every member: %llu; grid 2^16 all equal: %llu; all 2^17 equal: %llu = 2^%.2f\n",
         (unsigned long long)n, (unsigned long long)swapok, (unsigned long long)all16, (unsigned long long)all17, log2((double)all17 / n));
  printf("largest class (floor log2):"); for (int i = 0; i < 20; i++) if (hist[i]) printf(" 2^%d:%llu", i, (unsigned long long)hist[i]); printf("\n");
  return 0;
}
