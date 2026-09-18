/*
 * komihash_pair.c -- fixed message pairs that collide under komihash 5.34
 * for a large fraction of random 64-bit seeds (pair 1), or under one weak
 * seed (pair 2).  Single file, no dependencies beyond the C11 library.
 *
 * Layout:  this demo code first; the reference implementation follows,
 * appended verbatim from komihash.h version 5.34 by Aleksey Vaneev (MIT),
 * https://github.com/avaneev/komihash -- see the marker lines below.
 *
 * Build:   cc -O2 -std=c11 -o komihash_pair komihash_pair.c -lm
 * Run:     ./komihash_pair [log2 seeds = 24] [rng seed hex = c0ffee]
 *
 * Demo code (everything above the "komihash.h 5.34 verbatim" marker):
 *
 * MIT License
 *
 * Copyright (c) 2026 Thomas Dybdahl Ahle
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
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
 */
#include <inttypes.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* The two reference functions used below are defined in the verbatim
 * komihash.h block at the end of this file. */
static uint64_t komihash(const void *const Msg0, size_t MsgLen, const uint64_t UseSeed);
static void komihash_set_preseed_inner(uint64_t *const outSeed1, uint64_t *const outSeed5,
                                       const uint64_t UseSeed);

/* ------------------------------------------------------------------------ */
/* Published pairs and records (hex copied exactly from the panel's JSON).  */

/* Pair 1: 64 bytes, lanes 1 and 2 of the 64-byte loop tied through the
 * public constants IVAL2 / IVAL6; m' flips bit 0 of words 0 and 1.
 * Unconditional collision rate over uniform 64-bit seeds: 0.9106.          */
static const char *P1_M  = "0000000000000000447370032e8a191311111111111111112222222222222222f0f9dda4c1c0a15e9cf534900ea6f5e033333333333333334444444444444444";
static const char *P1_M2 = "0100000000000000457370032e8a191311111111111111112222222222222222f0f9dda4c1c0a15e9cf534900ea6f5e033333333333333334444444444444444";
static const uint64_t P1_SEED = UINT64_C(0x27d1f77dc2a01269); /* published colliding seed */
static const uint64_t P1_HASH = UINT64_C(0x113c6b88bc913857); /* h(m) = h(m') under it     */

/* Pair 2: 15 bytes, common 7-byte tail df e2 a4 80 a7 b1 ef.  Weak-seed
 * class: seeds whose state word S5 after the init round equals the tail
 * word 0x01efb1a780a4e2df, so r2h = S5 ^ tail = 0 and the final multiply
 * is dead (the output no longer depends on the first 8 bytes).            */
static const char *P2_M  = "8877665544332211dfe2a480a7b1ef";
static const char *P2_M2 = "1122334455667788dfe2a480a7b1ef";
static const uint64_t P2_SEED = UINT64_C(0xa41b7e7e02118bba);
static const uint64_t P2_HASH = UINT64_C(0x722cdb7c77771040);
static const uint64_t P2_S5   = UINT64_C(0x01efb1a780a4e2df);

/* ------------------------------------------------------------------------ */
/* RNG: splitmix64 seeding xoshiro256** (Blackman & Vigna, public domain).  */

static uint64_t splitmix64(uint64_t *x) {
    uint64_t z = (*x += UINT64_C(0x9E3779B97F4A7C15));
    z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
    z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
    return z ^ (z >> 31);
}
typedef struct { uint64_t s[4]; } rng_t;
static void rng_init(rng_t *r, uint64_t seed) { for (int k = 0; k < 4; k++) r->s[k] = splitmix64(&seed); }
static inline uint64_t rotl64(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
static inline uint64_t rng_next(rng_t *r) {
    uint64_t *s = r->s, res = rotl64(s[1] * 5, 7) * 9, t = s[1] << 17;
    s[2] ^= s[0]; s[3] ^= s[1]; s[1] ^= s[2]; s[0] ^= s[3]; s[2] ^= t; s[3] = rotl64(s[3], 45);
    return res;
}

/* ------------------------------------------------------------------------ */
/* Small helpers.                                                           */

static void die(const char *what) { fprintf(stderr, "FATAL: %s\n", what); exit(1); }
static int hexval(int c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    die("bad hex digit"); return 0;
}
static size_t unhex(const char *hex, uint8_t *out, size_t cap) {
    size_t n = strlen(hex) / 2;
    if (n > cap || strlen(hex) % 2) die("hex string length");
    for (size_t i = 0; i < n; i++) out[i] = (uint8_t)(hexval(hex[2 * i]) * 16 + hexval(hex[2 * i + 1]));
    return n;
}
static uint64_t ld64(const uint8_t *p) { uint64_t v; memcpy(&v, p, 8); return v; }
static uint64_t ld32(const uint8_t *p) { uint32_t v; memcpy(&v, p, 4); return v; }
static void st64le(uint8_t *p, uint64_t v) { for (int i = 0; i < 8; i++) p[i] = (uint8_t)(v >> (8 * i)); }
/* high 64 bits of the 128-bit product a*b, in portable C11 */
static uint64_t mulhi64(uint64_t a, uint64_t b) {
    uint64_t a0 = (uint32_t)a, a1 = a >> 32, b0 = (uint32_t)b, b1 = b >> 32;
    uint64_t p00 = a0 * b0, p01 = a0 * b1, p10 = a1 * b0, p11 = a1 * b1;
    uint64_t mid = (p00 >> 32) + (uint32_t)p01 + (uint32_t)p10;
    return p11 + (p01 >> 32) + (p10 >> 32) + (mid >> 32);
}
static void print_rate(const char *label, uint64_t hits, uint64_t n) {
    double p = (double)hits / (double)n;
    printf("  %s: %" PRIu64 "/%" PRIu64 " = %.6f", label, hits, n, p);
    if (hits) printf(" (log2 %.4f, s.e. %.6f)\n", log2(p), sqrt(p * (1 - p) / (double)n));
    else printf(" (< 2^-%.1f at this sample size)\n", log2((double)n));
}

/* ------------------------------------------------------------------------ */
/* Validation: the SMHasher3 verification value and upstream test vectors.  */

/* SMHasher3 lib/Hashinfo.cpp _ComputedVerifyImpl: hash keys {}, {0}, {0,1},
 * ..., {0..254} with seed 256-i, concatenate the little-endian outputs, hash
 * the 2048-byte array with seed 0, take the first 4 bytes little-endian.   */
static uint32_t smhasher3_verification(void) {
    uint8_t key[256] = {0}, hashes[8 * 256] = {0}, total[8];
    for (int i = 0; i < 256; i++) {
        st64le(&hashes[8 * i], komihash(key, (size_t)i, (uint64_t)(256 - i)));
        key[i] = (uint8_t)i;
    }
    st64le(total, komihash(hashes, sizeof hashes, 0));
    return (uint32_t)total[0] | (uint32_t)total[1] << 8 | (uint32_t)total[2] << 16 | (uint32_t)total[3] << 24;
}

static void validate(void) {
    uint32_t v = smhasher3_verification();
    printf("validation: SMHasher3 verification value 0x%08" PRIX32 " (expected 0x8157FF6D) %s\n", v,
           v == 0x8157FF6Du ? "OK" : "MISMATCH");
    if (v != 0x8157FF6Du) die("SMHasher3 verification value mismatch");
    /* Four vectors from the upstream README (testvec.c construction). */
    uint8_t bulk[256];
    for (int i = 0; i < 256; i++) bulk[i] = (uint8_t)i;
    const char *s32 = "This is a 32-byte testing string", *s16 = "A 16-byte string";
    struct { const void *m; size_t n; uint64_t seed, want; const char *name; } tv[4] = {
        { s32,  32,  0,                              UINT64_C(0x05ad960802903a9d), "\"This is a 32-byte testing string\", seed 0" },
        { s16,  16,  256,                            UINT64_C(0x11c31ccabaa524f1), "\"A 16-byte string\", seed 256" },
        { bulk, 64,  UINT64_C(0x0123456789abcdef),   UINT64_C(0x765490569ccd77f2), "bulk(64), seed 0x0123456789abcdef" },
        { bulk, 256, 0,                              UINT64_C(0x94c3dbdca59ddf57), "bulk(256), seed 0" },
    };
    int ok = 0;
    for (int i = 0; i < 4; i++) {
        uint64_t got = komihash(tv[i].m, tv[i].n, tv[i].seed);
        if (got == tv[i].want) ok++;
        else printf("  upstream vector FAILED: %s: got 0x%016" PRIx64 " want 0x%016" PRIx64 "\n", tv[i].name, got, tv[i].want);
    }
    printf("validation: upstream README test vectors %d/4 OK\n", ok);
    if (ok != 4) die("upstream test vector mismatch");
}

/* ------------------------------------------------------------------------ */
/* Pair 1: the lane-tie pair.                                               */

static void pair1(uint64_t N, uint64_t rngseed) {
    uint8_t A[128], B[128];
    size_t la = unhex(P1_M, A, 64), lb = unhex(P1_M2, B, 64);
    if (la != 64 || lb != 64) die("pair 1 length");
    printf("\n[pair 1] lane-tie pair, 64 bytes, unconditional (uniform 64-bit seed)\n");
    printf("  m  = %s\n  m' = %s\n  bytes that differ:", P1_M, P1_M2);
    for (int i = 0; i < 64; i++) if (A[i] != B[i]) printf(" %d", i);
    printf("\n");

    /* Sample N uniform seeds.  Alongside the collision count, split the
     * seeds by whether the high half of lane 1's product (S1^m0)*(S5^m4)
     * survives the bit-0 flip of m0 (no carry into it) -- the mechanism. */
    rng_t r; rng_init(&r, rngseed);
    uint64_t coll = 0, hi_eq = 0, coll_hi_eq = 0, first_seed = 0, first_h = 0; int have = 0;
    uint64_t w0a = ld64(A), w0b = ld64(B), w4 = ld64(A + 32);
    for (uint64_t i = 0; i < N; i++) {
        uint64_t seed = rng_next(&r);
        uint64_t ha = komihash(A, 64, seed), hb = komihash(B, 64, seed);
        uint64_t s1, s5; komihash_set_preseed_inner(&s1, &s5, seed);
        int eq = mulhi64(s1 ^ w0a, s5 ^ w4) == mulhi64(s1 ^ w0b, s5 ^ w4);
        hi_eq += (uint64_t)eq;
        if (ha == hb) { coll++; coll_hi_eq += (uint64_t)eq; if (!have) { have = 1; first_seed = seed; first_h = ha; } }
    }
    printf("  N = 2^%.0f seeds (rng seed 0x%" PRIx64 ")\n", log2((double)N), rngseed);
    print_rate("collisions", coll, N);
    print_rate("lane-1 high half unchanged (no carry)", hi_eq, N);
    print_rate("  collisions given no carry (predicted 1)", coll_hi_eq, hi_eq);
    print_rate("  collisions given a carry (predicted 3/4)", coll - coll_hi_eq, N - hi_eq);
    uint64_t hp = komihash(A, 64, P1_SEED), hp2 = komihash(B, 64, P1_SEED);
    printf("  published seed 0x%016" PRIx64 ": h(m) = 0x%016" PRIx64 " h(m') = 0x%016" PRIx64 " %s (record: 0x%016" PRIx64 ")\n",
           P1_SEED, hp, hp2, hp == hp2 ? "COLLIDE" : "differ", P1_HASH);
    if (hp != hp2 || hp != P1_HASH) die("published seed for pair 1 does not reproduce");
    if (have) printf("  first colliding seed in this run: 0x%016" PRIx64 " -> h = 0x%016" PRIx64 "\n", first_seed, first_h);

    /* The pair only needs to sit in the first 64-byte block: any common
     * suffix keeps the rate for lengths 64..127; 63 and 128 are controls. */
    memset(A + 64, 0x99, 64); memset(B + 64, 0x99, 64);
    static const size_t lens[6] = { 63, 64, 65, 96, 127, 128 };
    printf("  same pair as the first block of longer messages (suffix bytes 0x99, 2^16 seeds each):\n   ");
    for (int k = 0; k < 6; k++) {
        rng_t r2; rng_init(&r2, rngseed ^ (uint64_t)lens[k]);
        uint64_t hits = 0;
        for (uint64_t i = 0; i < 65536; i++) { uint64_t s = rng_next(&r2); hits += komihash(A, lens[k], s) == komihash(B, lens[k], s); }
        printf(" len %3zu: %5" PRIu64 "/65536 (%.4f)%s", lens[k], hits, hits / 65536.0, k == 2 ? "\n   " : "");
    }
    printf("\n");
}

/* ------------------------------------------------------------------------ */
/* Pair 2: the dead-multiply weak seed.                                     */

static void pair2(uint64_t N, uint64_t rngseed) {
    uint8_t A[16], B[16];
    size_t la = unhex(P2_M, A, 16), lb = unhex(P2_M2, B, 16);
    if (la != 15 || lb != 15) die("pair 2 length");
    printf("\n[pair 2] weak-seed pair, 15 bytes, conditional on S5 == 0x%016" PRIx64 " after the init round\n", P2_S5);
    printf("  m  = %s\n  m' = %s\n  bytes that differ:", P2_M, P2_M2);
    for (int i = 0; i < 15; i++) if (A[i] != B[i]) printf(" %d", i);
    printf("\n");

    rng_t r; rng_init(&r, rngseed ^ UINT64_C(0x2));
    uint64_t coll = 0, first_seed = 0, first_h = 0; int have = 0;
    for (uint64_t i = 0; i < N; i++) {
        uint64_t seed = rng_next(&r);
        uint64_t ha = komihash(A, 15, seed), hb = komihash(B, 15, seed);
        if (ha == hb) { coll++; if (!have) { have = 1; first_seed = seed; first_h = ha; } }
    }
    printf("  N = 2^%.0f uniform seeds\n", log2((double)N));
    print_rate("unconditional collisions", coll, N);
    if (have) printf("  first colliding seed in this run: 0x%016" PRIx64 " -> h = 0x%016" PRIx64 "\n", first_seed, first_h);
    printf("  seed class {S5 == 0x%016" PRIx64 "}: exactly 1 seed in 2^64, density 2^-64\n"
           "    (exhaustive count by the panel's solver, not re-run here; unconditional rate is therefore 2^-64)\n", P2_S5);

    /* The mechanism, checked exactly under the class member. */
    uint64_t s1, s5; komihash_set_preseed_inner(&s1, &s5, P2_SEED);
    uint64_t tail = ((ld32(A + 11) | UINT64_C(1) << 32) >> 8) << 32 | ld32(A + 8); /* komihash's 12..15-byte tail word */
    printf("  class member 0x%016" PRIx64 ": S5 after init = 0x%016" PRIx64 " %s, tail word = 0x%016" PRIx64 ", r2h = S5 ^ tail = 0x%" PRIx64 " (dead multiply)\n",
           P2_SEED, s5, s5 == P2_S5 ? "OK" : "MISMATCH", tail, s5 ^ tail);
    if (s5 != P2_S5 || (s5 ^ tail) != 0) die("weak-seed class membership does not reproduce");
    uint64_t ha = komihash(A, 15, P2_SEED), hb = komihash(B, 15, P2_SEED);
    printf("  under that seed: h(m) = 0x%016" PRIx64 " h(m') = 0x%016" PRIx64 " %s (record: 0x%016" PRIx64 ")\n",
           ha, hb, ha == hb ? "COLLIDE" : "differ", P2_HASH);
    if (ha != hb || ha != P2_HASH) die("published seed for pair 2 does not reproduce");
    /* Conditional rate: over the class (one seed) the pair collides 1/1; and
     * every 15-byte message with this tail collides -- sample the prefix. */
    uint64_t same = 0; uint8_t C[16]; memcpy(C, A, 15);
    for (uint64_t i = 0; i < 65536; i++) { st64le(C, rng_next(&r)); same += komihash(C, 15, P2_SEED) == ha; }
    uint64_t cm = (ha == hb);   /* the class has one member; die() above already checked it */
    printf("  conditional rate over the class: %" PRIu64 "/1 = %.6f (log2 %.0f)\n", cm, (double)cm, log2((double)cm));
    printf("  random 8-byte prefixes with the same 7-byte tail under that seed: %" PRIu64 "/65536 hash to 0x%016" PRIx64 "\n", same, ha);
}

int main(int argc, char **argv) {
    char *end = NULL;
    long lg = argc > 1 ? strtol(argv[1], &end, 10) : 24;
    if (argc > 1 && (end == argv[1] || *end)) die("log2 seeds must be a decimal number");
    if (lg < 1 || lg > 40) die("log2 seeds must be in 1..40");
    uint64_t rngseed = argc > 2 ? strtoull(argv[2], &end, 16) : UINT64_C(0xc0ffee);
    if (argc > 2 && (end == argv[2] || *end)) die("rng seed must be a hexadecimal number");
    printf("komihash 5.34 (reference code embedded verbatim), 64-bit output; collision = full 64-bit equality\n");
    validate();
    pair1(UINT64_C(1) << lg, rngseed);
    pair2(UINT64_C(1) << lg, rngseed);
    return 0;
}

/* >>>> komihash.h 5.34 verbatim begins on the next line (Copyright (c) 2021-2026 Aleksey Vaneev, MIT License; https://github.com/avaneev/komihash) */
/**
 * @file komihash.h
 *
 * @version 5.34
 *
 * @brief The header file for the `komihash` 64-bit hash function,
 * the `komirand` 64-bit PRNG, and the streamed `komihash` implementation.
 *
 * The source code is written in ISO C99 and automatically provides full C++
 * compatibility when compiled with a C++ compiler.
 *
 * The `komihash` function is named in honor of the Komi Republic (located in
 * Russia), the author's native region.
 *
 * The description is available at https://github.com/avaneev/komihash
 *
 * Email: aleksey.vaneev@gmail.com or info@voxengo.com
 *
 * LICENSE:
 *
 * Copyright (c) 2021-2026 Aleksey Vaneev
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
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
 */

#ifndef KOMIHASH_INCLUDED
#define KOMIHASH_INCLUDED

#define KOMIHASH_VER_STR "5.34" ///< KOMIHASH source code version string.

/**
 * @def KOMIHASH_NS_CUSTOM
 * @brief If this macro is defined externally, all symbols will be placed
 * in the namespace specified by the macro, and they will not be placed in the
 * global namespace. WARNING: If the value defined by the macro is empty, the
 * symbols will be placed in the global namespace anyway.
 */

/**
 * @def KOMIHASH_U64_C( x )
 * @brief Macro that defines a numeric constant as an unsigned 64-bit value.
 *
 * @param x Value.
 */

/**
 * @def KOMIHASH_NOEXC
 * @brief Macro that defines the `noexcept` function specifier in a C++
 * environment.
 */

/**
 * @def KOMIHASH_NS
 * @brief Macro that defines the actual implementation namespace in a C++
 * environment. Relevant symbols are also placed in the global namespace
 * (if @ref KOMIHASH_NS_CUSTOM is undefined).
 */

#if defined( __cplusplus )

	#include <cstring> // This header defines std::size_t.

	#if __cplusplus >= 201103L

		#include <cstdint>

		#define KOMIHASH_U64_C( x ) UINT64_C( x )
		#define KOMIHASH_NOEXC noexcept

	#else // __cplusplus >= 201103L

		#include <stdint.h> // A C99 fallback, as C++98 has no cstdint header.

		#define KOMIHASH_U64_C( x ) (uint64_t) x
		#define KOMIHASH_NOEXC throw()

	#endif // __cplusplus >= 201103L

	#if defined( KOMIHASH_NS_CUSTOM )
		#define KOMIHASH_NS KOMIHASH_NS_CUSTOM
	#else // defined( KOMIHASH_NS_CUSTOM )
		#define KOMIHASH_NS komihash_impl
	#endif // defined( KOMIHASH_NS_CUSTOM )

#else // defined( __cplusplus )

	#include <string.h> // This header defines size_t.
	#include <stdint.h>

	#define KOMIHASH_U64_C( x ) (uint64_t) x
	#define KOMIHASH_NOEXC

#endif // defined( __cplusplus )

/**
 * @{
 * @brief Unsigned 64-bit constant that defines the initial state of the
 * hash function (the first few digits of the fractional part of pi).
 */

#define KOMIHASH_IVAL1 KOMIHASH_U64_C( 0x243F6A8885A308D3 )
#define KOMIHASH_IVAL2 KOMIHASH_U64_C( 0x13198A2E03707344 )
#define KOMIHASH_IVAL3 KOMIHASH_U64_C( 0xA4093822299F31D0 )
#define KOMIHASH_IVAL4 KOMIHASH_U64_C( 0x082EFA98EC4E6C89 )
#define KOMIHASH_IVAL5 KOMIHASH_U64_C( 0x452821E638D01377 )
#define KOMIHASH_IVAL6 KOMIHASH_U64_C( 0xBE5466CF34E90C6C )
#define KOMIHASH_IVAL7 KOMIHASH_U64_C( 0xC0AC29B7C97C50DD )
#define KOMIHASH_IVAL8 KOMIHASH_U64_C( 0x3F84D5B5B5470917 )

/** @} */

/**
 * @def KOMIHASH_VAL01
 * @brief Unsigned 64-bit constant formed by repeating the `01` bit pair.
 */

#define KOMIHASH_VAL01 KOMIHASH_U64_C( 0x5555555555555555 )

/**
 * @def KOMIHASH_VAL10
 * @brief Unsigned 64-bit constant formed by repeating the `10` bit pair.
 */

#define KOMIHASH_VAL10 KOMIHASH_U64_C( 0xAAAAAAAAAAAAAAAA )

/**
 * @def KOMIHASH_DEFSEED1
 * @brief Initial `Seed1` value for the default (0) seed.
 */

#define KOMIHASH_DEFSEED1 KOMIHASH_U64_C( 0x01D2EE0AE40A48DC )

/**
 * @def KOMIHASH_DEFSEED5
 * @brief Initial `Seed5` value for the default (0) seed.
 */

#define KOMIHASH_DEFSEED5 KOMIHASH_U64_C( 0x4EF2E8526FEA8BC9 )

/**
 * @def KOMIHASH_ENDIAN_DEFS
 * @brief This macro is defined if the KOMIHASH_LITTLE_ENDIAN macro is not
 * defined externally.
 */

/**
 * @def KOMIHASH_LITTLE_ENDIAN
 * @brief Endianness definition macro that can be used as a logical constant.
 *
 * When C++20 is available, this macro is defined as 0, and the actual
 * endianness is determined at compile time via std::endian::native.
 * This means that a value of 0 for this macro indicates "big-endian" or
 * "unknown".
 *
 * Note that for exotic platforms, you may need to include
 * a compiler-dependent `endian.h` header before including `komihash.h` to
 * avoid using a potentially slower fallback.
 *
 * This macro can be externally defined as 1 to reduce overhead if endianness
 * correction and hash value portability are unnecessary.
 */

/**
 * @def KOMIHASH_COND_EC( vl, vb )
 * @brief Macro that emits either `vl` or `vb`, depending on the platform's
 * endianness.
 */

#if !defined( KOMIHASH_LITTLE_ENDIAN )

	#define KOMIHASH_ENDIAN_DEFS

	#if ( defined( __BYTE_ORDER__ ) && defined( __ORDER_LITTLE_ENDIAN__ ) && \
			__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ ) || \
		( defined( __BYTE_ORDER ) && defined( __LITTLE_ENDIAN ) && \
			__BYTE_ORDER == __LITTLE_ENDIAN ) || \
		( defined( _BYTE_ORDER ) && defined( _LITTLE_ENDIAN ) && \
			_BYTE_ORDER == _LITTLE_ENDIAN ) || \
		defined( __LITTLE_ENDIAN__ ) || defined( __little_endian__ ) || \
		( !defined( __BYTE_ORDER ) && !defined( __BIG_ENDIAN ) && \
			defined( __LITTLE_ENDIAN )) || \
		( !defined( _BYTE_ORDER ) && !defined( _BIG_ENDIAN ) && \
			defined( _LITTLE_ENDIAN )) || \
		defined( _WIN32 ) || (( defined( i386 ) || defined( __i386 ) || \
		defined( __i386__ )) && !defined( __VOS__ )) || defined( _X86_ ) || \
		defined( _M_IX86 ) || defined( _M_AMD64 ) || defined( _M_ARM ) || \
		defined( __x86_64 ) || defined( __x86_64__ ) || \
		defined( __amd64 ) || defined( __amd64__ )

		#define KOMIHASH_LITTLE_ENDIAN 1

	#elif ( defined( __BYTE_ORDER__ ) && defined( __ORDER_BIG_ENDIAN__ ) && \
			__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__ ) || \
		( defined( __BYTE_ORDER ) && defined( __BIG_ENDIAN ) && \
			__BYTE_ORDER == __BIG_ENDIAN ) || \
		( defined( _BYTE_ORDER ) && defined( _BIG_ENDIAN ) && \
			_BYTE_ORDER == _BIG_ENDIAN ) || \
		defined( __BIG_ENDIAN__ ) || defined( __big_endian__ ) || \
		( !defined( __BYTE_ORDER ) && !defined( __LITTLE_ENDIAN ) && \
			defined( __BIG_ENDIAN )) || \
		( !defined( _BYTE_ORDER ) && !defined( _LITTLE_ENDIAN ) && \
			defined( _BIG_ENDIAN )) || \
		defined( __SYSC_ZARCH__ ) || defined( __zarch__ ) || \
		defined( __s390__ ) || defined( __s390x__ ) || \
		defined( __sparc ) || defined( __sparc__ ) || defined( __VOS__ )

		#define KOMIHASH_LITTLE_ENDIAN 0
		#define KOMIHASH_COND_EC( vl, vb ) ( vb )

	#elif defined( __cplusplus ) && __cplusplus >= 202002L

		#include <bit>

		#define KOMIHASH_LITTLE_ENDIAN 0
		#define KOMIHASH_COND_EC( vl, vb ) ( std::endian::native == \
			std::endian::little ? vl : vb )

	#else // defined( __cplusplus )

		#define KOMIHASH_LITTLE_ENDIAN 0
		#define KOMIHASH_COND_EC( vl, vb ) ( kh_is_little_endian() ? vl : vb )

	#endif // defined( __cplusplus )

#else // !defined( KOMIHASH_LITTLE_ENDIAN )

	#if !KOMIHASH_LITTLE_ENDIAN
		#error The KOMIHASH_LITTLE_ENDIAN macro must evaluate to true.
	#endif // !KOMIHASH_LITTLE_ENDIAN

#endif // !defined( KOMIHASH_LITTLE_ENDIAN )

/**
 * @def KOMIHASH_ICC_GCC
 * @brief Macro that denotes the use of the ICC classic compiler with
 * GCC-style built-in functions.
 */

#if defined( __INTEL_COMPILER ) && __INTEL_COMPILER >= 1300 && \
	!defined( _MSC_VER )

	#define KOMIHASH_ICC_GCC

#endif // ICC check

/**
 * @def KOMIHASH_GCC_BUILTINS
 * @brief Macro that denotes the availability of GCC-style built-in functions.
 */

#if defined( __GNUC__ ) || defined( __clang__ ) || \
	defined( __IBMC__ ) || defined( __IBMCPP__ ) || \
	defined( __COMPCERT__ ) || defined( KOMIHASH_ICC_GCC )

	#define KOMIHASH_GCC_BUILTINS

#endif // GCC built-ins check

/**
 * @def KOMIHASH_BMI2
 * @brief Macro that denotes the availability of the `mulx` intrinsic
 * (MSVC-compatible compilers only).
 */

#if defined( _MSC_VER )
	#if defined( __BMI2__ ) || ( !defined( KOMIHASH_GCC_BUILTINS ) && \
		defined( _M_AMD64 ) && defined( __AVX2__ ) && \
		( defined( __INTEL_COMPILER ) || _MSC_VER >= 1900 ))

		#include <immintrin.h>
		#define KOMIHASH_BMI2

	#else // BMI2

		#include <intrin.h>

	#endif // BMI2
#endif // defined( _MSC_VER )

/**
 * @def KOMIHASH_EC32( v )
 * @brief Macro that applies 32-bit byte-swapping for endianness correction.
 *
 * On big-endian platforms, this macro is left undefined when an unknown
 * compiler is used.
 *
 * @param v Value to byte-swap.
 */

/**
 * @def KOMIHASH_EC64( v )
 * @brief Macro that applies 64-bit byte-swapping for endianness correction.
 *
 * On big-endian platforms, this macro is left undefined when an unknown
 * compiler is used.
 *
 * @param v Value to byte-swap.
 */

#if KOMIHASH_LITTLE_ENDIAN

	#define KOMIHASH_EC32( v ) ( v )
	#define KOMIHASH_EC64( v ) ( v )

#else // KOMIHASH_LITTLE_ENDIAN

	#if defined( KOMIHASH_GCC_BUILTINS )

		#define KOMIHASH_EC32( v ) KOMIHASH_COND_EC( v, __builtin_bswap32( v ))
		#define KOMIHASH_EC64( v ) KOMIHASH_COND_EC( v, __builtin_bswap64( v ))

	#elif defined( _MSC_VER )

		#if defined( __cplusplus )
			#include <cstdlib>
		#else // defined( __cplusplus )
			#include <stdlib.h>
		#endif // defined( __cplusplus )

		#define KOMIHASH_EC32( v ) KOMIHASH_COND_EC( v, _byteswap_ulong( v ))
		#define KOMIHASH_EC64( v ) KOMIHASH_COND_EC( v, _byteswap_uint64( v ))

	#elif defined( __cplusplus ) && __cplusplus >= 202302L

		#include <bit>

		#define KOMIHASH_EC32( v ) KOMIHASH_COND_EC( v, std::byteswap( v ))
		#define KOMIHASH_EC64( v ) KOMIHASH_COND_EC( v, std::byteswap( v ))

	#endif // defined( __cplusplus )

#endif // KOMIHASH_LITTLE_ENDIAN

/**
 * @def KOMIHASH_LIKELY( x )
 * @brief Macro that indicates an expression is likely to be true and is used
 * for manual micro-optimization.
 *
 * @param x Expression that is likely to evaluate to `true`.
 */

/**
 * @def KOMIHASH_UNLIKELY( x )
 * @brief Macro that indicates an expression is unlikely to be true and is
 * used for manual micro-optimization.
 *
 * @param x Expression that is unlikely to evaluate to `true`.
 */

/**
 * @def KOMIHASH_LIKELY_DO
 * @brief Macro that applies the C++20 `[[likely]]` attribute to do-while
 * loops.
 */

/**
 * @def KOMIHASH_LIKELY_DO_EXPR( x )
 * @brief Macro that indicates a likely condition and is used for manual
 * micro-optimization of do-while loops.
 *
 * @param x Expression that is likely to evaluate to `true`.
 */

#if defined( KOMIHASH_GCC_BUILTINS )

	#define KOMIHASH_LIKELY( x ) ( __builtin_expect( x, 1 ))
	#define KOMIHASH_UNLIKELY( x ) ( __builtin_expect( x, 0 ))

#elif defined( __cplusplus ) && __cplusplus >= 202002L

	#define KOMIHASH_LIKELY( x ) ( x ) [[likely]]
	#define KOMIHASH_UNLIKELY( x ) ( x ) [[unlikely]]
	#define KOMIHASH_LIKELY_DO [[likely]]
	#define KOMIHASH_LIKELY_DO_EXPR( x ) ( x )

#else // defined( __cplusplus )

	#define KOMIHASH_LIKELY( x ) ( x )
	#define KOMIHASH_UNLIKELY( x ) ( x )

#endif // defined( __cplusplus )

#if !defined( KOMIHASH_LIKELY_DO )
	#define KOMIHASH_LIKELY_DO
	#define KOMIHASH_LIKELY_DO_EXPR( x ) KOMIHASH_LIKELY( x )
#endif // !defined( KOMIHASH_LIKELY_DO )

/**
 * @def KOMIHASH_PREFETCH( a )
 * @brief Macro that prefetches data from the given memory address into the
 * CPU cache.
 *
 * The level-3 temporal locality hint is used because the data may later be
 * used for collision resolution or for a subsequent disk write.
 *
 * @param a Prefetch address.
 */

#if defined( KOMIHASH_GCC_BUILTINS ) && !defined( __COMPCERT__ )

	#define KOMIHASH_PREFETCH( a ) __builtin_prefetch( a, 0, 3 )

#elif defined( _MSC_VER ) && defined( _M_AMD64 ) && \
	!defined( __INTEL_COMPILER )

	#include <intrin.h>

	#define KOMIHASH_PREFETCH( a ) _mm_prefetch( (const char*) ( a ), \
		_MM_HINT_T0 )

#else // defined( _MSC_VER )

	#define KOMIHASH_PREFETCH( a ) (void) 0

#endif // defined( _MSC_VER )

/**
 * @def KOMIHASH_STATIC
 * @brief Macro that defines a function as "static".
 */

#if defined( KOMIHASH_GCC_BUILTINS )

	#define KOMIHASH_STATIC static __attribute__((unused))

#elif ( defined( __cplusplus ) && __cplusplus >= 201703L ) || \
	( defined( __STDC_VERSION__ ) && __STDC_VERSION__ >= 202311L )

	#define KOMIHASH_STATIC [[maybe_unused]] static

#else // defined( __cplusplus )

	#define KOMIHASH_STATIC static

#endif // defined( __cplusplus )

/**
 * @def KOMIHASH_INLINE
 * @brief Macro that defines a function as an inline function, at the
 * compiler's discretion.
 */

#define KOMIHASH_INLINE KOMIHASH_STATIC inline

/**
 * @def KOMIHASH_INLINE_F
 * @brief Macro that forces function inlining.
 */

#if defined( KOMIHASH_GCC_BUILTINS )

	#define KOMIHASH_INLINE_F KOMIHASH_INLINE __attribute__((always_inline))

#elif defined( _MSC_VER )

	#define KOMIHASH_INLINE_F KOMIHASH_STATIC __forceinline

#else // defined( _MSC_VER )

	#define KOMIHASH_INLINE_F KOMIHASH_INLINE

#endif // defined( _MSC_VER )

#if defined( KOMIHASH_NS )

namespace KOMIHASH_NS {

using std::memcpy;
using std::size_t;

#if __cplusplus >= 201103L

	using uint8_t = unsigned char; ///< For C++ type aliasing compliance.
	using std::uint32_t;
	using std::uint64_t;

#endif // __cplusplus >= 201103L

#endif // defined( KOMIHASH_NS )

/**
 * @brief Determines the platform's endianness at runtime.
 *
 * Note that modern compilers evaluate this function at compile time,
 * resulting in branch elimination.
 *
 * @return 1 if the platform is little-endian, 0 otherwise.
 */

KOMIHASH_INLINE_F int kh_is_little_endian(void) KOMIHASH_NOEXC
{
	static const uint32_t val = 0x04030201;

	unsigned char lsb;
	memcpy( &lsb, &val, 1 );

	return( lsb == 1 );
}

/**
 * @{
 * @brief Loads an unsigned value of the corresponding bit size, with
 * endianness correction.
 *
 * This is an auxiliary function that returns an unsigned value created from a
 * sequence of bytes in memory. This function is used to correct the
 * endianness of in-memory unsigned values and to avoid unaligned memory
 * accesses.
 *
 * @param p Pointer to bytes in memory. Alignment is unimportant.
 * @return The endianness-corrected value from memory (as `uint64_t`).
 */

KOMIHASH_INLINE_F uint64_t kh_lu32ec( const uint8_t* const p ) KOMIHASH_NOEXC
{
#if defined( KOMIHASH_EC32 )

	uint32_t v;
	memcpy( &v, p, 4 );

	return( KOMIHASH_EC32( v ));

#else // defined( KOMIHASH_EC32 )

	return( (uint64_t) p[ 0 ] | (uint64_t) p[ 1 ] << 8 |
		(uint64_t) p[ 2 ] << 16 | (uint64_t) p[ 3 ] << 24 );

#endif // defined( KOMIHASH_EC32 )
}

KOMIHASH_INLINE_F uint64_t kh_lu64ec( const uint8_t* const p ) KOMIHASH_NOEXC
{
#if defined( KOMIHASH_EC64 )

	uint64_t v;
	memcpy( &v, p, 8 );

	return( KOMIHASH_EC64( v ));

#else // defined( KOMIHASH_EC64 )

	return( kh_lu32ec( p ) | kh_lu32ec( p + 4 ) << 32 );

#endif // defined( KOMIHASH_EC64 )
}

/** @} */

/**
 * @def KOMIHASH_M128_IMPL
 * @brief Auxiliary macro for the kh_m128() implementation.
 */

/**
 * @def KOMIHASH_EMULU( u, v )
 * @brief Auxiliary macro for the `__emulu()` intrinsic.
 *
 * @param u The first multiplier.
 * @param v The second multiplier.
 */

#if defined( KOMIHASH_BMI2 )

	#define KOMIHASH_M128_IMPL \
		unsigned long long rh; \
		*rl = _mulx_u64( u, v, &rh );

#elif defined( _MSC_VER ) && \
	( defined( _M_ARM64 ) || defined( _M_ARM64EC ) || \
	( defined( __INTEL_COMPILER ) && defined( _M_AMD64 )))

	#define KOMIHASH_M128_IMPL \
		const uint64_t rh = __umulh( u, v ); \
		*rl = u * v;

#elif defined( _MSC_VER ) && ( defined( _M_AMD64 ) || defined( _M_IA64 ))

	#pragma intrinsic(_umul128)

	#define KOMIHASH_M128_IMPL \
		uint64_t rh; \
		*rl = _umul128( u, v, &rh );

#elif defined( __SIZEOF_INT128__ ) || \
	( defined( KOMIHASH_ICC_GCC ) && defined( __x86_64__ ))

	#define KOMIHASH_M128_IMPL \
		__uint128_t r = u; \
		r *= v; \
		const uint64_t rh = (uint64_t) ( r >> 64 ); \
		*rl = (uint64_t) r;

#elif ( defined( __IBMC__ ) || defined( __IBMCPP__ )) && defined( __LP64__ )

	#define KOMIHASH_M128_IMPL \
		const uint64_t rh = __mulhdu( u, v ); \
		*rl = u * v;

#else // defined( __IBMC__ )

	#if defined( _MSC_VER ) && !defined( __INTEL_COMPILER ) && \
		!defined( _M_ARM )

		#pragma intrinsic(__emulu)

		#define KOMIHASH_EMULU( u, v ) __emulu( u, v )

	#else // __emulu

		#define KOMIHASH_EMULU( u, v ) ( (uint64_t) ( u ) * ( v ))

	#endif // __emulu

#endif // defined( __IBMC__ )

/**
 * @brief 64-bit by 64-bit unsigned multiplication with result accumulation.
 *
 * @param u The first multiplier.
 * @param v The second multiplier.
 * @param[out] rl The lower half of the 128-bit result.
 * @param[in,out] rha The accumulator that receives the higher half of the
 * 128-bit result.
 */

#if defined( KOMIHASH_M128_IMPL )
KOMIHASH_INLINE_F
#else // defined( KOMIHASH_M128_IMPL )
KOMIHASH_INLINE
#endif // defined( KOMIHASH_M128_IMPL )

void kh_m128( const uint64_t u, const uint64_t v,
	uint64_t* const rl, uint64_t* const rha ) KOMIHASH_NOEXC
{
#if defined( KOMIHASH_M128_IMPL )

	KOMIHASH_M128_IMPL

#else // defined( KOMIHASH_M128_IMPL )

	// This is the `_umul128()` code for 32-bit systems, adapted from
	// Hacker's Delight by Henry S. Warren, Jr.

	*rl = u * v;

	const uint32_t u0 = (uint32_t) u;
	const uint32_t v0 = (uint32_t) v;
	const uint64_t w0 = KOMIHASH_EMULU( u0, v0 );
	const uint32_t u1 = (uint32_t) ( u >> 32 );
	const uint32_t v1 = (uint32_t) ( v >> 32 );
	const uint64_t t = KOMIHASH_EMULU( u1, v0 ) + (uint32_t) ( w0 >> 32 );
	const uint64_t w1 = KOMIHASH_EMULU( u0, v1 ) + (uint32_t) t;

	const uint64_t rh = KOMIHASH_EMULU( u1, v1 ) + (uint32_t) ( w1 >> 32 ) +
		(uint32_t) ( t >> 32 );

#endif // defined( KOMIHASH_M128_IMPL )

	*rha += rh;
}

/**
 * @def KOMIHASH_HASHROUND()
 * @brief Macro for a common hashing round without input data.
 */

#define KOMIHASH_HASHROUND() \
	kh_m128( Seed1, Seed5, &Seed1, &Seed5 ); \
	Seed1 ^= Seed5

/**
 * @def KOMIHASH_HASH16( m )
 * @brief Macro for a common hashing round with a 16-byte input.
 *
 * @param m Message pointer; alignment is unimportant.
 */

#define KOMIHASH_HASH16( m ) \
	kh_m128( kh_lu64ec( m ) ^ Seed1, \
		kh_lu64ec( m + 8 ) ^ Seed5, &Seed1, &Seed5 ); \
	Seed1 ^= Seed5

/**
 * @def KOMIHASH_HASHFIN()
 * @brief Macro for a common hashing finalization round.
 *
 * The final input to the hash function is expected to reside in the temporary
 * variables `r1h` and `r2h`. The macro includes the return statement.
 */

#define KOMIHASH_HASHFIN() \
	kh_m128( r1h, r2h, &Seed1, &Seed5 ); \
	Seed1 ^= Seed5; \
	KOMIHASH_HASHROUND(); \
	return( Seed1 )

/**
 * @def KOMIHASH_HASHLOOP64()
 * @brief Macro for a common 64-byte full-performance hashing loop.
 *
 * This macro expects `Msg` to point to the data and `MsgLen` to be greater
 * than 63, and requires `Seed1` through `Seed8` to be initialized.
 *
 * The "shifting" arrangement of the `Seed1` to `Seed4` XOR operations (below)
 * does not increase the PRNG period of individual `SeedN` values, but reduces
 * the chance of occasional synchronization between PRNG lanes. In practice,
 * `Seed1` to `Seed4` together become a single "fused" 256-bit PRNG value,
 * which gives the state a total PRNG period of 2^66.
 */

#define KOMIHASH_HASHLOOP64() \
	do KOMIHASH_LIKELY_DO \
	{ \
		kh_m128( kh_lu64ec( Msg ) ^ Seed1, \
			kh_lu64ec( Msg + 32 ) ^ Seed5, &Seed1, &Seed5 ); \
	\
		kh_m128( kh_lu64ec( Msg + 8 ) ^ Seed2, \
			kh_lu64ec( Msg + 40 ) ^ Seed6, &Seed2, &Seed6 ); \
	\
		kh_m128( kh_lu64ec( Msg + 16 ) ^ Seed3, \
			kh_lu64ec( Msg + 48 ) ^ Seed7, &Seed3, &Seed7 ); \
	\
		kh_m128( kh_lu64ec( Msg + 24 ) ^ Seed4, \
			kh_lu64ec( Msg + 56 ) ^ Seed8, &Seed4, &Seed8 ); \
	\
		Msg += 64; \
		MsgLen -= 64; \
	\
		KOMIHASH_PREFETCH( Msg ); \
	\
		Seed4 ^= Seed7; \
		Seed1 ^= Seed8; \
		Seed2 ^= Seed5; \
		Seed3 ^= Seed6; \
	\
	} while KOMIHASH_LIKELY_DO_EXPR( MsgLen > 63 )

/**
 * @brief The hashing epilogue function (for internal use).
 *
 * @param Msg Pointer to the remaining part of the message. It is assumed that
 * the original message is "long" so that `Msg + MsgLen - 8` does not point
 * beyond the original message.
 * @param MsgLen Length of the remaining part; can be zero.
 * @param Seed1 The latest `Seed1` value.
 * @param Seed5 The latest `Seed5` value.
 * @return The 64-bit hash value.
 */

KOMIHASH_INLINE_F uint64_t komihash_epi( const uint8_t* Msg, size_t MsgLen,
	uint64_t Seed1, uint64_t Seed5 ) KOMIHASH_NOEXC
{
	uint64_t r1h, r2h;

	if( MsgLen > 31 )
	{
		KOMIHASH_HASH16( Msg );
		KOMIHASH_HASH16( Msg + 16 );

		MsgLen -= 32;
		Msg += 32;
	}

	if( MsgLen > 15 )
	{
		KOMIHASH_HASH16( Msg );

		MsgLen -= 16;
		Msg += 16;
	}

	size_t ml8 = MsgLen * 8;

	if( MsgLen < 8 )
	{
		ml8 ^= 56;
		r1h = kh_lu64ec( Msg + MsgLen - 8 ) >> 8 | (uint64_t) 1 << 56;
		r2h = Seed5;
		r1h = ( r1h >> ml8 ) ^ Seed1;
	}
	else
	{
		r2h = kh_lu64ec( Msg + MsgLen - 8 ) >> 8 | (uint64_t) 1 << 56;
		ml8 ^= 120;
		r1h = kh_lu64ec( Msg ) ^ Seed1;
		r2h = ( r2h >> ml8 ) ^ Seed5;
	}

	KOMIHASH_HASHFIN();
}

/**
 * @brief Implementation of the KOMIHASH 64-bit hash function.
 *
 * This function produces and returns a 64-bit hash value of the specified
 * message, string, or binary data block. It is designed for hash tables and
 * hash maps and can also be used to generate checksums. It produces identical
 * hashes across big- and little-endian systems.
 *
 * @param Msg The message to hash.
 * @param MsgLen The message length, in bytes; can be zero.
 * @param Seed1 The initial `Seed1` value.
 * @param Seed5 The initial `Seed5` value.
 * @return The 64-bit hash of the input data.
 */

KOMIHASH_INLINE uint64_t komihash_inner( const uint8_t* Msg, size_t MsgLen,
	uint64_t Seed1, uint64_t Seed5 ) KOMIHASH_NOEXC
{
	uint64_t r1h, r2h;

	if KOMIHASH_LIKELY( MsgLen < 16 )
	{
		r1h = Seed1;
		r2h = Seed5;

		if( MsgLen > 7 )
		{
			// The following XOR operations are equivalent to mixing the
			// message with a cryptographic one-time pad (bitwise addition
			// modulo 2). The message's statistics and distribution are thus
			// unimportant.

			r1h ^= kh_lu64ec( Msg );

			size_t ml8 = MsgLen * 8;

			if( MsgLen < 12 )
			{
				ml8 ^= 88;
				const uint64_t m = (uint64_t) Msg[ MsgLen - 3 ] |
					(uint64_t) Msg[ MsgLen - 1 ] << 16 | (uint64_t) 1 << 24 |
					(uint64_t) Msg[ MsgLen - 2 ] << 8;

				r2h ^= m >> ml8;
			}
			else
			{
				const size_t mhs = 128 - ml8;
				const uint64_t mh = ( kh_lu32ec( Msg + MsgLen - 4 ) |
					(uint64_t) 1 << 32 ) >> mhs;

				const uint64_t ml = kh_lu32ec( Msg + 8 );

				r2h ^= mh << 32 | ml;
			}
		}
		else
		if KOMIHASH_LIKELY( MsgLen != 0 )
		{
			const size_t ml8 = MsgLen * 8;

			if( MsgLen < 4 )
			{
				r1h ^= (uint64_t) Msg[ 0 ];
				r1h ^= (uint64_t) 1 << ml8;

				if( MsgLen != 1 )
				{
					r1h ^= (uint64_t) Msg[ 1 ] << 8;

					if( MsgLen != 2 )
					{
						r1h ^= (uint64_t) Msg[ 2 ] << 16;
					}
				}
			}
			else
			{
				const size_t mhs = 64 - ml8;
				const uint64_t mh = ( kh_lu32ec( Msg + MsgLen - 4 ) |
					(uint64_t) 1 << 32 ) >> mhs;

				const uint64_t ml = kh_lu32ec( Msg );

				r1h ^= mh << 32 | ml;
			}
		}
	}
	else
	{
		if KOMIHASH_UNLIKELY( MsgLen > 31 )
		{
			goto longmsg;
		}

		KOMIHASH_HASH16( Msg );

		size_t ml8 = MsgLen * 8;

		if( MsgLen < 24 )
		{
			ml8 ^= 184;
			r1h = kh_lu64ec( Msg + MsgLen - 8 ) >> 8 | (uint64_t) 1 << 56;
			r2h = Seed5;
			r1h = ( r1h >> ml8 ) ^ Seed1;

			KOMIHASH_HASHFIN();
		}
		else
		{
			r2h = kh_lu64ec( Msg + MsgLen - 8 ) >> 8 | (uint64_t) 1 << 56;
			ml8 ^= 248;
			r1h = kh_lu64ec( Msg + 16 ) ^ Seed1;
			r2h = ( r2h >> ml8 ) ^ Seed5;
		}
	}

	KOMIHASH_HASHFIN();

longmsg:
	if KOMIHASH_LIKELY( MsgLen > 63 )
	{
		uint64_t Seed2 = KOMIHASH_IVAL2 ^ Seed1;
		uint64_t Seed3 = KOMIHASH_IVAL3 ^ Seed1;
		uint64_t Seed4 = KOMIHASH_IVAL4 ^ Seed1;
		uint64_t Seed6 = KOMIHASH_IVAL6 ^ Seed5;
		uint64_t Seed7 = KOMIHASH_IVAL7 ^ Seed5;
		uint64_t Seed8 = KOMIHASH_IVAL8 ^ Seed5;

		KOMIHASH_HASHLOOP64();

		Seed5 ^= Seed6 ^ Seed7 ^ Seed8;
		Seed1 ^= Seed2 ^ Seed3 ^ Seed4;
	}

	return( komihash_epi( Msg, MsgLen, Seed1, Seed5 ));
}

/**
 * @brief Context structure for hash function pre-seeding.
 *
 * The komihash_set_preseed() function should be called to initialize the
 * structure before hashing.
 */

typedef struct {
	uint64_t Seed1; ///< The initial Seed1 value.
	uint64_t Seed5; ///< The initial Seed5 value.
} komihash_preseed_t;

/**
 * @brief Implements pre-seeding of the hash function state.
 *
 * @param[out] outSeed1 The resulting `Seed1` value to be used by the hash
 * function.
 * @param[out] outSeed5 The resulting `Seed5` value to be used by the hash
 * function.
 * @param UseSeed Seed value.
 */

KOMIHASH_INLINE_F void komihash_set_preseed_inner( uint64_t* const outSeed1,
	uint64_t* const outSeed5, const uint64_t UseSeed ) KOMIHASH_NOEXC
{
	uint64_t Seed1 = KOMIHASH_IVAL1 ^ ( UseSeed & KOMIHASH_VAL01 );
	uint64_t Seed5 = KOMIHASH_IVAL5 ^ ( UseSeed & KOMIHASH_VAL10 );

	KOMIHASH_HASHROUND(); // Required for Perlin noise hashing.

	*outSeed1 = Seed1;
	*outSeed5 = Seed5;
}

/**
 * @brief Performs hash function state pre-seeding.
 *
 * @param[out] ps The pre-seeding context structure.
 * @param UseSeed Seed value. To use the default seed, set this value to 0.
 * This value can have any number of significant bits and any statistical
 * quality. It may need endianness correction if shared between big- and
 * little-endian systems.
 */

KOMIHASH_INLINE_F void komihash_set_preseed( komihash_preseed_t* const ps,
	const uint64_t UseSeed ) KOMIHASH_NOEXC
{
	komihash_set_preseed_inner( &ps -> Seed1, &ps -> Seed5, UseSeed );
}

/**
 * @brief The KOMIHASH 64-bit hash function.
 *
 * This function produces and returns a 64-bit hash value of the specified
 * message, string, or binary data block. It is designed for hash tables and
 * hash maps and can also be used to generate checksums. It produces identical
 * hashes across big- and little-endian systems.
 *
 * @param Msg0 The message to hash. The alignment of this pointer is
 * unimportant. It is valid to pass a null pointer when `MsgLen` equals 0
 * (assuming that the compiler's implementation of address prefetching is
 * non-faulting, according to the GCC specification).
 * @param MsgLen The message length, in bytes; can be zero.
 * @param UseSeed Optional value to use instead of the default seed. To use
 * the default seed, set this value to 0. This value can have any number of
 * significant bits and any statistical quality. It may need endianness
 * correction if shared between big- and little-endian systems.
 * @return The 64-bit hash of the input data. It should be corrected for
 * endianness when shared between big- and little-endian systems.
 */

KOMIHASH_INLINE_F uint64_t komihash( const void* const Msg0,
	size_t MsgLen, const uint64_t UseSeed ) KOMIHASH_NOEXC
{
	const uint8_t* Msg = (const uint8_t*) Msg0;

	KOMIHASH_PREFETCH( Msg );

	uint64_t Seed1, Seed5;
	komihash_set_preseed_inner( &Seed1, &Seed5, UseSeed );

	return( komihash_inner( Msg, MsgLen, Seed1, Seed5 ));
}

/**
 * @brief The KOMIHASH 64-bit hash function with pre-seeding.
 *
 * This is a faster, statically pre-seeded hash function. @see komihash() for
 * details.
 *
 * @param Msg0 The message to hash.
 * @param MsgLen The message length, in bytes; can be zero.
 * @param ps Pre-seeding context structure, which must be initialized by the
 * komihash_set_preseed() function.
 * @return The 64-bit hash of the input data.
 */

KOMIHASH_INLINE_F uint64_t komihash_with_preseed( const void* const Msg0,
	const size_t MsgLen, const komihash_preseed_t* const ps ) KOMIHASH_NOEXC
{
	const uint8_t* Msg = (const uint8_t*) Msg0;

	KOMIHASH_PREFETCH( Msg );

	return( komihash_inner( Msg, MsgLen, ps -> Seed1, ps -> Seed5 ));
}

/**
 * @brief The KOMIHASH 64-bit hash function without a seed.
 *
 * This is a faster hash function without a seed argument (using the default
 * seed of 0). @see komihash() for details.
 *
 * @param Msg0 The message to hash.
 * @param MsgLen The message length, in bytes; can be zero.
 * @return The 64-bit hash of the input data.
 */

KOMIHASH_INLINE_F uint64_t komihash_seedless( const void* const Msg0,
	const size_t MsgLen ) KOMIHASH_NOEXC
{
	const uint8_t* Msg = (const uint8_t*) Msg0;

	KOMIHASH_PREFETCH( Msg );

	return( komihash_inner( Msg, MsgLen,
		KOMIHASH_DEFSEED1, KOMIHASH_DEFSEED5 ));
}

/**
 * @brief The KOMIRAND 64-bit pseudorandom number generator.
 *
 * This is a simple, reliable, self-starting, and efficient PRNG with a period
 * of 2^64. Its performance is 0.62 cycles/byte. It self-starts within 4
 * iterations that constitute the suggested "warm-up" period (needed before
 * using the output if the seeds are initialized with an arbitrary value).
 *
 * If the seeds are initialized with a high-quality, uniformly random value
 * (e.g., from the operating system's entropy pool or the output of a hash
 * function), the PRNG produces valid output from the start.
 *
 * Note that although this PRNG is classified as "chaotic", one should not
 * assume that it can enter degenerate cycles. When properly initialized, it
 * does not exhibit degenerate cycles, regardless of the initial state.
 *
 * @param[in,out] Seed1 Seed value 1. Can be initialized to any value
 * (even 0). This is the usual "PRNG seed" value.
 * @param[in,out] Seed2 Seed value 2, a supporting variable. It must be
 * initialized to the same value as `Seed1`. It should not be used as the PRNG
 * output.
 * @return The next uniformly random 64-bit value.
 */

KOMIHASH_INLINE_F uint64_t komirand( uint64_t* const Seed1,
	uint64_t* const Seed2 ) KOMIHASH_NOEXC
{
	uint64_t s1 = *Seed1;
	uint64_t s2 = *Seed2;
	uint64_t rh = 0;

	// The three instructions performed by this function (multiplication,
	// addition, and XOR) represent the simplest constant-free PRNG that works
	// with state variables of any even bit width, where `Seed1` serves as the
	// PRNG output (the PRNG has a period of 2^64). It passes `PractRand`
	// tests with only rare, non-systematic "unusual" assessments.
	//
	// To make this PRNG reliable and self-starting, and to eliminate the risk
	// of stalling, the "register checkerboard" constants are added - a source
	// of raw entropy. These constants are not required for hashing (but work
	// in that context), since input entropy is abundantly available during
	// hashing. Besides that, to minimize the risk of stalling, the hashing
	// uses an adjusted construction of this PRNG.
	//
	// (The `0x5555` and `0xAAAA...` constants should match the width of the
	// register; essentially, they repeat the `01` and `10` bit pairs; they
	// are not arbitrary constants.)

	kh_m128( s1, s2, &s1, &rh );
	s2 += rh;
	s1 ^= rh;

	s2 += KOMIHASH_VAL10;
	s1 += KOMIHASH_VAL01;

	*Seed2 = s2;
	*Seed1 = s1;

	return( s1 );
}

/**
 * @def KOMIHASH_BUFSIZE
 * @brief Streamed hashing buffer size, in bytes.
 *
 * It must be a multiple of 64 and not less than 128. It can be defined
 * externally.
 */

#if !defined( KOMIHASH_BUFSIZE )
	#define KOMIHASH_BUFSIZE 768
#endif // !defined( KOMIHASH_BUFSIZE )

#if KOMIHASH_BUFSIZE < 128
	#error KOMIHASH_BUFSIZE must be at least 128.
#endif // KOMIHASH_BUFSIZE < 128

#if ( KOMIHASH_BUFSIZE % 64 ) != 0
	#error KOMIHASH_BUFSIZE must be a multiple of 64.
#endif // ( KOMIHASH_BUFSIZE % 64 ) != 0

/**
 * @brief Context structure for streamed `komihash` hashing.
 *
 * The komihash_stream_init() function should be called to initialize the
 * structure before hashing. Note that the default buffer size is modest,
 * which allows this structure to be placed on the stack. `Seed[ 0 ]` is used
 * to store the `UseSeed` value.
 */

typedef struct {
	uint8_t Buf[ 8 + KOMIHASH_BUFSIZE ]; ///< Buffer including padding bytes.
	uint64_t Seed[ 8 ]; ///< Hashing state variables.
	size_t BufFill; ///< Buffer fill count (position), in bytes.
	size_t IsHashing; ///< Flag (0 or 1); equals 1 if hashing has started.
} komihash_stream_t;

/**
 * @brief Initializes a streamed `komihash` hashing session.
 *
 * @param[out] ctx Pointer to the context structure.
 * @param UseSeed Optional value to use instead of the default seed. To use
 * the default seed, set this value to 0. This value can have any number of
 * significant bits and any statistical quality. It may need endianness
 * correction if shared between big- and little-endian systems.
 */

KOMIHASH_INLINE void komihash_stream_init( komihash_stream_t* const ctx,
	const uint64_t UseSeed ) KOMIHASH_NOEXC
{
	ctx -> Seed[ 0 ] = UseSeed;
	ctx -> BufFill = 0;
	ctx -> IsHashing = 0;
}

/**
 * @brief Updates the streamed hashing state with new input data.
 *
 * @param[in,out] ctx Pointer to the context structure. The structure must be
 * initialized by calling the komihash_stream_init() function.
 * @param Msg0 The next part of the message being hashed. The alignment of
 * this pointer is unimportant. It is valid to pass a null pointer when
 * `MsgLen` equals 0.
 * @param MsgLen The message length, in bytes; can be zero.
 */

KOMIHASH_INLINE void komihash_stream_update( komihash_stream_t* const ctx,
	const void* const Msg0, size_t MsgLen ) KOMIHASH_NOEXC
{
	const uint8_t* Msg = (const uint8_t*) Msg0;

	const uint8_t* SwMsg = Msg;
	size_t SwMsgLen = 0;
	size_t BufFill = ctx -> BufFill;

	if( MsgLen >= KOMIHASH_BUFSIZE - BufFill && BufFill != 0 )
	{
		const size_t CopyLen = KOMIHASH_BUFSIZE - BufFill;
		memcpy( ctx -> Buf + 8 + BufFill, Msg, CopyLen );
		BufFill = 0;

		SwMsg += CopyLen;
		SwMsgLen = MsgLen - CopyLen;

		Msg = ctx -> Buf + 8;
		MsgLen = KOMIHASH_BUFSIZE;
	}

	if( BufFill == 0 )
	{
		while( MsgLen > 127 )
		{
			uint64_t Seed1, Seed2, Seed3, Seed4;
			uint64_t Seed5, Seed6, Seed7, Seed8;

			KOMIHASH_PREFETCH( Msg );

			if( ctx -> IsHashing )
			{
				Seed1 = ctx -> Seed[ 0 ];
				Seed2 = ctx -> Seed[ 1 ];
				Seed3 = ctx -> Seed[ 2 ];
				Seed4 = ctx -> Seed[ 3 ];
				Seed5 = ctx -> Seed[ 4 ];
				Seed6 = ctx -> Seed[ 5 ];
				Seed7 = ctx -> Seed[ 6 ];
				Seed8 = ctx -> Seed[ 7 ];
			}
			else
			{
				ctx -> IsHashing = 1;

				komihash_set_preseed_inner( &Seed1, &Seed5, ctx -> Seed[ 0 ]);

				Seed2 = KOMIHASH_IVAL2 ^ Seed1;
				Seed3 = KOMIHASH_IVAL3 ^ Seed1;
				Seed4 = KOMIHASH_IVAL4 ^ Seed1;
				Seed6 = KOMIHASH_IVAL6 ^ Seed5;
				Seed7 = KOMIHASH_IVAL7 ^ Seed5;
				Seed8 = KOMIHASH_IVAL8 ^ Seed5;
			}

			KOMIHASH_HASHLOOP64();

			ctx -> Seed[ 0 ] = Seed1;
			ctx -> Seed[ 1 ] = Seed2;
			ctx -> Seed[ 2 ] = Seed3;
			ctx -> Seed[ 3 ] = Seed4;
			ctx -> Seed[ 4 ] = Seed5;
			ctx -> Seed[ 5 ] = Seed6;
			ctx -> Seed[ 6 ] = Seed7;
			ctx -> Seed[ 7 ] = Seed8;

			if( SwMsgLen == 0 )
			{
				if( MsgLen != 0 )
				{
					break;
				}

				ctx -> BufFill = 0;
				return;
			}

			Msg = SwMsg;
			MsgLen = SwMsgLen;
			SwMsgLen = 0;
		}
	}

	ctx -> BufFill = BufFill + MsgLen;
	uint8_t* op = ctx -> Buf + 8 + BufFill;

	while( MsgLen != 0 )
	{
		*op = *Msg;
		Msg++;
		op++;
		MsgLen--;
	}
}

/**
 * @brief Finalizes a streamed `komihash` hashing session.
 *
 * This function returns the hash value of the previously hashed data. This
 * value is equal to the value returned by the komihash() function for all of
 * the input data.
 *
 * Since this function does not destructively alter the context structure,
 * it can be used to obtain intermediate hashes of the data stream being
 * hashed, and hashing can then be resumed.
 *
 * @param[in] ctx Pointer to the context structure. The structure must be
 * initialized by calling the komihash_stream_init() function.
 * @return The 64-bit hash value. It should be corrected for endianness when
 * shared between big- and little-endian systems.
 */

KOMIHASH_INLINE uint64_t komihash_stream_final( komihash_stream_t* const ctx )
	KOMIHASH_NOEXC
{
	const uint8_t* Msg = ctx -> Buf + 8;
	size_t MsgLen = ctx -> BufFill;

	if( ctx -> IsHashing == 0 )
	{
		return( komihash( Msg, MsgLen, ctx -> Seed[ 0 ]));
	}

	const uint64_t zv = 0;
	memcpy( ctx -> Buf, &zv, 8 );

	uint64_t Seed1 = ctx -> Seed[ 0 ];
	uint64_t Seed2 = ctx -> Seed[ 1 ];
	uint64_t Seed3 = ctx -> Seed[ 2 ];
	uint64_t Seed4 = ctx -> Seed[ 3 ];
	uint64_t Seed5 = ctx -> Seed[ 4 ];
	uint64_t Seed6 = ctx -> Seed[ 5 ];
	uint64_t Seed7 = ctx -> Seed[ 6 ];
	uint64_t Seed8 = ctx -> Seed[ 7 ];

	if( MsgLen > 63 )
	{
		KOMIHASH_HASHLOOP64();
	}

	Seed5 ^= Seed6 ^ Seed7 ^ Seed8;
	Seed1 ^= Seed2 ^ Seed3 ^ Seed4;

	return( komihash_epi( Msg, MsgLen, Seed1, Seed5 ));
}

/**
 * @brief FOR TESTING PURPOSES ONLY: Use the komihash() function instead.
 *
 * @param Msg The message to hash.
 * @param MsgLen The message length, in bytes.
 * @param UseSeed The seed to use.
 * @return The 64-bit hash value.
 */

KOMIHASH_INLINE uint64_t komihash_stream_oneshot( const void* const Msg,
	const size_t MsgLen, const uint64_t UseSeed ) KOMIHASH_NOEXC
{
	komihash_stream_t ctx;

	komihash_stream_init( &ctx, UseSeed );
	komihash_stream_update( &ctx, Msg, MsgLen );

	return( komihash_stream_final( &ctx ));
}

#if defined( KOMIHASH_NS )

} // namespace KOMIHASH_NS

#if !defined( KOMIHASH_NS_CUSTOM )

namespace {

using KOMIHASH_NS::komihash_preseed_t;
using KOMIHASH_NS::komihash_set_preseed;
using KOMIHASH_NS::komihash;
using KOMIHASH_NS::komihash_with_preseed;
using KOMIHASH_NS::komihash_seedless;
using KOMIHASH_NS::komirand;
using KOMIHASH_NS::komihash_stream_t;
using KOMIHASH_NS::komihash_stream_init;
using KOMIHASH_NS::komihash_stream_update;
using KOMIHASH_NS::komihash_stream_final;
using KOMIHASH_NS::komihash_stream_oneshot;

} // namespace

#endif // !defined( KOMIHASH_NS_CUSTOM )

#endif // defined( KOMIHASH_NS )

// Macro definitions for Doxygen.

#if !defined( KOMIHASH_NS_CUSTOM )
	#define KOMIHASH_NS_CUSTOM
	#undef KOMIHASH_NS_CUSTOM
#endif // !defined( KOMIHASH_NS_CUSTOM )

#if defined( KOMIHASH_ENDIAN_DEFS )
	#undef KOMIHASH_LITTLE_ENDIAN
	#undef KOMIHASH_COND_EC
	#undef KOMIHASH_ENDIAN_DEFS
#endif // defined( KOMIHASH_ENDIAN_DEFS )

#undef KOMIHASH_NS
#undef KOMIHASH_U64_C
#undef KOMIHASH_NOEXC
#undef KOMIHASH_IVAL1
#undef KOMIHASH_IVAL2
#undef KOMIHASH_IVAL3
#undef KOMIHASH_IVAL4
#undef KOMIHASH_IVAL5
#undef KOMIHASH_IVAL6
#undef KOMIHASH_IVAL7
#undef KOMIHASH_IVAL8
#undef KOMIHASH_VAL01
#undef KOMIHASH_VAL10
#undef KOMIHASH_DEFSEED1
#undef KOMIHASH_DEFSEED5
#undef KOMIHASH_ICC_GCC
#undef KOMIHASH_GCC_BUILTINS
#undef KOMIHASH_BMI2
#undef KOMIHASH_EC32
#undef KOMIHASH_EC64
#undef KOMIHASH_LIKELY
#undef KOMIHASH_UNLIKELY
#undef KOMIHASH_LIKELY_DO
#undef KOMIHASH_LIKELY_DO_EXPR
#undef KOMIHASH_PREFETCH
#undef KOMIHASH_STATIC
#undef KOMIHASH_INLINE
#undef KOMIHASH_INLINE_F
#undef KOMIHASH_M128_IMPL
#undef KOMIHASH_EMULU
#undef KOMIHASH_HASHROUND
#undef KOMIHASH_HASH16
#undef KOMIHASH_HASHFIN
#undef KOMIHASH_HASHLOOP64

#endif // KOMIHASH_INCLUDED
/* <<<< komihash.h 5.34 verbatim ends on the previous line */
