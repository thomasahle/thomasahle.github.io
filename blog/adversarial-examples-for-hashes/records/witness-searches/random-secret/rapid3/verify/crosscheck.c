/* crosscheck.c -- upstream rapidhash_internal vs the searcher's port on random
   (message, length, seed, secret) tuples, lengths 0..300, plus the shipped secret. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
uint64_t port_hash(const void *, size_t, uint64_t, const uint64_t *);
uint64_t up_hash(const void *, size_t, uint64_t, const uint64_t *);
const uint64_t *up_secret(void);
static uint64_t st = 0x1234567887654321ULL;
static uint64_t rnd(void) { st += 0x9E3779B97F4A7C15ULL; uint64_t z = st; z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL; z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL; return z ^ (z >> 31); }
int main(void) {
  uint8_t buf[512]; uint64_t sec[8]; long n = 0, bad = 0;
  for (long it = 0; it < 2000000; it++) {
    size_t len = (size_t)(rnd() % 301);
    for (size_t i = 0; i < len; i++) buf[i] = (uint8_t)rnd();
    uint64_t seed = rnd();
    for (int i = 0; i < 8; i++) sec[i] = rnd();
    const uint64_t *S = (it & 1) ? sec : up_secret();
    uint64_t a = up_hash(buf, len, seed, S), b = port_hash(buf, len, seed, S);
    n++; if (a != b) { bad++; if (bad < 5) printf("MISMATCH len %zu seed %016llx: %016llx vs %016llx\n", len, (unsigned long long)seed, (unsigned long long)a, (unsigned long long)b); }
  }
  printf("crosscheck upstream vs port: %ld tuples (lengths 0..300, half random secret, half shipped), %ld mismatches -> %s\n", n, bad, bad ? "FAIL" : "PASS");
  return bad != 0;
}
