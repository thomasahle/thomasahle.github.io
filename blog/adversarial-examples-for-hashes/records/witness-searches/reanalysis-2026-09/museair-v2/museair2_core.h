/* MuseAir v2 (crate museair 0.6.0, src/lib.rs sha256 123772c6...1d42b1c),
 * one-shot hash / hash128 and bfast variants, written in C11 from the Rust
 * source (MIT OR Apache-2.0, K--Aethiax).  Little-endian host.
 * museair2_selfcheck() reproduces the crate's own stability_v2 values
 * (hashverify::compute = SMHasher verification procedure). */
#ifndef MUSEAIR2_CORE_H
#define MUSEAIR2_CORE_H
#include <stdint.h>
#include <string.h>
#include <stddef.h>

static const uint64_t MC[13] = {
    0x5ae31e589c56e17aull, 0x96d7bb04e64f6da9ull, 0x7ab1006b26f9eb64ull, 0x21233394220b8457ull,
    0x047cb9557c9f3b43ull, 0xd24f2590c0bcee28ull, 0x33ea8f71bb6016d8ull, 0xb5d2697595d0a01full,
    0x9bb30a32f00e2b4full, 0x4acea09317a429d1ull, 0xc2b2435dfdd545c6ull, 0xfda811a785572a42ull,
    0xe5f50676bf67137bull };
#define M_MASK_A 0xAAAAAAAAAAAAAAAAull
#define M_MASK_B 0x5555555555555555ull
#define M_MASK_I 01555555555555555555555ull
#define M_MASK_J 01333333333333333333333ull
#define M_MASK_K 00666666666666666666666ull

static inline void m_wmul(uint64_t a, uint64_t b, uint64_t *lo, uint64_t *hi) {
    unsigned __int128 r = (unsigned __int128)a * b; *lo = (uint64_t)r; *hi = (uint64_t)(r >> 64);
}
static inline uint64_t m_r64(const uint8_t *p) { uint64_t v; memcpy(&v, p, 8); return v; }
static inline uint64_t m_r32(const uint8_t *p) { uint32_t v; memcpy(&v, p, 4); return v; }
static inline uint64_t m_rotl(uint64_t x, unsigned r) { r &= 63; return r ? (x << r) | (x >> (64 - r)) : x; }
static inline uint64_t m_rotr(uint64_t x, unsigned r) { r &= 63; return r ? (x >> r) | (x << (64 - r)) : x; }

static inline void m_read_short(const uint8_t *b, size_t len, uint64_t *i, uint64_t *j) {
    if (len >= 8)      { *i = m_r64(b); *j = m_r64(b + len - 8); }
    else if (len >= 4) { *i = m_r32(b); *j = m_r32(b + len - 4); }
    else if (len >= 1) { *i = ((uint64_t)b[0] << 48) | b[len - 1]; *j = b[len >> 1]; }
    else               { *i = 0; *j = 0; }
}
static uint64_t m_short64(const uint8_t *b, size_t len, uint64_t seed, int bfast) {
    uint64_t i, j, lo, hi, L = len;
    m_read_short(b, len < 16 ? len : 16, &i, &j);
    m_wmul(MC[2] ^ seed ^ L, MC[3] ^ L, &lo, &hi); i ^= lo; j ^= hi;
    if (len > 16) {
        uint64_t u, v, lo0, hi0, lo1, hi1;
        m_read_short(b + 16, len - 16, &u, &v);
        m_wmul(MC[4] ^ seed ^ u, MC[5], &lo0, &hi0);
        m_wmul(MC[6] ^ seed ^ v, MC[7], &lo1, &hi1);
        i ^= lo0 ^ hi1; j ^= lo1 ^ hi0;
    }
    if (!bfast) {
        m_wmul(i ^ MC[8], j ^ MC[9], &lo, &hi); i -= lo; j -= hi;
        m_wmul(i ^ MC[10], j ^ MC[11], &lo, &hi); i -= lo; j -= hi;
    } else {
        m_wmul(i ^ MC[8], j ^ MC[9], &i, &j);
        m_wmul(i ^ MC[10], j ^ MC[11], &i, &j);
    }
    return i ^ j;
}
static void m_short128(const uint8_t *b, size_t len, uint64_t sa, uint64_t sb, int bfast, uint64_t out[2]) {
    uint64_t i, j, L = len, lo0, hi0, lo1, hi1;
    m_read_short(b, len < 16 ? len : 16, &i, &j);
    m_wmul((MC[0] + sa) ^ L, MC[1] ^ L, &lo0, &hi0);
    m_wmul((MC[2] - sb) ^ L, MC[3] ^ L, &lo1, &hi1);
    i ^= lo0 ^ hi1; j ^= lo1 ^ hi0;
    if (len > 16) {
        uint64_t u, v;
        m_read_short(b + 16, len - 16, &u, &v);
        m_wmul((MC[4] + sa) ^ u, MC[5], &lo0, &hi0);
        m_wmul((MC[6] - sb) ^ v, MC[7], &lo1, &hi1);
        i ^= lo0 ^ hi1; j ^= lo1 ^ hi0;
    }
    if (!bfast) {
        m_wmul(i ^ MC[8], j ^ MC[9], &lo0, &hi0);
        m_wmul(i ^ MC[11], j ^ MC[10], &lo1, &hi1);
        m_wmul(lo0 ^ MC[10], hi0 ^ MC[11], &lo0, &hi0);
        m_wmul(lo1 ^ MC[9], hi1 ^ MC[8], &lo1, &hi1);
    } else {
        m_wmul(i ^ MC[8], j ^ MC[9], &lo0, &hi0);
        m_wmul(i, j, &lo1, &hi1);
        m_wmul(lo0 ^ MC[10], hi0 ^ MC[11], &lo0, &hi0);
        m_wmul(lo1, hi1, &lo1, &hi1);
    }
    out[0] = lo0 ^ hi1; out[1] = lo1 ^ hi0;
}
static void m_compress(const uint8_t *c, uint64_t *s, uint64_t *circ, int bfast) {
    uint64_t lo0, hi0, lo1, hi1, lo2, hi2, lo3, hi3, lo4, hi4, lo5, hi5;
    if (!bfast) {
        s[0] ^= m_r64(c);      s[1] ^= m_r64(c + 8);  m_wmul(s[0], s[1], &lo0, &hi0); s[0] -= lo0 ^ *circ;
        s[1] ^= m_r64(c + 16); s[2] ^= m_r64(c + 24); m_wmul(s[1], s[2], &lo1, &hi1); s[1] -= lo1 ^ hi0;
        s[2] ^= m_r64(c + 32); s[3] ^= m_r64(c + 40); m_wmul(s[2], s[3], &lo2, &hi2); s[2] -= lo2 ^ hi1;
        s[3] ^= m_r64(c + 48); s[4] ^= m_r64(c + 56); m_wmul(s[3], s[4], &lo3, &hi3); s[3] -= lo3 ^ hi2;
        s[4] ^= m_r64(c + 64); s[5] ^= m_r64(c + 72); m_wmul(s[4], s[5], &lo4, &hi4); s[4] -= lo4 ^ hi3;
        s[5] ^= m_r64(c + 80); s[0] ^= m_r64(c + 88); m_wmul(s[5], s[0], &lo5, &hi5); s[5] -= lo5 ^ hi4;
        *circ = hi5;
    } else {
        s[0] ^= m_r64(c);      s[1] ^= m_r64(c + 8);  m_wmul(s[0], s[1], &lo0, &hi0); s[0] = *circ ^ hi0;
        s[1] ^= m_r64(c + 16); s[2] ^= m_r64(c + 24); m_wmul(s[1], s[2], &lo1, &hi1); s[1] = lo0 ^ hi1;
        s[2] ^= m_r64(c + 32); s[3] ^= m_r64(c + 40); m_wmul(s[2], s[3], &lo2, &hi2); s[2] = lo1 ^ hi2;
        s[3] ^= m_r64(c + 48); s[4] ^= m_r64(c + 56); m_wmul(s[3], s[4], &lo3, &hi3); s[3] = lo2 ^ hi3;
        s[4] ^= m_r64(c + 64); s[5] ^= m_r64(c + 72); m_wmul(s[4], s[5], &lo4, &hi4); s[4] = lo3 ^ hi4;
        s[5] ^= m_r64(c + 80); s[0] ^= m_r64(c + 88); m_wmul(s[5], s[0], &lo5, &hi5); s[5] = lo4 ^ hi5;
        *circ = lo5;
    }
}
/* hash_loong_common + finalize; returns (i,j,k) */
static void m_loong(const uint8_t *b, size_t len, uint64_t *s, int bfast, uint64_t ijk[3]) {
    const uint8_t *rest = b; size_t rl = len; uint64_t circ = MC[6];
    if (rl > 96) {
        while (rl > 96) { m_compress(rest, s, &circ, bfast); rest += 96; rl -= 96; }
        s[0] ^= circ;
    }
    const uint8_t *t = b + len - 32;
    uint64_t lo0 = 0, lo1 = 0, lo2 = 0, lo3 = 0, lo4, lo5;
    uint64_t hi0 = s[1], hi1 = s[2], hi2 = s[3], hi3 = s[4], hi4, hi5;
    if (rl > 32) {
        s[0] ^= m_r64(rest); s[1] ^= m_r64(rest + 8); m_wmul(s[0], s[1], &lo0, &hi0);
        if (rl > 48) {
            s[1] ^= m_r64(rest + 16); s[2] ^= m_r64(rest + 24); m_wmul(s[1], s[2], &lo1, &hi1);
            if (rl > 64) {
                s[2] ^= m_r64(rest + 32); s[3] ^= m_r64(rest + 40); m_wmul(s[2], s[3], &lo2, &hi2);
                if (rl > 80) {
                    s[3] ^= m_r64(rest + 48); s[4] ^= m_r64(rest + 56); m_wmul(s[3], s[4], &lo3, &hi3);
                }
            }
        }
    }
    s[4] ^= m_r64(t);      s[5] ^= m_r64(t + 8);  m_wmul(s[4], s[5], &lo4, &hi4);
    s[5] ^= m_r64(t + 16); s[0] ^= m_r64(t + 24); m_wmul(s[5], s[0], &lo5, &hi5);
    uint64_t i = (s[0] - s[1]) ^ MC[7], j = (s[2] - s[3]) ^ MC[8], k = (s[4] - s[5]) ^ MC[9];
    unsigned rot = (unsigned)(len & 63);
    i = m_rotl(i, rot); j = m_rotr(j, rot); k -= (uint64_t)len;
    i -= lo3 ^ hi3; i -= lo4 ^ hi4;
    j -= lo5 ^ hi5; j -= lo0 ^ hi0;
    k -= lo1 ^ hi1; k -= lo2 ^ hi2;
    uint64_t l0, h0, l1, h1, l2, h2;
    m_wmul(i, j, &l0, &h0); m_wmul(j, k, &l1, &h1); m_wmul(k, i, &l2, &h2);
    if (!bfast) { i -= l0 ^ h2; j -= l1 ^ h0; k -= l2 ^ h1; }
    else        { i = l2 ^ h0;  j = l0 ^ h1;  k = l1 ^ h2; }
    ijk[0] = i; ijk[1] = j; ijk[2] = k;
}
static uint64_t museair2_64(const void *in, size_t len, uint64_t seed, int bfast) {
    const uint8_t *b = (const uint8_t *)in;
    if (len <= 32) return m_short64(b, len, seed, bfast);
    uint64_t s[6], ijk[3];
    for (int x = 0; x < 6; x++) s[x] = MC[x] ^ (seed & ((x & 1) ? M_MASK_B : M_MASK_A));
    m_loong(b, len, s, bfast, ijk);
    return ijk[0] + ijk[1] + ijk[2];
}
static void museair2_128(const void *in, size_t len, uint64_t sa, uint64_t sb, int bfast, uint64_t out[2]) {
    const uint8_t *b = (const uint8_t *)in;
    if (len <= 32) { m_short128(b, len, sa, sb, bfast, out); return; }
    uint64_t s[6] = { MC[0] ^ (sa & M_MASK_I), MC[1] ^ (sb & M_MASK_J), MC[2] ^ (sa & M_MASK_K),
                      MC[3] ^ (sb & M_MASK_I), MC[4] ^ (sa & M_MASK_J), MC[5] ^ (sb & M_MASK_K) }, ijk[3];
    m_loong(b, len, s, bfast, ijk);
    uint64_t lo3, hi3, lo4, hi4, lo5, hi5;
    m_wmul(ijk[0], MC[10], &lo3, &hi3); m_wmul(ijk[1], MC[11], &lo4, &hi4); m_wmul(ijk[2], MC[12], &lo5, &hi5);
    out[0] = lo3 ^ hi4 ^ lo5; out[1] = hi3 ^ lo4 ^ hi5;
}
/* SMHasher-style verification (crate hashverify::compute) */
static uint32_t museair2_verif(int which) {
    /* which: 0 hash, 1 hash_folded, 2 hash128, 3 hash128_folded, 4..7 the same for bfast */
    int bf = which >= 4, w = which & 3;
    size_t hb = (w == 0) ? 8 : (w == 1) ? 4 : (w == 2) ? 16 : 8;
    uint8_t key[256], hashes[256 * 16];
    for (int i = 0; i < 256; i++) {
        key[i] = (uint8_t)i;
        uint64_t seed = (uint64_t)(256 - i), o[2];
        if (w == 0) { o[0] = museair2_64(key, (size_t)i, seed, bf); memcpy(hashes + i * hb, o, 8); }
        else if (w == 1) { uint64_t h = museair2_64(key, (size_t)i, seed, bf); uint32_t f = (uint32_t)h ^ (uint32_t)(h >> 32); memcpy(hashes + i * hb, &f, 4); }
        else if (w == 2) { museair2_128(key, (size_t)i, seed, seed, bf, o); memcpy(hashes + i * hb, o, 16); }
        else { museair2_128(key, (size_t)i, seed, seed, bf, o); uint64_t f = o[0] + o[1]; memcpy(hashes + i * hb, &f, 8); }
    }
    uint8_t fin[16]; uint64_t o[2];
    size_t tot = 256 * hb;
    if (w == 0) { o[0] = museair2_64(hashes, tot, 0, bf); memcpy(fin, o, 8); }
    else if (w == 1) { uint64_t h = museair2_64(hashes, tot, 0, bf); uint32_t f = (uint32_t)h ^ (uint32_t)(h >> 32); memcpy(fin, &f, 4); }
    else if (w == 2) { museair2_128(hashes, tot, 0, 0, bf, o); memcpy(fin, o, 16); }
    else { museair2_128(hashes, tot, 0, 0, bf, o); uint64_t f = o[0] + o[1]; memcpy(fin, &f, 8); }
    return (uint32_t)fin[0] | (uint32_t)fin[1] << 8 | (uint32_t)fin[2] << 16 | (uint32_t)fin[3] << 24;
}
static int museair2_selfcheck(void) {
    static const uint32_t want[8] = { 0x7140CABC, 0x0B8F0243, 0x38028C88, 0xB9CD57B7,
                                      0xA4BFD093, 0xDCCDD53A, 0x81863E77, 0x9BAAAF63 };
    int ok = 1;
    for (int w = 0; w < 8; w++) if (museair2_verif(w) != want[w]) ok = 0;
    return ok;
}
#endif
