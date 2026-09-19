// Single-file re-implementation of t1ha2_atonce (64-bit output, 64-bit seed),
// little-endian, portable.  Mirrors smhasher3 hashes/t1ha.cpp t1ha2<MODE_LE_NATIVE,false>.
#pragma once
#include <stdint.h>
#include <string.h>
#include <stddef.h>

static const uint64_t P0 = UINT64_C(0xEC99BF0D8372CAAB);
static const uint64_t P1 = UINT64_C(0x82434FE90EDCEF39);
static const uint64_t P2 = UINT64_C(0xD4F06DB99D67BE4B);
static const uint64_t P3 = UINT64_C(0xBD9CACC22C6E9571);
static const uint64_t P4 = UINT64_C(0x9C06FAF4D023E3AB);
static const uint64_t P5 = UINT64_C(0xC060724A8424F345);
static const uint64_t P6 = UINT64_C(0xCB5AF53AE3AAAC31);

static inline uint64_t rotr64(uint64_t v, unsigned n) { return (v >> n) | (v << (64 - n)); }
static inline uint64_t rd64(const uint8_t *p) { uint64_t v; memcpy(&v, p, 8); return v; }
static inline uint64_t tail64(const uint8_t *p, size_t tail) {
    // tail in 1..8: little-endian zero-extended read of `tail` bytes
    uint64_t r = 0;
    for (size_t i = 0; i < (tail & 7 ? (tail & 7) : 8); i++) r |= (uint64_t)p[i] << (8 * i);
    return r;
}
static inline void mul128(uint64_t a, uint64_t b, uint64_t *lo, uint64_t *hi) {
    __uint128_t r = (__uint128_t)a * b; *lo = (uint64_t)r; *hi = (uint64_t)(r >> 64);
}
static inline uint64_t mux64(uint64_t v, uint64_t p) { uint64_t l, h; mul128(v, p, &l, &h); return l ^ h; }
static inline void mixup64(uint64_t *a, uint64_t *b, uint64_t v, uint64_t p) {
    uint64_t l, h; mul128(*b + v, p, &l, &h); *a ^= l; *b += h;
}
static inline uint64_t final64(uint64_t a, uint64_t b) {
    uint64_t x = (a + rotr64(b, 41)) * P0;
    uint64_t y = (rotr64(a, 23) + b) * P6;
    return mux64(x ^ y, P5);
}
typedef struct { uint64_t a, b, c, d; } st4;
static inline void t1ha2_update(st4 *s, const uint8_t *v) {
    uint64_t w0 = rd64(v), w1 = rd64(v + 8), w2 = rd64(v + 16), w3 = rd64(v + 24);
    uint64_t d02 = w0 + rotr64(w2 + s->d, 56);
    uint64_t c13 = w1 + rotr64(w3 + s->c, 19);
    s->d ^= s->b + rotr64(w1, 38);
    s->c ^= s->a + rotr64(w0, 57);
    s->b ^= P6 * (c13 + w2);
    s->a ^= P5 * (d02 + w3);
}
static inline uint64_t t1ha2_tail(st4 *s, const uint8_t *v, size_t len) {
    switch (len) {
    default: mixup64(&s->a, &s->b, rd64(v), P4); v += 8; /* fall through */
    case 24: case 23: case 22: case 21: case 20: case 19: case 18: case 17:
             mixup64(&s->b, &s->a, rd64(v), P3); v += 8; /* fall through */
    case 16: case 15: case 14: case 13: case 12: case 11: case 10: case 9:
             mixup64(&s->a, &s->b, rd64(v), P2); v += 8; /* fall through */
    case 8: case 7: case 6: case 5: case 4: case 3: case 2: case 1:
             mixup64(&s->b, &s->a, tail64(v, len), P1); /* fall through */
    case 0:  return final64(s->a, s->b);
    }
}
static inline uint64_t t1ha2_atonce(const void *data, size_t len, uint64_t seed) {
    const uint8_t *in = (const uint8_t *)data;
    st4 s; s.a = seed; s.b = (uint64_t)len;
    size_t length = len;
    if (length > 32) {
        s.c = rotr64((uint64_t)len, 23) + ~seed;
        s.d = ~(uint64_t)len + rotr64(seed, 19);
        const uint8_t *detent = in + length - 31;
        do { t1ha2_update(&s, in); in += 32; } while (in < detent);
        s.a ^= P6 * (s.c + rotr64(s.d, 23));
        s.b ^= P5 * (rotr64(s.c, 19) + s.d);
        length &= 31;
    }
    return t1ha2_tail(&s, in, length);
}
