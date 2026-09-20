/*
 * Reproduction driver and C transcription: Copyright (c) 2026 Thomas Dybdahl Ahle.
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
/* Marvin32, the hash behind .NET string.GetHashCode(): transcribed line by line from
 * dotnet/runtime src/libraries/System.Private.CoreLib/src/System/Marvin.cs at tag v10.0.12
 * (commit 4271d88e; the file is vendored beside this program as Marvin.cs, sha256
 * a713890909dc99fe5f57880b6d997a8b26e78b1b75399f0dfad075bd29da4c14).  Upstream notice:
 * "Licensed to the .NET Foundation under one or more agreements. The .NET Foundation licenses
 * this file to you under the MIT license."  The transcription keeps ComputeHash32's control flow
 * (main 8-byte loop, 4..7-byte tail, overlapping last-4-byte read carrying the 0x80 pad byte,
 * 0..3-byte path) and Block (xor, rotl 20, add, rotl 9, xor, rotl 27, add, rotl 19); the
 * little-endian branches are the ones taken on x86-64 and arm64.
 *
 * Seed protocol (String.Comparison.cs, GetHashCode()):
 *   ulong seed = Marvin.DefaultSeed;                        // 64 random bits, drawn once per process
 *   Marvin.ComputeHash32(ref _firstChar as byte, 2 * Length, (uint)seed, (uint)(seed >> 32));
 * so the seed is the initial state only: p0 = low 32 bits, p1 = high 32 bits, and the message is
 * the string's UTF-16LE code units.  RandomizedStringEqualityComparer (Dictionary<string,_> and
 * HashSet<string> after a 100-long bucket chain) calls the same function with its own 64-bit seed
 * drawn the same way.  This program draws 64-bit seeds from its RNG and splits them identically. */
#include <stdint.h>
#include <stddef.h>
#include <string.h>

static inline uint32_t mv_rotl(uint32_t v, int s) { return (v << s) | (v >> (32 - s)); }
static inline uint32_t mv_ld32(const uint8_t *p) {
    return (uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24;
}
static inline uint32_t mv_ld16(const uint8_t *p) { return (uint32_t)p[0] | (uint32_t)p[1] << 8; }

/* Block(ref p0, ref p1) */
static inline void mv_block(uint32_t *rp0, uint32_t *rp1) {
    uint32_t p0 = *rp0, p1 = *rp1;
    p1 ^= p0;  p0 = mv_rotl(p0, 20);
    p0 += p1;  p1 = mv_rotl(p1, 9);
    p1 ^= p0;  p0 = mv_rotl(p0, 27);
    p0 += p1;  p1 = mv_rotl(p1, 19);
    *rp0 = p0; *rp1 = p1;
}

/* ComputeHash32(ref byte data, uint count, uint p0, uint p1) up to the final "return (int)(p1 ^ p0)":
 * leaves the closing 64-bit state in *out0, *out1 so that full-state collisions can be counted. */
static void marvin_state(const uint8_t *data, uint32_t count, uint32_t p0, uint32_t p1,
                         uint32_t *out0, uint32_t *out1) {
    uint32_t partialResult;
    if (count < 8) {
        if (count >= 4) goto Between4And7BytesRemain;
        goto InputTooSmallToEnterMainLoop;
    }
    {
        uint32_t loopCount = count / 8;
        do {
            p0 += mv_ld32(data);
            uint32_t nextUInt32 = mv_ld32(data + 4);
            mv_block(&p0, &p1);
            p0 += nextUInt32;
            mv_block(&p0, &p1);
            data += 8;
        } while (--loopCount > 0);
    }
    if ((count & 4u) == 0) goto DoFinalPartialRead;
Between4And7BytesRemain:
    p0 += mv_ld32(data);                 /* 'data' is not advanced, as in the C# */
    mv_block(&p0, &p1);
DoFinalPartialRead:
    partialResult = mv_ld32(data + (count & 7u) - 4);   /* the last 4 bytes of the buffer */
    {
        uint32_t shift = (~count << 3) & 0x1Fu;
        partialResult >>= 8;             /* make room for the 0x80 byte */
        partialResult |= 0x80000000u;    /* put the 0x80 byte at the beginning */
        partialResult >>= shift;         /* shift out the bytes already consumed */
    }
    goto DoFinalRoundsAndReturn;
InputTooSmallToEnterMainLoop:
    partialResult = 0x80u;
    if (count & 1u) { partialResult = data[count & 2u]; partialResult |= 0x8000u; }
    if (count & 2u) { partialResult <<= 16; partialResult |= mv_ld16(data); }
DoFinalRoundsAndReturn:
    p0 += partialResult;
    mv_block(&p0, &p1);
    mv_block(&p0, &p1);
    *out0 = p0; *out1 = p1;
}

/* string.GetHashCode() value for the UTF-16LE bytes 'data' under the 64-bit DefaultSeed 'seed'. */
static inline uint32_t marvin32(const uint8_t *data, size_t count, uint64_t seed) {
    uint32_t a, b;
    marvin_state(data, (uint32_t)count, (uint32_t)seed, (uint32_t)(seed >> 32), &a, &b);
    return b ^ a;
}
/* Legacy corefx Marvin.ComputeHash 64-bit output ((long)p1 << 32 | p0), used only for its test vectors. */
static inline uint64_t marvin64_legacy(const uint8_t *data, size_t count, uint64_t seed) {
    uint32_t a, b;
    marvin_state(data, (uint32_t)count, (uint32_t)seed, (uint32_t)(seed >> 32), &a, &b);
    return (uint64_t)b << 32 | a;
}

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <errno.h>
#include <math.h>

/* ---- validation data --------------------------------------------------------------------- */

/* (a) dotnet/corefx src/Common/tests/Tests/System/MarvinTests.cs: 64-bit legacy output. */
typedef struct { uint64_t seed; const char *hex; uint64_t expected; } Kat64;
static const Kat64 corefx_vectors[] = {
 {0x4FB61A001BDBCCULL,"",0x30ED35C100CD3C7DULL},{0x4FB61A001BDBCCULL,"af",0x48E73FC77D75DDC1ULL},
 {0x4FB61A001BDBCCULL,"e70f",0xB5F6E1FC485DBFF8ULL},{0x4FB61A001BDBCCULL,"37f495",0xF0B07C789B8CF7E8ULL},
 {0x4FB61A001BDBCCULL,"8642dc59",0x7008F2E87E9CF556ULL},{0x4FB61A001BDBCCULL,"153fb79826",0xE6C08C6DA2AFA997ULL},
 {0x4FB61A001BDBCCULL,"0932e6246c47",0x6F04BF1A5EA24060ULL},{0x4FB61A001BDBCCULL,"ab427ea8d10fc7",0xE11847E4F0678C41ULL},
 {0x804FB61A001BDBCCULL,"",0x10A9D5D3996FD65DULL},{0x804FB61A001BDBCCULL,"af",0x68201F91960EBF91ULL},
 {0x804FB61A001BDBCCULL,"e70f",0x64B581631F6AB378ULL},{0x804FB61A001BDBCCULL,"37f495",0xE1F2DFA6E5131408ULL},
 {0x804FB61A001BDBCCULL,"8642dc59",0x36289D9654FB49F6ULL},{0x804FB61A001BDBCCULL,"153fb79826",0xA06114B13464DBDULL},
 {0x804FB61A001BDBCCULL,"0932e6246c47",0xD6DD5E40AD1BC2EDULL},{0x804FB61A001BDBCCULL,"ab427ea8d10fc7",0xE203987DBA252FB3ULL},
 {0x804FB61A801BDBCCULL,"00",0xA37FB0DA2ECAE06CULL},{0x804FB61A801BDBCCULL,"FF",0xFECEF370701AE054ULL},
 {0x804FB61A801BDBCCULL,"00FF",0xA638E75700048880ULL},{0x804FB61A801BDBCCULL,"FF00",0xBDFB46D969730E2AULL},
 {0x804FB61A801BDBCCULL,"FF00FF",0x9D8577C0FE0D30BFULL},{0x804FB61A801BDBCCULL,"00FF00",0x4F9FBDDE15099497ULL},
 {0x804FB61A801BDBCCULL,"00FF00FF",0x24EAA279D9A529CAULL},{0x804FB61A801BDBCCULL,"FF00FF00",0xD3BEC7726B057943ULL},
 {0x804FB61A801BDBCCULL,"FF00FF00FF",0x920B62BBCA3E0B72ULL},{0x804FB61A801BDBCCULL,"00FF00FF00",0x1D7DDF9DFDF3C1BFULL},
 {0x804FB61A801BDBCCULL,"00FF00FF00FF",0xEC21276A17E821A5ULL},{0x804FB61A801BDBCCULL,"FF00FF00FF00",0x6911A53CA8C12254ULL},
 {0x804FB61A801BDBCCULL,"FF00FF00FF00FF",0xFDFD187B1D3CE784ULL},{0x804FB61A801BDBCCULL,"00FF00FF00FF00",0x71876F2EFB1B0EE8ULL},
};

/* (b) .NET 10.0.12 runtime vectors printed by dotnet_check.cs (dotnet_check.txt): Marvin.ComputeHash32 at
 * 4 splitmix64 seeds x lengths 0..15; the seeds and message bytes are regenerated here from the same stream. */
static const uint32_t runtime_vectors[4][16] = {
 {0xc0464577,0xe056428d,0x792a20c2,0xc101b223,0x6ca3239e,0x959e3a8e,0xfea0e768,0x766aae7b,0x74021bcb,0x0712ccfd,0x69b9f825,0x6da3725d,0x736419bd,0x84aa62b6,0x00abbfc5,0x381921e9},
 {0x4eace609,0x908d4074,0xdd586409,0xd9b73293,0xe689a914,0xb01b3736,0x0e2f2671,0x1f2d158b,0x71915f85,0x9ade5504,0x5ff9da22,0x33d0543d,0xdafb5b39,0x65c7c713,0xde330a09,0xff1f5c83},
 {0x9372f869,0x9d9088b4,0x8d7cb51d,0x36368b73,0x1a3224b9,0x9595f484,0x7c9c4e55,0x4f72684b,0x6a0323be,0x334f0765,0x8f8af56f,0x5bba8be8,0x05a90590,0x7f199b87,0xf3c2dd61,0x8dc20e42},
 {0x3e937639,0x22de54e2,0xb893cff8,0x11c410b8,0xbf088f3e,0xe0162708,0x130dbb8f,0xe3ef2b16,0x5edf501d,0x0a1cb4d9,0x927f9cc7,0x91217425,0x9fc7f05d,0xd184872a,0xe64c3897,0xce7ce2f3},
};
static const uint64_t runtime_vector_stream = UINT64_C(0x4d617276696e3332);

/* ---- pairs -------------------------------------------------------------------------------- */

typedef struct {
    const char *name, *a, *b, *a_text, *b_text, *mechanism;
    uint64_t seed;        /* recorded colliding seed */
    uint32_t expected;    /* GetHashCode of both strings under it */
} Pair;
static const Pair pairs[] = {
    {"A", "610061002f546d1662006200", "610061802f9579a262fc6100",
     "\"aa\" U+542F U+166D \"bb\"", "\"a\" U+8061 U+952F U+A279 U+FC62 \"a\"",
     "  mechanism: LE words m = 00610061 166d542f 00620062, m' = 80610061 a279952f 0061fc62, additive\n"
     "  word differences 0x80000000, 0x8c0c4100, -0x400.  After Block 1 the state difference is\n"
     "  (84084100, 08040040); adding the second difference leaves (08040000, 08040040), which\n"
     "  cancels inside Block 2 for about one seed in 480; the third difference removes the last bit.\n"
     "  The 64-bit state then agrees and every later step is keyless, so the hash codes agree.",
     UINT64_C(0x9bcb44c8bff5b6f1), 0x7eec677cu},
    {"B", "6100610061006100", "7211721150ef4fef",
     "\"aaaa\"", "U+1172 U+1172 U+EF50 U+EF4F",
     "  mechanism: one-word pair, word differences (+0x11111111, -0x11111111), a period-4 pattern\n"
     "  fixed by rotl 20; cancels for about one seed in 2^22.5 (the L = 1 witness).",
     UINT64_C(0xc4cec22cffed4464), 0x22e699d5u},
};

/* ---- RNG: splitmix64-seeded xoshiro256** ------------------------------------------------- */

static uint64_t rng_state[4];
static uint64_t rng_rot(uint64_t x, unsigned n) { return x << n | x >> (64 - n); }
static uint64_t splitmix(uint64_t *s) {
    uint64_t z = (*s += UINT64_C(0x9e3779b97f4a7c15));
    z = (z ^ (z >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
    z = (z ^ (z >> 27)) * UINT64_C(0x94d049bb133111eb);
    return z ^ (z >> 31);
}
static void rng_init(uint64_t s) { for (int i = 0; i < 4; i++) rng_state[i] = splitmix(&s); }
static uint64_t rng_next(void) {
    uint64_t *s = rng_state, r = rng_rot(s[1] * 5, 7) * 9, t = s[1] << 17;
    s[2] ^= s[0]; s[3] ^= s[1]; s[1] ^= s[2]; s[0] ^= s[3]; s[2] ^= t; s[3] = rng_rot(s[3], 45);
    return r;
}

/* ---- helpers ------------------------------------------------------------------------------ */

static size_t decode(const char *s, uint8_t *out) {
    size_t n = strlen(s) / 2;
    if (strlen(s) % 2 || n > 64) { fputs("invalid built-in vector\n", stderr); exit(1); }
    for (size_t i = 0; i < n; i++) {
        unsigned x;
        if (sscanf(s + 2 * i, "%2x", &x) != 1) { fputs("invalid built-in vector\n", stderr); exit(1); }
        out[i] = (uint8_t)x;
    }
    return n;
}
static uint64_t argument(const char *s, uint64_t max) {
    char *end;
    if (!*s || *s == '-' || *s == '+' || *s == ' ' || *s == '\t') goto bad;
    errno = 0;
    uint64_t x = strtoull(s, &end, 0);
    if (errno || *end || x > max) goto bad;
    return x;
bad:
    fputs("invalid argument\n", stderr); exit(2);
}

static int validate(void) {
    uint8_t buf[64];
    int fail = 0, n = 0;
    for (size_t i = 0; i < sizeof corefx_vectors / sizeof *corefx_vectors; i++) {
        size_t len = decode(corefx_vectors[i].hex, buf);
        uint64_t got = marvin64_legacy(buf, len, corefx_vectors[i].seed);
        n++;
        if (got != corefx_vectors[i].expected) {
            fail++;
            printf("corefx vector seed %016" PRIx64 " data %s: %016" PRIx64 " expected %016" PRIx64 " FAIL\n",
                   corefx_vectors[i].seed, corefx_vectors[i].hex, got, corefx_vectors[i].expected);
        }
    }
    printf("validation: corefx MarvinTests.cs 64-bit vectors (seeds 4fb61a001bdbcc, 804fb61a001bdbcc, 804fb61a801bdbcc): %d/%d %s\n",
           n - fail, n, fail ? "FAIL" : "PASS");
    {
        static const uint8_t abcdefg[] = {'A',0,'b',0,'c',0,'d',0,'e',0,'f',0,'g',0};
        uint32_t got = marvin32(abcdefg, sizeof abcdefg, UINT64_C(0x5D70D359C498B3F8));
        printf("validation: floodyberry Marvin32.c \"Abcdefg\" (UTF-16LE) seed 5d70d359c498b3f8: %08" PRIx32 " expected ba627c81 %s\n",
               got, got == 0xba627c81u ? "PASS" : "FAIL");
        if (got != 0xba627c81u) fail++;
    }
    {
        uint64_t st = runtime_vector_stream; int rn = 0, rfail = 0;
        for (int k = 0; k < 4; k++) {
            uint64_t seed = splitmix(&st);
            for (int len = 0; len <= 15; len++) {
                for (int i = 0; i < len; i++) buf[i] = (uint8_t)splitmix(&st);
                uint32_t got = marvin32(buf, (size_t)len, seed);
                rn++;
                if (got != runtime_vectors[k][len]) {
                    rfail++;
                    printf("runtime vector seed %016" PRIx64 " len %d: %08" PRIx32 " expected %08" PRIx32 " FAIL\n",
                           seed, len, got, runtime_vectors[k][len]);
                }
            }
        }
        printf("validation: .NET 10.0.12 runtime vectors from dotnet_check.txt (4 seeds x lengths 0..15): %d/%d %s\n",
               rn - rfail, rn, rfail ? "FAIL" : "PASS");
        fail += rfail;
    }
    return fail == 0;
}

int main(int argc, char **argv) {
    if (argc > 4) {
        fprintf(stderr, "usage: %s [log2 N (0..40), default 20] [rng seed, default 1] [pairs: A, B or AB, default AB]\n", argv[0]);
        return 2;
    }
    unsigned lg = argc > 1 ? (unsigned)argument(argv[1], 40) : 20;
    uint64_t rseed = argc > 2 ? argument(argv[2], UINT64_MAX) : 1;
    const char *which = argc > 3 ? argv[3] : "AB";
    if (strcmp(which, "A") && strcmp(which, "B") && strcmp(which, "AB")) { fputs("invalid argument\n", stderr); return 2; }
    uint64_t n = UINT64_C(1) << lg;

    puts("Marvin32 (.NET string.GetHashCode) fixed-pair collision check");
    puts("hash: dotnet/runtime System/Marvin.cs at tag v10.0.12 (commit 4271d88e), transcribed; runtime executed for the vectors: .NET 10.0.12");
    if (!validate()) return 1;
    puts("seed protocol: string.GetHashCode() = Marvin.ComputeHash32(UTF-16LE bytes, 2*Length, (uint)seed, (uint)(seed >> 32)) with seed = Marvin.DefaultSeed,");
    puts("  64 random bits drawn once per process (per instance for RandomizedStringEqualityComparer); sampled here as uniform 64-bit values from xoshiro256**.");

    for (size_t i = 0; i < sizeof pairs / sizeof *pairs; i++) {
        const Pair *p = &pairs[i];
        if (!strchr(which, p->name[0])) continue;
        uint8_t a[64], b[64];
        size_t na = decode(p->a, a), nb = decode(p->b, b);
        if (na == nb && !memcmp(a, b, na)) { fputs("built-in pair is not a pair\n", stderr); return 1; }
        size_t L = ((na > nb ? na : nb) + 7) / 8;
        printf("\npair %s / Marvin32 .NET 10.0.12 (L = %zu word%s)\n", p->name, L, L == 1 ? "" : "s");
        printf("M  (%zu B) = %s   %s\n", na, p->a, p->a_text);
        printf("M' (%zu B) = %s   %s\n", nb, p->b, p->b_text);
        puts(p->mechanism);
        uint32_t ha = marvin32(a, na, p->seed), hb = marvin32(b, nb, p->seed);
        printf("recorded colliding seed %016" PRIx64 ": H(M)=%08" PRIx32 " H(M')=%08" PRIx32 " expected %08" PRIx32 " %s\n",
               p->seed, ha, hb, p->expected, ha == hb && ha == p->expected ? "PASS" : "FAIL");
        if (ha != hb || ha != p->expected) { fputs("recorded output mismatch\n", stderr); return 1; }

        uint64_t count = 0, state64 = 0, first_seed = 0; uint32_t first_hash = 0;
        rng_init(rseed);   /* the same stream for every pair, deliberately correlated */
        for (uint64_t t = 0; t < n; t++) {
            uint64_t seed = rng_next();
            uint32_t a0, a1, b0, b1;
            marvin_state(a, (uint32_t)na, (uint32_t)seed, (uint32_t)(seed >> 32), &a0, &a1);
            marvin_state(b, (uint32_t)nb, (uint32_t)seed, (uint32_t)(seed >> 32), &b0, &b1);
            if ((a0 ^ a1) == (b0 ^ b1)) {
                if (!count) { first_seed = seed; first_hash = a0 ^ a1; }
                count++;
                if (a0 == b0 && a1 == b1) state64++;
            }
        }
        double rate = (double)count / (double)n;
        printf("random seeds: N = %" PRIu64 " (2^%u), rng seed %" PRIu64 "\n", n, lg, rseed);
        printf("collisions = %" PRIu64 " / %" PRIu64 "; rate = %.12g", count, n, rate);
        if (count) printf("; log2(rate) = %.6f; sampled score = log2(L) - log2(rate) = %.6f", log2(rate), log2((double)L) - log2(rate));
        else printf("; log2(rate) = -inf (zero hits; no population-rate estimate)");
        puts("");
        printf("of which full 64-bit state collisions: %" PRIu64 "\n", state64);
        if (count) printf("first sampled colliding seed %016" PRIx64 ": H(M)=%08" PRIx32 " H(M')=%08" PRIx32 "\n", first_seed, first_hash, first_hash);
        else puts("no sampled collision; the recorded witness above was checked separately");
    }
    return 0;
}
