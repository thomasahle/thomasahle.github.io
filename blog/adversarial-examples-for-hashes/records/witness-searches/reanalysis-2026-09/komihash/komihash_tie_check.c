/* Independent check of komihash 5.34 lane-tie constructions.
 * Includes the upstream komihash.h unmodified (build with -I<komihash repo>).
 * Usage: ./komihash_tie_check <log2 seeds> <rng seed>
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "komihash.h"

static uint64_t sm_state;
static uint64_t splitmix64(void) {
  uint64_t z = (sm_state += 0x9e3779b97f4a7c15ULL);
  z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
  z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
  return z ^ (z >> 31);
}
static uint64_t xs[4];
static uint64_t rotl(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
static uint64_t xoshiro(void) {
  uint64_t r = rotl(xs[1] * 5, 7) * 9, t = xs[1] << 17;
  xs[2] ^= xs[0]; xs[3] ^= xs[1]; xs[1] ^= xs[2]; xs[0] ^= xs[3];
  xs[2] ^= t; xs[3] = rotl(xs[3], 45);
  return r;
}
static void rng_init(uint64_t s) { sm_state = s; for (int i = 0; i < 4; i++) xs[i] = splitmix64(); }

static int check_vectors(void) {
  static const uint64_t bulk0[][2] = {{3, 0x7a9717e9eea4be8bULL}, {8, 0x00b4313a24431306ULL}, {31, 0xc77e02ed4b201b9aULL},
    {64, 0x90b07e2158f88cc0ULL}, {72, 0x24c9621701603741ULL}, {80, 0x1d4c1d97ca684334ULL}, {112, 0xd1a425d530652287ULL},
    {132, 0x72623be342c20ab5ULL}, {256, 0x94c3dbdca59ddf57ULL}};
  uint8_t bulk[256];
  for (int i = 0; i < 256; i++) bulk[i] = (uint8_t)i;
  int bad = 0;
  for (size_t i = 0; i < sizeof bulk0 / sizeof bulk0[0]; i++)
    bad |= komihash(bulk, (size_t)bulk0[i][0], 0) != bulk0[i][1];
  bad |= komihash("This is a 32-byte testing string", 32, 0) != 0x05ad960802903a9dULL;
  bad |= komihash("The cat is out of the bag", 25, 0x0123456789abcdefULL) != 0x5b1da0b43545d196ULL;
  /* SMHasher3 verification procedure */
  uint8_t key[256], table[256 * 8];
  for (int i = 0; i < 256; i++) {
    key[i] = (uint8_t)i;
    uint64_t h = komihash(key, (size_t)i, (uint64_t)(256 - i));
    memcpy(table + 8 * i, &h, 8);
  }
  uint32_t v = (uint32_t)komihash(table, sizeof table, 0);
  printf("README vectors %s; SMHasher3 verification 0x%08X (expected 0x8157FF6D)\n", bad ? "FAIL" : "ok", v);
  return bad || v != 0x8157FF6Du;
}

static void put64(uint8_t *p, uint64_t v) { memcpy(p, &v, 8); }
static void hex(const char *n, const uint8_t *m, size_t l) { printf("%s", n); for (size_t i = 0; i < l; i++) printf("%02x", m[i]); printf("\n"); }

/* 64-byte block with lane1 == lane2 and lane3 == lane4 for every seed */
static void tied_block(uint8_t *b, uint64_t m0, uint64_t m4, uint64_t m2, uint64_t m6) {
  put64(b + 0, m0); put64(b + 8, m0 ^ 0x13198A2E03707344ULL);                 /* m1 = m0 ^ IVAL2 */
  put64(b + 16, m2); put64(b + 24, m2 ^ 0xA4093822299F31D0ULL ^ 0x082EFA98EC4E6C89ULL); /* m3 = m2 ^ IVAL3 ^ IVAL4 */
  put64(b + 32, m4); put64(b + 40, m4 ^ 0xBE5466CF34E90C6CULL);               /* m5 = m4 ^ IVAL6 */
  put64(b + 48, m6); put64(b + 56, m6 ^ 0xC0AC29B7C97C50DDULL ^ 0x3F84D5B5B5470917ULL); /* m7 = m6 ^ IVAL7 ^ IVAL8 */
}

int main(int argc, char **argv) {
  if (check_vectors()) return 1;
  int lg = argc > 1 ? atoi(argv[1]) : 20;
  rng_init(argc > 2 ? strtoull(argv[2], 0, 0) : 1);
  uint64_t n = 1ULL << lg;

  /* (a) the claimed 73-byte swap pair: tied base + tail [y,01,00x6,z] vs [z,01,00x6,y] */
  uint8_t A[73], B[73];
  tied_block(A, xoshiro(), xoshiro(), xoshiro(), xoshiro());
  uint8_t y = 0x3c, z = 0xa7;
  memset(A + 64, 0, 9); A[64] = y; A[65] = 1; A[72] = z;
  memcpy(B, A, 73); B[64] = z; B[72] = y;
  hex("M  = ", A, 73); hex("M' = ", B, 73);
  uint64_t c = 0;
  for (uint64_t t = 0; t < n; t++) { uint64_t s = xoshiro(); c += komihash(A, 73, s) == komihash(B, 73, s); }
  printf("73-byte swap pair: %llu / %llu seeds collide; seed 0 -> %016llx %016llx\n", (unsigned long long)c,
         (unsigned long long)n, (unsigned long long)komihash(A, 73, 0), (unsigned long long)komihash(B, 73, 0));

  /* (b) adversarial probe: two DIFFERENT tied first blocks, same tail. If the tie zeroed the state,
         these would collide key-free. */
  uint8_t C1[73], C2[73];
  tied_block(C1, xoshiro(), xoshiro(), xoshiro(), xoshiro());
  tied_block(C2, xoshiro(), xoshiro(), xoshiro(), xoshiro());
  memcpy(C1 + 64, A + 64, 9); memcpy(C2 + 64, A + 64, 9);
  c = 0;
  for (uint64_t t = 0; t < (1ULL << 16); t++) { uint64_t s = xoshiro(); c += komihash(C1, 73, s) == komihash(C2, 73, s); }
  printf("two different tied blocks, same tail: %llu / 65536 collide\n", (unsigned long long)c);
  return 0;
}
