/* komihash 5.34 (Aleksey Vaneev, MIT), 64-bit one-shot komihash(Msg, Len,
 * UseSeed), written in C11 from komihash.h (tag 5.34; SMHasher3 ships the
 * identical 5.27 core).  Little-endian reads via explicit byte assembly.
 * komihash_verif() reproduces SMHasher3's verification value 0x8157FF6D. */
#ifndef KOMIHASH_CORE_H
#define KOMIHASH_CORE_H
#include <stdint.h>
#include <string.h>
#include <stddef.h>

static const uint64_t KI[9] = { 0,
    0x243F6A8885A308D3ull, 0x13198A2E03707344ull, 0xA4093822299F31D0ull, 0x082EFA98EC4E6C89ull,
    0x452821E638D01377ull, 0xBE5466CF34E90C6Cull, 0xC0AC29B7C97C50DDull, 0x3F84D5B5B5470917ull };
#define K_V01 0x5555555555555555ull
#define K_V10 0xAAAAAAAAAAAAAAAAull

static inline uint64_t k_lu32(const uint8_t *p) {
    return (uint64_t)p[0] | (uint64_t)p[1] << 8 | (uint64_t)p[2] << 16 | (uint64_t)p[3] << 24;
}
static inline uint64_t k_lu64(const uint8_t *p) { return k_lu32(p) | k_lu32(p + 4) << 32; }
/* kh_m128: *rl = lo(u*v); *rha += hi(u*v) */
static inline void k_m128(uint64_t u, uint64_t v, uint64_t *rl, uint64_t *rha) {
    unsigned __int128 r = (unsigned __int128)u * v;
    *rl = (uint64_t)r; *rha += (uint64_t)(r >> 64);
}
#define K_ROUND(S1, S5) do { k_m128(S1, S5, &S1, &S5); S1 ^= S5; } while (0)
#define K_H16(m, S1, S5) do { k_m128(k_lu64(m) ^ S1, k_lu64((m) + 8) ^ S5, &S1, &S5); S1 ^= S5; } while (0)

static inline uint64_t k_fin(uint64_t r1h, uint64_t r2h, uint64_t S1, uint64_t S5) {
    k_m128(r1h, r2h, &S1, &S5); S1 ^= S5;
    K_ROUND(S1, S5);
    return S1;
}
static uint64_t k_epi(const uint8_t *Msg, size_t MsgLen, uint64_t S1, uint64_t S5) {
    uint64_t r1h, r2h;
    if (MsgLen > 31) { K_H16(Msg, S1, S5); K_H16(Msg + 16, S1, S5); MsgLen -= 32; Msg += 32; }
    if (MsgLen > 15) { K_H16(Msg, S1, S5); MsgLen -= 16; Msg += 16; }
    size_t ml8 = MsgLen * 8;
    if (MsgLen < 8) {
        ml8 ^= 56;
        r1h = k_lu64(Msg + MsgLen - 8) >> 8 | (uint64_t)1 << 56;
        r2h = S5;
        r1h = (r1h >> ml8) ^ S1;
    } else {
        r2h = k_lu64(Msg + MsgLen - 8) >> 8 | (uint64_t)1 << 56;
        ml8 ^= 120;
        r1h = k_lu64(Msg) ^ S1;
        r2h = (r2h >> ml8) ^ S5;
    }
    return k_fin(r1h, r2h, S1, S5);
}
/* state after the 64-byte loop (MsgLen >= 64 on entry); advances Msg/MsgLen */
static void k_loop(const uint8_t **pMsg, size_t *pLen, uint64_t *pS1, uint64_t *pS5) {
    const uint8_t *Msg = *pMsg; size_t MsgLen = *pLen; uint64_t S1 = *pS1, S5 = *pS5;
    uint64_t S2 = KI[2] ^ S1, S3 = KI[3] ^ S1, S4 = KI[4] ^ S1;
    uint64_t S6 = KI[6] ^ S5, S7 = KI[7] ^ S5, S8 = KI[8] ^ S5;
    do {
        k_m128(k_lu64(Msg) ^ S1, k_lu64(Msg + 32) ^ S5, &S1, &S5);
        k_m128(k_lu64(Msg + 8) ^ S2, k_lu64(Msg + 40) ^ S6, &S2, &S6);
        k_m128(k_lu64(Msg + 16) ^ S3, k_lu64(Msg + 48) ^ S7, &S3, &S7);
        k_m128(k_lu64(Msg + 24) ^ S4, k_lu64(Msg + 56) ^ S8, &S4, &S8);
        Msg += 64; MsgLen -= 64;
        S4 ^= S7; S1 ^= S8; S2 ^= S5; S3 ^= S6;
    } while (MsgLen > 63);
    S5 ^= S6 ^ S7 ^ S8;
    S1 ^= S2 ^ S3 ^ S4;
    *pMsg = Msg; *pLen = MsgLen; *pS1 = S1; *pS5 = S5;
}
static void komihash_preseed(uint64_t UseSeed, uint64_t *pS1, uint64_t *pS5) {
    uint64_t S1 = KI[1] ^ (UseSeed & K_V01), S5 = KI[5] ^ (UseSeed & K_V10);
    K_ROUND(S1, S5);
    *pS1 = S1; *pS5 = S5;
}
static uint64_t komihash64(const void *Msg0, size_t MsgLen, uint64_t UseSeed) {
    const uint8_t *Msg = (const uint8_t *)Msg0;
    uint64_t S1, S5, r1h, r2h;
    komihash_preseed(UseSeed, &S1, &S5);
    if (MsgLen < 16) {
        r1h = S1; r2h = S5;
        if (MsgLen > 7) {
            r1h ^= k_lu64(Msg);
            size_t ml8 = MsgLen * 8;
            if (MsgLen < 12) {
                ml8 ^= 88;
                uint64_t m = (uint64_t)Msg[MsgLen - 3] | (uint64_t)Msg[MsgLen - 1] << 16 |
                             (uint64_t)1 << 24 | (uint64_t)Msg[MsgLen - 2] << 8;
                r2h ^= m >> ml8;
            } else {
                size_t mhs = 128 - ml8;
                uint64_t mh = (k_lu32(Msg + MsgLen - 4) | (uint64_t)1 << 32) >> mhs;
                uint64_t ml = k_lu32(Msg + 8);
                r2h ^= mh << 32 | ml;
            }
        } else if (MsgLen != 0) {
            size_t ml8 = MsgLen * 8;
            if (MsgLen < 4) {
                r1h ^= (uint64_t)Msg[0];
                r1h ^= (uint64_t)1 << ml8;
                if (MsgLen != 1) {
                    r1h ^= (uint64_t)Msg[1] << 8;
                    if (MsgLen != 2) r1h ^= (uint64_t)Msg[2] << 16;
                }
            } else {
                size_t mhs = 64 - ml8;
                uint64_t mh = (k_lu32(Msg + MsgLen - 4) | (uint64_t)1 << 32) >> mhs;
                uint64_t ml = k_lu32(Msg);
                r1h ^= mh << 32 | ml;
            }
        }
        return k_fin(r1h, r2h, S1, S5);
    }
    if (MsgLen < 32) {
        K_H16(Msg, S1, S5);
        size_t ml8 = MsgLen * 8;
        if (MsgLen < 24) {
            ml8 ^= 184;
            r1h = k_lu64(Msg + MsgLen - 8) >> 8 | (uint64_t)1 << 56;
            r2h = S5;
            r1h = (r1h >> ml8) ^ S1;
        } else {
            r2h = k_lu64(Msg + MsgLen - 8) >> 8 | (uint64_t)1 << 56;
            ml8 ^= 248;
            r1h = k_lu64(Msg + 16) ^ S1;
            r2h = (r2h >> ml8) ^ S5;
        }
        return k_fin(r1h, r2h, S1, S5);
    }
    if (MsgLen > 63) k_loop(&Msg, &MsgLen, &S1, &S5);
    return k_epi(Msg, MsgLen, S1, S5);
}
static uint32_t komihash_verif(void) {
    uint8_t key[256], hashes[256 * 8];
    for (int i = 0; i < 256; i++) {
        key[i] = (uint8_t)i;
        uint64_t h = komihash64(key, (size_t)i, (uint64_t)(256 - i));
        memcpy(hashes + 8 * i, &h, 8);
    }
    uint64_t f = komihash64(hashes, sizeof hashes, 0);
    return (uint32_t)f;
}
#endif
