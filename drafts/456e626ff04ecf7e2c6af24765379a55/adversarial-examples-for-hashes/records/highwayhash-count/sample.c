/* Uniform sampling over the class hi32(key[0]) = 0xdbe6d5d5 (condition 1): all other 224 key bits
   uniform.  Runs the reference on both 96-byte messages, counts full-state collisions after
   packet 3 and HighwayHash64 collisions.  3 threads. */
#include "common.h"
#include <pthread.h>
#include <math.h>
typedef struct { uint64_t seed, n, coll, h64; } Job;
static void *run(void *p) {
  Job *j = p; uint8_t m1[96], m2[96]; make_pair(m1, m2);
  uint64_t s = j->seed;
  for (uint64_t i = 0; i < j->n; i++) {
    uint64_t key[4];
    for (int t = 0; t < 4; t++) key[t] = splitmix64(&s);
    key[0] = (key[0] & 0xffffffffull) | (0xdbe6d5d5ull << 32);
    HighwayHashState s1, s2;
    HighwayHashReset(key, &s1); HighwayHashReset(key, &s2);
    for (int q = 0; q < 3; q++) { HighwayHashUpdatePacket(m1 + 32 * q, &s1); HighwayHashUpdatePacket(m2 + 32 * q, &s2); }
    if (!memcmp(&s1, &s2, sizeof s1)) {
      j->coll++;
      if (HighwayHash64(m1, 96, key) == HighwayHash64(m2, 96, key)) j->h64++;
      printf("  key %016" PRIx64 " %016" PRIx64 " %016" PRIx64 " %016" PRIx64 "\n", key[0], key[1], key[2], key[3]);
    }
  }
  return 0;
}
int main(int argc, char **argv) {
  int lg = argc > 1 ? atoi(argv[1]) : 30; uint64_t seed = argc > 2 ? strtoull(argv[2], 0, 0) : 1;
  uint64_t N = 1ull << lg; pthread_t th[3]; Job jb[3];
  for (int t = 0; t < 3; t++) { jb[t].seed = seed * 0x1000000001ull + t; jb[t].n = N / 3 + (t < (int)(N % 3)); jb[t].coll = jb[t].h64 = 0; pthread_create(&th[t], 0, run, &jb[t]); }
  uint64_t coll = 0, h64 = 0; for (int t = 0; t < 3; t++) { pthread_join(th[t], 0); coll += jb[t].coll; h64 += jb[t].h64; }
  double p = 56165.0 / 1099511627776.0;
  printf("class keys N = 2^%d (seed %" PRIu64 "): full-state collisions after packet 3: %" PRIu64 " (of which HighwayHash64 collides: %" PRIu64 ")\n", lg, seed, coll, h64);
  printf("  expected under the proved rate 235*239/2^40: %.2f (sd %.2f);  observed rate 2^%.3f\n", N * p, sqrt(N * p), log2((double)coll / N));
  return 0;
}
