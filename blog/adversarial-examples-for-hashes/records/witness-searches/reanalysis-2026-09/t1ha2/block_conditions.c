/* block_conditions.c -- per-block failure rates of the t1ha2 family conditions.
 *
 * Uses the base message of t1ha2_trail_check.c (same RNG, same construction) and
 * measures, for each switchable block k, how often (w3_k + c_k) mod 2^64 < 2^56
 * over uniform seeds (c_k = loop state word c entering block k).  Block 0 fails
 * with probability exactly 2^-8 (c_0 = rotr(len, 23) + ~seed).  Block 1 does not:
 * c_1 = c_0 ^ (seed + rotr(w0_0, 57)) is a fixed function of the seed with a
 * strongly non-uniform top byte, so its failure rate depends on the base words
 * (0 for some bases, about 10% for others).  From block 2 on the state has passed
 * through the products and the rate is 2^-8 again.
 * Build: cc -O2 -std=c11 block_conditions.c -o block_conditions
 * Run:   ./block_conditions <blocks n> <rng seed>     e.g. 18 404, 18 606, 22 505
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static uint64_t sm_state, xs[4];
static uint64_t splitmix64(void) {
  uint64_t z = (sm_state += 0x9e3779b97f4a7c15ULL);
  z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL; z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
  return z ^ (z >> 31);
}
static uint64_t rotl(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
static uint64_t xoshiro(void) {
  uint64_t r = rotl(xs[1] * 5, 7) * 9, t = xs[1] << 17;
  xs[2] ^= xs[0]; xs[3] ^= xs[1]; xs[1] ^= xs[2]; xs[0] ^= xs[3]; xs[2] ^= t; xs[3] = rotl(xs[3], 45);
  return r;
}
static uint64_t r64(uint64_t v, unsigned s) { return (v >> s) | (v << (64 - s)); }

int main(int argc, char **argv) {
  if (argc < 3) { fprintf(stderr, "usage: %s <blocks n <= 32> <rng seed>\n", argv[0]); return 2; }
  int n = atoi(argv[1]);
  if (n < 3 || n > 32) return 2;
  sm_state = strtoull(argv[2], 0, 0);
  for (int i = 0; i < 4; i++) xs[i] = splitmix64();
  uint64_t base[4 * 32];
  for (int i = 0; i < 4 * n; i++) base[i] = xoshiro();
  for (int i = 0; i < n - 2; i++) { base[4 * i] &= ~(1ULL << 56); base[4 * i + 1] &= ~(1ULL << 37); }
  const uint64_t p5 = 0xC060724A8424F345ULL, p6 = 0xCB5AF53AE3AAAC31ULL, len = 32 * (uint64_t)n;
  const long N = 1L << 22;
  long fail[32] = {0}, allok = 0;
  for (long t = 0; t < N; t++) {
    uint64_t s = xoshiro(), a = s, b = len, c = r64(len, 23) + ~s, d = ~len + r64(s, 19);
    int ok = 1;
    for (int i = 0; i < n; i++) {
      const uint64_t *w = base + 4 * i;
      if (i < n - 2 && w[3] + c < (1ULL << 56)) { fail[i]++; ok = 0; }
      uint64_t d02 = w[0] + r64(w[2] + d, 56), c13 = w[1] + r64(w[3] + c, 19);
      d ^= b + r64(w[1], 38); c ^= a + r64(w[0], 57);
      b ^= p6 * (c13 + w[2]); a ^= p5 * (d02 + w[3]);
    }
    allok += ok;
  }
  printf("n=%d rng %s, 2^22 seeds: all conditions hold %.4f; per-block failure rate x 256:", n, argv[2], (double)allok / N);
  for (int i = 0; i < n - 2; i++) printf(" %.2f", 256.0 * fail[i] / N);
  printf("\n");
  return 0;
}
