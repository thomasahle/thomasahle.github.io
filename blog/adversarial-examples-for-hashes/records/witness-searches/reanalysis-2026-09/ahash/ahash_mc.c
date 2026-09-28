/* ahash_mc.c -- aHash 0.8.12 AES path (byte slice via Hash: write_usize(len) then write(bytes), i.e. hash_one(&[u8])):
 * (1) re-verification of the published 424-byte lane-echo pair E; (2) lane-tied echo multicollisions.
 *
 * The hash is transcribed from the SMHasher3 port hashes/rust-ahash.cpp (from_random_state, hash_in, add_data, finish;
 * constants PI2 and SHUFFLE_MASK), which follows aHash src/aes_hash.rs.  The AES rounds are an independent byte-level
 * implementation (x86 AESENC/AESDEC semantics) with an optional AES-NI / ARMv8-AES path cross-checked against it.
 * Startup check: SMHasher3 verification value rust_ahash = 0x3BF4383B (seed expansion k_j = PI2[j] ^ seed);
 * exit 1 on mismatch.
 * Key model for all sampling: four independent uniform internal RandomState words (with_seeds(a,b,c,d) with
 * uniform arguments; the arguments XOR PI2 give the internal words bijectively).
 *
 * Build: cc -O2 -std=c11 -maes -pthread ahash_mc.c -o ahash_mc -lm      (arm64: -march=armv8-a+crypto)
 * Run:   ./ahash_mc pairE LOG2N THREADS SEED
 *        ./ahash_mc tie   LOG2N THREADS SEED        (448-byte, four echo slots, 16 messages per key)
 * MIT licence.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>

typedef struct { uint8_t b[16]; } blk;
static uint8_t SB[256], ISB[256];
static uint8_t xt(uint8_t a) { return (uint8_t)((a << 1) ^ ((a & 0x80) ? 0x1b : 0)); }
static uint8_t gm(uint8_t a, uint8_t b) { uint8_t r = 0; while (b) { if (b & 1) r ^= a; a = xt(a); b >>= 1; } return r; }
static void init_sbox(void) {
    for (int x = 0; x < 256; x++) { uint8_t inv = 0;
        if (x) for (int y = 1; y < 256; y++) if (gm((uint8_t)x, (uint8_t)y) == 1) { inv = (uint8_t)y; break; }
        uint8_t s = inv, r = inv; for (int i = 0; i < 4; i++) { r = (uint8_t)((r << 1) | (r >> 7)); s ^= r; }
        SB[x] = s ^ 0x63; ISB[s ^ 0x63] = (uint8_t)x; }
}
static blk sw_enc(blk a, blk k) { blk t, o; for (int c = 0; c < 4; c++) for (int r = 0; r < 4; r++) t.b[4*c+r] = SB[a.b[4*((c+r)&3)+r]];
    for (int c = 0; c < 4; c++) { uint8_t *s = t.b + 4*c; for (int r = 0; r < 4; r++) o.b[4*c+r] = gm(s[r],2) ^ gm(s[(r+1)&3],3) ^ s[(r+2)&3] ^ s[(r+3)&3] ^ k.b[4*c+r]; } return o; }
static blk sw_dec(blk a, blk k) { blk t, o; for (int c = 0; c < 4; c++) for (int r = 0; r < 4; r++) t.b[4*c+r] = ISB[a.b[4*((c-r)&3)+r]];
    for (int c = 0; c < 4; c++) { uint8_t *s = t.b + 4*c; for (int r = 0; r < 4; r++) o.b[4*c+r] = gm(s[r],14) ^ gm(s[(r+1)&3],11) ^ gm(s[(r+2)&3],13) ^ gm(s[(r+3)&3],9) ^ k.b[4*c+r]; } return o; }
#if defined(__AES__) && !defined(PORTABLE)
#include <wmmintrin.h>
#define IMPL "x86-aesni"
static inline blk enc(blk a, blk k) { blk o; _mm_storeu_si128((__m128i*)o.b, _mm_aesenc_si128(_mm_loadu_si128((const __m128i*)a.b), _mm_loadu_si128((const __m128i*)k.b))); return o; }
static inline blk dec(blk a, blk k) { blk o; _mm_storeu_si128((__m128i*)o.b, _mm_aesdec_si128(_mm_loadu_si128((const __m128i*)a.b), _mm_loadu_si128((const __m128i*)k.b))); return o; }
#elif defined(__ARM_FEATURE_AES) && !defined(PORTABLE)
#include <arm_neon.h>
#define IMPL "armv8-aes"
static inline blk enc(blk a, blk k) { blk o; vst1q_u8(o.b, veorq_u8(vaesmcq_u8(vaeseq_u8(vld1q_u8(a.b), vdupq_n_u8(0))), vld1q_u8(k.b))); return o; }
static inline blk dec(blk a, blk k) { blk o; vst1q_u8(o.b, veorq_u8(vaesimcq_u8(vaesdq_u8(vld1q_u8(a.b), vdupq_n_u8(0))), vld1q_u8(k.b))); return o; }
#else
#define IMPL "portable"
#define enc sw_enc
#define dec sw_dec
#endif

/* ---- aHash AES path (SMHasher3 rust-ahash.cpp, little-endian host) ---- */
static const uint64_t PI2[4] = { 0x452821e638d01377ULL, 0xbe5466cf34e90c6cULL, 0xc0ac29b7c97c50ddULL, 0x3f84d5b5b5470917ULL };
static const uint8_t SHUF[16] = { 4,11,9,6,8,13,15,5,14,3,1,12,0,7,10,2 };   /* SHUFFLE_MASK, out[i] = in[SHUF[i]] */
typedef struct { uint64_t w[2]; } u128;
static inline blk U2B(u128 v) { blk o; memcpy(o.b, v.w, 16); return o; }
static inline u128 B2U(blk b) { u128 v; memcpy(v.w, b.b, 16); return v; }
static inline u128 shuf_add(u128 a, u128 b) { blk x = U2B(a), y; for (int i = 0; i < 16; i++) y.b[i] = x.b[SHUF[i]]; u128 r = B2U(y); r.w[0] += b.w[0]; r.w[1] += b.w[1]; return r; }
static inline u128 L128(const uint8_t *p) { u128 v; memcpy(v.w, p, 16); return v; }
typedef struct { u128 enc, sum, key; } AH;
static inline void hash_in(AH *s, u128 v) { s->enc = B2U(dec(U2B(s->enc), U2B(v))); s->sum = shuf_add(s->sum, v); }
static uint64_t ahash(const uint64_t rs[4], const uint8_t *data, size_t len) {
    AH s; s.enc.w[0] = rs[0]; s.enc.w[1] = rs[1]; s.sum.w[0] = rs[2]; s.sum.w[1] = rs[3]; s.key.w[0] = rs[0] ^ rs[2]; s.key.w[1] = rs[1] ^ rs[3];
    u128 l = { { (uint64_t)len, 0 } }; hash_in(&s, l); s.enc.w[0] += len;
    if (len <= 8) { u128 v = { { 0, 0 } };
        if (len >= 4) { uint32_t a, b; memcpy(&a, data, 4); memcpy(&b, data + len - 4, 4); v.w[0] = a; v.w[1] = b; }
        else if (len >= 2) { uint16_t a; memcpy(&a, data, 2); v.w[0] = a; v.w[1] = data[len - 1]; }
        else if (len == 1) { v.w[0] = v.w[1] = data[0]; }
        hash_in(&s, v);
    } else if (len > 64) {
        blk key = U2B(s.key); blk cur[4] = { key, key, key, key };
        u128 sum0 = s.key, sum1 = { { ~s.key.w[0], ~s.key.w[1] } }; const uint8_t *t = data + len - 64; u128 tv;
        tv = L128(t);      cur[0] = enc(cur[0], U2B(tv)); sum0.w[0] += tv.w[0]; sum0.w[1] += tv.w[1];
        tv = L128(t + 16); cur[1] = dec(cur[1], U2B(tv)); sum1.w[0] += tv.w[0]; sum1.w[1] += tv.w[1];
        tv = L128(t + 32); cur[2] = enc(cur[2], U2B(tv)); sum0 = shuf_add(sum0, tv);
        tv = L128(t + 48); cur[3] = dec(cur[3], U2B(tv)); sum1 = shuf_add(sum1, tv);
        const uint8_t *p = data; size_t r = len;
        while (r > 64) {
            u128 b0 = L128(p), b1 = L128(p + 16), b2 = L128(p + 32), b3 = L128(p + 48);
            cur[0] = dec(cur[0], U2B(b0)); sum0 = shuf_add(sum0, b0);
            cur[1] = dec(cur[1], U2B(b1)); sum1 = shuf_add(sum1, b1);
            cur[2] = dec(cur[2], U2B(b2)); sum0 = shuf_add(sum0, b2);
            cur[3] = dec(cur[3], U2B(b3)); sum1 = shuf_add(sum1, b3);
            p += 64; r -= 64;
        }
        for (int j = 0; j < 4; j++) hash_in(&s, B2U(cur[j]));
        hash_in(&s, sum0); hash_in(&s, sum1);
    } else if (len > 32) {
        hash_in(&s, L128(data)); hash_in(&s, L128(data + 16)); hash_in(&s, L128(data + len - 32)); hash_in(&s, L128(data + len - 16));
    } else if (len > 16) {
        hash_in(&s, L128(data)); hash_in(&s, L128(data + len - 16));
    } else {
        u128 v; memcpy(&v.w[0], data, 8); memcpy(&v.w[1], data + len - 8, 8); hash_in(&s, v);
    }
    blk comb = enc(U2B(s.sum), U2B(s.enc)); blk prev = comb; comb = dec(comb, U2B(s.key)); comb = dec(comb, prev);
    uint64_t o; memcpy(&o, comb.b, 8); return o;
}

/* long-path internal state (cur[0..3], sum0, sum1) before the six final hash_in calls; len > 64 only */
typedef struct { blk cur[4]; u128 sum0, sum1; } LS;
static LS long_state(const uint64_t rs[4], const uint8_t *data, size_t len) {
    LS o; u128 key = { { rs[0] ^ rs[2], rs[1] ^ rs[3] } }; blk kb = U2B(key); for (int j = 0; j < 4; j++) o.cur[j] = kb;
    o.sum0 = key; o.sum1.w[0] = ~key.w[0]; o.sum1.w[1] = ~key.w[1]; const uint8_t *t = data + len - 64; u128 tv;
    tv = L128(t);      o.cur[0] = enc(o.cur[0], U2B(tv)); o.sum0.w[0] += tv.w[0]; o.sum0.w[1] += tv.w[1];
    tv = L128(t + 16); o.cur[1] = dec(o.cur[1], U2B(tv)); o.sum1.w[0] += tv.w[0]; o.sum1.w[1] += tv.w[1];
    tv = L128(t + 32); o.cur[2] = enc(o.cur[2], U2B(tv)); o.sum0 = shuf_add(o.sum0, tv);
    tv = L128(t + 48); o.cur[3] = dec(o.cur[3], U2B(tv)); o.sum1 = shuf_add(o.sum1, tv);
    const uint8_t *p = data; size_t r = len;
    while (r > 64) { u128 b0 = L128(p), b1 = L128(p + 16), b2 = L128(p + 32), b3 = L128(p + 48);
        o.cur[0] = dec(o.cur[0], U2B(b0)); o.sum0 = shuf_add(o.sum0, b0); o.cur[1] = dec(o.cur[1], U2B(b1)); o.sum1 = shuf_add(o.sum1, b1);
        o.cur[2] = dec(o.cur[2], U2B(b2)); o.sum0 = shuf_add(o.sum0, b2); o.cur[3] = dec(o.cur[3], U2B(b3)); o.sum1 = shuf_add(o.sum1, b3);
        p += 64; r -= 64; }
    return o;
}
static int eqA(const LS *a, const LS *b) { return !memcmp(a->cur[0].b, b->cur[0].b, 16) && !memcmp(a->cur[2].b, b->cur[2].b, 16) && !memcmp(&a->sum0, &b->sum0, 16); }
static int eqB(const LS *a, const LS *b) { return !memcmp(a->cur[1].b, b->cur[1].b, 16) && !memcmp(a->cur[3].b, b->cur[3].b, 16) && !memcmp(&a->sum1, &b->sum1, 16); }
static uint32_t smh_verify(void) {
    uint8_t key[256], hashes[256 * 8], fin[8];
    for (int i = 0; i < 256; i++) { key[i] = (uint8_t)i; uint64_t rs[4]; for (int j = 0; j < 4; j++) rs[j] = PI2[j] ^ (uint64_t)(256 - i);
        uint64_t h = ahash(rs, key, (size_t)i); memcpy(hashes + 8 * i, &h, 8); }
    uint64_t rs[4]; for (int j = 0; j < 4; j++) rs[j] = PI2[j]; uint64_t h = ahash(rs, hashes, sizeof hashes); memcpy(fin, &h, 8);
    return (uint32_t)fin[0] | (uint32_t)fin[1] << 8 | (uint32_t)fin[2] << 16 | (uint32_t)fin[3] << 24;
}

/* ---- RNG ---- */
typedef struct { uint64_t s[4]; } rng;
static uint64_t smx(uint64_t *x) { uint64_t z = (*x += 0x9e3779b97f4a7c15ULL); z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL; z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL; return z ^ (z >> 31); }
static uint64_t rl(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
static uint64_t nx(rng *r) { uint64_t *S = r->s, o = rl(S[1] * 5, 7) * 9, t = S[1] << 17; S[2] ^= S[0]; S[3] ^= S[1]; S[1] ^= S[2]; S[0] ^= S[3]; S[2] ^= t; S[3] = rl(S[3], 45); return o; }
static void rinit(rng *r, uint64_t seed) { for (int i = 0; i < 4; i++) r->s[i] = smx(&seed); }

/* ---- messages ---- */
static uint8_t E1[424], E2[424];
static void build_E(void) {   /* published pair E */
    memset(E1, 0, 424); memset(E2, 0, 424);
    const uint8_t a1[4] = {0x7c,0x06,0x7e,0x7b}, a2[4] = {0x81,0x08,0x81,0x80}, c1[4] = {0x80,0x08,0x82,0x80}, c2[4] = {0x7d,0x06,0x7d,0x7b};
    memcpy(E1 + 24, a1, 4); memcpy(E2 + 24, a2, 4); E1[280] = 0x08; E2[280] = 0x03; memcpy(E1 + 344, c1, 4); memcpy(E2 + 344, c2, 4); E1[384] = 0x03; E2[384] = 0x08;
}
/* 448-byte message with four echo slots (lane j = 0..3 at offset 16j); choice bits select the M or M' bytes of E's pattern */
#define TL 448
static void build_tie(uint8_t *m, unsigned choice) {
    memset(m, 0, TL);
    const uint8_t col0[2][4] = { {0x7c,0x06,0x7e,0x7b}, {0x81,0x08,0x81,0x80} }, col5[2][4] = { {0x80,0x08,0x82,0x80}, {0x7d,0x06,0x7d,0x7b} };
    const uint8_t tb[2] = { 0x03, 0x08 }, b4[2] = { 0x08, 0x03 };
    for (int j = 0; j < 4; j++) { int c = (choice >> j) & 1, o = 16 * j;
        m[384 + o + 8] = tb[c]; memcpy(m + o + 8, col0[c], 4); m[256 + o + 8] = b4[c]; memcpy(m + 320 + o + 8, col5[c], 4); }
}

static uint8_t (*TIE)[TL];
static uint8_t SH[16][576];

/* general slot builder: echo pattern of pair E in lane `lane` starting at iteration `sh` (sh = 0 uses the tail as the
   first injection; sh >= 1 injects in loop block sh-1).  Message length len (multiple of 64, > 64). */
static void put_slot(uint8_t *m, size_t len, int lane, int sh, int c) {
    const uint8_t col0[2][4] = { {0x7c,0x06,0x7e,0x7b}, {0x81,0x08,0x81,0x80} }, col5[2][4] = { {0x80,0x08,0x82,0x80}, {0x7d,0x06,0x7d,0x7b} };
    const uint8_t tb[2] = { 0x03, 0x08 }, b4[2] = { 0x08, 0x03 }; int o = 16 * lane + 8;
    size_t inj = sh == 0 ? len - 64 + (size_t)o : (size_t)(64 * (sh - 1) + o);
    m[inj] = tb[c]; memcpy(m + 64 * sh + o, col0[c], 4); m[64 * (sh + 4) + o] = b4[c]; memcpy(m + 64 * (sh + 5) + o, col5[c], 4);
}
typedef struct { int mode; uint64_t n, seed; uint64_t c[64]; uint64_t ex16[4]; int have16; } job;
static void *worker(void *arg) {
    job *J = arg; rng R; rinit(&R, J->seed);
    if (J->mode == 0) {
        for (uint64_t i = 0; i < J->n; i++) { uint64_t rs[4] = { nx(&R), nx(&R), nx(&R), nx(&R) };
            if (ahash(rs, E1, 424) == ahash(rs, E2, 424)) J->c[0]++; }
    } else if (J->mode == 2) {
        /* 576-byte messages; slot a = (lane 1, shift 0), slot b = (lane 1, shift 2), slot c = (lane 3, shift 2) */
        /* slots: bit0 (lane1,sh0), bit1 (lane3,sh0), bit2 (lane1,sh2), bit3 (lane3,sh2) */
        for (uint64_t i = 0; i < J->n; i++) {
            uint64_t rs[4] = { nx(&R), nx(&R), nx(&R), nx(&R) }; uint64_t h[16];
            h[0] = ahash(rs, SH[0], 576); h[4] = ahash(rs, SH[4], 576); h[8] = ahash(rs, SH[8], 576); h[12] = ahash(rs, SH[12], 576);
            h[1] = ahash(rs, SH[1], 576); h[2] = ahash(rs, SH[2], 576); h[3] = ahash(rs, SH[3], 576);
            J->c[0] += h[0] == h[4]; J->c[1] += h[0] == h[8];
            int t2 = h[0] == h[4] && h[0] == h[8] && h[0] == h[12]; J->c[2] += t2;
            int t0 = h[0] == h[1] && h[0] == h[2] && h[0] == h[3]; J->c[3] += t0;
            if (t0 && t2) { int all = 1; for (int c = 1; c < 16; c++) all &= ahash(rs, SH[c], 576) == h[0]; J->c[4] += all; }
        }
    } else {
        uint8_t (*M)[TL] = TIE; const unsigned CA[4] = { 0, 1, 4, 5 }, CB[4] = { 0, 2, 8, 10 };
        for (uint64_t i = 0; i < J->n; i++) {
            uint64_t rs[4] = { nx(&R), nx(&R), nx(&R), nx(&R) }; LS A[4], B[4];
            for (int c = 0; c < 4; c++) { A[c] = long_state(rs, M[CA[c]], TL); B[c] = c ? long_state(rs, M[CB[c]], TL) : A[0]; }
            /* single-slot pairs (full-state equality of that slot's group) */
            int p0 = eqA(&A[0], &A[1]), p2 = eqA(&A[0], &A[2]), p1 = eqB(&B[0], &B[1]), p3 = eqB(&B[0], &B[2]);
            J->c[0] += p0; J->c[1] += p1; J->c[2] += p2; J->c[3] += p3;
            J->c[4] += p0 && p2; J->c[5] += p1 && p3; J->c[6] += p0 && p1;
            int a = 1, b = 1, na = 0, nb = 0; for (int c = 1; c < 4; c++) { a &= eqA(&A[0], &A[c]); b &= eqB(&B[0], &B[c]); }
            for (int c = 0; c < 4; c++) { na += eqA(&A[0], &A[c]); nb += eqB(&B[0], &B[c]); }
            J->c[7] += a; J->c[8] += b; J->c[10 + na * nb]++;
            if (a && b) { uint64_t h0 = ahash(rs, M[0], TL); int all = 1; for (int c = 1; c < 16; c++) all &= ahash(rs, M[c], TL) == h0;
                J->c[9] += all; if (all && !J->have16) { J->have16 = 1; memcpy(J->ex16, rs, 32); } }
        }
    }
    return 0;
}
static void ci(double k, double N, double *lo, double *hi) { *lo = k > 0 ? k * pow(1 - 1/(9*k) - 1.96/(3*sqrt(k)), 3) / N : 0; *hi = (k+1) * pow(1 - 1/(9*(k+1)) + 1.96/(3*sqrt(k+1)), 3) / N; }

int main(int argc, char **argv) {
    init_sbox();
    printf("aHash 0.8.12 AES path (SMHasher3 rust-ahash port, hash_one(&[u8]) sequence), AES = %s\n", IMPL);
    { rng R; rinit(&R, 7); int bad = 0; for (int i = 0; i < 4096; i++) { blk a, k; for (int j = 0; j < 16; j++) { a.b[j] = (uint8_t)nx(&R); k.b[j] = (uint8_t)nx(&R); }
        blk x = enc(a, k), y = sw_enc(a, k), u = dec(a, k), v = sw_dec(a, k); if (memcmp(x.b, y.b, 16) || memcmp(u.b, v.b, 16)) bad++; }
      printf("validation: hardware vs software AES rounds: %d mismatches / 4096\n", bad); if (bad) return 1; }
    uint32_t v = smh_verify(); printf("validation: SMHasher3 rust_ahash verification 0x%08X (expected 0x3BF4383B) %s\n", v, v == 0x3BF4383B ? "OK" : "FAIL");
    if (v != 0x3BF4383B) return 1;
    build_E();
    { uint64_t rs[4] = { 0x0e17425e994c2579ULL, 0x4e057e193aec29dcULL, 0x665cc79160d3c675ULL, 0x3d80ed4ec94cfe49ULL };
      uint64_t a = ahash(rs, E1, 424), b = ahash(rs, E2, 424);
      printf("pair E explicit internal key: %016llx / %016llx (published 93ecf09ded4decc5) %s\n", (unsigned long long)a, (unsigned long long)b,
             a == b && a == 0x93ecf09ded4decc5ULL ? "OK" : "FAIL");
      if (!(a == b && a == 0x93ecf09ded4decc5ULL)) return 1; }
    const char *mode = argc > 1 ? argv[1] : "pairE"; int lg = argc > 2 ? atoi(argv[2]) : 20, T = argc > 3 ? atoi(argv[3]) : 1;
    uint64_t seed = argc > 4 ? strtoull(argv[4], 0, 0) : 0xa4a5ULL; if (lg < 0 || lg > 40 || T < 1 || T > 64) return 2;
    uint64_t N = 1ULL << lg;
    int m = strcmp(mode, "tie") == 0 ? 1 : strcmp(mode, "shift") == 0 ? 2 : 0;
    if (m == 2) for (int c = 0; c < 16; c++) { memset(SH[c], 0, 576); put_slot(SH[c], 576, 1, 0, c & 1); put_slot(SH[c], 576, 3, 0, (c >> 1) & 1); put_slot(SH[c], 576, 1, 2, (c >> 2) & 1); put_slot(SH[c], 576, 3, 2, (c >> 3) & 1); }
    if (m == 1) { TIE = malloc(16 * TL); for (unsigned c = 0; c < 16; c++) build_tie(TIE[c], c);
}
    job *J = calloc((size_t)T, sizeof(job)); pthread_t *th = calloc((size_t)T, sizeof(pthread_t)); uint64_t ms = seed;
    for (int t = 0; t < T; t++) { J[t].mode = m; J[t].n = N / (uint64_t)T + ((uint64_t)t < N % (uint64_t)T); J[t].seed = smx(&ms); }
    for (int t = 0; t < T; t++) pthread_create(&th[t], 0, worker, &J[t]);
    uint64_t c[64] = {0}; uint64_t ex[4] = {0}; int have = 0;
    for (int t = 0; t < T; t++) { pthread_join(th[t], 0); for (int i = 0; i < 64; i++) c[i] += J[t].c[i]; if (J[t].have16 && !have) { have = 1; memcpy(ex, J[t].ex16, 32); } }
    double lo, hi;
    if (m == 2) { const char *nm2[5] = { "slot (lane1, shift2) pair", "slot (lane3, shift2) pair", "4-way tied lanes1,3 @ shift2", "4-way tied lanes1,3 @ shift0", "16-way (both tied pairs)" };
        printf("shift: 576-byte messages, N = 2^%d keys\n", lg);
        for (int i = 0; i < 5; i++) { ci((double)c[i], (double)N, &lo, &hi); printf("  %-30s %10llu  rate 2^%.3f [2^%.3f, 2^%.3f]\n", nm2[i], (unsigned long long)c[i], c[i] ? log2((double)c[i] / N) : -INFINITY, log2(lo), log2(hi)); }
    } else if (m == 0) { ci((double)c[0], (double)N, &lo, &hi);
        printf("pair E (424 B, L = 53): N = 2^%d keys, rng 0x%llx, %d threads: %llu collisions, eps = 2^%.3f [2^%.3f, 2^%.3f], score %.2f [%.2f, %.2f]\n",
               lg, (unsigned long long)seed, T, (unsigned long long)c[0], log2((double)c[0] / N), log2(lo), log2(hi),
               log2(53.0) - log2((double)c[0] / N), log2(53.0) - log2(hi), log2(53.0) - log2(lo));
    } else {
        const char *nm[10] = { "slot0 pair", "slot1 pair", "slot2 pair", "slot3 pair", "slot0&slot2 pairs", "slot1&slot3 pairs", "slot0&slot1 pairs",
                               "4-way {slots 0,2} (state)", "4-way {slots 1,3} (state)", "16-way, all 16 outputs equal" };
        printf("tie: 448-byte messages, 16 = 2^4 choices of four echo slots, N = 2^%d keys, rng 0x%llx, %d threads\n", lg, (unsigned long long)seed, T);
        for (int i = 0; i < 10; i++) { ci((double)c[i], (double)N, &lo, &hi);
            printf("  %-26s %12llu  rate 2^%.3f  [2^%.3f, 2^%.3f]\n", nm[i], (unsigned long long)c[i], c[i] ? log2((double)c[i] / N) : -INFINITY, log2(lo), log2(hi)); }
        printf("  class size of message 0 among the 16:"); for (int k = 1; k <= 16; k++) if (c[10 + k]) printf(" %d:%llu", k, (unsigned long long)c[10 + k]); printf("\n");
        if (have) { printf("  16-way explicit internal key %016llx %016llx %016llx %016llx:", (unsigned long long)ex[0], (unsigned long long)ex[1], (unsigned long long)ex[2], (unsigned long long)ex[3]);
            uint64_t h0 = ahash(ex, TIE[0], TL); int ok = 1; for (int c2 = 1; c2 < 16; c2++) ok &= ahash(ex, TIE[c2], TL) == h0;
            printf(" all 16 -> %016llx %s; with_seeds args = %016llx %016llx %016llx %016llx\n", (unsigned long long)h0, ok ? "OK" : "FAIL",
                   (unsigned long long)(ex[0] ^ PI2[0]), (unsigned long long)(ex[1] ^ PI2[1]), (unsigned long long)(ex[2] ^ PI2[2]), (unsigned long long)(ex[3] ^ PI2[3])); }
    }
    return 0;
}
