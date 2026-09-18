#include "common.h"
static int aside(uint32_t a, unsigned w4, unsigned w6, unsigned w7) {
  uint64_t V0 = (uint64_t)a + I10; uint32_t k = a ^ 0xfe4cce2fu, b = 0x3bd39e10u ^ k;
  uint64_t v1_0 = ((uint64_t)b << 32) | 0x10e82046ull;
  uint64_t W1 = ((uint64_t)w4 << 32) | ((uint64_t)w6 << 48) | ((uint64_t)w7 << 56);
  uint64_t U0 = V0 + zip0(W1, v1_0); uint32_t h = (uint32_t)(U0 >> 32), lo = (uint32_t)U0;
  uint8_t h0 = h, h1 = h >> 8, h2 = h >> 16, u2 = lo >> 16, u3 = lo >> 24;
  return h1 == 0xbe && w7 >= 0x15 && u3 == (uint8_t)(h0 + 0xba) && u2 == (uint8_t)(h2 + 0x18);
}
int main(void) {
  uint64_t seed = 99; uint8_t m1[96], m2[96]; make_pair(m1, m2); int shown = 0;
  while (shown < 6) {
    uint64_t r = splitmix64(&seed); uint32_t a = (uint32_t)r; unsigned w4 = (r >> 32) & 255, w5 = (r >> 40) & 255;
    uint64_t V0 = (uint64_t)a + I10; uint32_t b = 0x3bd39e10u ^ (a ^ 0xfe4cce2fu);
    uint64_t U0 = V0 + zip0((uint64_t)w4 << 32, ((uint64_t)b << 32) | 0x10e82046ull);
    uint32_t h = (uint32_t)(U0 >> 32), lo = (uint32_t)U0;
    unsigned w7 = ((lo >> 16) - 0xeb) & 255, w6 = ((lo >> 24) - 0xba - (h & 255)) & 255;
    if (!aside(a, w4, w6, w7)) continue;
    shown++;
    uint32_t W1hi = (w7 << 24) | (w6 << 16) | (w5 << 8) | w4;
    for (unsigned c1 = 0; c1 < 2; c1++) {
      uint32_t K1l = (W1hi - 0xa4093822u - c1) ^ 0xc0acf169u;
      printf("a=%08x w4=%02x w6=%02x w7=%02x c1-align=%u: colliding w0 =", a, w4, w6, w7, c1);
      for (unsigned w0 = 0; w0 < 256; w0++) {
        uint32_t W1lo = (0x299f3100u) | w0; uint32_t K1h = (W1lo - 0x299f31d0u) ^ 0xb5f18a8cu;
        uint64_t key[4] = { (0xdbe6d5d5ull << 32) | (a ^ 0xfe4cce2fu), ((uint64_t)K1h << 32) | K1l, 0, 0 };
        HighwayHashState s1, s2; HighwayHashReset(key, &s1); HighwayHashReset(key, &s2);
        for (int q = 0; q < 3; q++) { HighwayHashUpdatePacket(m1 + 32 * q, &s1); HighwayHashUpdatePacket(m2 + 32 * q, &s2); }
        if (!memcmp(&s1, &s2, sizeof s1)) printf(" %02x", w0);
        if (w0 == 0x7c && c1 == 1) { /* show the actual W1 for this key */
          P1 f = formula_p1((uint32_t)key[0], key[1]);
          printf(" [W1=%016" PRIx64 " lo32(T)-h=%08x]", f.W1, (uint32_t)f.T - (uint32_t)(f.U0 >> 32));
        }
      }
      printf("\n");
    }
  }
  return 0;
}
