// rs_rapid1.cpp -- rapidhash v1.0 under the RANDOM-SECRET key model.
//
// Key model: the 64-bit seed AND the three 64-bit secret words are independent
// uniform hidden key material (256 bits).  The hash is the upstream v1.0 header
// (rapidhash.h, tag rapidhash_v1.0, RAPIDHASH_FAST + RAPIDHASH_UNROLLED defaults),
// called through rapidhash_internal(key, len, seed, secret) -- the only entry
// point that accepts a caller-supplied secret triplet.
//
// Build:  g++ -O3 -std=c++17 -march=native -pthread rs_rapid1.cpp -o rs_rapid1
//
// Modes (N = 2^log2N keys, each drawn from xoshiro256** seeded by splitmix64(rngseed + thread)):
//   pair  <hexM> <hexM'> <log2N> <rngseed> [random|default|odd] [threads]
//         collision count of the fixed pair over N keys; prints rate, 95% CI, an example key.
//   fold  <d0hex> <d1hex> <log2N> <rngseed> [threads]
//         P(d0,d1) = Pr_{A,B uniform}[ mix(A,B) = mix(A^d0, B^d1) ], mix = lo ^ hi of the 128-bit product.
//   hill  <d0hex> <d1hex> <log2N> <rngseed> [threads]
//         P for (d0,d1) and all 128 single-bit XOR neighbours, sorted.
//   scan  <log2N> <rngseed> [threads]
//         hash-level sweep: lengths 1..64 and block boundaries, structural + low-weight + random pairs.
#include "rapidhash.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cmath>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <algorithm>

typedef unsigned __int128 u128;

/* ---------------- RNG: xoshiro256** seeded from splitmix64 ---------------- */
struct Rng {
    uint64_t s[4];
    static uint64_t splitmix(uint64_t& x) { uint64_t z = (x += 0x9e3779b97f4a7c15ull); z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ull; z = (z ^ (z >> 27)) * 0x94d049bb133111ebull; return z ^ (z >> 31); }
    explicit Rng(uint64_t seed) { for (int i = 0; i < 4; i++) s[i] = splitmix(seed); }
    static inline uint64_t rotl(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
    inline uint64_t next() { uint64_t r = rotl(s[1] * 5, 7) * 9, t = s[1] << 17; s[2] ^= s[0]; s[3] ^= s[1]; s[1] ^= s[2]; s[0] ^= s[3]; s[2] ^= t; s[3] = rotl(s[3], 45); return r; }
};

/* ---------------- key models ---------------- */
enum KeyModel { KM_RANDOM = 0, KM_DEFAULT = 1, KM_ODD = 2 };
static KeyModel parse_model(const char* s) {
    if (!strcmp(s, "random")) return KM_RANDOM;
    if (!strcmp(s, "default")) return KM_DEFAULT;
    if (!strcmp(s, "odd")) return KM_ODD;
    fprintf(stderr, "unknown key model %s\n", s); exit(2);
}
static const char* model_name(KeyModel m) { return m == KM_RANDOM ? "random seed + 3 uniform secret words" : m == KM_DEFAULT ? "random seed, default secret" : "random seed + 3 odd secret words"; }
static inline void draw_key(Rng& r, KeyModel m, uint64_t& seed, uint64_t sec[3]) {
    seed = r.next();
    if (m == KM_DEFAULT) { sec[0] = rapid_secret[0]; sec[1] = rapid_secret[1]; sec[2] = rapid_secret[2]; }
    else for (int i = 0; i < 3; i++) sec[i] = m == KM_ODD ? (r.next() | 1) : r.next();
}

/* ---------------- statistics: exact Poisson (Garwood) 95% interval ---------------- */
static double gammaln(double x) { return lgamma(x); }
static double gamma_p(double a, double x) {            // regularized lower incomplete gamma P(a,x)
    if (x <= 0) return 0;
    if (x < a + 1) { double ap = a, sum = 1.0 / a, del = sum; for (int n = 0; n < 10000; n++) { ap += 1; del *= x / ap; sum += del; if (fabs(del) < fabs(sum) * 1e-15) break; } return sum * exp(-x + a * log(x) - gammaln(a)); }
    double b = x + 1 - a, c = 1e300, d = 1 / b, h = d;
    for (int i = 1; i < 10000; i++) { double an = -i * (i - a); b += 2; d = an * d + b; if (fabs(d) < 1e-300) d = 1e-300; c = b + an / c; if (fabs(c) < 1e-300) c = 1e-300; d = 1 / d; double del = d * c; h *= del; if (fabs(del - 1) < 1e-15) break; }
    return 1 - exp(-x + a * log(x) - gammaln(a)) * h;
}
static double poisson_quantile_mean(double count_param, double prob) { // mean m with P(Gamma(shape=count_param) <= m) = prob
    if (count_param <= 0) return 0;
    double lo = 0, hi = std::max(10.0, count_param * 5 + 50);
    for (int i = 0; i < 200; i++) { double mid = 0.5 * (lo + hi); if (gamma_p(count_param, mid) < prob) lo = mid; else hi = mid; }
    return 0.5 * (lo + hi);
}
static void garwood95(uint64_t c, double& lo, double& hi) {   // central 95% interval for the Poisson mean given count c
    lo = c == 0 ? 0 : poisson_quantile_mean((double)c, 0.025);
    hi = poisson_quantile_mean((double)c + 1, 0.975);
}

/* ---------------- helpers ---------------- */
static std::vector<uint8_t> unhex(const std::string& h) {
    std::vector<uint8_t> v; if (h.size() % 2) { fprintf(stderr, "odd hex\n"); exit(2); }
    for (size_t i = 0; i < h.size(); i += 2) v.push_back((uint8_t)strtoul(h.substr(i, 2).c_str(), nullptr, 16));
    return v;
}
static std::string hex(const std::vector<uint8_t>& v) { std::string s; char b[4]; for (uint8_t x : v) { snprintf(b, 4, "%02x", x); s += b; } return s; }
static inline uint64_t mixf(uint64_t a, uint64_t b) { u128 r = (u128)a * b; return (uint64_t)r ^ (uint64_t)(r >> 64); }

struct Hit { bool have = false; uint64_t seed, sec[3], out; };

static uint64_t count_pair(const std::vector<uint8_t>& m1, const std::vector<uint8_t>& m2, uint64_t N, uint64_t rngseed, KeyModel km, int threads, Hit* ex) {
    std::vector<std::thread> th; std::vector<uint64_t> cnt(threads, 0); std::vector<Hit> hits(threads);
    uint64_t per = N / threads;
    for (int t = 0; t < threads; t++) th.emplace_back([&, t]() {
        Rng r(rngseed * 0x100000000ull + (uint64_t)t); uint64_t c = 0, seed, sec[3];
        for (uint64_t i = 0; i < per; i++) {
            draw_key(r, km, seed, sec);
            uint64_t h1 = rapidhash_internal(m1.data(), m1.size(), seed, sec);
            uint64_t h2 = rapidhash_internal(m2.data(), m2.size(), seed, sec);
            if (h1 == h2) { c++; if (!hits[t].have) { hits[t].have = true; hits[t].seed = seed; memcpy(hits[t].sec, sec, 24); hits[t].out = h1; } }
        }
        cnt[t] = c;
    });
    for (auto& x : th) x.join();
    uint64_t total = 0; for (int t = 0; t < threads; t++) { total += cnt[t]; if (ex && !ex->have && hits[t].have) *ex = hits[t]; }
    return total;
}

static void report(const char* label, uint64_t hits, uint64_t N, size_t L, const Hit* ex) {
    double lo, hi; garwood95(hits, lo, hi);
    double rate = (double)hits / N;
    printf("%s: hits=%llu N=2^%.0f rate=%.4e", label, (unsigned long long)hits, log2((double)N), rate);
    if (hits) printf(" log2rate=%.4f [%.4f, %.4f] cap_bits(L=%zu)=%.3f [%.3f, %.3f]", log2(rate), log2(lo / N), log2(hi / N), L, log2((double)L) - log2(rate), log2((double)L) - log2(hi / N), log2((double)L) - log2(lo / N));
    else printf(" none; 95%% upper rate %.3e = 2^%.2f -> cap_bits(L=%zu) >= %.2f", hi / N, log2(hi / N), L, log2((double)L) - log2(hi / N));
    if (ex && ex->have) printf("  example seed=%016llx secret={%016llx,%016llx,%016llx} out=%016llx", (unsigned long long)ex->seed, (unsigned long long)ex->sec[0], (unsigned long long)ex->sec[1], (unsigned long long)ex->sec[2], (unsigned long long)ex->out);
    printf("\n"); fflush(stdout);
}

static uint64_t count_fold(uint64_t d0, uint64_t d1, uint64_t N, uint64_t rngseed, int threads) {
    std::vector<std::thread> th; std::vector<uint64_t> cnt(threads, 0); uint64_t per = N / threads;
    for (int t = 0; t < threads; t++) th.emplace_back([&, t]() {
        Rng r(rngseed * 0x100000000ull + 0x7000 + (uint64_t)t); uint64_t c = 0;
        for (uint64_t i = 0; i < per; i++) { uint64_t a = r.next(), b = r.next(); c += mixf(a, b) == mixf(a ^ d0, b ^ d1); }
        cnt[t] = c;
    });
    for (auto& x : th) x.join();
    uint64_t total = 0; for (auto c : cnt) total += c; return total;
}

/* ---------------- scan candidates ---------------- */
struct Cand { std::string name; std::vector<uint8_t> m1, m2; };
static std::vector<uint8_t> rndmsg(Rng& r, size_t len) { std::vector<uint8_t> v(len); for (size_t i = 0; i < len; i++) v[i] = (uint8_t)r.next(); return v; }
static void add(std::vector<Cand>& out, const std::string& name, std::vector<uint8_t> a, std::vector<uint8_t> b) { if (a == b) return; out.push_back({name, std::move(a), std::move(b)}); }
static std::vector<Cand> candidates(size_t len, Rng& r, int nrandom) {
    std::vector<Cand> v; char nm[128];
    std::vector<uint8_t> base = rndmsg(r, len);
    // random pairs
    for (int k = 0; k < nrandom; k++) { snprintf(nm, sizeof nm, "len%zu random#%d", len, k); add(v, nm, rndmsg(r, len), rndmsg(r, len)); }
    // full complement
    { auto b = base; for (auto& x : b) x = ~x; snprintf(nm, sizeof nm, "len%zu complement-all", len); add(v, nm, base, b); }
    // single-bit flips and byte complements at every byte
    for (size_t i = 0; i < len; i++) {
        { auto b = base; b[i] ^= 1; snprintf(nm, sizeof nm, "len%zu bit0 byte%zu", len, i); add(v, nm, base, b); }
        { auto b = base; b[i] ^= 0x80; snprintf(nm, sizeof nm, "len%zu bit7 byte%zu", len, i); add(v, nm, base, b); }
        { auto b = base; b[i] = ~b[i]; snprintf(nm, sizeof nm, "len%zu complement byte%zu", len, i); add(v, nm, base, b); }
    }
    // complement of each 8-byte word window (aligned) and of each adjacent pair, and of each 8-byte window at every offset
    for (size_t o = 0; o + 8 <= len; o++) { auto b = base; for (size_t j = o; j < o + 8; j++) b[j] = ~b[j]; snprintf(nm, sizeof nm, "len%zu complement bytes[%zu..%zu]", len, o, o + 7); add(v, nm, base, b); }
    for (size_t o = 0; o + 16 <= len; o += 8) { auto b = base; for (size_t j = o; j < o + 16; j++) b[j] = ~b[j]; snprintf(nm, sizeof nm, "len%zu complement words at %zu,%zu", len, o / 8, o / 8 + 1); add(v, nm, base, b); }
    // 4-byte chunk complements (short-length a/b construction)
    for (size_t o = 0; o + 4 <= len; o++) { auto b = base; for (size_t j = o; j < o + 4; j++) b[j] = ~b[j]; snprintf(nm, sizeof nm, "len%zu complement bytes[%zu..%zu]", len, o, o + 3); add(v, nm, base, b); }
    // short-length operand complements: a = first4||last4, b = bytes[delta..delta+4)||bytes[len-4-delta..len-delta)
    if (len >= 4 && len <= 16) {
        size_t delta = (len & 24) >> (len >> 3);
        auto b = base; for (size_t j = 0; j < 4; j++) { b[j] = ~b[j]; b[len - 4 + j] = ~b[len - 4 + j]; }
        snprintf(nm, sizeof nm, "len%zu complement a-operand bytes", len); add(v, nm, base, b);
        auto c = base; for (size_t j = 0; j < 4; j++) { c[delta + j] = ~c[delta + j]; c[len - 4 - delta + j] = ~c[len - 4 - delta + j]; }
        snprintf(nm, sizeof nm, "len%zu complement b-operand bytes", len); add(v, nm, base, c);
    }
    // swap of adjacent 8-byte words and of 4-byte halves (commutativity probes)
    for (size_t o = 0; o + 16 <= len; o += 8) { auto b = base; for (size_t j = 0; j < 8; j++) std::swap(b[o + j], b[o + 8 + j]); snprintf(nm, sizeof nm, "len%zu swap words %zu,%zu", len, o / 8, o / 8 + 1); add(v, nm, base, b); }
    // cross-length: same content, one byte appended / prepended
    { auto b = base; b.push_back(0); snprintf(nm, sizeof nm, "len%zu vs len%zu append 0x00", len, len + 1); add(v, nm, base, b); }
    { auto b = base; b.insert(b.begin(), 0); snprintf(nm, sizeof nm, "len%zu vs len%zu prepend 0x00", len, len + 1); add(v, nm, base, b); }
    return v;
}

int main(int argc, char** argv) {
    if (argc < 2) { fprintf(stderr, "usage: see header\n"); return 2; }
    std::string mode = argv[1];
    if (mode == "pair") {
        if (argc < 6) { fprintf(stderr, "pair <hexM> <hexM'> <log2N> <rngseed> [model] [threads]\n"); return 2; }
        auto m1 = unhex(argv[2]), m2 = unhex(argv[3]); int lg = atoi(argv[4]); uint64_t rs = strtoull(argv[5], 0, 0);
        KeyModel km = argc > 6 ? parse_model(argv[6]) : KM_RANDOM; int threads = argc > 7 ? atoi(argv[7]) : 8;
        size_t L = (std::max(m1.size(), m2.size()) + 7) / 8;
        printf("rapidhash v1.0 pair: M=%s (%zu B) M'=%s (%zu B) L=%zu model=%s rngseed=%llu threads=%d\n", hex(m1).c_str(), m1.size(), hex(m2).c_str(), m2.size(), L, model_name(km), (unsigned long long)rs, threads);
        Hit ex; uint64_t N = 1ull << lg; uint64_t h = count_pair(m1, m2, N, rs, km, threads, &ex);
        report("result", h, N, L, &ex);
    } else if (mode == "fold") {
        if (argc < 6) { fprintf(stderr, "fold <d0> <d1> <log2N> <rngseed> [threads]\n"); return 2; }
        uint64_t d0 = strtoull(argv[2], 0, 16), d1 = strtoull(argv[3], 0, 16); int lg = atoi(argv[4]); uint64_t rs = strtoull(argv[5], 0, 0); int threads = argc > 6 ? atoi(argv[6]) : 8;
        uint64_t N = 1ull << lg, h = count_fold(d0, d1, N, rs, threads);
        char lab[80]; snprintf(lab, sizeof lab, "fold(%016llx,%016llx)", (unsigned long long)d0, (unsigned long long)d1);
        report(lab, h, N, 1, nullptr);
    } else if (mode == "hill") {
        if (argc < 6) { fprintf(stderr, "hill <d0> <d1> <log2N> <rngseed> [threads]\n"); return 2; }
        uint64_t d0 = strtoull(argv[2], 0, 16), d1 = strtoull(argv[3], 0, 16); int lg = atoi(argv[4]); uint64_t rs = strtoull(argv[5], 0, 0); int threads = argc > 6 ? atoi(argv[6]) : 8;
        uint64_t N = 1ull << lg;
        struct R { uint64_t d0, d1, h; };
        std::vector<R> rows; rows.push_back({d0, d1, count_fold(d0, d1, N, rs, threads)});
        for (int i = 0; i < 128; i++) { uint64_t e0 = d0 ^ (i < 64 ? 1ull << i : 0), e1 = d1 ^ (i >= 64 ? 1ull << (i - 64) : 0); rows.push_back({e0, e1, count_fold(e0, e1, N, rs, threads)}); }
        std::sort(rows.begin(), rows.end(), [](const R& a, const R& b) { return a.h > b.h; });
        printf("hill-climb around (%016llx,%016llx), N=2^%d per candidate, base hits=%llu; top 12 of 129:\n", (unsigned long long)d0, (unsigned long long)d1, lg, (unsigned long long)rows[0].h);
        for (auto& r : rows) if (r.d0 == d0 && r.d1 == d1) printf("  base  (%016llx,%016llx) hits=%llu log2=%.3f\n", (unsigned long long)r.d0, (unsigned long long)r.d1, (unsigned long long)r.h, r.h ? log2((double)r.h / N) : -INFINITY);
        for (int i = 0; i < 12; i++) printf("  #%-2d   (%016llx,%016llx) hits=%llu log2=%.3f\n", i + 1, (unsigned long long)rows[i].d0, (unsigned long long)rows[i].d1, (unsigned long long)rows[i].h, rows[i].h ? log2((double)rows[i].h / N) : -INFINITY);
        uint64_t s = 0; for (size_t i = 1; i < rows.size(); i++) s += rows[i].h; printf("  neighbours: total hits %llu over 128 (mean %.2f)\n", (unsigned long long)s, s / 128.0);
    } else if (mode == "scan") {
        if (argc < 4) { fprintf(stderr, "scan <log2N> <rngseed> [threads] [maxlen]\n"); return 2; }
        int lg = atoi(argv[2]); uint64_t rs = strtoull(argv[3], 0, 0); int threads = argc > 4 ? atoi(argv[4]) : 8; int maxlen = argc > 5 ? atoi(argv[5]) : 64;
        uint64_t N = 1ull << lg; Rng cr(rs ^ 0xabcdef);
        std::vector<size_t> lens; for (int l = 1; l <= maxlen; l++) lens.push_back(l);
        for (size_t l : {65, 80, 95, 96, 97, 112, 128, 143, 144, 145, 160, 192, 193}) if ((int)l > maxlen) lens.push_back(l);
        uint64_t ncand = 0, nhit = 0;
        printf("scan: N=2^%d keys per candidate, model=%s; candidates with >0 hits are listed\n", lg, model_name(KM_RANDOM));
        for (size_t len : lens) {
            auto cs = candidates(len, cr, 4);
            uint64_t lhits = 0;
            for (auto& c : cs) {
                Hit ex; uint64_t h = count_pair(c.m1, c.m2, N, rs + 1000 + len, KM_RANDOM, threads, &ex); ncand++;
                if (h) { nhit++; lhits += h; size_t L = (std::max(c.m1.size(), c.m2.size()) + 7) / 8; printf("HIT %s M=%s M'=%s ", c.name.c_str(), hex(c.m1).c_str(), hex(c.m2).c_str()); report("", h, N, L, &ex); }
            }
            printf("len %zu: %zu candidates, %llu total hits\n", len, cs.size(), (unsigned long long)lhits); fflush(stdout);
        }
        double lo, hi; garwood95(0, lo, hi);
        printf("scan done: %llu candidates, %llu with hits; per-candidate floor at 0 hits: rate < %.3e = 2^%.2f (95%%)\n", (unsigned long long)ncand, (unsigned long long)nhit, hi / N, log2(hi / N));
    } else if (mode == "selftest") {
        const char* msg = "message digest"; uint64_t h = rapidhash_internal(msg, 14, 3, rapid_secret);
        printf("rapidhash_internal(\"message digest\", 14, seed 3, default secret) = %016llx expected 0031cdc21324150f %s\n", (unsigned long long)h, h == 0x0031cdc21324150full ? "PASS" : "FAIL");
        auto m1 = unhex("9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c"), m2 = unhex("642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c");
        uint64_t a = rapidhash_internal(m1.data(), 32, 0x3788f2419a81e2d6ull, rapid_secret), b = rapidhash_internal(m2.data(), 32, 0x3788f2419a81e2d6ull, rapid_secret);
        printf("row witness seed 3788f2419a81e2d6: %016llx %016llx expected 8f7a71ffebd4a14b %s\n", (unsigned long long)a, (unsigned long long)b, (a == b && a == 0x8f7a71ffebd4a14bull) ? "PASS" : "FAIL");
        return (h == 0x0031cdc21324150full && a == b && a == 0x8f7a71ffebd4a14bull) ? 0 : 1;
    } else { fprintf(stderr, "unknown mode\n"); return 2; }
    return 0;
}
