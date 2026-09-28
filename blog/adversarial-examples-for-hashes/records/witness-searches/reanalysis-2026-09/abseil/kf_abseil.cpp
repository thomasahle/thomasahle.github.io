/* absl::Hash<std::string_view> (abseil-cpp 73d2688 = LTS 20260817.0), strings of 0..32 bytes, both compile-time
 * variants of absl/hash/internal/hash.h, transcribed from that file:
 *   default  (ABSL_OPTION_INLINE_HW_ACCEL_STRATEGY 0, the shipped options.h): PrecombineLengthMix + Mix paths;
 *   crc      (strategy != 0 on SSE4.2 x86-64 / ARMv8-CRC): CRC32C path.
 * Seed = the 64-bit state passed by hash_internal::HashWithSeed (SwissTable per-table seed, or Seed()).
 * Validated against vectors printed by the real library in both builds (vectors.inc). */
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>
typedef unsigned __int128 u128;
static const uint64_t kMul = 0x79d5f9e0de1e8cf5ULL;
static const uint64_t kStaticRandomData[] = {0x243f6a8885a308d3ULL, 0x13198a2e03707344ULL, 0xa4093822299f31d0ULL,
                                             0x082efa98ec4e6c89ULL, 0x452821e638d01377ULL};
static uint64_t Read8(const uint8_t *p) { uint64_t v; memcpy(&v, p, 8); return v; }
static uint32_t Read4(const uint8_t *p) { uint32_t v; memcpy(&v, p, 4); return v; }
static uint64_t Mix(uint64_t a, uint64_t b) { u128 m = (u128)a * b; return (uint64_t)(m >> 64) ^ (uint64_t)m; }
static uint64_t PrecombineLengthMix(uint64_t state, size_t len) { return state ^ Read8((const uint8_t *)kStaticRandomData + len); }
static uint64_t Read4To8(const uint8_t *p, size_t len) { return ((uint64_t)Read4(p) << 32) | Read4(p + len - 4); }
static uint32_t Read1To3(const uint8_t *p, size_t len) { return ((uint32_t)p[0] << 16) | p[len - 1] | ((uint32_t)p[len / 2] << 8); }
static uint64_t hash_default(const uint8_t *p, size_t len, uint64_t seed) {
  if (len <= 8) { uint64_t st = PrecombineLengthMix(seed, len), v;
    if (len >= 4) v = Read4To8(p, len); else if (len > 0) v = Read1To3(p, len); else v = 0x57;
    return Mix(st ^ v, kMul); }                                               /* CombineRawImpl, no CRC */
  if (len <= 16) { uint64_t st = PrecombineLengthMix(seed, len); return Mix(st ^ Read8(p), kMul ^ Read8(p + len - 8)); }
  if (len <= 32) { uint64_t st = PrecombineLengthMix(seed, len);
    uint64_t m0 = Mix(Read8(p) ^ kStaticRandomData[1], Read8(p + 8) ^ st);
    const uint8_t *t = p + (len - 16); uint64_t m1 = Mix(Read8(t) ^ kStaticRandomData[3], Read8(t + 8) ^ st); return m0 ^ m1; }
  fprintf(stderr, "len > 32 not transcribed\n"); exit(3);
}
/* software CRC32C with the semantics of _mm_crc32_u8/u32/u64 (reflected poly 0x82F63B78, no inversion) */
static uint32_t crc_bytes(uint32_t c, const uint8_t *d, int n) { for (int i = 0; i < n; i++) { c ^= d[i]; for (int k = 0; k < 8; k++) c = (c >> 1) ^ (0x82F63B78u & (0u - (c & 1))); } return c; }
static uint64_t crc64(uint64_t c, uint64_t v) { uint8_t b[8]; memcpy(b, &v, 8); return crc_bytes((uint32_t)c, b, 8); }
static uint64_t crc32w(uint32_t c, uint32_t v) { uint8_t b[4]; memcpy(b, &v, 4); return crc_bytes(c, b, 4); }
static uint64_t crc8(uint32_t c, uint8_t v) { return crc_bytes(c, &v, 1); }
static uint64_t rotr(uint64_t x, int r) { r &= 63; return r ? (x >> r) | (x << (64 - r)) : x; }
static uint64_t hash_crc(const uint8_t *p, size_t len, uint64_t state) {
  if (len > 32) { fprintf(stderr, "len > 32 not transcribed\n"); exit(3); }
  uint64_t mul = rotr(kMul, (int)len);
  uint64_t f = state + 8 * len, s = __builtin_bswap64(state);
  if (len > 8) { f = crc64(f, Read8(p)); s = crc64(s, Read8(p + len - 8));
    if (len > 16) { f = crc64(f, Read8(p + len - 16)); s = crc64(s, Read8(p + 8)); } }
  else if (len >= 4) { f = crc32w((uint32_t)f, Read4(p)); s = crc32w((uint32_t)s, Read4(p + len - 4)); }
  else if (len >= 1) { f = crc8((uint32_t)f, p[0]); s = crc8((uint32_t)s, p[len - 1]); mul += p[len / 2]; }
  return Mix(mul - f, s - mul);
}
/* real-library vectors, abseil-cpp 73d2688 via hash_internal::HashWithSeed; message byte i = (uint8_t)(37*i+11) */
static const struct { int len; uint64_t seed, h_default, h_crc; } kVec[] = {
  {0, 0x0000000000000000ULL, 0x2bda3ac53577c4b7ULL, 0xce33b9f134fbcac6ULL},
  {0, 0x0000000000000001ULL, 0xa5303ce4d31571aaULL, 0x1fec65c8f3ff5fb2ULL},
  {0, 0x0123456789abcdefULL, 0xacf53861def408afULL, 0xd3e074f75b536aa8ULL},
  {0, 0xfedcba9876543210ULL, 0xa0e1346218f6ee8fULL, 0x8f23e0b71fdb97e6ULL},
  {1, 0x0000000000000000ULL, 0x6e6d8109c476d637ULL, 0xf2b984bae2108109ULL},
  {1, 0x0000000000000001ULL, 0xf4438b691a17590dULL, 0xc92ec12e8575da40ULL},
  {1, 0x0123456789abcdefULL, 0x55790d1ebcad515eULL, 0x123a744e13275cd9ULL},
  {1, 0xfedcba9876543210ULL, 0x487d7150b82a195dULL, 0x3c4ecc503a6d8c19ULL},
  {2, 0x0000000000000000ULL, 0x98246023119336cdULL, 0x4846037e9c1a54c2ULL},
  {2, 0x0000000000000001ULL, 0x03f27ec2737483d8ULL, 0xfc75ccce6627cfb3ULL},
  {2, 0x0123456789abcdefULL, 0x4928fab7239c0f72ULL, 0xc0ef9affb49752ceULL},
  {2, 0xfedcba9876543210ULL, 0x45acfd3723d92f49ULL, 0xad243b4cb712ff0bULL},
  {3, 0x0000000000000000ULL, 0x9f9a43cd9625b19aULL, 0x38d7f3ac08859247ULL},
  {3, 0x0000000000000001ULL, 0x16704c2cb7c4028eULL, 0xa46e24818334efd6ULL},
  {3, 0x0123456789abcdefULL, 0x77bc9f8f6f0b0f8dULL, 0x6e5609ab431c1305ULL},
  {3, 0xfedcba9876543210ULL, 0x00309fc86f4a2feeULL, 0xe49fcbbf59420c49ULL},
  {4, 0x0000000000000000ULL, 0x681a643a967bf3a1ULL, 0x6882fcafdbc71fbcULL},
  {4, 0x0000000000000001ULL, 0xee401e5bf05a0697ULL, 0xc26bfeb6af7bbfc6ULL},
  {4, 0x0123456789abcdefULL, 0x5292c48076e05403ULL, 0x9c8540620a080c51ULL},
  {4, 0xfedcba9876543210ULL, 0x5a2ad84646e5a63aULL, 0x5c25966ea37b1788ULL},
  {5, 0x0000000000000000ULL, 0x2500bb22ab89f944ULL, 0x10f74f96c937affaULL},
  {5, 0x0000000000000001ULL, 0xaedab10249b77471ULL, 0x4bab22e82250272cULL},
  {5, 0x0123456789abcdefULL, 0x01b92ec3a9cdbec1ULL, 0x3f1492941b697243ULL},
  {5, 0xfedcba9876543210ULL, 0x0c3d4e802d883f2eULL, 0xd38543f3bf704448ULL},
  {6, 0x0000000000000000ULL, 0xcc8a50d589a1c7cbULL, 0x8da25718f9c48d47ULL},
  {6, 0x0000000000000001ULL, 0x4abc5abaa64374ddULL, 0x54ab7022062e7121ULL},
  {6, 0x0123456789abcdefULL, 0x1e8757ab594c94cdULL, 0x9fb1bcaab0e87813ULL},
  {6, 0xfedcba9876543210ULL, 0x1f3747931a8a86b2ULL, 0xb35903a5dc1031dbULL},
  {7, 0x0000000000000000ULL, 0xf43fa9032a178017ULL, 0x8772fc2a40599204ULL},
  {7, 0x0000000000000001ULL, 0x7a09af620be93d09ULL, 0x009180b6ef5b88a6ULL},
  {7, 0x0123456789abcdefULL, 0x9677833c17afde2aULL, 0xb125c4f66c2bb553ULL},
  {7, 0xfedcba9876543210ULL, 0x8607733fd7afc002ULL, 0x1bdec1aa1db8d483ULL},
  {8, 0x0000000000000000ULL, 0xa6f47763099d4540ULL, 0x10f40547293416a8ULL},
  {8, 0x0000000000000001ULL, 0x290e70822ffbd675ULL, 0x27d895fe17488dabULL},
  {8, 0x0123456789abcdefULL, 0xe0364a91f7831f43ULL, 0x6d5e3217f8671c3eULL},
  {8, 0xfedcba9876543210ULL, 0xfc862addc7c01f2bULL, 0x18ad603d26b9770cULL},
  {9, 0x0000000000000000ULL, 0x5b31cc44244b5ed8ULL, 0xef206c6f9c5c919cULL},
  {9, 0x0000000000000001ULL, 0x25eefc6866e4249dULL, 0xab684e737097111eULL},
  {9, 0x0123456789abcdefULL, 0x03e26c988ca0cbceULL, 0xa8eb32a62ce2a997ULL},
  {9, 0xfedcba9876543210ULL, 0x7be84cc002daabcfULL, 0xec9fc69e4118cce0ULL},
  {10, 0x0000000000000000ULL, 0xdf6ff2cc09b0f1eaULL, 0x54f31d6a18ecb2d6ULL},
  {10, 0x0000000000000001ULL, 0xbd16e9c51232c74aULL, 0x748affec0c329a79ULL},
  {10, 0x0123456789abcdefULL, 0xf696a7e6a78907aeULL, 0xbea9da9e565efb8aULL},
  {10, 0xfedcba9876543210ULL, 0xc6c4b7d0a78d1790ULL, 0x187e7be8f1e1bf34ULL},
  {11, 0x0000000000000000ULL, 0xb95c3b491a625746ULL, 0x573964b69fb0daecULL},
  {11, 0x0000000000000001ULL, 0xc2c9c05762487b39ULL, 0x0c767f8917f0e0feULL},
  {11, 0x0123456789abcdefULL, 0x8d5c7652b03746a4ULL, 0x1bd3edc50716e665ULL},
  {11, 0xfedcba9876543210ULL, 0x6a50785320272489ULL, 0xbcd8c07aea1fa0e1ULL},
  {12, 0x0000000000000000ULL, 0xfddf489db569ded7ULL, 0xdfa0fb5ed582f7feULL},
  {12, 0x0000000000000001ULL, 0x21862aae6460976dULL, 0xf6be8fdff1bdf65fULL},
  {12, 0x0123456789abcdefULL, 0xa205c87ec380408dULL, 0x20b7b394b3530213ULL},
  {12, 0xfedcba9876543210ULL, 0x1bb2047ee39352cdULL, 0x814a0b5baa42040eULL},
  {13, 0x0000000000000000ULL, 0x5d1dcf8a8957f7e9ULL, 0x1c958417c3b6bf1eULL},
  {13, 0x0000000000000001ULL, 0x9c954ac39e4750d7ULL, 0x37d2950ac648b3edULL},
  {13, 0x0123456789abcdefULL, 0xf4795f22014e5102ULL, 0xa2824ba171991424ULL},
  {13, 0xfedcba9876543210ULL, 0xf77e5eb0292d6ce2ULL, 0xb877391f2d139304ULL},
  {14, 0x0000000000000000ULL, 0xed766fca7f894574ULL, 0xa0853bf7ade59f56ULL},
  {14, 0x0000000000000001ULL, 0x9859d22ff1b7c099ULL, 0x2907686c2ce4f767ULL},
  {14, 0x0123456789abcdefULL, 0x21d12314d4dd4729ULL, 0x8ef3a013ae5be9c5ULL},
  {14, 0xfedcba9876543210ULL, 0x1d0dbbd0c6d847aeULL, 0x61263a5c5549f81aULL},
  {15, 0x0000000000000000ULL, 0x048e31b0314d3bd6ULL, 0x55a6438ae6476decULL},
  {15, 0x0000000000000001ULL, 0x6cf5766dd414fbddULL, 0xb685d61eeb747769ULL},
  {15, 0x0123456789abcdefULL, 0xc8e215593889992fULL, 0x0f86c36d4c5d9a1eULL},
  {15, 0xfedcba9876543210ULL, 0xa8f5915ec106192fULL, 0x2b433482d69ea376ULL},
  {16, 0x0000000000000000ULL, 0x7688936d5eaee805ULL, 0x7c1df51d3e1b2425ULL},
  {16, 0x0000000000000001ULL, 0xc6ccfe52dacad5faULL, 0xd5b62560a17416c8ULL},
  {16, 0x0123456789abcdefULL, 0x4aafd8a36da26ea7ULL, 0xb8b8f99695007765ULL},
  {16, 0xfedcba9876543210ULL, 0x2b2fa2d26c2b4d24ULL, 0x29bea29915c2962dULL},
  {17, 0x0000000000000000ULL, 0xc66ace9aba6685d5ULL, 0xac5827903338b7abULL},
  {17, 0x0000000000000001ULL, 0x9eba81ccae0c636fULL, 0x98d8fc150944e126ULL},
  {17, 0x0123456789abcdefULL, 0x0f5a9190287f0debULL, 0x1217233131ff3d1aULL},
  {17, 0xfedcba9876543210ULL, 0x469abf1814650a9fULL, 0xce0ce51d463c0314ULL},
  {18, 0x0000000000000000ULL, 0x9a77bc420612122dULL, 0x3f3e1a5b39b543beULL},
  {18, 0x0000000000000001ULL, 0xe85ef42ce6604a76ULL, 0x6aca4c7aa23db3ceULL},
  {18, 0x0123456789abcdefULL, 0x39f366f0599b759eULL, 0x5074d9986f5f0727ULL},
  {18, 0xfedcba9876543210ULL, 0x9d3cad506b74ff99ULL, 0x30bc6b4a6023b232ULL},
  {19, 0x0000000000000000ULL, 0x82970e86f1569d82ULL, 0xc0fefd5ff8557bc1ULL},
  {19, 0x0000000000000001ULL, 0x2f0e9662778052d9ULL, 0xd8427830b47487c3ULL},
  {19, 0x0123456789abcdefULL, 0xaf0d9c45bf687e6aULL, 0x1ca213acecc4c96eULL},
  {19, 0xfedcba9876543210ULL, 0x600e864599fb90acULL, 0x696c2e606c8959d9ULL},
  {20, 0x0000000000000000ULL, 0x44f5c2e72cdb3e5cULL, 0x51b521a0c6d02da6ULL},
  {20, 0x0000000000000001ULL, 0xd032d32870ad8aa5ULL, 0xabb8b896b722c1edULL},
  {20, 0x0123456789abcdefULL, 0x42da2c3875a2975eULL, 0x6faa18a1e4f2b6e2ULL},
  {20, 0xfedcba9876543210ULL, 0x570433143225802eULL, 0x7ad362fba643b2c7ULL},
  {21, 0x0000000000000000ULL, 0xb7fd236aff6dc167ULL, 0xa34a3e244013f750ULL},
  {21, 0x0000000000000001ULL, 0x0541e45896c938c6ULL, 0x7cb454c73c001349ULL},
  {21, 0x0123456789abcdefULL, 0x016d875ae68dbb4dULL, 0x87f011ab47afb31eULL},
  {21, 0xfedcba9876543210ULL, 0xfa5c89b6f44cb901ULL, 0x39b3e6a9b3edaf5aULL},
  {22, 0x0000000000000000ULL, 0x49ac75fda788c0eaULL, 0x1ebf5d49103c0ed5ULL},
  {22, 0x0000000000000001ULL, 0xb4c553a1d8d12659ULL, 0x333645f635aed714ULL},
  {22, 0x0123456789abcdefULL, 0x08bf3ab0bea0d791ULL, 0xa955d9200e0dc8bbULL},
  {22, 0xfedcba9876543210ULL, 0x0f5f05d1b1b2b313ULL, 0x1a2fe21e2306e3baULL},
  {23, 0x0000000000000000ULL, 0x0753833bb9dca9a1ULL, 0xd693cfb38c33a403ULL},
  {23, 0x0000000000000001ULL, 0xcb65703391ccd40eULL, 0x45fab5b0fd1744f4ULL},
  {23, 0x0123456789abcdefULL, 0x279cc25e7f9466a4ULL, 0xd88032e534489a53ULL},
  {23, 0xfedcba9876543210ULL, 0xe33c4298648aff55ULL, 0x4786fc8865991453ULL},
  {24, 0x0000000000000000ULL, 0x2d4eeda7cf53c212ULL, 0x52a144767083fd50ULL},
  {24, 0x0000000000000001ULL, 0x093f446ba25e521bULL, 0x5d1700b92f972755ULL},
  {24, 0x0123456789abcdefULL, 0x144e025cb30a7701ULL, 0x6b9a79c125eae62dULL},
  {24, 0xfedcba9876543210ULL, 0x154f8d9c01ed8d1dULL, 0x42f904e978ba7a24ULL},
  {25, 0x0000000000000000ULL, 0xa55e92dda46c3e6aULL, 0xbbcf0a6320bb978dULL},
  {25, 0x0000000000000001ULL, 0x9c679e1ae8ed65e8ULL, 0xe742f5fc8d0c6f77ULL},
  {25, 0x0123456789abcdefULL, 0xfe835cf60c73cc4dULL, 0x0146aecc785119c5ULL},
  {25, 0xfedcba9876543210ULL, 0x4091412fa29049b0ULL, 0x04afe427025cee6aULL},
  {26, 0x0000000000000000ULL, 0xcc218d7b4d9f0cdaULL, 0xb9825ebbc9f5add5ULL},
  {26, 0x0000000000000001ULL, 0x93a80343ca1a11f8ULL, 0x1beac20aa7b7caa4ULL},
  {26, 0x0123456789abcdefULL, 0xb83e4cd0c7596aedULL, 0x9506bda0828ed4eaULL},
  {26, 0xfedcba9876543210ULL, 0x848ef382d81f2af2ULL, 0x0dcebe5d7696a776ULL},
  {27, 0x0000000000000000ULL, 0xc9ba6a4c73270e2bULL, 0xda7da08fea64cd49ULL},
  {27, 0x0000000000000001ULL, 0x196546edee290497ULL, 0xf4d724ece61e0cb2ULL},
  {27, 0x0123456789abcdefULL, 0xb3c2d5086ed6cbddULL, 0x48bdef949c19a7b4ULL},
  {27, 0xfedcba9876543210ULL, 0xf08dc50f996e4845ULL, 0x5683daad720a5889ULL},
  {28, 0x0000000000000000ULL, 0x9725b1bbc3cd711dULL, 0xb8260bb7db140320ULL},
  {28, 0x0000000000000001ULL, 0x74415d286fc6bd18ULL, 0x2b4e66f8943f5523ULL},
  {28, 0x0123456789abcdefULL, 0x72629789d86502bdULL, 0xbe2435dac36be36dULL},
  {28, 0xfedcba9876543210ULL, 0xb86bf9a3b8d585e2ULL, 0x14e0c05e481ba596ULL},
  {29, 0x0000000000000000ULL, 0x638a12ba6d35e729ULL, 0x04b836427c4d6e1bULL},
  {29, 0x0000000000000001ULL, 0x39bf6c84bdf2d93aULL, 0x2a665588510ca45fULL},
  {29, 0x0123456789abcdefULL, 0xe5b1375e175c2aacULL, 0x815cebf48d97cb86ULL},
  {29, 0xfedcba9876543210ULL, 0xa5f11486190c1e7dULL, 0x106c25ca0be28326ULL},
  {30, 0x0000000000000000ULL, 0xe04c062c5ed97491ULL, 0xf64a10fc041115afULL},
  {30, 0x0000000000000001ULL, 0x3f3d09bb71a2dd7fULL, 0x9619cbd0428359bbULL},
  {30, 0x0123456789abcdefULL, 0xb84fc12c4694ede2ULL, 0x98cc6f6e124c62cbULL},
  {30, 0xfedcba9876543210ULL, 0x8ae8c9f28709b75eULL, 0x85f56358bbb5f7c1ULL},
  {31, 0x0000000000000000ULL, 0x213343c0d3b8a354ULL, 0x9a6a1f4f3e664b1bULL},
  {31, 0x0000000000000001ULL, 0xee7d1a6f22b45c45ULL, 0x475156ccf5f716f9ULL},
  {31, 0x0123456789abcdefULL, 0xf18b450bb66bbc0dULL, 0x22457d0b440f43feULL},
  {31, 0xfedcba9876543210ULL, 0x170c9fc0549a5ae9ULL, 0xa6da292ba1f07dadULL},
  {32, 0x0000000000000000ULL, 0x1b2dda64b8837975ULL, 0x886ffc228f1c80d6ULL},
  {32, 0x0000000000000001ULL, 0xdf2587bfdc902110ULL, 0xa9f011d91e361d62ULL},
  {32, 0x0123456789abcdefULL, 0x1e02fb25da80f443ULL, 0x073a5ac4919c738aULL},
  {32, 0xfedcba9876543210ULL, 0x9bedd8e6cf6060c6ULL, 0x02ced9fcef58f8b6ULL},
};
static uint64_t sm_state, xs[4];
static uint64_t splitmix64() { uint64_t z = (sm_state += 0x9e3779b97f4a7c15ULL); z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL; z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL; return z ^ (z >> 31); }
static uint64_t rl(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
static void rng_init(uint64_t s) { sm_state = s; for (int i = 0; i < 4; i++) xs[i] = splitmix64(); }
static uint64_t rng64() { uint64_t r = rl(xs[1] * 5, 7) * 9, t = xs[1] << 17; xs[2] ^= xs[0]; xs[3] ^= xs[1]; xs[1] ^= xs[2]; xs[0] ^= xs[3]; xs[2] ^= t; xs[3] = rl(xs[3], 45); return r; }
typedef std::vector<uint8_t> Msg;
static std::string hex(const Msg &m) { std::string s; char b[3]; for (auto c : m) { snprintf(b, 3, "%02x", c); s += b; } return s.empty() ? "(empty)" : s; }
static Msg unhex(const char *h) { Msg v; for (size_t i = 0; h[i] && h[i + 1]; i += 2) { unsigned b; sscanf(h + i, "%2x", &b); v.push_back((uint8_t)b); } return v; }
typedef uint64_t (*Fn)(const uint8_t *, size_t, uint64_t);
static int g_fail = 0;
static void pair(const char *label, Fn f, const Msg &a, const Msg &b, unsigned logN) {
  uint64_t N = 1ULL << logN, c = 0, ex = 0, eo = 0;
  for (uint64_t t = 0; t < N; t++) { uint64_t s = rng64(); uint64_t x = f(a.data(), a.size(), s), y = f(b.data(), b.size(), s); if (x == y) { if (!c) { ex = s; eo = x; } c++; } }
  uint64_t sw = 0; for (uint64_t s = 0; s < 2048; s += 64) sw += f(a.data(), a.size(), s) == f(b.data(), b.size(), s);
  double lo = c == N ? pow(0.025, 1.0 / N) : 0;
  printf("%-46s m=%s (%zu B) m'=%s (%zu B) L=%zu: %llu/%llu uniform seeds (95%% lower %.7f), %llu/32 SwissTable seeds",
         label, hex(a).c_str(), a.size(), hex(b).c_str(), b.size(), (std::max(a.size(), b.size()) + 7) / 8,
         (unsigned long long)c, (unsigned long long)N, lo, (unsigned long long)sw);
  if (c) printf("; e.g. seed %016llx -> %016llx", (unsigned long long)ex, (unsigned long long)eo);
  printf("\n");
}
static uint64_t family(const char *label, Fn f, const std::vector<Msg> &S, unsigned logN) {
  std::vector<Msg> T = S; std::sort(T.begin(), T.end()); size_t d = std::unique(T.begin(), T.end()) - T.begin();
  uint64_t N = 1ULL << logN, all = 0, sw = 0;
  for (uint64_t t = 0; t < N; t++) { uint64_t s = rng64(), h0 = f(S[0].data(), S[0].size(), s); bool ok = true;
    for (size_t i = 1; i < S.size() && ok; i++) ok = f(S[i].data(), S[i].size(), s) == h0; all += ok; }
  for (uint64_t s = 0; s < 2048; s += 64) { uint64_t h0 = f(S[0].data(), S[0].size(), s); bool ok = true;
    for (size_t i = 1; i < S.size() && ok; i++) ok = f(S[i].data(), S[i].size(), s) == h0; sw += ok; }
  printf("%-46s %zu members (%zu distinct), %zu bytes: all equal on %llu/%llu uniform seeds, %llu/32 SwissTable seeds; members[0,1,last] = %s %s %s\n",
         label, S.size(), d, S[0].size(), (unsigned long long)all, (unsigned long long)N, (unsigned long long)sw,
         hex(S[0]).c_str(), hex(S[1]).c_str(), hex(S.back()).c_str());
  if (all != N || sw != 32 || d != S.size()) g_fail = 1;
  return all;
}
/* kernel of the GF(2)-linear map x (nbits, little-endian bytes) -> image (64 bits) */
static std::vector<Msg> kernel(size_t nbytes, uint64_t (*img)(const Msg &)) {
  size_t n = 8 * nbytes; std::vector<uint64_t> col(n); std::vector<Msg> vec(n, Msg(nbytes, 0));
  for (size_t i = 0; i < n; i++) { Msg e(nbytes, 0); e[i / 8] = (uint8_t)(1u << (i % 8)); col[i] = img(e); vec[i] = e; }
  /* column reduction: eliminate pivots, columns that reduce to zero give kernel vectors */
  std::vector<Msg> ker; std::vector<std::pair<uint64_t, Msg>> piv;
  for (size_t i = 0; i < n; i++) { uint64_t c = col[i]; Msg v = vec[i];
    for (auto &p : piv) if (c & (p.first & (0 - p.first)) ) { /* pivot bit = lowest set bit of p.first */ c ^= p.first; for (size_t k = 0; k < nbytes; k++) v[k] ^= p.second[k]; }
    if (c) { /* normalise so pivots have distinct lowest bits */ piv.push_back({c, v});
      std::sort(piv.begin(), piv.end(), [](auto &x, auto &y) { return (x.first & (0 - x.first)) < (y.first & (0 - y.first)); }); }
    else ker.push_back(v); }
  return ker;
}
static uint64_t img_w8(const Msg &e) { return crc64(0, Read8(e.data())); }
static uint64_t img_9(const Msg &e) { return crc64(0, Read8(e.data())) | (crc64(0, Read8(e.data() + 1)) << 32); }
static uint64_t img_10(const Msg &e) { return crc64(0, Read8(e.data())) | (crc64(0, Read8(e.data() + 2)) << 32); }
int main(int argc, char **argv) {
  unsigned logN = argc > 1 ? atoi(argv[1]) : 20; rng_init(argc > 2 ? strtoull(argv[2], 0, 0) : 0x0f5577abULL);
  size_t bad = 0, nv = sizeof(kVec) / sizeof(kVec[0]);
  for (size_t i = 0; i < nv; i++) { Msg m(kVec[i].len); for (int k = 0; k < kVec[i].len; k++) m[k] = (uint8_t)(37 * k + 11);
    bad += hash_default(m.data(), m.size(), kVec[i].seed) != kVec[i].h_default; bad += hash_crc(m.data(), m.size(), kVec[i].seed) != kVec[i].h_crc; }
  printf("validation: %zu vectors x 2 builds against the real library: %zu mismatches %s\n", nv, bad, bad ? "FAIL" : "OK");
  if (bad) return 1;
  /* linearity sanity of the software CRC: crc(c,d) = crc(c,0) ^ crc(0,d) */
  for (int i = 0; i < 1000; i++) { uint64_t c = rng64() & 0xffffffff, d = rng64(); if (crc64(c, d) != (crc64(c, 0) ^ crc64(0, d))) { printf("CRC not linear?\n"); return 1; } }
  printf("\n== Job 1: the recorded pair (default build) ==\n");
  Msg e0, r8 = unhex("a6e02637c07bd386");
  pair("record pair, default build", hash_default, e0, r8, logN);
  pair("record pair, CRC build (information)", hash_crc, e0, r8, 16);
  printf("\n== CRC build: key-free pairs from ker(CRC32C: 64 -> 32 bits) ==\n");
  auto K8 = kernel(8, img_w8), K9 = kernel(9, img_9), K10 = kernel(10, img_10);
  printf("kernel dimensions: one 8-byte word %zu, 9-byte two-window %zu, 10-byte two-window %zu\n", K8.size(), K9.size(), K10.size());
  Msg z9(9, 0); pair("CRC build 9-byte pair (0 vs kernel vector)", hash_crc, z9, K9[0], logN);
  pair("  same pair on the default build (info)", hash_default, z9, K9[0], 16);
  Msg u16a(16, 0), u16b(16, 0); for (int k = 0; k < 8; k++) u16a[8 + k] = u16b[8 + k] = (uint8_t)(kMul >> (8 * k));
  for (int k = 0; k < 8; k++) u16b[k] = K8[0][k];
  pair("16-byte pair, default build", hash_default, u16a, u16b, logN);
  pair("16-byte pair, CRC build", hash_crc, u16a, u16b, logN);
  printf("\n== flooding families at 10 bytes ==\n");
  std::vector<Msg> F10d, F10c, F16;
  for (uint32_t i = 0; i < 65536; i++) { Msg m(10); m[0] = (uint8_t)i; m[1] = (uint8_t)(i >> 8); for (int k = 0; k < 8; k++) m[2 + k] = (uint8_t)(kMul >> (8 * k)); F10d.push_back(m); }
  family("default build, bytes 2..9 = kMul (every 2-byte prefix)", hash_default, F10d, logN > 12 ? 12 : logN);
  Msg base10(10); for (auto &b : base10) b = (uint8_t)rng64();
  for (uint32_t i = 0; i < (1u << K10.size()); i++) { Msg m = base10; for (size_t j = 0; j < K10.size(); j++) if (i >> j & 1) for (int k = 0; k < 10; k++) m[k] ^= K10[j][k]; F10c.push_back(m); }
  family("CRC build, base xor span(10-byte two-window kernel)", hash_crc, F10c, logN > 12 ? 12 : logN);
  Msg base16(16); for (auto &b : base16) b = (uint8_t)rng64(); for (int k = 0; k < 8; k++) base16[8 + k] = (uint8_t)(kMul >> (8 * k));
  for (uint32_t i = 0; i < 65536; i++) { Msg m = base16; for (size_t j = 0; j < 16; j++) if (i >> j & 1) for (int k = 0; k < 8; k++) m[k] ^= K8[j][k]; F16.push_back(m); }
  family("16 B, w1 = kMul, w0 in base xor ker: default build", hash_default, F16, logN > 12 ? 12 : logN);
  family("  same 16-byte family, CRC build", hash_crc, F16, logN > 12 ? 12 : logN);
  FILE *fo = fopen("abseil_families.txt", "w"); if (fo) { for (auto *F : {&F10d, &F10c, &F16}) { fprintf(fo, "# family\n"); for (auto &m : *F) fprintf(fo, "%s\n", hex(m).c_str()); } fclose(fo); }
  printf(g_fail ? "RESULT: FAIL\n" : "RESULT: PASS\n"); return g_fail;
}
