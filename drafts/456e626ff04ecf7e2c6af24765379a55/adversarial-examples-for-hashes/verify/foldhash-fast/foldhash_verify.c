/*
 * foldhash_verify.c -- independent verification package for a claimed
 * fixed-pair hidden-seed collision in foldhash 0.2.0 (fast variant).
 *
 * Pair under test (hashed as a HashMap<Vec<u8>> / HashSet<&[u8]> key):
 *     m1 = 00 00 00 00 00 00 00 00   (8 bytes)
 *     m2 = ff ff ff ff ff ff ff ff   (8 bytes)
 *
 * Part 1 reproduces reference vectors emitted by the real, unmodified
 * foldhash 0.2.0 crate (crates.io, `=0.2.0`) for five (per-hasher seed,
 * SharedSeed::from_u64 argument, input) triples, over five call sequences:
 *   raw      : Hasher::write(bytes); finish()
 *   vec      : <Vec<u8> as Hash>::hash   == write_usize(len); write(bytes); finish()
 *   str      : <str as Hash>::hash       == write(bytes); write_u8(0xff); finish()
 *   q_raw    : same as raw through foldhash::quality
 *   q_vec    : same as vec through foldhash::quality
 *
 * Part 2 measures the collision probability of the pair over uniformly random
 * hidden seeds.  Threat model (the generous one): the per-hasher seed and all
 * six SharedSeed words are independent uniform 64-bit values.  Real foldhash
 * derives all six words from ONE u64 via SharedSeed::from_u64 and then forces
 * bits 0, 31 and 63 on, so the real key space is strictly smaller; the uniform
 * model can only help the hash.
 *
 * Build: cc -O2 -std=c11 -pthread -o foldhash_verify foldhash_verify.c
 * Usage: ./foldhash_verify [log2_seeds] [threads] [base_seed]
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

/* ---------- foldhash 0.2.0 primitives (64-bit path) ---------- */

static const uint64_t ARBITRARY0  = 0x243f6a8885a308d3ULL;
static const uint64_t ARBITRARY5  = 0xbe5466cf34e90c6cULL;

static inline uint64_t folded_multiply(uint64_t x, uint64_t y) {
    unsigned __int128 full = (unsigned __int128)x * (unsigned __int128)y;
    return (uint64_t)full ^ (uint64_t)(full >> 64);
}

static inline uint64_t ror64(uint64_t x, uint32_t r) {
    r &= 63;
    return r ? ((x >> r) | (x << (64 - r))) : x;
}

/* SharedSeed::from_u64 */
static void shared_seed_from_u64(uint64_t seed, uint64_t out[6]) {
    const uint64_t FORCED_ONES = (1ULL << 63) | (1ULL << 31) | 1ULL;
#define MIX(v) folded_multiply((v), ARBITRARY5)
    uint64_t a = MIX(MIX(MIX(seed)));
    uint64_t b = MIX(MIX(MIX(a)));
    uint64_t c = MIX(MIX(MIX(b)));
    uint64_t d = MIX(MIX(MIX(c)));
    uint64_t e = MIX(MIX(MIX(d)));
    uint64_t f = MIX(MIX(MIX(e)));
#undef MIX
    out[0] = a | FORCED_ONES; out[1] = b | FORCED_ONES; out[2] = c | FORCED_ONES;
    out[3] = d | FORCED_ONES; out[4] = e | FORCED_ONES; out[5] = f | FORCED_ONES;
}

static inline uint64_t load_le64(const uint8_t *p) {
    uint64_t v; memcpy(&v, p, 8); return v;   /* x86-64 / aarch64: native == little */
}
static inline uint32_t load_le32(const uint8_t *p) {
    uint32_t v; memcpy(&v, p, 4); return v;
}

static uint64_t hash_bytes_short(const uint8_t *b, size_t len, uint64_t acc, const uint64_t s[6]) {
    uint64_t s0 = acc, s1 = s[1];
    if (len >= 8) {
        s0 ^= load_le64(b);
        s1 ^= load_le64(b + len - 8);
    } else if (len >= 4) {
        s0 ^= (uint64_t)load_le32(b);
        s1 ^= (uint64_t)load_le32(b + len - 4);
    } else if (len > 0) {
        uint64_t lo = b[0], mid = b[len / 2], hi = b[len - 1];
        s0 ^= lo;
        s1 ^= (hi << 8) | mid;
    }
    return folded_multiply(s0, s1);
}

static uint64_t hash_bytes_long(const uint8_t *v, size_t n, uint64_t acc, const uint64_t s[6]) {
    uint64_t s0 = acc, s1 = acc + s[1];
    if (n > 128) {
        uint64_t s2 = s0 + s[2], s3 = s0 + s[3];
        if (n > 256) {
            uint64_t s4 = s0 + s[4], s5 = s0 + s[5];
            for (;;) {
                s0 = folded_multiply(load_le64(v +  0) ^ s0, load_le64(v + 48) ^ s[0]);
                s1 = folded_multiply(load_le64(v +  8) ^ s1, load_le64(v + 56) ^ s[0]);
                s2 = folded_multiply(load_le64(v + 16) ^ s2, load_le64(v + 64) ^ s[0]);
                s3 = folded_multiply(load_le64(v + 24) ^ s3, load_le64(v + 72) ^ s[0]);
                s4 = folded_multiply(load_le64(v + 32) ^ s4, load_le64(v + 80) ^ s[0]);
                s5 = folded_multiply(load_le64(v + 40) ^ s5, load_le64(v + 88) ^ s[0]);
                v += 96; n -= 96;
                if (n <= 256) break;
            }
            s0 ^= s4; s1 ^= s5;
        }
        for (;;) {
            s0 = folded_multiply(load_le64(v +  0) ^ s0, load_le64(v + 32) ^ s[0]);
            s1 = folded_multiply(load_le64(v +  8) ^ s1, load_le64(v + 40) ^ s[0]);
            s2 = folded_multiply(load_le64(v + 16) ^ s2, load_le64(v + 48) ^ s[0]);
            s3 = folded_multiply(load_le64(v + 24) ^ s3, load_le64(v + 56) ^ s[0]);
            v += 64; n -= 64;
            if (n <= 128) break;
        }
        s0 ^= s2; s1 ^= s3;
    }
    size_t len = n;
    s0 = folded_multiply(load_le64(v + 0) ^ s0, load_le64(v + len - 16) ^ s[0]);
    s1 = folded_multiply(load_le64(v + 8) ^ s1, load_le64(v + len -  8) ^ s[0]);
    if (len >= 32) {
        s0 = folded_multiply(load_le64(v + 16) ^ s0, load_le64(v + len - 32) ^ s[0]);
        s1 = folded_multiply(load_le64(v + 24) ^ s1, load_le64(v + len - 24) ^ s[0]);
        if (len >= 64) {
            s0 = folded_multiply(load_le64(v + 32) ^ s0, load_le64(v + len - 48) ^ s[0]);
            s1 = folded_multiply(load_le64(v + 40) ^ s1, load_le64(v + len - 40) ^ s[0]);
            if (len >= 96) {
                s0 = folded_multiply(load_le64(v + 48) ^ s0, load_le64(v + len - 64) ^ s[0]);
                s1 = folded_multiply(load_le64(v + 56) ^ s1, load_le64(v + len - 56) ^ s[0]);
            }
        }
    }
    return s0 ^ s1;
}

/* FoldHasher::write */
static uint64_t fh_write(uint64_t acc, const uint8_t *b, size_t len, const uint64_t s[6]) {
    acc = ror64(acc, (uint32_t)len);
    if (len <= 16) return hash_bytes_short(b, len, acc, s);
    return hash_bytes_long(b, len, acc, s);
}

/* finish() with a sponge holding `sponge` over `sponge_len` bits (<=64 here) */
static inline uint64_t fh_finish(uint64_t acc, unsigned __int128 sponge, int sponge_len, const uint64_t s[6]) {
    if (sponge_len > 0)
        return folded_multiply((uint64_t)sponge ^ acc, (uint64_t)(sponge >> 64) ^ s[0]);
    return acc;
}

/* full call sequences */
static uint64_t fast_raw(uint64_t phs, const uint8_t *b, size_t len, const uint64_t s[6]) {
    return fh_finish(fh_write(phs, b, len, s), 0, 0, s);
}
/* <Vec<u8> as Hash>::hash : write_length_prefix(len) -> write_usize(len), then write(bytes) */
static uint64_t fast_vec(uint64_t phs, const uint8_t *b, size_t len, const uint64_t s[6]) {
    unsigned __int128 sponge = (unsigned __int128)(uint64_t)len;  /* write_num(u64), sponge_len 0 -> 64 */
    int sponge_len = 64;
    uint64_t acc = fh_write(phs, b, len, s);
    return fh_finish(acc, sponge, sponge_len, s);
}
/* <str as Hash>::hash : write(bytes); write_u8(0xff) */
static uint64_t fast_str(uint64_t phs, const uint8_t *b, size_t len, const uint64_t s[6]) {
    uint64_t acc = fh_write(phs, b, len, s);
    return fh_finish(acc, (unsigned __int128)0xffULL, 8, s);
}
static uint64_t qual_raw(uint64_t phs, const uint8_t *b, size_t len, const uint64_t s[6]) {
    return folded_multiply(fast_raw(phs, b, len, s), ARBITRARY0);
}
static uint64_t qual_vec(uint64_t phs, const uint8_t *b, size_t len, const uint64_t s[6]) {
    return folded_multiply(fast_vec(phs, b, len, s), ARBITRARY0);
}

/* ---------- Part 1: cross-check against the real Rust crate ---------- */

struct refvec {
    uint64_t phs, ss;
    const char *hex;
    uint64_t raw, vec, str, qraw, qvec;
};

static const struct refvec REFS[] = {
  {0x0123456789abcdefULL, 0xfedcba9876543210ULL, "0000000000000000",
   0x76515ea1a03101fbULL, 0x66ca112256c774faULL, 0x9ac395840828b9aeULL,
   0x6608bc5df43f5b74ULL, 0x647e5b7098fa10deULL},
  {0x0123456789abcdefULL, 0xfedcba9876543210ULL, "ffffffffffffffff",
   0x84165e99364de5f4ULL, 0x1126871eba2b7a20ULL, 0xa612287b16657ff7ULL,
   0x757e226dd898e948ULL, 0xc9a03fd89c1af7faULL},
  {0xdeadbeefcafebabeULL, 0x0000000000000001ULL, "68656c6c6f20776f726c64",
   0x9cac4433811168a2ULL, 0x9e731322f0728b3fULL, 0x92d7b847ace9dd11ULL,
   0x1748bcd35d9a46d4ULL, 0xc4f0e6fce3f1277dULL},
  {0x0000000000000000ULL, 0x0000000000000000ULL, "",
   0x0000000000000000ULL, 0x0000000000000000ULL, 0x8000007f80000080ULL,
   0x0000000000000000ULL, 0x0000000000000000ULL},
  {0x8badf00d12345678ULL, 0x00ff00ff00ff00ffULL,
   "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2021222324252627",
   0x0de58c94102bd10eULL, 0xf3a8f69ee5d0f760ULL, 0xd3bad978ef5051edULL,
   0x347ad077f49083bfULL, 0x8c1b5510a56851c4ULL},
};

static size_t unhex(const char *h, uint8_t *out) {
    size_t n = strlen(h) / 2;
    for (size_t i = 0; i < n; i++) {
        unsigned v; sscanf(h + 2 * i, "%2x", &v); out[i] = (uint8_t)v;
    }
    return n;
}

static int selftest(void) {
    int bad = 0;
    uint8_t buf[256];
    printf("== Part 1: cross-check against real foldhash 0.2.0 (Rust) ==\n");
    for (size_t i = 0; i < sizeof REFS / sizeof REFS[0]; i++) {
        const struct refvec *r = &REFS[i];
        uint64_t s[6];
        shared_seed_from_u64(r->ss, s);
        size_t len = unhex(r->hex, buf);
        uint64_t g[5] = {fast_raw(r->phs, buf, len, s), fast_vec(r->phs, buf, len, s),
                         fast_str(r->phs, buf, len, s), qual_raw(r->phs, buf, len, s),
                         qual_vec(r->phs, buf, len, s)};
        uint64_t e[5] = {r->raw, r->vec, r->str, r->qraw, r->qvec};
        const char *nm[5] = {"fast_raw", "fast_vec", "fast_str", "qual_raw", "qual_vec"};
        for (int k = 0; k < 5; k++) {
            int ok = (g[k] == e[k]);
            if (!ok) bad++;
            printf("  case %zu len=%3zu %-8s C=%016llx rust=%016llx %s\n", i, len, nm[k],
                   (unsigned long long)g[k], (unsigned long long)e[k], ok ? "OK" : "MISMATCH");
        }
    }
    printf("  -> %s (%d mismatches)\n\n", bad ? "FAILED" : "all reference vectors reproduced", bad);
    return bad;
}

/* ---------- Part 2: measurement ---------- */

static inline uint64_t splitmix64(uint64_t *x) {
    uint64_t z = (*x += 0x9e3779b97f4a7c15ULL);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    return z ^ (z >> 31);
}
typedef struct { uint64_t s[4]; } xo_t;
static inline uint64_t rotl(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
static inline uint64_t xo_next(xo_t *r) {
    uint64_t *s = r->s;
    uint64_t res = rotl(s[0] + s[3], 23) + s[0];
    uint64_t t = s[1] << 17;
    s[2] ^= s[0]; s[3] ^= s[1]; s[1] ^= s[2]; s[0] ^= s[3]; s[2] ^= t;
    s[3] = rotl(s[3], 45);
    return res;
}

struct job {
    uint64_t n, base, tid;
    uint64_t hit_raw, hit_vec, hit_str, hit_qvec, hit_acc;
};

static void *worker(void *p) {
    struct job *j = p;
    uint64_t sm = j->base ^ (j->tid * 0xa0761d6478bd642fULL);
    xo_t r;
    for (int i = 0; i < 4; i++) r.s[i] = splitmix64(&sm);
    for (int i = 0; i < 32; i++) (void)xo_next(&r);

    uint64_t hr = 0, hv = 0, hs = 0, hq = 0, ha = 0;
    const uint64_t M = ~0ULL;
    for (uint64_t i = 0; i < j->n; i++) {
        uint64_t phs = xo_next(&r);   /* per-hasher seed (accumulator) */
        uint64_t s0  = xo_next(&r);   /* SharedSeed word 0 */
        uint64_t s1  = xo_next(&r);   /* SharedSeed word 1 */
        /* words 2..5 are unread on the len<=16 path; drawn for model fidelity only */

        uint64_t accr = ror64(phs, 8);               /* write(): rotate_right(acc, len=8) */
        /* hash_bytes_short, len==8: s0' = acc ^ w, s1' = seeds[1] ^ w (same word twice) */
        uint64_t a1 = folded_multiply(accr,     s1);          /* w = 0x00..00 */
        uint64_t a2 = folded_multiply(accr ^ M, s1 ^ M);      /* w = 0xff..ff */
        ha += (a1 == a2);
        hr += (a1 == a2);                                     /* raw finish(): sponge empty */
        uint64_t v1 = folded_multiply(8ULL ^ a1, s0);         /* Vec<u8>: sponge lo=len=8, hi=0 */
        uint64_t v2 = folded_multiply(8ULL ^ a2, s0);
        hv += (v1 == v2);
        uint64_t t1 = folded_multiply(0xffULL ^ a1, s0);      /* str: sponge lo=0xff */
        uint64_t t2 = folded_multiply(0xffULL ^ a2, s0);
        hs += (t1 == t2);
        hq += (folded_multiply(v1, ARBITRARY0) == folded_multiply(v2, ARBITRARY0));
    }
    j->hit_raw = hr; j->hit_vec = hv; j->hit_str = hs; j->hit_qvec = hq; j->hit_acc = ha;
    return NULL;
}

int main(int argc, char **argv) {
    int log2n  = argc > 1 ? atoi(argv[1]) : 30;
    int nthr   = argc > 2 ? atoi(argv[2]) : 24;
    uint64_t base = argc > 3 ? strtoull(argv[3], NULL, 0) : 0xC0FFEE1234567890ULL;
    if (nthr < 1) nthr = 1;
    if (nthr > 24) nthr = 24;

    if (selftest()) { fprintf(stderr, "self-test failed; refusing to measure\n"); return 1; }

    /* show what a real from_u64 seed looks like vs. the uniform model */
    uint64_t demo[6]; shared_seed_from_u64(0x0123456789abcdefULL, demo);
    printf("note: SharedSeed::from_u64(0x0123456789abcdef) = [%016llx %016llx %016llx %016llx %016llx %016llx]\n",
           (unsigned long long)demo[0], (unsigned long long)demo[1], (unsigned long long)demo[2],
           (unsigned long long)demo[3], (unsigned long long)demo[4], (unsigned long long)demo[5]);
    printf("      (all six derived from ONE u64; bits 0, 31, 63 forced on.  The measurement below\n"
           "       instead draws the words independently uniform -- the model generous to foldhash.)\n\n");

    uint64_t total = 1ULL << log2n;
    uint64_t per = total / (uint64_t)nthr, rem = total % (uint64_t)nthr;

    printf("== Part 2: 2^%d uniformly random hidden seeds, %d threads ==\n", log2n, nthr);
    fflush(stdout);

    pthread_t *th = malloc(sizeof(pthread_t) * nthr);
    struct job *jobs = calloc(nthr, sizeof *jobs);
    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    for (int t = 0; t < nthr; t++) {
        jobs[t].n = per + (t < (int)rem ? 1 : 0);
        jobs[t].base = base; jobs[t].tid = (uint64_t)t + 1;
        pthread_create(&th[t], NULL, worker, &jobs[t]);
    }
    uint64_t HR = 0, HV = 0, HS = 0, HQ = 0, HA = 0, N = 0;
    for (int t = 0; t < nthr; t++) {
        pthread_join(th[t], NULL);
        HR += jobs[t].hit_raw; HV += jobs[t].hit_vec; HS += jobs[t].hit_str;
        HQ += jobs[t].hit_qvec; HA += jobs[t].hit_acc; N += jobs[t].n;
    }
    clock_gettime(CLOCK_MONOTONIC, &t1);
    double secs = (t1.tv_sec - t0.tv_sec) + 1e-9 * (t1.tv_nsec - t0.tv_nsec);

    printf("seeds            = %llu (2^%d)\n", (unsigned long long)N, log2n);
    printf("hits accumulator = %llu\n", (unsigned long long)HA);
    printf("hits fast raw    = %llu\n", (unsigned long long)HR);
    printf("hits fast Vec<u8>= %llu\n", (unsigned long long)HV);
    printf("hits fast str    = %llu\n", (unsigned long long)HS);
    printf("hits quality Vec = %llu\n", (unsigned long long)HQ);
    printf("elapsed          = %.1f s\n", secs);
    fflush(stdout);
    return 0;
}
