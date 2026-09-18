/*
 * spookyhash2_64_pair.c -- approximately half-rate fixed collision pairs for
 * SpookyHash V2 with 64-bit output (SMHasher3's "SpookyHash2_64").
 *
 * Copyright (c) 2026 Thomas Dybdahl Ahle
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
 * SpookyHash V2 is by Bob Jenkins (public domain), https://www.burtleburtle.net/bob/hash/spooky.html
 * The implementation below was rewritten from the reference (SpookyV2.cpp, as carried in
 * SMHasher3 hashes/spookyhash.cpp) and is validated at startup against SMHasher3's
 * verification values.  No dependencies beyond the C11 standard library.
 *
 * Build:  cc -O2 -std=c11 -o spookyhash2_64_pair spookyhash2_64_pair.c -lm
 * Run:    ./spookyhash2_64_pair [log2 seeds = 24] [rng seed = 1] [seed mode = 0]
 *         seed mode 0: h1 = h2 = seed (SMHasher3's 64-bit seeding of the 128-bit interface)
 *         seed mode 1: independent uniform (h1, h2) (the native 128-bit seed)
 */
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------------------------------ */
/* SpookyHash V2 (Bob Jenkins, public domain), written from the reference.               */
/* ------------------------------------------------------------------------------------ */

#define SC_CONST 0xdeadbeefdeadbeefULL
#define SC_BLOCK 96   /* bytes per Mix block (12 words)          */
#define SC_SHORT 192  /* messages below this take the Short path */

static inline uint64_t rotl64(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
static inline uint64_t ld64(const uint8_t *p) {  /* little-endian loads, host-independent */
    uint64_t v = 0; for (int i = 7; i >= 0; i--) v = (v << 8) | p[i]; return v;
}
static inline uint32_t ld32(const uint8_t *p) {
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}
static inline void st64(uint8_t *p, uint64_t v) { for (int i = 0; i < 8; i++) p[i] = (uint8_t)(v >> (8 * i)); }

/* Mix: absorb one 96-byte block into the 12-word state s[0..11].  If `probe` is non-NULL it
 * receives bit 63 of s11 right after `s11 += w11`, before the rotate (the mechanism check). */
static void mix(const uint8_t *d, uint64_t *s, int *probe) {
    s[0]  += ld64(d +  0);  s[2]  ^= s[10];  s[11] ^= s[0];   s[0]  = rotl64(s[0],  11);  s[11] += s[1];
    s[1]  += ld64(d +  8);  s[3]  ^= s[11];  s[0]  ^= s[1];   s[1]  = rotl64(s[1],  32);  s[0]  += s[2];
    s[2]  += ld64(d + 16);  s[4]  ^= s[0];   s[1]  ^= s[2];   s[2]  = rotl64(s[2],  43);  s[1]  += s[3];
    s[3]  += ld64(d + 24);  s[5]  ^= s[1];   s[2]  ^= s[3];   s[3]  = rotl64(s[3],  31);  s[2]  += s[4];
    s[4]  += ld64(d + 32);  s[6]  ^= s[2];   s[3]  ^= s[4];   s[4]  = rotl64(s[4],  17);  s[3]  += s[5];
    s[5]  += ld64(d + 40);  s[7]  ^= s[3];   s[4]  ^= s[5];   s[5]  = rotl64(s[5],  28);  s[4]  += s[6];
    s[6]  += ld64(d + 48);  s[8]  ^= s[4];   s[5]  ^= s[6];   s[6]  = rotl64(s[6],  39);  s[5]  += s[7];
    s[7]  += ld64(d + 56);  s[9]  ^= s[5];   s[6]  ^= s[7];   s[7]  = rotl64(s[7],  57);  s[6]  += s[8];
    s[8]  += ld64(d + 64);  s[10] ^= s[6];   s[7]  ^= s[8];   s[8]  = rotl64(s[8],  55);  s[7]  += s[9];
    s[9]  += ld64(d + 72);  s[11] ^= s[7];   s[8]  ^= s[9];   s[9]  = rotl64(s[9],  54);  s[8]  += s[10];
    s[10] += ld64(d + 80);  s[0]  ^= s[8];   s[9]  ^= s[10];  s[10] = rotl64(s[10], 22);  s[9]  += s[11];
    s[11] += ld64(d + 88);  if (probe) *probe = (int)(s[11] >> 63);
                            s[1]  ^= s[9];   s[10] ^= s[11];  s[11] = rotl64(s[11], 46);  s[10] += s[0];
}

static void endpartial(uint64_t *h) {
    h[11] += h[1];   h[2]  ^= h[11];  h[1]  = rotl64(h[1],  44);
    h[0]  += h[2];   h[3]  ^= h[0];   h[2]  = rotl64(h[2],  15);
    h[1]  += h[3];   h[4]  ^= h[1];   h[3]  = rotl64(h[3],  34);
    h[2]  += h[4];   h[5]  ^= h[2];   h[4]  = rotl64(h[4],  21);
    h[3]  += h[5];   h[6]  ^= h[3];   h[5]  = rotl64(h[5],  38);
    h[4]  += h[6];   h[7]  ^= h[4];   h[6]  = rotl64(h[6],  33);
    h[5]  += h[7];   h[8]  ^= h[5];   h[7]  = rotl64(h[7],  10);
    h[6]  += h[8];   h[9]  ^= h[6];   h[8]  = rotl64(h[8],  13);
    h[7]  += h[9];   h[10] ^= h[7];   h[9]  = rotl64(h[9],  38);
    h[8]  += h[10];  h[11] ^= h[8];   h[10] = rotl64(h[10], 53);
    h[9]  += h[11];  h[0]  ^= h[9];   h[11] = rotl64(h[11], 42);
    h[10] += h[0];   h[1]  ^= h[10];  h[0]  = rotl64(h[0],  54);
}

/* End (V2): add the padded last block word-wise, then three EndPartial rounds. */
static void end_block(uint64_t *h, const uint8_t *d) {
    for (int i = 0; i < 12; i++) h[i] += ld64(d + 8 * i);
    endpartial(h); endpartial(h); endpartial(h);
}

static void shortmix(uint64_t *h) {
    h[2] = rotl64(h[2], 50);  h[2] += h[3];  h[0] ^= h[2];
    h[3] = rotl64(h[3], 52);  h[3] += h[0];  h[1] ^= h[3];
    h[0] = rotl64(h[0], 30);  h[0] += h[1];  h[2] ^= h[0];
    h[1] = rotl64(h[1], 41);  h[1] += h[2];  h[3] ^= h[1];
    h[2] = rotl64(h[2], 54);  h[2] += h[3];  h[0] ^= h[2];
    h[3] = rotl64(h[3], 48);  h[3] += h[0];  h[1] ^= h[3];
    h[0] = rotl64(h[0], 38);  h[0] += h[1];  h[2] ^= h[0];
    h[1] = rotl64(h[1], 37);  h[1] += h[2];  h[3] ^= h[1];
    h[2] = rotl64(h[2], 62);  h[2] += h[3];  h[0] ^= h[2];
    h[3] = rotl64(h[3], 34);  h[3] += h[0];  h[1] ^= h[3];
    h[0] = rotl64(h[0],  5);  h[0] += h[1];  h[2] ^= h[0];
    h[1] = rotl64(h[1], 36);  h[1] += h[2];  h[3] ^= h[1];
}

static void shortend(uint64_t *h) {
    h[3] ^= h[2];  h[2] = rotl64(h[2], 15);  h[3] += h[2];
    h[0] ^= h[3];  h[3] = rotl64(h[3], 52);  h[0] += h[3];
    h[1] ^= h[0];  h[0] = rotl64(h[0], 26);  h[1] += h[0];
    h[2] ^= h[1];  h[1] = rotl64(h[1], 51);  h[2] += h[1];
    h[3] ^= h[2];  h[2] = rotl64(h[2], 28);  h[3] += h[2];
    h[0] ^= h[3];  h[3] = rotl64(h[3],  9);  h[0] += h[3];
    h[1] ^= h[0];  h[0] = rotl64(h[0], 47);  h[1] += h[0];
    h[2] ^= h[1];  h[1] = rotl64(h[1], 54);  h[2] += h[1];
    h[3] ^= h[2];  h[2] = rotl64(h[2], 32);  h[3] += h[2];
    h[0] ^= h[3];  h[3] = rotl64(h[3], 25);  h[0] += h[3];
    h[1] ^= h[0];  h[0] = rotl64(h[0], 63);  h[1] += h[0];
}

/* Short path (messages under 192 bytes). */
static void spooky_short(const uint8_t *p, size_t len, uint64_t *h1, uint64_t *h2) {
    size_t rem = len % 32;
    uint64_t h[4] = { *h1, *h2, SC_CONST, SC_CONST };  /* a, b, c, d */
    if (len > 15) {
        const uint8_t *end = p + (len / 32) * 32;
        for (; p < end; p += 32) {
            h[2] += ld64(p); h[3] += ld64(p + 8); shortmix(h); h[0] += ld64(p + 16); h[1] += ld64(p + 24);
        }
        if (rem >= 16) { h[2] += ld64(p); h[3] += ld64(p + 8); shortmix(h); p += 16; rem -= 16; }
    }
    h[3] += ((uint64_t)len) << 56;
    switch (rem) {
    case 15: h[3] += ((uint64_t)p[14]) << 48; /* fallthrough */
    case 14: h[3] += ((uint64_t)p[13]) << 40; /* fallthrough */
    case 13: h[3] += ((uint64_t)p[12]) << 32; /* fallthrough */
    case 12: h[3] += ld32(p + 8); h[2] += ld64(p); break;
    case 11: h[3] += ((uint64_t)p[10]) << 16; /* fallthrough */
    case 10: h[3] += ((uint64_t)p[ 9]) <<  8; /* fallthrough */
    case  9: h[3] +=  (uint64_t)p[ 8];        /* fallthrough */
    case  8: h[2] += ld64(p); break;
    case  7: h[2] += ((uint64_t)p[ 6]) << 48; /* fallthrough */
    case  6: h[2] += ((uint64_t)p[ 5]) << 40; /* fallthrough */
    case  5: h[2] += ((uint64_t)p[ 4]) << 32; /* fallthrough */
    case  4: h[2] += ld32(p); break;
    case  3: h[2] += ((uint64_t)p[ 2]) << 16; /* fallthrough */
    case  2: h[2] += ((uint64_t)p[ 1]) <<  8; /* fallthrough */
    case  1: h[2] +=  (uint64_t)p[ 0]; break;
    case  0: h[2] += SC_CONST; h[3] += SC_CONST; break;
    }
    shortend(h);
    *h1 = h[0]; *h2 = h[1];
}

static void init_state(uint64_t *h, uint64_t h1, uint64_t h2) {
    h[0] = h[3] = h[6] = h[9]  = h1;
    h[1] = h[4] = h[7] = h[10] = h2;
    h[2] = h[5] = h[8] = h[11] = SC_CONST;
}

/* SpookyHash V2, 128-bit interface: *h1, *h2 are the seed on entry and the hash on exit. */
static void spooky2_128(const uint8_t *m, size_t len, uint64_t *h1, uint64_t *h2) {
    if (len < SC_SHORT) { spooky_short(m, len, h1, h2); return; }
    uint64_t h[12];
    init_state(h, *h1, *h2);
    const uint8_t *p = m, *end = m + (len / SC_BLOCK) * SC_BLOCK;
    for (; p < end; p += SC_BLOCK) mix(p, h, NULL);
    uint8_t buf[SC_BLOCK];
    size_t rem = len - (size_t)(p - m);
    memcpy(buf, p, rem);
    memset(buf + rem, 0, SC_BLOCK - rem - 1);
    buf[SC_BLOCK - 1] = (uint8_t)rem;
    end_block(h, buf);
    *h1 = h[0]; *h2 = h[1];
}

/* ------------------------------------------------------------------------------------ */
/* SMHasher3 verification (lib/Hashinfo.cpp _ComputedVerifyImpl): hash keys {}, {0},      */
/* {0,1}, ..., {0..254} with seed 256-i (SpookyHash2_*: h1 = h2 = seed), concatenate the */
/* little-endian outputs, hash that with seed 0, take its first 4 bytes little-endian.   */
/* ------------------------------------------------------------------------------------ */
static uint32_t smhasher3_verification(int hashbytes) {
    uint8_t key[256], hashes[256 * 16], out[16];
    memset(key, 0, sizeof key);
    for (int i = 0; i < 256; i++) {
        uint64_t h1 = (uint64_t)(256 - i), h2 = h1;
        spooky2_128(key, (size_t)i, &h1, &h2);
        st64(out, h1); st64(out + 8, h2);
        memcpy(hashes + i * hashbytes, out, (size_t)hashbytes);
        key[i] = (uint8_t)i;
    }
    uint64_t h1 = 0, h2 = 0;
    spooky2_128(hashes, (size_t)(256 * hashbytes), &h1, &h2);
    return (uint32_t)(h1 & 0xffffffffu);  /* output bytes 0..3, little-endian */
}

/* ------------------------------------------------------------------------------------ */
/* RNG: splitmix64 seeding xoshiro256** (our own, so the seed stream is reproducible).    */
/* ------------------------------------------------------------------------------------ */
static uint64_t xs[4];
static uint64_t splitmix64(uint64_t *x) {
    uint64_t z = (*x += 0x9e3779b97f4a7c15ULL);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    return z ^ (z >> 31);
}
static void rng_seed(uint64_t s) { for (int i = 0; i < 4; i++) xs[i] = splitmix64(&s); }
static uint64_t rng_next(void) {
    uint64_t r = rotl64(xs[1] * 5, 7) * 9, t = xs[1] << 17;
    xs[2] ^= xs[0]; xs[3] ^= xs[1]; xs[1] ^= xs[2]; xs[0] ^= xs[3]; xs[2] ^= t; xs[3] = rotl64(xs[3], 45);
    return r;
}

/* ------------------------------------------------------------------------------------ */
/* The published pairs: 286 bytes each = 2 Mix blocks + a 94-byte End block.  M2 = M with */
/* Mix-block-2 word 11 += 2^63 (byte 191 ^= 0x80) and End word 10 += 2^63 (byte 279 ^=  */
/* 0x80), which cancel exactly, plus End word 11 -= 2^45 (pair 1) or += 2^45 (pair 2),  */
/* which cancels the rotated s11 difference iff bit 63 of s11 before the step-11 rotate  */
/* is 0 (pair 1) or 1 (pair 2).  The two pairs therefore cover complementary halves of   */
/* the seed space, each with measured rate near 1/2; exact population balance is unproved.               */
/* ------------------------------------------------------------------------------------ */
static const char *PAIR_HEX[2][2] = {
    { "a0ed1dcaeb2e421c17cf8516645510b61c1e80eb014c077fdbb3b3ab867298f2ccfb12c325d96dd09c3de65c247a0257a6d22a9da89710cfb724db191c748806e9d6b0272f125818a2c061b9e406999b788261dcd2487f7064e1bf4d00670a3d09230d92b37de4141a5e5d12e205c37e42c13193ab064833476cb9e5a22bd74c85073313646062ca36b300b4a62fa6797864ca181b97739bf4dd0567f5ce9a24398e0ddb544ee4926c6c033f6397e4790f51641c3d6dc04d5dcfe95ffe3054262ff9e5d8a8f17cce6b3c565acbecc693b77d90b1564fd1ddc71df2c56f2fd52fd6d8c9fb3b84612e5f83397bb5ac932651ce382aa90592ebb81d61c29ac53c2bace0711020c947c1ce55f2cfd79f821e92ba90562ba05eeab1b8f0771534",
      "a0ed1dcaeb2e421c17cf8516645510b61c1e80eb014c077fdbb3b3ab867298f2ccfb12c325d96dd09c3de65c247a0257a6d22a9da89710cfb724db191c748806e9d6b0272f125818a2c061b9e406999b788261dcd2487f7064e1bf4d00670a3d09230d92b37de4141a5e5d12e205c37e42c13193ab064833476cb9e5a22bd74c85073313646062ca36b300b4a62fa6797864ca181b97739bf4dd0567f5ce9a24398e0ddb544ee4926c6c033f6397e4790f51641c3d6dc04d5dcfe95ffe3054a62ff9e5d8a8f17cce6b3c565acbecc693b77d90b1564fd1ddc71df2c56f2fd52fd6d8c9fb3b84612e5f83397bb5ac932651ce382aa90592ebb81d61c29ac53c2bace0711020c947c1ce55f2cfd79f821e92ba90562ba05e6ab1b8f0771514" },
    { "a0ed1dcaeb2e421c17cf8516645510b61c1e80eb014c077fdbb3b3ab867298f2ccfb12c325d96dd09c3de65c247a0257a6d22a9da89710cfb724db191c748806e9d6b0272f125818a2c061b9e406999b788261dcd2487f7064e1bf4d00670a3d09230d92b37de4141a5e5d12e205c37e42c13193ab064833476cb9e5a22bd74c85073313646062ca36b300b4a62fa6797864ca181b97739bf4dd0567f5ce9a24398e0ddb544ee4926c6c033f6397e4790f51641c3d6dc04d5dcfe95ffe3054262ff9e5d8a8f17cce6b3c565acbecc693b77d90b1564fd1ddc71df2c56f2fd52fd6d8c9fb3b84612e5f83397bb5ac932651ce382aa90592ebb81d61c29ac53c2bace0711020c947c1ce55f2cfd79f821e92ba90562ba05eeab1b8f0771514",
      "a0ed1dcaeb2e421c17cf8516645510b61c1e80eb014c077fdbb3b3ab867298f2ccfb12c325d96dd09c3de65c247a0257a6d22a9da89710cfb724db191c748806e9d6b0272f125818a2c061b9e406999b788261dcd2487f7064e1bf4d00670a3d09230d92b37de4141a5e5d12e205c37e42c13193ab064833476cb9e5a22bd74c85073313646062ca36b300b4a62fa6797864ca181b97739bf4dd0567f5ce9a24398e0ddb544ee4926c6c033f6397e4790f51641c3d6dc04d5dcfe95ffe3054a62ff9e5d8a8f17cce6b3c565acbecc693b77d90b1564fd1ddc71df2c56f2fd52fd6d8c9fb3b84612e5f83397bb5ac932651ce382aa90592ebb81d61c29ac53c2bace0711020c947c1ce55f2cfd79f821e92ba90562ba05e6ab1b8f0771534" },
};
static const char *PAIR_DESC[2] = {
    "pair 1: End word 11 -= 2^45, collides iff bit 63 of s11 (Mix block 2, before the step-11 rotate) is 0",
    "pair 2: End word 11 += 2^45, collides iff that bit is 1 (the complementary state-bit class)",
};
static const uint64_t PUBLISHED_SEED[2] = { 4, 0 };  /* explicit colliding seeds from the record */

static size_t unhex(const char *h, uint8_t *out) {
    size_t n = 0;
    for (; h[0] && h[1]; h += 2) { unsigned v; if (sscanf(h, "%2x", &v) != 1) abort(); out[n++] = (uint8_t)v; }
    return n;
}

/* bit 63 of s11 after `s11 += w11` in Mix block 2 (all four messages share bytes 0..183). */
static int s11_top_bit(const uint8_t *m, uint64_t h1, uint64_t h2) {
    uint64_t h[12]; int bit = 0;
    init_state(h, h1, h2);
    mix(m, h, NULL);
    mix(m + SC_BLOCK, h, &bit);
    return bit;
}


/* Integration pass 2: literal pair and witness independently reproduced in
 * experiment/verify-spooky32/pair275/, using this already validated complete
 * C implementation. No exact balance over the seed domain is assumed. */
static void selected_275(unsigned lg, uint64_t rseed, int mode) {
    const char *ahex="9d21a7fafac0e24f428e3c0124de5f8c54c1de8bef8535a636cfd97be5aad365055fb9bc0a26d493c5f21f4acbfade067eba007efbf79f8729cacdc39c584138336a77d8330a62803d3750884eb91b907e091a0ff547078c45198f347078fe21b0fa47014d7d1cd6142861dadfecd7fd0bf71f80b4e9112d36a8d91996608d73c49284098d900d7ef1f9c16471246346ca2277cbf87725a6d861b0db4924da8f5c5307034eb3933fcca647e8404e141afc956d0eb643e7ee9381dabba3c2d09e2dae44a4bda150f889d0704a2463cd7cb2b194ea3ccc9982ceae11e212336b6b3ad81125bd494e3f1b7291e3b0c73a74fa886a41f0fe25055b0f1fc96e8f7c0123002a69c35989a703b718ce8a1bfbddafb3e3", *bhex="9d21a7fafac0e24f428e3c0124de5f8c54c1de8bef8535a636cfd97be5aad365055fb9bc0a26d493c5f21f4acbfade067eba007efbf79f8729cacdc39c584138336a77d8330a62803d3750884eb91b907e091a0ff547078c45198f347078fe21b0fa47014d7d1cd6142861dadfecd7fd0bf71f80b4e9112d36a8d91996608d73c49284098d900d7ef1f9c16471246346ca2277cbf87725a6d861b0db4924da8f5c5307034eb3933fcca647e8404e141afc956d0eb643e76e9381dabba3c2d09e2dae44a4bda150f889d0704a2463cdfcb2b194ea3ccc9982ceae11e212336b6b3ad81125bd494e3f1b7291e3b0c73a74fa886a41f0fe25055b0f1fc96e8f7c0123002a69c35989a703b718ce8a1bfb5dafb3c3";
    uint8_t a[275], b[275];
    if(unhex(ahex,a)!=275 || unhex(bhex,b)!=275) abort();
    puts("\nselected 275-byte pair (L=35), same late-injection mechanism at Mix step 10:");
    printf("  M1 (275 bytes) = %s\n  M2 (275 bytes) = %s\n",ahex,bhex);
    uint64_t a1=0,a2=0,b1=0,b2=0;
    spooky2_128(a,275,&a1,&a2); spooky2_128(b,275,&b1,&b2);
    if(a1!=UINT64_C(0x6d1347279ecef355) || a2!=UINT64_C(0x147d4b759d34be02) || a1!=b1 || a2!=b2) abort();
    printf("  recorded colliding seed 0: H32(M1)=H32(M2)=%08x; H64(M1)=H64(M2)=%016llx; second word=%016llx\n",(unsigned)(uint32_t)a1,(unsigned long long)a1,(unsigned long long)a2);
    a1=a2=b1=b2=3;
    spooky2_128(a,275,&a1,&a2); spooky2_128(b,275,&b1,&b2);
    if((uint32_t)a1!=UINT32_C(0x1d115fbd) || (uint32_t)b1!=UINT32_C(0xaa6a510b)) abort();
    uint64_t n=UINT64_C(1)<<lg,c32=0,c64=0,c128=0,fs1=0,fs2=0,fh1=0,fh2=0;
    rng_seed(rseed);
    for(uint64_t i=0;i<n;i++) {
        uint64_t s1=rng_next(),s2=mode?rng_next():s1;
        a1=b1=s1;a2=b2=s2;
        spooky2_128(a,275,&a1,&a2); spooky2_128(b,275,&b1,&b2);
        c32+=(uint32_t)a1==(uint32_t)b1; c64+=a1==b1;
        if(a1==b1 && a2==b2) {
            if(!c128) {fs1=s1;fs2=s2;fh1=a1;fh2=a2;}
            c128++;
        }
    }
    printf("  selected pair: 32-bit collisions %llu / %llu; 64-bit collisions %llu / %llu; 128-bit collisions %llu / %llu\n",(unsigned long long)c32,(unsigned long long)n,(unsigned long long)c64,(unsigned long long)n,(unsigned long long)c128,(unsigned long long)n);
    printf("  full-output rate %.12g; sampled score %.6f (population balance unproved)\n",(double)c128/n,c128?log2(35.0*n/c128):INFINITY);
    if(c128) printf("  first colliding seed words %016llx %016llx: H64(M1)=H64(M2)=%016llx, second word=%016llx\n",(unsigned long long)fs1,(unsigned long long)fs2,(unsigned long long)fh1,(unsigned long long)fh2);
}

int main(int argc, char **argv) {
    char *end = NULL; int badarg = 0;
    int lg = argc > 1 ? (int)strtol(argv[1], &end, 10) : 24;
    if (argc > 1 && (end == argv[1] || *end)) badarg = 1;
    uint64_t rseed = argc > 2 ? strtoull(argv[2], &end, 0) : 1;
    if (argc > 2 && (end == argv[2] || *end)) badarg = 1;
    int mode = argc > 3 ? (int)strtol(argv[3], &end, 10) : 0;
    if (argc > 3 && (end == argv[3] || *end)) badarg = 1;
    if (badarg || lg < 0 || lg > 40 || (mode != 0 && mode != 1)) {
        fprintf(stderr, "usage: %s [log2 seeds (0..40), default 24] [rng seed, default 1] [seed mode 0|1]\n", argv[0]);
        return 2;
    }

    /* 1. Validate the implementation against SMHasher3's verification values. */
    uint32_t v64 = smhasher3_verification(8), v32 = smhasher3_verification(4), v128 = smhasher3_verification(16);
    printf("SMHasher3 verification: SpookyHash2_64 0x%08X (expect 0x972C4BDC), SpookyHash2_32 0x%08X (expect 0xA48BE265), SpookyHash2_128 0x%08X (expect 0x893CFCBE)\n", v64, v32, v128);
    if (v64 != 0x972C4BDCu || v32 != 0xA48BE265u || v128 != 0x893CFCBEu) {
        fprintf(stderr, "FATAL: SpookyHash V2 implementation does not reproduce the SMHasher3 verification values\n");
        abort();
    }
    printf("verification OK\n\n");

    /* 2. Decode and print the pairs. */
    static uint8_t M[2][2][512]; size_t L[2][2];
    for (int p = 0; p < 2; p++) {
        printf("%s\n", PAIR_DESC[p]);
        for (int q = 0; q < 2; q++) {
            L[p][q] = unhex(PAIR_HEX[p][q], M[p][q]);
            printf("  M%d (%zu bytes) = %s\n", q + 1, L[p][q], PAIR_HEX[p][q]);
        }
        if (L[p][0] != L[p][1] || L[p][0] < SC_SHORT) { fprintf(stderr, "bad pair\n"); abort(); }
        printf("  differing bytes:");
        for (size_t i = 0; i < L[p][0]; i++)
            if (M[p][0][i] != M[p][1][i]) printf(" [%zu] %02x->%02x", i, M[p][0][i], M[p][1][i]);
        printf("\n");
    }

    /* 3. The explicit colliding seeds from the record (h1 = h2 = seed). */
    printf("\nexplicit colliding seeds (h1 = h2 = seed):\n");
    for (int p = 0; p < 2; p++) {
        uint64_t s = PUBLISHED_SEED[p], a1 = s, a2 = s, b1 = s, b2 = s;
        spooky2_128(M[p][0], L[p][0], &a1, &a2);
        spooky2_128(M[p][1], L[p][1], &b1, &b2);
        printf("  pair %d, seed 0x%016llx: SpookyHash2_64(M1) = %016llx, SpookyHash2_64(M2) = %016llx  %s"
               "  (128-bit: %016llx%016llx / %016llx%016llx  %s)\n", p + 1, (unsigned long long)s,
               (unsigned long long)a1, (unsigned long long)b1, a1 == b1 ? "COLLISION" : "no collision",
               (unsigned long long)a1, (unsigned long long)a2, (unsigned long long)b1, (unsigned long long)b2,
               (a1 == b1 && a2 == b2) ? "collision" : "no collision");
        if (a1 != b1) { fprintf(stderr, "FATAL: published seed does not collide\n"); abort(); }
    }

    /* 4. Uniform random seeds. */
    uint64_t N = 1ULL << lg, coll64[2] = { 0, 0 }, coll128[2] = { 0, 0 }, both = 0, neither = 0, predicted = 0;
    uint64_t first_s1[2] = { 0, 0 }, first_s2[2] = { 0, 0 }, first_h[2] = { 0, 0 }; int have_first[2] = { 0, 0 };
    rng_seed(rseed);
    for (uint64_t t = 0; t < N; t++) {
        uint64_t s1 = rng_next(), s2 = mode ? rng_next() : s1;
        int c[2];
        for (int p = 0; p < 2; p++) {
            uint64_t a1 = s1, a2 = s2, b1 = s1, b2 = s2;
            spooky2_128(M[p][0], L[p][0], &a1, &a2);
            spooky2_128(M[p][1], L[p][1], &b1, &b2);
            c[p] = (a1 == b1);
            coll64[p] += (uint64_t)c[p];
            coll128[p] += (uint64_t)(a1 == b1 && a2 == b2);
            if (c[p] && !have_first[p]) { have_first[p] = 1; first_s1[p] = s1; first_s2[p] = s2; first_h[p] = a1; }
        }
        both += (uint64_t)(c[0] && c[1]);
        neither += (uint64_t)(!c[0] && !c[1]);
        int bit = s11_top_bit(M[0][0], s1, s2);
        predicted += (uint64_t)(c[0] == (bit == 0) && c[1] == (bit == 1));
    }
    printf("\nrandom seeds: N = 2^%d, seed mode %d (%s), rng seed %llu\n", lg, mode,
           mode ? "independent uniform h1, h2" : "h1 = h2 = uniform 64-bit seed", (unsigned long long)rseed);
    for (int p = 0; p < 2; p++) {
        double r = (double)coll64[p] / (double)N;
        printf("  pair %d: 64-bit collisions %llu / %llu = %.6f = 2^%.4f   (128-bit collisions %llu, 2^%.4f)\n",
               p + 1, (unsigned long long)coll64[p], (unsigned long long)N, r, log2(r),
               (unsigned long long)coll128[p], log2((double)coll128[p] / (double)N));
    }
    printf("  mechanism: exactly one of the two pairs collides for %llu / %llu seeds (both: %llu, neither: %llu);\n"
           "             the bit-63 predictor is right for %llu / %llu seeds\n",
           (unsigned long long)(N - both - neither), (unsigned long long)N, (unsigned long long)both,
           (unsigned long long)neither, (unsigned long long)predicted, (unsigned long long)N);
    for (int p = 0; p < 2; p++) {
        if (!have_first[p]) { printf("  pair %d: no colliding seed found\n", p + 1); continue; }
        if (mode) printf("  pair %d: first colliding seed h1 = 0x%016llx, h2 = 0x%016llx, both hash to %016llx\n",
                         p + 1, (unsigned long long)first_s1[p], (unsigned long long)first_s2[p], (unsigned long long)first_h[p]);
        else      printf("  pair %d: first colliding seed 0x%016llx, both hash to %016llx\n",
                         p + 1, (unsigned long long)first_s1[p], (unsigned long long)first_h[p]);
    }
    selected_275((unsigned)lg,rseed,mode);
    return 0;
}
