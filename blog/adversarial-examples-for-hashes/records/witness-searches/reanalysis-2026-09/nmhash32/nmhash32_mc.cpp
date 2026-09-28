/* nmhash32_mc.cpp -- nmhash32 v2 multicollision and long-input experiments.
 * Hashes with the upstream header nmhash.h (gzm55/hash-garage e022156ca8; see SOURCES.md),
 * included unmodified.  The search runs used SMHasher3's scalar port of the same function
 * (verification 0x12A30553, checked again at start-up here).
 *
 * Build: c++ -O2 -std=c++17 -pthread -o nmhash32_mc nmhash32_mc.cpp
 */
#include <math.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <string>
#include "nmhash.h"
template <bool bswap> static inline uint32_t GET_U32(const uint8_t * b, size_t i) { uint32_t v; memcpy(&v, b + i, 4); return bswap ? __builtin_bswap32(v) : v; }
template <bool bswap> static inline void PUT_U32(uint32_t v, uint8_t * b, size_t i) { if (bswap) v = __builtin_bswap32(v); memcpy(b + i, &v, 4); }
template <bool bswap> static inline uint32_t NMHASH32_T(const uint8_t * m, size_t len, uint32_t seed) { return NMHASH32(m, len, seed); }
/* ======================================================================================
 * nmhash32 v2 multicollision / pair driver.  The hash is the upstream nmhash.h (see the top of this file);
 * below is the experiment.
 *
 * Build: g++ -O2 -std=c++17 -pthread -o nmhash32_mc nmhash32_mc.cpp
 *
 * Key model: uniform 32-bit seed (nmhash32 has a 32-bit seed and a 32-bit output).
 *
 * Families ("lane-tied"): every lane gets the SAME state.  In the 33..255-byte path the x lane
 * starts at P_j, so the x word of lane j is P_j ^ A; in the >=256-byte path the x lane starts at
 * NMH_ACC_INIT[j], so the x word is NMH_ACC_INIT[j] ^ A.  All y words are B.  Then every lane runs
 * the identical computation, so a per-lane differential either succeeds in every lane or in none.
 *
 * Trail T1 (the record's):  D = 0x80400000 into the x and y word of a lane in round 0; the next
 *   round's x word cancels C = 0x80202808 (or the carry variants 0x80206818 / 0x8020e838) and the
 *   y word cancels D.  Conditions: bit 22 of x ^ y is 1 (a seed bit) and one carry in the low
 *   16-bit M3 product.
 * Trail T2 (NEW): D = 0x04008040 = L^-1(0x8000) where L(v) = v ^ v<<11 ^ v>>9.  x+y is unchanged
 *   iff bits 26, 15, 6 of x ^ y are all 1; then the only surviving difference is D in y, which
 *   becomes 0x8000 before M3 -- the top bit of a 16-bit half, which an odd 16-bit multiplier maps
 *   to itself -- and 0x8020 after the final xorshift.  C = 0x00008020.  No carry condition: the
 *   trail holds exactly when three seed bits take one value (y = seed ^ B in the long path,
 *   y = (seed + len) ^ B in the 33..255 path).
 * ====================================================================================== */
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <vector>
#include <algorithm>
#include <thread>
#include <atomic>
#include <mutex>

static uint32_t H(const uint8_t * m, size_t len, uint32_t seed) { return NMHASH32_T<false>(m, len, seed); }

/* SMHasher3 VerificationTest, as in SMHasher3 util/ (keys {}, {0}, {0,1}, ..., seed 256-i). */
static uint32_t smhasher3_verification(void) {
    uint8_t key[256], hashes[4 * 256], total[4];
    memset(key, 0, sizeof key);
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t h = H(key, i, 256 - i);
        PUT_U32<false>(h, hashes, 4 * i);
        key[i] = (uint8_t)i;
    }
    PUT_U32<false>(H(hashes, sizeof hashes, 0), total, 0);
    return GET_U32<false>(total, 0);
}

/* splitmix64 -> xoshiro256** */
struct Rng {
    uint64_t s[4];
    static uint64_t sm(uint64_t & x) { uint64_t z = (x += 0x9e3779b97f4a7c15ull); z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ull; z = (z ^ (z >> 27)) * 0x94d049bb133111ebull; return z ^ (z >> 31); }
    explicit Rng(uint64_t seed) { for (int i = 0; i < 4; i++) s[i] = sm(seed); }
    static uint64_t rotl(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
    uint64_t next() { uint64_t r = rotl(s[1] * 5, 7) * 9, t = s[1] << 17; s[2] ^= s[0]; s[3] ^= s[1]; s[1] ^= s[2]; s[0] ^= s[3]; s[2] ^= t; s[3] = rotl(s[3], 45); return r; }
};

static void put32(uint8_t * m, size_t off, uint32_t v) { PUT_U32<false>(v, m, off); }
static uint32_t get32(const uint8_t * m, size_t off) { return GET_U32<false>(m, off); }

static const uint32_t P4c[4] = { NMH_PRIME32_1, NMH_PRIME32_2, NMH_PRIME32_3, NMH_PRIME32_4 };

struct Trail { uint32_t D, C; const char * name; };
static const Trail T1a = { 0x80400000u, 0x80202808u, "T1 (D=80400000, C=80202808)" };
static const Trail T1b = { 0x80400000u, 0x80206818u, "T1 carry variant (C=80206818)" };
static const Trail T1c = { 0x80400000u, 0x8020e838u, "T1 carry variant (C=8020e838)" };
static const Trail T2  = { 0x04008040u, 0x00008020u, "T2 (D=04008040, C=00008020)" };

/* 64-byte message (33..255 path, one loop round then the final words at len-32, len-16).
 * lanes: bit j of mask = inject in lane j.  A = common x-lane state, B = common y word. */
static void build64(uint8_t m[64], uint32_t mask, const Trail & t, uint32_t A, uint32_t B) {
    for (int j = 0; j < 4; j++) {
        int on = (mask >> j) & 1;
        put32(m, 4 * j,      (P4c[j] ^ A) ^ (on ? t.D : 0));
        put32(m, 16 + 4 * j, B ^ (on ? t.D : 0));
        put32(m, 32 + 4 * j, on ? t.C : 0);
        put32(m, 48 + 4 * j, on ? t.D : 0);
    }
}
/* 512-byte message (long path: round 0 on bytes 0..255, final round on bytes 256..511).
 * lanes: bit j of the 32-bit mask. */
static void build512(uint8_t m[512], uint32_t mask, const Trail & t, uint32_t A, uint32_t B) {
    for (int j = 0; j < 32; j++) {
        int on = (mask >> j) & 1;
        put32(m, 4 * j,       (NMH_ACC_INIT[j] ^ A) ^ (on ? t.D : 0));
        put32(m, 128 + 4 * j, B ^ (on ? t.D : 0));
        put32(m, 256 + 4 * j, on ? t.C : 0);
        put32(m, 384 + 4 * j, on ? t.D : 0);
    }
}
/* A for a guess g of the trail's seed bits (B = 0).  T2: condition "bits 26,15,6 of y equal g";
 * x ^ y must be 1 there, so A has those bits = ~g.  T1: condition "bit 22 of y equals g". */
static uint32_t A_for(const Trail & t, unsigned g) {
    if (t.D == T2.D) {
        uint32_t A = 0;
        if (!((g >> 0) & 1)) A |= 1u << 6;
        if (!((g >> 1) & 1)) A |= 1u << 15;
        if (!((g >> 2) & 1)) A |= 1u << 26;
        return A;
    }
    return ((g & 1) ? 0 : (1u << 22));
}
static unsigned seedbits_T2(uint32_t y) { return ((y >> 6) & 1) | (((y >> 15) & 1) << 1) | (((y >> 26) & 1) << 2); }

/* ---------- parallel exhaustive helper over all 2^32 seeds ---------- */
template <class F> static void par_all_seeds(int threads, F f) {
    std::vector<std::thread> th;
    for (int t = 0; t < threads; t++) th.emplace_back([=]() { for (uint64_t s = (uint64_t)t; s < (1ull << 32); s += threads) f(t, (uint32_t)s); });
    for (auto & x : th) x.join();
}

static int NT = 8;

/* Mode pair: the published 64-byte pair, all 2^32 seeds. */
static int mode_pair(void) {
    uint8_t a[64] = {0}, b[64] = {0};
    put32(b, 0, 0x80400000u); put32(b, 16, 0x80400000u); put32(b, 32, 0x80202808u); put32(b, 48, 0x80400000u);
    static const char * hx = "00004080000000000000000000000000000040800000000000000000000000000828208000000000000000000000000000004080000000000000000000000000";
    for (int i = 0; i < 64; i++) { unsigned v; sscanf(hx + 2 * i, "%2x", &v); if (b[i] != v) { printf("pair mismatch with data.json at byte %d\n", i); return 1; } }
    printf("m' matches data.json m_prime_hex; H(m,0xb54cda26)=%08x H(m',0xb54cda26)=%08x\n", H(a, 64, 0xb54cda26u), H(b, 64, 0xb54cda26u));
    std::vector<uint64_t> cnt(NT, 0);
    std::vector<uint32_t> first(NT, 0xffffffffu);
    par_all_seeds(NT, [&](int t, uint32_t s) { if (H(a, 64, s) == H(b, 64, s)) { if (!cnt[t]) first[t] = s; cnt[t]++; } });
    uint64_t c = 0; for (auto x : cnt) c += x;
    printf("published pair: %" PRIu64 " / 2^32 seeds collide (record: 1078944392)\n", c);
    return 0;
}

/* Mode exh64: exhaustive over 2^32 seeds, 16-way lane-tied family at 64 bytes, for trail t with guess g. */
static int mode_exh64(const Trail & t, unsigned g) {
    uint32_t A = A_for(t, g), B = 0;
    uint8_t msgs[16][64];
    for (uint32_t mask = 0; mask < 16; mask++) build64(msgs[mask], mask, t, A, B);
    std::vector<uint64_t> all(NT, 0), pair(NT, 0), pred(NT, 0), allNotPred(NT, 0), predNotAll(NT, 0);
    std::vector<uint32_t> ex(NT, 0);
    par_all_seeds(NT, [&](int th, uint32_t s) {
        uint32_t h0 = H(msgs[0], 64, s);
        bool p = H(msgs[1], 64, s) == h0;
        if (p) pair[th]++;
        bool a = p;
        if (a) for (int k = 2; k < 16 && a; k++) a = (H(msgs[k], 64, s) == h0);
        if (a) { all[th]++; ex[th] = s; }
        if (t.D == T2.D) {
            bool pr = seedbits_T2((s + 64u) ^ B) == g;
            if (pr) pred[th]++;
            if (a && !pr) allNotPred[th]++;
            if (pr && !a) predNotAll[th]++;
        }
    });
    uint64_t A1 = 0, P1 = 0, PR = 0, ANP = 0, PNA = 0; uint32_t e = 0;
    for (int i = 0; i < NT; i++) { A1 += all[i]; P1 += pair[i]; PR += pred[i]; ANP += allNotPred[i]; PNA += predNotAll[i]; if (all[i]) e = ex[i]; }
    printf("exh64 %s guess %u: 16-way family (64 B) collides for %" PRIu64 " / 2^32 seeds = 2^%.4f; lane-0 pair %" PRIu64 " = 2^%.4f\n",
           t.name, g, A1, A1 ? log2((double)A1) - 32 : -99.0, P1, P1 ? log2((double)P1) - 32 : -99.0);
    if (t.D == T2.D) printf("  predictor (bits 26,15,6 of seed+64 == guess): %" PRIu64 " seeds; family-without-predictor %" PRIu64 "; predictor-without-family %" PRIu64 "\n", PR, ANP, PNA);
    if (A1) {
        printf("  example colliding seed %08x: all 16 members hash to %08x\n", e, H(msgs[0], 64, e));
        printf("  member 0  = "); for (int i = 0; i < 64; i++) printf("%02x", msgs[0][i]); printf("\n");
        printf("  member 15 = "); for (int i = 0; i < 64; i++) printf("%02x", msgs[15][i]); printf("\n");
    }
    return 0;
}

/* Mode exh512: exhaustive over 2^32 seeds of the three-member predictor set {empty, lane 0, all 32 lanes}
 * at 512 bytes, plus the T2 seed-bit predictor. */
static int mode_exh512(const Trail & t, unsigned g) {
    uint32_t A = A_for(t, g), B = 0;
    uint8_t m0[512], m1[512], mall[512];
    build512(m0, 0, t, A, B); build512(m1, 1, t, A, B); build512(mall, 0xffffffffu, t, A, B);
    std::vector<uint64_t> c3(NT, 0), c1(NT, 0), pr(NT, 0), bad(NT, 0);
    par_all_seeds(NT, [&](int th, uint32_t s) {
        uint32_t h0 = H(m0, 512, s);
        bool p = H(m1, 512, s) == h0;
        bool a = p && H(mall, 512, s) == h0;
        c1[th] += p; c3[th] += a;
        if (t.D == T2.D) { bool q = seedbits_T2(s ^ B) == g; pr[th] += q; bad[th] += (q != a); }
    });
    uint64_t C3 = 0, C1 = 0, PR = 0, BAD = 0; for (int i = 0; i < NT; i++) { C3 += c3[i]; C1 += c1[i]; PR += pr[i]; BAD += bad[i]; }
    printf("exh512 %s guess %u: lane-0 pair %" PRIu64 " / 2^32 = 2^%.4f; {empty, lane0, all-32-lanes} equal for %" PRIu64 " = 2^%.4f\n",
           t.name, g, C1, C1 ? log2((double)C1) - 32 : -99.0, C3, C3 ? log2((double)C3) - 32 : -99.0);
    if (t.D == T2.D) printf("  T2 predictor (bits 26,15,6 of seed == guess) holds for %" PRIu64 " seeds; disagreements with the 3-member check: %" PRIu64 "\n", PR, BAD);
    return 0;
}

/* Mode fam512: whole-family check with k = 2^m members (subsets of lanes 0..m-1) on uniform seeds and on
 * seeds drawn from the trail's condition class (T2: rejection-sample the three seed bits). */
static int mode_fam512(const Trail & t, unsigned g, int m, long nseeds, uint64_t rs) {
    uint32_t A = A_for(t, g), B = 0;
    size_t k = (size_t)1 << m;
    std::vector<uint8_t> msgs(k * 512);
    for (size_t j = 0; j < k; j++) build512(&msgs[j * 512], (uint32_t)j, t, A, B);
    for (int cls = 0; cls < 2; cls++) {
        if (cls == 1 && t.D != T2.D) break;
        std::atomic<long> hit(0), idx(0); std::mutex mu; uint32_t exs = 0, exh = 0; bool have = false;
        std::vector<std::thread> th;
        for (int ti = 0; ti < NT; ti++) th.emplace_back([&, ti]() {
            Rng r(rs * 1000003ull + (uint64_t)ti * 7919ull + (uint64_t)cls);
            long i;
            while ((i = idx++) < nseeds) {
                uint32_t s;
                do { s = (uint32_t)r.next(); } while (cls == 1 && seedbits_T2(s ^ B) != g);
                uint32_t h0 = H(&msgs[0], 512, s); bool ok = true;
                for (size_t j = 1; j < k && ok; j++) ok = H(&msgs[j * 512], 512, s) == h0;
                if (ok) { hit++; std::lock_guard<std::mutex> lk(mu); if (!have) { have = true; exs = s; exh = h0; } }
            }
        });
        for (auto & x : th) x.join();
        long hh = hit.load();
        printf("fam512 %s guess %u k=2^%d %s seeds: whole family collides for %ld / %ld = %.5f", t.name, g, m, cls ? "class" : "uniform", hh, nseeds, (double)hh / nseeds);
        if (have) printf("  (example seed %08x -> %08x)", exs, exh);
        printf("\n");
    }
    return 0;
}

/* Mode perkey: fixed union set.  T2: the 8 guess sub-families (8 * 2^m members, 512 B).
 * T1: 2 guesses x 3 carry variants (the empty-lane member of a guess is shared by its 3 variants).
 * For each uniform seed, sort the outputs and record the largest full-output class and the largest
 * class of the low 16 / low 20 / high 16 bits. */
static int mode_perkey(int trail, int m, long nseeds, uint64_t rs, int len) {
    std::vector<std::vector<uint8_t>> set;
    uint32_t lanes = (len == 512) ? 32 : 4;
    if (m > (int)lanes) { printf("m too large\n"); return 1; }
    size_t k = (size_t)1 << m;
    auto add = [&](const Trail & t, unsigned g, bool includeEmpty) {
        uint32_t A = A_for(t, g);
        for (size_t j = includeEmpty ? 0 : 1; j < k; j++) {
            std::vector<uint8_t> v(len);
            if (len == 512) build512(v.data(), (uint32_t)j, t, A, 0); else build64(v.data(), (uint32_t)j, t, A, 0);
            set.push_back(v);
        }
    };
    if (trail == 2) { for (unsigned g = 0; g < 8; g++) add(T2, g, true); }
    else { for (unsigned g = 0; g < 2; g++) { add(T1a, g, true); add(T1b, g, false); add(T1c, g, false); } }
    { std::vector<std::vector<uint8_t>> c = set; std::sort(c.begin(), c.end()); if (std::unique(c.begin(), c.end()) != c.end()) { printf("duplicate members!\n"); return 1; } }
    size_t N = set.size();
    printf("perkey trail %s len %d: fixed set of %zu distinct messages (sub-family size 2^%d)\n", trail == 2 ? "T2" : "T1", len, N, m);
    std::vector<long> full, lo16, lo20, hi16;
    std::mutex mu; std::atomic<long> idx(0);
    std::vector<std::thread> th;
    for (int ti = 0; ti < NT; ti++) th.emplace_back([&, ti]() {
        Rng r(rs * 1000003ull + (uint64_t)ti * 104729ull);
        std::vector<uint32_t> h(N), tmp(N);
        long i;
        while ((i = idx++) < nseeds) {
            uint32_t s = (uint32_t)r.next();
            for (size_t j = 0; j < N; j++) h[j] = H(set[j].data(), len, s);
            auto largest = [&](uint32_t mask, int shift) { for (size_t j = 0; j < N; j++) tmp[j] = (h[j] >> shift) & mask; std::sort(tmp.begin(), tmp.end()); long best = 1, run = 1; for (size_t j = 1; j < N; j++) { run = (tmp[j] == tmp[j - 1]) ? run + 1 : 1; best = std::max(best, run); } return best; };
            long f = largest(0xffffffffu, 0), a = largest(0xffffu, 0), b = largest(0xfffffu, 0), c = largest(0xffffu, 16);
            std::lock_guard<std::mutex> lk(mu); full.push_back(f); lo16.push_back(a); lo20.push_back(b); hi16.push_back(c);
        }
    });
    for (auto & x : th) x.join();
    auto stats = [&](const char * nm, std::vector<long> v) { std::sort(v.begin(), v.end()); printf("  largest %s class over %zu seeds: min %ld, 1st pct %ld, median %ld, max %ld\n", nm, v.size(), v[0], v[v.size() / 100], v[v.size() / 2], v.back()); };
    stats("full-output", full); stats("low-16-bit", lo16); stats("low-20-bit", lo20); stats("high-16-bit", hi16);
    return 0;
}

/* Mode outcomes: T1 carry outcome distribution for the lane-tied 64-byte pair (which C works). */
static int mode_outcomes(long nseeds, uint64_t rs) {
    const Trail * ts[3] = { &T1a, &T1b, &T1c };
    long c[2][3] = {{0}}; Rng r(rs);
    for (long i = 0; i < nseeds; i++) {
        uint32_t s = (uint32_t)r.next();
        for (unsigned g = 0; g < 2; g++) {
            uint8_t a[64], b[64]; build64(a, 0, T1a, A_for(T1a, g), 0);
            uint32_t h0 = H(a, 64, s);
            for (int v = 0; v < 3; v++) { build64(b, 1, *ts[v], A_for(T1a, g), 0); c[g][v] += (H(b, 64, s) == h0); }
        }
    }
    for (unsigned g = 0; g < 2; g++) printf("T1 lane-tied 64-B pair, guess %u, %ld seeds: C=80202808 %ld, C=80206818 %ld, C=8020e838 %ld\n", g, nseeds, c[g][0], c[g][1], c[g][2]);
    return 0;
}


/* Mode short8: sampled search over 8-byte pairs (w0, w1) vs (w0 + a, w1 ^ dy): the 5..8-byte path
 * (L = 1).  dy over all weight <= 2 xor differences, a over 0 and all signed single bits; w0, w1
 * random per candidate; 4096 seeds per candidate; the best candidates are re-measured on 2^20 seeds. */
static int mode_short8(uint64_t rs) {
    Rng r(rs);
    struct C { double p; uint32_t a, dy, w0, w1; };
    std::vector<C> res;
    std::vector<uint32_t> dys; for (int i = 0; i < 32; i++) { dys.push_back(1u << i); for (int j = i + 1; j < 32; j++) dys.push_back((1u << i) | (1u << j)); }
    std::vector<uint32_t> as; as.push_back(0); for (int i = 0; i < 32; i++) { as.push_back(1u << i); as.push_back(0u - (1u << i)); }
    std::vector<uint32_t> S(4096); for (auto & x : S) x = (uint32_t)r.next();
    for (uint32_t dy : dys) for (uint32_t a : as) {
        uint32_t w0 = (uint32_t)r.next(), w1 = (uint32_t)r.next(); uint8_t m[8], mp[8];
        put32(m, 0, w0); put32(m, 4, w1); put32(mp, 0, w0 + a); put32(mp, 4, w1 ^ dy);
        int c = 0; for (uint32_t s : S) c += H(m, 8, s) == H(mp, 8, s);
        res.push_back({ (double)c / S.size(), a, dy, w0, w1 });
    }
    std::sort(res.begin(), res.end(), [](const C & x, const C & y) { return x.p > y.p; });
    printf("short8: %zu candidates x 4096 seeds; top 5 re-measured on 2^20 seeds:\n", res.size());
    for (int i = 0; i < 5; i++) {
        uint8_t m[8], mp[8]; put32(m, 0, res[i].w0); put32(m, 4, res[i].w1); put32(mp, 0, res[i].w0 + res[i].a); put32(mp, 4, res[i].w1 ^ res[i].dy);
        long c = 0; for (long k = 0; k < (1 << 20); k++) { uint32_t s = (uint32_t)r.next(); c += H(m, 8, s) == H(mp, 8, s); }
        printf("  a=%08x dy=%08x: screening %.5f, re-measured %ld/2^20 = 2^%.2f (score %.2f)\n", res[i].a, res[i].dy, res[i].p, c, c ? log2((double)c) - 20 : -99.0, c ? 20 - log2((double)c) : 99.0);
    }
    return 0;
}


/* Mode longins: random message of len bytes (len a multiple of 256, >= 512); insert the single-lane trail
 * (lane j) into round `pos` (bytes 256*pos .. ) and cancel it in round pos+1.  nins > 1: that many disjoint
 * insertions (lanes 0.. of one round if sameRound, else lane 0 of every other round) all present at once.
 * Message content is fresh random per trial (so the round states are arbitrary), key = uniform seed. */
static int mode_longins(size_t len, size_t pos, int trail, int nins, int sameRound, long n, uint64_t rs) {
    const Trail & t = trail == 2 ? T2 : T1a;
    std::atomic<long> c(0), idx(0); std::vector<std::thread> th;
    for (int ti = 0; ti < NT; ti++) th.emplace_back([&, ti]() {
        Rng r(rs * 1000003ull + ti); std::vector<uint8_t> a(len), b(len); long i, l = 0;
        while ((i = idx++) < n) {
            for (size_t k = 0; k < len; k += 8) { uint64_t v = r.next(); memcpy(&a[k], &v, 8); }
            b = a;
            for (int q = 0; q < nins; q++) {
                size_t rd = sameRound ? pos : pos + 2 * q; int lane = sameRound ? q : 0;
                size_t o = 256 * rd, o2 = 256 * (rd + 1);
                put32(b.data(), o + 4 * lane, get32(b.data(), o + 4 * lane) ^ t.D);
                put32(b.data(), o + 128 + 4 * lane, get32(b.data(), o + 128 + 4 * lane) ^ t.D);
                put32(b.data(), o2 + 4 * lane, get32(b.data(), o2 + 4 * lane) ^ t.C);
                put32(b.data(), o2 + 128 + 4 * lane, get32(b.data(), o2 + 128 + 4 * lane) ^ t.D);
            }
            uint32_t s = (uint32_t)r.next(); l += H(a.data(), len, s) == H(b.data(), len, s);
        }
        c += l; });
    for (auto & x : th) x.join();
    double pp = (double)c / n;
    printf("longins %s len %zu round %zu, %d insertion(s)%s: %ld / %ld = %.5f = 2^%.3f (+-2se %.4f)\n", t.name, len, pos, nins, nins > 1 ? (sameRound ? " (lanes 0.. of one round)" : " (every other round)") : "", (long)c, n, pp, log2(pp), 2 * sqrt(pp * (1 - pp) / n));
    return 0;
}


/* Mode longtied: lane-symmetric random message (round 0 x words INIT_j ^ A_r, every other 4-byte word equal
 * across the 32 lanes of its half-round, values random per trial); insert the trail into ALL 32 lanes of round
 * pos (cancel in pos+1) and compare with the base.  Also lane 0 only.  Tied lanes => both rates equal. */
static int mode_longtied(size_t len, size_t pos, int trail, long n, uint64_t rs) {
    const Trail & t = trail == 2 ? T2 : T1a;
    std::atomic<long> c1(0), c32(0), idx(0); std::vector<std::thread> th;
    for (int ti = 0; ti < NT; ti++) th.emplace_back([&, ti]() {
        Rng r(rs * 1000003ull + ti); std::vector<uint8_t> a(len), b(len), b1(len); long i, l1 = 0, l32 = 0;
        while ((i = idx++) < n) {
            for (size_t rd = 0; rd < len / 256; rd++) {
                uint32_t xv = (uint32_t)r.next(), yv = (uint32_t)r.next();
                for (int j = 0; j < 32; j++) { put32(a.data(), 256 * rd + 4 * j, rd == 0 ? (NMH_ACC_INIT[j] ^ xv) : xv); put32(a.data(), 256 * rd + 128 + 4 * j, yv); }
            }
            b = a; b1 = a;
            size_t o = 256 * pos, o2 = 256 * (pos + 1);
            for (int lane = 0; lane < 32; lane++) for (auto * m : { &b, &b1 }) {
                if (m == &b1 && lane) continue;
                put32(m->data(), o + 4 * lane, get32(m->data(), o + 4 * lane) ^ t.D); put32(m->data(), o + 128 + 4 * lane, get32(m->data(), o + 128 + 4 * lane) ^ t.D);
                put32(m->data(), o2 + 4 * lane, get32(m->data(), o2 + 4 * lane) ^ t.C); put32(m->data(), o2 + 128 + 4 * lane, get32(m->data(), o2 + 128 + 4 * lane) ^ t.D);
            }
            uint32_t s = (uint32_t)r.next(); uint32_t h0 = H(a.data(), len, s);
            bool p1 = H(b1.data(), len, s) == h0, p32 = H(b.data(), len, s) == h0; l1 += p1; l32 += (p1 && p32);
        }
        c1 += l1; c32 += l32; });
    for (auto & x : th) x.join();
    printf("longtied %s len %zu round %zu: lane-0 pair %ld / %ld = %.4f; {base, lane0, all-32-lanes} all equal %ld / %ld = %.4f\n", t.name, len, pos, (long)c1, n, (double)c1 / n, (long)c32, n, (double)c32 / n);
    return 0;
}

int main(int argc, char ** argv) {
    uint32_t v = smhasher3_verification();
    printf("SMHasher3 verification of nmhash32 (upstream nmhash.h): %08X (expected 12A30553) %s\n", v, v == 0x12A30553u ? "OK" : "MISMATCH");
    if (v != 0x12A30553u) return 1;
    if (argc < 2) { printf("modes: pair | exh64 <1|2> <g> | exh512 <1|2> <g> | fam512 <1|2> <g> <m> <nseeds> <rs> | perkey <1|2> <m> <nseeds> <rs> <64|512> | outcomes <n> <rs>   (env NT=threads)\n"); return 0; }
    if (getenv("NT")) NT = atoi(getenv("NT"));
    std::string md = argv[1];
    auto tr = [&](int i) -> const Trail & { return atoi(argv[i]) == 2 ? T2 : T1a; };
    if (md == "pair") return mode_pair();
    if (md == "exh64") return mode_exh64(tr(2), (unsigned)atoi(argv[3]));
    if (md == "exh512") return mode_exh512(tr(2), (unsigned)atoi(argv[3]));
    if (md == "fam512") return mode_fam512(tr(2), (unsigned)atoi(argv[3]), atoi(argv[4]), atol(argv[5]), strtoull(argv[6], 0, 0));
    if (md == "perkey") return mode_perkey(atoi(argv[2]), atoi(argv[3]), atol(argv[4]), strtoull(argv[5], 0, 0), atoi(argv[6]));
    if (md == "longins") return mode_longins(strtoul(argv[2],0,0), strtoul(argv[3],0,0), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]), atol(argv[7]), strtoull(argv[8],0,0));
    if (md == "longtied") return mode_longtied(strtoul(argv[2],0,0), strtoul(argv[3],0,0), atoi(argv[4]), atol(argv[5]), strtoull(argv[6],0,0));
    if (md == "short8") return mode_short8(strtoull(argv[2], 0, 0));
    if (md == "outcomes") return mode_outcomes(atol(argv[2]), strtoull(argv[3], 0, 0));
    printf("unknown mode\n"); return 1;
}
