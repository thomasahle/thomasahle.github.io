/*
 * ahash_pairs.c -- fixed message pairs that collide under aHash (Rust crate
 * `ahash` 0.8.12, https://github.com/tkaitchuck/aHash) with elevated
 * probability over a uniformly random secret.
 *
 * Copyright (c) 2026 Thomas Dybdahl Ahle (this program)
 * Copyright (c) 2018 Tom Kaitchuck (aHash, MIT OR Apache-2.0: the two constants PI2 and
 *   SHUFFLE_MASK below are copied from the crate, everything else is written from it)
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Single file, C11, libc + libm only.  It contains
 *   1. aHash re-implemented from the crate source, both code paths, in the
 *      exact variant SMHasher3 (hashes/rust-ahash.cpp) tests as `rust_ahash`
 *      (AES path) and `rust_ahash_fb` (fallback path): the hash of a byte
 *      slice &[u8] -- write_usize(len) followed by write(bytes) -- keyed by
 *      the four-word RandomState {k0,k1,k2,k3};
 *   2. the SMHasher3 verification procedure, run at startup: keys {}, {0},
 *      {0,1}, ..., {0..254} hashed with seed 256-i (k_j = PI2[j] ^ seed),
 *      the 2048 output bytes hashed with seed 0, first four bytes LE.  The
 *      program aborts unless both variants reproduce their registered values;
 *   3. the message pairs, each hashed under N random secrets in two models:
 *      rs4 = four independent uniform 64-bit keys (what RandomState::new()
 *      amounts to), smh = one uniform 64-bit seed with k_j = PI2[j] ^ seed
 *      (SMHasher3's convention, == RandomState::with_seeds(s,s,s,s)).
 *
 * Build (choose the line for your CPU; the last one needs no flags at all):
 *   aarch64: cc -O2 -march=armv8-a+crypto -o ahash_pairs ahash_pairs.c -lm
 *   x86-64:  cc -O2 -maes -o ahash_pairs ahash_pairs.c -lm
 *   any:     cc -O2 -o ahash_pairs ahash_pairs.c -lm    (software AES, slower)
 * Run:  ./ahash_pairs [log2 N = 24] [rng seed = 1] [pair tag A|B = all]
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

/* ---------------------------------------------------------------- 128-bit lanes */
typedef union { uint8_t b[16]; uint64_t w[2]; } v128;   /* little-endian host assumed */
static v128 v_make(uint64_t lo, uint64_t hi) { v128 r; r.w[0] = lo; r.w[1] = hi; return r; }
static v128 v_load(const uint8_t *p) { v128 r; memcpy(r.b, p, 16); return r; }
static v128 v_xor(v128 a, v128 b) { a.w[0] ^= b.w[0]; a.w[1] ^= b.w[1]; return a; }
static v128 v_add64(v128 a, v128 b) { a.w[0] += b.w[0]; a.w[1] += b.w[1]; return a; }
static v128 v_not(v128 a) { a.w[0] = ~a.w[0]; a.w[1] = ~a.w[1]; return a; }
static uint64_t rd64(const uint8_t *p) { uint64_t x; memcpy(&x, p, 8); return x; }
static uint32_t rd32(const uint8_t *p) { uint32_t x; memcpy(&x, p, 4); return x; }
static uint16_t rd16(const uint8_t *p) { uint16_t x; memcpy(&x, p, 2); return x; }
static uint64_t rotl64(uint64_t x, unsigned r) { r &= 63; return r ? (x << r) | (x >> (64 - r)) : x; }

/* ---------------------------------------------------------------- one AES round
 * aes_enc(s,k) = MixColumns(ShiftRows(SubBytes(s))) ^ k          (x86 AESENC)
 * aes_dec(s,k) = InvMixColumns(InvSubBytes(InvShiftRows(s))) ^ k (x86 AESDEC)
 * These are the `aesenc`/`aesdec` of ahash/src/operations.rs on every target. */
#if !defined(AHASH_SOFT_AES) && defined(__aarch64__) && (defined(__ARM_FEATURE_AES) || defined(__ARM_FEATURE_CRYPTO))
#include <arm_neon.h>
#define AES_BACKEND "ARMv8 AES instructions"
static void aes_init(void) {}
static v128 aes_enc(v128 s, v128 k) {
    uint8x16_t x = veorq_u8(vaesmcq_u8(vaeseq_u8(vld1q_u8(s.b), vdupq_n_u8(0))), vld1q_u8(k.b));
    v128 r; vst1q_u8(r.b, x); return r;
}
static v128 aes_dec(v128 s, v128 k) {
    uint8x16_t x = veorq_u8(vaesimcq_u8(vaesdq_u8(vld1q_u8(s.b), vdupq_n_u8(0))), vld1q_u8(k.b));
    v128 r; vst1q_u8(r.b, x); return r;
}
#elif !defined(AHASH_SOFT_AES) && defined(__x86_64__) && defined(__AES__)
#include <wmmintrin.h>
#define AES_BACKEND "AES-NI"
static void aes_init(void) {}
static v128 aes_enc(v128 s, v128 k) {
    __m128i x = _mm_aesenc_si128(_mm_loadu_si128((const __m128i *)s.b), _mm_loadu_si128((const __m128i *)k.b));
    v128 r; _mm_storeu_si128((__m128i *)r.b, x); return r;
}
static v128 aes_dec(v128 s, v128 k) {
    __m128i x = _mm_aesdec_si128(_mm_loadu_si128((const __m128i *)s.b), _mm_loadu_si128((const __m128i *)k.b));
    v128 r; _mm_storeu_si128((__m128i *)r.b, x); return r;
}
#else
#define AES_BACKEND "portable software AES (FIPS-197 tables built at startup)"
static uint8_t SB[256], ISB[256], GM[16][256];              /* S-box, inverse, GF(2^8) multiples */
static uint8_t gmul(uint8_t a, uint8_t b) {
    uint8_t r = 0;
    while (b) { if (b & 1) r ^= a; a = (uint8_t)((a << 1) ^ ((a >> 7) * 0x1b)); b >>= 1; }
    return r;
}
static void aes_init(void) {
    for (int x = 0; x < 256; x++) {                          /* S-box = affine(inverse(x)) */
        uint8_t inv = 0;
        for (int y = 1; x && y < 256; y++) if (gmul((uint8_t)x, (uint8_t)y) == 1) { inv = (uint8_t)y; break; }
        uint8_t s = inv, t = inv;
        for (int i = 0; i < 4; i++) { t = (uint8_t)((t << 1) | (t >> 7)); s ^= t; }
        SB[x] = s ^ 0x63; ISB[SB[x]] = (uint8_t)x;
    }
    for (int m = 0; m < 16; m++) for (int x = 0; x < 256; x++) GM[m][x] = gmul((uint8_t)m, (uint8_t)x);
}
static const uint8_t MC[4][4]  = {{2, 3, 1, 1}, {1, 2, 3, 1}, {1, 1, 2, 3}, {3, 1, 1, 2}};
static const uint8_t IMC[4][4] = {{14, 11, 13, 9}, {9, 14, 11, 13}, {13, 9, 14, 11}, {11, 13, 9, 14}};
static v128 aes_round(v128 s, v128 k, int inv) {            /* state byte i = row i%4, column i/4 */
    v128 t, o;
    for (int i = 0; i < 16; i++) {                           /* (Inv)ShiftRows, then (Inv)SubBytes */
        int r = i & 3, src = inv ? (i - 4 * r) & 15 : (i + 4 * r) & 15;
        t.b[i] = inv ? ISB[s.b[src]] : SB[s.b[src]];
    }
    for (int c = 0; c < 4; c++) for (int r = 0; r < 4; r++) { /* (Inv)MixColumns, then add key */
        const uint8_t *m = inv ? IMC[r] : MC[r]; uint8_t v = 0;
        for (int j = 0; j < 4; j++) v ^= GM[m[j]][t.b[4 * c + j]];
        o.b[4 * c + r] = v ^ k.b[4 * c + r];
    }
    return o;
}
static v128 aes_enc(v128 s, v128 k) { return aes_round(s, k, 0); }
static v128 aes_dec(v128 s, v128 k) { return aes_round(s, k, 1); }
#endif

/* ---------------------------------------------------------------- aHash constants */
static const uint64_t PI2[4] = {              /* random_state.rs; with_seeds() xors these in */
    UINT64_C(0x452821e638d01377), UINT64_C(0xbe5466cf34e90c6c),
    UINT64_C(0xc0ac29b7c97c50dd), UINT64_C(0x3f84d5b5b5470917),
};
static const uint8_t SHUFFLE_MASK[16] = {     /* operations.rs SHUFFLE_MASK, as pshufb bytes */
    0x4, 0xb, 0x9, 0x6, 0x8, 0xd, 0xf, 0x5, 0xe, 0x3, 0x1, 0xc, 0x0, 0x7, 0xa, 0x2,
};
#define MULTIPLE UINT64_C(6364136223846793005)

/* read_small(): 0..8 bytes into two words (order/duplication not guaranteed, as upstream) */
static void read_small(const uint8_t *p, size_t n, uint64_t o[2]) {
    if (n >= 4)      { o[0] = rd32(p); o[1] = rd32(p + n - 4); }
    else if (n >= 2) { o[0] = rd16(p); o[1] = p[n - 1]; }
    else             { o[0] = o[1] = n ? p[0] : 0; }
}

/* ---------------------------------------------------------------- AES path (aes_hash.rs) */
typedef struct { v128 enc, sum, key; } aes_hasher;
static v128 shuffle(v128 a) { v128 r; for (int i = 0; i < 16; i++) r.b[i] = a.b[SHUFFLE_MASK[i]]; return r; }
static void hash_in(aes_hasher *h, v128 v) {
    h->enc = aes_dec(h->enc, v);                              /* enc <- InvRound(enc) ^ v        */
    h->sum = v_add64(shuffle(h->sum), v);                     /* sum <- shuffle(sum) + v (2x64)  */
}
static uint64_t ahash_aes(const uint8_t *d, size_t n, const uint64_t k[4]) {
    aes_hasher h;
    h.enc = v_make(k[0], k[1]); h.sum = v_make(k[2], k[3]); h.key = v_xor(h.enc, h.sum);
    hash_in(&h, v_make((uint64_t)n, 0));                      /* <[u8] as Hash>: write_usize(len) */
    h.enc.w[0] += (uint64_t)n;                                /* write(): add_in_length           */
    if (n <= 8) {
        uint64_t v[2]; read_small(d, n, v); hash_in(&h, v_make(v[0], v[1]));
    } else if (n <= 16) {
        hash_in(&h, v_make(rd64(d), rd64(d + n - 8)));
    } else if (n <= 32) {
        hash_in(&h, v_load(d)); hash_in(&h, v_load(d + n - 16));
    } else if (n <= 64) {
        hash_in(&h, v_load(d)); hash_in(&h, v_load(d + 16));
        hash_in(&h, v_load(d + n - 32)); hash_in(&h, v_load(d + n - 16));
    } else {
        v128 cur[4] = {h.key, h.key, h.key, h.key}, sum[2] = {h.key, v_not(h.key)};
        const uint8_t *t = d + n - 64;
        cur[0] = aes_enc(cur[0], v_load(t));      sum[0] = v_add64(sum[0], v_load(t));
        cur[1] = aes_dec(cur[1], v_load(t + 16)); sum[1] = v_add64(sum[1], v_load(t + 16));
        cur[2] = aes_enc(cur[2], v_load(t + 32)); sum[0] = v_add64(shuffle(sum[0]), v_load(t + 32));
        cur[3] = aes_dec(cur[3], v_load(t + 48)); sum[1] = v_add64(shuffle(sum[1]), v_load(t + 48));
        for (size_t left = n; left > 64; left -= 64, d += 64)
            for (int i = 0; i < 4; i++) {
                v128 b = v_load(d + 16 * i);
                cur[i] = aes_dec(cur[i], b); sum[i & 1] = v_add64(shuffle(sum[i & 1]), b);
            }
        for (int i = 0; i < 4; i++) hash_in(&h, cur[i]);
        hash_in(&h, sum[0]); hash_in(&h, sum[1]);
    }
    v128 c = aes_enc(h.sum, h.enc);                           /* finish() */
    return aes_dec(aes_dec(c, h.key), c).w[0];
}

/* ---------------------------------------------------------------- fallback path (fallback_hash.rs) */
static uint64_t folded_multiply(uint64_t a, uint64_t b) {    /* lo64(a*b) ^ hi64(a*b) */
#ifdef __SIZEOF_INT128__
    unsigned __int128 p = (unsigned __int128)a * b; return (uint64_t)p ^ (uint64_t)(p >> 64);
#else
    uint64_t a0 = (uint32_t)a, a1 = a >> 32, b0 = (uint32_t)b, b1 = b >> 32;
    uint64_t p00 = a0 * b0, p01 = a0 * b1, p10 = a1 * b0, p11 = a1 * b1;
    uint64_t mid = (p00 >> 32) + (uint32_t)p01 + (uint32_t)p10;
    return ((mid << 32) | (uint32_t)p00) ^ (p11 + (p01 >> 32) + (p10 >> 32) + (mid >> 32));
#endif
}
typedef struct { uint64_t buffer, pad, ek0, ek1; } fb_hasher;
static void large_update(fb_hasher *h, uint64_t b0, uint64_t b1) {
    uint64_t combined = folded_multiply(b0 ^ h->ek0, b1 ^ h->ek1);
    h->buffer = rotl64((h->buffer + h->pad) ^ combined, 23);
}
static uint64_t ahash_fb(const uint8_t *d, size_t n, const uint64_t k[4]) {
    fb_hasher h = {k[1], k[0], k[2], k[3]};                   /* buffer, pad, extra_keys */
    h.buffer = folded_multiply(h.buffer ^ (uint64_t)n, MULTIPLE);   /* write_usize(len) */
    h.buffer = (h.buffer + (uint64_t)n) * MULTIPLE;                 /* write(): length mix */
    if (n > 16) {
        large_update(&h, rd64(d + n - 16), rd64(d + n - 8));
        for (size_t left = n; left > 16; left -= 16, d += 16) large_update(&h, rd64(d), rd64(d + 8));
    } else if (n > 8) {
        large_update(&h, rd64(d), rd64(d + n - 8));
    } else {
        uint64_t v[2]; read_small(d, n, v); large_update(&h, v[0], v[1]);
    }
    return rotl64(folded_multiply(h.buffer, h.pad), (unsigned)(h.buffer & 63));   /* finish() */
}

/* ---------------------------------------------------------------- SMHasher3 verification */
typedef uint64_t (*hashfn)(const uint8_t *, size_t, const uint64_t[4]);
static void keys_from_seed(uint64_t seed, uint64_t k[4]) { for (int j = 0; j < 4; j++) k[j] = PI2[j] ^ seed; }
static uint32_t smhasher3_verify(hashfn H) {
    uint8_t key[256] = {0}, hashes[2048]; uint64_t k[4], h;
    for (int i = 0; i < 256; i++) {
        keys_from_seed((uint64_t)(256 - i), k);
        h = H(key, (size_t)i, k); memcpy(hashes + 8 * i, &h, 8);
        key[i] = (uint8_t)i;
    }
    keys_from_seed(0, k);
    h = H(hashes, sizeof hashes, k);
    return (uint32_t)h;                                       /* first four output bytes, LE */
}
static void validate(const char *name, hashfn H, uint32_t want) {
    uint32_t got = smhasher3_verify(H);
    printf("  %-14s SMHasher3 verification 0x%08X (registered 0x%08X) %s\n", name, got, want, got == want ? "OK" : "MISMATCH");
    if (got != want) { fflush(stdout); fprintf(stderr, "validation failed for %s -- aborting\n", name); exit(1); }
}

/* ---------------------------------------------------------------- RNG: splitmix64 -> xoshiro256** */
static uint64_t splitmix64(uint64_t *s) {
    uint64_t z = (*s += UINT64_C(0x9e3779b97f4a7c15));
    z = (z ^ (z >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
    z = (z ^ (z >> 27)) * UINT64_C(0x94d049bb133111eb);
    return z ^ (z >> 31);
}
typedef struct { uint64_t s[4]; } xoshiro;
static void xo_seed(xoshiro *r, uint64_t seed) { for (int i = 0; i < 4; i++) r->s[i] = splitmix64(&seed); }
static uint64_t xo_next(xoshiro *r) {
    uint64_t *s = r->s, res = rotl64(s[1] * 5, 7) * 9, t = s[1] << 17;
    s[2] ^= s[0]; s[3] ^= s[1]; s[1] ^= s[2]; s[0] ^= s[3]; s[2] ^= t; s[3] = rotl64(s[3], 45);
    return res;
}

/* ---------------------------------------------------------------- the pairs */
typedef struct { const char *tag, *variant; hashfn H; const char *m1, *m2; uint64_t keys[4]; const char *mech; } pair_t;
static const pair_t PAIRS[] = {
    {"A", "rust_ahash (AES path), 56-byte messages", ahash_aes,
     "40313233343536373839253b3c3d3e3f408142094445464748494a4b4c4d4e4f8d896d065c5d5e5f606162636465666768696a6b6c6d6e6f",
     "3e313233343536373839403b3c3d3e3f405e42204445464748494a4b4c4d4e4f727290085c5d5e5f606162636465666768696a6b6c6d6e6f",
     {UINT64_C(0x711096b41e1b7d99), UINT64_C(0x59af3b7ae3b7d595), UINT64_C(0xd2fb75eb5658a771), UINT64_C(0x5a0f91d8ed1ffe38)},
     "3-S-box differential trail through hash_in: the byte differences in block 1 (bytes 0,10)\n"
     "  are chosen so that InvShiftRows puts them in one column and InvMixColumns of the inverse-S-box\n"
     "  outputs (DDT 4/256 each) leaves one byte, which block 2 cancels except for one byte (DDT 2/256)\n"
     "  that block 3 cancels; the sum lane cancels additively through the shuffle.  Predicted\n"
     "  4/256 * 4/256 * 2/256 * (carry factor ~2^-0.7) = 2^-19.7; unconditional over the keys."},
    {"B", "rust_ahash_fb (fallback path), 16-byte messages", ahash_fb,
     "0123456789abcdef0011223344556677",
     "fedcba9876543210ffeeddccbbaa9988",
     {UINT64_C(0x64721c4a2c381d36), UINT64_C(0x6b5648524261eaab), UINT64_C(0xb59e91f82c7d1e12), UINT64_C(0x94c815ea45f3d238)},
     "complement both words of a one-block message: the single folded multiply sees (~a)(~b) =\n"
     "  a*b + (a+b+1) - 2^64*(a+b+2) mod 2^128 with a = w0^k2, b = w1^k3, so the low and high halves\n"
     "  of the product move by nearly opposite amounts and lo^hi cancels for about 2^-26 of the keys\n"
     "  (panel: 55/2^32 = 2^-26.2; below the resolution of a 2^24 run, see README)."},
};

static size_t unhex(const char *h, uint8_t *o) {
    size_t n = strlen(h) / 2;
    for (size_t i = 0; i < n; i++) { unsigned v; sscanf(h + 2 * i, "%2x", &v); o[i] = (uint8_t)v; }
    return n;
}
static void print_rate(uint64_t c, int lg) {
    printf("collisions/N = %llu/2^%d", (unsigned long long)c, lg);
    if (c) printf(" = 2^%.2f\n", log2((double)c) - lg); else printf("  (none observed in 2^%d trials; no population bound)\n", lg);
}
static void print_keys(const uint64_t k[4]) {
    printf("k0..k3 = %016llx %016llx %016llx %016llx", (unsigned long long)k[0], (unsigned long long)k[1],
           (unsigned long long)k[2], (unsigned long long)k[3]);
}

/* hash both messages under N random secrets; model 0 = rs4, 1 = smh */
static void run_model(const pair_t *p, const uint8_t *m1, size_t n1, const uint8_t *m2, size_t n2,
                      int model, int lg, uint64_t rngseed) {
    xoshiro r; xo_seed(&r, rngseed ^ (model ? UINT64_C(0x5eed) : 0));
    uint64_t N = (uint64_t)1 << lg, c = 0, k[4], ex[4] = {0}, exs = 0, exh = 0; int have = 0;
    clock_t t0 = clock();
    for (uint64_t i = 0; i < N; i++) {
        uint64_t s = 0;
        if (model == 0) { for (int j = 0; j < 4; j++) k[j] = xo_next(&r); }
        else { s = xo_next(&r); keys_from_seed(s, k); }
        uint64_t h1 = p->H(m1, n1, k);
        if (h1 == p->H(m2, n2, k)) { c++; if (!have) { have = 1; memcpy(ex, k, 32); exs = s; exh = h1; } }
    }
    double secs = (double)(clock() - t0) / CLOCKS_PER_SEC;
    printf("  model %s: ", model ? "smh (one uniform seed, k_j = PI2[j]^seed)" : "rs4 (four uniform keys k0..k3)      ");
    print_rate(c, lg);
    printf("    %.1f s;  ", secs);
    if (!have) { printf("no collision found in this run\n"); return; }
    printf("first colliding secret: ");
    if (model) printf("seed = %016llx, ", (unsigned long long)exs);
    print_keys(ex); printf("\n    H(m1) = H(m2) = %016llx\n", (unsigned long long)exh);
}

int main(int argc, char **argv) {
    char *end = NULL;
    int lg = argc > 1 ? (int)strtol(argv[1], &end, 10) : 24;
    if (argc > 1 && (end == argv[1] || *end)) { fprintf(stderr, "usage: %s [log2 N = 24] [rng seed = 1] [pair tag A|B]\n", argv[0]); return 2; }
    uint64_t rngseed = argc > 2 ? strtoull(argv[2], &end, 0) : 1;
    if (argc > 2 && (end == argv[2] || *end)) { fprintf(stderr, "usage: %s [log2 N = 24] [rng seed = 1] [pair tag A|B]\n", argv[0]); return 2; }
    const char *only = argc > 3 ? argv[3] : NULL;               /* run just this pair */
    if (only) {
        int known = 0;
        for (size_t pi = 0; pi < sizeof PAIRS / sizeof PAIRS[0]; pi++) known |= strcmp(only, PAIRS[pi].tag) == 0;
        if (!known) {
            fprintf(stderr, "unknown pair tag '%s'; valid tags:", only);
            for (size_t pi = 0; pi < sizeof PAIRS / sizeof PAIRS[0]; pi++) fprintf(stderr, " %s", PAIRS[pi].tag);
            fprintf(stderr, "\n"); return 2;
        }
    }
    uint16_t one = 1;
    if (*(uint8_t *)&one != 1 || lg < 1 || lg > 62) { fprintf(stderr, "little-endian host and 1 <= log2 N <= 62 required\n"); return 1; }
    aes_init();
    printf("rust-ahash: aHash 0.8.12 as tested by SMHasher3 (hashes/rust-ahash.cpp); AES backend: %s\n", AES_BACKEND);
    validate("rust_ahash", ahash_aes, 0x3BF4383B);
    validate("rust_ahash_fb", ahash_fb, 0x53C9F167);
    printf("N = 2^%d random secrets per model, rng seed %llu\n", lg, (unsigned long long)rngseed);

    int bad = 0;
    for (size_t pi = 0; pi < sizeof PAIRS / sizeof PAIRS[0]; pi++) {
        const pair_t *p = &PAIRS[pi];
        if (only && strcmp(only, p->tag) != 0) continue;
        uint8_t m1[256], m2[256];
        size_t n1 = unhex(p->m1, m1), n2 = unhex(p->m2, m2);
        printf("\n== pair %s: %s\n  m1 = %s\n  m2 = %s\n  xor= ", p->tag, p->variant, p->m1, p->m2);
        for (size_t i = 0; i < n1; i++) printf("%02x", n1 == n2 ? m1[i] ^ m2[i] : 0);
        printf("\n  mechanism: %s\n", p->mech);
        uint64_t h1 = p->H(m1, n1, p->keys), h2 = p->H(m2, n2, p->keys);
        printf("  explicit colliding secret: "); print_keys(p->keys);
        printf("\n    H(m1) = %016llx  H(m2) = %016llx  %s\n", (unsigned long long)h1, (unsigned long long)h2,
               h1 == h2 ? "COLLIDE" : "DO NOT COLLIDE");
        if (h1 != h2) bad = 1;
        run_model(p, m1, n1, m2, n2, 0, lg, rngseed);
        run_model(p, m1, n1, m2, n2, 1, lg, rngseed);
    }
    if (bad) fprintf(stderr, "\nan explicit secret did not collide -- the pair records are inconsistent\n");
    return bad;
}
