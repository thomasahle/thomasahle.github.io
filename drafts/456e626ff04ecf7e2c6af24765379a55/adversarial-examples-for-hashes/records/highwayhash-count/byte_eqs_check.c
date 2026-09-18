/* Check that the byte-level system (i),(ii),(iii),(iv') is EQUIVALENT to D0 == 4d000000, key by key,
   on (1) random class keys and (2) constructed keys (a-side solved, 256 values of W1[0]) so that the
   true branch is exercised ~1/256 of the time.  D0 is taken from the reference state after packet 1. */
#include "common.h"
static int byte_eqs(uint32_t k, uint64_t K1) {
  uint32_t a = 0xfe4cce2fu ^ k; uint32_t K1l = (uint32_t)K1, K1h = (uint32_t)(K1 >> 32);
  unsigned a0 = a & 255, a1 = (a >> 8) & 255, b0 = a0 ^ 0x3f, b1 = a1 ^ 0x50;
  uint64_t W1lo_full = (uint64_t)(0xb5f18a8cu ^ K1h) + 0x299f31d0u; unsigned c1 = W1lo_full >> 32; uint32_t W1lo = (uint32_t)W1lo_full;
  uint32_t W1hi = (0xc0acf169u ^ K1l) + 0xa4093822u + c1;
  uint64_t W0lo_full = (uint64_t)(0x299f31d0u ^ K1l) + 0xb5f18a8cu; unsigned c0 = W0lo_full >> 32; uint32_t W0lo = (uint32_t)W0lo_full;
  unsigned w1 = (W1lo >> 8) & 255, w2 = (W1lo >> 16) & 255, w3 = W1lo >> 24;
  unsigned w4 = W1hi & 255, w5 = (W1hi >> 8) & 255, w6 = (W1hi >> 16) & 255, w7 = W1hi >> 24;
  /* a-side */
  uint64_t S = 0xcb0ef593ull + ((uint64_t)b1 << 24) + 0xe80000ull + ((uint64_t)w4 << 8) + 0x10;
  uint64_t sum = a + S; unsigned gamma = (unsigned)(sum >> 32); uint32_t U0lo = (uint32_t)sum;
  unsigned u2 = (U0lo >> 16) & 255, u3 = U0lo >> 24;
  unsigned e0 = (0x10 + w6 + gamma) >> 8;
  unsigned h0 = (0x10 + w6 + gamma) & 255;
  unsigned h2 = (0xd3 + w7) & 255;
  int iv = (e0 == 0) && (w7 >= 0x15);
  int i_ = (u3 == ((h0 + 0xba) & 255));
  int iii = (u2 == ((h2 + 0x18) & 255));
  unsigned g0 = h0 >= 0x46;
  /* (ii) */
  uint64_t Z1lo = ((uint64_t)w5 << 24) | ((uint64_t)w2 << 16) | ((uint64_t)b0 << 8) | w3;
  unsigned cp = (unsigned)(((uint64_t)W0lo + Z1lo) >> 32);
  unsigned U1_4 = ((0x22 ^ (K1h & 255)) + 0x69 + c0 + w1 + cp) & 255;
  int ii = (U1_4 == ((0x9d + g0) & 255));
  return iv && i_ && iii && ii;
}
int main(int argc, char **argv) {
  uint64_t seed = argc > 1 ? strtoull(argv[1], 0, 0) : 3; uint64_t N = argc > 2 ? strtoull(argv[2], 0, 0) : 1000000;
  uint8_t m1[96], m2[96]; make_pair(m1, m2);
  uint64_t mism = 0, tru = 0, cmism = 0, ctru = 0, cn = 0;
  for (uint64_t n = 0; n < N; n++) {
    uint64_t key[4]; for (int i = 0; i < 4; i++) key[i] = splitmix64(&seed);
    key[0] = (key[0] & 0xffffffffull) | (0xdbe6d5d5ull << 32);
    HighwayHashState s; HighwayHashReset(key, &s); HighwayHashUpdatePacket(m1, &s);
    int ref = ((uint32_t)s.v1[0] - (uint32_t)(s.v0[0] >> 32)) == TARGET_D0;
    int be = byte_eqs((uint32_t)key[0], key[1]); tru += ref; mism += (ref != be);
  }
  /* constructed: a-side solved from (a, w4); w5, w3, w2, w1 random; all 256 w0 */
  for (uint64_t n = 0; n < N / 256; n++) {
    uint64_t r = splitmix64(&seed), r2 = splitmix64(&seed);
    uint32_t a = (uint32_t)r; unsigned w4 = (r >> 32) & 255, w5 = (r >> 40) & 255, w3 = (r >> 48) & 255, w2 = (r >> 56) & 255, w1 = r2 & 255;
    uint64_t V0 = (uint64_t)a + I10; uint32_t b = 0x3bd39e10u ^ (a ^ 0xfe4cce2fu);
    uint64_t U0 = V0 + zip0((uint64_t)w4 << 32, ((uint64_t)b << 32) | 0x10e82046ull);
    uint32_t h = (uint32_t)(U0 >> 32), lo = (uint32_t)U0;
    unsigned w7 = ((lo >> 16) - 0xeb) & 255, w6 = ((lo >> 24) - 0xba - (h & 255)) & 255;
    unsigned c1 = ((w3 << 24) | (w2 << 16) | (w1 << 8) | 0x80) < 0x299f31d0u;   /* exact except at the special triple */
    uint32_t W1hi = (w7 << 24) | (w6 << 16) | (w5 << 8) | w4, K1l = (W1hi - 0xa4093822u - c1) ^ 0xc0acf169u;
    for (unsigned w0 = 0; w0 < 256; w0++) {
      uint32_t W1lo = (w3 << 24) | (w2 << 16) | (w1 << 8) | w0; uint32_t K1h = (W1lo - 0x299f31d0u) ^ 0xb5f18a8cu;
      uint64_t key[4] = { (0xdbe6d5d5ull << 32) | (a ^ 0xfe4cce2fu), ((uint64_t)K1h << 32) | K1l, r2 >> 8, r2 * 3 };
      HighwayHashState s; HighwayHashReset(key, &s); HighwayHashUpdatePacket(m1, &s);
      int ref = ((uint32_t)s.v1[0] - (uint32_t)(s.v0[0] >> 32)) == TARGET_D0;
      int be = byte_eqs((uint32_t)key[0], key[1]); cn++; ctru += ref; cmism += (ref != be);
    }
  }
  printf("random class keys: N = %" PRIu64 ", D0 == target: %" PRIu64 ", byte-system mismatches: %" PRIu64 "\n", N, tru, mism);
  printf("constructed keys : N = %" PRIu64 ", D0 == target: %" PRIu64 " (expected about N/256 * 0.857/0.857.. = %.0f), byte-system mismatches: %" PRIu64 "\n", cn, ctru, cn / 256.0 * 0.8570098877, cmism);
  return 0;
}
