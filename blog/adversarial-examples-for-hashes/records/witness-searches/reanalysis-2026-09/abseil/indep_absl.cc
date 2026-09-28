/* Independent check against the real abseil-cpp library (absl::Hash<std::string_view> through
 * absl::hash_internal::HashWithSeed, the hook raw_hash_set uses).
 *  (1) default build (ABSL_OPTION_INLINE_HW_ACCEL_STRATEGY 0): the 10-byte family "any 2 bytes, then the
 *      8 little-endian bytes of kMul" (f58c1edee0f9d579).  The 9..16-byte path returns
 *      Mix(seed ^ D(n) ^ load64(m), kMul ^ load64(m+n-8)) and Mix(x, 0) = 0, so every member hashes to 0.
 *  (2) HW-accelerated build (strategy 1 or 2 with CRC32C): the 9-byte pair "ABCDEFGHI" vs 41b335a840474748 49.
 * Build (default):   c++ -O2 -std=c++17 -I<abseil> indep_absl.cc <abseil>/absl/hash/internal/{hash,city}.cc -o indep_absl
 * Build (HW accel):  same with a copy of abseil whose absl/base/options.h sets ABSL_OPTION_INLINE_HW_ACCEL_STRATEGY 1,
 *                    plus -march=armv8-a+crc (arm64) or -msse4.2 (x86-64)
 * Run: ./indep_absl [log2 seeds, default 12] */
#include "absl/hash/hash.h"
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <string_view>
#include <vector>
static uint64_t st = 0xab5e11;
static uint64_t rnd() { uint64_t z = (st += 0x9e3779b97f4a7c15ULL); z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL; z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL; return z ^ (z >> 31); }
static uint64_t H(const std::string &s, uint64_t seed) {
  return absl::hash_internal::HashWithSeed{}.hash(absl::Hash<std::string_view>{}, std::string_view(s), (size_t)seed); }
int main(int argc, char **argv) {
  int lg = argc > 1 ? atoi(argv[1]) : 12; uint64_t N = 1ULL << lg;
  printf("ABSL_OPTION_INLINE_HW_ACCEL_STRATEGY = %d\n", ABSL_OPTION_INLINE_HW_ACCEL_STRATEGY);
  const unsigned char kmul[8] = {0xf5, 0x8c, 0x1e, 0xde, 0xe0, 0xf9, 0xd5, 0x79};
  std::vector<std::string> F; for (int t = 0; t < 65536; t++) { std::string s(10, 0); s[0] = (char)t; s[1] = (char)(t >> 8); for (int i = 0; i < 8; i++) s[2 + i] = (char)kmul[i]; F.push_back(s); }
  uint64_t all = 0, zero = 0, tbl = 0;
  for (uint64_t t = 0; t < N; t++) { uint64_t s = rnd(), h0 = H(F[0], s); bool ok = true; for (size_t i = 1; i < F.size() && ok; i++) ok = H(F[i], s) == h0; all += ok; zero += ok && h0 == 0; }
  for (uint64_t s = 0; s < 2048; s += 64) { uint64_t h0 = H(F[0], s); bool ok = true; for (size_t i = 1; i < F.size() && ok; i++) ok = H(F[i], s) == h0; tbl += ok; }
  printf("(1) 10-byte family (2 free bytes + kMul), 65536 members: all equal on %llu/%llu random seeds (common value 0 on %llu), %llu/32 SwissTable seeds\n",
         (unsigned long long)all, (unsigned long long)N, (unsigned long long)zero, (unsigned long long)tbl);
  std::string a = "ABCDEFGHI", b = "\x41\xb3\x35\xa8\x40\x47\x47\x48\x49"; uint64_t pc = 0;
  for (uint64_t t = 0; t < N; t++) { uint64_t s = rnd(); pc += H(a, s) == H(b, s); }
  printf("(2) 9-byte pair ABCDEFGHI / 41b335a84047474849: equal on %llu/%llu random seeds; seed 0 -> %016llx / %016llx; seed 1 -> %016llx / %016llx\n",
         (unsigned long long)pc, (unsigned long long)N, (unsigned long long)H(a, 0), (unsigned long long)H(b, 0), (unsigned long long)H(a, 1), (unsigned long long)H(b, 1));
  return 0;
}
