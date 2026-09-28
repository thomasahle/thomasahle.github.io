// absl_long.cc -- absl::Hash (abseil-cpp 73d2688 = LTS 20260817.0) on inputs > 64 bytes: LowLevelHashLenGt64.
// Links the REAL library sources (absl/hash/internal/hash.cc + city.cc); hashes through
// absl::hash_internal::HashWithSeed().hash(absl::Hash<std::string_view>{}, s, seed), the hook raw_hash_set uses.
// Build (one binary per compile-time variant; the variant is chosen by the same macros hash.cc tests):
//   scalar  : c++ -O2 -std=c++17 -I<abseil> absl_long.cc <abseil>/absl/hash/internal/{hash,city}.cc      (x86-64 default)
//   x86 AES : add -msse4.2 -maes
//   ARM AES : default flags on Apple silicon (__ARM_FEATURE_CRYPTO), or -march=armv8-a+crypto elsewhere
// Usage: ./absl_long [log2 seeds] [rng seed]
//
// Mechanisms (own analysis):
//  scalar : cs = Mix(a ^ K1, b ^ cs), ds0 = Mix(c ^ K2, d ^ ds0), ds1 = Mix(e ^ K3, ...), ds2 = Mix(g ^ K4, ...).
//           A 64-byte "reset chunk" (a,c,e,g) = (K1,K2,K3,K4) sets all four lanes to Mix(0, .) = 0: the seed,
//           the length and everything before the chunk are erased.  Works at every loop chunk of every 1024-byte piece.
//  ARM AES: lane update s <- InvMC(InvSB(InvSR(m ^ s))) (MixA/MixB), MC(SB(SR(m ^ s))) (MixC/MixD), no feed-forward.
//           One S-box difference delta -> gamma in chunk i is cancelled by XORing Delta = InvMC(gamma) into the same
//           lane's word of chunk i+1: probability DDT(delta, gamma)/256 = 2^-6 at ANY chunk.  In chunk 0 the state is
//           Set128(seed, len): its low 8 bytes are the public length, so differences there are deterministic (p = 1).
//  x86 AES: s <- aesdec(s + m, s) (feed-forward); the same idea costs an S-box on both steps plus 2^-9 for cancelling
//           Delta additively: 2^-16 in chunk 0 (first step public), 2^-23 elsewhere.
#include "absl/hash/hash.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cmath>
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>

#if defined(__SSE4_2__) && defined(__AES__)
#define VARIANT 1
static const char *VNAME = "x86 AES (LowLevelHash with aesenc/aesdec)";
#elif defined(__ARM_NEON) && defined(__ARM_FEATURE_CRYPTO)
#define VARIANT 2
static const char *VNAME = "ARM AES (LowLevelHash with AESD/AESIMC)";
#else
#define VARIANT 0
static const char *VNAME = "scalar (LowLevelHash with Mix32Bytes)";
#endif

static uint64_t H(const uint8_t *p, size_t n, uint64_t seed) {
  return absl::hash_internal::HashWithSeed().hash(absl::Hash<std::string_view>{}, std::string_view((const char *)p, n), seed);
}
// ---- RNG: splitmix64 -> xoshiro256** ----
static uint64_t SMS, XS[4];
static uint64_t sm64() { uint64_t z = (SMS += 0x9E3779B97F4A7C15ull); z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull; z = (z ^ (z >> 27)) * 0x94D049BB133111EBull; return z ^ (z >> 31); }
static inline uint64_t rl(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
static uint64_t rng64() { uint64_t r = rl(XS[1] * 5, 7) * 9, t = XS[1] << 17; XS[2] ^= XS[0]; XS[3] ^= XS[1]; XS[1] ^= XS[2]; XS[0] ^= XS[3]; XS[2] ^= t; XS[3] = rl(XS[3], 45); return r; }
static void rng_init(uint64_t s) { SMS = s; for (int i = 0; i < 4; i++) XS[i] = sm64(); }
static void fill(uint8_t *p, size_t n) { for (size_t i = 0; i < n; i++) p[i] = (uint8_t)rng64(); }
static std::string hx(const uint8_t *p, size_t n) { std::string s; char b[3]; for (size_t i = 0; i < n; i++) { snprintf(b, 3, "%02x", p[i]); s += b; } return s; }
static void ci95(uint64_t k, uint64_t n, double *lo, double *hi) {  // Wilson interval (k > 0), exact edge cases
  if (k == 0) { *lo = 0; *hi = 1 - pow(0.025, 1.0 / n); return; }
  if (k == n) { *lo = pow(0.025, 1.0 / n); *hi = 1; return; }
  double z = 1.959964, p = (double)k / n, d = 1 + z * z / n, c = (p + z * z / (2 * n)) / d, h = z * sqrt(p * (1 - p) / n + z * z / (4.0 * n * n)) / d;
  *lo = c - h; *hi = c + h;
}
static void st64(uint8_t *p, uint64_t v) { for (int i = 0; i < 8; i++) p[i] = (uint8_t)(v >> (8 * i)); }
static uint64_t ld64(const uint8_t *p) { uint64_t v = 0; for (int i = 7; i >= 0; i--) v = (v << 8) | p[i]; return v; }

// ---- own AES tables (byte i of a block = row i%4, column i/4) ----
static uint8_t S[256], IS[256];
static uint8_t xt(uint8_t a) { return (uint8_t)((a << 1) ^ ((a & 0x80) ? 0x1b : 0)); }
static uint8_t gm(uint8_t a, uint8_t b) { uint8_t r = 0; while (b) { if (b & 1) r ^= a; a = xt(a); b >>= 1; } return r; }
static void aes_init() {
  uint8_t p = 1, q = 1;
  do { p = p ^ xt(p); q ^= q << 1; q ^= q << 2; q ^= q << 4; if (q & 0x80) q ^= 0x09;
       uint8_t x = q ^ (uint8_t)((q << 1) | (q >> 7)) ^ (uint8_t)((q << 2) | (q >> 6)) ^ (uint8_t)((q << 3) | (q >> 5)) ^ (uint8_t)((q << 4) | (q >> 4));
       S[p] = x ^ 0x63; } while (p != 1);
  S[0] = 0x63; for (int i = 0; i < 256; i++) IS[S[i]] = (uint8_t)i;
}
static int ddt_inv(uint8_t din, uint8_t dout) { int c = 0; for (int x = 0; x < 256; x++) c += (IS[x] ^ IS[x ^ din]) == dout; return c; }
static int ddt_fwd(uint8_t din, uint8_t dout) { int c = 0; for (int x = 0; x < 256; x++) c += (S[x] ^ S[x ^ din]) == dout; return c; }
static const uint8_t IMC[4][4] = {{14, 11, 13, 9}, {9, 14, 11, 13}, {13, 9, 14, 11}, {11, 13, 9, 14}};
static const uint8_t FMC[4][4] = {{2, 3, 1, 1}, {1, 2, 3, 1}, {1, 1, 2, 3}, {3, 1, 1, 2}};
// ShiftRows moves input byte (r, c) to output column c - r; InvShiftRows to c + r.
static int sr_pos(int j) { int r = j % 4, c = j / 4; return 4 * ((c - r + 4) % 4) + r; }
static int isr_pos(int j) { int r = j % 4, c = j / 4; return 4 * ((c + r) % 4) + r; }
// difference Delta (16 bytes) after (Inv)MixColumns of a single byte difference g at position p
static void col_delta(const uint8_t M[4][4], int p, uint8_t g, uint8_t D[16]) {
  memset(D, 0, 16); int r = p % 4, c = p / 4; for (int k = 0; k < 4; k++) D[4 * c + k] = gm(M[k][r], g);
}

// ---- own transcription of LowLevelHashLenGt64 for the compiled variant (validated against the library) ----
static const uint64_t KS[5] = {0x243f6a8885a308d3ull, 0x13198a2e03707344ull, 0xa4093822299f31d0ull, 0x082efa98ec4e6c89ull, 0x452821e638d01377ull};
typedef struct { uint8_t b[16]; } V;
static V vld(const uint8_t *p) { V v; memcpy(v.b, p, 16); return v; }
static V vset(uint64_t hi, uint64_t lo) { V v; st64(v.b, lo); st64(v.b + 8, hi); return v; }
static V vadd(V a, V b) { V r; st64(r.b, ld64(a.b) + ld64(b.b)); st64(r.b + 8, ld64(a.b + 8) + ld64(b.b + 8)); return r; }
static V vsub(V a, V b) { V r; st64(r.b, ld64(a.b) - ld64(b.b)); st64(r.b + 8, ld64(a.b + 8) - ld64(b.b + 8)); return r; }
static V vxor(V a, V b) { for (int i = 0; i < 16; i++) a.b[i] ^= b.b[i]; return a; }
static V mixcols(V a, const uint8_t M[4][4]) { V r; for (int c = 0; c < 4; c++) for (int k = 0; k < 4; k++) { uint8_t s = 0; for (int j = 0; j < 4; j++) s ^= gm(M[k][j], a.b[4 * c + j]); r.b[4 * c + k] = s; } return r; }
static V enc_core(V a) { V t; for (int j = 0; j < 16; j++) t.b[sr_pos(j)] = S[a.b[j]]; return mixcols(t, FMC); }   // MC.SR.SB
static V dec_core(V a) { V t; for (int j = 0; j < 16; j++) t.b[isr_pos(j)] = IS[a.b[j]]; return mixcols(t, IMC); } // IMC.ISR.ISB
#if VARIANT == 1
static V Enc(V d, V k) { return vxor(enc_core(d), k); }
static V Dec(V d, V k) { return vxor(dec_core(d), k); }
static V MixA(V a, V s) { return Dec(vadd(s, a), s); }
static V MixB(V b, V s) { return Dec(vsub(s, b), s); }
static V MixC(V c, V s) { return Enc(vadd(s, c), s); }
static V MixD(V d, V s) { return Enc(vsub(s, d), s); }
static uint64_t M4(V a, V b, V c, V d) { V r = vadd(Enc(vadd(a, c), d), Dec(vsub(b, d), a)); return ld64(r.b) ^ ld64(r.b + 8); }
#elif VARIANT == 2
static V Enc(V d, V k) { return enc_core(vxor(d, k)); }
static V Dec(V d, V k) { return dec_core(vxor(d, k)); }
static V MixA(V a, V s) { return Dec(a, s); }
static V MixB(V b, V s) { return Dec(b, s); }
static V MixC(V c, V s) { return Enc(c, s); }
static V MixD(V d, V s) { return Enc(d, s); }
static uint64_t M4(V a, V b, V c, V d) { V r = vadd(Enc(a, c), Dec(b, d)); return ld64(r.b) ^ ld64(r.b + 8); }
#endif
static uint64_t Mix(uint64_t a, uint64_t b) { unsigned __int128 m = (unsigned __int128)a * b; return (uint64_t)m ^ (uint64_t)(m >> 64); }
static uint64_t my_llh_gt64(uint64_t seed, const uint8_t *p, size_t len) {
  const uint8_t *last32 = p + len - 32;
#if VARIANT == 0
  uint64_t cs = seed ^ KS[0] ^ len, d0 = cs, d1 = cs, d2 = cs;
  do { cs = Mix(ld64(p) ^ KS[1], ld64(p + 8) ^ cs); d0 = Mix(ld64(p + 16) ^ KS[2], ld64(p + 24) ^ d0);
       d1 = Mix(ld64(p + 32) ^ KS[3], ld64(p + 40) ^ d1); d2 = Mix(ld64(p + 48) ^ KS[4], ld64(p + 56) ^ d2);
       p += 64; len -= 64; } while (len > 64);
  cs = (cs ^ d0) ^ (d1 + d2);
  auto m32 = [](const uint8_t *q, uint64_t c) { return Mix(ld64(q) ^ KS[1], ld64(q + 8) ^ c) ^ Mix(ld64(q + 16) ^ KS[2], ld64(q + 24) ^ c); };
  if (len > 32) cs = m32(p, cs);
  return m32(last32, cs);
#else
  V s0 = vset(seed, len), s1 = s0, s2 = s0, s3 = s0;
  do { s0 = MixA(vld(p), s0); s1 = MixB(vld(p + 16), s1); s2 = MixC(vld(p + 32), s2); s3 = MixD(vld(p + 48), s3); p += 64; len -= 64; } while (len > 64);
  if (len > 32) { s0 = MixA(vld(p), s0); s1 = MixB(vld(p + 16), s1); }
  s2 = MixC(vld(last32), s2); s3 = MixD(vld(last32 + 16), s3);
  return M4(s0, s1, s2, s3);
#endif
}

static int g_fail = 0;
// run a fixed pair over N seeds; returns hits
static uint64_t run_pair(const char *label, const std::vector<uint8_t> &A, const std::vector<uint8_t> &B, uint64_t N, bool print_msgs, double expect_log2) {
  uint64_t hits = 0, ex = 0, hv = 0;
  for (uint64_t t = 0; t < N; t++) { uint64_t sd = rng64(); uint64_t ha = H(A.data(), A.size(), sd), hb = H(B.data(), B.size(), sd);
    if (ha == hb) { if (!hits) { ex = sd; hv = ha; } hits++; } }
  double lo, hi; ci95(hits, N, &lo, &hi);
  size_t nd = 0; for (size_t i = 0; i < std::min(A.size(), B.size()); i++) nd += A[i] != B[i];
  printf("  %-58s |m|=%zu |m'|=%zu (L=%zu) bytes differing %zu: %llu/%llu = 2^%.3f  95%%CI [2^%.3f, 2^%.3f]", label, A.size(), B.size(),
         (std::max(A.size(), B.size()) + 7) / 8, nd, (unsigned long long)hits, (unsigned long long)N, hits ? log2((double)hits / N) : -INFINITY,
         lo > 0 ? log2(lo) : -INFINITY, log2(hi));
  if (hits) printf("  e.g. seed %016llx -> %016llx", (unsigned long long)ex, (unsigned long long)hv);
  printf("\n");
  if (print_msgs) printf("    m  = %s\n    m' = %s\n", hx(A.data(), A.size()).c_str(), hx(B.data(), B.size()).c_str());
  if (expect_log2 == 0 && hits != N) g_fail = 1;
  fflush(stdout);
  return hits;
}
// chunk offsets usable for a 2-step lane trail: chunk i and chunk i+1 of the same <=1024-byte piece must both be absorbed
// by the lane (loop, or the tail mix for the last one).  We use full pieces or lengths <= 1024 and chunks i with
// 64*(i+2) < piece length.
int main(int argc, char **argv) {
  unsigned logN = argc > 1 ? atoi(argv[1]) : 16; rng_init(argc > 2 ? strtoull(argv[2], 0, 0) : 0x4c6f6e6743ull);
  unsigned logM = argc > 3 ? atoi(argv[3]) : logN;   // seeds for the mid-message trail
  int only_mid = argc > 4 ? atoi(argv[4]) : 0;        // 1: run only the mid-message trail (for parallel x86 runs)
  aes_init();
  printf("absl::Hash long-input analysis, variant: %s\n", VNAME);
  // 1. transcription check against the real library for 65..1024-byte inputs
  { std::vector<uint8_t> m(1024); int bad = 0;
    for (int t = 0; t < 20000; t++) { size_t n = 65 + rng64() % 960; fill(m.data(), n); uint64_t sd = rng64();
      if (my_llh_gt64(sd, m.data(), n) != H(m.data(), n, sd)) bad++; }
    printf("transcription of LowLevelHashLenGt64 vs the real library, 20000 random (len 65..1024, seed): %d mismatches\n", bad);
    if (bad) return 1; }
  const size_t LENS[4] = {256, 1024, 65536, 1048576};
#if VARIANT == 0
  // 2. reset chunk
  printf("\n[scalar] reset chunk: words 0,2,4,6 of a 64-byte loop chunk = kStaticRandomData[1..4]; all lanes become 0.\n");
  for (int li = 0; li < 4; li++) {
    size_t n = LENS[li]; std::vector<uint8_t> A(n); fill(A.data(), n);
    size_t npieces = n / 1024, piece = npieces ? rng64() % npieces : 0, plen = std::min<size_t>(n, 1024);
    size_t slots = (plen - 1) / 64;                                    // loop chunks o with o + 64 < plen
    size_t o = 1024 * piece + 64 * (rng64() % slots);
    for (int w = 0; w < 4; w++) st64(&A[o + 16 * w], KS[1 + w]);
    std::vector<uint8_t> B = A; fill(B.data(), o);                      // everything before the chunk re-randomized
    for (int w = 0; w < 4; w++) st64(&B[o + 16 * w + 8], rng64());      // and the four free words of the chunk
    char lab[160]; snprintf(lab, sizeof lab, "len %zu: reset at byte %zu (piece %zu), prefix+free words differ", n, o, piece);
    uint64_t N = 1ull << (n > 100000 ? std::min(logN, 10u) : logN);
    run_pair(lab, A, B, N, n == 256, 0);
  }
  { // shortest: 65 bytes, reset chunk at byte 0, words 1 and 3 (bytes 8..15, 24..31; not re-read by the 32-byte tail) differ
    std::vector<uint8_t> A(65); fill(A.data(), 65); for (int w = 0; w < 4; w++) st64(&A[16 * w], KS[1 + w]);
    std::vector<uint8_t> B = A; st64(&B[8], rng64()); st64(&B[24], rng64());
    run_pair("shortest: 65 B, reset at byte 0, bytes 8..15 and 24..31 differ", A, B, 1ull << logN, true, 0);
  }
  { // cross-length: prefixes of different lengths (multiples of 64), same reset chunk and suffix, one piece
    std::vector<uint8_t> R(64), Sfx(333); fill(R.data(), 64); fill(Sfx.data(), Sfx.size()); for (int w = 0; w < 4; w++) st64(&R[16 * w], KS[1 + w]);
    std::vector<uint8_t> A, B; A.resize(128); fill(A.data(), 128); B.resize(512); fill(B.data(), 512);
    A.insert(A.end(), R.begin(), R.end()); A.insert(A.end(), Sfx.begin(), Sfx.end());
    B.insert(B.end(), R.begin(), R.end()); B.insert(B.end(), Sfx.begin(), Sfx.end());
    run_pair("cross-length: 128-B vs 512-B prefix, same reset + 333-B suffix", A, B, 1ull << logN, false, 0);
  }
  { // multicollision family 2^16 at 1000 bytes: prefix (bytes 0..127) and the chunk's free words vary
    size_t n = 1000, K = 1 << 16, o = 128; std::vector<uint8_t> base(n); fill(base.data(), n); for (int w = 0; w < 4; w++) st64(&base[o + 16 * w], KS[1 + w]);
    std::vector<std::vector<uint8_t>> F(K, base);
    for (size_t i = 0; i < K; i++) { st64(&F[i][o + 8], i); fill(F[i].data(), 8); }      // member i: free word 1 = i, random first 8 bytes
    std::vector<std::vector<uint8_t>> T = F; std::sort(T.begin(), T.end()); size_t d = std::unique(T.begin(), T.end()) - T.begin();
    uint64_t N = 1ull << std::min(logN, 10u), all = 0, lo16 = 0; uint64_t ex = 0, exh = 0;
    for (uint64_t t = 0; t < N; t++) { uint64_t sd = rng64(), h0 = H(F[0].data(), n, sd); size_t ok = 1, ok16 = 1;
      for (size_t i = 1; i < K; i++) { uint64_t h = H(F[i].data(), n, sd); ok += h == h0; ok16 += (h & 0xffff) == (h0 & 0xffff); }
      if (ok == K) { if (!all) { ex = sd; exh = h0; } all++; } lo16 += ok16 == K; }
    printf("  family: %zu members (%zu distinct) of %zu bytes, reset at byte %zu: all equal (64-bit) for %llu/%llu seeds, low-16 %llu/%llu; e.g. seed %016llx -> %016llx\n",
           K, d, n, o, (unsigned long long)all, (unsigned long long)N, (unsigned long long)lo16, (unsigned long long)N, (unsigned long long)ex, (unsigned long long)exh);
    if (all != N || d != K) g_fail = 1;
  }
#else
  // lane helpers: word offset of lane w in chunk k: 64k + 16w (loop and tail agree for our lengths)
  const bool arm = VARIANT == 2;
  // 2. chunk-0 construction (state low half = public length)
  if (!only_mid) {
  printf("\n[%s] chunk 0: the state is Set128(seed, len) = (len | seed); the low 8 bytes of every lane input are public.\n", arm ? "ARM" : "x86");
  for (int li = 0; li < 5; li++) {
    size_t n = li == 0 ? 128 : li == 4 ? 1048576 : li == 3 ? 65536 : LENS[li]; std::vector<uint8_t> A(n); fill(A.data(), n);
    std::vector<uint8_t> B = A;
    // every 1024-byte piece restarts LowLevelHash with Set128(chained state, 1024): chunk 0 of ANY piece works
    size_t npieces = n / 1024, piece = npieces > 1 ? rng64() % npieces : 0, base = 1024 * piece, pl = n >= 1024 ? 1024 : n;
    if (arm) {  // every lane: change all 8 public bytes of chunk-0 word, correct chunk-1 word: p = 1
      for (int w = 0; w < 4; w++) {
        bool enc = w >= 2; uint8_t xin[16], xin2[16];
        for (int j = 0; j < 8; j++) { xin[j] = A[base + 16 * w + j] ^ (uint8_t)(pl >> (8 * j)); B[base + 16 * w + j] = (uint8_t)rng64(); xin2[j] = B[base + 16 * w + j] ^ (uint8_t)(pl >> (8 * j)); }
        uint8_t D[16] = {0};
        for (int j = 0; j < 8; j++) { uint8_t g = enc ? (S[xin[j]] ^ S[xin2[j]]) : (IS[xin[j]] ^ IS[xin2[j]]); uint8_t d1[16];
          col_delta(enc ? FMC : IMC, enc ? sr_pos(j) : isr_pos(j), g, d1); for (int k = 0; k < 16; k++) D[k] ^= d1[k]; }
        size_t c1 = base + 64 + 16 * w; if (pl <= 128 && w >= 2) c1 = base + pl - 32 + 16 * (w - 2);   // tail mix_cd reads last 32
        for (int k = 0; k < 16; k++) B[c1 + k] ^= D[k];
      }
      char lab[128]; snprintf(lab, sizeof lab, "len %zu: 32 public bytes at piece %zu start (byte %zu) changed, next chunk corrected", n, piece, base);
      run_pair(lab, A, B, 1ull << (n > 100000 ? std::min(logN, 12u) : logN), n == 128, 0);
    } else {    // x86 lane a: top byte 7 of (len + a0) moved so gamma = 0x73; step 2 guesses 9 sign bits and one DDT event
      uint64_t lo = pl + ld64(&A[base]); uint8_t x7 = (uint8_t)(lo >> 56), g = 0x73, x7n = S[IS[x7] ^ g];
      uint64_t lo2 = (lo & 0x00ffffffffffffffull) | ((uint64_t)x7n << 56); st64(&B[base], lo2 - pl);
      uint8_t D[16]; col_delta(IMC, isr_pos(7), g, D);                  // Delta in bytes 0..3 (lane 0 low 32 bits)
      uint64_t dl = ld64(D), cprime = 0; for (int k = 0; k < 64; k++) if ((dl >> k) & 1) cprime += 1ull << k;   // guess s1 bits = 1
      st64(&B[base + 64], ld64(&A[base + 64]) + (1ull << 63) + cprime);
      char lab[128]; snprintf(lab, sizeof lab, "len %zu: byte 7 at piece %zu start (byte %zu), next lane-a word corrected", n, piece, base);
      run_pair(lab, A, B, 1ull << logN, n == 128, -16);
    }
  }
  if (arm) { // 2b. flooding family: 2^16 members, bytes 0..1 of the lane-a word at a piece start take all values
    for (int fi = 0; fi < 2; fi++) {
      size_t n = fi == 0 ? 128 : 4096 + 77, base = fi == 0 ? 0 : 2048, pl = fi == 0 ? n : 1024, K = 1 << 16;
      std::vector<uint8_t> A(n); fill(A.data(), n); std::vector<std::vector<uint8_t>> F;
      for (size_t i = 0; i < K; i++) { std::vector<uint8_t> B = A; uint8_t D[16] = {0};
        for (int j = 0; j < 2; j++) { uint8_t x = A[base + j] ^ (uint8_t)(pl >> (8 * j)); B[base + j] = (uint8_t)(i >> (8 * j));
          uint8_t x2 = B[base + j] ^ (uint8_t)(pl >> (8 * j)), d1[16]; col_delta(IMC, isr_pos(j), IS[x] ^ IS[x2], d1); for (int k = 0; k < 16; k++) D[k] ^= d1[k]; }
        for (int k = 0; k < 16; k++) B[base + 64 + k] ^= D[k]; F.push_back(B); }
      std::vector<std::vector<uint8_t>> T = F; std::sort(T.begin(), T.end()); size_t d = std::unique(T.begin(), T.end()) - T.begin();
      uint64_t N = 1ull << std::min(logN, 10u), all = 0, ex = 0, exh = 0;
      for (uint64_t t = 0; t < N; t++) { uint64_t sd = rng64(), h0 = H(F[0].data(), n, sd); size_t ok = 1;
        for (size_t i = 1; i < K; i++) ok += H(F[i].data(), n, sd) == h0; if (ok == K) { if (!all) { ex = sd; exh = h0; } all++; } }
      printf("  family: %zu members (%zu distinct) of %zu bytes, bytes %zu..%zu vary (piece start), chunk %zu corrected: all equal for %llu/%llu seeds; e.g. seed %016llx -> %016llx\n",
             K, d, n, base, base + 1, base / 64 + 1, (unsigned long long)all, (unsigned long long)N, (unsigned long long)ex, (unsigned long long)exh);
      if (all != N || d != K) g_fail = 1;
    }
  }
  }
  // 3. mid-message trail at a random chunk, random content
  printf("\n[%s] mid-message trail at a random chunk (state secret):\n", arm ? "ARM" : "x86");
  uint8_t bdelta = 0, bgamma = 0; int bc = 0;
  if (arm) { for (int d = 1; d < 256; d++) for (int g = 1; g < 256; g++) { int c = ddt_inv((uint8_t)d, (uint8_t)g); if (c > bc) { bc = c; bdelta = (uint8_t)d; bgamma = (uint8_t)g; } }
    printf("  inverse-S-box DDT(%02x -> %02x) = %d/256 (lane a, byte 5 of the word)\n", bdelta, bgamma, bc); }
  for (int li = 0; li < 4; li++) {
    size_t n = LENS[li]; std::vector<uint8_t> A(n); fill(A.data(), n); std::vector<uint8_t> B = A;
    size_t npieces = n / 1024, piece = npieces ? rng64() % npieces : 0, plen = std::min<size_t>(n, 1024);
    size_t k = 1 + rng64() % ((plen - 1) / 64 - 1);   // chunk k >= 1 (secret state), k+1 absorbed in the same piece
    size_t o = 1024 * piece + 64 * k;
    if (arm) { int j = 5; B[o + j] ^= bdelta; uint8_t D[16]; col_delta(IMC, isr_pos(j), bgamma, D); for (int t = 0; t < 16; t++) B[o + 64 + t] ^= D[t]; }
    else { B[o + 15] ^= 0x80;                                            // lane-1 top bit of the lane-a word: carry-free
      uint8_t D[16]; col_delta(IMC, isr_pos(15), 0x73, D);               // Delta in bytes 8..11
      uint64_t dh = ld64(D + 8), cprime = dh; st64(&B[o + 64 + 8], ld64(&A[o + 64 + 8]) + (1ull << 63) + cprime); }
    char lab[160]; snprintf(lab, sizeof lab, "len %zu: chunk %zu of piece %zu (byte %zu) and chunk %zu", n, k, piece, o, k + 1);
    if (!arm && n > 1024 && logM > 22) { printf("  len %zu: skipped at 2^%u seeds (same in-piece computation as <= 1 KiB; cost)\n", n, logM); continue; }
    uint64_t N = 1ull << (n > 100000 ? std::min(logM, arm ? 18u : 26u) : logM);
    run_pair(lab, A, B, N, n == 256, arm ? -6 : -23);
  }
  if (arm) { // 4. weak-key 256-way: all 256 values of one byte, corrections for a guessed state byte x0
    size_t n = 1024, o = 64 * 5; int j = 5; std::vector<uint8_t> A(n); fill(A.data(), n);
    uint8_t x0 = 0x3c; std::vector<std::vector<uint8_t>> F;
    for (int v = 0; v < 256; v++) { std::vector<uint8_t> B = A; B[o + j] = (uint8_t)v;
      uint8_t g = IS[x0 ^ A[o + j]] ^ IS[x0 ^ (uint8_t)v]; uint8_t D[16]; col_delta(IMC, isr_pos(j), g, D);
      for (int t = 0; t < 16; t++) B[o + 64 + t] ^= D[t]; F.push_back(B); }
    uint64_t N = 1ull << logN, all = 0; std::vector<uint64_t> hist(257, 0);
    for (uint64_t t = 0; t < N; t++) { uint64_t sd = rng64(); std::vector<uint64_t> h(256); for (int v = 0; v < 256; v++) h[v] = H(F[v].data(), n, sd);
      std::sort(h.begin(), h.end()); size_t best = 1, run = 1; for (int v = 1; v < 256; v++) { run = h[v] == h[v - 1] ? run + 1 : 1; best = std::max(best, run); }
      hist[best]++; all += best == 256; }
    double lo, hi; ci95(all, N, &lo, &hi);
    printf("\n[ARM] weak-key 256-way at 1024 B (byte %zu takes all 256 values, chunk %zu corrected for a guessed state byte):\n"
           "  all 256 equal for %llu/%llu seeds = 2^%.3f (95%%CI [2^%.3f, 2^%.3f]); largest class per seed:", o + j, o / 64 + 1,
           (unsigned long long)all, (unsigned long long)N, all ? log2((double)all / N) : -INFINITY, lo > 0 ? log2(lo) : -INFINITY, log2(hi));
    for (int b = 1; b <= 256; b++) if (hist[b]) printf(" %d:%llu", b, (unsigned long long)hist[b]); printf("\n");
  }
#endif
  printf(g_fail ? "RESULT: FAIL\n" : "RESULT: PASS\n");
  return g_fail;
}
