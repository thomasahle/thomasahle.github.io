/* Validate the explicit packet-1 formulas and the trail bookkeeping against the reference
   on N random keys from the class hi32(key[0]) = 0xdbe6d5d5 (condition 1). */
#include "common.h"
int main(int argc, char **argv) {
  uint64_t N = argc > 1 ? strtoull(argv[1], 0, 0) : 1000000, seed0 = argc > 2 ? strtoull(argv[2], 0, 0) : 1, seed = seed0;
  uint8_t m1[96], m2[96]; make_pair(m1, m2);
  uint64_t bad_state = 0, bad_pattern1 = 0, bad_h1 = 0, bad_equiv = 0, coll = 0, cond2 = 0, bad_p2 = 0;
  uint64_t hist_top[256] = {0};
  for (uint64_t n = 0; n < N; n++) {
    uint64_t key[4];
    for (int i = 0; i < 4; i++) key[i] = splitmix64(&seed);
    key[0] = (key[0] & 0xffffffffull) | (0xdbe6d5d5ull << 32);
    HighwayHashState s1, s2;
    HighwayHashReset(key, &s1); HighwayHashReset(key, &s2);
    HighwayHashUpdatePacket(m1, &s1); HighwayHashUpdatePacket(m2, &s2);
    P1 f = formula_p1((uint32_t)key[0], key[1]);
    if (s1.v0[0] != f.U0 || s1.v1[0] != f.T || s1.v0[1] != f.U1 || s1.v1[1] != f.T1 ||
        s1.mul0[0] != I00 || s1.mul1[0] != f.M10 || s1.mul0[1] != f.M01 || s1.mul1[1] != f.M11) bad_state++;
    /* predicted difference pattern after packet 1 */
    int ok = (s2.v0[0] == s1.v0[0] + (1ull << 40)) && (s2.v1[0] == s1.v1[0] + (1ull << 8) + (1ull << 24));
    for (int i = 0; i < 4; i++) {
      if (i) ok &= (s2.v0[i] == s1.v0[i]) && (s2.v1[i] == s1.v1[i]);
      ok &= (s2.mul0[i] == s1.mul0[i]) && (s2.mul1[i] == s1.mul1[i]);
    }
    /* condition A is automatic: byte 5 of v0[0] (= byte 1 of h) is 0xbe or 0xbf */
    uint32_t h = (uint32_t)(s1.v0[0] >> 32);
    uint8_t h1 = (uint8_t)(h >> 8);
    if (h1 != 0xbe && h1 != 0xbf) bad_h1++;
    if (!ok) bad_pattern1++;
    uint32_t D0 = (uint32_t)(s1.v1[0]) - h;
    hist_top[D0 >> 24]++;
    int c2 = (D0 == TARGET_D0);
    cond2 += c2;
    /* packets 2 and 3 through the reference; full-state compare */
    HighwayHashUpdatePacket(m1 + 32, &s1); HighwayHashUpdatePacket(m2 + 32, &s2);
    if (c2) {  /* after packet 2 the only difference must be v1[0] -= 2^8 */
      int ok2 = (s2.v1[0] == s1.v1[0] - (1ull << 8));
      for (int i = 0; i < 4; i++) { ok2 &= s1.v0[i] == s2.v0[i]; ok2 &= s1.mul0[i] == s2.mul0[i]; ok2 &= s1.mul1[i] == s2.mul1[i]; if (i) ok2 &= s1.v1[i] == s2.v1[i]; }
      if (!ok2) bad_p2++;
    }
    HighwayHashUpdatePacket(m1 + 64, &s1); HighwayHashUpdatePacket(m2 + 64, &s2);
    int same = !memcmp(&s1, &s2, sizeof s1);
    coll += same;
    if (same != c2) bad_equiv++;
  }
  printf("N = %" PRIu64 " class keys (seed %" PRIu64 ")\n", N, seed0);
  printf("formula mismatches (v0[0],v1[0],v0[1],v1[1],mul0[0],mul1[0],mul0[1],mul1[1] after packet 1): %" PRIu64 "\n", bad_state);
  printf("packet-1 difference pattern (v0[0]+2^40, v1[0]+2^8+2^24, rest equal) violations: %" PRIu64 "\n", bad_pattern1);
  printf("byte 1 of h not in {be,bf}: %" PRIu64 "\n", bad_h1);
  printf("cond 2 (D0 == 4d000000): %" PRIu64 ";  packet-2 pattern violations among them: %" PRIu64 "\n", cond2, bad_p2);
  printf("full-state collisions after packet 3: %" PRIu64 ";  (collision xor cond2) mismatches: %" PRIu64 "\n", coll, bad_equiv);
  printf("top byte of D0 histogram (nonzero bins):");
  for (int i = 0; i < 256; i++) if (hist_top[i]) printf(" %02x:%" PRIu64, i, hist_top[i]);
  printf("\n");
  return 0;
}
