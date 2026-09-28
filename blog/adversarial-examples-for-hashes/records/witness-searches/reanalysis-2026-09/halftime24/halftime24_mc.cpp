/* halftime24_mc.cpp -- HalftimeHash advanced 24-byte API experiments.  Uses the unmodified upstream
 * header halftime-hash.hpp (jbapple/HalftimeHash caf7924, sha256 7ef5dd48...; see SOURCES.md). */
#include "halftime-hash.hpp"
/* ======================================================================================
 * HalftimeHash24 driver.  The upstream header is included unmodified
 * (sha256 7ef5dd48f545...).  Build: g++ -O2 -march=native -std=c++17 -o halftime24_mc halftime24_mc.cpp
 *
 * Entry points: halftime_hash::advanced::V1<3> .. V4<3> (24-byte output, block width b = 1,2,4,8).
 * Key model: independent uniform 64-bit entropy words (the first NW words are drawn per trial; the
 * 168b-byte and shorter messages read far fewer).  core_key[i] = entropy[i].
 * No SMHasher3 value exists for this API.  Start-up check: on x86-64 with AVX-512 the dispatched
 * V2..V4 (Sse2 / Avx2 / Avx512) must equal the header's own scalar V2Scalar..V4Scalar on random
 * inputs of many lengths (the header defines them as the same function); on other hosts only the
 * scalar path exists and the check is reported as skipped.
 * ====================================================================================== */
#include <cstdio>
#include <cstdlib>
#include <cinttypes>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>

using namespace halftime_hash::advanced;
static const int NW = 1024;
struct Rng {
    uint64_t s[4];
    static uint64_t sm(uint64_t & x) { uint64_t z = (x += 0x9e3779b97f4a7c15ull); z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ull; z = (z ^ (z >> 27)) * 0x94d049bb133111ebull; return z ^ (z >> 31); }
    explicit Rng(uint64_t seed) { for (int i = 0; i < 4; i++) s[i] = sm(seed); }
    static uint64_t rotl(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
    uint64_t next() { uint64_t r = rotl(s[1] * 5, 7) * 9, t = s[1] << 17; s[2] ^= s[0]; s[3] ^= s[1]; s[1] ^= s[2]; s[0] ^= s[3]; s[2] ^= t; s[3] = rotl(s[3], 45); return r; }
};
static void Hb(int b, const uint64_t * e, const char * m, size_t n, uint64_t o[3]) {
    switch (b) { case 1: V1<3>(e, m, n, o); break; case 2: V2<3>(e, m, n, o); break; case 4: V3<3>(e, m, n, o); break; default: V4<3>(e, m, n, o); }
}
static void HbScalar(int b, const uint64_t * e, const char * m, size_t n, uint64_t o[3]) {
    switch (b) { case 1: V1Scalar<7, 3, 9, 3>(e, m, n, o); break; case 2: V2Scalar<7, 3, 9, 3>(e, m, n, o); break; case 4: V3Scalar<7, 3, 9, 3>(e, m, n, o); break; default: V4Scalar<7, 3, 9, 3>(e, m, n, o); }
}
static bool eq3(const uint64_t a[3], const uint64_t b[3]) { return a[0] == b[0] && a[1] == b[1] && a[2] == b[2]; }
static void keygen(Rng & r, uint64_t * e) { for (int i = 0; i < NW; i++) e[i] = r.next(); }

/* pair P(b, len, word): zero message vs same with LE 64-bit word `word` = 1 */
static int mode_class(int b, size_t len, int word, int kw, long n, uint64_t rs) {
    std::vector<char> a(len, 0), c(len, 0); uint64_t one = 1; memcpy(&c[8 * word], &one, 8);
    std::vector<uint64_t> e(NW); Rng r(rs); uint64_t o1[3], o2[3];
    long hz = 0, hnz = 0, hu = 0, nz = 0;
    for (long i = 0; i < n; i++) {
        keygen(r, e.data()); e[kw] &= 0xffffffffull;                 /* class: high32(core_key[kw]) = 0 */
        Hb(b, e.data(), a.data(), len, o1); Hb(b, e.data(), c.data(), len, o2); hz += eq3(o1, o2);
        keygen(r, e.data()); if (!(e[kw] >> 32)) e[kw] |= 1ull << 32;  /* complement */
        Hb(b, e.data(), a.data(), len, o1); Hb(b, e.data(), c.data(), len, o2); hnz += eq3(o1, o2);
        keygen(r, e.data()); Hb(b, e.data(), a.data(), len, o1); Hb(b, e.data(), c.data(), len, o2); hu += eq3(o1, o2); nz += !(e[kw] >> 32);
    }
    printf("class b=%d len=%zu word=%d cond high32(core_key[%d])=0: in-class %ld/%ld, out-of-class %ld/%ld, uniform %ld/%ld\n", b, len, word, kw, hz, n, hnz, n, hu, n);
    return 0;
}
/* 2^m family: member j = zero message with the low half of block `blk` lane 0 = j (b-lane blocks) */
static int mode_family(int b, int blk, int m, long n, uint64_t rs) {
    size_t len = 168 * b, k = (size_t)1 << m; std::vector<char> msgs(k * len, 0);
    for (size_t j = 0; j < k; j++) { uint32_t v = (uint32_t)j; memcpy(&msgs[j * len + 8 * b * blk], &v, 4); }
    std::vector<uint64_t> e(NW); Rng r(rs); long all = 0, anyp = 0;
    for (long i = 0; i < n; i++) {
        keygen(r, e.data()); e[blk] &= 0xffffffffull;
        uint64_t o0[3], o[3]; Hb(b, e.data(), &msgs[0], len, o0); bool ok = true;
        for (size_t j = 1; j < k && ok; j++) { Hb(b, e.data(), &msgs[j * len], len, o); ok = eq3(o, o0); }
        all += ok;
    }
    for (long i = 0; i < n; i++) {
        keygen(r, e.data()); uint64_t o0[3], o[3]; Hb(b, e.data(), &msgs[0], len, o0);
        for (size_t j = 1; j < 64; j++) { Hb(b, e.data(), &msgs[j * len], len, o); if (eq3(o, o0)) { anyp++; break; } }
    }
    printf("family b=%d block %d k=2^%d (%zu B): class keys, whole family equal %ld/%ld; uniform keys, any of 63 members equal to member 0: %ld/%ld\n", b, blk, m, len, all, n, anyp, n);
    return 0;
}
/* key-free window: zero messages of lengths w*8b .. w*8b+8b-1 collide for every key; the next length does not */
static int mode_keyfree(int b, long n, uint64_t rs) {
    std::vector<uint64_t> e(NW); Rng r(rs); std::vector<char> z(4096, 0);
    for (int base : {0, 8 * b, 168 * b, 168 * b + 16 * b}) {
        long allin = 0, next = 0;
        for (long i = 0; i < n; i++) {
            keygen(r, e.data()); uint64_t o0[3], o[3]; Hb(b, e.data(), z.data(), base, o0); bool ok = true;
            for (int l = base + 1; l < base + 8 * b; l++) { Hb(b, e.data(), z.data(), l, o); ok = ok && eq3(o, o0); }
            allin += ok; Hb(b, e.data(), z.data(), base + 8 * b, o); next += eq3(o, o0);
        }
        printf("keyfree b=%d: zero messages of lengths %d..%d all equal for %ld/%ld keys; length %d equal to them for %ld/%ld\n", b, base, base + 8 * b - 1, allin, n, base + 8 * b, next, n);
    }
    return 0;
}
static int selfcheck(void) {
#if defined(__AVX512F__)
    Rng r(12345); std::vector<uint64_t> e(NW); std::vector<char> m(20000); long bad = 0, tot = 0;
    for (int t = 0; t < 200; t++) {
        keygen(r, e.data()); for (auto & c : m) c = (char)r.next();
        for (int b : {2, 4, 8}) for (size_t len : {0ul, 1ul, 7ul, 8ul, 63ul, 168ul, 336ul, 1343ul, 1344ul, 1345ul, 3000ul, 10752ul, 12000ul, 19999ul}) {
            uint64_t o1[3], o2[3]; Hb(b, e.data(), m.data(), len, o1); HbScalar(b, e.data(), m.data(), len, o2); bad += !eq3(o1, o2); tot++;
        }
    }
    printf("self-check: dispatched V2Sse2/V3Avx2/V4Avx512 vs header scalar V2..V4Scalar: %ld mismatches in %ld\n", bad, tot);
    return bad ? 1 : 0;
#else
    printf("self-check skipped (no AVX-512 on this host; V1..V4 resolve to the scalar path)\n");
    return 0;
#endif
}

/* mode long: V4<3> (b = 8) on a random message of len bytes (any tail).  Member j of a 2^m family flips the
 * low half of lane 0 of block 6 in m random leaves (leaf = 1344 B) according to the bits of j (value = random
 * nonzero 32-bit per leaf).  Class keys: high32(core_key[6]) = 0.  The base-layer key is the same for every
 * leaf, so all leaves share one condition.  Entropy: NWL words, all random. */
static const int NWL = 1 << 15;
static int mode_long(size_t len, int m, long nkeys, uint64_t rs) {
    const int b = 8; Rng r(rs); std::vector<char> base(len); for (auto & c : base) c = (char)r.next();
    size_t leaves = len / (168 * b); std::vector<size_t> pos; std::vector<uint32_t> val;
    for (int i = 0; i < m; i++) { pos.push_back((size_t)(r.next() % leaves) * 168 * b + 8 * b * 6); val.push_back((uint32_t)r.next() | 1); }
    std::sort(pos.begin(), pos.end()); pos.erase(std::unique(pos.begin(), pos.end()), pos.end()); m = (int)pos.size();
    for (size_t q : pos) memset(&base[q + 4], 0, 4);   /* the varied word's high half is the attacker's: 0, so the condition is high32(k[6]) = 0 */
    size_t k = (size_t)1 << m; std::vector<uint64_t> e(NWL);
    long allc = 0, anyu = 0, anyo = 0;
    for (int cls = 0; cls < 3; cls++) for (long t = 0; t < nkeys; t++) {
        for (auto & w : e) w = r.next();
        if (cls == 0) e[6] &= 0xffffffffull; else if (cls == 1 && !(e[6] >> 32)) e[6] |= 1ull << 32;
        uint64_t o0[3], o[3]; Hb(b, e.data(), base.data(), len, o0); bool all = true, any = false;
        std::vector<char> mm(len);
        for (size_t j = 1; j < k; j++) {
            mm = base; for (int q = 0; q < m; q++) if ((j >> q) & 1) { uint32_t v; memcpy(&v, &mm[pos[q]], 4); v ^= val[q]; memcpy(&mm[pos[q]], &v, 4); }
            Hb(b, e.data(), mm.data(), len, o); bool eq = eq3(o, o0); all = all && eq; any = any || eq;
            if (cls && j > 64) break;
        }
        if (cls == 0) allc += all; else if (cls == 1) anyo += any; else anyu += any;
    }
    printf("long V4<3> len %zu (%zu leaves), %d varied leaves, k=2^%d: class keys whole family equal %ld/%ld; out-of-class keys any of first 64 members equal to base %ld/%ld; uniform %ld/%ld\n", len, leaves, m, m, allc, nkeys, anyo, nkeys, anyu, nkeys);
    return 0;
}

int main(int argc, char ** argv) {
    if (selfcheck()) return 1;
    if (argc < 2) { printf("modes: class <b> <len> <word> <keyword> <n> <rs> | family <b> <block> <m> <n> <rs> | keyfree <b> <n> <rs>\n"); return 0; }
    std::string md = argv[1];
    if (md == "class") return mode_class(atoi(argv[2]), strtoul(argv[3], 0, 0), atoi(argv[4]), atoi(argv[5]), atol(argv[6]), strtoull(argv[7], 0, 0));
    if (md == "family") return mode_family(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atol(argv[5]), strtoull(argv[6], 0, 0));
    if (md == "long") return mode_long(strtoul(argv[2], 0, 0), atoi(argv[3]), atol(argv[4]), strtoull(argv[5], 0, 0));
    if (md == "keyfree") return mode_keyfree(atoi(argv[2]), atol(argv[3]), strtoull(argv[4], 0, 0));
    return 1;
}
