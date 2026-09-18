/* Shared helpers: the pair, the explicit packet-1 formulas, a PRNG.
   The reference implementation is #included unmodified (static internals visible). */
#include "c/highwayhash.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#define I00 0xdbe6d5d5fe4cce2full
#define I01 0xa4093822299f31d0ull
#define I10 0x3bd39e10cb0ef593ull
#define I11 0xc0acf169b5f18a8cull
#define C2  0xb3000100ull            /* packet-2 lane-0 constant c2 */
#define TARGET_D0 0x4d000000u        /* 2^8 - c2 mod 2^32 */

static inline uint64_t rot32(uint64_t x) { return (x >> 32) | (x << 32); }
static inline uint64_t splitmix64(uint64_t *s) {
  uint64_t z = (*s += 0x9e3779b97f4a7c15ull);
  z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ull;
  z = (z ^ (z >> 27)) * 0x94d049bb133111ebull;
  return z ^ (z >> 31);
}
static inline void store64(uint8_t *p, uint64_t v) { for (int i = 0; i < 8; i++) p[i] = (uint8_t)(v >> (8 * i)); }

/* The 96-byte pair (three packets, all differences in lane 0). */
static void make_pair(uint8_t m1[96], uint8_t m2[96]) {
  memset(m1, 0, 96); memset(m2, 0, 96);
  uint64_t p1 = 0 - I00, p2 = C2 - I00, p3 = 0;
  store64(m1 + 0, p1);  store64(m1 + 32, p2);  store64(m1 + 64, p3);
  store64(m2 + 0, p1 + (1ull << 8));
  store64(m2 + 32, p2 - ((1ull << 9) + (1ull << 24)));
  store64(m2 + 64, p3 + (1ull << 8));
}

/* Explicit zipper terms, copied verbatim from ZipperMergeAndAdd (v1 = first arg, v0 = second). */
static inline uint64_t zip0(uint64_t v1, uint64_t v0) {
  return (((v0 & 0xff000000ull) | (v1 & 0xff00000000ull)) >> 24) |
         (((v0 & 0xff0000000000ull) | (v1 & 0xff000000000000ull)) >> 16) |
         (v0 & 0xff0000ull) | ((v0 & 0xff00ull) << 32) |
         ((v1 & 0xff00000000000000ull) >> 8) | (v0 << 56);
}
static inline uint64_t zip1(uint64_t v1, uint64_t v0) {
  return (((v1 & 0xff000000ull) | (v0 & 0xff00000000ull)) >> 24) |
         (v1 & 0xff0000ull) | ((v1 & 0xff0000000000ull) >> 16) |
         ((v1 & 0xff00ull) << 24) | ((v0 & 0xff000000000000ull) >> 8) |
         ((v1 & 0xffull) << 48) | (v0 & 0xff00000000000000ull);
}

/* Lane-0/1 state after packet 1 of m1 under condition (1), as explicit functions of
   k = lo32(key[0]) and K1 = key[1].  Returns v0[0], v1[0], v0[1], v1[1], mul1[0], mul0[1], mul1[1]. */
typedef struct { uint64_t U0, T, U1, T1, M10, M01, M11, W1, W0, V0, v1_0; } P1;
static P1 formula_p1(uint32_t k, uint64_t K1) {
  P1 r;
  uint32_t a = 0xfe4cce2fu ^ k, b = 0x3bd39e10u ^ k;
  r.v1_0 = ((uint64_t)b << 32) | 0x10e82046ull;         /* v1[0] after the packet add (+0) */
  r.V0 = (uint64_t)a + I10;                               /* v0[0] after += mul1[0] */
  r.M10 = I10 ^ ((r.V0 & 0xffffffffull) * (r.v1_0 >> 32));
  r.W1 = (I11 ^ rot32(K1)) + I01;                         /* v1[1] after the packet add */
  uint64_t v0_1 = I01 ^ K1;
  r.M01 = I01 ^ ((r.W1 & 0xffffffffull) * (v0_1 >> 32));
  r.W0 = v0_1 + I11;                                      /* v0[1] after += mul1[1] */
  r.M11 = I11 ^ ((r.W0 & 0xffffffffull) * (r.W1 >> 32));
  r.U0 = r.V0 + zip0(r.W1, r.v1_0);
  r.U1 = r.W0 + zip1(r.W1, r.v1_0);
  r.T  = r.v1_0 + zip0(r.U1, r.U0);
  r.T1 = r.W1 + zip1(r.U1, r.U0);
  return r;
}
