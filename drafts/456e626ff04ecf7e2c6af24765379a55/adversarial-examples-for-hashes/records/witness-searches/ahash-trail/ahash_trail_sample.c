/* ahash_trail_sample.c -- count collisions of fixed message pairs under aHash 0.8.12 (AES path,
 * SMHasher3 rust_ahash variant: write_usize(len) then write(bytes), keyed by RandomState {k0..k3})
 * over N uniformly random four-word keys (rs4 model).  AES-path implementation and SMHasher3
 * verification follow verify/rust-ahash/ahash_pairs.c (T. D. Ahle, MIT); constants PI2 and
 * SHUFFLE_MASK are aHash's (T. Kaitchuck, MIT OR Apache-2.0).
 * Build: cc -O2 -maes -o ahash_trail_sample ahash_trail_sample.c -lm
 * Run:   ./ahash_trail_sample pairs.txt log2N rngseed      (pairs.txt: "m1hex m2hex label" per line)
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <wmmintrin.h>
#include <tmmintrin.h>
typedef __m128i v128;
static const uint64_t PI2[4] = {0x452821e638d01377ULL, 0xbe5466cf34e90c6cULL, 0xc0ac29b7c97c50ddULL, 0x3f84d5b5b5470917ULL};
static const uint8_t SHUFFLE_MASK[16] = {0x4,0xb,0x9,0x6,0x8,0xd,0xf,0x5,0xe,0x3,0x1,0xc,0x0,0x7,0xa,0x2};
static inline v128 mk(uint64_t lo, uint64_t hi) { return _mm_set_epi64x((long long)hi, (long long)lo); }
static inline v128 ld(const uint8_t *p) { return _mm_loadu_si128((const v128 *)p); }
static inline v128 shuffle_add(v128 s, v128 v) { return _mm_add_epi64(_mm_shuffle_epi8(s, ld(SHUFFLE_MASK)), v); }
static inline uint64_t lo64(v128 x) { return (uint64_t)_mm_cvtsi128_si64(x); }
static uint32_t rd32(const uint8_t *p) { uint32_t x; memcpy(&x, p, 4); return x; }
static uint16_t rd16(const uint8_t *p) { uint16_t x; memcpy(&x, p, 2); return x; }
static uint64_t rd64(const uint8_t *p) { uint64_t x; memcpy(&x, p, 8); return x; }
static void read_small(const uint8_t *p, size_t n, uint64_t o[2]) {
    if (n >= 4) { o[0] = rd32(p); o[1] = rd32(p + n - 4); }
    else if (n >= 2) { o[0] = rd16(p); o[1] = p[n - 1]; }
    else o[0] = o[1] = n ? p[0] : 0;
}
static uint64_t ahash_aes(const uint8_t *d, size_t n, const uint64_t k[4]) {
    v128 enc = mk(k[0], k[1]), sum = mk(k[2], k[3]), key = _mm_xor_si128(enc, sum);
#define HIN(v) do { v128 _v = (v); enc = _mm_aesdec_si128(enc, _v); sum = shuffle_add(sum, _v); } while (0)
    HIN(mk((uint64_t)n, 0));
    enc = _mm_add_epi64(enc, mk((uint64_t)n, 0));
    if (n <= 8) { uint64_t v[2]; read_small(d, n, v); HIN(mk(v[0], v[1])); }
    else if (n <= 16) HIN(mk(rd64(d), rd64(d + n - 8)));
    else if (n <= 32) { HIN(ld(d)); HIN(ld(d + n - 16)); }
    else if (n <= 64) { HIN(ld(d)); HIN(ld(d + 16)); HIN(ld(d + n - 32)); HIN(ld(d + n - 16)); }
    else {
        v128 cur[4] = {key, key, key, key}, s[2] = {key, _mm_xor_si128(key, _mm_set1_epi32(-1))};
        const uint8_t *t = d + n - 64;
        cur[0] = _mm_aesenc_si128(cur[0], ld(t));      s[0] = _mm_add_epi64(s[0], ld(t));
        cur[1] = _mm_aesdec_si128(cur[1], ld(t + 16)); s[1] = _mm_add_epi64(s[1], ld(t + 16));
        cur[2] = _mm_aesenc_si128(cur[2], ld(t + 32)); s[0] = shuffle_add(s[0], ld(t + 32));
        cur[3] = _mm_aesdec_si128(cur[3], ld(t + 48)); s[1] = shuffle_add(s[1], ld(t + 48));
        for (size_t left = n; left > 64; left -= 64, d += 64)
            for (int i = 0; i < 4; i++) { v128 b = ld(d + 16 * i); cur[i] = _mm_aesdec_si128(cur[i], b); s[i & 1] = shuffle_add(s[i & 1], b); }
        for (int i = 0; i < 4; i++) HIN(cur[i]);
        HIN(s[0]); HIN(s[1]);
    }
    v128 c = _mm_aesenc_si128(sum, enc);
    return lo64(_mm_aesdec_si128(_mm_aesdec_si128(c, key), c));
#undef HIN
}
static uint32_t smhasher3_verify(void) {
    uint8_t key[256] = {0}, hashes[2048]; uint64_t k[4], h;
    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 4; j++) k[j] = PI2[j] ^ (uint64_t)(256 - i);
        h = ahash_aes(key, (size_t)i, k); memcpy(hashes + 8 * i, &h, 8); key[i] = (uint8_t)i;
    }
    for (int j = 0; j < 4; j++) k[j] = PI2[j];
    return (uint32_t)ahash_aes(hashes, sizeof hashes, k);
}
static uint64_t splitmix64(uint64_t *s) {
    uint64_t z = (*s += 0x9e3779b97f4a7c15ULL);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL; z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL; return z ^ (z >> 31);
}
static uint64_t rotl64(uint64_t x, unsigned r) { return (x << r) | (x >> (64 - r)); }
typedef struct { uint64_t s[4]; } xoshiro;
static void xo_seed(xoshiro *r, uint64_t seed) { for (int i = 0; i < 4; i++) r->s[i] = splitmix64(&seed); }
static uint64_t xo_next(xoshiro *r) {
    uint64_t *s = r->s, res = rotl64(s[1] * 5, 7) * 9, t = s[1] << 17;
    s[2] ^= s[0]; s[3] ^= s[1]; s[1] ^= s[2]; s[0] ^= s[3]; s[2] ^= t; s[3] = rotl64(s[3], 45); return res;
}
#define MAXP 64
static uint8_t M1[MAXP][256], M2[MAXP][256]; static size_t N1[MAXP], N2[MAXP]; static char LAB[MAXP][128];
static size_t unhex(const char *h, uint8_t *o) { size_t n = strlen(h) / 2; for (size_t i = 0; i < n; i++) { unsigned v; sscanf(h + 2 * i, "%2x", &v); o[i] = (uint8_t)v; } return n; }
int main(int argc, char **argv) {
    if (argc < 4) { fprintf(stderr, "usage: %s pairs.txt log2N rngseed\n", argv[0]); return 2; }
    uint32_t ver = smhasher3_verify();
    if (ver != 0x3BF4383B) { fprintf(stderr, "SMHasher3 verification 0x%08X != 0x3BF4383B; aborting\n", ver); return 1; }
    FILE *f = fopen(argv[1], "r"); if (!f) { perror("pairs"); return 2; }
    int np = 0; char a[600], b[600], lab[128];
    while (np < MAXP && fscanf(f, "%599s %599s %127s", a, b, lab) == 3) { N1[np] = unhex(a, M1[np]); N2[np] = unhex(b, M2[np]); strcpy(LAB[np], lab); np++; }
    fclose(f);
    int lg = atoi(argv[2]); uint64_t rngseed = strtoull(argv[3], NULL, 0);
    xoshiro r; xo_seed(&r, rngseed);
    uint64_t N = (uint64_t)1 << lg, cnt[MAXP] = {0}, k[4];
    for (uint64_t i = 0; i < N; i++) {
        for (int j = 0; j < 4; j++) k[j] = xo_next(&r);
        for (int p = 0; p < np; p++) if (ahash_aes(M1[p], N1[p], k) == ahash_aes(M2[p], N2[p], k)) cnt[p]++;
    }
    printf("# verification 0x%08X OK; model rs4; N = 2^%d; rngseed %llu\n", ver, lg, (unsigned long long)rngseed);
    for (int p = 0; p < np; p++) printf("%s %llu %d %.4f\n", LAB[p], (unsigned long long)cnt[p], lg, cnt[p] ? log2((double)cnt[p]) - lg : -INFINITY);
    return 0;
}
