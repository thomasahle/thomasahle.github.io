/* Independent check: 10-byte every-seed families and every-length key-free pairs for
 * mx3 v3 (jonmaiga/mx3 mx3.h), MUM (vnmakarov/mum-hash mum.h, default build) and mir_hash
 * (vnmakarov/mir mir-hash.h, relaxed = exact and strict = inexact).  Upstream headers are included unmodified.
 *
 * mx3:  hash = mix(state), state = seed*C^(n+1) + sum_k g(v_k) C^(n+1-k) + public length term,
 *       g(x) = (xC ^ (xC >> 39)) C is a bijection.  Word 0 carries C^n, so fixing a target and solving
 *       word 0 for each 2-byte tail gives exactly 2^16 ten-byte members.
 * MUM:  10 bytes: state = mum(seed+10, bsp) ^ mum(w0, p0) ^ mum(t, tail_prime); mum(v,p) = hi+lo of v*p.
 *       Solve mum(w0,p0) = X ^ mum(t,tp) for each 2-byte tail t (mum(w,p) == w*p mod 2^64-1 up to +1).
 * mir:  10 bytes: r = seed+10; r ^= mum(w0,p1); r ^= mum(part(t),p2); then a seed-dependent finalizer.
 * Every length: change the last byte, then re-solve one earlier word so the XORed public terms are unchanged.
 * Build: c++ -O2 -std=c++17 -I. indep_mx3_mum_mir.cc -o indep_mx3_mum_mir
 * Run:   ./indep_mx3_mum_mir [log2 seeds per family, default 8] [max length, default 1100] */
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <set>
#include <algorithm>
#include "mx3.h"
#include "mum.h"
#include "mir-hash.h"
typedef unsigned __int128 u128;
static uint64_t st = 0x5eed1dc0;
static uint64_t rnd() { uint64_t z = (st += 0x9e3779b97f4a7c15ULL); z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL; z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL; return z ^ (z >> 31); }
static uint64_t inv2(uint64_t a) { uint64_t x = a; for (int i = 0; i < 6; i++) x *= 2 - a * x; return x; }
/* ---------- mod 2^64-1 helpers for mum-style folds ---------- */
static const uint64_t MM = ~0ULL;
static uint64_t mulM(uint64_t a, uint64_t b) { u128 r = (u128)a * b; uint64_t lo = (uint64_t)r, hi = (uint64_t)(r >> 64), s = lo + hi; if (s < lo) s++; return s == MM ? 0 : s; }
static bool invM(uint64_t p, uint64_t *out) { __int128 t = 0, nt = 1; u128 r = MM, nr = p % MM;
  while (nr) { u128 q = r / nr; __int128 tt = t - (__int128)q * nt; t = nt; nt = tt; u128 rr = r - q * nr; r = nr; nr = rr; }
  if (r != 1) return false; if (t < 0) t += MM; *out = (uint64_t)t; return true; }
static uint64_t fold(uint64_t a, uint64_t b) { u128 r = (u128)a * b; return (uint64_t)r + (uint64_t)(r >> 64); }
/* all w with fold(w,p) == y */
static std::vector<uint64_t> solvefold(uint64_t y, uint64_t p) {
  std::vector<uint64_t> out; uint64_t pi; if (!invM(p, &pi)) return out;
  uint64_t c[4]; int n = 0; c[n++] = mulM(y % MM, pi); c[n++] = mulM((y + 1) % MM, pi);
  for (int i = 0; i < 2; i++) if (c[i] == 0) c[n++] = MM;
  std::sort(c, c + n); n = std::unique(c, c + n) - c;
  for (int i = 0; i < n; i++) if (fold(c[i], p) == y) out.push_back(c[i]);
  return out; }
typedef uint64_t (*H64)(const uint8_t *, size_t, uint64_t);
static uint64_t h_mx3(const uint8_t *m, size_t n, uint64_t s) { return mx3::hash(m, n, s); }
static uint64_t h_mum(const uint8_t *m, size_t n, uint64_t s) { return mum_hash(m, n, s); }
static uint64_t h_mir(const uint8_t *m, size_t n, uint64_t s) { return mir_hash(m, n, s); }
static uint64_t h_mirs(const uint8_t *m, size_t n, uint64_t s) { return mir_hash_strict(m, n, s); }
static int fails = 0;
static void family(const char *name, H64 h, const std::vector<std::vector<uint8_t>> &S, int lg) {
  std::set<std::vector<uint8_t>> d(S.begin(), S.end());
  uint64_t N = 1ULL << lg, ok = 0, ex = 0, eo = 0;
  for (uint64_t t = 0; t < N; t++) { uint64_t s = rnd(), h0 = h(S[0].data(), S[0].size(), s); bool a = true;
    for (size_t i = 1; i < S.size() && a; i++) a = h(S[i].data(), S[i].size(), s) == h0;
    if (a && !ok) { ex = s; eo = h0; } ok += a; }
  uint64_t zs = 0, z0 = h(S[0].data(), S[0].size(), 0); bool za = true; for (auto &m : S) za &= h(m.data(), m.size(), 0) == z0; zs = za;
  printf("%-28s %zu members (%zu distinct), length %zu: all equal on %llu/%llu random seeds + seed 0 %s; e.g. seed %016llx -> %016llx\n",
         name, S.size(), d.size(), S[0].size(), (unsigned long long)ok, (unsigned long long)N, zs ? "yes" : "NO",
         (unsigned long long)ex, (unsigned long long)eo);
  printf("   member[0] = "); for (auto b : S[0]) printf("%02x", b); printf("\n   member[1] = "); for (auto b : S[1]) printf("%02x", b); printf("\n");
  if (ok != N || !zs || d.size() != S.size() || S.size() != 65536) fails++; }
static uint64_t ld(const std::vector<uint8_t> &m, size_t o, size_t n = 8) { uint64_t v = 0; memcpy(&v, &m[o], n); return v; }
static void st64(std::vector<uint8_t> &m, size_t o, uint64_t v) { memcpy(&m[o], &v, 8); }
static bool pair_ok(H64 h, const std::vector<uint8_t> &a, const std::vector<uint8_t> &b, int trials) {
  if (a == b) return false; for (int t = 0; t < trials; t++) { uint64_t s = rnd(); if (h(a.data(), a.size(), s) != h(b.data(), b.size(), s)) return false; } return true; }
/* ---------------- mx3 ---------------- */
static const uint64_t C = mx3::C;
static uint64_t g(uint64_t x) { x *= C; x ^= x >> 39; return x * C; }
static uint64_t gi(uint64_t y) { uint64_t x = y * inv2(C); x ^= x >> 39; return x * inv2(C); }
/* public part of the mx3 state with seed 0 and word 0 replaced by zero contribution; words enter as h=(h+g(v))C */
static uint64_t mx3_state0(const std::vector<uint8_t> &m, bool skip0) {
  size_t len = m.size(), nw = len / 8, tl = len % 8; uint64_t h = (0 + g(len + 1)) * C;
  for (size_t k = 0; k < nw; k++) h = (h + ((k == 0 && skip0) ? 0 : g(ld(m, 8 * k)))) * C;
  if (tl) { uint64_t v = ld(m, 8 * nw, tl); h = (h + ((nw == 0 && skip0) ? 0 : g(v))) * C; }
  return h; }
int main(int argc, char **argv) {
  int lg = argc > 1 ? atoi(argv[1]) : 8; size_t maxlen = argc > 2 ? strtoull(argv[2], 0, 0) : 1100;
  /* sanity: my mx3 state model reproduces mx3::hash (mix of the state) */
  { int bad = 0; for (int t = 0; t < 2000; t++) { size_t len = rnd() % 300; std::vector<uint8_t> m(len); for (auto &x : m) x = rnd();
      uint64_t h = mx3_state0(m, false); if (mx3::mix(h) != mx3::hash(m.data(), len, 0)) bad++; }
    printf("mx3 state model vs mx3::hash (seed 0, 2000 random inputs 0..299 B): %d mismatches\n", bad); if (bad) fails++; }
  /* mx3 10-byte family */
  { uint64_t T = rnd(); std::vector<std::vector<uint8_t>> S;
    for (uint32_t t = 0; t < 65536; t++) { /* state = (((g(11))C + g(w0))C + g(t))C with seed 0 part; need g(w0)C + g(t) = T */
      uint64_t w0 = gi((T - g(t)) * inv2(C)); std::vector<uint8_t> m(10); st64(m, 0, w0); m[8] = t; m[9] = t >> 8; S.push_back(m); }
    family("mx3 v3", h_mx3, S, lg); }
  /* MUM 10-byte family (default build: exact 128-bit fold) */
  { std::vector<std::vector<uint8_t>> S; for (int att = 0; att < 8 && S.size() < 65536; att++) { S.clear(); uint64_t X = rnd();
      for (uint32_t t = 0; t < 65536 && S.size() < 65536; t++) for (uint64_t w : solvefold(X ^ fold(t, _mum_tail_prime), _mum_primes[0])) {
        std::vector<uint8_t> m(10); st64(m, 0, w); m[8] = t; m[9] = t >> 8; if (S.size() < 65536) S.push_back(m); } }
    family("MUM (mum.h default)", h_mum, S, lg); }
  /* mir 10-byte family (relaxed/exact fold) */
  { std::vector<std::vector<uint8_t>> S; for (int att = 0; att < 8 && S.size() < 65536; att++) { S.clear(); uint64_t X = rnd();
      for (uint32_t t = 0; t < 65536 && S.size() < 65536; t++) { uint8_t tb[2] = {(uint8_t)t, (uint8_t)(t >> 8)};
        uint64_t tp = mir_get_key_part(tb, 2, 1);
        for (uint64_t w : solvefold(X ^ fold(tp, mir_hash_p2), mir_hash_p1)) { std::vector<uint8_t> m(10); st64(m, 0, w); m[8] = tb[0]; m[9] = tb[1]; if (S.size() < 65536) S.push_back(m); } } }
    family("mir_hash (relaxed)", h_mir, S, lg); }
  /* every length 9..maxlen: one pair per length per hash, 256 seeds each */
  unsigned okx = 0, okm = 0, okr = 0, nl = 0;
  for (size_t len = 9; len <= maxlen; len++, nl++) {
    /* mx3: flip last byte, re-solve word 0 */
    { std::vector<uint8_t> a(len); for (auto &x : a) x = rnd(); std::vector<uint8_t> b = a; b[len - 1] ^= 0x5a;
      size_t nsteps = (len + 7) / 8; uint64_t Cn = 1; for (size_t k = 0; k < nsteps; k++) Cn *= C;
      uint64_t target = mx3_state0(a, false), rest = mx3_state0(b, true);
      st64(b, 0, gi((target - rest) * inv2(Cn))); okx += pair_ok(h_mx3, a, b, 256); }
    /* MUM: final segment after the bulk loop (loop runs while len > 8*UNROLL) */
    { const size_t B = 8 * _MUM_UNROLL_FACTOR; bool done = false;
      for (int att = 0; att < 64 && !done; att++) {
        std::vector<uint8_t> a(len); for (auto &x : a) x = rnd(); std::vector<uint8_t> b = a;
        size_t rem = len, off = 0; while (rem > B) { rem -= B; off += B; }
        if (rem >= 9) { b[len - 1] ^= 0x5a; /* terms of the final segment are XORed: re-solve its word 0 */
          size_t nw = rem / 8, tl = rem % 8; uint64_t da = 0;
          for (size_t i = 1; i < nw; i++) da ^= fold(ld(a, off + 8 * i), _mum_primes[i]) ^ fold(ld(b, off + 8 * i), _mum_primes[i]);
          if (tl) { /* tail word as mum.h builds it (little-endian, TAIL_START 0 in the default build) */
            uint64_t ta = ld(a, off + 8 * nw, tl), tb = ld(b, off + 8 * nw, tl); da ^= fold(ta, _mum_tail_prime) ^ fold(tb, _mum_tail_prime); }
          auto sol = solvefold(fold(ld(a, off), _mum_primes[0]) ^ da, _mum_primes[0]); if (sol.empty()) continue; st64(b, off, sol[0]);
        } else { /* last bulk block: change word 1, re-solve word 0 in mum(w0^p0, w1^p1) */
          size_t bo = off - B; uint64_t w1n = ld(a, bo + 8) ^ 0x5a5a5a5a5a5a5a5aULL;
          uint64_t y = fold(ld(a, bo) ^ _mum_primes[0], ld(a, bo + 8) ^ _mum_primes[1]);
          auto sol = solvefold(y, w1n ^ _mum_primes[1]); if (sol.empty()) continue;
          st64(b, bo + 8, w1n); st64(b, bo, sol[0] ^ _mum_primes[0]); }
        done = true; okm += pair_ok(h_mum, a, b, 256); } }
    /* mir: 16-byte blocks (w0*p1 ^ w1*p2 then state mixing), final word (p1) and tail (p2) */
    { bool done = false;
      for (int att = 0; att < 64 && !done; att++) {
        std::vector<uint8_t> a(len); for (auto &x : a) x = rnd(); std::vector<uint8_t> b = a;
        size_t nb = len / 16, rem = len % 16, o;
        if (rem >= 9) { o = 16 * nb; b[len - 1] ^= 0x5a;
          uint64_t ta = mir_get_key_part(&a[o + 8], rem - 8, 1), tb = mir_get_key_part(&b[o + 8], rem - 8, 1);
          auto sol = solvefold(fold(ld(a, o), mir_hash_p1) ^ fold(ta, mir_hash_p2) ^ fold(tb, mir_hash_p2), mir_hash_p1); if (sol.empty()) continue; st64(b, o, sol[0]); }
        else { o = 16 * (nb - 1); uint64_t w1n = ld(a, o + 8) ^ 0x5a5a5a5a5a5a5a5aULL;
          auto sol = solvefold(fold(ld(a, o), mir_hash_p1) ^ fold(ld(a, o + 8), mir_hash_p2) ^ fold(w1n, mir_hash_p2), mir_hash_p1); if (sol.empty()) continue;
          st64(b, o + 8, w1n); st64(b, o, sol[0]); }
        done = true; okr += pair_ok(h_mir, a, b, 256); } }
  }
  printf("every length 9..%zu (one pair per length, 256 random seeds each): mx3 %u/%u, MUM %u/%u (unroll %d), mir %u/%u lengths collide on every seed\n",
         maxlen, okx, nl, okm, nl, _MUM_UNROLL_FACTOR, okr, nl);
  if (okx != nl || okm != nl || okr != nl) fails++;
  printf("%s\n", fails ? "FAIL" : "PASS"); return fails != 0;
}
