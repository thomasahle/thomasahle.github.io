/* Independent check: the 16-byte CityHash64 / FarmHash64-NA family.
 * Links upstream google/cityhash city.cc and google/farmhash farmhash.cc unmodified.
 * Reads one 16-byte member (hex) per line; checks that all members have the same unseeded
 * CityHash64 and that CityHash64WithSeed(s), CityHash64WithSeeds(s0,s1), farmhashna::Hash64WithSeed(s)
 * and farmhashna::Hash64WithSeeds(s0,s1) agree across all members for N random seeds.
 * Build: c++ -O2 -std=c++17 -I. indep_city_farm.cc city.cc farmhash.cc -o indep_city_farm
 * Run:   ./indep_city_farm members.txt [log2 seeds, default 8] */
#include "city.h"
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>
#include <set>
namespace farmhashna {
uint64_t Hash64(const char *s, size_t len);
uint64_t Hash64WithSeed(const char *s, size_t len, uint64_t seed);
uint64_t Hash64WithSeeds(const char *s, size_t len, uint64_t seed0, uint64_t seed1);
}
static uint64_t st = 0x1dc0ffee;
static uint64_t rnd() { uint64_t z = (st += 0x9e3779b97f4a7c15ULL); z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL; z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL; return z ^ (z >> 31); }
int main(int argc, char **argv) {
  if (argc < 2) return 2;
  int lg = argc > 2 ? atoi(argv[2]) : 8;
  if (CityHash64("hello", 5) != 0xb48be5a931380ce8ULL) { printf("anchor FAIL\n"); return 1; }
  FILE *f = fopen(argv[1], "r"); char line[256]; std::vector<std::string> M; std::set<std::string> seen;
  while (fgets(line, sizeof line, f) && M.size() < 65536) {
    std::string h(line); while (!h.empty() && (h.back() == '\n' || h.back() == '\r')) h.pop_back();
    if (h.size() != 32 || seen.count(h)) continue; seen.insert(h);
    std::string b; for (int i = 0; i < 32; i += 2) b.push_back((char)strtol(h.substr(i, 2).c_str(), 0, 16)); M.push_back(b); }
  fclose(f);
  size_t n = M.size(); printf("members: %zu distinct 16-byte strings\n", n);
  uint64_t u0 = CityHash64(M[0].data(), 16), f0 = farmhashna::Hash64(M[0].data(), 16); size_t ueq = 0, feq = 0;
  for (auto &m : M) { ueq += CityHash64(m.data(), 16) == u0; feq += farmhashna::Hash64(m.data(), 16) == f0; }
  printf("unseeded CityHash64 = %016llx shared by %zu/%zu; unseeded farmhashna::Hash64 = %016llx shared by %zu/%zu\n",
         (unsigned long long)u0, ueq, n, (unsigned long long)f0, feq, n);
  uint64_t N = 1ULL << lg, ok[4] = {0, 0, 0, 0};
  for (uint64_t t = 0; t < N; t++) {
    uint64_t s = rnd(), s1 = rnd();
    uint64_t r0 = CityHash64WithSeed(M[0].data(), 16, s), r1 = CityHash64WithSeeds(M[0].data(), 16, s, s1),
             r2 = farmhashna::Hash64WithSeed(M[0].data(), 16, s), r3 = farmhashna::Hash64WithSeeds(M[0].data(), 16, s, s1);
    bool a0 = 1, a1 = 1, a2 = 1, a3 = 1;
    for (size_t i = 1; i < n; i++) { const char *p = M[i].data();
      a0 &= CityHash64WithSeed(p, 16, s) == r0; a1 &= CityHash64WithSeeds(p, 16, s, s1) == r1;
      a2 &= farmhashna::Hash64WithSeed(p, 16, s) == r2; a3 &= farmhashna::Hash64WithSeeds(p, 16, s, s1) == r3; }
    ok[0] += a0; ok[1] += a1; ok[2] += a2; ok[3] += a3; }
  printf("all %zu members equal: CityHash64WithSeed %llu/%llu, CityHash64WithSeeds %llu/%llu, farmhashna WithSeed %llu/%llu, farmhashna WithSeeds %llu/%llu seeds\n",
         n, (unsigned long long)ok[0], (unsigned long long)N, (unsigned long long)ok[1], (unsigned long long)N,
         (unsigned long long)ok[2], (unsigned long long)N, (unsigned long long)ok[3], (unsigned long long)N);
  bool pass = ueq == n && feq == n && ok[0] == N && ok[1] == N && ok[2] == N && ok[3] == N && n == 65536;
  printf("%s\n", pass ? "PASS" : "FAIL"); return !pass;
}
