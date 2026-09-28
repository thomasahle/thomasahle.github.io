/* Check "hexA hexB" pairs (one per stdin line) against the real abseil-cpp absl::Hash<std::string_view>
 * over 2^k random seeds and the 32 SwissTable seeds.  Build: c++ -O2 -std=c++17 -I<abseil> indep_absl_pairs.cc
 * <abseil>/absl/hash/internal/{hash,city}.cc -o indep_absl_pairs ; run: ./indep_absl_pairs [k] < pairs.txt */
#include "absl/hash/hash.h"
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
static uint64_t st = 0xab5e12;
static uint64_t rnd() { uint64_t z = (st += 0x9e3779b97f4a7c15ULL); z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL; z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL; return z ^ (z >> 31); }
static uint64_t H(const std::string &s, uint64_t seed) { return absl::hash_internal::HashWithSeed{}.hash(absl::Hash<std::string_view>{}, std::string_view(s), (size_t)seed); }
static std::string unhex(const std::string &h) { std::string r; for (size_t i = 0; i + 1 < h.size(); i += 2) r.push_back((char)std::stoi(h.substr(i, 2), nullptr, 16)); return r; }
int main(int argc, char **argv) { int lg = argc > 1 ? atoi(argv[1]) : 16; uint64_t N = 1ULL << lg; std::string x, y;
  while (std::cin >> x >> y) { std::string a = unhex(x), b = unhex(y); uint64_t c = 0, t32 = 0;
    for (uint64_t t = 0; t < N; t++) { uint64_t s = rnd(); c += H(a, s) == H(b, s); }
    for (uint64_t s = 0; s < 2048; s += 64) t32 += H(a, s) == H(b, s);
    printf("pair |m|=%zu |m'|=%zu: equal on %llu/%llu random seeds, %llu/32 SwissTable seeds\n", a.size(), b.size(), (unsigned long long)c, (unsigned long long)N, (unsigned long long)t32); } }
