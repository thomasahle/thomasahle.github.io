// Single-file re-implementation of a5hash v5.21 (64-bit and 128-bit), little-endian,
// transcribed from smhasher3 hashes/a5hash.cpp. Validated against the SMHasher3
// verification values (see verify.c).
#pragma once
#include <stdint.h>
#include <string.h>
#include <stddef.h>

#define A5_VAL10 UINT64_C(0xAAAAAAAAAAAAAAAA)
#define A5_VAL01 UINT64_C(0x5555555555555555)
#define A5_K1 UINT64_C(0x243F6A8885A308D3)
#define A5_K2 UINT64_C(0x452821E638D01377)
#define A5_K3 UINT64_C(0xA4093822299F31D0)
#define A5_K4 UINT64_C(0xC0AC29B7C97C50DD)
#define A5_K5 UINT64_C(0x082EFA98EC4E6C89)
#define A5_K6 UINT64_C(0x3F84D5B5B5470917)
#define A5_K7 UINT64_C(0x13198A2E03707344)
#define A5_K8 UINT64_C(0xBE5466CF34E90C6C)

static inline uint32_t a5_lu32(const uint8_t *p){ uint32_t v; memcpy(&v,p,4); return v; }
static inline uint64_t a5_lu64(const uint8_t *p){ uint64_t v; memcpy(&v,p,8); return v; }
static inline void a5_umul128(uint64_t u, uint64_t v, uint64_t *rl, uint64_t *rh){
    unsigned __int128 r = (unsigned __int128)u * v; *rl = (uint64_t)r; *rh = (uint64_t)(r >> 64);
}

// initial state (Seed1, Seed2) as a function of (len, seed)
static inline void a5_init(size_t MsgLen, uint64_t UseSeed, uint64_t *S1, uint64_t *S2){
    uint64_t Seed1 = A5_K1 ^ MsgLen, Seed2 = A5_K2 ^ MsgLen;
    a5_umul128(Seed2 ^ (UseSeed & A5_VAL10), Seed1 ^ (UseSeed & A5_VAL01), S1, S2);
}

static inline uint64_t a5hash64(const void *Msg0, size_t MsgLen, uint64_t UseSeed){
    const uint8_t *Msg = (const uint8_t*)Msg0;
    uint64_t val01 = A5_VAL01, val10 = A5_VAL10;
    uint64_t Seed1, Seed2;
    a5_init(MsgLen, UseSeed, &Seed1, &Seed2);
    if (MsgLen > 16){
        val01 ^= Seed1; val10 ^= Seed2;
        do {
            a5_umul128(((uint64_t)a5_lu32(Msg) << 32) ^ a5_lu32(Msg+4) ^ Seed1,
                       ((uint64_t)a5_lu32(Msg+8) << 32) ^ a5_lu32(Msg+12) ^ Seed2, &Seed1, &Seed2);
            MsgLen -= 16; Msg += 16;
            Seed1 += val01; Seed2 += val10;
        } while (MsgLen > 16);
    }
    if (MsgLen == 0) goto _fin;
    if (MsgLen > 3){
        const uint8_t *Msg4 = Msg + MsgLen - 4; size_t mo = MsgLen >> 3;
        Seed1 ^= (uint64_t)a5_lu32(Msg) << 32 | a5_lu32(Msg4);
        Seed2 ^= (uint64_t)a5_lu32(Msg + mo*4) << 32 | a5_lu32(Msg4 - mo*4);
    } else {
        Seed1 ^= Msg[0];
        if (MsgLen > 1){ Seed1 ^= (uint64_t)Msg[1] << 8; if (MsgLen > 2) Seed1 ^= (uint64_t)Msg[2] << 16; }
    }
_fin:
    a5_umul128(Seed1, Seed2, &Seed1, &Seed2);
    a5_umul128(Seed1 ^ val01, Seed2, &Seed1, &Seed2);
    return Seed1 ^ Seed2;
}

// 128-bit version. rh may be NULL (truncated variant).
static inline uint64_t a5hash128(const void *Msg0, size_t MsgLen, uint64_t UseSeed, uint64_t *rh){
    const uint8_t *Msg = (const uint8_t*)Msg0;
    uint64_t val01 = A5_VAL01, val10 = A5_VAL10;
    uint64_t Seed1 = A5_K1 ^ MsgLen, Seed2 = A5_K2 ^ MsgLen;
    uint64_t Seed3 = A5_K3, Seed4 = A5_K4;
    uint64_t a, b, c, d;
    a5_umul128(Seed2 ^ (UseSeed & val10), Seed1 ^ (UseSeed & val01), &Seed1, &Seed2);
    if (MsgLen < 17){
        if (MsgLen > 3){
            const uint8_t *Msg4 = Msg + MsgLen - 4; size_t mo = MsgLen >> 3;
            a = (uint64_t)a5_lu32(Msg) << 32 | a5_lu32(Msg4);
            b = (uint64_t)a5_lu32(Msg + mo*4) << 32 | a5_lu32(Msg4 - mo*4);
        } else {
            a = 0; b = 0;
            if (MsgLen){ a = Msg[0]; if (MsgLen > 1){ a |= (uint64_t)Msg[1] << 8; if (MsgLen > 2) a |= (uint64_t)Msg[2] << 16; } }
        }
        a5_umul128(a + Seed1, b + Seed2, &Seed1, &Seed2);
        a5_umul128(val01 ^ Seed1, Seed2, &a, &b);
        a ^= b;
        if (rh){ a5_umul128(Seed1 ^ Seed3, Seed2 ^ Seed4, &Seed3, &Seed4); *rh = Seed3 ^ Seed4; }
        return a;
    }
    if (MsgLen < 33){
        a = (uint64_t)a5_lu32(Msg) << 32 | a5_lu32(Msg+4);
        b = (uint64_t)a5_lu32(Msg+8) << 32 | a5_lu32(Msg+12);
        c = (uint64_t)a5_lu32(Msg+MsgLen-16) << 32 | a5_lu32(Msg+MsgLen-12);
        d = (uint64_t)a5_lu32(Msg+MsgLen-8) << 32 | a5_lu32(Msg+MsgLen-4);
        goto _fin_m;
    }
    val01 ^= Seed1; val10 ^= Seed2;
    if (MsgLen > 64){
        uint64_t Seed5 = A5_K5, Seed6 = A5_K6, Seed7 = A5_K7, Seed8 = A5_K8;
        do {
            const uint64_t s1 = Seed1, s3 = Seed3, s5 = Seed5;
            a5_umul128(a5_lu64(Msg) + Seed1, a5_lu64(Msg+32) + Seed2, &Seed1, &Seed2);
            Seed1 += val01; Seed2 += Seed8;
            a5_umul128(a5_lu64(Msg+8) + Seed3, a5_lu64(Msg+40) + Seed4, &Seed3, &Seed4);
            Seed3 += s1; Seed4 += val10;
            a5_umul128(a5_lu64(Msg+16) + Seed5, a5_lu64(Msg+48) + Seed6, &Seed5, &Seed6);
            a5_umul128(a5_lu64(Msg+24) + Seed7, a5_lu64(Msg+56) + Seed8, &Seed7, &Seed8);
            MsgLen -= 64; Msg += 64;
            Seed5 += s3; Seed6 += val10; Seed7 += s5; Seed8 += val10;
        } while (MsgLen > 64);
        Seed1 ^= Seed5; Seed2 ^= Seed6; Seed3 ^= Seed7; Seed4 ^= Seed8;
        if (MsgLen > 32) goto _tail32;
    } else {
        uint64_t s1;
_tail32:
        s1 = Seed1;
        a5_umul128(a5_lu64(Msg) + Seed1, a5_lu64(Msg+8) + Seed2, &Seed1, &Seed2);
        Seed1 += val01; Seed2 += Seed4;
        a5_umul128(a5_lu64(Msg+16) + Seed3, a5_lu64(Msg+24) + Seed4, &Seed3, &Seed4);
        MsgLen -= 32; Msg += 32;
        Seed3 += s1; Seed4 += val10;
    }
    a = a5_lu64(Msg + MsgLen - 16);
    b = a5_lu64(Msg + MsgLen - 8);
    if (MsgLen < 17) goto _fin;
    c = a5_lu64(Msg + MsgLen - 32);
    d = a5_lu64(Msg + MsgLen - 24);
_fin_m:
    a5_umul128(c + Seed3, d + Seed4, &Seed3, &Seed4);
_fin:
    Seed1 ^= Seed3; Seed2 ^= Seed4;
    a5_umul128(a + Seed1, b + Seed2, &Seed1, &Seed2);
    a5_umul128(val01 ^ Seed1, Seed2, &a, &b);
    a ^= b;
    if (rh){ a5_umul128(Seed1 ^ Seed3, Seed2 ^ Seed4, &Seed3, &Seed4); *rh = Seed3 ^ Seed4; }
    return a;
}
