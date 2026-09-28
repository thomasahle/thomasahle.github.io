/* pair73_check.c -- the 73-byte komihash 5.34 pair from README.md, checked with the
 * upstream komihash.h (included unmodified).
 * Build: cc -O2 -std=c11 -I<komihash> pair73_check.c -o pair73_check
 * Run:   ./pair73_check [log2 seeds = 24]
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "komihash.h"

static const char *HM  = "2a87bfdc2277e89c6ef4cfdf0cfdf18f7827cd8bcaebf22c217a1c4e7029d580"
                         "6e9047ff9934dc7f029caecb565288c17557d8339262c6b9bf0ee34f909eee46"
                         "3c01000000000000a7";
static const char *HMp = "2a87bfdc2277e89c6ef4cfdf0cfdf18f7827cd8bcaebf22c217a1c4e7029d580"
                         "6e9047ff9934dc7f029caecb565288c17557d8339262c6b9bf0ee34f909eee46"
                         "a7010000000000003c";
static void unhex(const char *h, uint8_t *o, int n) { for (int i = 0; i < n; i++) { unsigned v; sscanf(h + 2 * i, "%2x", &v); o[i] = (uint8_t)v; } }
static uint64_t sm;
static uint64_t splitmix64(void) {
  uint64_t z = (sm += 0x9e3779b97f4a7c15ULL);
  z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL; z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
  return z ^ (z >> 31);
}
int main(int argc, char **argv) {
  uint8_t key[256], table[2048];
  for (int i = 0; i < 256; i++) { key[i] = (uint8_t)i; uint64_t h = komihash(key, (size_t)i, (uint64_t)(256 - i)); memcpy(table + 8 * i, &h, 8); }
  uint32_t v = (uint32_t)komihash(table, sizeof table, 0);
  printf("SMHasher3 verification 0x%08X (expected 0x8157FF6D)\n", v);
  if (v != 0x8157FF6Du) return 1;
  uint8_t M[73], Mp[73];
  unhex(HM, M, 73); unhex(HMp, Mp, 73);
  printf("seed 0: %016llx %016llx\n", (unsigned long long)komihash(M, 73, 0), (unsigned long long)komihash(Mp, 73, 0));
  int lg = argc > 1 ? atoi(argv[1]) : 24; uint64_t N = 1ULL << lg, c = 0; sm = 1;
  for (uint64_t t = 0; t < N; t++) { uint64_t s = splitmix64(); c += komihash(M, 73, s) == komihash(Mp, 73, s); }
  printf("uniform seeds: %llu / %llu collide\n", (unsigned long long)c, (unsigned long long)N);
  return c != N;
}
