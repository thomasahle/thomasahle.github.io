/* Independent check of three short every-seed flooding families, each regenerated here from the algebra
 * and checked on the unmodified reference source:
 *  - MuseAir v0.3 (SMHasher3 hashes/museair.cpp, the version SMHasher3 measures), 18 bytes:
 *    short path state (i,j) = read16(head) ^ tail_term(tail) ^ seed_term(seed, len), read16 a bijection of the
 *    first 16 bytes, so head = read16^-1(I ^ tail_term(tail)) for each 2-byte tail gives 2^16 members.
 *  - a5hash-128 (avaneev/a5hash a5hash.h), 18 bytes: on the 17..32-byte path c = bytes [len-16, len-8) read as
 *    lu32<<32|lu32; c + Seed3 = 0 zeroes the product with d = the last 8 bytes; bytes 16..17 occur only in d.
 *  - MurmurHash3_x64_128 (aappleby/smhasher MurmurHash3.cpp), 144 bytes: per 16-byte block, top-bit XOR
 *    differences 2^36 (k1 side) and 2^32 (k2 side) rotate onto bit 63; 4 choices per block, 8 free blocks and one
 *    closing block give 4^8 = 2^16 members with identical internal state.
 * Build: c++ -O2 -std=c++17 -I. -Ismh3shim indep_museair_a5_murmur.cc -o indep_museair_a5_murmur
 * Run:   ./indep_museair_a5_murmur [log2 seeds, default 8] */
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <set>
#include "museair_smhasher3.cpp"
#include "a5hash.h"
#include "MurmurHash3.h"
static uint64_t st = 0xa11ce5eed;
static uint64_t rnd() { uint64_t z = (st += 0x9e3779b97f4a7c15ULL); z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL; z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL; return z ^ (z >> 31); }
typedef void (*HF)(const void *, size_t, uint64_t, void *);
static void h_muse(const void *m, size_t n, uint64_t s, void *o) { MuseAirHash<false, false, false>(m, n, s, o); }
static void h_museb(const void *m, size_t n, uint64_t s, void *o) { MuseAirHash<false, true, false>(m, n, s, o); }
static void h_muse128(const void *m, size_t n, uint64_t s, void *o) { MuseAirHash<false, false, true>(m, n, s, o); }
static void h_museb128(const void *m, size_t n, uint64_t s, void *o) { MuseAirHash<false, true, true>(m, n, s, o); }
static void h_a5(const void *m, size_t n, uint64_t s, void *o) { uint64_t hi; uint64_t lo = a5hash128(m, n, s, &hi); memcpy(o, &lo, 8); memcpy((char *)o + 8, &hi, 8); }
static void h_mm3(const void *m, size_t n, uint64_t s, void *o) { MurmurHash3_x64_128(m, (int)n, (uint32_t)s, o); }
/* SMHasher3 verification value (lib/Hashinfo.cpp): hash key[0..i) with seed 256-i, then hash the concatenation with seed 0 */
static uint32_t smh_verify(HF h, int bytes) { uint8_t key[256], hs[256 * 16], fin[16];
  for (int i = 0; i < 256; i++) { key[i] = (uint8_t)i; uint8_t o[16]; h(key, i, 256 - i, o); memcpy(hs + i * bytes, o, bytes); }
  h(hs, 256 * bytes, 0, fin); return fin[0] | fin[1] << 8 | fin[2] << 16 | (uint32_t)fin[3] << 24; }
static int fails = 0;
static void family(const char *name, HF h, int ob, bool seed32, const std::vector<std::vector<uint8_t>> &S, int lg) {
  std::set<std::vector<uint8_t>> d(S.begin(), S.end()); uint64_t N = 1ULL << lg, ok = 0; uint8_t o0[16], o[16];
  for (uint64_t t = 0; t <= N; t++) { uint64_t s = t == N ? 0 : rnd(); if (seed32) s &= 0xffffffffULL;
    h(S[0].data(), S[0].size(), s, o0); bool a = true;
    for (size_t i = 1; i < S.size() && a; i++) { h(S[i].data(), S[i].size(), s, o); a = !memcmp(o, o0, ob); } ok += a; }
  printf("%-34s %zu members (%zu distinct), length %zu: all equal on the full %d-bit output for %llu/%llu seeds (random + seed 0)\n",
         name, S.size(), d.size(), S[0].size(), ob * 8, (unsigned long long)ok, (unsigned long long)(N + 1));
  printf("   member[0] = "); for (auto b : S[0]) printf("%02x", b); printf("\n   member[1] = "); for (auto b : S[1]) printf("%02x", b); printf("\n");
  if (ok != N + 1 || d.size() != S.size() || S.size() != 65536) fails++; }
static void put32(uint8_t *p, uint32_t v) { memcpy(p, &v, 4); }
int main(int argc, char **argv) {
  int lg = argc > 1 ? atoi(argv[1]) : 8;
  struct { const char *n; HF h; int b; uint32_t want; } V[] = { {"MuseAir", h_muse, 8, 0xF89F1683}, {"MuseAir__bfast", h_museb, 8, 0xC61BEE56},
    {"MuseAir_128", h_muse128, 16, 0xD3DFE238}, {"MuseAir_128__bfast", h_museb128, 16, 0x27939BF1}, {"MurmurHash3_x64_128", h_mm3, 16, 0x6384BA69} };
  for (auto &v : V) { uint32_t got = smh_verify(v.h, v.b); printf("verification %-20s %08X (SMHasher3 %08X) %s\n", v.n, got, v.want, got == v.want ? "OK" : "MISMATCH"); if (got != v.want) fails++; }
  printf("a5hash.h version %s\n", A5HASH_VER_STR);
  /* MuseAir 18-byte family */
  { const size_t len = 18; uint64_t I = rnd(), J = rnd(); std::vector<std::vector<uint8_t>> S;
    for (uint32_t t = 0; t < 65536; t++) { std::vector<uint8_t> m(len, 0); m[16] = t; m[17] = t >> 8;
      uint64_t u, v, lo0, hi0, lo1, hi1; museair_read_short<false>(m.data() + 16, 2, &u, &v);
      MathMult::mult64_128(lo0, hi0, MUSEAIR_CONSTANT[2], MUSEAIR_CONSTANT[3] ^ u);
      MathMult::mult64_128(lo1, hi1, MUSEAIR_CONSTANT[4], MUSEAIR_CONSTANT[5] ^ v);
      uint64_t i = I ^ lo0 ^ hi1, j = J ^ lo1 ^ hi0; /* invert read16: i = lu32(0)<<32 | lu32(12), j = lu32(4)<<32 | lu32(8) */
      put32(&m[0], i >> 32); put32(&m[12], (uint32_t)i); put32(&m[4], j >> 32); put32(&m[8], (uint32_t)j);
      S.push_back(m); }
    family("MuseAir v0.3", h_muse, 8, false, S, lg); family("MuseAir v0.3 bfast", h_museb, 8, false, S, lg);
    family("MuseAir v0.3 128", h_muse128, 16, false, S, lg); family("MuseAir v0.3 bfast 128", h_museb128, 16, false, S, lg); }
  /* a5hash-128 18-byte family */
  { const size_t len = 18; std::vector<uint8_t> base(len); for (auto &x : base) x = rnd();
    uint64_t c = 0 - 0xA4093822299F31D0ULL; put32(&base[len - 16], c >> 32); put32(&base[len - 12], (uint32_t)c);
    std::vector<std::vector<uint8_t>> S; for (uint32_t t = 0; t < 65536; t++) { auto m = base; m[16] = t; m[17] = t >> 8; S.push_back(m); }
    family("a5hash-128", h_a5, 16, false, S, lg); }
  /* MurmurHash3_x64_128 144-byte family */
  { const uint64_t c1 = 0x87c37b91114253d5ULL, c2 = 0x4cf5ad432745937fULL, T = 1ULL << 63;
    auto rl = [](uint64_t x, int r) { return (x << r) | (x >> (64 - r)); };
    auto inv = [](uint64_t a) { uint64_t x = a; for (int i = 0; i < 6; i++) x *= 2 - a * x; return x; };
    auto g1 = [&](uint64_t k) { return rl(k * c1, 31) * c2; }; auto g2 = [&](uint64_t k) { return rl(k * c2, 33) * c1; };
    auto g1i = [&](uint64_t y) { return rl(y * inv(c2), 33) * inv(c1); }; auto g2i = [&](uint64_t y) { return rl(y * inv(c1), 31) * inv(c2); };
    const int nfree = 8, nblk = nfree + 1; std::vector<uint64_t> K1(nblk), K2(nblk); for (int i = 0; i < nblk; i++) { K1[i] = rnd(); K2[i] = rnd(); }
    std::vector<std::vector<uint8_t>> S;
    for (uint32_t idx = 0; idx < 65536; idx++) { std::vector<uint8_t> m(16 * nblk); int a = 0, b = 0; /* incoming top-bit differences on h1, h2 */
      for (int i = 0; i < nblk; i++) { int r1, r2; if (i < nfree) { r1 = idx >> (2 * i) & 1; r2 = idx >> (2 * i + 1) & 1; } else { r1 = b; r2 = 0; }
        /* h1 ^= g1(k1) with difference a*2^63 ^ r1*2^36: after rotl 27 it is r1*2^63; h1 += h2 gives (r1^b)*2^63 */
        uint64_t k1 = g1i(g1(K1[i]) ^ (a ? T : 0) ^ (r1 ? 1ULL << 36 : 0)), k2 = g2i(g2(K2[i]) ^ (b ? T : 0) ^ (r2 ? 1ULL << 32 : 0));
        memcpy(&m[16 * i], &k1, 8); memcpy(&m[16 * i + 8], &k2, 8); a = r1 ^ b; b = r2 ^ a; }
      if (a || b) { printf("internal error\n"); return 2; } S.push_back(m); }
    family("MurmurHash3_x64_128 (uint32 seed)", h_mm3, 16, true, S, lg); }
  printf("%s\n", fails ? "FAIL" : "PASS"); return fails != 0;
}
