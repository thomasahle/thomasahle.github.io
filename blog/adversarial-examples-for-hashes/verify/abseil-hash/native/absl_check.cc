// absl_check.cc -- the blog post's absl::Hash pairs through the REAL abseil-cpp
// implementation, pinned at commit 73d2688300440c8af028eec865ee0dcd85e93025
// (2026-09-17; the same absl/hash/internal/hash.{h,cc} as LTS 20260817.0).
//
// Copyright (c) 2026 Thomas Dybdahl Ahle.  MIT License (full text in
// ../abseil_hash_verify.c).  Abseil itself is Copyright Google LLC, Apache-2.0;
// nothing of it is copied here, it is compiled from the pinned checkout.
//
// Build (../Makefile target `native` does this, including the pinned fetch):
//   g++ -O2 -std=c++17 -I abseil-cpp absl_check.cc \
//       abseil-cpp/absl/hash/internal/hash.cc abseil-cpp/absl/hash/internal/city.cc -o absl_check_scalar
//   g++ -O2 -std=c++17 -msse4.2 -maes -I abseil-cpp ... -o absl_check_aes      # x86 AES-NI LowLevelHash
// (Apple clang on arm64 builds the ARM-crypto LowLevelHash with no extra flag.)
//
// Modes:
//   absl_check vectors                 "len seed hash" lines for the shared schedule; the C program's
//                                      `--vectors` prints the same lines from its re-implementation
//   absl_check ctable                  the same values as C initializers (how the tables in
//                                      ../abseil_hash_verify.c were produced)
//   absl_check seed                    the process-seed protocol: Seed() is the address of
//                                      MixingHashState::kSeed, absl::Hash<T>{}(x) == hash_with_seed(x, Seed()),
//                                      and the three pairs under that seed
//   absl_check pairs [log2 N] [rng seed]   the three pairs under the 32 SwissTable seeds and 2^log2N
//                                      uniform 64-bit seeds through hash_internal::HashWithSeed, the
//                                      hook raw_hash_set uses; same RNG stream as the C program
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <string_view>
#include <vector>

#include "absl/hash/hash.h"
#include "absl/hash/internal/hash.h"

// MixingHashState::kSeed is private; Seed() returns its address (hash.h, Seed()).
// Alias the linker symbol.  HEAD has ABSL_OPTION_USE_INLINE_NAMESPACE 0; an LTS
// build would need the inline namespace in the mangled name (e.g.
// _ZN4absl12lts_2026081713hash_internal15MixingHashState5kSeedE).
#ifndef KSEED_SYM
#ifdef __APPLE__
#define KSEED_SYM "__ZN4absl13hash_internal15MixingHashState5kSeedE"
#else
#define KSEED_SYM "_ZN4absl13hash_internal15MixingHashState5kSeedE"
#endif
#endif
extern const void* const kSeedAlias __asm__(KSEED_SYM);
static uint64_t process_seed() { return (uint64_t)(uintptr_t)&kSeedAlias; }

static uint64_t real_hash(uint64_t seed, const uint8_t* p, size_t len) {
  std::string_view sv(reinterpret_cast<const char*>(p), len);
  return absl::hash_internal::HashWithSeed().hash(absl::Hash<std::string_view>{}, sv, (size_t)seed);
}

// ---- shared vector schedule (identical in ../abseil_hash_verify.c) ----
#define VEC_BUF 8192
static uint64_t splitmix64(uint64_t* s) {
  uint64_t z = (*s += 0x9E3779B97F4A7C15ull);
  z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull;
  z = (z ^ (z >> 27)) * 0x94D049BB133111EBull;
  return z ^ (z >> 31);
}
static void vec_fill(uint8_t* buf) {
  uint64_t s = 0x243F6A8885A308D3ull;
  for (size_t i = 0; i < VEC_BUF; i += 8) {
    uint64_t v = splitmix64(&s);
    for (int k = 0; k < 8; k++) buf[i + k] = (uint8_t)(v >> (8 * k));
  }
}
static const uint64_t vec_seeds[5] = {0, 1, 0x00007f3a1c5e2000ull, 0xdeadbeefcafef00dull, 0xffffffffffffffffull};
static const size_t vec_lens[] = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                                  21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
                                  48, 63, 64, 65, 96, 100, 127, 128, 129, 200, 255, 256, 511, 512, 1000, 1023,
                                  1024, 1025, 1100, 2048, 2049, 3073, 4097, 8192};
static const size_t vec_nlens = sizeof(vec_lens) / sizeof(vec_lens[0]);

// ---- the pairs (the post's rows: abseil-hash pair, non-empty variant, other_pair) ----
struct Pair { const char* name; const char* a; const char* b; };
static const Pair PAIRS[3] = {
    {"pair 1: '' (0 B) vs a6e02637c07bd386 (8 B)", "", "a6e02637c07bd386"},
    {"pair 2: 'a' (1 B) vs 44b53d572db1948b (8 B)", "61", "44b53d572db1948b"},
    {"pair 3: abcdefgh||kMul vs ABCDEFGH||kMul (16 B, both hash to 0)",
     "6162636465666768f58c1edee0f9d579", "4142434445464748f58c1edee0f9d579"}};

static std::vector<uint8_t> unhex(const char* s) {
  std::vector<uint8_t> v;
  for (size_t i = 0; s[i] && s[i + 1]; i += 2) { unsigned b; sscanf(s + i, "%2x", &b); v.push_back((uint8_t)b); }
  return v;
}

// ---- RNG: splitmix64-seeded xoshiro256**, as in every program of the package ----
static uint64_t rotl(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
struct Xo { uint64_t s[4]; };
static void xo_seed(Xo* x, uint64_t seed) { for (int i = 0; i < 4; i++) x->s[i] = splitmix64(&seed); }
static uint64_t xo_next(Xo* x) {
  uint64_t* s = x->s; uint64_t r = rotl(s[1] * 5, 7) * 9, t = s[1] << 17;
  s[2] ^= s[0]; s[3] ^= s[1]; s[1] ^= s[2]; s[0] ^= s[3]; s[2] ^= t; s[3] = rotl(s[3], 45); return r;
}

static const char* backend() {
#if defined(__SSE4_2__) && defined(__AES__)
  return "x86 AES-NI";
#elif defined(__ARM_NEON) && defined(__ARM_FEATURE_CRYPTO)
  return "ARM crypto";
#else
  return "scalar";
#endif
}

int main(int argc, char** argv) {
  const char* mode = argc > 1 ? argv[1] : "pairs";
  static uint8_t buf[VEC_BUF];
  vec_fill(buf);
  if (!strcmp(mode, "vectors") || !strcmp(mode, "ctable")) {
    bool ct = !strcmp(mode, "ctable");
    if (ct) printf("/* abseil-cpp 73d2688, real library, LowLevelHash backend for len > 32: %s */\n", backend());
    for (size_t i = 0; i < vec_nlens; i++)
      for (int s = 0; s < 5; s++) {
        uint64_t h = real_hash(vec_seeds[s], buf, vec_lens[i]);
        if (ct) printf("  {%zu, 0x%016llxull, 0x%016llxull},\n", vec_lens[i], (unsigned long long)vec_seeds[s], (unsigned long long)h);
        else printf("%zu %016llx %016llx\n", vec_lens[i], (unsigned long long)vec_seeds[s], (unsigned long long)h);
      }
    return 0;
  }
  printf("absl_check: real abseil-cpp 73d2688 through hash_internal::HashWithSeed (LowLevelHash backend for len > 32: %s)\n", backend());
  if (!strcmp(mode, "seed")) {
    uint64_t s = process_seed();
    printf("Seed() = 0x%llx = &MixingHashState::kSeed (an address: fixed in a non-PIE binary, ASLR-slid in a PIE one)\n", (unsigned long long)s);
    const char* xs[] = {"", "a", "hello world", "0123456789abcdefghij", "a key of more than sixty-four bytes so that the LowLevelHash loop path runs too..."};
    int bad = 0;
    for (const char* x : xs) {
      std::string str(x);
      uint64_t h1 = absl::Hash<std::string_view>{}(std::string_view(str)), h2 = absl::Hash<std::string>{}(str);
      uint64_t h3 = real_hash(s, (const uint8_t*)x, strlen(x));
      printf("  len %2zu: Hash<string_view>{}(x) = %016llx  Hash<string>{}(x) = %016llx  hash_with_seed(x, Seed()) = %016llx  %s\n",
             strlen(x), (unsigned long long)h1, (unsigned long long)h2, (unsigned long long)h3, (h1 == h2 && h1 == h3) ? "equal" : "DIFFER");
      if (h1 != h2 || h1 != h3) bad++;
    }
    int coll = 0;
    for (const Pair& p : PAIRS) {
      std::vector<uint8_t> a = unhex(p.a), b = unhex(p.b);
      uint64_t ha = absl::Hash<std::string_view>{}(std::string_view((const char*)a.data(), a.size()));
      uint64_t hb = absl::Hash<std::string_view>{}(std::string_view((const char*)b.data(), b.size()));
      printf("  %s under the process seed: %016llx %016llx %s\n", p.name, (unsigned long long)ha, (unsigned long long)hb, ha == hb ? "COLLIDE" : "differ");
      coll += (ha == hb);
    }
    printf("seed protocol: %d mismatches; %d of 3 pairs collide under the process seed\n", bad, coll);
    return (bad != 0 || coll != 3);
  }
  if (!strcmp(mode, "pairs")) {
    int lg = 20; uint64_t rseed = 1;
    if (argc > 2) { char* e; long v = strtol(argv[2], &e, 10); if (*e || v < 0 || v > 40) { fprintf(stderr, "usage: absl_check pairs [log2 N 0..40] [rng seed]\n"); return 2; } lg = (int)v; }
    if (argc > 3) { char* e; rseed = strtoull(argv[3], &e, 0); if (*e) { fprintf(stderr, "bad rng seed\n"); return 2; } }
    uint64_t N = 1ull << lg;
    printf("N = 2^%d uniform 64-bit seeds per pair (rng seed %llu), plus the 32 SwissTable seeds {0, 64, ..., 1984}\n", lg, (unsigned long long)rseed);
    int fail = 0;
    for (const Pair& p : PAIRS) {
      std::vector<uint8_t> a = unhex(p.a), b = unhex(p.b);
      printf("\n%s\n", p.name);
      for (uint64_t s : {0ull, 0x4055c8ull}) {
        uint64_t ha = real_hash(s, a.data(), a.size()), hb = real_hash(s, b.data(), b.size());
        printf("  seed 0x%llx: H(m) = %016llx  H(m') = %016llx  %s\n", (unsigned long long)s, (unsigned long long)ha, (unsigned long long)hb, ha == hb ? "COLLIDE" : "differ");
      }
      uint64_t tab = 0;
      for (uint64_t t = 0; t < 32; t++) tab += real_hash(t << 6, a.data(), a.size()) == real_hash(t << 6, b.data(), b.size());
      printf("  SwissTable seeds: collisions = %llu / 32\n", (unsigned long long)tab);
      Xo x; xo_seed(&x, rseed);
      uint64_t c = 0, fs = 0, fh = 0; bool have = false;
      for (uint64_t i = 0; i < N; i++) {
        uint64_t s = xo_next(&x);
        uint64_t ha = real_hash(s, a.data(), a.size()), hb = real_hash(s, b.data(), b.size());
        if (ha == hb) { c++; if (!have) { have = true; fs = s; fh = ha; } }
      }
      printf("  uniform seeds: collisions = %llu / %llu\n", (unsigned long long)c, (unsigned long long)N);
      if (have) printf("  first colliding seed 0x%016llx: H = %016llx\n", (unsigned long long)fs, (unsigned long long)fh);
      if (tab != 32 || c != N) fail = 1;
    }
    printf("\n%s\n", fail ? "FAILED: some seed did not collide" : "ALL PAIRS COLLIDE FOR EVERY SAMPLED SEED (real abseil-cpp 73d2688)");
    return fail;
  }
  fprintf(stderr, "usage: absl_check vectors|ctable|seed|pairs [log2 N] [rng seed]\n");
  return 2;
}
