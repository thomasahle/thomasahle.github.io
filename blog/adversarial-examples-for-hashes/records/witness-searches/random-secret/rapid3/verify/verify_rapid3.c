/* verify_rapid3.c -- independent random-secret collision sampler for rapidhash v3.
 *
 * Hash body: the UNMODIFIED upstream header rapidhash.h (tag rapidhash_v3,
 * sha256 807874eeb23339eaa4b435fbadd33d46718c88e32f3eb67bab3eb965eecc626e), included
 * verbatim from upstream/rapidhash_v3.h; we call its header-visible
 *   rapidhash_internal(key, len, seed, secret)
 * with a caller-supplied 8-word secret.  Default configuration (RAPIDHASH_COMPACT,
 * RAPIDHASH_FAST), which is what the public rapidhash()/rapidhash_withSeed() use.
 *
 * Key model ("random"): the 64-bit seed and all eight 64-bit secret words are drawn
 * independently and uniformly for every key (576 bits).  Model "default": uniform seed,
 * shipped rapid_secret[].
 *
 * RNG (independent of the searcher's xoshiro256** harness): counter-mode splitmix64,
 * word j of key k = sm64(base + (9k+j) * golden), so every sampled key is reproducible
 * from (rngseed, k).  Optional cross-check RNG: ChaCha20 (-DRNG_CHACHA), one 64-byte
 * block (counter = k, nonce 0) for seed + secret[0..6] and one (counter = k, nonce 1)
 * for secret[7].
 *
 * Build: gcc -O3 -march=native -fopenmp -o verify_rapid3 verify_rapid3.c -lm
 * Usage: ./verify_rapid3 selftest
 *        ./verify_rapid3 pair LOG2N RNGSEED HEXM HEXM2 [random|default]
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h>
#include "upstream/rapidhash_v3.h"

static inline uint64_t sm64(uint64_t z) {
  z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
  z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
  return z ^ (z >> 31);
}
#define GOLDEN 0x9E3779B97F4A7C15ULL

#ifdef RNG_CHACHA
#define ROTL32(x, n) (((x) << (n)) | ((x) >> (32 - (n))))
#define QR(a, b, c, d) \
  a += b; d ^= a; d = ROTL32(d, 16); c += d; b ^= c; b = ROTL32(b, 12); \
  a += b; d ^= a; d = ROTL32(d, 8);  c += d; b ^= c; b = ROTL32(b, 7);
static void chacha20_block(const uint32_t key[8], uint64_t counter, uint64_t nonce, uint32_t out[16]) {
  uint32_t s[16] = {0x61707865, 0x3320646e, 0x79622d32, 0x6b206574,
                    key[0], key[1], key[2], key[3], key[4], key[5], key[6], key[7],
                    (uint32_t)counter, (uint32_t)(counter >> 32), (uint32_t)nonce, (uint32_t)(nonce >> 32)};
  uint32_t x[16];
  memcpy(x, s, sizeof x);
  for (int r = 0; r < 10; r++) {
    QR(x[0], x[4], x[8], x[12])  QR(x[1], x[5], x[9], x[13])
    QR(x[2], x[6], x[10], x[14]) QR(x[3], x[7], x[11], x[15])
    QR(x[0], x[5], x[10], x[15]) QR(x[1], x[6], x[11], x[12])
    QR(x[2], x[7], x[8], x[13])  QR(x[3], x[4], x[9], x[14])
  }
  for (int i = 0; i < 16; i++) out[i] = x[i] + s[i];
}
static void draw_key(uint64_t base, uint64_t k, uint64_t *seed, uint64_t secret[8]) {
  uint32_t key[8];
  for (int i = 0; i < 8; i++) key[i] = (uint32_t)sm64(base + (uint64_t)(i + 1) * GOLDEN);
  uint32_t o[16], o2[16];
  chacha20_block(key, k, 0, o);
  chacha20_block(key, k, 1, o2);
  uint64_t w[8];
  for (int i = 0; i < 8; i++) w[i] = (uint64_t)o[2 * i] | ((uint64_t)o[2 * i + 1] << 32);
  *seed = w[0];
  for (int i = 0; i < 7; i++) secret[i] = w[i + 1];
  secret[7] = (uint64_t)o2[0] | ((uint64_t)o2[1] << 32);
}
#define RNG_NAME "chacha20"
#else
static inline void draw_key(uint64_t base, uint64_t k, uint64_t *seed, uint64_t secret[8]) {
  uint64_t idx = 9 * k;
  *seed = sm64(base + idx * GOLDEN);
  for (int i = 0; i < 8; i++) secret[i] = sm64(base + (idx + 1 + i) * GOLDEN);
}
#define RNG_NAME "splitmix64-counter"
#endif

static int hex2bytes(const char *h, uint8_t *out, int max) {
  int n = (int)strlen(h);
  if (n % 2 || n / 2 > max) return -1;
  for (int i = 0; i < n / 2; i++) {
    unsigned v;
    if (sscanf(h + 2 * i, "%2x", &v) != 1) return -1;
    out[i] = (uint8_t)v;
  }
  return n / 2;
}

static int selftest(void) {
  /* Row witness (from the page's record): seed 3187ae8a8617e034, default secret,
     both 32-byte messages of pair A hash to a7ee6375a78a86f0. */
  uint8_t m[32], m2[32];
  hex2bytes("9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c", m, 32);
  hex2bytes("642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c", m2, 32);
  uint64_t s = 0x3187ae8a8617e034ULL;
  uint64_t h1 = rapidhash_internal(m, 32, s, rapid_secret), h2 = rapidhash_internal(m2, 32, s, rapid_secret);
  uint64_t w1 = rapidhash_withSeed(m, 32, s), w2 = rapidhash_withSeed(m2, 32, s);
  printf("witness seed %016llx: internal %016llx %016llx  withSeed %016llx %016llx  (record a7ee6375a78a86f0, both)\n",
         (unsigned long long)s, (unsigned long long)h1, (unsigned long long)h2, (unsigned long long)w1, (unsigned long long)w2);
  int ok = (h1 == 0xa7ee6375a78a86f0ULL && h2 == h1 && w1 == h1 && w2 == h1);
  printf("rapidhash(\"\",0)=%016llx rapidhash(\"a\",1)=%016llx rapidhash(\"abc\",3)=%016llx rapidhash(m,32)=%016llx\n",
         (unsigned long long)rapidhash("", 0), (unsigned long long)rapidhash("a", 1),
         (unsigned long long)rapidhash("abc", 3), (unsigned long long)rapidhash(m, 32));
  printf("secret words: ");
  for (int i = 0; i < 8; i++) printf("%016llx ", (unsigned long long)rapid_secret[i]);
  printf("\nRNG: %s; sample key 0 of rngseed 1: ", RNG_NAME);
  uint64_t seed, sec[8];
  draw_key(sm64(1), 0, &seed, sec);
  printf("seed %016llx secret", (unsigned long long)seed);
  for (int i = 0; i < 8; i++) printf(" %016llx", (unsigned long long)sec[i]);
  printf("\nselftest %s\n", ok ? "PASS" : "FAIL");
  return ok ? 0 : 1;
}

int main(int argc, char **argv) {
  if (argc >= 2 && !strcmp(argv[1], "selftest")) return selftest();
  if (argc < 6 || strcmp(argv[1], "pair")) {
    fprintf(stderr, "usage: %s selftest | pair LOG2N RNGSEED HEXM HEXM2 [random|default]\n", argv[0]);
    return 2;
  }
  int log2n = atoi(argv[2]);
  uint64_t rngseed = strtoull(argv[3], 0, 10);
  uint8_t m[512], m2[512];
  int len = hex2bytes(argv[4], m, 512), len2 = hex2bytes(argv[5], m2, 512);
  if (len < 0 || len2 < 0) { fprintf(stderr, "bad hex\n"); return 2; }
  int use_default = (argc >= 7 && !strcmp(argv[6], "default"));
  uint64_t N = 1ULL << log2n;
  uint64_t base = sm64(rngseed ^ 0x5eed5eed5eed5eedULL);
  int nthreads = omp_get_max_threads();
  printf("verify_rapid3: upstream rapidhash_v3.h, model %s, RNG %s, rngseed %llu, 2^%d keys, %d threads\n",
         use_default ? "uniform seed + shipped rapid_secret" : "uniform seed + 8 uniform secret words (576 bits)",
         RNG_NAME, (unsigned long long)rngseed, log2n, nthreads);
  printf("m  = %s (%d B)\nm' = %s (%d B)\n", argv[4], len, argv[5], len2);
  fflush(stdout);
  uint64_t count = 0, ex_k = ~0ULL;
  double t0 = omp_get_wtime();
#pragma omp parallel for reduction(+ : count) schedule(static)
  for (uint64_t k = 0; k < N; k++) {
    uint64_t seed, sec[8];
    draw_key(base, k, &seed, sec);
    const uint64_t *S = use_default ? rapid_secret : sec;
    uint64_t h1 = rapidhash_internal(m, (size_t)len, seed, S);
    uint64_t h2 = rapidhash_internal(m2, (size_t)len2, seed, S);
    if (h1 == h2) {
      count++;
#pragma omp critical
      if (k < ex_k) ex_k = k;
    }
  }
  double dt = omp_get_wtime() - t0;
  int L = (len > len2 ? len : len2);
  L = (L + 7) / 8;
  double rate = (double)count / (double)N;
  printf("count %llu / 2^%d keys", (unsigned long long)count, log2n);
  if (count) printf("  rate 2^%.3f  cap (L=%d) %.3f bits", log2(rate), L, log2((double)L) - log2(rate));
  printf("  [%.0f s wall]\n", dt);
  if (ex_k != ~0ULL) {
    uint64_t seed, sec[8];
    draw_key(base, ex_k, &seed, sec);
    const uint64_t *S = use_default ? rapid_secret : sec;
    printf("example (key index %llu): seed %016llx", (unsigned long long)ex_k, (unsigned long long)seed);
    if (!use_default) { printf(" secret"); for (int i = 0; i < 8; i++) printf(" %016llx", (unsigned long long)sec[i]); }
    printf(" -> %016llx %016llx\n", (unsigned long long)rapidhash_internal(m, (size_t)len, seed, S),
           (unsigned long long)rapidhash_internal(m2, (size_t)len2, seed, S));
  }
  return 0;
}
