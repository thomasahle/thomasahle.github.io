/* Minimal stand-in for SMHasher3 Mathmult.h: the full 64x64 -> 128-bit product. */
#pragma once
#include <cstdint>
namespace MathMult { static inline void mult64_128(uint64_t &lo, uint64_t &hi, uint64_t a, uint64_t b) {
  unsigned __int128 r = (unsigned __int128)a * b; lo = (uint64_t)r; hi = (uint64_t)(r >> 64); } }
