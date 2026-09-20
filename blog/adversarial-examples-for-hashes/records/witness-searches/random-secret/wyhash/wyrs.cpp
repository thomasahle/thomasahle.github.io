// wyrs: wyhash final v4.3 fixed-pair collision rates under caller-chosen key models.
// Uses the upstream header verbatim (wyhash_upstream.h = wangyi-fudan/wyhash master wyhash.h).
// Build: g++ -O2 -std=c++17 -pthread -o wyrs wyrs.cpp
// Key models (secret_mode):
//   0 = default public secret _wyp, uniform 64-bit API seed
//   1 = uniform 64-bit API seed + four independent uniform 64-bit secret words   (row model)
//   2 = uniform seed + four uniform ODD words (harness convention: r.next()|1)
//   3 = uniform seed + secret = make_secret(uniform 64-bit) from a precomputed table of 2^T entries
#include "wyhash_upstream.h"
#include <atomic>
#include <chrono>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

struct Rng {
    uint64_t s[4];
    static uint64_t splitmix(uint64_t& x) { uint64_t z = (x += 0x9e3779b97f4a7c15ull); z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ull; z = (z ^ (z >> 27)) * 0x94d049bb133111ebull; return z ^ (z >> 31); }
    explicit Rng(uint64_t seed) { for (int i = 0; i < 4; i++) s[i] = splitmix(seed); }
    static uint64_t rotl(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
    uint64_t next() { uint64_t r = rotl(s[1] * 5, 7) * 9, t = s[1] << 17; s[2] ^= s[0]; s[3] ^= s[1]; s[1] ^= s[2]; s[0] ^= s[3]; s[2] ^= t; s[3] = rotl(s[3], 45); return r; }
};

static std::vector<uint64_t> g_table; // make_secret table, 4 words per entry
static int g_table_log2 = 0;

struct Msg { std::vector<uint8_t> b; };
static Msg decode(const std::string& hex) {
    Msg m; if (hex == "-") return m; if (hex.size() % 2) { fprintf(stderr, "odd hex\n"); exit(2); }
    for (size_t i = 0; i < hex.size(); i += 2) { unsigned x; if (sscanf(hex.c_str() + i, "%2x", &x) != 1) exit(2); m.b.push_back((uint8_t)x); }
    return m;
}
static std::string enc(const std::vector<uint8_t>& b) { std::string s; char t[3]; for (auto x : b) { snprintf(t, 3, "%02x", x); s += t; } return s; }

struct Witness { bool have = false; uint64_t seed = 0, sec[4] = {0, 0, 0, 0}, out = 0; };

static void sample_secret(Rng& r, int mode, uint64_t* sec) {
    switch (mode) {
    case 0: memcpy(sec, _wyp, sizeof _wyp); break;
    case 1: for (int i = 0; i < 4; i++) sec[i] = r.next(); break;
    case 2: for (int i = 0; i < 4; i++) sec[i] = r.next() | 1; break;
    case 3: { uint64_t k = r.next() >> (64 - g_table_log2); memcpy(sec, &g_table[4 * k], 32); break; }
    }
}

// Returns collision count over 2^lg trials split across threads; each thread has its own stream.
static uint64_t run_pair(const Msg& a, const Msg& b, int lg, int mode, uint64_t salt, int threads, Witness& w, double* secs) {
    std::atomic<uint64_t> total{0};
    std::mutex mu;
    uint64_t per = (uint64_t(1) << lg) / threads, rem = (uint64_t(1) << lg) % threads;
    auto t0 = std::chrono::steady_clock::now();
    std::vector<std::thread> th;
    for (int t = 0; t < threads; t++) th.emplace_back([&, t]() {
        Rng r(salt * 0x100000001b3ull + 0x9E3779B97F4A7C15ull * (t + 1) + mode * 7919);
        uint64_t n = per + (t < (int)rem ? 1 : 0), c = 0, sec[4];
        Witness lw;
        const uint8_t *pa = a.b.data(), *pb = b.b.data(); size_t la = a.b.size(), lb = b.b.size();
        for (uint64_t i = 0; i < n; i++) {
            uint64_t seed = r.next(); sample_secret(r, mode, sec);
            uint64_t ha = wyhash(pa, la, seed, sec), hb = wyhash(pb, lb, seed, sec);
            if (ha == hb) { c++; if (!lw.have) { lw.have = true; lw.seed = seed; memcpy(lw.sec, sec, 32); lw.out = ha; } }
        }
        total += c;
        std::lock_guard<std::mutex> g(mu); if (lw.have && !w.have) w = lw;
    });
    for (auto& x : th) x.join();
    *secs = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
    return total.load();
}

// Exact central 95% Poisson (Garwood) interval on a count.
static void poisson95(uint64_t k, double* lo, double* hi) {
    // use chi-square quantiles via inverse regularized gamma; simple bisection on the gamma CDF
    auto gamma_cdf_upper = [](double x, double a) { // P(Poisson(x) >= a) = regularized lower gamma(a, x)
        // regularized lower incomplete gamma via series/continued fraction (Numerical Recipes)
        if (x <= 0) return 0.0;
        double gln = lgamma(a);
        if (x < a + 1) { double ap = a, sum = 1.0 / a, del = sum; for (int n = 0; n < 1000; n++) { ap += 1; del *= x / ap; sum += del; if (fabs(del) < fabs(sum) * 1e-15) break; } return sum * exp(-x + a * log(x) - gln); }
        double b = x + 1 - a, c = 1e300, d = 1 / b, h = d;
        for (int i = 1; i < 1000; i++) { double an = -i * (i - a); b += 2; d = an * d + b; if (fabs(d) < 1e-300) d = 1e-300; c = b + an / c; if (fabs(c) < 1e-300) c = 1e-300; d = 1 / d; double del = d * c; h *= del; if (fabs(del - 1) < 1e-15) break; }
        return 1 - exp(-x + a * log(x) - gln) * h;
    };
    // lower: mu s.t. P(X >= k | mu) = 0.025  -> Q(k, mu) = 0.025 where Q = regularized lower gamma(k, mu)
    if (k == 0) *lo = 0; else { double a = 0, b = k + 10 * sqrt((double)k) + 10; for (int i = 0; i < 200; i++) { double m = (a + b) / 2; if (gamma_cdf_upper(m, (double)k) < 0.025) a = m; else b = m; } *lo = (a + b) / 2; }
    { double a = 0, b = k + 10 * sqrt((double)k) + 20; for (int i = 0; i < 200; i++) { double m = (a + b) / 2; if (gamma_cdf_upper(m, (double)k + 1) < 0.975) a = m; else b = m; } *hi = (a + b) / 2; }
}

static uint32_t smhasher_verification() {
    uint8_t key[256] = {0}, hashes[256 * 8], fin[8];
    for (int i = 0; i < 256; i++) { uint64_t h = wyhash(key, i, 256 - i, _wyp); memcpy(hashes + 8 * i, &h, 8); key[i] = (uint8_t)i; }
    uint64_t h = wyhash(hashes, sizeof hashes, 0, _wyp); memcpy(fin, &h, 8);
    return (uint32_t)fin[0] | (uint32_t)fin[1] << 8 | (uint32_t)fin[2] << 16 | (uint32_t)fin[3] << 24;
}

static void build_table(int lg, int threads) {
    g_table_log2 = lg; g_table.assign(4ull << lg, 0);
    std::vector<std::thread> th;
    for (int t = 0; t < threads; t++) th.emplace_back([&, t]() {
        Rng r(0xABCDEF ^ (uint64_t)t * 0x9E3779B97F4A7C15ull);
        for (uint64_t k = t; k < (1ull << lg); k += threads) { uint64_t s = r.next(); make_secret(s, &g_table[4 * k]); }
    });
    for (auto& x : th) x.join();
}

static void report(const char* tag, const Msg& a, const Msg& b, uint64_t n, uint64_t c, int mode, const Witness& w, double secs) {
    double lo, hi; poisson95(c, &lo, &hi);
    size_t L = (std::max(a.b.size(), b.b.size()) + 7) / 8; if (L == 0) L = 1;
    double ln = log2((double)n);
    printf("%s len=%zu/%zu L=%zu mode=%d N=2^%.0f hits=%llu", tag, a.b.size(), b.b.size(), L, mode, ln, (unsigned long long)c);
    if (c) printf(" log2rate=%.4f [%.4f,%.4f] bits=%.3f [%.3f,%.3f]", log2((double)c) - ln, log2(lo) - ln, log2(hi) - ln, log2((double)L) - log2((double)c) + ln, log2((double)L) - log2(hi) + ln, log2((double)L) - log2(lo) + ln);
    else printf(" log2rate=-inf rate<=2^%.4f(97.5%%) bits>=%.3f", log2(hi) - ln, log2((double)L) - log2(hi) + ln);
    printf(" secs=%.1f\n", secs);
    if (w.have) printf("  witness seed=%016llx secret=%016llx,%016llx,%016llx,%016llx out=%016llx\n", (unsigned long long)w.seed, (unsigned long long)w.sec[0], (unsigned long long)w.sec[1], (unsigned long long)w.sec[2], (unsigned long long)w.sec[3], (unsigned long long)w.out);
    fflush(stdout);
}

int main(int argc, char** argv) {
    uint32_t v = smhasher_verification();
    if (v != 0x9DAE7DD3u) { fprintf(stderr, "SMHasher3 verification %08X != 9DAE7DD3\n", v); return 1; }
    if (argc < 2) { fprintf(stderr, "usage: wyrs pair hexA hexB lg mode salt threads | batch file lg mode salt threads [table_lg]\n"); return 2; }
    std::string cmd = argv[1];
    if (cmd == "pair") {
        if (argc < 8) return 2;
        Msg a = decode(argv[2]), b = decode(argv[3]); int lg = atoi(argv[4]), mode = atoi(argv[5]); uint64_t salt = strtoull(argv[6], 0, 0); int threads = atoi(argv[7]);
        if (mode == 3) build_table(argc > 8 ? atoi(argv[8]) : 20, threads);
        printf("verification 9DAE7DD3 PASS\n");
        Witness w; double secs; uint64_t c = run_pair(a, b, lg, mode, salt, threads, w, &secs);
        report("pair", a, b, 1ull << lg, c, mode, w, secs);
        return 0;
    }
    if (cmd == "batch") { // file lines: tag hexA hexB [lg]; "-" denotes the empty message
        if (argc < 7) return 2;
        std::ifstream f(argv[2]); int lg0 = atoi(argv[3]), mode = atoi(argv[4]); uint64_t salt = strtoull(argv[5], 0, 0); int threads = atoi(argv[6]);
        if (mode == 3) build_table(argc > 7 ? atoi(argv[7]) : 20, threads);
        printf("verification 9DAE7DD3 PASS\n");
        std::string tag, ha, hb; int lg;
        std::string line;
        while (std::getline(f, line)) {
            if (line.empty() || line[0] == '#') continue;
            char t[256], x[2048], y[2048]; lg = lg0; int k = sscanf(line.c_str(), "%255s %2047s %2047s %d", t, x, y, &lg);
            if (k < 3) continue;
            Msg a = decode(x), b = decode(y);
            if (a.b == b.b) { printf("%s SKIP identical\n", t); continue; }
            Witness w; double secs; uint64_t c = run_pair(a, b, lg, mode, salt, threads, w, &secs);
            report(t, a, b, 1ull << lg, c, mode, w, secs);
        }
        return 0;
    }
    return 2;
}
