// Real-library harness: absl::Hash<std::string_view> through hash_internal::HashWithSeed (the SwissTable hook).
// Modes:  vec              -> print hash vectors (len 0..32, several seeds) for validating a re-implementation
//         pairs <logN>     -> test pairs from stdin ("hexA hexB" per line) over 2^logN uniform 64-bit seeds
//         family <logN>    -> read one family (hex per line) from stdin, count keys where all members agree
#include "absl/hash/hash.h"
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
static uint64_t sm; static uint64_t spl() { uint64_t z = (sm += 0x9e3779b97f4a7c15ULL); z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL; z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL; return z ^ (z >> 31); }
static uint64_t H(const std::string &s, uint64_t seed) {
  return absl::hash_internal::HashWithSeed{}.hash(absl::Hash<std::string_view>{}, std::string_view(s), (size_t)seed); }
static std::string unhex(const std::string &h) { std::string r; for (size_t i = 0; i + 1 < h.size(); i += 2) r.push_back((char)std::stoi(h.substr(i, 2), nullptr, 16)); return r; }
int main(int argc, char **argv) {
  std::string mode = argc > 1 ? argv[1] : "vec"; unsigned logN = argc > 2 ? atoi(argv[2]) : 16; sm = 0x0f5577;
#ifdef ABSL_HASH_INTERNAL_HAS_CRC32
  fprintf(stderr, "build: CRC32 path (ABSL_HASH_INTERNAL_HAS_CRC32 defined)\n");
#else
  fprintf(stderr, "build: default path (no CRC32)\n");
#endif
  if (mode == "vec") {
    uint64_t seeds[4] = {0, 1, 0x0123456789abcdefULL, 0xfedcba9876543210ULL};
    for (int L = 0; L <= 32; L++) { std::string s; for (int i = 0; i < L; i++) s.push_back((char)(i * 37 + 11));
      for (uint64_t sd : seeds) printf("%d %016llx %016llx\n", L, (unsigned long long)sd, (unsigned long long)H(s, sd)); }
    return 0; }
  std::string line; std::vector<std::string> lines; while (std::getline(std::cin, line)) if (!line.empty()) lines.push_back(line);
  uint64_t N = 1ULL << logN;
  if (mode == "pairs") {
    for (auto &l : lines) { size_t sp = l.find(' '); std::string a = unhex(l.substr(0, sp)), b = unhex(l.substr(sp + 1));
      if (l.substr(0, sp) == "-") a = "";
      uint64_t c = 0, ex = 0, eo = 0; for (uint64_t t = 0; t < N; t++) { uint64_t s = spl(); uint64_t x = H(a, s), y = H(b, s); if (x == y) { if (!c) { ex = s; eo = x; } c++; } }
      uint64_t sw = 0; for (uint64_t s = 0; s < 2048; s += 64) sw += H(a, s) == H(b, s);
      printf("pair %s | %s : %llu/%llu uniform seeds, %llu/32 SwissTable seeds; ex seed %016llx -> %016llx\n", l.substr(0, sp).c_str(), l.substr(sp + 1).c_str(),
             (unsigned long long)c, (unsigned long long)N, (unsigned long long)sw, (unsigned long long)ex, (unsigned long long)eo); }
    return 0; }
  if (mode == "family") {
    std::vector<std::string> S; for (auto &l : lines) S.push_back(unhex(l));
    uint64_t all = 0; for (uint64_t t = 0; t < N; t++) { uint64_t s = spl(), h0 = H(S[0], s); bool ok = true;
      for (size_t i = 1; i < S.size() && ok; i++) ok = H(S[i], s) == h0; all += ok; }
    uint64_t sw = 0; for (uint64_t s = 0; s < 2048; s += 64) { uint64_t h0 = H(S[0], s); bool ok = true; for (size_t i = 1; i < S.size() && ok; i++) ok = H(S[i], s) == h0; sw += ok; }
    printf("family of %zu (len %zu): all-equal on %llu/%llu uniform seeds, %llu/32 SwissTable seeds\n", S.size(), S[0].size(), (unsigned long long)all, (unsigned long long)N, (unsigned long long)sw);
    return 0; }
}
