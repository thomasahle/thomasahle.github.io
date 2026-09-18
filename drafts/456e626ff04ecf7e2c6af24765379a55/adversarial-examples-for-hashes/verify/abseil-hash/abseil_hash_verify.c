/* abseil_hash_verify.c -- seed-independent (key-free) collisions in absl::Hash.
 *
 * absl::Hash<std::string_view> / absl::Hash<std::string>, abseil-cpp commit
 * 73d2688300440c8af028eec865ee0dcd85e93025 (2026-09-17), whose
 * absl/hash/internal/hash.h and hash.cc are what LTS 20260817.0 ships.  The hash
 * is re-implemented below from those two files for all lengths: the len <= 32
 * paths (where the pairs live, identical on every 64-bit build with the shipped
 * ABSL_OPTION_INLINE_HW_ACCEL_STRATEGY 0), and the three LowLevelHash backends
 * for len > 32 (scalar = the x86-64 default build; x86 AES-NI when compiled
 * with -msse4.2 -maes, the condition hash.cc uses; ARM crypto when the compiler
 * defines __ARM_NEON and __ARM_FEATURE_CRYPTO, as Apple clang does on arm64
 * macOS by default).  -DABSL_VERIFY_SCALAR forces the scalar backend.
 *
 * Startup validation: 325 (len, seed, hash) vectors recorded from the real
 * library at that commit through absl::hash_internal::HashWithSeed (the hook
 * raw_hash_set uses) -- 165 for len 0..32 (build-independent) and 160 for
 * len 33..8192 of the backend compiled in -- must all match, the three pair
 * recipes must re-derive the published hex from the constants, and the hash
 * values recorded from the real library at seeds 0 and 0x4055c8 must be
 * reproduced.  Any mismatch exits non-zero before a seed is sampled.
 * native/absl_check.cc is the same check run inside the real library
 * (../Makefile target `native` fetches the pinned commit and diffs vectors).
 *
 * Then each pair is hashed under the 32 SwissTable per-table seeds
 * {0, 64, ..., 1984} (exhaustive: flat_hash_set/map draw a 5-bit seed and
 * shift it by 6) and under N uniform 64-bit seeds from splitmix64-seeded
 * xoshiro256** (the model of the post's table; every 64-bit seed collides by
 * the identity in the README).  A pair that fails to collide on any sampled
 * seed fails the run.
 *
 * Build:  cc -O2 -std=c11 -o abseil_hash_verify abseil_hash_verify.c -lm
 * Run:    ./abseil_hash_verify [log2 N (0..40), default 20] [rng seed, default 1]
 *         ./abseil_hash_verify --vectors     (print the vector schedule; diff with native/absl_check vectors)
 * Single-threaded, reads no files, writes only stdout/stderr.  Needs the
 * GCC/Clang unsigned __int128 extension (as Abseil's own Mix does on 64-bit).
 *
 * Copyright (c) 2026 Thomas Dybdahl Ahle
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
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * The algorithm and its two constant tables (kMul and the five kStaticRandomData
 * words, hexadecimal digits of pi) are Abseil's: Copyright 2018 The Abseil
 * Authors, Apache License 2.0 (https://github.com/abseil/abseil-cpp).  No Abseil
 * source text is copied; the functions below were written from reading hash.h
 * and hash.cc and are validated against the real library as described above.
 */
#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if !defined(ABSL_VERIFY_SCALAR) && defined(__SSE4_2__) && defined(__AES__)
#include <smmintrin.h>
#include <wmmintrin.h>
#define BACKEND_X86AES 1
#define BACKEND_NAME "x86 AES-NI (-msse4.2 -maes)"
#elif !defined(ABSL_VERIFY_SCALAR) && defined(__ARM_NEON) && defined(__ARM_FEATURE_CRYPTO)
#include <arm_neon.h>
#define BACKEND_ARM 1
#define BACKEND_NAME "ARM crypto (arm64 default)"
#else
#define BACKEND_SCALAR 1
#define BACKEND_NAME "scalar (x86-64 default build)"
#endif

/* ---------------------------------------------------------------- the hash */
static const uint64_t KMUL = 0x79d5f9e0de1e8cf5ull;                 /* hash.h kMul */
static const uint64_t K[5] = {0x243f6a8885a308d3ull, 0x13198a2e03707344ull,  /* kStaticRandomData */
                              0xa4093822299f31d0ull, 0x082efa98ec4e6c89ull,
                              0x452821e638d01377ull};

static uint64_t ld64(const uint8_t *p) { uint64_t v; memcpy(&v, p, 8); return v; }
static uint32_t ld32(const uint8_t *p) { uint32_t v; memcpy(&v, p, 4); return v; }
static uint64_t mix(uint64_t a, uint64_t b) {                    /* Mix: hi64(a*b) ^ lo64(a*b) */
  unsigned __int128 m = (unsigned __int128)a * b;
  return (uint64_t)(m >> 64) ^ (uint64_t)m;
}
/* PrecombineLengthMix: XOR an unaligned 8-byte window of kStaticRandomData at byte offset len.
 * This is D(len) in the README. */
static uint64_t D(size_t len) {
  uint8_t kb[40];
  for (int i = 0; i < 5; i++) for (int j = 0; j < 8; j++) kb[8 * i + j] = (uint8_t)(K[i] >> (8 * j));
  return ld64(kb + len);
}
/* CombineSmallContiguousImpl: len 0..8 */
static uint64_t small_v(const uint8_t *p, size_t len) {
  if (len >= 4) return ((uint64_t)ld32(p) << 32) | ld32(p + len - 4);          /* Read4To8 */
  if (len > 0) return ((uint32_t)p[0] << 16) | p[len - 1] | ((uint32_t)p[len / 2] << 8); /* Read1To3 */
  return 0x57;                                                                   /* empty string */
}
static uint64_t h_0to8(uint64_t state, const uint8_t *p, size_t len) {
  return mix(state ^ small_v(p, len), KMUL);                                    /* CombineRawImpl */
}
static uint64_t h_9to16(uint64_t state, const uint8_t *p, size_t len) {
  return mix(state ^ ld64(p), KMUL ^ ld64(p + len - 8));
}
static uint64_t h_17to32(uint64_t state, const uint8_t *p, size_t len) {
  const uint8_t *t = p + len - 16;
  return mix(ld64(p) ^ K[1], ld64(p + 8) ^ state) ^ mix(ld64(t) ^ K[3], ld64(t + 8) ^ state);
}

#if defined(BACKEND_SCALAR)
static uint64_t mix32(const uint8_t *p, uint64_t cs) {                          /* Mix32Bytes */
  return mix(ld64(p) ^ K[1], ld64(p + 8) ^ cs) ^ mix(ld64(p + 16) ^ K[2], ld64(p + 24) ^ cs);
}
static uint64_t llh(uint64_t seed, const uint8_t *p, size_t len) {              /* LowLevelHashLenGt32 */
  uint64_t cs = seed ^ K[0] ^ (uint64_t)len;
  const uint8_t *last32 = p + len - 32;
  if (len <= 64) return mix32(last32, mix32(p, cs));
  uint64_t s0 = cs, s1 = cs, s2 = cs;
  do {
    cs = mix(ld64(p) ^ K[1], ld64(p + 8) ^ cs);
    s0 = mix(ld64(p + 16) ^ K[2], ld64(p + 24) ^ s0);
    s1 = mix(ld64(p + 32) ^ K[3], ld64(p + 40) ^ s1);
    s2 = mix(ld64(p + 48) ^ K[4], ld64(p + 56) ^ s2);
    p += 64; len -= 64;
  } while (len > 64);
  cs = (cs ^ s0) ^ (s1 + s2);
  if (len > 32) cs = mix32(p, cs);
  return mix32(last32, cs);
}
#elif defined(BACKEND_X86AES)
typedef __m128i v128;
static v128 L(const uint8_t *p) { return _mm_loadu_si128((const v128 *)p); }
static v128 mixA(v128 x, v128 s) { return _mm_aesdec_si128(_mm_add_epi64(s, x), s); }
static v128 mixB(v128 x, v128 s) { return _mm_aesdec_si128(_mm_sub_epi64(s, x), s); }
static v128 mixC(v128 x, v128 s) { return _mm_aesenc_si128(_mm_add_epi64(s, x), s); }
static v128 mixD(v128 x, v128 s) { return _mm_aesenc_si128(_mm_sub_epi64(s, x), s); }
static uint64_t mix4(v128 a, v128 b, v128 c, v128 d) {                          /* Mix4x16Vectors */
  v128 r = _mm_add_epi64(_mm_aesenc_si128(_mm_add_epi64(a, c), d), _mm_aesdec_si128(_mm_sub_epi64(b, d), a));
  return (uint64_t)_mm_cvtsi128_si64(r) ^ (uint64_t)_mm_extract_epi64(r, 1);
}
static uint64_t llh(uint64_t seed, const uint8_t *p, size_t len) {
  v128 st = _mm_set_epi64x((long long)seed, (long long)len);                    /* Set128(seed, len) */
  const uint8_t *last32 = p + len - 32;
  if (len <= 64) return mix4(mixA(L(p), st), mixB(L(p + 16), st), mixC(L(last32), st), mixD(L(last32 + 16), st));
  v128 s0 = st, s1 = st, s2 = st, s3 = st;
  do {
    s0 = mixA(L(p), s0); s1 = mixB(L(p + 16), s1); s2 = mixC(L(p + 32), s2); s3 = mixD(L(p + 48), s3);
    p += 64; len -= 64;
  } while (len > 64);
  if (len > 32) { s0 = mixA(L(p), s0); s1 = mixB(L(p + 16), s1); }
  s2 = mixC(L(last32), s2); s3 = mixD(L(last32 + 16), s3);
  return mix4(s0, s1, s2, s3);
}
#else /* BACKEND_ARM */
typedef uint8x16_t v128;
static v128 L(const uint8_t *p) { return vld1q_u8(p); }
static v128 add128(v128 a, v128 b) { return vreinterpretq_u8_u64(vaddq_u64(vreinterpretq_u64_u8(a), vreinterpretq_u64_u8(b))); }
static v128 enc(v128 d, v128 k) { return vaesmcq_u8(vaeseq_u8(d, k)); }         /* Encrypt128: key XORed before SubBytes */
static v128 dec(v128 d, v128 k) { return vaesimcq_u8(vaesdq_u8(d, k)); }        /* Decrypt128 */
static uint64_t mix4(v128 a, v128 b, v128 c, v128 d) {
  v128 r = add128(enc(a, c), dec(b, d));
  return vgetq_lane_u64(vreinterpretq_u64_u8(r), 0) ^ vgetq_lane_u64(vreinterpretq_u64_u8(r), 1);
}
static uint64_t llh(uint64_t seed, const uint8_t *p, size_t len) {
  v128 st = vreinterpretq_u8_u64(vsetq_lane_u64(seed, vdupq_n_u64((uint64_t)len), 1)); /* Set128(seed, len) */
  const uint8_t *last32 = p + len - 32;
  if (len <= 64) return mix4(dec(L(p), st), dec(L(p + 16), st), enc(L(last32), st), enc(L(last32 + 16), st));
  v128 s0 = st, s1 = st, s2 = st, s3 = st;
  do {
    s0 = dec(L(p), s0); s1 = dec(L(p + 16), s1); s2 = enc(L(p + 32), s2); s3 = enc(L(p + 48), s3);
    p += 64; len -= 64;
  } while (len > 64);
  if (len > 32) { s0 = dec(L(p), s0); s1 = dec(L(p + 16), s1); }
  s2 = enc(L(last32), s2); s3 = enc(L(last32 + 16), s3);
  return mix4(s0, s1, s2, s3);
}
#endif

/* CombineContiguousImpl (64-bit size_t), incl. the >1024-byte PiecewiseChunkSize split */
static uint64_t absl_hash(uint64_t seed, const uint8_t *p, size_t len) {
  if (len <= 8) return h_0to8(seed ^ D(len), p, len);
  if (len <= 16) return h_9to16(seed ^ D(len), p, len);
  if (len <= 32) return h_17to32(seed ^ D(len), p, len);
  if (len <= 1024) return llh(seed, p, len);
  uint64_t state = seed;
  while (len >= 1024) { state = llh(state, p, 1024); p += 1024; len -= 1024; }
  return len == 0 ? state : absl_hash(state, p, len);
}

/* ------------------------------------------------ vectors from the real library */
struct vec { uint32_t len; uint64_t seed, hash; };
/* len 0..32: identical on every 64-bit build (recorded on x86-64, scalar and -maes builds agree) */
static const struct vec VEC_LE32[] = {
  {0, 0x0000000000000000ull, 0x2bda3ac53577c4b7ull},
  {0, 0x0000000000000001ull, 0xa5303ce4d31571aaull},
  {0, 0x00007f3a1c5e2000ull, 0xa134c16ea3c47255ull},
  {0, 0xdeadbeefcafef00dull, 0x8ed09e943e13c283ull},
  {0, 0xffffffffffffffffull, 0x231a3205b177e6a6ull},
  {1, 0x0000000000000000ull, 0x3a6c7210b1d3e275ull},
  {1, 0x0000000000000001ull, 0x80427c7197f16f0full},
  {1, 0x00007f3a1c5e2000ull, 0xc4cb26648585391full},
  {1, 0xdeadbeefcafef00dull, 0x903ec49eb2ac7aa9ull},
  {1, 0xffffffffffffffffull, 0x35404252352b1862ull},
  {2, 0x0000000000000000ull, 0xaf6625ddac452ebeull},
  {2, 0x0000000000000001ull, 0x293c23fc8e27a24bull},
  {2, 0x00007f3a1c5e2000ull, 0xf210fb47af7d844aull},
  {2, 0xdeadbeefcafef00dull, 0xc479c9ea7f53fbe3ull},
  {2, 0xffffffffffffffffull, 0xae96311e298008beull},
  {3, 0x0000000000000000ull, 0xa32181847d36dc3cull},
  {3, 0x0000000000000001ull, 0x294b87a55b1453c9ull},
  {3, 0x00007f3a1c5e2000ull, 0x7787b46938288bf2ull},
  {3, 0xdeadbeefcafef00dull, 0xd7ea0d9d8f2f2960ull},
  {3, 0xffffffffffffffffull, 0xabd58dc270b8de3cull},
  {4, 0x0000000000000000ull, 0x23a22081e4f2263aull},
  {4, 0x0000000000000001ull, 0xa58c2aa0c6edbb4eull},
  {4, 0x00007f3a1c5e2000ull, 0x82ebca663a67c82aull},
  {4, 0xdeadbeefcafef00dull, 0xa9e0b612dc065a5full},
  {4, 0xffffffffffffffffull, 0xd312344de0f0c423ull},
  {5, 0x0000000000000000ull, 0x96f5d7655290843aull},
  {5, 0x0000000000000001ull, 0x1d2bd905b4f1310eull},
  {5, 0x00007f3a1c5e2000ull, 0x2141062f562aeb06ull},
  {5, 0xdeadbeefcafef00dull, 0x3b0ab92fb8bafcacull},
  {5, 0xffffffffffffffffull, 0x84c1c7669ad6a202ull},
  {6, 0x0000000000000000ull, 0x2eb34b287f2fbd41ull},
  {6, 0x0000000000000001ull, 0xa0e571489dcc30b3ull},
  {6, 0x00007f3a1c5e2000ull, 0x77ff1b1e1de40d14ull},
  {6, 0xdeadbeefcafef00dull, 0x030c6ab02e446ae9ull},
  {6, 0xffffffffffffffffull, 0x0f23432e7f2fb956ull},
  {7, 0x0000000000000000ull, 0x029ea2e8b69c7706ull},
  {7, 0x0000000000000001ull, 0xb944d889557eea7bull},
  {7, 0x00007f3a1c5e2000ull, 0x2c0770881de6b201ull},
  {7, 0xdeadbeefcafef00dull, 0xa8bfc815c2eb3b2full},
  {7, 0xffffffffffffffffull, 0x331ed2ee37596771ull},
  {8, 0x0000000000000000ull, 0x2a5aa26c4e25ea76ull},
  {8, 0x0000000000000001ull, 0xadb0ab8d6fc76542ull},
  {8, 0x00007f3a1c5e2000ull, 0x829a56c6bb80b357ull},
  {8, 0xdeadbeefcafef00dull, 0x055a4bb530e29f12ull},
  {8, 0xffffffffffffffffull, 0xca12ba6e8e25cc67ull},
  {9, 0x0000000000000000ull, 0x10c290166b833355ull},
  {9, 0x0000000000000001ull, 0x03c9df7c2a2ffeb9ull},
  {9, 0x00007f3a1c5e2000ull, 0xcdb85c78b59588b8ull},
  {9, 0xdeadbeefcafef00dull, 0xa85481a470cdf0beull},
  {9, 0xffffffffffffffffull, 0x12d006666b7bac01ull},
  {10, 0x0000000000000000ull, 0xabf899ab9a2f4347ull},
  {10, 0x0000000000000001ull, 0xdb846cfbb2a8898bull},
  {10, 0x00007f3a1c5e2000ull, 0x127686f30423e677ull},
  {10, 0xdeadbeefcafef00dull, 0x8370fa42bf27872eull},
  {10, 0xffffffffffffffffull, 0x8bf2140b1b2f4ab7ull},
  {11, 0x0000000000000000ull, 0x93247d27e60c8f67ull},
  {11, 0x0000000000000001ull, 0x0049ea727734bb27ull},
  {11, 0x00007f3a1c5e2000ull, 0x39a4b4620b40761aull},
  {11, 0xdeadbeefcafef00dull, 0x5f2394095c25848dull},
  {11, 0xffffffffffffffffull, 0x93b57a86002caf67ull},
  {12, 0x0000000000000000ull, 0xc40efe690572390aull},
  {12, 0x0000000000000001ull, 0x6ac06f9308dcc4f0ull},
  {12, 0x00007f3a1c5e2000ull, 0x1ea9a8ea1e1595a2ull},
  {12, 0xdeadbeefcafef00dull, 0x9999cc4e9a2da64bull},
  {12, 0xffffffffffffffffull, 0x878cff5561713c4cull},
  {13, 0x0000000000000000ull, 0x201a8ea89391f382ull},
  {13, 0x0000000000000001ull, 0xe701f8362ac3be7cull},
  {13, 0x00007f3a1c5e2000ull, 0xd2d848e056afb2a5ull},
  {13, 0xdeadbeefcafef00dull, 0x37e31758e535273eull},
  {13, 0xffffffffffffffffull, 0xa624d0aa2b55fb83ull},
  {14, 0x0000000000000000ull, 0xbd55a5891da6f06bull},
  {14, 0x0000000000000001ull, 0x26c682bd453e5029ull},
  {14, 0x00007f3a1c5e2000ull, 0xcacb85086f5a525aull},
  {14, 0xdeadbeefcafef00dull, 0x8192069565b17e02ull},
  {14, 0xffffffffffffffffull, 0xa059b9b29d56f073ull},
  {15, 0x0000000000000000ull, 0x5e1067340037394bull},
  {15, 0x0000000000000001ull, 0xad669878ce803312ull},
  {15, 0x00007f3a1c5e2000ull, 0x1afc225f120e6a4bull},
  {15, 0xdeadbeefcafef00dull, 0xa34a37c06cdb46acull},
  {15, 0xffffffffffffffffull, 0x1c4f5744b537356aull},
  {16, 0x0000000000000000ull, 0xb696e6748c1a5cffull},
  {16, 0x0000000000000001ull, 0xa45be1ac8f0c5902ull},
  {16, 0x00007f3a1c5e2000ull, 0x70a384da7d950559ull},
  {16, 0xdeadbeefcafef00dull, 0x7978c48b81044641ull},
  {16, 0xffffffffffffffffull, 0x27149064b21b98ffull},
  {17, 0x0000000000000000ull, 0x183787d4a2809f3full},
  {17, 0x0000000000000001ull, 0x5970f31547b713a1ull},
  {17, 0x00007f3a1c5e2000ull, 0xe975b01dcb498cfdull},
  {17, 0xdeadbeefcafef00dull, 0x5e0b6d699a281c16ull},
  {17, 0xffffffffffffffffull, 0x18a93c349ed9b671ull},
  {18, 0x0000000000000000ull, 0x9696ecb4be4ecd02ull},
  {18, 0x0000000000000001ull, 0xd784971f7f5379f8ull},
  {18, 0x00007f3a1c5e2000ull, 0x673853c260bce605ull},
  {18, 0xdeadbeefcafef00dull, 0x9f2c7c709c324680ull},
  {18, 0xffffffffffffffffull, 0x9195a4b7b98ec30dull},
  {19, 0x0000000000000000ull, 0xe089fa83b85d00e8ull},
  {19, 0x0000000000000001ull, 0x9d2b0d252d356db2ull},
  {19, 0x00007f3a1c5e2000ull, 0x56cccacfcc759579ull},
  {19, 0xdeadbeefcafef00dull, 0x25ddc4c1a43c4b43ull},
  {19, 0xffffffffffffffffull, 0xb17d0c8140c3108cull},
  {20, 0x0000000000000000ull, 0x1ef2e17232f37a36ull},
  {20, 0x0000000000000001ull, 0x33d31916b722b565ull},
  {20, 0x00007f3a1c5e2000ull, 0x95caef52a36d4261ull},
  {20, 0xdeadbeefcafef00dull, 0x3f7cd1411513a21bull},
  {20, 0xffffffffffffffffull, 0x3df1fbdd24addc70ull},
  {21, 0x0000000000000000ull, 0x1b61f1df5a751961ull},
  {21, 0x0000000000000001ull, 0x08da9a9f91654f47ull},
  {21, 0x00007f3a1c5e2000ull, 0xd6ffef8f942476e8ull},
  {21, 0xdeadbeefcafef00dull, 0x2c78110fe446bf38ull},
  {21, 0xffffffffffffffffull, 0x7e00e9cce5eb2fe1ull},
  {22, 0x0000000000000000ull, 0x1da6e4dd7ee0d87bull},
  {22, 0x0000000000000001ull, 0xe105338722c4af56ull},
  {22, 0x00007f3a1c5e2000ull, 0x69d758a85ba10ae8ull},
  {22, 0xdeadbeefcafef00dull, 0x9961cc05d21073f1ull},
  {22, 0xffffffffffffffffull, 0xdd00a5b8bf7c1c02ull},
  {23, 0x0000000000000000ull, 0x985646e402586e6full},
  {23, 0x0000000000000001ull, 0xf9780c12fd4ebb97ull},
  {23, 0x00007f3a1c5e2000ull, 0xeac038f4fbad396bull},
  {23, 0xdeadbeefcafef00dull, 0xfc97a68e45adb9fcull},
  {23, 0xffffffffffffffffull, 0x584a428a00359a34ull},
  {24, 0x0000000000000000ull, 0x4f32b760c8dccdc9ull},
  {24, 0x0000000000000001ull, 0x2bb3323f65c09badull},
  {24, 0x00007f3a1c5e2000ull, 0x65a95d9905918622ull},
  {24, 0xdeadbeefcafef00dull, 0xd5a16c470847b9aeull},
  {24, 0xffffffffffffffffull, 0x8e10b0a8797cc3beull},
  {25, 0x0000000000000000ull, 0x14359eb5d2e88d2cull},
  {25, 0x0000000000000001ull, 0x4bda00b84f33c781ull},
  {25, 0x00007f3a1c5e2000ull, 0xb280e02906af9e42ull},
  {25, 0xdeadbeefcafef00dull, 0x1057a75c92e0e825ull},
  {25, 0xffffffffffffffffull, 0xf41c9d08b46fbc8bull},
  {26, 0x0000000000000000ull, 0x52ec57f6c5721d6cull},
  {26, 0x0000000000000001ull, 0x041749b0002a1e5dull},
  {26, 0x00007f3a1c5e2000ull, 0xe27c88a5871b6898ull},
  {26, 0xdeadbeefcafef00dull, 0xd6030e9c929658b2ull},
  {26, 0xffffffffffffffffull, 0x20bc16d6935e7da4ull},
  {27, 0x0000000000000000ull, 0xbd8764088f79fd1dull},
  {27, 0x0000000000000001ull, 0x8b06e62868dd40e5ull},
  {27, 0x00007f3a1c5e2000ull, 0xc5116b6e29be7a1cull},
  {27, 0xdeadbeefcafef00dull, 0x73d931b817e77486ull},
  {27, 0xffffffffffffffffull, 0x56a3920c82d36515ull},
  {28, 0x0000000000000000ull, 0xe7ff48fa0c817e2dull},
  {28, 0x0000000000000001ull, 0x3ffbc9874e9b3d86ull},
  {28, 0x00007f3a1c5e2000ull, 0xd63604181b4b8238ull},
  {28, 0xdeadbeefcafef00dull, 0x65ffc801cf05f799ull},
  {28, 0xffffffffffffffffull, 0xd6af4c9664b3121aull},
  {29, 0x0000000000000000ull, 0xc182cbc270ebd57cull},
  {29, 0x0000000000000001ull, 0x698cbdb9aeff8a52ull},
  {29, 0x00007f3a1c5e2000ull, 0xd824617c5ed14ae9ull},
  {29, 0xdeadbeefcafef00dull, 0xb8ba6c800f035990ull},
  {29, 0xffffffffffffffffull, 0x22ffb3569c2a6f6bull},
  {30, 0x0000000000000000ull, 0xffc5afc889432c85ull},
  {30, 0x0000000000000001ull, 0x6c14b08b0126e5bfull},
  {30, 0x00007f3a1c5e2000ull, 0xd938b55eeccd79ccull},
  {30, 0xdeadbeefcafef00dull, 0x56f351dcf8ee4940ull},
  {30, 0xffffffffffffffffull, 0x6084b36fcab95a8cull},
  {31, 0x0000000000000000ull, 0xb7bc9e534471d491ull},
  {31, 0x0000000000000001ull, 0x499eb229fc7014d0ull},
  {31, 0x00007f3a1c5e2000ull, 0xb4c7825551aa76dbull},
  {31, 0xdeadbeefcafef00dull, 0xe48b63fc185df20full},
  {31, 0xffffffffffffffffull, 0xb561c765af13fdd0ull},
  {32, 0x0000000000000000ull, 0x150e1835ee3594efull},
  {32, 0x0000000000000001ull, 0x6ff0f8cc01fbf0ffull},
  {32, 0x00007f3a1c5e2000ull, 0x234002a10c8a8dbeull},
  {32, 0xdeadbeefcafef00dull, 0x3c6d7e8afef08ee6ull},
  {32, 0xffffffffffffffffull, 0x522a54d7b3ad69d8ull},
};
#if defined(BACKEND_SCALAR)
/* len 33..8192, scalar LowLevelHash (x86-64 default build), recorded on a Xeon 8375C */
static const struct vec VEC_GT32[] = {
  {33, 0x0000000000000000ull, 0x239b29d4a11536d6ull},
  {33, 0x0000000000000001ull, 0xbe44d0d1e118231eull},
  {33, 0x00007f3a1c5e2000ull, 0x14ddba343c03a8abull},
  {33, 0xdeadbeefcafef00dull, 0x9d73dcf5a2c690a4ull},
  {33, 0xffffffffffffffffull, 0x521aecd2e65dbee3ull},
  {34, 0x0000000000000000ull, 0x9ede088f6ec9a703ull},
  {34, 0x0000000000000001ull, 0x54342585ae1a6fc1ull},
  {34, 0x00007f3a1c5e2000ull, 0x0bc685f5e2368f08ull},
  {34, 0xdeadbeefcafef00dull, 0x8983d6a935ef46b8ull},
  {34, 0xffffffffffffffffull, 0xcf7d49836974219cull},
  {35, 0x0000000000000000ull, 0x191dacb739c111e7ull},
  {35, 0x0000000000000001ull, 0x70cb9883f7dd7f6eull},
  {35, 0x00007f3a1c5e2000ull, 0xe8a2a7e6278ba940ull},
  {35, 0xdeadbeefcafef00dull, 0x69bf5a30f65ddd3dull},
  {35, 0xffffffffffffffffull, 0x9f79a5add6b10b82ull},
  {36, 0x0000000000000000ull, 0x51f887cc5182954cull},
  {36, 0x0000000000000001ull, 0xa394719881db8668ull},
  {36, 0x00007f3a1c5e2000ull, 0x59f3f497f93f97faull},
  {36, 0xdeadbeefcafef00dull, 0x3cbe062c25c008ffull},
  {36, 0xffffffffffffffffull, 0x7f9158599aa4b31full},
  {37, 0x0000000000000000ull, 0x9ec9ce8e92321af0ull},
  {37, 0x0000000000000001ull, 0x35800091e840ec5full},
  {37, 0x00007f3a1c5e2000ull, 0x184e8060ea11f69bull},
  {37, 0xdeadbeefcafef00dull, 0x5069648221cdf3d6ull},
  {37, 0xffffffffffffffffull, 0xd4d830efbd8eaf31ull},
  {38, 0x0000000000000000ull, 0x92108d67f3e3988full},
  {38, 0x0000000000000001ull, 0x0f783a97dc4208a7ull},
  {38, 0x00007f3a1c5e2000ull, 0x7d5f10c3091e0b8eull},
  {38, 0xdeadbeefcafef00dull, 0xa0b055e8bbbd9866ull},
  {38, 0xffffffffffffffffull, 0x675b8205418963b7ull},
  {39, 0x0000000000000000ull, 0x83548c4b70567397ull},
  {39, 0x0000000000000001ull, 0x19a7c29d9bd5d7d1ull},
  {39, 0x00007f3a1c5e2000ull, 0x2ea4cddb0de5a734ull},
  {39, 0xdeadbeefcafef00dull, 0x1b8b9373638b4080ull},
  {39, 0xffffffffffffffffull, 0x93784b8e685fe5a9ull},
  {40, 0x0000000000000000ull, 0xbe3bd4c92b683699ull},
  {40, 0x0000000000000001ull, 0x8887c1bf9f34c355ull},
  {40, 0x00007f3a1c5e2000ull, 0xa8362762c747e309ull},
  {40, 0xdeadbeefcafef00dull, 0xf3acc14f26bad409ull},
  {40, 0xffffffffffffffffull, 0x0f293e451eb34d38ull},
  {48, 0x0000000000000000ull, 0x262966d95cdc2f8dull},
  {48, 0x0000000000000001ull, 0x9c38b47878297a1cull},
  {48, 0x00007f3a1c5e2000ull, 0x43abfcae3966e5ccull},
  {48, 0xdeadbeefcafef00dull, 0x96f488e249c41a0full},
  {48, 0xffffffffffffffffull, 0x5369bb138c3d0ef2ull},
  {63, 0x0000000000000000ull, 0x7a16b743becb53dfull},
  {63, 0x0000000000000001ull, 0x666deb5f12a66b25ull},
  {63, 0x00007f3a1c5e2000ull, 0x2261e4a4f74f6ac3ull},
  {63, 0xdeadbeefcafef00dull, 0x57898aac6bb43269ull},
  {63, 0xffffffffffffffffull, 0xcc3991ad4f87325dull},
  {64, 0x0000000000000000ull, 0x4904024b040afd33ull},
  {64, 0x0000000000000001ull, 0x0e869650d41f6121ull},
  {64, 0x00007f3a1c5e2000ull, 0xbf58b78a78fb11f1ull},
  {64, 0xdeadbeefcafef00dull, 0xb9abff880f24bca0ull},
  {64, 0xffffffffffffffffull, 0xef25b758c8f997dcull},
  {65, 0x0000000000000000ull, 0x4f3a4f35ae196218ull},
  {65, 0x0000000000000001ull, 0x0a8d5b3649cc2af6ull},
  {65, 0x00007f3a1c5e2000ull, 0x2aca0b86ed415581ull},
  {65, 0xdeadbeefcafef00dull, 0xfdc793514970d0e6ull},
  {65, 0xffffffffffffffffull, 0x77942e9168043be3ull},
  {96, 0x0000000000000000ull, 0x3121f5ab10a605a3ull},
  {96, 0x0000000000000001ull, 0xe24a9fb7627273ecull},
  {96, 0x00007f3a1c5e2000ull, 0xfc4774dbfc4dec4cull},
  {96, 0xdeadbeefcafef00dull, 0x79493fd88dfe114cull},
  {96, 0xffffffffffffffffull, 0x0778d3f47552c543ull},
  {100, 0x0000000000000000ull, 0x3848383a976a1863ull},
  {100, 0x0000000000000001ull, 0x28a1b9383abdfbe5ull},
  {100, 0x00007f3a1c5e2000ull, 0x259402c556eb09edull},
  {100, 0xdeadbeefcafef00dull, 0x1cbdc11c66b9c914ull},
  {100, 0xffffffffffffffffull, 0xd6a00407abe548afull},
  {127, 0x0000000000000000ull, 0xfa779a7b6bbc5889ull},
  {127, 0x0000000000000001ull, 0xd6d145df75da2fa3ull},
  {127, 0x00007f3a1c5e2000ull, 0x64df89f66740c4cbull},
  {127, 0xdeadbeefcafef00dull, 0x8ca252729628ebbdull},
  {127, 0xffffffffffffffffull, 0x6853d36346736d07ull},
  {128, 0x0000000000000000ull, 0xc3c7f2f7101df40bull},
  {128, 0x0000000000000001ull, 0x8d7922d51d4ccdc0ull},
  {128, 0x00007f3a1c5e2000ull, 0x9fdcbf53916e619full},
  {128, 0xdeadbeefcafef00dull, 0x34fa529ef5e978a4ull},
  {128, 0xffffffffffffffffull, 0x62353bba957add04ull},
  {129, 0x0000000000000000ull, 0x4c0c05399f22d06full},
  {129, 0x0000000000000001ull, 0x2dbb4aecc7043a1cull},
  {129, 0x00007f3a1c5e2000ull, 0x251afd57c3833b15ull},
  {129, 0xdeadbeefcafef00dull, 0xd89a4b8511592228ull},
  {129, 0xffffffffffffffffull, 0xa035d2dec6a87c12ull},
  {200, 0x0000000000000000ull, 0x2b3661cae8fb05aeull},
  {200, 0x0000000000000001ull, 0x6c32e9f3a45b0d3dull},
  {200, 0x00007f3a1c5e2000ull, 0xf7e14adf4a2d3bcaull},
  {200, 0xdeadbeefcafef00dull, 0x658b569679a35e02ull},
  {200, 0xffffffffffffffffull, 0x25a4d4272337739full},
  {255, 0x0000000000000000ull, 0x6e1fcbac05ca9951ull},
  {255, 0x0000000000000001ull, 0xc4b6998abb637a99ull},
  {255, 0x00007f3a1c5e2000ull, 0x8ec364a382960541ull},
  {255, 0xdeadbeefcafef00dull, 0xf4f38827f0135a7cull},
  {255, 0xffffffffffffffffull, 0x1f68dff3c08a7bbeull},
  {256, 0x0000000000000000ull, 0x93a2d1687873416full},
  {256, 0x0000000000000001ull, 0x3de168e955ecd59dull},
  {256, 0x00007f3a1c5e2000ull, 0x759f7ca7737827d7ull},
  {256, 0xdeadbeefcafef00dull, 0x5ceeb351b5b19121ull},
  {256, 0xffffffffffffffffull, 0x244d52b81bfc5296ull},
  {511, 0x0000000000000000ull, 0x930ffceb3b692512ull},
  {511, 0x0000000000000001ull, 0x8482d02f573825c8ull},
  {511, 0x00007f3a1c5e2000ull, 0x84bbd750c4b75527ull},
  {511, 0xdeadbeefcafef00dull, 0x92fb2169ac8be1abull},
  {511, 0xffffffffffffffffull, 0xe1a195a82e03adb7ull},
  {512, 0x0000000000000000ull, 0x7ee29e47f57050d7ull},
  {512, 0x0000000000000001ull, 0x1bc5e814ec5c0225ull},
  {512, 0x00007f3a1c5e2000ull, 0xfb41824c5849710cull},
  {512, 0xdeadbeefcafef00dull, 0x68f6ef8ea7c2f1dbull},
  {512, 0xffffffffffffffffull, 0xa47145f6e3b59f67ull},
  {1000, 0x0000000000000000ull, 0x6853ef523580742dull},
  {1000, 0x0000000000000001ull, 0xe5fba2083e0291c6ull},
  {1000, 0x00007f3a1c5e2000ull, 0xe2899e71c502a276ull},
  {1000, 0xdeadbeefcafef00dull, 0x3de1a280559f563aull},
  {1000, 0xffffffffffffffffull, 0x3694eb1ead1db1afull},
  {1023, 0x0000000000000000ull, 0x9642faa647e837acull},
  {1023, 0x0000000000000001ull, 0xd83091e390919220ull},
  {1023, 0x00007f3a1c5e2000ull, 0xcdf39e079bb38ccbull},
  {1023, 0xdeadbeefcafef00dull, 0xf7000cdd864a2815ull},
  {1023, 0xffffffffffffffffull, 0x9dd083c1864a4495ull},
  {1024, 0x0000000000000000ull, 0x9b25ec0d520fe294ull},
  {1024, 0x0000000000000001ull, 0xb60c26cf094482cbull},
  {1024, 0x00007f3a1c5e2000ull, 0xb2d4068cf68ad8a1ull},
  {1024, 0xdeadbeefcafef00dull, 0x0b134d00f11743efull},
  {1024, 0xffffffffffffffffull, 0x9325ad4411a32e5aull},
  {1025, 0x0000000000000000ull, 0x9302b19f604c68f8ull},
  {1025, 0x0000000000000001ull, 0x19e47c1038072167ull},
  {1025, 0x00007f3a1c5e2000ull, 0x15ffa8d02fedce2bull},
  {1025, 0xdeadbeefcafef00dull, 0x736a9ad2de3000b7ull},
  {1025, 0xffffffffffffffffull, 0x01561fe2d5c6a82eull},
  {1100, 0x0000000000000000ull, 0x20eeeb9f8ead18b1ull},
  {1100, 0x0000000000000001ull, 0x61f8cd2eb290d856ull},
  {1100, 0x00007f3a1c5e2000ull, 0x5bbda94e8a41095bull},
  {1100, 0xdeadbeefcafef00dull, 0xeca16f7d0ed1cfbdull},
  {1100, 0xffffffffffffffffull, 0xc1eddec4275fa446ull},
  {2048, 0x0000000000000000ull, 0xed95f88ae0bd3f17ull},
  {2048, 0x0000000000000001ull, 0xba86650ed9bba888ull},
  {2048, 0x00007f3a1c5e2000ull, 0xf14c6ce9debe152cull},
  {2048, 0xdeadbeefcafef00dull, 0xa97195d518d59710ull},
  {2048, 0xffffffffffffffffull, 0x03e53f541ce47d6dull},
  {2049, 0x0000000000000000ull, 0xfe60735616500ca5ull},
  {2049, 0x0000000000000001ull, 0x1d44a4cae6509ac8ull},
  {2049, 0x00007f3a1c5e2000ull, 0xcf55a07053136dceull},
  {2049, 0xdeadbeefcafef00dull, 0x73ecc8faff0dea12ull},
  {2049, 0xffffffffffffffffull, 0x6dfed559022f49dcull},
  {3073, 0x0000000000000000ull, 0xeaf0b2813e0f5ffbull},
  {3073, 0x0000000000000001ull, 0x0cacbd8a397ac937ull},
  {3073, 0x00007f3a1c5e2000ull, 0x29571c15a0e6dc98ull},
  {3073, 0xdeadbeefcafef00dull, 0x2090bdcf4d0e5bdbull},
  {3073, 0xffffffffffffffffull, 0x197f70df77213017ull},
  {4097, 0x0000000000000000ull, 0x9afcd8616930053aull},
  {4097, 0x0000000000000001ull, 0x22d6f6a91c7b74faull},
  {4097, 0x00007f3a1c5e2000ull, 0xeeb9d736d5d3966aull},
  {4097, 0xdeadbeefcafef00dull, 0xac80dc632a32126aull},
  {4097, 0xffffffffffffffffull, 0x5176fb252ea18c5dull},
  {8192, 0x0000000000000000ull, 0xf0eea9619106839eull},
  {8192, 0x0000000000000001ull, 0xf99c2b0d2fb902a0ull},
  {8192, 0x00007f3a1c5e2000ull, 0x73f3523a37ca9beeull},
  {8192, 0xdeadbeefcafef00dull, 0x3c11dd97c68540d3ull},
  {8192, 0xffffffffffffffffull, 0x4ec8717583d3434aull},
};
#elif defined(BACKEND_X86AES)
/* len 33..8192, x86 AES-NI LowLevelHash (-msse4.2 -maes), recorded on a Xeon 8375C */
static const struct vec VEC_GT32[] = {
  {33, 0x0000000000000000ull, 0x4f9e60f771e0dadaull},
  {33, 0x0000000000000001ull, 0x60f7560a9aab5764ull},
  {33, 0x00007f3a1c5e2000ull, 0x9e8f4d200205c802ull},
  {33, 0xdeadbeefcafef00dull, 0xc757d581972636beull},
  {33, 0xffffffffffffffffull, 0xe4b1c20b5f987922ull},
  {34, 0x0000000000000000ull, 0xd610b69faa50f876ull},
  {34, 0x0000000000000001ull, 0xa99fb30f485742d1ull},
  {34, 0x00007f3a1c5e2000ull, 0xe93cf4fe80d5ac59ull},
  {34, 0xdeadbeefcafef00dull, 0xd7133629f9cf7157ull},
  {34, 0xffffffffffffffffull, 0xded44b7a6d8ec4c2ull},
  {35, 0x0000000000000000ull, 0xf3956caa8a0a0803ull},
  {35, 0x0000000000000001ull, 0xd5c2c51421716815ull},
  {35, 0x00007f3a1c5e2000ull, 0x7f038086ffe20e0eull},
  {35, 0xdeadbeefcafef00dull, 0x2986792ba3696e70ull},
  {35, 0xffffffffffffffffull, 0x4ca05eecbb5f04eaull},
  {36, 0x0000000000000000ull, 0xf9d19efc130d5630ull},
  {36, 0x0000000000000001ull, 0xe7e14e9deb503968ull},
  {36, 0x00007f3a1c5e2000ull, 0x8f7395fcf45c4ecbull},
  {36, 0xdeadbeefcafef00dull, 0x1a464674019734beull},
  {36, 0xffffffffffffffffull, 0xef5a3e86949a9c3aull},
  {37, 0x0000000000000000ull, 0x77cbde9db1c1e800ull},
  {37, 0x0000000000000001ull, 0x230f3532ef6e01c4ull},
  {37, 0x00007f3a1c5e2000ull, 0xd5a010872762a316ull},
  {37, 0xdeadbeefcafef00dull, 0xc054d40dd028b33dull},
  {37, 0xffffffffffffffffull, 0x69835f8dd7b24381ull},
  {38, 0x0000000000000000ull, 0x26f351083faad734ull},
  {38, 0x0000000000000001ull, 0x0d80c23b267c3c4bull},
  {38, 0x00007f3a1c5e2000ull, 0x8625c4c3de076dc8ull},
  {38, 0xdeadbeefcafef00dull, 0x8e8bc50def3a543aull},
  {38, 0xffffffffffffffffull, 0x2c7ebc2d85887348ull},
  {39, 0x0000000000000000ull, 0x7092ba3068a8c2ddull},
  {39, 0x0000000000000001ull, 0x060cd0312a5e3944ull},
  {39, 0x00007f3a1c5e2000ull, 0x846d86401ca3a1beull},
  {39, 0xdeadbeefcafef00dull, 0x474b772fc7c3db62ull},
  {39, 0xffffffffffffffffull, 0xf3e4d51ed6de1e10ull},
  {40, 0x0000000000000000ull, 0xd39acbcdae246a39ull},
  {40, 0x0000000000000001ull, 0xdd6057380e7ae16bull},
  {40, 0x00007f3a1c5e2000ull, 0x09b53193f2cc7b50ull},
  {40, 0xdeadbeefcafef00dull, 0x7717dc99919b811dull},
  {40, 0xffffffffffffffffull, 0x6ffacd758211e311ull},
  {48, 0x0000000000000000ull, 0xc5cce2825cb2cb66ull},
  {48, 0x0000000000000001ull, 0xae304b383188ba01ull},
  {48, 0x00007f3a1c5e2000ull, 0xa5b030ddbca0a0c2ull},
  {48, 0xdeadbeefcafef00dull, 0xa12330ccf4e2babcull},
  {48, 0xffffffffffffffffull, 0x1f55e31bc0510ef7ull},
  {63, 0x0000000000000000ull, 0xda6e593cc66c22f7ull},
  {63, 0x0000000000000001ull, 0x5e2840f997435d3dull},
  {63, 0x00007f3a1c5e2000ull, 0x4f0c224dbcf87c70ull},
  {63, 0xdeadbeefcafef00dull, 0x58ff689c0b88fd05ull},
  {63, 0xffffffffffffffffull, 0xf143824eb90e73f2ull},
  {64, 0x0000000000000000ull, 0x06736b3571a10982ull},
  {64, 0x0000000000000001ull, 0xdea409dfd2bebd11ull},
  {64, 0x00007f3a1c5e2000ull, 0x2f3cc94d5de778cbull},
  {64, 0xdeadbeefcafef00dull, 0xa1222a4149477856ull},
  {64, 0xffffffffffffffffull, 0x2255024593012b94ull},
  {65, 0x0000000000000000ull, 0xef2024b452b4ca02ull},
  {65, 0x0000000000000001ull, 0xbf41a385b8135ff2ull},
  {65, 0x00007f3a1c5e2000ull, 0x481a0851dcc9da46ull},
  {65, 0xdeadbeefcafef00dull, 0x6f9a6fb69d7fce31ull},
  {65, 0xffffffffffffffffull, 0xd9160904ddb17481ull},
  {96, 0x0000000000000000ull, 0x33288429d0257b30ull},
  {96, 0x0000000000000001ull, 0xfe27af1f5825d82dull},
  {96, 0x00007f3a1c5e2000ull, 0x9b53dda273d1d6edull},
  {96, 0xdeadbeefcafef00dull, 0x2574401735a762d6ull},
  {96, 0xffffffffffffffffull, 0x83b479fa9b67133dull},
  {100, 0x0000000000000000ull, 0x8531ea7e7ecf600eull},
  {100, 0x0000000000000001ull, 0xeac3ddb1914f8d41ull},
  {100, 0x00007f3a1c5e2000ull, 0x995462322c162c33ull},
  {100, 0xdeadbeefcafef00dull, 0xd5f3c461096cb51aull},
  {100, 0xffffffffffffffffull, 0xa0f842ca3e61d1a1ull},
  {127, 0x0000000000000000ull, 0x61308964d3074a0dull},
  {127, 0x0000000000000001ull, 0xb659b2b4750a692eull},
  {127, 0x00007f3a1c5e2000ull, 0x4cde15eaf4117e34ull},
  {127, 0xdeadbeefcafef00dull, 0xffffd5b5178eb901ull},
  {127, 0xffffffffffffffffull, 0xfe64a99d5aef80fbull},
  {128, 0x0000000000000000ull, 0xb29640da6f4518d7ull},
  {128, 0x0000000000000001ull, 0x09a0c032401d1573ull},
  {128, 0x00007f3a1c5e2000ull, 0xa5ca7f68160f08b5ull},
  {128, 0xdeadbeefcafef00dull, 0x38701d835fe29ebaull},
  {128, 0xffffffffffffffffull, 0xfdf9a175db6d3f81ull},
  {129, 0x0000000000000000ull, 0xf72be7f4cb199459ull},
  {129, 0x0000000000000001ull, 0x0ac1d1ae32330fa3ull},
  {129, 0x00007f3a1c5e2000ull, 0xd6b72d1fd58044f6ull},
  {129, 0xdeadbeefcafef00dull, 0x29b0c8af74d94a20ull},
  {129, 0xffffffffffffffffull, 0x6d41240cb879fcafull},
  {200, 0x0000000000000000ull, 0xbdc3626a350f1313ull},
  {200, 0x0000000000000001ull, 0x8434b863abf1e834ull},
  {200, 0x00007f3a1c5e2000ull, 0x02520796ec6d954full},
  {200, 0xdeadbeefcafef00dull, 0xbe8a3e5f753c7d8bull},
  {200, 0xffffffffffffffffull, 0x1e89db24cccbbb91ull},
  {255, 0x0000000000000000ull, 0x7c82e7154d567198ull},
  {255, 0x0000000000000001ull, 0x86d40bc4f05fe1e6ull},
  {255, 0x00007f3a1c5e2000ull, 0x6e5a5caa0b64e5e4ull},
  {255, 0xdeadbeefcafef00dull, 0x69ce84fd98eb34f9ull},
  {255, 0xffffffffffffffffull, 0xab464122c7305ab1ull},
  {256, 0x0000000000000000ull, 0x1ae78e7fb35e098cull},
  {256, 0x0000000000000001ull, 0x38a260e8d829ecfdull},
  {256, 0x00007f3a1c5e2000ull, 0xe90b44a64ec18cdaull},
  {256, 0xdeadbeefcafef00dull, 0xc00049940205a0f9ull},
  {256, 0xffffffffffffffffull, 0x118d30be6adf607full},
  {511, 0x0000000000000000ull, 0xe49a51b79a653684ull},
  {511, 0x0000000000000001ull, 0x003be9988cc142f3ull},
  {511, 0x00007f3a1c5e2000ull, 0xe812a20861c6f00bull},
  {511, 0xdeadbeefcafef00dull, 0x1c8707f5425e498dull},
  {511, 0xffffffffffffffffull, 0x43a681565eb3a30bull},
  {512, 0x0000000000000000ull, 0xd59d0e41a4ee46eeull},
  {512, 0x0000000000000001ull, 0x4f4aebb3f0dd7238ull},
  {512, 0x00007f3a1c5e2000ull, 0xe55c32d04fb1ae85ull},
  {512, 0xdeadbeefcafef00dull, 0x43f5cf9052ca62dbull},
  {512, 0xffffffffffffffffull, 0xa6ed1392c84d8dd5ull},
  {1000, 0x0000000000000000ull, 0x205e23a9bbf5960cull},
  {1000, 0x0000000000000001ull, 0xe0c13e786879c839ull},
  {1000, 0x00007f3a1c5e2000ull, 0xda7ec9c9a10114d4ull},
  {1000, 0xdeadbeefcafef00dull, 0x792a737f5a5e25b6ull},
  {1000, 0xffffffffffffffffull, 0x8972ba9a2cf6b7a9ull},
  {1023, 0x0000000000000000ull, 0xa12273900956e5baull},
  {1023, 0x0000000000000001ull, 0x09113c399a9fe3ccull},
  {1023, 0x00007f3a1c5e2000ull, 0xdfd045abd016f186ull},
  {1023, 0xdeadbeefcafef00dull, 0xb4227d6376a3344bull},
  {1023, 0xffffffffffffffffull, 0x5ef49d99312050bfull},
  {1024, 0x0000000000000000ull, 0x3963585b2d65120eull},
  {1024, 0x0000000000000001ull, 0x7497ceb1238953daull},
  {1024, 0x00007f3a1c5e2000ull, 0xfea7cdf4e4bcb04full},
  {1024, 0xdeadbeefcafef00dull, 0x0f9c24f030a2844bull},
  {1024, 0xffffffffffffffffull, 0xb519623f71ee9b93ull},
  {1025, 0x0000000000000000ull, 0x10a91936e5edebd5ull},
  {1025, 0x0000000000000001ull, 0x52fd918c838a2288ull},
  {1025, 0x00007f3a1c5e2000ull, 0x23ce3cdbdcc7cf95ull},
  {1025, 0xdeadbeefcafef00dull, 0xc79b9a82237f59c1ull},
  {1025, 0xffffffffffffffffull, 0x837800eaed759d11ull},
  {1100, 0x0000000000000000ull, 0xfaa9ee0ed03d2c89ull},
  {1100, 0x0000000000000001ull, 0x0384851a45c355f4ull},
  {1100, 0x00007f3a1c5e2000ull, 0xde1952ec4f3c5236ull},
  {1100, 0xdeadbeefcafef00dull, 0x1a4e22c1ae1b77ebull},
  {1100, 0xffffffffffffffffull, 0x3d8accbb9dac63f6ull},
  {2048, 0x0000000000000000ull, 0xdac515390c321b25ull},
  {2048, 0x0000000000000001ull, 0xf28e28f65b6a4000ull},
  {2048, 0x00007f3a1c5e2000ull, 0x0b5e3fe916f8cc91ull},
  {2048, 0xdeadbeefcafef00dull, 0xd3c6865c3733490bull},
  {2048, 0xffffffffffffffffull, 0x618201e36ea2b40bull},
  {2049, 0x0000000000000000ull, 0x1e4cd0c19430bd16ull},
  {2049, 0x0000000000000001ull, 0xeac2fd6fd2d53da3ull},
  {2049, 0x00007f3a1c5e2000ull, 0x744c1525085b0754ull},
  {2049, 0xdeadbeefcafef00dull, 0x314437919a7e3cbbull},
  {2049, 0xffffffffffffffffull, 0xb53225ddbecd9f92ull},
  {3073, 0x0000000000000000ull, 0xb73e4f567e6a8313ull},
  {3073, 0x0000000000000001ull, 0x4749e4ae104dd71bull},
  {3073, 0x00007f3a1c5e2000ull, 0xb619cf72f896fe65ull},
  {3073, 0xdeadbeefcafef00dull, 0xa7320fc72d7e3a60ull},
  {3073, 0xffffffffffffffffull, 0x161e8efb55019b77ull},
  {4097, 0x0000000000000000ull, 0xf419ce828bfb6476ull},
  {4097, 0x0000000000000001ull, 0xbc13fd40deb31371ull},
  {4097, 0x00007f3a1c5e2000ull, 0xc0b82de4ed34ec33ull},
  {4097, 0xdeadbeefcafef00dull, 0xf8a5759f165272b8ull},
  {4097, 0xffffffffffffffffull, 0x37473e1c35f07e1full},
  {8192, 0x0000000000000000ull, 0x6fa42d2176be2e94ull},
  {8192, 0x0000000000000001ull, 0x5fa5dc1c75c60689ull},
  {8192, 0x00007f3a1c5e2000ull, 0x34198cbbc08e7c9aull},
  {8192, 0xdeadbeefcafef00dull, 0xc1a831d2618e8564ull},
  {8192, 0xffffffffffffffffull, 0xc603888777439736ull},
};
#else
/* len 33..8192, ARM crypto LowLevelHash, recorded on an Apple M2 Pro (Apple clang 17, default flags) */
static const struct vec VEC_GT32[] = {
  {33, 0x0000000000000000ull, 0x4ed5269ac4b9991aull},
  {33, 0x0000000000000001ull, 0x650974a4a02bf256ull},
  {33, 0x00007f3a1c5e2000ull, 0x4212ac1cb2eb1041ull},
  {33, 0xdeadbeefcafef00dull, 0xb9d1d22660147348ull},
  {33, 0xffffffffffffffffull, 0x026915124328d085ull},
  {34, 0x0000000000000000ull, 0xd58a4f4d5a61bd82ull},
  {34, 0x0000000000000001ull, 0xef4e3b0512737252ull},
  {34, 0x00007f3a1c5e2000ull, 0x3c670588a5d2f381ull},
  {34, 0xdeadbeefcafef00dull, 0xf97ea49c822c2d8cull},
  {34, 0xffffffffffffffffull, 0x76f0e36556598268ull},
  {35, 0x0000000000000000ull, 0xf389227341bd77ccull},
  {35, 0x0000000000000001ull, 0xc0a6992d84e75ea7ull},
  {35, 0x00007f3a1c5e2000ull, 0x5ec49fb6065341c6ull},
  {35, 0xdeadbeefcafef00dull, 0x24f4cd2a6e128bfdull},
  {35, 0xffffffffffffffffull, 0xf7f3a9d48d3da0ecull},
  {36, 0x0000000000000000ull, 0xba11abbc114a2f8aull},
  {36, 0x0000000000000001ull, 0x06cfa960aeb8c6c2ull},
  {36, 0x00007f3a1c5e2000ull, 0xd7045fc64895f080ull},
  {36, 0xdeadbeefcafef00dull, 0xf8fcaba3527fe918ull},
  {36, 0xffffffffffffffffull, 0x8a8c6590049778caull},
  {37, 0x0000000000000000ull, 0xc4539a7b690c7f4aull},
  {37, 0x0000000000000001ull, 0x6ccb25dc6ae1b8cfull},
  {37, 0x00007f3a1c5e2000ull, 0xf7878a4a22406ffcull},
  {37, 0xdeadbeefcafef00dull, 0xf342cb58a7be9ac9ull},
  {37, 0xffffffffffffffffull, 0x72566345089e9a97ull},
  {38, 0x0000000000000000ull, 0xb43de8fc34247f02ull},
  {38, 0x0000000000000001ull, 0xc72074ae5b50e727ull},
  {38, 0x00007f3a1c5e2000ull, 0xbdf7dd84f8376eebull},
  {38, 0xdeadbeefcafef00dull, 0x56dbb5d47f0316efull},
  {38, 0xffffffffffffffffull, 0xc2f83fce83864a5aull},
  {39, 0x0000000000000000ull, 0x1b0e80d65fb8fb28ull},
  {39, 0x0000000000000001ull, 0x788c46a0dd76ccebull},
  {39, 0x00007f3a1c5e2000ull, 0xf4fea01b71f32c50ull},
  {39, 0xdeadbeefcafef00dull, 0x009e034c48f95b81ull},
  {39, 0xffffffffffffffffull, 0x5aae75073bdffcf8ull},
  {40, 0x0000000000000000ull, 0xcb5c5b5a739a867aull},
  {40, 0x0000000000000001ull, 0xd3613082645f0943ull},
  {40, 0x00007f3a1c5e2000ull, 0xd1e209cd0ab6a02dull},
  {40, 0xdeadbeefcafef00dull, 0xbaa91a68836c9688ull},
  {40, 0xffffffffffffffffull, 0xc94d76a5361a6e97ull},
  {48, 0x0000000000000000ull, 0x70a5106f80d5d2a5ull},
  {48, 0x0000000000000001ull, 0x9eed56fa9fe1e902ull},
  {48, 0x00007f3a1c5e2000ull, 0x05c1373ecc1a2177ull},
  {48, 0xdeadbeefcafef00dull, 0xeb4837f2a3553ccaull},
  {48, 0xffffffffffffffffull, 0x808333185db07386ull},
  {63, 0x0000000000000000ull, 0x159e071397a3d928ull},
  {63, 0x0000000000000001ull, 0x93358253ebb951b1ull},
  {63, 0x00007f3a1c5e2000ull, 0xb06b20ea81d9a97full},
  {63, 0xdeadbeefcafef00dull, 0xee1bf43d6a1cf8b1ull},
  {63, 0xffffffffffffffffull, 0x6a864b3aa478f370ull},
  {64, 0x0000000000000000ull, 0xd0a2f35e0c98f639ull},
  {64, 0x0000000000000001ull, 0x68445aae18e90d67ull},
  {64, 0x00007f3a1c5e2000ull, 0xb56dab9f564f8971ull},
  {64, 0xdeadbeefcafef00dull, 0x6431e117517702aaull},
  {64, 0xffffffffffffffffull, 0xf72264b025ce3e57ull},
  {65, 0x0000000000000000ull, 0xd7b5d4b6fc53b391ull},
  {65, 0x0000000000000001ull, 0x4b24082f43994fcaull},
  {65, 0x00007f3a1c5e2000ull, 0x189844910bc2f03full},
  {65, 0xdeadbeefcafef00dull, 0x3ec90f5d818eb1ebull},
  {65, 0xffffffffffffffffull, 0x898a4d5ce8754125ull},
  {96, 0x0000000000000000ull, 0x4705c09ab19ab594ull},
  {96, 0x0000000000000001ull, 0x44292378b3dd311aull},
  {96, 0x00007f3a1c5e2000ull, 0x082eddfda9837c3bull},
  {96, 0xdeadbeefcafef00dull, 0x105b2b52107b1f97ull},
  {96, 0xffffffffffffffffull, 0x8b67188f22102d9bull},
  {100, 0x0000000000000000ull, 0x1801ab330638ca95ull},
  {100, 0x0000000000000001ull, 0x5efb76e980bbd814ull},
  {100, 0x00007f3a1c5e2000ull, 0xa5e818b71f946085ull},
  {100, 0xdeadbeefcafef00dull, 0x21a974f9be167658ull},
  {100, 0xffffffffffffffffull, 0x3721c2a30e2128daull},
  {127, 0x0000000000000000ull, 0xe1c095f54cb94ddcull},
  {127, 0x0000000000000001ull, 0x9983dd1af9466319ull},
  {127, 0x00007f3a1c5e2000ull, 0xb34d7de8e2a30d33ull},
  {127, 0xdeadbeefcafef00dull, 0xe299e564e15ad225ull},
  {127, 0xffffffffffffffffull, 0x774ef3a72323e9fbull},
  {128, 0x0000000000000000ull, 0x05983e13c2db982cull},
  {128, 0x0000000000000001ull, 0x9c446dfb6d08b767ull},
  {128, 0x00007f3a1c5e2000ull, 0xe49a9ca696e35777ull},
  {128, 0xdeadbeefcafef00dull, 0x382a97f71a62dcd3ull},
  {128, 0xffffffffffffffffull, 0x91137da5a188fdf5ull},
  {129, 0x0000000000000000ull, 0x7ac85d1e8b093520ull},
  {129, 0x0000000000000001ull, 0x126fc1c2f804de62ull},
  {129, 0x00007f3a1c5e2000ull, 0xd209782a41569f7full},
  {129, 0xdeadbeefcafef00dull, 0xdec122cf6af0f315ull},
  {129, 0xffffffffffffffffull, 0xbbb8e15f263acc68ull},
  {200, 0x0000000000000000ull, 0x12758f9c8865c5bcull},
  {200, 0x0000000000000001ull, 0x86703b2a6b3d4f95ull},
  {200, 0x00007f3a1c5e2000ull, 0x50496ed435646671ull},
  {200, 0xdeadbeefcafef00dull, 0x9c98bcee13ceef83ull},
  {200, 0xffffffffffffffffull, 0xd783c3a89365a859ull},
  {255, 0x0000000000000000ull, 0x40584ff1718b6eeaull},
  {255, 0x0000000000000001ull, 0x1c61bc9953f0d76dull},
  {255, 0x00007f3a1c5e2000ull, 0x92270620b59c6c47ull},
  {255, 0xdeadbeefcafef00dull, 0xdde448839564822full},
  {255, 0xffffffffffffffffull, 0xe9594ee235bb4d65ull},
  {256, 0x0000000000000000ull, 0x64ef7d3aba1faf18ull},
  {256, 0x0000000000000001ull, 0x1edeab5cffbe0512ull},
  {256, 0x00007f3a1c5e2000ull, 0x733fe05bb582fb2eull},
  {256, 0xdeadbeefcafef00dull, 0xfe4abb8781231ccdull},
  {256, 0xffffffffffffffffull, 0xa0757848a7e438fcull},
  {511, 0x0000000000000000ull, 0xbb8204bef42b2c4bull},
  {511, 0x0000000000000001ull, 0x5166b86b3fe79ad6ull},
  {511, 0x00007f3a1c5e2000ull, 0x6329ac89feff42d3ull},
  {511, 0xdeadbeefcafef00dull, 0xc2e0168e18f4f14bull},
  {511, 0xffffffffffffffffull, 0x699712a74a83d180ull},
  {512, 0x0000000000000000ull, 0x295d46643b34c2bdull},
  {512, 0x0000000000000001ull, 0x747d101a81a8465dull},
  {512, 0x00007f3a1c5e2000ull, 0xc00ec105dcc52f2full},
  {512, 0xdeadbeefcafef00dull, 0x30b597d42e027312ull},
  {512, 0xffffffffffffffffull, 0xedfffe5663f30266ull},
  {1000, 0x0000000000000000ull, 0x85671662e854e6d4ull},
  {1000, 0x0000000000000001ull, 0x0ebfdf42830cfebbull},
  {1000, 0x00007f3a1c5e2000ull, 0x0d2800eaa049a2a2ull},
  {1000, 0xdeadbeefcafef00dull, 0x6fcca2b645ed00a3ull},
  {1000, 0xffffffffffffffffull, 0xbcb05532a62db4c3ull},
  {1023, 0x0000000000000000ull, 0x448a75804c04dc85ull},
  {1023, 0x0000000000000001ull, 0xa0d32ec887fedd2aull},
  {1023, 0x00007f3a1c5e2000ull, 0x3a9a2cbba8671d82ull},
  {1023, 0xdeadbeefcafef00dull, 0x34cf27f2c9c3aaabull},
  {1023, 0xffffffffffffffffull, 0xd6d20e12b03f9fecull},
  {1024, 0x0000000000000000ull, 0xc77a6d56b74f2a3full},
  {1024, 0x0000000000000001ull, 0x2b03ebc81837b281ull},
  {1024, 0x00007f3a1c5e2000ull, 0xd87cf2841e2d5a3bull},
  {1024, 0xdeadbeefcafef00dull, 0x37949ad9706bba30ull},
  {1024, 0xffffffffffffffffull, 0x61cf8b9926bad6beull},
  {1025, 0x0000000000000000ull, 0xb2bfe3e665374e1bull},
  {1025, 0x0000000000000001ull, 0x9e8e6eb902fda995ull},
  {1025, 0x00007f3a1c5e2000ull, 0x2b4b73c040956d0cull},
  {1025, 0xdeadbeefcafef00dull, 0x8c2992696856f4efull},
  {1025, 0xffffffffffffffffull, 0x534b03629ea72b64ull},
  {1100, 0x0000000000000000ull, 0x0b3eb7d9dff9dfdeull},
  {1100, 0x0000000000000001ull, 0x8c8babd2e9a1e193ull},
  {1100, 0x00007f3a1c5e2000ull, 0xca00db984e17c93eull},
  {1100, 0xdeadbeefcafef00dull, 0x8998791370020848ull},
  {1100, 0xffffffffffffffffull, 0x9e4e25bcc67b02beull},
  {2048, 0x0000000000000000ull, 0xbf8fc7832043c93full},
  {2048, 0x0000000000000001ull, 0x5cfb3810c276dba8ull},
  {2048, 0x00007f3a1c5e2000ull, 0x9085950720f0b1d1ull},
  {2048, 0xdeadbeefcafef00dull, 0x3295aad915500794ull},
  {2048, 0xffffffffffffffffull, 0x3fe4383e9e29136cull},
  {2049, 0x0000000000000000ull, 0x5dcb979ddbe8b00dull},
  {2049, 0x0000000000000001ull, 0x2d83717f1ac7f5e9ull},
  {2049, 0x00007f3a1c5e2000ull, 0x420f2e1b4e1731baull},
  {2049, 0xdeadbeefcafef00dull, 0x8fcf09cbf8ada9faull},
  {2049, 0xffffffffffffffffull, 0xa7d1dcfa8d268decull},
  {3073, 0x0000000000000000ull, 0x9df7d17f8b14a754ull},
  {3073, 0x0000000000000001ull, 0xb5bbf5cba8473de9ull},
  {3073, 0x00007f3a1c5e2000ull, 0xe26eaa70e6a5dbeeull},
  {3073, 0xdeadbeefcafef00dull, 0x25faab4be9a2039bull},
  {3073, 0xffffffffffffffffull, 0x2a7237b38da7ac83ull},
  {4097, 0x0000000000000000ull, 0x5c5ac8ca3f7af7b5ull},
  {4097, 0x0000000000000001ull, 0x59648f2cb5b14932ull},
  {4097, 0x00007f3a1c5e2000ull, 0x8bfa82fc5d0d3d2aull},
  {4097, 0xdeadbeefcafef00dull, 0x2b19f42a50f103f9ull},
  {4097, 0xffffffffffffffffull, 0xb67b72e7c2538d0aull},
  {8192, 0x0000000000000000ull, 0x7c532b1d58b02272ull},
  {8192, 0x0000000000000001ull, 0x2177dfeab43fe6a8ull},
  {8192, 0x00007f3a1c5e2000ull, 0x7cb24a93376b6e28ull},
  {8192, 0xdeadbeefcafef00dull, 0x650e3df1186cc52eull},
  {8192, 0xffffffffffffffffull, 0xb584e047b2494cc1ull},
};
#endif
#define NVEC(t) (sizeof(t) / sizeof((t)[0]))

/* the vector schedule shared with native/absl_check.cc */
#define VEC_BUF 8192
static uint64_t splitmix64(uint64_t *s) {
  uint64_t z = (*s += 0x9E3779B97F4A7C15ull);
  z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull;
  z = (z ^ (z >> 27)) * 0x94D049BB133111EBull;
  return z ^ (z >> 31);
}
static void vec_fill(uint8_t *buf) {
  uint64_t s = 0x243F6A8885A308D3ull;
  for (size_t i = 0; i < VEC_BUF; i += 8) {
    uint64_t v = splitmix64(&s);
    for (int k = 0; k < 8; k++) buf[i + k] = (uint8_t)(v >> (8 * k));
  }
}
static const uint64_t vec_seeds[5] = {0, 1, 0x00007f3a1c5e2000ull, 0xdeadbeefcafef00dull, 0xffffffffffffffffull};
static const size_t vec_lens[] = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                                  21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
                                  48, 63, 64, 65, 96, 100, 127, 128, 129, 200, 255, 256, 511, 512, 1000, 1023,
                                  1024, 1025, 1100, 2048, 2049, 3073, 4097, 8192};

/* ------------------------------------------------------------------- RNG */
static uint64_t rotl(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
static uint64_t xs[4];
static void rng_seed(uint64_t seed) { for (int i = 0; i < 4; i++) xs[i] = splitmix64(&seed); }
static uint64_t rng_next(void) { /* xoshiro256** */
  uint64_t r = rotl(xs[1] * 5, 7) * 9, t = xs[1] << 17;
  xs[2] ^= xs[0]; xs[3] ^= xs[1]; xs[1] ^= xs[2]; xs[0] ^= xs[3]; xs[2] ^= t; xs[3] = rotl(xs[3], 45);
  return r;
}

/* ------------------------------------------------------------------ pairs */
struct pair {
  const char *title, *mechanism, *a_hex, *b_hex;
  uint64_t h_seed0, h_seed4055c8; /* recorded from the real library (native/absl_check pairs) */
};
static const struct pair PAIRS[3] = {
    {"Pair 1: 0-byte vs 8-byte cross-length pair (L = 1 word), key-free -- the post's row",
     "len 0: Mix(seed ^ D(0) ^ 0x57, kMul); len 8: Mix(seed ^ D(8) ^ v, kMul) with v = load32(m')<<32 | load32(m'+4).\n"
     "   Choosing m' so that v = D(0) ^ D(8) ^ 0x57 makes the two multiplicands identical for every seed.",
     "", "a6e02637c07bd386", 0x2bda3ac53577c4b7ull, 0x06bb4292e0eae907ull},
    {"Pair 2: 1-byte vs 8-byte cross-length pair (L = 1 word), key-free -- non-empty variant of the same identity",
     "len 1: v = p[0]<<16 | p[0]<<8 | p[0] = 0x616161 for 'a'; m' packs v = D(1) ^ D(8) ^ 0x616161.",
     "61", "44b53d572db1948b", 0xa65e8ab2c950c927ull, 0xe3a0eef311f0006aull},
    {"Pair 3: 16-byte same-length pair (L = 2 words), key-free -- the post's other_pair",
     "len 9..16: Mix(seed ^ D(16) ^ w0, kMul ^ w1) with w1 = the last 8 bytes.  With w1 = kMul the second\n"
     "   multiplicand is 0, so every such string hashes to 0 for every seed (a 2^64-way multicollision).",
     "6162636465666768f58c1edee0f9d579", "4142434445464748f58c1edee0f9d579", 0, 0}};

static size_t unhex(const char *s, uint8_t *out) {
  size_t n = 0;
  for (; s[0] && s[1]; s += 2) { unsigned b; sscanf(s, "%2x", &b); out[n++] = (uint8_t)b; }
  return n;
}
static void print_hex(const uint8_t *p, size_t n) {
  if (n == 0) { printf("(empty)"); return; }
  for (size_t i = 0; i < n; i++) printf("%02x", p[i]);
}
/* inverse of Read4To8 for len 8: bytes whose packing is v */
static void pack8(uint64_t v, uint8_t *out) {
  uint32_t hi = (uint32_t)(v >> 32), lo = (uint32_t)v;
  memcpy(out, &hi, 4); memcpy(out + 4, &lo, 4);
}

static int usage(void) {
  fprintf(stderr, "usage: abseil_hash_verify [log2 N (0..40), default 20] [rng seed, default 1] | --vectors\n");
  return 2;
}

int main(int argc, char **argv) {
  static uint8_t buf[VEC_BUF + 64];
  vec_fill(buf);
  if (argc > 1 && strcmp(argv[1], "--vectors") == 0) {
    for (size_t i = 0; i < sizeof(vec_lens) / sizeof(vec_lens[0]); i++)
      for (int s = 0; s < 5; s++)
        printf("%zu %016" PRIx64 " %016" PRIx64 "\n", vec_lens[i], vec_seeds[s], absl_hash(vec_seeds[s], buf, vec_lens[i]));
    return 0;
  }
  int lg = 20; uint64_t rseed = 1;
  if (argc > 1) { char *e; long v = strtol(argv[1], &e, 10); if (*argv[1] == 0 || *e || v < 0 || v > 40) return usage(); lg = (int)v; }
  if (argc > 2) { char *e; rseed = strtoull(argv[2], &e, 0); if (*argv[2] == 0 || *e) return usage(); }
  if (argc > 3) return usage();
  uint64_t N = 1ull << lg;

  printf("absl::Hash key-free collision check  (LowLevelHash backend for len > 32: %s)\n", BACKEND_NAME);
  printf("hash: absl::Hash<std::string_view>, abseil-cpp 73d2688 (= LTS 20260817.0), re-implemented from absl/hash/internal/hash.{h,cc}\n");

  /* 1. vectors recorded from the real library */
  size_t bad = 0, n = 0;
  for (size_t i = 0; i < NVEC(VEC_LE32); i++, n++) {
    const struct vec *v = &VEC_LE32[i];
    if (absl_hash(v->seed, buf, v->len) != v->hash) { bad++; if (bad <= 5) printf("  MISMATCH len %u seed %016" PRIx64 "\n", v->len, v->seed); }
  }
  for (size_t i = 0; i < NVEC(VEC_GT32); i++, n++) {
    const struct vec *v = &VEC_GT32[i];
    if (absl_hash(v->seed, buf, v->len) != v->hash) { bad++; if (bad <= 5) printf("  MISMATCH len %u seed %016" PRIx64 "\n", v->len, v->seed); }
  }
  printf("validation: %zu vectors recorded from the real library (len 0..8192, 5 seeds): %zu mismatches %s\n", n, bad, bad ? "FAIL" : "OK");
  if (bad) { printf("FAILED: implementation does not match the real absl::Hash\n"); return 1; }

  /* 2. re-derive the pairs from the constants and check the recorded values */
  uint8_t d8[8], want[32];
  int ok = 1;
  pack8(D(0) ^ D(8) ^ 0x57, d8); unhex(PAIRS[0].b_hex, want);
  int r1 = memcmp(d8, want, 8) == 0; ok &= r1;
  printf("validation: pair 1 recipe bytes8(D(0) ^ D(8) ^ 0x57) = "); print_hex(d8, 8); printf(" %s\n", r1 ? "matches the published hex" : "DIFFERS");
  pack8(D(1) ^ D(8) ^ 0x616161, d8); unhex(PAIRS[1].b_hex, want);
  int r2 = memcmp(d8, want, 8) == 0; ok &= r2;
  printf("validation: pair 2 recipe bytes8(D(1) ^ D(8) ^ 0x616161) = "); print_hex(d8, 8); printf(" %s\n", r2 ? "matches the published hex" : "DIFFERS");
  uint8_t km[8]; for (int j = 0; j < 8; j++) km[j] = (uint8_t)(KMUL >> (8 * j));
  uint8_t p3a[16], p3b[16]; unhex(PAIRS[2].a_hex, p3a); unhex(PAIRS[2].b_hex, p3b);
  int r3 = memcmp(p3a + 8, km, 8) == 0 && memcmp(p3b + 8, km, 8) == 0; ok &= r3;
  printf("validation: pair 3 last 8 bytes = kMul little-endian ("); print_hex(km, 8); printf(") on both strings: %s\n", r3 ? "yes" : "NO");
  if (!ok) { printf("FAILED: a pair recipe does not reproduce the published strings\n"); return 1; }

  /* 3. the pairs */
  int fail = 0;
  for (int pi = 0; pi < 3; pi++) {
    const struct pair *P = &PAIRS[pi];
    uint8_t a[64] = {0}, b[64] = {0};
    size_t la = unhex(P->a_hex, a), lb = unhex(P->b_hex, b);
    printf("\n%s\n   mechanism: %s\n", P->title, P->mechanism);
    printf("  m  (%2zu bytes) = ", la); print_hex(a, la); printf("\n");
    printf("  m' (%2zu bytes) = ", lb); print_hex(b, lb); printf("\n");
    /* recorded values from the real library */
    uint64_t s0a = absl_hash(0, a, la), s0b = absl_hash(0, b, lb);
    uint64_t s1a = absl_hash(0x4055c8, a, la), s1b = absl_hash(0x4055c8, b, lb);
    int rec = s0a == s0b && s0a == P->h_seed0 && s1a == s1b && s1a == P->h_seed4055c8;
    printf("  seed 0x0:      H(m) = %016" PRIx64 "  H(m') = %016" PRIx64 "  (real library: %016" PRIx64 ")\n", s0a, s0b, P->h_seed0);
    printf("  seed 0x4055c8: H(m) = %016" PRIx64 "  H(m') = %016" PRIx64 "  (real library: %016" PRIx64 ")  %s\n", s1a, s1b, P->h_seed4055c8,
           rec ? "recorded values reproduced, both COLLIDE" : "MISMATCH");
    if (!rec) fail = 1;
    /* the 32 SwissTable seeds */
    uint64_t tab = 0;
    for (uint64_t t = 0; t < 32; t++) tab += absl_hash(t << 6, a, la) == absl_hash(t << 6, b, lb);
    printf("  SwissTable per-table seeds {0, 64, ..., 1984} (exhaustive): collisions = %" PRIu64 " / 32\n", tab);
    if (tab != 32) fail = 1;
    /* N uniform seeds */
    rng_seed(rseed);
    uint64_t c = 0, first_s = 0, first_h = 0; int have = 0;
    for (uint64_t i = 0; i < N; i++) {
      uint64_t s = rng_next();
      uint64_t ha = absl_hash(s, a, la), hb = absl_hash(s, b, lb);
      if (ha == hb) { c++; if (!have) { have = 1; first_s = s; first_h = ha; } }
    }
    double rate = (double)c / (double)N;
    printf("  uniform 64-bit seeds, N = %" PRIu64 " (2^%d): collisions = %" PRIu64 " / %" PRIu64 ", rate = %.6f, log2 rate = %s%.3f\n",
           N, lg, c, N, rate, c ? "" : "-inf ", c ? log2(rate) : 0.0);
    if (have) printf("  first random colliding seed 0x%016" PRIx64 ": H(m) = H(m') = %016" PRIx64 "\n", first_s, first_h);
    if (c != N) { fail = 1; printf("  FAIL: %" PRIu64 " sampled seeds did not collide\n", N - c); }
  }
  printf("\n%s\n", fail ? "FAILED" : "ALL CHECKS PASSED: all three pairs collide for every SwissTable seed and every sampled 64-bit seed.");
  return fail;
}
