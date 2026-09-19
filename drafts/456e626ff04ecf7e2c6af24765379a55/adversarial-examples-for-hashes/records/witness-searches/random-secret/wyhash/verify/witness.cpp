// witness: recompute wyhash of two hex messages under an explicit (seed, secret[4]).
// build: g++ -O2 -std=c++17 -o witness witness.cpp
// usage: ./witness hexA hexB seed s0 s1 s2 s3   (all hex)
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include "wyhash_upstream.h"
static std::vector<uint8_t> hex(const char* s) { std::vector<uint8_t> v; for (size_t i = 0; i < strlen(s); i += 2) v.push_back((uint8_t)strtoul(std::string(s + i, 2).c_str(), 0, 16)); return v; }
int main(int argc, char** argv) {
    if (argc != 8) { fprintf(stderr, "usage: witness hexA hexB seed s0 s1 s2 s3\n"); return 2; }
    auto A = hex(argv[1]), B = hex(argv[2]); uint64_t seed = strtoull(argv[3], 0, 16), s[4];
    for (int i = 0; i < 4; i++) s[i] = strtoull(argv[4 + i], 0, 16);
    uint64_t h1 = wyhash(A.data(), A.size(), seed, s), h2 = wyhash(B.data(), B.size(), seed, s);
    printf("h(A)=%016llx h(B)=%016llx %s\n", (unsigned long long)h1, (unsigned long long)h2, h1 == h2 ? "COLLIDE" : "differ");
    return h1 == h2 ? 0 : 1;
}
