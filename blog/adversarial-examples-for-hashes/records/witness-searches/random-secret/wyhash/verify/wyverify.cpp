// wyverify: independent re-measurement of fixed-pair collision rates for wyhash
// (final v4.3, upstream header verbatim) under the random-secret key model:
// per trial, a uniform 64-bit API seed and four independent uniform 64-bit
// secret words are drawn, and the two fixed messages are hashed with them.
// Written from scratch as a cross-check of a separate search harness; shares
// no code with it (different RNG, different threading, different reporting).
//
// build: g++ -O2 -std=c++17 -pthread -o wyverify wyverify.cpp
// usage: ./wyverify hexA hexB lg threads salt rng model
//   lg      trials = 2^lg (split evenly over threads)
//   rng     mt   = std::mt19937_64 seeded by seed_seq(salt, thread)
//           sm   = splitmix64 stream seeded by (salt, thread)
//   model   random  = uniform seed + 4 uniform secret words (the row model)
//           default = uniform seed, public default secret _wyp
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <random>
#include <string>
#include <thread>
#include <vector>
#include <chrono>
#include "wyhash_upstream.h"

static std::vector<uint8_t> hex(const char* s) {
    std::vector<uint8_t> v; size_t n = strlen(s);
    if (n % 2) { fprintf(stderr, "odd hex length\n"); exit(2); }
    for (size_t i = 0; i < n; i += 2) v.push_back((uint8_t)strtoul(std::string(s + i, 2).c_str(), 0, 16));
    return v;
}

struct SplitMix { uint64_t x; uint64_t next() { uint64_t z = (x += 0x9e3779b97f4a7c15ull); z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ull; z = (z ^ (z >> 27)) * 0x94d049bb133111ebull; return z ^ (z >> 31); } };

struct Result { uint64_t hits = 0, trials = 0; bool have = false; uint64_t wseed = 0, wsec[4] = {0,0,0,0}, wout = 0; };

// Self-check 1: upstream test_vector.cpp messages, seed = index, default secret.
static void self_check() {
    const char* msgs[] = {"", "a", "abc", "message digest", "abcdefghijklmnopqrstuvwxyz",
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
        "12345678901234567890123456789012345678901234567890123456789012345678901234567890"};
    printf("test vectors (upstream test_vector.cpp: wyhash(msg, len, i, _wyp)):\n");
    for (int i = 0; i < 7; i++) printf("  %016llx  \"%s\", %d\n", (unsigned long long)wyhash(msgs[i], strlen(msgs[i]), i, _wyp), msgs[i], i);
    // Self-check 2: SMHasher-style verification value (key[i]=i, len i, seed 256-i; then hash the 256*8-byte buffer with seed 0).
    uint8_t key[256], buf[256 * 8];
    for (int i = 0; i < 256; i++) { key[i] = (uint8_t)i; uint64_t h = wyhash(key, i, 256 - i, _wyp); memcpy(buf + 8 * i, &h, 8); }
    uint64_t f = wyhash(buf, sizeof buf, 0, _wyp); uint32_t v; memcpy(&v, &f, 4);
    printf("SMHasher verification value: 0x%08X\n", v);
}

int main(int argc, char** argv) {
    if (argc != 8) { fprintf(stderr, "usage: wyverify hexA hexB lg threads salt rng(mt|sm) model(random|default)\n"); return 2; }
    self_check();
    std::vector<uint8_t> A = hex(argv[1]), B = hex(argv[2]);
    int lg = atoi(argv[3]), T = atoi(argv[4]); uint64_t salt = strtoull(argv[5], 0, 0);
    std::string rng = argv[6], model = argv[7];
    bool use_mt = rng == "mt", random_secret = model == "random";
    if (A == B) { fprintf(stderr, "messages identical\n"); return 2; }
    uint64_t total = 1ull << lg, per = total / T;
    std::vector<Result> res(T); std::vector<std::thread> th;
    auto t0 = std::chrono::steady_clock::now();
    for (int t = 0; t < T; t++) th.emplace_back([&, t] {
        Result& r = res[t];
        std::seed_seq sq{(uint32_t)salt, (uint32_t)(salt >> 32), (uint32_t)t, 0x57795679u};
        std::mt19937_64 mt(sq);
        SplitMix sm{salt ^ (0x9e3779b97f4a7c15ull * (uint64_t)(t + 1))};
        for (int k = 0; k < 8; k++) sm.next(); // decorrelate from the seeding constant
        uint64_t sec[4];
        for (uint64_t n = 0; n < per; n++) {
            uint64_t seed;
            if (use_mt) { seed = mt(); if (random_secret) for (int k = 0; k < 4; k++) sec[k] = mt(); }
            else        { seed = sm.next(); if (random_secret) for (int k = 0; k < 4; k++) sec[k] = sm.next(); }
            const uint64_t* s = random_secret ? sec : _wyp;
            uint64_t h1 = wyhash(A.data(), A.size(), seed, s);
            uint64_t h2 = wyhash(B.data(), B.size(), seed, s);
            if (h1 == h2) { r.hits++; if (!r.have) { r.have = true; r.wseed = seed; memcpy(r.wsec, s, 32); r.wout = h1; } }
        }
        r.trials = per;
    });
    for (auto& x : th) x.join();
    double secs = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
    uint64_t hits = 0, trials = 0; const Result* w = nullptr;
    for (auto& r : res) { hits += r.hits; trials += r.trials; if (r.have && !w) w = &r; }
    printf("RESULT lenA=%zu lenB=%zu model=%s rng=%s salt=%llu threads=%d trials=%llu hits=%llu", A.size(), B.size(), model.c_str(), rng.c_str(),
           (unsigned long long)salt, T, (unsigned long long)trials, (unsigned long long)hits);
    if (hits) printf(" log2rate=%.4f", std::log2((double)hits / (double)trials));
    printf(" secs=%.1f\n", secs);
    if (w) printf("  witness seed=%016llx secret=%016llx,%016llx,%016llx,%016llx out=%016llx\n", (unsigned long long)w->wseed,
                  (unsigned long long)w->wsec[0], (unsigned long long)w->wsec[1], (unsigned long long)w->wsec[2], (unsigned long long)w->wsec[3], (unsigned long long)w->wout);
    return 0;
}
