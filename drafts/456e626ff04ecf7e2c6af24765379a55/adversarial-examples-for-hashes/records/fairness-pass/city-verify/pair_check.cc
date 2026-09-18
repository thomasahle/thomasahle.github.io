// Independent check of the blog's CityHash64 pairs against upstream google/cityhash src/city.cc.
#include "city.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <string>
#include <vector>

static uint64_t sm_state;
static uint64_t sm64() { uint64_t z = (sm_state += 0x9e3779b97f4a7c15ULL); z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL; z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL; return z ^ (z >> 31); }

static std::string unhex(const char* h) { std::string s; for (size_t i = 0; h[i] && h[i+1]; i += 2) { unsigned b; sscanf(h + i, "%2x", &b); s.push_back((char)b); } return s; }
static void phex(const std::string& s) { for (unsigned char c : s) printf("%02x", c); }

static uint32_t smhasher3_verification() {
  // SMHasher3 lib/Hashinfo.cpp _ComputedVerifyImpl: key i has bytes 0..i-1, seed 256-i, hash stored LE; then hash of the 2048 bytes with seed 0; first 4 bytes LE.
  unsigned char key[256], hashes[256 * 8];
  for (int i = 0; i < 256; i++) {
    for (int j = 0; j < i; j++) key[j] = (unsigned char)j;
    uint64 h = CityHash64WithSeed((const char*)key, i, (uint64)(256 - i));
    for (int b = 0; b < 8; b++) hashes[i * 8 + b] = (unsigned char)(h >> (8 * b));
  }
  uint64 f = CityHash64WithSeed((const char*)hashes, sizeof hashes, 0);
  return (uint32_t)(f & 0xffffffffULL);
}

static void check_pair(const char* name, const char* ah, const char* bh, uint64_t N, uint64_t ex_seed) {
  std::string a = unhex(ah), b = unhex(bh);
  printf("%s: len %zu/%zu bytes\n  m1 = ", name, a.size(), b.size()); phex(a); printf("\n  m2 = "); phex(b); printf("\n");
  uint64 ua = CityHash64(a.data(), a.size()), ub = CityHash64(b.data(), b.size());
  printf("  unseeded CityHash64: %016llx %016llx %s\n", (unsigned long long)ua, (unsigned long long)ub, ua == ub ? "EQUAL" : "DIFFER");
  uint64 seeds[3] = {0, 1, ex_seed};
  for (uint64 s : seeds) { uint64 x = CityHash64WithSeed(a.data(), a.size(), s), y = CityHash64WithSeed(b.data(), b.size(), s);
    printf("  seed %016llx: %016llx %016llx %s\n", (unsigned long long)s, (unsigned long long)x, (unsigned long long)y, x == y ? "COLLIDE" : "differ"); }
  uint64_t c1 = 0, c2 = 0;
  for (uint64_t i = 0; i < N; i++) { uint64 s = sm64(); if (CityHash64WithSeed(a.data(), a.size(), s) == CityHash64WithSeed(b.data(), b.size(), s)) c1++; }
  for (uint64_t i = 0; i < N; i++) { uint64 s0 = sm64(), s1 = sm64(); if (CityHash64WithSeeds(a.data(), a.size(), s0, s1) == CityHash64WithSeeds(b.data(), b.size(), s0, s1)) c2++; }
  printf("  random 64-bit seeds (WithSeed): %llu / %llu collide\n", (unsigned long long)c1, (unsigned long long)N);
  printf("  random (seed0,seed1) (WithSeeds): %llu / %llu collide\n", (unsigned long long)c2, (unsigned long long)N);
}

int main(int argc, char** argv) {
  uint64_t N = argc > 1 ? strtoull(argv[1], 0, 0) : (1ULL << 20);
  sm_state = argc > 2 ? strtoull(argv[2], 0, 0) : 0x1234567ULL;
  uint32_t v = smhasher3_verification();
  printf("SMHasher3 verification value (LE): 0x%08X (expected 0x5FABC5C5) %s\n", v, v == 0x5FABC5C5u ? "OK" : "MISMATCH");
  check_pair("pair A", "a01109025ea76be1", "020bd424b04ae555", N, 0x6637c1ce6357a2c8ULL);
  check_pair("pair B", "436974794861736836342d6f6b21212183454502d40dfa393030303030303030", "436974794861736836342d6f6b21212183ff1a7c2ab06b6a3030303030303031", N, 0xcbd18ebcd1f9b00dULL);
  check_pair("control (pair A m1 vs last byte 0xe0)", "a01109025ea76be1", "a01109025ea76be0", N, 0x6637c1ce6357a2c8ULL);
  // Optional: Peters' strings from a file (one hex string per line), report unseeded hashes.
  if (argc > 3) { FILE* f = fopen(argv[3], "r"); char line[4096]; printf("extra strings from %s (unseeded CityHash64 / WithSeed 0 / WithSeed 1):\n", argv[3]);
    while (f && fgets(line, sizeof line, f)) { size_t n = strcspn(line, "\r\n"); line[n] = 0; if (!n) continue; std::string s = unhex(line);
      printf("  len %2zu %s -> %016llx %016llx %016llx\n", s.size(), line, (unsigned long long)CityHash64(s.data(), s.size()), (unsigned long long)CityHash64WithSeed(s.data(), s.size(), 0), (unsigned long long)CityHash64WithSeed(s.data(), s.size(), 1)); } }
  return 0;
}
