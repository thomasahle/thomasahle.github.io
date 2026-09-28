/* t1ha2_atonce (64-bit) and t1ha2_atonce128, little-endian, written from
 * the SMHasher3 port hashes/t1ha.cpp (t1ha v2.1.4, Leonid Yuriev, zlib
 * license; SMHasher3 port by Frank J. T. Wojcik).  Only the portable
 * byte-wise reads are used (memcpy), which is what the unaligned path of
 * the upstream code computes on a little-endian machine.
 *
 * t1ha2_selfcheck() reproduces the SMHasher3 verification value of
 * t1ha2_64 (0x8F16C948) and t1ha2_128 (0xB44C43A1) with the
 * procedure of SMHasher3 lib/Hashinfo.cpp.
 */
#ifndef T1HA2_CORE_H
#define T1HA2_CORE_H
#include <stdint.h>
#include <string.h>
#include <stddef.h>

static const uint64_t T_P0 = UINT64_C(0xEC99BF0D8372CAAB);
static const uint64_t T_P1 = UINT64_C(0x82434FE90EDCEF39);
static const uint64_t T_P2 = UINT64_C(0xD4F06DB99D67BE4B);
static const uint64_t T_P3 = UINT64_C(0xBD9CACC22C6E9571);
static const uint64_t T_P4 = UINT64_C(0x9C06FAF4D023E3AB);
static const uint64_t T_P5 = UINT64_C(0xC060724A8424F345);
static const uint64_t T_P6 = UINT64_C(0xCB5AF53AE3AAAC31);

static inline uint64_t t_rotr(uint64_t v, unsigned s) { return (v >> s) | (v << (64 - s)); }
static inline uint64_t t_rd64(const uint8_t *p) { uint64_t v; memcpy(&v, p, 8); return v; }

static inline void t_mul128(uint64_t *lo, uint64_t *hi, uint64_t a, uint64_t b) {
    unsigned __int128 r = (unsigned __int128)a * b;
    *lo = (uint64_t)r; *hi = (uint64_t)(r >> 64);
}
static inline void t_mixup64(uint64_t *a, uint64_t *b, uint64_t v, uint64_t prime) {
    uint64_t l, h;
    t_mul128(&l, &h, *b + v, prime);
    *a ^= l;
    *b += h;
}
static inline uint64_t t_mux64(uint64_t v, uint64_t prime) {
    uint64_t l, h; t_mul128(&l, &h, v, prime); return l ^ h;
}
static inline uint64_t t_final64(uint64_t a, uint64_t b) {
    uint64_t x = (a + t_rotr(b, 41)) * T_P0;
    uint64_t y = (t_rotr(a, 23) + b) * T_P6;
    return t_mux64(x ^ y, T_P5);
}
static inline uint64_t t_final128(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t *h) {
    t_mixup64(&a, &b, t_rotr(c, 41) ^ d, T_P0);
    t_mixup64(&b, &c, t_rotr(d, 23) ^ a, T_P6);
    t_mixup64(&c, &d, t_rotr(a, 19) ^ b, T_P5);
    t_mixup64(&d, &a, t_rotr(b, 31) ^ c, T_P4);
    *h = c + d;
    return a ^ b;
}
/* little-endian zero-extended read of the last 1..8 bytes */
static inline uint64_t t_tail64(const uint8_t *p, size_t tail) {
    size_t n = tail & 7; if (n == 0) n = 8;
    uint64_t r = 0; for (size_t i = 0; i < n; i++) r |= (uint64_t)p[i] << (8 * i);
    return r;
}
typedef struct { uint64_t a, b, c, d; } t_state;

static inline void t_update(t_state *s, const uint8_t *v) {
    const uint64_t w0 = t_rd64(v), w1 = t_rd64(v + 8), w2 = t_rd64(v + 16), w3 = t_rd64(v + 24);
    const uint64_t d02 = w0 + t_rotr(w2 + s->d, 56);
    const uint64_t c13 = w1 + t_rotr(w3 + s->c, 19);
    s->d ^= s->b + t_rotr(w1, 38);
    s->c ^= s->a + t_rotr(w0, 57);
    s->b ^= T_P6 * (c13 + w2);
    s->a ^= T_P5 * (d02 + w3);
}
/* T1HA2_TAIL, use_ABCD = abcd */
static inline uint64_t t_tail(t_state *s, const uint8_t *v, size_t len, int abcd, uint64_t *xh) {
    switch (len) {
    default:
        if (abcd) t_mixup64(&s->a, &s->d, t_rd64(v), T_P4); else t_mixup64(&s->a, &s->b, t_rd64(v), T_P4);
        v += 8; /* fall through */
    case 24: case 23: case 22: case 21: case 20: case 19: case 18: case 17:
        t_mixup64(&s->b, &s->a, t_rd64(v), T_P3);
        v += 8; /* fall through */
    case 16: case 15: case 14: case 13: case 12: case 11: case 10: case 9:
        if (abcd) t_mixup64(&s->c, &s->b, t_rd64(v), T_P2); else t_mixup64(&s->a, &s->b, t_rd64(v), T_P2);
        v += 8; /* fall through */
    case 8: case 7: case 6: case 5: case 4: case 3: case 2: case 1: {
        uint64_t val = t_tail64(v, len);
        if (abcd) t_mixup64(&s->d, &s->c, val, T_P1); else t_mixup64(&s->b, &s->a, val, T_P1);
    }   /* fall through */
    case 0:
        if (abcd) return t_final128(s->a, s->b, s->c, s->d, xh);
        return t_final64(s->a, s->b);
    }
}
static uint64_t t1ha2_generic(const void *in, size_t len, uint64_t seed, int xwidth, uint64_t *xh) {
    const uint8_t *p = (const uint8_t *)in;
    t_state s; uint64_t length = (uint64_t)len;
    s.a = seed; s.b = length; s.c = 0; s.d = 0;
    if (length > 32) {
        s.c = t_rotr(length, 23) + ~seed;
        s.d = ~length + t_rotr(seed, 19);
        const uint8_t *detent = p + len - 31;
        do { const uint8_t *v = p; p += 32; t_update(&s, v); } while (p < detent);
        if (!xwidth) {
            s.a ^= T_P6 * (s.c + t_rotr(s.d, 23));
            s.b ^= T_P5 * (t_rotr(s.c, 19) + s.d);
        }
        length &= 31;
    } else if (xwidth) {
        s.c = t_rotr(length, 23) + ~seed;
        s.d = ~length + t_rotr(seed, 19);
    }
    return t_tail(&s, p, (size_t)length, xwidth, xh);
}
static inline uint64_t t1ha2_64(const void *in, size_t len, uint64_t seed) {
    return t1ha2_generic(in, len, seed, 0, NULL);
}
static inline uint64_t t1ha2_128(const void *in, size_t len, uint64_t seed, uint64_t *hi) {
    return t1ha2_generic(in, len, seed, 1, hi);
}

/* SMHasher3 lib/Hashinfo.cpp verification: key i = bytes 0..i-1 (values
 * 0..i-1), seed 256-i, outputs concatenated little-endian, hashed with seed 0,
 * first 4 bytes little-endian. */
static uint32_t t1ha2_verif(int wide) {
    uint8_t key[256], hashes[256 * 16];
    size_t hb = wide ? 16 : 8;
    for (int i = 0; i < 256; i++) {
        key[i] = (uint8_t)i;
        uint64_t hi = 0, lo = wide ? t1ha2_128(key, (size_t)i, (uint64_t)(256 - i), &hi)
                                   : t1ha2_64(key, (size_t)i, (uint64_t)(256 - i));
        memcpy(hashes + i * hb, &lo, 8);
        if (wide) memcpy(hashes + i * hb + 8, &hi, 8);
    }
    uint64_t hi = 0, fin = wide ? t1ha2_128(hashes, 256 * hb, 0, &hi) : t1ha2_64(hashes, 256 * hb, 0);
    uint8_t out[16]; memcpy(out, &fin, 8); memcpy(out + 8, &hi, 8);
    return (uint32_t)out[0] | (uint32_t)out[1] << 8 | (uint32_t)out[2] << 16 | (uint32_t)out[3] << 24;
}
#endif
