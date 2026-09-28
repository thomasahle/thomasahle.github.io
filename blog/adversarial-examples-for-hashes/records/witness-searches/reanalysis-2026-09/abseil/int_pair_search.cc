// int_pair_search.cc -- find 64-bit integers v != v' that collide under
// absl::Hash<uint64_t> for every seed when abseil-cpp is built with
// ABSL_OPTION_INLINE_HW_ACCEL_STRATEGY != 0 and CRC32C available.
//
// In that build the integer fast path is CombineRawImpl (absl/hash/internal/hash.h):
//   high32 = crc32c_u64(high32(seed), v),  low32 = crc32c_u64(low32(seed), 3*v)
// and the hash is high32 << 32 | low32, with no further mixing.  CRC32C is
// GF(2)-affine in the data with the seed only in the initial value, so
//   H(v) == H(v') for every seed  <=>  C(v ^ v') == 0 and C(3v ^ 3v') == 0,
// where C(d) = crc32c_u64(0, d) is GF(2)-linear.  The first condition says
// v' = v ^ k with k in the 32-dimensional kernel K of C; this program walks
// all 2^32 elements of K (Gray code) for a few starting values v and checks
// the second condition with the hardware instruction.  Expected hits: about
// one per starting value.
//
// Stand-alone (no Abseil needed); the pairs it prints are checked against the
// real library by crc_collide.cc.
// Build: c++ -O2 -std=c++17 -msse4.2 int_pair_search.cc -o int_pair_search -pthread
//        (arm64: -march=armv8-a+crc)
// Usage: ./int_pair_search [threads=8] [starts=8]
// Copyright (c) 2026 Thomas Dybdahl Ahle.  MIT License.
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <mutex>
#include <thread>
#include <vector>
#if defined(__x86_64__)
#include <nmmintrin.h>
static inline uint32_t C(uint64_t d) { return (uint32_t)_mm_crc32_u64(0, d); }
#elif defined(__ARM_FEATURE_CRC32)
#include <arm_acle.h>
static inline uint32_t C(uint64_t d) { return __crc32cd(0, d); }
#else
#error "needs SSE4.2 or ARM CRC32"
#endif

static uint64_t splitmix(uint64_t& s) {
  uint64_t z = (s += 0x9e3779b97f4a7c15ull);
  z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ull;
  z = (z ^ (z >> 27)) * 0x94d049bb133111ebull;
  return z ^ (z >> 31);
}

// Basis of K = ker C (32 vectors) by elimination on the 64 unit vectors.
static std::vector<uint64_t> kernel_basis() {
  std::vector<std::pair<uint32_t, uint64_t>> piv;  // (image, preimage), reduced
  std::vector<uint64_t> ker;
  for (int i = 0; i < 64; i++) {
    uint32_t img = C(1ull << i);
    uint64_t pre = 1ull << i;
    for (auto& [pi, pp] : piv)
      if (img & (pi & -pi)) { img ^= pi; pre ^= pp; }
    if (img) {
      // keep basis reduced: eliminate the new pivot bit from older rows
      for (auto& [pi, pp] : piv)
        if (pi & (img & -img)) { pi ^= img; pp ^= pre; }
      piv.push_back({img, pre});
    } else {
      ker.push_back(pre);
    }
  }
  return ker;
}

int main(int argc, char** argv) {
  int threads = argc > 1 ? atoi(argv[1]) : 8;
  int starts = argc > 2 ? atoi(argv[2]) : 8;
  std::vector<uint64_t> K = kernel_basis();
  printf("dim ker C = %zu\n", K.size());
  for (uint64_t k : K)
    if (C(k)) { printf("bad kernel vector\n"); return 1; }
  std::mutex mu;
  std::vector<std::thread> pool;
  for (int t = 0; t < threads; t++)
    pool.emplace_back([&, t] {
      for (int s = t; s < starts; s += threads) {
        uint64_t rs = 0x1234abcd00000000ull + (uint64_t)s;
        uint64_t v0 = splitmix(rs);
        uint64_t k = 0, t3 = 3 * v0;
        long hits = 0;
        for (uint64_t g = 1; g < (1ull << K.size()); g++) {
          k ^= K[__builtin_ctzll(g)];
          uint64_t v = v0 ^ k;
          if (C(3 * v ^ t3) == 0) {
            hits++;
            std::lock_guard<std::mutex> lk(mu);
            printf("start %d: v = 0x%016llx  v' = 0x%016llx  (v^v' = 0x%016llx)\n", s,
                   (unsigned long long)v0, (unsigned long long)v, (unsigned long long)k);
            fflush(stdout);
          }
        }
        std::lock_guard<std::mutex> lk(mu);
        printf("start %d done: %ld partner(s)\n", s, hits);
        fflush(stdout);
      }
    });
  for (auto& th : pool) th.join();
  return 0;
}
