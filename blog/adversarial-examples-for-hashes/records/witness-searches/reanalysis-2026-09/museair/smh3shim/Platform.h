/* Minimal stand-in for the SMHasher3 platform header: only what hashes/museair.cpp uses (little-endian host). */
#pragma once
#include <cstdint>
#include <cstring>
#include <cstddef>
typedef uint64_t seed_t;
#define FORCE_INLINE inline __attribute__((always_inline))
#define NEVER_INLINE __attribute__((noinline))
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
static inline bool isBE(void) { return false; }
static inline uint64_t BSWAP64(uint64_t v) { return __builtin_bswap64(v); }
static inline uint32_t BSWAP32(uint32_t v) { return __builtin_bswap32(v); }
#define COND_BSWAP(v, c) ((c) ? BSWAP64(v) : (v))
static inline uint64_t ROTL64(uint64_t x, unsigned r) { r &= 63; return r ? (x << r) | (x >> (64 - r)) : x; }
static inline uint64_t ROTR64(uint64_t x, unsigned r) { r &= 63; return r ? (x >> r) | (x << (64 - r)) : x; }
template <bool bswap> static inline uint32_t GET_U32(const uint8_t *b, size_t o) { uint32_t v; memcpy(&v, b + o, 4); return bswap ? BSWAP32(v) : v; }
template <bool bswap> static inline uint64_t GET_U64(const uint8_t *b, size_t o) { uint64_t v; memcpy(&v, b + o, 8); return bswap ? BSWAP64(v) : v; }
template <bool bswap> static inline void PUT_U64(uint64_t v, uint8_t *b, size_t o) { if (bswap) v = BSWAP64(v); memcpy(b + o, &v, 8); }
