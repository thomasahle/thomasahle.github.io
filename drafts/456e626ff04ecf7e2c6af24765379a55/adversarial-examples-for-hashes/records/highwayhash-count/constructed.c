/* Deterministic checks of the reduction on constructed keys, all through the reference.
   (A) For random (a, K1l, w3, w2, w1) built so that the a-side predicate holds, exactly one of the
       256 values of w0 = W1[0] gives a collision.  (B) With w6 perturbed, none does.
   (C) For uniformly random (a, K1l, w3, w2, w1), the number of colliding w0 is 0 or 1 and equals the
       a-side predicate.  (D) The special triple (w3,w2,w1) = (29,9f,31). */
#include "common.h"
static int aside(uint32_t a, unsigned w4, unsigned w6, unsigned w7) {
  uint64_t V0 = (uint64_t)a + I10;
  uint32_t k = a ^ 0xfe4cce2fu, b = 0x3bd39e10u ^ k;
  uint64_t v1_0 = ((uint64_t)b << 32) | 0x10e82046ull;
  uint64_t W1 = ((uint64_t)w4 << 32) | ((uint64_t)w6 << 48) | ((uint64_t)w7 << 56);
  uint64_t U0 = V0 + zip0(W1, v1_0);
  uint32_t h = (uint32_t)(U0 >> 32), lo = (uint32_t)U0;
  uint8_t h0 = h, h1 = h >> 8, h2 = h >> 16, u2 = lo >> 16, u3 = lo >> 24;
  return h1 == 0xbe && w7 >= 0x15 && u3 == (uint8_t)(h0 + 0xba) && u2 == (uint8_t)(h2 + 0x18);
}
/* number of w0 in [0,256) giving a full-state collision, for key built from (a, K1l, w3, w2, w1) */
static int count_w0(uint32_t a, uint32_t K1l, unsigned w3, unsigned w2, unsigned w1, const uint8_t *m1, const uint8_t *m2, uint64_t k23[2]) {
  int c = 0;
  for (unsigned w0 = 0; w0 < 256; w0++) {
    uint32_t W1lo = (w3 << 24) | (w2 << 16) | (w1 << 8) | w0;
    uint32_t K1h = (W1lo - 0x299f31d0u) ^ 0xb5f18a8cu;
    uint64_t key[4] = { (0xdbe6d5d5ull << 32) | (a ^ 0xfe4cce2fu), ((uint64_t)K1h << 32) | K1l, k23[0], k23[1] };
    HighwayHashState s1, s2; HighwayHashReset(key, &s1); HighwayHashReset(key, &s2);
    for (int q = 0; q < 3; q++) { HighwayHashUpdatePacket(m1 + 32 * q, &s1); HighwayHashUpdatePacket(m2 + 32 * q, &s2); }
    c += !memcmp(&s1, &s2, sizeof s1);
  }
  return c;
}
int main(int argc, char **argv) {
  uint64_t seed = argc > 1 ? strtoull(argv[1], 0, 0) : 5; int T = argc > 2 ? atoi(argv[2]) : 65536;
  uint8_t m1[96], m2[96]; make_pair(m1, m2);
  int badA = 0, badB = 0, badC = 0, badD = 0, nA = 0, nC1 = 0, nC = 0;
  for (int t = 0; t < T; t++) {
    uint64_t r = splitmix64(&seed), r2 = splitmix64(&seed), k23[2] = { splitmix64(&seed), splitmix64(&seed) };
    uint32_t a = (uint32_t)r; unsigned w4 = (r >> 32) & 255, w5 = (r >> 40) & 255;
    unsigned w3 = (r >> 48) & 255, w2 = (r >> 56) & 255, w1 = r2 & 255;
    if ((w3 << 16 | w2 << 8 | w1) == 0x299f31) w1 ^= 1;                 /* keep the special triple for (D) */
    unsigned c1 = ((w3 << 24) | (w2 << 16) | (w1 << 8)) < 0x299f31d0u;   /* c1 is decided by (w3,w2,w1) here */
    /* (A)/(B): solve w6, w7 from (a, w4) */
    uint64_t V0 = (uint64_t)a + I10; uint32_t b = 0x3bd39e10u ^ (a ^ 0xfe4cce2fu);
    uint64_t U0 = V0 + zip0((uint64_t)w4 << 32, ((uint64_t)b << 32) | 0x10e82046ull);
    uint32_t h = (uint32_t)(U0 >> 32), lo = (uint32_t)U0;
    unsigned w7 = ((lo >> 16) - 0xeb) & 255, w6 = ((lo >> 24) - 0xba - (h & 255)) & 255;
    if (aside(a, w4, w6, w7)) {
      nA++;
      uint32_t W1hi = (w7 << 24) | (w6 << 16) | (w5 << 8) | w4;
      uint32_t K1l = (W1hi - 0xa4093822u - c1) ^ 0xc0acf169u;
      if (count_w0(a, K1l, w3, w2, w1, m1, m2, k23) != 1) badA++;
      unsigned d = 1 + (unsigned)(r2 >> 8) % 255;
      W1hi = (w7 << 24) | (((w6 + d) & 255) << 16) | (w5 << 8) | w4;
      K1l = (W1hi - 0xa4093822u - c1) ^ 0xc0acf169u;
      if (count_w0(a, K1l, w3, w2, w1, m1, m2, k23) != 0) badB++;
    }
    /* (C): uniformly random K1l */
    uint32_t K1l = (uint32_t)(r2 >> 16); uint32_t W1hi = (0xc0acf169u ^ K1l) + 0xa4093822u + c1;
    int pred = aside((uint32_t)(r2 >> 32) ^ 0, W1hi & 255, (W1hi >> 16) & 255, W1hi >> 24);
    int cnt = count_w0((uint32_t)(r2 >> 32), K1l, w3, w2, w1, m1, m2, k23);
    nC++; nC1 += cnt; if (cnt != pred) badC++;
  }
  /* (D): the special triple (w3,w2,w1) = (29,9f,31), where c1 = [w0 < 0xd0] depends on w0.
     Claim: the colliding w0 (if any) is < 0xd0, unique, and exists iff the a-side holds for the
     c1 = 1 alignment; no w0 >= 0xd0 collides even when the a-side holds for the c1 = 0 alignment. */
  int nD = 0;
  for (int t = 0; t < 4096; t++) {
    uint64_t r = splitmix64(&seed), k23[2] = { splitmix64(&seed), splitmix64(&seed) };
    uint32_t a = (uint32_t)r; unsigned w4 = (r >> 32) & 255, w5 = (r >> 40) & 255;
    uint64_t V0 = (uint64_t)a + I10; uint32_t b = 0x3bd39e10u ^ (a ^ 0xfe4cce2fu);
    uint64_t U0 = V0 + zip0((uint64_t)w4 << 32, ((uint64_t)b << 32) | 0x10e82046ull);
    uint32_t h = (uint32_t)(U0 >> 32), lo = (uint32_t)U0;
    unsigned w7 = ((lo >> 16) - 0xeb) & 255, w6 = ((lo >> 24) - 0xba - (h & 255)) & 255;
    if (!aside(a, w4, w6, w7)) continue;
    nD++;
    uint32_t W1hi = (w7 << 24) | (w6 << 16) | (w5 << 8) | w4;
    for (unsigned align = 0; align < 2; align++) {
      uint32_t K1l = (W1hi - 0xa4093822u - align) ^ 0xc0acf169u;
      int lowc = 0, highc = 0;
      for (unsigned w0 = 0; w0 < 256; w0++) {
        uint32_t W1lo = 0x299f3100u | w0; uint32_t K1h = (W1lo - 0x299f31d0u) ^ 0xb5f18a8cu;
        uint64_t key[4] = { (0xdbe6d5d5ull << 32) | (a ^ 0xfe4cce2fu), ((uint64_t)K1h << 32) | K1l, k23[0], k23[1] };
        HighwayHashState s1, s2; HighwayHashReset(key, &s1); HighwayHashReset(key, &s2);
        for (int q = 0; q < 3; q++) { HighwayHashUpdatePacket(m1 + 32 * q, &s1); HighwayHashUpdatePacket(m2 + 32 * q, &s2); }
        if (!memcmp(&s1, &s2, sizeof s1)) { if (w0 < 0xd0) lowc++; else highc++; }
      }
      /* actual W1hi seen by w0 < 0xd0 is W1hi_designed + 1 - align (c1 = 1 there) */
      uint32_t W1hi_low = (0xc0acf169u ^ K1l) + 0xa4093822u + 1;
      int pred_low = aside(a, W1hi_low & 255, (W1hi_low >> 16) & 255, W1hi_low >> 24);
      if (highc != 0 || lowc != pred_low) badD++;
    }
  }
  printf("(A) a-side solvable tuples: %d ; tuples whose 256 w0 values do NOT give exactly one collision: %d\n", nA, badA);
  printf("(B) same tuples with w6 perturbed: tuples with any collision: %d\n", badB);
  printf("(C) uniformly random (a,K1l,w3,w2,w1): %d tuples, colliding-w0 total %d (expected ~ %d*0.857/65536 = %.1f), mismatches with the a-side predicate: %d\n", nC, nC1, nC, nC * 0.8570098877 / 65536, badC);
  printf("(D) special triple (29,9f,31): %d a-side tuples x 2 alignments; violations of {no w0>=d0 collides; #w0<d0 collisions == a-side(c1=1 W1hi)}: %d\n", nD, badD);
  return 0;
}
