/* foldhash_verify.c -- independent verification package for a claimed
 * foldhash 0.2.0 fixed-pair hidden-seed collision.
 *
 * Part 1 ("verify"): a from-scratch C11 re-implementation of foldhash 0.2.0
 *   (folded_multiply, hash_bytes_short, hash_bytes_long, the fast FoldHasher
 *   state machine, the quality final fold, and SharedSeed::from_u64) checked
 *   against values printed by the real Rust crate (foldhash = "=0.2.0",
 *   rustc 1.92.0) for several (per_hasher_seed, shared_seed, input) triples.
 *
 * Part 2 ("measure"): draws N uniformly random hidden seeds and counts full
 *   64-bit collisions of the two claimed messages, for the fast and quality
 *   variants, both for a raw Hasher::write(bytes)+finish() and for the exact
 *   <Vec<u8> as Hash>::hash path a HashMap<Vec<u8>,_> uses.
 *
 * Build: cc -O3 -std=c11 -pthread -o foldhash_verify foldhash_verify.c
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

/* ------------------------------------------------------------------ */
/* foldhash 0.2.0 re-implementation                                     */
/* ------------------------------------------------------------------ */

static const uint64_t ARBITRARY0  = 0x243f6a8885a308d3ULL;
static const uint64_t ARBITRARY5  = 0xbe5466cf34e90c6cULL;

static inline uint64_t fm(uint64_t x, uint64_t y) {          /* folded_multiply */
    unsigned __int128 full = (unsigned __int128)x * (unsigned __int128)y;
    return (uint64_t)full ^ (uint64_t)(full >> 64);
}
static inline uint64_t rotr64(uint64_t x, uint32_t r) {
    r &= 63; return r ? ((x >> r) | (x << (64 - r))) : x;    /* Rust: r mod 64 */
}
static inline uint64_t ld64(const uint8_t *p) {              /* little endian = ne on x86-64 */
    uint64_t v; memcpy(&v, p, 8); return v;
}
static inline uint32_t ld32(const uint8_t *p) {
    uint32_t v; memcpy(&v, p, 4); return v;
}

typedef struct { uint64_t s[6]; } shared_seed;

static shared_seed shared_from_u64(uint64_t seed) {          /* SharedSeed::from_u64 */
    #define MIX(x) fm((x), ARBITRARY5)
    uint64_t a = MIX(MIX(MIX(seed)));
    uint64_t b = MIX(MIX(MIX(a)));
    uint64_t c = MIX(MIX(MIX(b)));
    uint64_t d = MIX(MIX(MIX(c)));
    uint64_t e = MIX(MIX(MIX(d)));
    uint64_t f = MIX(MIX(MIX(e)));
    #undef MIX
    const uint64_t FORCED_ONES = (1ULL << 63) | (1ULL << 31) | 1ULL;
    shared_seed ss = {{ a|FORCED_ONES, b|FORCED_ONES, c|FORCED_ONES,
                        d|FORCED_ONES, e|FORCED_ONES, f|FORCED_ONES }};
    return ss;
}

static uint64_t hash_bytes_short(const uint8_t *b, size_t len, uint64_t acc, const uint64_t *sd) {
    uint64_t s0 = acc, s1 = sd[1];
    if (len >= 8) {
        s0 ^= ld64(b);
        s1 ^= ld64(b + len - 8);
    } else if (len >= 4) {
        s0 ^= (uint64_t)ld32(b);
        s1 ^= (uint64_t)ld32(b + len - 4);
    } else if (len > 0) {
        uint64_t lo = b[0], mid = b[len/2], hi = b[len-1];
        s0 ^= lo;
        s1 ^= (hi << 8) | mid;
    }
    return fm(s0, s1);
}

static uint64_t hash_bytes_long(const uint8_t *v, size_t n, uint64_t acc, const uint64_t *sd) {
    uint64_t s0 = acc, s1 = acc + sd[1];
    if (n > 128) {
        uint64_t s2 = s0 + sd[2], s3 = s0 + sd[3];
        if (n > 256) {
            uint64_t s4 = s0 + sd[4], s5 = s0 + sd[5];
            for (;;) {
                s0 = fm(ld64(v+0)  ^ s0, ld64(v+48) ^ sd[0]);
                s1 = fm(ld64(v+8)  ^ s1, ld64(v+56) ^ sd[0]);
                s2 = fm(ld64(v+16) ^ s2, ld64(v+64) ^ sd[0]);
                s3 = fm(ld64(v+24) ^ s3, ld64(v+72) ^ sd[0]);
                s4 = fm(ld64(v+32) ^ s4, ld64(v+80) ^ sd[0]);
                s5 = fm(ld64(v+40) ^ s5, ld64(v+88) ^ sd[0]);
                v += 96; n -= 96;
                if (n <= 256) break;
            }
            s0 ^= s4; s1 ^= s5;
        }
        for (;;) {
            s0 = fm(ld64(v+0)  ^ s0, ld64(v+32) ^ sd[0]);
            s1 = fm(ld64(v+8)  ^ s1, ld64(v+40) ^ sd[0]);
            s2 = fm(ld64(v+16) ^ s2, ld64(v+48) ^ sd[0]);
            s3 = fm(ld64(v+24) ^ s3, ld64(v+56) ^ sd[0]);
            v += 64; n -= 64;
            if (n <= 128) break;
        }
        s0 ^= s2; s1 ^= s3;
    }
    size_t len = n;
    s0 = fm(ld64(v+0) ^ s0, ld64(v+len-16) ^ sd[0]);
    s1 = fm(ld64(v+8) ^ s1, ld64(v+len-8)  ^ sd[0]);
    if (len >= 32) {
        s0 = fm(ld64(v+16) ^ s0, ld64(v+len-32) ^ sd[0]);
        s1 = fm(ld64(v+24) ^ s1, ld64(v+len-24) ^ sd[0]);
        if (len >= 64) {
            s0 = fm(ld64(v+32) ^ s0, ld64(v+len-48) ^ sd[0]);
            s1 = fm(ld64(v+40) ^ s1, ld64(v+len-40) ^ sd[0]);
            if (len >= 96) {
                s0 = fm(ld64(v+48) ^ s0, ld64(v+len-64) ^ sd[0]);
                s1 = fm(ld64(v+56) ^ s1, ld64(v+len-56) ^ sd[0]);
            }
        }
    }
    return s0 ^ s1;
}

/* fast::FoldHasher state machine */
typedef struct {
    uint64_t acc;
    unsigned __int128 sponge;
    uint8_t sponge_len;
    const uint64_t *sd;
} fold_hasher;

static inline void fh_init(fold_hasher *h, uint64_t per_hasher_seed, const shared_seed *ss) {
    h->acc = per_hasher_seed; h->sponge = 0; h->sponge_len = 0; h->sd = ss->s;
}
static inline void fh_write(fold_hasher *h, const uint8_t *b, size_t len) {
    h->acc = rotr64(h->acc, (uint32_t)len);
    h->acc = (len <= 16) ? hash_bytes_short(b, len, h->acc, h->sd)
                         : hash_bytes_long(b, len, h->acc, h->sd);
}
static inline void fh_write_num(fold_hasher *h, unsigned __int128 x, unsigned bits) {
    if ((unsigned)h->sponge_len + bits > 128) {
        uint64_t lo = (uint64_t)h->sponge, hi = (uint64_t)(h->sponge >> 64);
        h->acc = fm(lo ^ h->acc, hi ^ h->sd[0]);
        h->sponge = x; h->sponge_len = (uint8_t)bits;
    } else {
        h->sponge |= x << h->sponge_len;
        h->sponge_len += (uint8_t)bits;
    }
}
static inline uint64_t fh_finish(const fold_hasher *h) {
    if (h->sponge_len > 0) {
        uint64_t lo = (uint64_t)h->sponge, hi = (uint64_t)(h->sponge >> 64);
        return fm(lo ^ h->acc, hi ^ h->sd[0]);
    }
    return h->acc;
}
static inline uint64_t quality_finish(const fold_hasher *h) {   /* quality::finish */
    return fm(fh_finish(h), ARBITRARY0);
}

/* --- the four key-hashing paths ----------------------------------- */

/* raw Hasher::write(bytes) then finish() */
static uint64_t H_fast_write(uint64_t phs, const shared_seed *ss, const uint8_t *m, size_t n) {
    fold_hasher h; fh_init(&h, phs, ss); fh_write(&h, m, n); return fh_finish(&h);
}
static uint64_t H_qual_write(uint64_t phs, const shared_seed *ss, const uint8_t *m, size_t n) {
    fold_hasher h; fh_init(&h, phs, ss); fh_write(&h, m, n); return quality_finish(&h);
}
/* <Vec<u8> as Hash>::hash : write_length_prefix(len) == write_usize(len), then write(bytes) */
static uint64_t H_fast_vec(uint64_t phs, const shared_seed *ss, const uint8_t *m, size_t n) {
    fold_hasher h; fh_init(&h, phs, ss);
    fh_write_num(&h, (unsigned __int128)(uint64_t)n, 64);
    fh_write(&h, m, n);
    return fh_finish(&h);
}
static uint64_t H_qual_vec(uint64_t phs, const shared_seed *ss, const uint8_t *m, size_t n) {
    fold_hasher h; fh_init(&h, phs, ss);
    fh_write_num(&h, (unsigned __int128)(uint64_t)n, 64);
    fh_write(&h, m, n);
    return quality_finish(&h);
}
/* <str as Hash>::hash on stable: write(bytes) then write_u8(0xff) */
static uint64_t H_fast_str(uint64_t phs, const shared_seed *ss, const uint8_t *m, size_t n) {
    fold_hasher h; fh_init(&h, phs, ss);
    fh_write(&h, m, n);
    fh_write_num(&h, (unsigned __int128)0xffu, 8);
    return fh_finish(&h);
}
static uint64_t H_qual_str(uint64_t phs, const shared_seed *ss, const uint8_t *m, size_t n) {
    fold_hasher h; fh_init(&h, phs, ss);
    fh_write(&h, m, n);
    fh_write_num(&h, (unsigned __int128)0xffu, 8);
    return quality_finish(&h);
}

/* ------------------------------------------------------------------ */
/* Part 1: cross-check against the real Rust crate                     */
/* ------------------------------------------------------------------ */

typedef struct {
    uint64_t phs, shared;
    const char *hex;                 /* input bytes */
    uint64_t fast_write, qual_write, fast_vec, qual_vec, str_fast, str_qual;
    int utf8;                        /* whether the str case is meaningful */
} refcase;

/* Values printed by the real foldhash 0.2.0 crate on x86-64 (rustc 1.92.0). */
static const refcase REF[] = {
 {0x0000000000000000ULL,0x0000000000000000ULL,"0000000000000000",
  0x0000000000000000ULL,0x0000000000000000ULL,0x000000040000000cULL,0xc98521b2d359c3c7ULL,0x8000007f80000080ULL,0x3c779c0f01e37afaULL,1},
 {0x0000000000000000ULL,0x0000000000000000ULL,"ffffffffffffffff",
  0xffffffffffffffffULL,0xffffffffffffffffULL,0xfffffffb0000000bULL,0xd6b50d396e649538ULL,0,0,0},
 {0x0123456789abcdefULL,0xfedcba9876543210ULL,"0000000000000000",
  0x76515ea1a03101fbULL,0x6608bc5df43f5b74ULL,0x66ca112256c774faULL,0x647e5b7098fa10deULL,0x9ac395840828b9aeULL,0xc66cb1667e167858ULL,1},
 {0x0123456789abcdefULL,0xfedcba9876543210ULL,"ffffffffffffffff",
  0x84165e99364de5f4ULL,0x757e226dd898e948ULL,0x1126871eba2b7a20ULL,0xc9a03fd89c1af7faULL,0,0,0},
 {0xdeadbeefcafebabeULL,0x1234567890abcdefULL,"68656c6c6f20776f",
  0x699aaa658862201cULL,0x046cfd03e5b6dfacULL,0xc950c3f5c770e58bULL,0xba866fffd11076f3ULL,0x5c73ffbe42d1e643ULL,0x0cdb533a0b245e88ULL,1},
 {0xdeadbeefcafebabeULL,0x1234567890abcdefULL,
  "68656c6c6f20776f726c642c2074686973206973206c6f6e676572207468616e207369787465656e206279746573",
  0x9edcc16969c1eadfULL,0x754a82842d3203c2ULL,0x97212a55d58e8273ULL,0xf72e980797f0060bULL,0x1952ece73497548bULL,0x5c70e016b94903edULL,1},
 {0x1111111111111111ULL,0x2222222222222222ULL,"616263",
  0xab935cc9b6ebc5f5ULL,0xc61c63aec120005aULL,0x165d7a7f525affbaULL,0x079128a980943d49ULL,0x3df64b12d6be66fbULL,0xe2b23aa4ccec01d9ULL,1},
 {0x1111111111111111ULL,0x2222222222222222ULL,"",
  0x6776847774707777ULL,0xff467324c5a4ebeaULL,0xcbfa8734d4bc667aULL,0xff8aa3beeb53da39ULL,0x6474742916af6d8aULL,0xc22bb3b2b955eb7fULL,1},
 {0x8000000000000001ULL,0x00000000ffffffffULL,"3031323334353637383961626364656667",
  0x826f909c248166e9ULL,0x53f5b06bf9f1aa00ULL,0xce25e0fa23d059e7ULL,0x2ed2d0df61e27ad8ULL,0xddb43e912282f28cULL,0x9629aa1c1f515ac7ULL,1},
};

static size_t unhex(const char *hex, uint8_t *out) {
    size_t n = strlen(hex) / 2;
    for (size_t i = 0; i < n; i++) {
        unsigned v; sscanf(hex + 2*i, "%2x", &v); out[i] = (uint8_t)v;
    }
    return n;
}

static int run_verify(void) {
    int bad = 0;
    uint8_t buf[512];
    for (size_t i = 0; i < sizeof(REF)/sizeof(REF[0]); i++) {
        const refcase *c = &REF[i];
        size_t n = unhex(c->hex, buf);
        shared_seed ss = shared_from_u64(c->shared);
        uint64_t a = H_fast_write(c->phs,&ss,buf,n), b = H_qual_write(c->phs,&ss,buf,n);
        uint64_t d = H_fast_vec(c->phs,&ss,buf,n),   e = H_qual_vec(c->phs,&ss,buf,n);
        uint64_t f = c->utf8 ? H_fast_str(c->phs,&ss,buf,n) : 0;
        uint64_t g = c->utf8 ? H_qual_str(c->phs,&ss,buf,n) : 0;
        int ok = (a==c->fast_write)&&(b==c->qual_write)&&(d==c->fast_vec)&&
                 (e==c->qual_vec)&&(f==c->str_fast)&&(g==c->str_qual);
        printf("case %zu len=%zu : %s\n", i, n, ok ? "MATCH" : "MISMATCH");
        if (!ok) {
            bad++;
            printf("   C   fast_write=%016llx qual_write=%016llx fast_vec=%016llx qual_vec=%016llx str_fast=%016llx str_qual=%016llx\n",
                (unsigned long long)a,(unsigned long long)b,(unsigned long long)d,
                (unsigned long long)e,(unsigned long long)f,(unsigned long long)g);
            printf("   rust fast_write=%016llx qual_write=%016llx fast_vec=%016llx qual_vec=%016llx str_fast=%016llx str_qual=%016llx\n",
                (unsigned long long)c->fast_write,(unsigned long long)c->qual_write,
                (unsigned long long)c->fast_vec,(unsigned long long)c->qual_vec,
                (unsigned long long)c->str_fast,(unsigned long long)c->str_qual);
        }
    }
    printf(bad ? "VERIFY: FAILED (%d)\n" : "VERIFY: all %d cases match the real Rust crate\n",
           bad ? bad : (int)(sizeof(REF)/sizeof(REF[0])));
    return bad;
}

/* ------------------------------------------------------------------ */
/* Part 2: measurement                                                  */
/* ------------------------------------------------------------------ */

/* xoshiro256++ */
typedef struct { uint64_t s[4]; } rng_t;
static inline uint64_t rotl(uint64_t x, int k){ return (x<<k)|(x>>(64-k)); }
static inline uint64_t rng_next(rng_t *r) {
    uint64_t res = rotl(r->s[0] + r->s[3], 23) + r->s[0];
    uint64_t t = r->s[1] << 17;
    r->s[2] ^= r->s[0]; r->s[3] ^= r->s[1]; r->s[1] ^= r->s[2]; r->s[0] ^= r->s[3];
    r->s[2] ^= t; r->s[3] = rotl(r->s[3], 45);
    return res;
}
static uint64_t splitmix(uint64_t *x){
    uint64_t z = (*x += 0x9e3779b97f4a7c15ULL);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    return z ^ (z >> 31);
}

#define MAXMSG 512
static uint8_t M1[MAXMSG], M2[MAXMSG];
static size_t L1, L2;
static int MODEL;                 /* 0 = uniform independent seed words, 1 = SharedSeed::from_u64 */
static uint64_t NPER;             /* iterations per thread */

typedef struct {
    uint64_t seed;
    uint64_t c_fast_write, c_qual_write, c_fast_vec, c_qual_vec;
    uint64_t c_qual_not_fast;     /* quality collides but fast does not */
} task_t;

static void *worker(void *arg) {
    task_t *t = (task_t*)arg;
    rng_t r; uint64_t sm = t->seed;
    for (int i = 0; i < 4; i++) r.s[i] = splitmix(&sm);
    for (int i = 0; i < 16; i++) (void)rng_next(&r);
    uint64_t cfw=0, cqw=0, cfv=0, cqv=0, cqnf=0;
    for (uint64_t it = 0; it < NPER; it++) {
        uint64_t phs = rng_next(&r);
        shared_seed ss;
        if (MODEL == 0) { for (int k = 0; k < 6; k++) ss.s[k] = rng_next(&r); }
        else            { ss = shared_from_u64(rng_next(&r)); }

        uint64_t a1 = H_fast_write(phs,&ss,M1,L1), a2 = H_fast_write(phs,&ss,M2,L2);
        uint64_t b1 = fm(a1, ARBITRARY0),          b2 = fm(a2, ARBITRARY0);
        int fw = (a1 == a2), qw = (b1 == b2);
        cfw += fw; cqw += qw; cqnf += (qw && !fw);
        uint64_t c1 = H_fast_vec(phs,&ss,M1,L1),  c2 = H_fast_vec(phs,&ss,M2,L2);
        cfv += (c1 == c2);
        cqv += (fm(c1,ARBITRARY0) == fm(c2,ARBITRARY0));
    }
    t->c_fast_write=cfw; t->c_qual_write=cqw; t->c_fast_vec=cfv;
    t->c_qual_vec=cqv; t->c_qual_not_fast=cqnf;
    return NULL;
}

/* "dump": print the colliding hidden-seed values so the mechanism can be
 * inspected -- for the len-8 pair the whole hash depends only on
 * a = rotate_right(per_hasher_seed, 8) and b = seeds[1].                */
static int run_dump(const char *h1, const char *h2, int log2N, int maxprint) {
    L1 = unhex(h1, M1); L2 = unhex(h2, M2);
    rng_t r; uint64_t sm = 0xd00d2026091804ULL;
    for (int i = 0; i < 4; i++) r.s[i] = splitmix(&sm);
    int printed = 0;
    for (uint64_t it = 0; it < (1ULL << log2N); it++) {
        uint64_t phs = rng_next(&r);
        shared_seed ss; for (int k = 0; k < 6; k++) ss.s[k] = rng_next(&r);
        if (H_fast_write(phs,&ss,M1,L1) == H_fast_write(phs,&ss,M2,L2)) {
            uint64_t a = rotr64(phs, (uint32_t)L1), b = ss.s[1];
            unsigned __int128 pr = (unsigned __int128)a * b;
            printf("HIT a=%016llx b=%016llx a+b+1=%016llx lo=%016llx hi=%016llx\n",
                (unsigned long long)a,(unsigned long long)b,
                (unsigned long long)(a+b+1),
                (unsigned long long)(uint64_t)pr,(unsigned long long)(uint64_t)(pr>>64));
            if (++printed >= maxprint) break;
        }
    }
    printf("dump: %d hits printed\n", printed);
    return 0;
}

int main(int argc, char **argv) {
    if (argc >= 2 && strcmp(argv[1], "verify") == 0) return run_verify();
    if (argc >= 6 && strcmp(argv[1], "dump") == 0)
        return run_dump(argv[2], argv[3], atoi(argv[4]), atoi(argv[5]));

    if (argc < 7) {
        fprintf(stderr,
          "usage: %s verify\n"
          "       %s measure <m1hex> <m2hex> <log2N> <threads> <model 0|1> [rngseed]\n"
          "  model 0: per_hasher_seed and all six SharedSeed words uniform & independent\n"
          "  model 1: per_hasher_seed uniform, SharedSeed = SharedSeed::from_u64(uniform)\n", argv[0], argv[0]);
        return 2;
    }
    L1 = unhex(argv[2], M1);
    L2 = unhex(argv[3], M2);
    int log2N  = atoi(argv[4]);
    int nthr   = atoi(argv[5]);
    MODEL      = atoi(argv[6]);
    uint64_t rngseed = (argc >= 8) ? strtoull(argv[7], NULL, 0)
                                   : (uint64_t)time(NULL) * 0x9e3779b97f4a7c15ULL;
    if (log2N < 1 || nthr < 1 || nthr > 64) { fprintf(stderr, "bad args\n"); return 2; }

    /* Always re-run the Rust cross-check before measuring. */
    if (run_verify() != 0) { fprintf(stderr, "refusing to measure: model does not match Rust\n"); return 1; }

    int lg_thr = 0; while ((1 << lg_thr) < nthr) lg_thr++;
    if ((1 << lg_thr) != nthr) { fprintf(stderr, "threads must be a power of two\n"); return 2; }
    if (log2N <= lg_thr) { fprintf(stderr, "log2N too small\n"); return 2; }
    NPER = 1ULL << (log2N - lg_thr);

    printf("measure: m1=%s (len %zu)  m2=%s (len %zu)  N=2^%d  threads=%d  model=%d  rngseed=0x%016llx\n",
           argv[2], L1, argv[3], L2, log2N, nthr, MODEL, (unsigned long long)rngseed);

    pthread_t th[64]; task_t tk[64];
    struct timespec t0, t1; clock_gettime(CLOCK_MONOTONIC, &t0);
    uint64_t sm = rngseed;
    for (int i = 0; i < nthr; i++) {
        memset(&tk[i], 0, sizeof(tk[i]));
        tk[i].seed = splitmix(&sm);
        pthread_create(&th[i], NULL, worker, &tk[i]);
    }
    uint64_t cfw=0,cqw=0,cfv=0,cqv=0,cqnf=0;
    for (int i = 0; i < nthr; i++) {
        pthread_join(th[i], NULL);
        cfw+=tk[i].c_fast_write; cqw+=tk[i].c_qual_write; cfv+=tk[i].c_fast_vec;
        cqv+=tk[i].c_qual_vec;   cqnf+=tk[i].c_qual_not_fast;
    }
    clock_gettime(CLOCK_MONOTONIC, &t1);
    double secs = (t1.tv_sec-t0.tv_sec) + 1e-9*(t1.tv_nsec-t0.tv_nsec);
    printf("N=2^%d  hits: fast_write=%llu  quality_write=%llu  fast_vec(HashMap<Vec<u8>>)=%llu  quality_vec=%llu  quality_only=%llu  (%.1f s)\n",
        log2N,(unsigned long long)cfw,(unsigned long long)cqw,(unsigned long long)cfv,
        (unsigned long long)cqv,(unsigned long long)cqnf, secs);
    return 0;
}
