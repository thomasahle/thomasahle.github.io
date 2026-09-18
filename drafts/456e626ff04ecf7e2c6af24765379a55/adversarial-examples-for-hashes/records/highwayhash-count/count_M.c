/* Exact count M = #{(a, w4, w6, w7) in 2^32 x 2^24 : (i), (iii), (iv')} by a 2^32 loop over
   V0lo = lo32(a + I10) (a bijective image of a), collapsing w4 through the byte-1 carry f1.
   `brute` re-counts M by a direct 2^40 loop over (a, w4) using 64-bit arithmetic on the real
   zipper term (no byte bookkeeping), split over 3 threads. */
#include "common.h"
#include <pthread.h>
#include <math.h>

/* a-side predicate for one (a, w4, w6, w7), computed with the real zipper term */
static inline int aside(uint32_t a, unsigned w4, unsigned w6, unsigned w7) {
  uint64_t V0 = (uint64_t)a + I10;
  uint32_t k = a ^ 0xfe4cce2fu, b = 0x3bd39e10u ^ k;
  uint64_t v1_0 = ((uint64_t)b << 32) | 0x10e82046ull;
  uint64_t W1 = ((uint64_t)w4 << 32) | ((uint64_t)w6 << 48) | ((uint64_t)w7 << 56); /* other bytes irrelevant */
  uint64_t U0 = V0 + zip0(W1, v1_0);
  uint32_t h = (uint32_t)(U0 >> 32), lo = (uint32_t)U0;
  /* conditions (i), (iii), (iv') written on h and lo32(U0) directly */
  uint8_t h0 = h, h1 = h >> 8, h2 = h >> 16, h3 = h >> 24;
  uint8_t u2 = lo >> 16, u3 = lo >> 24;
  /* (iv'): e0 = 0 <=> h1 == 0xbe ; W1[7] >= 0x15 */
  if (h1 != 0xbe) return 0;
  if (w7 < 0x15) return 0;
  (void)h3;
  if (u3 != (uint8_t)(h0 + 0xba)) return 0;            /* (i) */
  if (u2 != (uint8_t)(h2 + 0x18)) return 0;            /* (iii) */
  return 1;
}

static uint64_t dp_count(void) {
  uint64_t M = 0;
  for (uint64_t V = 0; V < (1ull << 32); V++) {
    uint32_t V0lo = (uint32_t)V;
    unsigned ca = V0lo < 0xcb0ef593u;
    uint32_t a = V0lo - 0xcb0ef593u;
    unsigned b1 = ((a >> 8) & 255) ^ 0x50;
    unsigned v0 = V0lo & 255, v1 = (V0lo >> 8) & 255, v2 = (V0lo >> 16) & 255, v3 = V0lo >> 24;
    unsigned f0 = (v0 + 0x10) >> 8;
    unsigned n1 = v1 + f0, n0 = 256 - n1;          /* #w4 giving f1 = 1 / 0 */
    for (unsigned f1 = 0; f1 < 2; f1++) {
      unsigned n = f1 ? n1 : n0; if (!n) continue;
      unsigned s2 = v2 + 0xe8 + f1, u2 = s2 & 255, f2 = s2 >> 8;
      unsigned s3 = v3 + b1 + f2, u3 = s3 & 255, cz = s3 >> 8;
      unsigned w7 = (u2 - 0xeb) & 255;
      unsigned w6 = (u3 - 0xca - ca - cz) & 255;
      if (w7 >= 0x15 && 0x10 + w6 + ca + cz <= 255) M += n;
    }
  }
  return M;
}

typedef struct { uint64_t lo, hi, count; } Job;
static void *brute(void *p) {
  Job *j = p; uint64_t c = 0;
  for (uint64_t a = j->lo; a < j->hi; a++)
    for (unsigned w4 = 0; w4 < 256; w4++) {
      /* solve w7, w6 from the byte equations, then confirm with the real-arithmetic predicate */
      uint64_t V0 = a + I10;
      uint32_t k = (uint32_t)a ^ 0xfe4cce2fu, b = 0x3bd39e10u ^ k;
      uint64_t v1_0 = ((uint64_t)b << 32) | 0x10e82046ull;
      uint64_t U0 = V0 + zip0((uint64_t)w4 << 32, v1_0);
      uint32_t h = (uint32_t)(U0 >> 32), lo = (uint32_t)U0;
      unsigned w7 = ((lo >> 16) - 0xeb) & 255;                  /* forces (iii) */
      unsigned w6 = ((lo >> 24) - 0xba - (h & 255)) & 255;      /* forces (i): h0 + w6 ... */
      /* h0 here was computed with w6 = 0: h0 = 0x10 + ca + cz; the real h0 = h0 + w6 (mod 256) */
      c += aside((uint32_t)a, w4, w6, w7);
      /* any other (w6', w7') fails (i) or (iii) since h0 and h2 are injective in w6, w7 mod 256 given
         e0 = 0; double-check a random alternative: */
      if ((a & 0xfffff) == 0x12345) { unsigned d = 1 + (w4 % 255); c += 1000000000ull * aside((uint32_t)a, w4, (w6 + d) & 255, w7); }
    }
  j->count = c; return 0;
}

typedef struct { uint64_t lo, hi, count; unsigned w4; } JobW;
static void *brute_w4(void *p) {
  JobW *j = p; uint64_t c = 0;
  for (uint64_t a = j->lo; a < j->hi; a++) {
    uint64_t V0 = a + I10;
    uint32_t k = (uint32_t)a ^ 0xfe4cce2fu, b = 0x3bd39e10u ^ k;
    uint64_t v1_0 = ((uint64_t)b << 32) | 0x10e82046ull;
    uint64_t U0 = V0 + zip0((uint64_t)j->w4 << 32, v1_0);
    uint32_t h = (uint32_t)(U0 >> 32), lo = (uint32_t)U0;
    unsigned w7 = ((lo >> 16) - 0xeb) & 255, w6 = ((lo >> 24) - 0xba - (h & 255)) & 255;
    c += aside((uint32_t)a, j->w4, w6, w7);
  }
  j->count = c; return 0;
}
int main(int argc, char **argv) {
  if (argc > 2 && !strcmp(argv[1], "brute_w4")) {
    /* for each listed w4: count over ALL 2^32 values of a with the real-arithmetic predicate.
       The proof (step 5) says the count is 256*256*235*239 = 3680829440 for EVERY w4. */
    for (int i = 2; i < argc; i++) {
      unsigned w4 = (unsigned)strtoul(argv[i], 0, 16) & 255;
      pthread_t th[3]; JobW jb[3];
      for (int t = 0; t < 3; t++) { jb[t].lo = (1ull << 32) * t / 3; jb[t].hi = (1ull << 32) * (t + 1) / 3; jb[t].w4 = w4; pthread_create(&th[t], 0, brute_w4, &jb[t]); }
      uint64_t B = 0; for (int t = 0; t < 3; t++) { pthread_join(th[t], 0); B += jb[t].count; }
      printf("w4 = %02x: #{a in 2^32 : (i),(iii),(iv') solvable} = %" PRIu64 "  %s 3680829440\n", w4, B, B == 3680829440ull ? "==" : "!=");
      fflush(stdout);
    }
    return 0;
  }
  uint64_t M = dp_count();
  printf("M (DP over V0lo, 2^32 steps) = %" PRIu64 "\n", M);
  printf("M / 2^56 = %.10f ;  M / 2^40 = %.10f ;  log2(M/2^64) = %.6f\n", (double)M / 72057594037927936.0, (double)M / 1099511627776.0, log2((double)M) - 64);
  if (argc > 1 && !strcmp(argv[1], "brute")) {
    pthread_t th[3]; Job jb[3];
    for (int t = 0; t < 3; t++) { jb[t].lo = (1ull << 32) * t / 3; jb[t].hi = (1ull << 32) * (t + 1) / 3; pthread_create(&th[t], 0, brute, &jb[t]); }
    uint64_t B = 0; for (int t = 0; t < 3; t++) { pthread_join(th[t], 0); B += jb[t].count; }
    printf("M (brute force over (a, w4), 2^40 steps, real arithmetic) = %" PRIu64 "  %s\n", B, B == M ? "MATCHES DP" : "DIFFERS");
  }
  return 0;
}
