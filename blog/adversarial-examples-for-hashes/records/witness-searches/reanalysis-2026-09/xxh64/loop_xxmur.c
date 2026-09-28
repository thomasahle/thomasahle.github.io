/*
 * loop_xxmur.c -- position-independent bulk-loop differentials in XXH64, XXH32,
 * MurmurHash64A and MurmurHash2 (32-bit), checked inside long messages
 * (256 B .. 1 MB) with random surrounding content and random seeds.
 *
 * Hash code:
 *   XXH64, XXH32: upstream xxhash.h v0.8.3 (Yann Collet, BSD-2-Clause), included
 *     unmodified from upstream/xxhash.h (XXH_INLINE_ALL).
 *   MurmurHash2_32 and MurmurHash2_64 (= MurmurHash64A): Austin Appleby (public
 *     domain), bodies copied from SMHasher3 hashes/murmurhash2.cpp with the
 *     template/endianness wrapper replaced by little-endian loads.
 *   Startup check: SMHasher3 verification values XXH64 0x8F8224C4, XXH-32
 *   0x6FD78385 (both canonical big-endian output), MurmurHash2-64 0x1F0D3804,
 *   MurmurHash2-32 0x27864C1E (little-endian output).  Exit 1 on mismatch.
 *
 * Differentials (one "slot" = the two words that carry a difference):
 *   XXH64  lane l, stripes s and s+1 (32-byte stripes, any s with s+1 < len/32):
 *          w[s][l] += inv(P2)             -> V = acc + w*P2 grows by exactly 1;
 *          x = rotl(V,31) then grows by 2^31 unless the low 33 bits of V are all
 *          ones; acc = x*P1 differs by 2^31*P1;
 *          w[s+1][l] -= 2^31*P1*inv(P2)   -> cancels it.  Failure iff
 *          low33(V) = 2^33-1.  acc at stripe s is a bijective function of the
 *          seed (composition of bijections), so eps = 1 - 2^-33 EXACTLY for any
 *          position and any surrounding content.
 *   XXH32  the same with rotl 13: w += inv(P2), next -= 2^13*P1*inv(P2); failure
 *          iff low19(V) = 2^19-1; eps = 1 - 2^-19 exactly (2^13 of 2^32 seeds).
 *   M64A   words t, t+1 (8-byte, in the body): mix(w) ^= 2^63 in both (mix is
 *          k*=m; k^=k>>47; k*=m, a public bijection).  h ^= k; h *= m keeps a
 *          lone 2^63 difference, the next word cancels it.  Key-free, eps = 1.
 *   M2-32  words t, t+1 (4-byte): h = h*m ^ mix(w), mix(w) ^= 2^31 in both.
 *          Key-free, eps = 1.  (Top-bit trick: Aumasson-Bernstein 2012.)
 *
 * Modes (hash = xxh64 | xxh32 | m64a | m2):
 *   rand  HASH LEN NTRIALS        fresh random message, slot, seed per trial
 *   fixed HASH LEN NKEYS          one message (RNG), one slot, NKEYS seeds
 *   plant HASH LEN NTRIALS        (xxh only) seeds back-solved so that the slot
 *                                 fails / sits one step from failing
 *   exh32 LEN                     xxh32: all 2^32 seeds for one pair
 *   cube  HASH LEN R NKEYS        R disjoint slots -> 2^R messages, NKEYS seeds
 * Options via env: THREADS (default 1), RNGSEED (default 0x4C6F6E6742).
 *
 * Build: cc -std=c11 -O2 -pthread -Iupstream -o loop_xxmur loop_xxmur.c -lm
 */
#define XXH_INLINE_ALL
#include "xxhash.h"
#include <inttypes.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* ---------------- MurmurHash2 (SMHasher3 bodies, LE) ---------------- */
static inline uint32_t GET_U32(const uint8_t *p, size_t o) { uint32_t v; memcpy(&v, p + o, 4); return v; }
static inline uint64_t GET_U64(const uint8_t *p, size_t o) { uint64_t v; memcpy(&v, p + o, 8); return v; }
static inline void PUT_U32(uint32_t v, uint8_t *p, size_t o) { memcpy(p + o, &v, 4); }
static inline void PUT_U64(uint64_t v, uint8_t *p, size_t o) { memcpy(p + o, &v, 8); }

static void MurmurHash2_32( const void * in, const size_t olen, const uint64_t seed, void * out ) {
    // 'm' and 'r' are mixing constants generated offline.
    // They're not really 'magic', they just happen to work well.
    const uint32_t m   = 0x5bd1e995;
    const uint32_t r   = 24;
    size_t         len = olen;

    // Initialize the hash to a 'random' value
    uint32_t h = seed ^ olen;

    // Mix 4 bytes at a time into the hash
    const uint8_t * data = (const uint8_t *)in;

    while (len >= 4) {
        uint32_t k = GET_U32(data, 0);

        k    *= m;
        k    ^= k >> r;
        k    *= m;

        h    *= m;
        h    ^= k;

        data += 4;
        len  -= 4;
    }

    // Handle the last few bytes of the input array
    switch (len) {
    case 3: h ^= data[2] << 16; /* FALLTHROUGH */
    case 2: h ^= data[1] <<  8; /* FALLTHROUGH */
    case 1: h ^= data[0];
            h *= m;
    }

    // Do a few final mixes of the hash to ensure the last few
    // bytes are well-incorporated.
    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    PUT_U32(h, (uint8_t *)out, 0);
}

static void MurmurHash2_64( const void * in, const size_t len, const uint64_t seed, void * out ) {
    const uint64_t m     = UINT64_C(0xc6a4a7935bd1e995);
    const uint32_t r     = 47;

    uint64_t h           = seed ^ (len * m);

    const uint8_t * data = (const uint8_t *)in;
    const uint8_t * end  = data + len - (len & 7);

    while (data != end) {
        uint64_t k = GET_U64(data, 0);

        k    *= m;
        k    ^= k >> r;
        k    *= m;

        h    ^= k;
        h    *= m;

        data += 8;
    }

    switch (len & 7) {
    case 7: h ^= (uint64_t)(data[6]) << 48; /* FALLTHROUGH */
    case 6: h ^= (uint64_t)(data[5]) << 40; /* FALLTHROUGH */
    case 5: h ^= (uint64_t)(data[4]) << 32; /* FALLTHROUGH */
    case 4: h ^= (uint64_t)(data[3]) << 24; /* FALLTHROUGH */
    case 3: h ^= (uint64_t)(data[2]) << 16; /* FALLTHROUGH */
    case 2: h ^= (uint64_t)(data[1]) <<  8; /* FALLTHROUGH */
    case 1: h ^= (uint64_t)(data[0]);
            h *= m;
    }

    h ^= h >> r;
    h *= m;
    h ^= h >> r;

    PUT_U64(h, (uint8_t *)out, 0);
}

/* ---------------- uniform interface ---------------- */
enum { H_XXH64, H_XXH32, H_M64A, H_M2, NH };
static const char *hname[NH] = { "xxh64", "xxh32", "m64a", "m2" };
static const int hbytes[NH] = { 8, 4, 8, 4 };

static uint64_t hash(int h, const uint8_t *m, size_t n, uint64_t seed) {
    uint8_t o[8];
    switch (h) {
    case H_XXH64: return XXH64(m, n, seed);
    case H_XXH32: return XXH32(m, n, (uint32_t)seed);
    case H_M64A: MurmurHash2_64(m, n, seed, o); return GET_U64(o, 0);
    default: MurmurHash2_32(m, n, seed, o); return GET_U32(o, 0);
    }
}
/* SMHasher3 output bytes: xxh canonical big-endian, murmur little-endian */
static void hash_bytes(int h, const uint8_t *m, size_t n, uint64_t seed, uint8_t *out) {
    uint64_t v = hash(h, m, n, seed);
    int b = hbytes[h];
    for (int i = 0; i < b; i++)
        out[i] = (h == H_XXH64 || h == H_XXH32) ? (uint8_t)(v >> (8 * (b - 1 - i))) : (uint8_t)(v >> (8 * i));
}
static uint32_t verification(int h) {
    static uint8_t key[256], hashes[8 * 256];
    uint8_t total[8];
    int b = hbytes[h];
    memset(key, 0, sizeof key);
    for (int i = 0; i < 256; i++) { hash_bytes(h, key, (size_t)i, (uint64_t)(256 - i), hashes + b * i); key[i] = (uint8_t)i; }
    hash_bytes(h, hashes, (size_t)(256 * b), 0, total);
    return (uint32_t)total[0] | (uint32_t)total[1] << 8 | (uint32_t)total[2] << 16 | (uint32_t)total[3] << 24;
}
static const uint32_t expect_verif[NH] = { 0x8F8224C4u, 0x6FD78385u, 0x1F0D3804u, 0x27864C1Eu };

/* ---------------- RNG: splitmix64 -> xoshiro256** ---------------- */
typedef struct { uint64_t s[4]; } rng_t;
static inline uint64_t rotl64(uint64_t x, int r) { return (x << r) | (x >> (64 - r)); }
static inline uint32_t rotl32(uint32_t x, int r) { return (x << r) | (x >> (32 - r)); }
static uint64_t splitmix64(uint64_t *s) { uint64_t z = (*s += UINT64_C(0x9e3779b97f4a7c15)); z = (z ^ (z >> 30)) * UINT64_C(0xbf58476d1ce4e5b9); z = (z ^ (z >> 27)) * UINT64_C(0x94d049bb133111eb); return z ^ (z >> 31); }
static void rng_init(rng_t *r, uint64_t seed) { for (int i = 0; i < 4; i++) r->s[i] = splitmix64(&seed); }
static uint64_t rng(rng_t *r) { uint64_t *s = r->s; const uint64_t res = rotl64(s[1] * 5, 7) * 9, t = s[1] << 17; s[2] ^= s[0]; s[3] ^= s[1]; s[1] ^= s[2]; s[0] ^= s[3]; s[2] ^= t; s[3] = rotl64(s[3], 45); return res; }
static uint64_t rng_below(rng_t *r, uint64_t n) { return rng(r) % n; }
static void rng_fill(rng_t *r, uint8_t *p, size_t n) { size_t i = 0; for (; i + 8 <= n; i += 8) { uint64_t v = rng(r); memcpy(p + i, &v, 8); } if (i < n) { uint64_t v = rng(r); memcpy(p + i, &v, n - i); } }

/* ---------------- arithmetic helpers ---------------- */
static uint64_t inv64(uint64_t a) { uint64_t x = a; for (int i = 0; i < 6; i++) x *= 2 - a * x; return x; }
static uint32_t inv32(uint32_t a) { uint32_t x = a; for (int i = 0; i < 5; i++) x *= 2 - a * x; return x; }
static const uint64_t P64_1 = UINT64_C(0x9E3779B185EBCA87), P64_2 = UINT64_C(0xC2B2AE3D27D4EB4F);
static const uint32_t P32_1 = 0x9E3779B1u, P32_2 = 0x85EBCA77u;
static const uint64_t MM64 = UINT64_C(0xc6a4a7935bd1e995);
static const uint32_t MM32 = 0x5bd1e995u;
static uint64_t mix64(uint64_t k) { k *= MM64; k ^= k >> 47; k *= MM64; return k; }
static uint64_t mix64inv(uint64_t k) { uint64_t im = inv64(MM64); k *= im; k ^= k >> 47; k *= im; return k; } /* x^(x>>47) is an involution */
static uint32_t mix32(uint32_t k) { k *= MM32; k ^= k >> 24; k *= MM32; return k; }
static uint32_t mix32inv(uint32_t k) { uint32_t im = inv32(MM32); k *= im; k ^= k >> 24; k *= im; return k; } /* x^(x>>24) is an involution on 32 bits */

/* ---------------- slots ---------------- */
/* xxh: slot = (stripe s, lane l) touching words (s,l),(s+1,l); murmur: slot = word t, t+1 */
static size_t unitsz(int h) { return (h == H_XXH64 || h == H_M64A) ? 8 : 4; }
static size_t stripesz(int h) { return h == H_XXH64 ? 32 : h == H_XXH32 ? 16 : unitsz(h); }
static int nlanes(int h) { return (h == H_XXH64 || h == H_XXH32) ? 4 : 1; }
/* number of loop stripes (xxh) or body words (murmur) */
static size_t nstripes(int h, size_t len) { return len / stripesz(h); }
static size_t woff(int h, size_t s, int l) { return s * stripesz(h) + (size_t)l * unitsz(h); }

static void apply_slot(int h, uint8_t *m, size_t s, int l) {
    size_t o1 = woff(h, s, l), o2 = woff(h, s + 1, l);
    switch (h) {
    case H_XXH64: {
        uint64_t i2 = inv64(P64_2);
        PUT_U64(GET_U64(m, o1) + i2, m, o1);
        PUT_U64(GET_U64(m, o2) - (UINT64_C(1) << 31) * P64_1 * i2, m, o2);
        break; }
    case H_XXH32: {
        uint32_t i2 = inv32(P32_2);
        PUT_U32(GET_U32(m, o1) + i2, m, o1);
        PUT_U32(GET_U32(m, o2) - (UINT32_C(1) << 13) * P32_1 * i2, m, o2);
        break; }
    case H_M64A:
        PUT_U64(mix64inv(mix64(GET_U64(m, o1)) ^ (UINT64_C(1) << 63)), m, o1);
        PUT_U64(mix64inv(mix64(GET_U64(m, o2)) ^ (UINT64_C(1) << 63)), m, o2);
        break;
    default:
        PUT_U32(mix32inv(mix32(GET_U32(m, o1)) ^ (UINT32_C(1) << 31)), m, o1);
        PUT_U32(mix32inv(mix32(GET_U32(m, o2)) ^ (UINT32_C(1) << 31)), m, o2);
    }
}

/* model: lane accumulator entering stripe s, V = acc + w*P2, failure iff low bits all ones */
static const uint64_t *dummy;
static uint64_t xxh64_lane_init(uint64_t seed, int l) {
    return l == 0 ? seed + P64_1 + P64_2 : l == 1 ? seed + P64_2 : l == 2 ? seed : seed - P64_1;
}
static uint32_t xxh32_lane_init(uint32_t seed, int l) {
    return l == 0 ? seed + P32_1 + P32_2 : l == 1 ? seed + P32_2 : l == 2 ? seed : seed - P32_1;
}
static int predict_fail(int h, const uint8_t *m, size_t s, int l, uint64_t seed) {
    if (h == H_XXH64) {
        uint64_t acc = xxh64_lane_init(seed, l);
        for (size_t t = 0; t < s; t++) acc = rotl64(acc + GET_U64(m, woff(h, t, l)) * P64_2, 31) * P64_1;
        uint64_t V = acc + GET_U64(m, woff(h, s, l)) * P64_2;
        return (V & ((UINT64_C(1) << 33) - 1)) == ((UINT64_C(1) << 33) - 1);
    }
    if (h == H_XXH32) {
        uint32_t acc = xxh32_lane_init((uint32_t)seed, l);
        for (size_t t = 0; t < s; t++) acc = rotl32(acc + GET_U32(m, woff(h, t, l)) * P32_2, 13) * P32_1;
        uint32_t V = acc + GET_U32(m, woff(h, s, l)) * P32_2;
        return (V & ((1u << 19) - 1)) == ((1u << 19) - 1);
    }
    return 0;
}
/* back-solve a seed with V(s,l) = target (xxh only) */
static uint64_t seed_for_V(int h, const uint8_t *m, size_t s, int l, uint64_t Vt) {
    if (h == H_XXH64) {
        uint64_t iP1 = inv64(P64_1);
        uint64_t acc = Vt - GET_U64(m, woff(h, s, l)) * P64_2;          /* acc entering stripe s */
        for (size_t t = s; t-- > 0;) {                                   /* invert stripe t */
            uint64_t V = acc * iP1; V = (V >> 31) | (V << 33);
            acc = V - GET_U64(m, woff(h, t, l)) * P64_2;
        }
        return acc - xxh64_lane_init(0, l);
    } else {
        uint32_t iP1 = inv32(P32_1);
        uint32_t acc = (uint32_t)Vt - GET_U32(m, woff(h, s, l)) * P32_2;
        for (size_t t = s; t-- > 0;) {
            uint32_t V = acc * iP1; V = (V >> 13) | (V << 19);
            acc = V - GET_U32(m, woff(h, t, l)) * P32_2;
        }
        return (uint32_t)(acc - xxh32_lane_init(0, l));
    }
}

/* ---------------- stats ---------------- */
static void wilson(uint64_t k, uint64_t n, double *lo, double *hi) {
    double z = 1.959963984540054, p = (double)k / n, d = 1 + z * z / n, c = p + z * z / (2.0 * n);
    double s = z * sqrt(p * (1 - p) / n + z * z / (4.0 * n * n));
    *lo = (c - s) / d; *hi = (c + s) / d;
}
/* exact-ish upper 95% bound on failures when 0 observed: 3/n */

/* ---------------- threaded jobs ---------------- */
static int NT = 1;
static uint64_t RNGSEED = UINT64_C(0x4C6F6E6742);

typedef struct {
    int tid, h; size_t len; uint64_t n; int mode; int R;
    const uint8_t *fixm; const uint8_t *fixm2; size_t fs; int fl; /* fixed pair */
    uint64_t lo_seed, hi_seed;                                      /* exh32 range */
    uint64_t coll, pred_fail, mismatch, done;
    uint64_t ex_seed, ex_h; int have_ex;
} job_t;

static void *worker(void *arg) {
    job_t *J = (job_t *)arg;
    rng_t R; rng_init(&R, RNGSEED ^ (UINT64_C(0x1000193) * (uint64_t)(J->tid + 1)) ^ ((uint64_t)J->h << 40) ^ ((uint64_t)J->len << 8) ^ (uint64_t)J->mode);
    size_t len = J->len; int h = J->h;
    uint8_t *m = malloc(len), *m2 = malloc(len);
    size_t ns = nstripes(h, len);
    for (uint64_t i = 0; i < J->n; i++) {
        size_t s; int l; uint64_t seed; const uint8_t *a = m, *b = m2;
        if (J->mode == 0) {          /* rand */
            rng_fill(&R, m, len);
            s = rng_below(&R, ns - 1); l = (int)rng_below(&R, (uint64_t)nlanes(h));
            memcpy(m2, m, len); apply_slot(h, m2, s, l);
            seed = rng(&R);
        } else if (J->mode == 1) {   /* fixed */
            a = J->fixm; b = J->fixm2; s = J->fs; l = J->fl; seed = rng(&R);
        } else if (J->mode == 2) {   /* plant */
            rng_fill(&R, m, len);
            s = rng_below(&R, ns - 1); l = (int)rng_below(&R, (uint64_t)nlanes(h));
            memcpy(m2, m, len); apply_slot(h, m2, s, l);
            int bits = h == H_XXH64 ? 33 : 19;
            uint64_t mask = (UINT64_C(1) << bits) - 1;
            uint64_t Vt = (rng(&R) & ~mask) | mask;           /* failing V */
            if (i & 1) Vt -= 1;                                /* one below: must collide */
            if (h == H_XXH32) Vt &= 0xffffffffu;
            seed = seed_for_V(h, m, s, l, Vt);
        } else {                     /* exh32 */
            a = J->fixm; b = J->fixm2; s = J->fs; l = J->fl; seed = J->lo_seed + i;
        }
        uint64_t h1 = hash(h, a, len, seed), h2 = hash(h, b, len, seed);
        int c = h1 == h2;
        int pf = predict_fail(h, a, s, l, seed);
        J->coll += c; J->pred_fail += pf; J->mismatch += (c == pf); /* collide <=> !fail */
        if (c && !J->have_ex) { J->have_ex = 1; J->ex_seed = seed; J->ex_h = h1; }
        J->done++;
    }
    free(m); free(m2);
    return NULL;
}

static void run_jobs(job_t *proto, uint64_t n) {
    pthread_t th[256]; job_t J[256];
    for (int t = 0; t < NT; t++) {
        J[t] = *proto; J[t].tid = t;
        J[t].n = n / NT + ((uint64_t)t < n % NT);
        if (proto->mode == 3) { uint64_t per = n / NT; J[t].lo_seed = proto->lo_seed + per * t; J[t].n = (t == NT - 1) ? n - per * t : per; }
        pthread_create(&th[t], NULL, worker, &J[t]);
    }
    proto->coll = proto->pred_fail = proto->mismatch = proto->done = 0; proto->have_ex = 0;
    for (int t = 0; t < NT; t++) {
        pthread_join(th[t], NULL);
        proto->coll += J[t].coll; proto->pred_fail += J[t].pred_fail; proto->mismatch += J[t].mismatch; proto->done += J[t].done;
        if (J[t].have_ex && !proto->have_ex) { proto->have_ex = 1; proto->ex_seed = J[t].ex_seed; proto->ex_h = J[t].ex_h; }
    }
}

static int parse_hash(const char *s) { for (int i = 0; i < NH; i++) if (!strcmp(s, hname[i])) return i; fprintf(stderr, "unknown hash %s\n", s); exit(2); }

static void report(const char *what, int h, size_t len, job_t *J) {
    double lo, hi; wilson(J->coll, J->done, &lo, &hi);
    printf("%s %s len=%zu: collisions %" PRIu64 "/%" PRIu64 " = %.9f [95%% %.9f, %.9f]; model-predicted failures %" PRIu64 "; model/outcome disagreements %" PRIu64 "\n",
           what, hname[h], len, J->coll, J->done, (double)J->coll / J->done, lo, hi, J->pred_fail, J->mismatch);
    if (J->have_ex) printf("  example colliding seed %016" PRIx64 " -> both hash to %0*" PRIx64 "\n", J->ex_seed, 2 * hbytes[h], J->ex_h);
    fflush(stdout);
}

static void print_words(int h, const uint8_t *m, const uint8_t *m2, size_t len) {
    size_t u = unitsz(h);
    for (size_t o = 0; o + u <= len; o += u)
        if (memcmp(m + o, m2 + o, u)) {
            if (u == 8) printf("  offset %7zu: %016" PRIx64 " -> %016" PRIx64 "\n", o, GET_U64(m, o), GET_U64(m2, o));
            else printf("  offset %7zu: %08" PRIx32 " -> %08" PRIx32 "\n", o, GET_U32(m, o), GET_U32(m2, o));
        }
}

int main(int argc, char **argv) {
    if (getenv("THREADS")) NT = atoi(getenv("THREADS"));
    if (getenv("RNGSEED")) RNGSEED = strtoull(getenv("RNGSEED"), 0, 0);
    int bad = 0;
    for (int h = 0; h < NH; h++) {
        uint32_t v = verification(h);
        printf("SMHasher3 verification %-6s %08" PRIX32 " (expected %08" PRIX32 ") %s\n", hname[h], v, expect_verif[h], v == expect_verif[h] ? "OK" : "MISMATCH");
        bad |= v != expect_verif[h];
    }
    if (bad) return 1;
    if (argc < 2) { printf("modes: rand|fixed|plant|exh32|cube\n"); return 0; }
    const char *mode = argv[1];
    job_t P; memset(&P, 0, sizeof P);
    if (!strcmp(mode, "rand") || !strcmp(mode, "plant")) {
        P.h = parse_hash(argv[2]); P.len = strtoull(argv[3], 0, 0); uint64_t n = strtoull(argv[4], 0, 0);
        P.mode = !strcmp(mode, "rand") ? 0 : 2;
        run_jobs(&P, n);
        report(mode, P.h, P.len, &P);
        if (P.mode == 2) printf("  (plant: odd trials use V one below the failing value and must collide; even trials must fail => expected collisions = n/2, disagreements 0)\n");
    } else if (!strcmp(mode, "fixed") || !strcmp(mode, "exh32")) {
        int h; size_t len; uint64_t n;
        if (!strcmp(mode, "fixed")) { h = parse_hash(argv[2]); len = strtoull(argv[3], 0, 0); n = strtoull(argv[4], 0, 0); P.mode = 1; }
        else { h = H_XXH32; len = strtoull(argv[2], 0, 0); n = UINT64_C(1) << 32; P.mode = 3; P.lo_seed = 0; }
        rng_t R; rng_init(&R, RNGSEED ^ 0xF1ED);
        uint8_t *m = malloc(len), *m2 = malloc(len);
        rng_fill(&R, m, len);
        size_t ns = nstripes(h, len);
        size_t s = ns / 2 + rng_below(&R, ns / 2 - 1); int l = (int)rng_below(&R, (uint64_t)nlanes(h));
        memcpy(m2, m, len); apply_slot(h, m2, s, l);
        printf("%s %s len=%zu: message = %zu bytes from xoshiro256** seeded by splitmix64(0x%" PRIx64 "), slot stripe/word %zu lane %d; differing words:\n",
               mode, hname[h], len, len, RNGSEED ^ 0xF1ED, s, l);
        print_words(h, m, m2, len);
        if (len <= 256) { printf("  M  = "); for (size_t i = 0; i < len; i++) printf("%02x", m[i]); printf("\n  M' = "); for (size_t i = 0; i < len; i++) printf("%02x", m2[i]); printf("\n"); }
        P.h = h; P.len = len; P.fixm = m; P.fixm2 = m2; P.fs = s; P.fl = l;
        run_jobs(&P, n);
        report(mode, h, len, &P);
        if (h == H_XXH32 || h == H_XXH64) {
            int bits = h == H_XXH64 ? 33 : 19; uint64_t mask = (UINT64_C(1) << bits) - 1;
            uint64_t fs = seed_for_V(h, m, s, l, ((h == H_XXH32 ? 0x5A5A5A5Au : UINT64_C(0x5A5A5A5A5A5A5A5A)) & ~mask) | mask);
            printf("  a back-solved FAILING seed %016" PRIx64 ": %0*" PRIx64 " vs %0*" PRIx64 " (predicted fail %d)\n", fs, 2 * hbytes[h], hash(h, m, len, fs), 2 * hbytes[h], hash(h, m2, len, fs), predict_fail(h, m, s, l, fs));
        }
    } else if (!strcmp(mode, "cube")) {
        int h = parse_hash(argv[2]); size_t len = strtoull(argv[3], 0, 0); int r = atoi(argv[4]); uint64_t nk = strtoull(argv[5], 0, 0);
        rng_t R; rng_init(&R, RNGSEED ^ 0xC0BE);
        uint8_t *base = malloc(len); rng_fill(&R, base, len);
        size_t ns = nstripes(h, len); int L = nlanes(h);
        /* choose r distinct slots (stripe pair 2q,2q+1 ; lane) at random */
        size_t nslots = (ns / 2) * (size_t)L;
        size_t *ss = malloc(sizeof(size_t) * r); int *sl = malloc(sizeof(int) * r);
        uint64_t *chosen = malloc(sizeof(uint64_t) * r);
        for (int k = 0; k < r; k++) {
            uint64_t id; int dup;
            do { id = rng_below(&R, nslots); dup = 0; for (int j = 0; j < k; j++) dup |= chosen[j] == id; } while (dup);
            chosen[k] = id; ss[k] = 2 * (id / L); sl[k] = (int)(id % L);
        }
        printf("cube %s len=%zu: base = %zu bytes from xoshiro256** seeded by splitmix64(0x%" PRIx64 "); %d slots:", hname[h], len, len, RNGSEED ^ 0xC0BE, r);
        for (int k = 0; k < r; k++) printf(" (%zu,%d)", ss[k], sl[k]); printf("\n");
        uint64_t M = UINT64_C(1) << r, all_ok = 0, pred_ok = 0, mism = 0, minclass = UINT64_MAX;
        uint8_t *msg = malloc(len);
        uint64_t *hv = malloc(sizeof(uint64_t) * M);
        for (uint64_t key = 0; key < nk; key++) {
            uint64_t seed = rng(&R);
            /* hash every member (full messages) */
            #define CUBE_T 64
            for (uint64_t mb = 0; mb < M; mb++) {
                memcpy(msg, base, len);
                for (int k = 0; k < r; k++) if (mb >> k & 1) apply_slot(h, msg, ss[k], sl[k]);
                hv[mb] = hash(h, msg, len, seed);
            }
            uint64_t eq = 0; for (uint64_t mb = 0; mb < M; mb++) eq += hv[mb] == hv[0];
            int ok = eq == M; all_ok += ok;
            int pok = 1; for (int k = 0; k < r; k++) pok &= !predict_fail(h, base, ss[k], sl[k], seed);
            pred_ok += pok; mism += ok != pok;
            if (eq < minclass) minclass = eq;
            if (key == 0) printf("  key %016" PRIx64 ": %" PRIu64 "/%" PRIu64 " members hash to %0*" PRIx64 "\n", seed, eq, M, 2 * hbytes[h], hv[0]);
        }
        double lo, hi; wilson(all_ok, nk, &lo, &hi);
        printf("cube %s len=%zu r=%d: whole %" PRIu64 "-set collides for %" PRIu64 "/%" PRIu64 " keys [95%% %.4f, %.4f]; model predicts %" PRIu64 "; disagreements %" PRIu64 "; smallest class-of-member-0 %" PRIu64 "\n",
               hname[h], len, r, M, all_ok, nk, lo, hi, pred_ok, mism, minclass);
    } else { fprintf(stderr, "bad mode\n"); return 2; }
    (void)dummy;
    return 0;
}
