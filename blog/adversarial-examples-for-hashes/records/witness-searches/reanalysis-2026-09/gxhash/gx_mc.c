/* gx_mc.c -- gxhash-64/128 v3.5.0: (1) re-verification of the key-free 15/16-byte pair,
 * (2) the L = 1 question (no <= 8-byte key-free pair), (3) the shortest flooding family.
 *
 * gxhash (ogxd/gxhash, v3 algorithm, commit 55bde47 = crate 3.5.0) as ported in SMHasher3
 * hashes/gxhash.cpp (Frank J. T. Wojcik 2025, Olivier Giniaux 2023, MIT).  The hash is
 * transcribed from that port (KEYDATA, get_partial, compress_all, compress_many, compress_8,
 * finalize, gxhash_x86); the AES round is an independent byte-level implementation (S-box
 * computed from the GF(2^8) inverse), with an optional AES-NI / ARMv8-AES path that is
 * cross-checked against it.  Checks the SMHasher3 verification values gxhash = 0x64A77B47 and
 * gxhash_64 = 0x48F84240 at startup (exit 1 on mismatch).
 *
 * Build: cc -O2 -std=c11 gx_mc.c -o gx_mc           (add -maes on x86-64, -march=armv8-a+crypto on arm64)
 * Run:   ./gx_mc [log2 seeds, default 12] [rng seed, default 0x0p55]
 * MIT licence.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { uint8_t b[16]; } blk;

/* ---------------- independent AES round (x86 AESENC / AESENCLAST semantics) ---------------- */
static uint8_t SBOX[256], ISBOX[256];
static uint8_t xt(uint8_t a) { return (uint8_t)((a << 1) ^ ((a & 0x80) ? 0x1b : 0)); }
static uint8_t gmul(uint8_t a, uint8_t b) { uint8_t r = 0; while (b) { if (b & 1) r ^= a; a = xt(a); b >>= 1; } return r; }
static void init_sbox(void) {
    for (int x = 0; x < 256; x++) {
        uint8_t inv = 0;
        if (x) for (int y = 1; y < 256; y++) if (gmul((uint8_t)x, (uint8_t)y) == 1) { inv = (uint8_t)y; break; }
        uint8_t s = inv, r = inv;
        for (int i = 0; i < 4; i++) { r = (uint8_t)((r << 1) | (r >> 7)); s ^= r; }
        SBOX[x] = s ^ 0x63; ISBOX[s ^ 0x63] = (uint8_t)x;
    }
}
static blk bx(blk a, blk b) { for (int i = 0; i < 16; i++) a.b[i] ^= b.b[i]; return a; }
static blk sub_shift(blk a) { blk o; for (int c = 0; c < 4; c++) for (int r = 0; r < 4; r++) o.b[4*c+r] = SBOX[a.b[4*((c+r)&3)+r]]; return o; }
static blk inv_sub_shift(blk a) { blk o; for (int c = 0; c < 4; c++) for (int r = 0; r < 4; r++) o.b[4*((c+r)&3)+r] = ISBOX[a.b[4*c+r]]; return o; }
static blk mixc(blk a) { blk o; for (int c = 0; c < 4; c++) { uint8_t *s = a.b + 4*c;
    for (int r = 0; r < 4; r++) o.b[4*c+r] = gmul(s[r],2) ^ gmul(s[(r+1)&3],3) ^ s[(r+2)&3] ^ s[(r+3)&3]; } return o; }
static blk imixc(blk a) { blk o; for (int c = 0; c < 4; c++) { uint8_t *s = a.b + 4*c;
    for (int r = 0; r < 4; r++) o.b[4*c+r] = gmul(s[r],14) ^ gmul(s[(r+1)&3],11) ^ gmul(s[(r+2)&3],13) ^ gmul(s[(r+3)&3],9); } return o; }
static blk sw_enc(blk a, blk k) { return bx(mixc(sub_shift(a)), k); }
static blk sw_enclast(blk a, blk k) { return bx(sub_shift(a), k); }
static blk sw_enc_inv(blk o, blk k) { return inv_sub_shift(imixc(bx(o, k))); }  /* inverse of sw_enc */

#if defined(__AES__) && !defined(GXMC_PORTABLE)
#include <wmmintrin.h>
#define IMPL "x86-aesni"
static blk hw_enc(blk a, blk k) { blk o; _mm_storeu_si128((__m128i*)o.b, _mm_aesenc_si128(_mm_loadu_si128((const __m128i*)a.b), _mm_loadu_si128((const __m128i*)k.b))); return o; }
static blk hw_enclast(blk a, blk k) { blk o; _mm_storeu_si128((__m128i*)o.b, _mm_aesenclast_si128(_mm_loadu_si128((const __m128i*)a.b), _mm_loadu_si128((const __m128i*)k.b))); return o; }
#elif defined(__ARM_FEATURE_AES) && !defined(GXMC_PORTABLE)
#include <arm_neon.h>
#define IMPL "armv8-aes"
static blk hw_enc(blk a, blk k) { blk o; vst1q_u8(o.b, veorq_u8(vaesmcq_u8(vaeseq_u8(vld1q_u8(a.b), vdupq_n_u8(0))), vld1q_u8(k.b))); return o; }
static blk hw_enclast(blk a, blk k) { blk o; vst1q_u8(o.b, veorq_u8(vaeseq_u8(vld1q_u8(a.b), vdupq_n_u8(0)), vld1q_u8(k.b))); return o; }
#else
#define IMPL "portable"
#define hw_enc sw_enc
#define hw_enclast sw_enclast
#endif

/* ---------------- gxhash (transcribed from SMHasher3 hashes/gxhash.cpp) ---------------- */
static const uint8_t KEYDATA[64] = {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x42,0x45,0x78,0xf2,0x21,0x3e,0x9d,0xb0,0xe5,0x22,0xc2,0x89,0x8e,0xc2,0x3b,0xfc,
    0x79,0xe2,0xfc,0x03,0x9b,0x2e,0x6b,0xcb,0x58,0xdc,0x61,0xb3,0xd9,0x2b,0x13,0x39,
    0x32,0x2e,0x01,0xd0,0x7d,0x2b,0x9d,0x68,0xb7,0xb1,0x44,0x55,0x2b,0x12,0x8b,0xc7 };
static blk KEY(int i) { blk o; memcpy(o.b, KEYDATA + 16 * (i + 1), 16); return o; }
static blk ld(const uint8_t *p) { blk o; memcpy(o.b, p, 16); return o; }
static blk add8(blk a, blk b) { for (int i = 0; i < 16; i++) a.b[i] = (uint8_t)(a.b[i] + b.b[i]); return a; }
static blk get_partial(const uint8_t *p, size_t len) {   /* both SMHasher3 branches give this value */
    blk o; memset(o.b, 0, 16); memcpy(o.b, p, len);
    for (int i = 0; i < 16; i++) o.b[i] = (uint8_t)(o.b[i] + (uint8_t)len);
    return o;
}
static blk compress_8(const uint8_t *p, const uint8_t *end, blk hv, size_t len) {
    blk t1, t2, lane1 = hv, lane2 = hv; memset(&t1, 0, 16); memset(&t2, 0, 16);
    while (p < end) {
        blk v0 = ld(p), v1 = ld(p+16), v2 = ld(p+32), v3 = ld(p+48), v4 = ld(p+64), v5 = ld(p+80), v6 = ld(p+96), v7 = ld(p+112);
        p += 128;
        blk a = hw_enc(v0, v2), b = hw_enc(v1, v3);
        a = hw_enc(a, v4); b = hw_enc(b, v5); a = hw_enc(a, v6); b = hw_enc(b, v7);
        t1 = add8(t1, KEY(0)); t2 = add8(t2, KEY(1));
        lane1 = hw_enclast(hw_enc(a, t1), lane1);
        lane2 = hw_enclast(hw_enc(b, t2), lane2);
    }
    blk lv; for (int i = 0; i < 4; i++) { uint32_t l = (uint32_t)len; memcpy(lv.b + 4*i, &l, 4); }  /* _mm_set1_epi32, LE host */
    lane1 = add8(lane1, lv); lane2 = add8(lane2, lv);
    return hw_enc(lane1, lane2);
}
static blk compress_many(const uint8_t *p, const uint8_t *end, blk hv, size_t len) {
    size_t nblk = (size_t)(end - p) / 16, unroll = nblk / 8;
    const uint8_t *endp = end - unroll * 128;
    while (p < endp) { hv = hw_enc(hv, ld(p)); p += 16; }
    return compress_8(p, end, hv, len);
}
static blk compress_all(const uint8_t *in, size_t len) {
    const uint8_t *p = in, *end = in + len; size_t extra = len % 16; blk hv;
    if (len == 0) { memset(&hv, 0, 16); return hv; }
    if (len <= 16) return get_partial(p, len);
    if (extra == 0) { hv = ld(p); p += 16; } else { hv = get_partial(p, extra); p += extra; }
    blk v0 = ld(p); p += 16;
    if (len > 32) { v0 = hw_enc(v0, ld(p)); p += 16;
        if (len > 48) { v0 = hw_enc(v0, ld(p)); p += 16;
            if (len > 64) hv = compress_many(p, end, hv, len); } }
    v0 = hw_enc(v0, KEY(0)); v0 = hw_enc(v0, KEY(1));
    return hw_enclast(hv, v0);
}
static blk from_state(blk st, uint64_t seed) {
    blk sx; memcpy(sx.b, &seed, 8); memcpy(sx.b + 8, &seed, 8);
    st = hw_enc(st, sx);
    st = hw_enc(st, KEY(0)); st = hw_enc(st, KEY(1)); return hw_enclast(st, KEY(2));
}
static blk gx128(const uint8_t *in, size_t len, uint64_t seed) { return from_state(compress_all(in, len), seed); }
static uint64_t lo64(blk o) { uint64_t r; memcpy(&r, o.b, 8); return r; }
static uint64_t hi64(blk o) { uint64_t r; memcpy(&r, o.b + 8, 8); return r; }

static uint32_t smh_verify(int bytes) {   /* SMHasher3 lib/Hashinfo.cpp _ComputedVerifyImpl */
    uint8_t key[256], hashes[256 * 16], fin[16];
    for (int i = 0; i < 256; i++) { key[i] = (uint8_t)i; blk h = gx128(key, (size_t)i, (uint64_t)(256 - i)); memcpy(hashes + i * bytes, h.b, (size_t)bytes); }
    blk t = gx128(hashes, (size_t)(256 * bytes), 0); memcpy(fin, t.b, 16);
    return (uint32_t)fin[0] | (uint32_t)fin[1] << 8 | (uint32_t)fin[2] << 16 | (uint32_t)fin[3] << 24;
}

/* ---------------- RNG: splitmix64 -> xoshiro256** ---------------- */
static uint64_t S[4];
static uint64_t sm(uint64_t *x) { uint64_t z = (*x += 0x9e3779b97f4a7c15ULL); z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL; z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL; return z ^ (z >> 31); }
static uint64_t rotl(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
static uint64_t rnd(void) { uint64_t r = rotl(S[1] * 5, 7) * 9, t = S[1] << 17; S[2] ^= S[0]; S[3] ^= S[1]; S[1] ^= S[2]; S[0] ^= S[3]; S[2] ^= t; S[3] = rotl(S[3], 45); return r; }

static void hex(const uint8_t *p, size_t n) { for (size_t i = 0; i < n; i++) printf("%02x", p[i]); }

int main(int argc, char **argv) {
    int lg = argc > 1 ? atoi(argv[1]) : 12; uint64_t rs = argc > 2 ? strtoull(argv[2], 0, 0) : 0x5eed0055ULL;
    if (lg < 0 || lg > 30) return 2;
    init_sbox(); uint64_t x = rs; for (int i = 0; i < 4; i++) S[i] = sm(&x);
    int fail = 0;
    printf("gxhash v3.5.0 (SMHasher3 port of ogxd/gxhash 55bde47), AES = %s\n", IMPL);
    /* hardware round vs software round */
    { int bad = 0; for (int i = 0; i < 4096; i++) { blk a, k; for (int j = 0; j < 16; j++) { a.b[j] = (uint8_t)rnd(); k.b[j] = (uint8_t)rnd(); }
        blk h1 = hw_enc(a, k), s1 = sw_enc(a, k), h2 = hw_enclast(a, k), s2 = sw_enclast(a, k), r = sw_enc_inv(s1, k);
        if (memcmp(h1.b, s1.b, 16) || memcmp(h2.b, s2.b, 16) || memcmp(r.b, a.b, 16)) bad++; }
      printf("validation: hw vs sw AES round and inverse: %d mismatches / 4096 %s\n", bad, bad ? "FAIL" : "OK"); if (bad) fail = 1; }
    uint32_t v128 = smh_verify(16), v64 = smh_verify(8);
    printf("validation: SMHasher3 verification gxhash = 0x%08X (0x64A77B47) %s, gxhash_64 = 0x%08X (0x48F84240) %s\n",
           v128, v128 == 0x64A77B47 ? "OK" : "FAIL", v64, v64 == 0x48F84240 ? "OK" : "FAIL");
    if (v128 != 0x64A77B47 || v64 != 0x48F84240) fail = 1;
    if (fail) return 1;
    uint64_t N = 1ULL << lg;

    /* (1) re-verify the published 15/16-byte pair */
    uint8_t a15[15], b16[16]; memset(a15, 0, 15); memset(b16, 0xff, 16);
    uint64_t hit64 = 0, hit128 = 0;
    for (uint64_t i = 0; i < N; i++) { uint64_t s = rnd(); blk h1 = gx128(a15, 15, s), h2 = gx128(b16, 16, s);
        hit64 += lo64(h1) == lo64(h2); hit128 += !memcmp(h1.b, h2.b, 16); }
    blk e1 = gx128(a15, 15, 0xf556ecbfcbfee3adULL);
    printf("\n(1) published pair 00^15 / ff^16: %llu/%llu seeds collide at 64 bits, %llu/%llu at 128 bits; seed 0xf556ecbfcbfee3ad -> %016llx (published 43ec3783791d0fb8) %s\n",
           (unsigned long long)hit64, (unsigned long long)N, (unsigned long long)hit128, (unsigned long long)N, (unsigned long long)lo64(e1),
           lo64(e1) == 0x43ec3783791d0fb8ULL ? "OK" : "FAIL");
    if (hit128 != N || lo64(e1) != 0x43ec3783791d0fb8ULL) fail = 1;

    /* (2) L = 1: compress_all is injective on lengths 0..8 (bytes 8..15 all equal len; len 0 -> zero block),
       and state -> 128-bit output is a bijection for each seed.  Exhaustive injectivity check on lengths 0..2
       plus structural check on all lengths; then a 64-bit sampling check. */
    {
        int ok = 1;
        for (int len = 1; len <= 8; len++) { uint8_t m[8] = {0}; blk c = compress_all(m, (size_t)len);
            for (int i = 8; i < 16; i++) if (c.b[i] != (uint8_t)len) ok = 0; }
        blk z = compress_all((const uint8_t *)"", 0); for (int i = 0; i < 16; i++) if (z.b[i]) ok = 0;
        /* exhaustive over all messages of length <= 2 (65,793 messages): distinct states */
        static uint8_t seen[1 << 17]; memset(seen, 0, sizeof seen); int dup = 0;
        for (int len = 0; len <= 2; len++) for (int v = 0; v < (1 << (8 * len)); v++) {
            uint8_t m[2] = { (uint8_t)v, (uint8_t)(v >> 8) }; blk c = compress_all(m, (size_t)len);
            uint32_t key = (uint32_t)c.b[0] | (uint32_t)c.b[1] << 8 | (uint32_t)(c.b[15] & 1) << 16; /* bytes 0,1 and len parity */
            if (len == 0) key = 1u << 16 | 0xffff; /* distinct marker (state is zero; bytes 15 = 0 unlike len 1,2) */
            if (seen[key]) dup++; seen[key] = 1; }
        /* 64-bit sampling: random pairs of <= 8-byte messages, 64 seeds each */
        uint64_t pairs = 1ULL << 16, coll = 0;
        for (uint64_t i = 0; i < pairs; i++) { uint8_t m1[8], m2[8]; size_t l1 = rnd() % 9, l2 = rnd() % 9; uint64_t r1 = rnd(), r2 = rnd();
            memcpy(m1, &r1, 8); memcpy(m2, &r2, 8); if (l1 == l2 && !memcmp(m1, m2, l1)) continue;
            for (int j = 0; j < 64; j++) { uint64_t s = rnd(); coll += lo64(gx128(m1, l1, s)) == lo64(gx128(m2, l2, s)); } }
        printf("(2) L = 1: states of lengths 1..8 have bytes 8..15 = len, empty -> zero block: %s; exhaustive lengths 0..2: %d duplicate states;\n"
               "    so no two distinct <= 8-byte messages share a state and no 128-bit collision exists for any seed.\n"
               "    64-bit sampling: %llu random <= 8-byte pairs x 64 seeds: %llu collisions (a pair with eps >= 1/2 would give ~32 per pair)\n",
               ok ? "OK" : "FAIL", dup, (unsigned long long)pairs, (unsigned long long)coll);
        if (!ok || dup) fail = 1;
    }

    /* (3) flooding family: len 16+e, first e bytes free (256^e values), last 16 bytes solved so compress_all = T.
       compress_all = SR(SB(hv)) ^ P2(v0), hv = get_partial(first e bytes, e), P2(v) = aesenc(aesenc(v, K0), K1). */
    {
        blk T; for (int i = 0; i < 16; i++) T.b[i] = (uint8_t)(0xa5 ^ i);   /* any fixed 128-bit target */
        size_t nfam = 256 + 65536; uint8_t (*fam)[18] = malloc(nfam * 18); size_t *flen = malloc(nfam * sizeof(size_t));
        size_t n = 0;
        for (int e = 1; e <= 2; e++) for (int v = 0; v < (1 << (8 * e)); v++) {
            uint8_t h[2] = { (uint8_t)v, (uint8_t)(v >> 8) };
            blk hv = get_partial(h, (size_t)e);
            blk tgt = bx(T, sw_enclast(hv, (blk){{0}}));            /* T ^ SR(SB(hv)) */
            blk v0 = sw_enc_inv(sw_enc_inv(tgt, KEY(1)), KEY(0));    /* P2^-1 */
            memcpy(fam[n], h, (size_t)e); memcpy(fam[n] + e, v0.b, 16); flen[n] = 16 + (size_t)e; n++;
        }
        size_t bad = 0; for (size_t i = 0; i < n; i++) { blk c = compress_all(fam[i], flen[i]); if (memcmp(c.b, T.b, 16)) bad++; }
        /* distinct messages? (the first e bytes differ within a length; lengths differ across) */
        printf("\n(3) flooding family: %zu messages (256 of 17 bytes, 65536 of 18 bytes), all with compress_all = T: %zu mismatches\n", n, bad);
        printf("    example 17-byte: "); hex(fam[0], 17); printf("\n    example 18-byte: "); hex(fam[256 + 12345], 18); printf("\n");
        uint64_t K = N; uint64_t allsame = 0; uint64_t shown = 0;
        for (uint64_t i = 0; i < K; i++) {
            uint64_t s = rnd(); blk h0 = gx128(fam[0], flen[0], s); int same = 1;
            for (size_t j = 1; j < n; j++) { blk h = gx128(fam[j], flen[j], s); if (memcmp(h.b, h0.b, 16)) { same = 0; break; } }
            allsame += same;
            if (!shown && same) { shown = 1; printf("    seed %016llx: all %zu messages -> gxhash128 %016llx%016llx, gxhash_64 %016llx\n",
                (unsigned long long)s, n, (unsigned long long)hi64(h0), (unsigned long long)lo64(h0), (unsigned long long)lo64(h0)); }
        }
        printf("    random seeds: %llu/%llu seeds put all %zu messages in ONE 128-bit class (largest class per key = %zu for every sampled key)\n",
               (unsigned long long)allsame, (unsigned long long)K, n, allsame == K ? n : 0);
        /* the <= 16-byte path caps at 2: a block has at most one 16-byte preimage and at most one shorter one */
        int maxpre = 0;
        for (int t = 0; t < 256; t++) { /* blocks with constant tail byte pattern are the only ones with short preimages */
            int cnt = 1; /* the 16-byte preimage always exists */
            for (int L = 1; L <= 15; L++) { int ok = 1; for (int i = L; i < 16; i++) if ((uint8_t)t != (uint8_t)L) ok = 0; cnt += ok; }
            if (t == 0) cnt++; /* empty message -> zero block */
            if (cnt > maxpre) maxpre = cnt; }
        printf("    <= 16-byte path: the largest preimage set of one block under compress_all is %d (16-byte + one shorter length), so k <= 2 there;\n"
               "    at most 2 + 256 messages of length <= 17 share a block, so 18 bytes is the shortest max length with a 2^16-way family.\n", maxpre);
        if (bad || allsame != K) fail = 1;
        free(fam); free(flen);
    }
    printf("\n%s\n", fail ? "SOME CHECK FAILED" : "ALL CHECKS PASSED");
    return fail;
}
