/* Independent check of the t1ha2 top-bit loop trail.
 * Links the upstream t1ha v2.1.4 sources (src/t1ha2.c etc.) unmodified.
 * Build: cc -O2 -std=c11 -I<t1ha> t1ha2_trail_check.c <t1ha>/src/t1ha2.c
 *        <t1ha>/src/t1ha2_selfcheck.c <t1ha>/src/t1ha_selfcheck.c -o t1ha2_trail_check
 * Usage: ./t1ha2_trail_check pair  <log2 seeds> <rng seed>
 *        ./t1ha2_trail_check family <blocks n> <log2 seeds> <rng seed>
 * The message construction below is written from the mechanism description only.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "t1ha.h"

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

/* SMHasher3 verification value: hash i bytes {0..i-1} with seed 256-i, then hash the table with seed 0 */
static uint32_t smhasher_verif(void) {
  uint8_t key[256], table[256 * 8];
  for (int i = 0; i < 256; i++) {
    key[i] = (uint8_t)i;
    uint64_t h = t1ha2_atonce(key, (size_t)i, (uint64_t)(256 - i));
    memcpy(table + 8 * i, &h, 8);
  }
  uint64_t f = t1ha2_atonce(table, sizeof table, 0);
  return (uint32_t)f;
}

/* build the member of the family with switch mask `mask` (bit i -> block i switched) */
static void build(const uint64_t *base, int n, uint64_t mask, uint64_t *out) {
  int da = 0, db = 0, dc = 0, dd = 0;
  for (int i = 0; i < n; i++) {
    uint64_t w0 = base[4 * i], w1 = base[4 * i + 1], w2 = base[4 * i + 2], w3 = base[4 * i + 3];
    int f = (int)((mask >> i) & 1);
    if (dd) w2 ^= 1ULL << 63;
    if (dc) w3 ^= 1ULL << 63;
    if (f) { w0 ^= 1ULL << 56; w1 ^= 1ULL << 37; w3 -= 1ULL << 56; }
    out[4 * i] = w0; out[4 * i + 1] = w1; out[4 * i + 2] = w2; out[4 * i + 3] = w3;
    int na = da ^ dc, nb = db ^ dd, nc = dc ^ da ^ f, nd = dd ^ db ^ f;
    da = na; db = nb; dc = nc; dd = nd;
  }
  if (da | db | dc | dd) { fprintf(stderr, "trail does not close\n"); exit(2); }
}

/* model: recompute the loop state for the base (own transcription of T1HA2_UPDATE)
 * and report whether every switched block has (w3 + c) >= 2^56 */
static uint64_t r64(uint64_t v, unsigned s) { return (v >> s) | (v << (64 - s)); }
static int model_ok(const uint64_t *base, int n, uint64_t mask, uint64_t seed, size_t len) {
  const uint64_t p5 = UINT64_C(0xC060724A8424F345), p6 = UINT64_C(0xCB5AF53AE3AAAC31);
  /* init_ab / init_cd from t1ha2.c; prime_0..4 from t1ha_bits.h */
  const uint64_t prime_0 = UINT64_C(0xEC99BF0D8372CAAB), prime_1 = UINT64_C(0x82434FE90EDCEF39);
  const uint64_t prime_2 = UINT64_C(0xD4F06DB99D67BE4B), prime_3 = UINT64_C(0xBD9CACC22C6E9571);
  const uint64_t prime_4 = UINT64_C(0x9C06FAF4D023E3AB);
  (void)prime_3; (void)prime_4; (void)prime_2;
  uint64_t x = seed, y = len;
  uint64_t a = x, b = y;
  uint64_t c = r64(y, 23) + ~x, d = ~y + r64(x, 19);
  (void)prime_0; (void)prime_1;
  int ok = 1;
  for (int i = 0; i < n; i++) {
    const uint64_t *w = base + 4 * i;
    if (((mask >> i) & 1) && (w[3] + c) < (1ULL << 56)) ok = 0;
    uint64_t d02 = w[0] + r64(w[2] + d, 56), c13 = w[1] + r64(w[3] + c, 19);
    d ^= b + r64(w[1], 38); c ^= a + r64(w[0], 57);
    b ^= p6 * (c13 + w[2]); a ^= p5 * (d02 + w[3]);
  }
  return ok;
}

static void wilson(double k, double n, double *lo, double *hi) {
  double z = 1.959964, p = k / n, den = 1 + z * z / n, c = p + z * z / (2 * n);
  double s = z * sqrt(p * (1 - p) / n + z * z / (4 * n * n));
  *lo = (c - s) / den; *hi = (c + s) / den;
}

int main(int argc, char **argv) {
  if (t1ha_selfcheck__t1ha2_atonce() || t1ha_selfcheck__t1ha2_atonce128()) { fprintf(stderr, "upstream selfcheck FAILED\n"); return 1; }
  uint32_t v = smhasher_verif();
  printf("SMHasher3 verification 0x%08X (expected 0x8F16C948)\n", v);
  if (v != 0x8F16C948u) return 1;
  if (argc < 2) return 1;
  int family = !strcmp(argv[1], "family");
  int n = family ? atoi(argv[2]) : 3;
  int lg = atoi(argv[family ? 3 : 2]);
  uint64_t rs = strtoull(argv[family ? 4 : 3], 0, 0);
  rng_init(rs);
  size_t len = (size_t)32 * n;
  uint64_t *base = malloc(len), *m = malloc(len);
  for (int i = 0; i < 4 * n; i++) base[i] = xoshiro();
  for (int i = 0; i < n - 2; i++) { base[4 * i] &= ~(1ULL << 56); base[4 * i + 1] &= ~(1ULL << 37); }
  uint64_t trials = 1ULL << lg;
  if (!family) {
    build(base, n, 1, m);
    printf("M  = "); for (size_t i = 0; i < len; i++) printf("%02x", ((uint8_t *)base)[i]); printf("\n");
    printf("M' = "); for (size_t i = 0; i < len; i++) printf("%02x", ((uint8_t *)m)[i]); printf("\n");
    uint64_t col = 0, col128 = 0, both = 0, mis = 0, ex = 0; int shown = 0;
    for (uint64_t t = 0; t < trials; t++) {
      uint64_t s = xoshiro();
      uint64_t h1 = t1ha2_atonce(base, len, s), h2 = t1ha2_atonce(m, len, s);
      uint64_t x1, x2, g1 = t1ha2_atonce128(&x1, base, len, s), g2 = t1ha2_atonce128(&x2, m, len, s);
      int c64 = h1 == h2, c128 = (g1 == g2 && x1 == x2);
      int pr = model_ok(base, n, 1, s, len);
      col += c64; col128 += c128; both += c64 & c128; mis += (pr != c64); ex += pr;
      if (c64 && !shown) { printf("seed %016llx -> %016llx both\n", (unsigned long long)s, (unsigned long long)h1); shown = 1; }
    }
    double lo, hi; wilson((double)col, (double)trials, &lo, &hi);
    printf("64-bit collisions %llu / %llu = %.7f [%.7f, %.7f]; 1-2^-8 = %.7f\n", (unsigned long long)col,
           (unsigned long long)trials, (double)col / trials, lo, hi, 1 - 1.0 / 256);
    printf("128-bit collisions %llu; both %llu; model predicts %llu; model/actual mismatches %llu\n",
           (unsigned long long)col128, (unsigned long long)both, (unsigned long long)ex, (unsigned long long)mis);
    double eps = (double)col / trials;
    printf("score log2(L/eps) = %.4f (L=%zu)\n", log2((len / 8) / eps), len / 8);
  } else {
    int k = n - 2; uint64_t K = 1ULL << k;
    uint64_t **mem = malloc(K * sizeof *mem);
    for (uint64_t j = 0; j < K; j++) { mem[j] = malloc(len); build(base, n, j, mem[j]); }
    /* all members distinct? */
    uint64_t full = 0, mis = 0; uint64_t *hs = malloc(K * 8);
    uint64_t hist[40] = {0};
    for (uint64_t t = 0; t < trials; t++) {
      uint64_t s = xoshiro();
      for (uint64_t j = 0; j < K; j++) hs[j] = t1ha2_atonce(mem[j], len, s);
      /* largest class: count members equal to the base's hash (all differences are structured) and do an exact count via sort */
      int cmp(const void *x, const void *y);
      qsort(hs, K, 8, cmp);
      uint64_t best = 1, run = 1;
      for (uint64_t j = 1; j < K; j++) { run = (hs[j] == hs[j - 1]) ? run + 1 : 1; if (run > best) best = run; }
      int all = best == K; full += all;
      int pr = model_ok(base, n, K - 1, s, len);
      mis += (pr != all);
      int lb = 0; while ((1ULL << (lb + 1)) <= best) lb++;
      hist[lb]++;
    }
    double lo, hi; wilson((double)full, (double)trials, &lo, &hi);
    printf("family n=%d blocks (%zu bytes), k=2^%d members, seeds %llu\n", n, len, k, (unsigned long long)trials);
    printf("all members collide: %llu / %llu = %.4f [%.4f, %.4f]; model (255/256)^%d = %.4f; model/actual mismatches %llu\n",
           (unsigned long long)full, (unsigned long long)trials, (double)full / trials, lo, hi, k, pow(255.0 / 256, k),
           (unsigned long long)mis);
    printf("largest class histogram (floor log2):");
    for (int i = 0; i < 40; i++) if (hist[i]) printf(" 2^%d:%llu", i, (unsigned long long)hist[i]);
    printf("\n");
  }
  return 0;
}
int cmp(const void *x, const void *y) {
  uint64_t a = *(const uint64_t *)x, b = *(const uint64_t *)y;
  return a < b ? -1 : a > b;
}
