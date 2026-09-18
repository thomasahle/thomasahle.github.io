/*
 * highwayhash_verify.c -- reader-runnable check of the HighwayHash weak-key collision pair.
 *
 * SPDX-License-Identifier: MIT (everything outside the "verbatim copy" section)
 * Copyright (c) 2026 Thomas Dybdahl Ahle
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and
 * to permit persons to whom the Software is furnished to do so, subject to the following conditions: The above
 * copyright notice and this permission notice shall be included in all copies or substantial portions of the
 * Software. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
 * LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
 * SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * The hash is NOT re-implemented.  The section marked "BEGIN/END verbatim copy" is a byte-for-byte copy of
 * c/highwayhash.c from the reference repository https://github.com/google/highwayhash (Copyright 2017 Google
 * Inc., Apache License, Version 2.0, http://www.apache.org/licenses/LICENSE-2.0; distributed "AS IS", without
 * warranties or conditions of any kind), as vendored at tools/bench/adversarial/vendor/highwayhash/c/ in the paper
 * repository (sha256 fb316726f8d95ab83ad4721058586abb2e98ede635d48a2fd7ec5d6b15543fca).  The only edit is line 1,
 * `#include "c/highwayhash.h"`, replaced by a comment; the two typedefs that header supplies are copied just above.
 *
 * Build:  cc -O2 -std=c11 -o highwayhash_verify highwayhash_verify.c -lm
 * Run:    ./highwayhash_verify [log2 N] [rng seed] [sm3]        (default N = 2^24 keys per experiment)
 *
 *   1. validates the copy against the SMHasher3 verification values of the registered variants HighwayHash_64
 *      (0xF3246108), HighwayHash_128 (0x232D434E), HighwayHash_256 (0x0D50D328), computed by SMHasher3's own
 *      procedure, and against the upstream 33-byte test vector; aborts on any mismatch;
 *   2. prints the published 96-byte pair and hashes both messages under N uniformly random 256-bit keys;
 *   3. samples the weak-key class hi32(key[0]) = 0xdbe6d5d5 (density 2^-32): N class keys hashed in full at all
 *      three widths, then a 16N-key screen of the lemma's packet-2 event E_2 with every hit hashed in full;
 *   4. prints the published explicit colliding key with both hash values at 64, 128 and 256 bits.
 *   With the third argument "sm3" step 3 uses SMHasher3's seed-to-key map instead (see the README).
 */
#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* The two typedefs of c/highwayhash.h (verbatim excerpt; the rest of that header is prototypes and examples). */
typedef struct {
  uint64_t v0[4];
  uint64_t v1[4];
  uint64_t mul0[4];
  uint64_t mul1[4];
} HighwayHashState;
typedef struct {
  HighwayHashState state;
  uint8_t packet[32];
  int num;
} HighwayHashCat;

/* ===== BEGIN verbatim copy of c/highwayhash.c (google/highwayhash, Apache-2.0); the only edit is line 1 ===== */
/* #include "c/highwayhash.h" -- the header's typedefs are inlined above (this comment is the only edit) */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*
This code is compatible with C90 with the additional requirement of
supporting uint64_t.
*/

/*////////////////////////////////////////////////////////////////////////////*/
/* Internal implementation                                                    */
/*////////////////////////////////////////////////////////////////////////////*/

void HighwayHashReset(const uint64_t key[4], HighwayHashState* state) {
  state->mul0[0] = 0xdbe6d5d5fe4cce2full;
  state->mul0[1] = 0xa4093822299f31d0ull;
  state->mul0[2] = 0x13198a2e03707344ull;
  state->mul0[3] = 0x243f6a8885a308d3ull;
  state->mul1[0] = 0x3bd39e10cb0ef593ull;
  state->mul1[1] = 0xc0acf169b5f18a8cull;
  state->mul1[2] = 0xbe5466cf34e90c6cull;
  state->mul1[3] = 0x452821e638d01377ull;
  state->v0[0] = state->mul0[0] ^ key[0];
  state->v0[1] = state->mul0[1] ^ key[1];
  state->v0[2] = state->mul0[2] ^ key[2];
  state->v0[3] = state->mul0[3] ^ key[3];
  state->v1[0] = state->mul1[0] ^ ((key[0] >> 32) | (key[0] << 32));
  state->v1[1] = state->mul1[1] ^ ((key[1] >> 32) | (key[1] << 32));
  state->v1[2] = state->mul1[2] ^ ((key[2] >> 32) | (key[2] << 32));
  state->v1[3] = state->mul1[3] ^ ((key[3] >> 32) | (key[3] << 32));
}

static void ZipperMergeAndAdd(const uint64_t v1, const uint64_t v0,
                              uint64_t* add1, uint64_t* add0) {
  *add0 += (((v0 & 0xff000000ull) | (v1 & 0xff00000000ull)) >> 24) |
           (((v0 & 0xff0000000000ull) | (v1 & 0xff000000000000ull)) >> 16) |
           (v0 & 0xff0000ull) | ((v0 & 0xff00ull) << 32) |
           ((v1 & 0xff00000000000000ull) >> 8) | (v0 << 56);
  *add1 += (((v1 & 0xff000000ull) | (v0 & 0xff00000000ull)) >> 24) |
           (v1 & 0xff0000ull) | ((v1 & 0xff0000000000ull) >> 16) |
           ((v1 & 0xff00ull) << 24) | ((v0 & 0xff000000000000ull) >> 8) |
           ((v1 & 0xffull) << 48) | (v0 & 0xff00000000000000ull);
}

static void Update(const uint64_t lanes[4], HighwayHashState* state) {
  int i;
  for (i = 0; i < 4; ++i) {
    state->v1[i] += state->mul0[i] + lanes[i];
    state->mul0[i] ^= (state->v1[i] & 0xffffffff) * (state->v0[i] >> 32);
    state->v0[i] += state->mul1[i];
    state->mul1[i] ^= (state->v0[i] & 0xffffffff) * (state->v1[i] >> 32);
  }
  ZipperMergeAndAdd(state->v1[1], state->v1[0], &state->v0[1], &state->v0[0]);
  ZipperMergeAndAdd(state->v1[3], state->v1[2], &state->v0[3], &state->v0[2]);
  ZipperMergeAndAdd(state->v0[1], state->v0[0], &state->v1[1], &state->v1[0]);
  ZipperMergeAndAdd(state->v0[3], state->v0[2], &state->v1[3], &state->v1[2]);
}

static uint64_t Read64(const uint8_t* src) {
  return (uint64_t)src[0] | ((uint64_t)src[1] << 8) |
      ((uint64_t)src[2] << 16) | ((uint64_t)src[3] << 24) |
      ((uint64_t)src[4] << 32) | ((uint64_t)src[5] << 40) |
      ((uint64_t)src[6] << 48) | ((uint64_t)src[7] << 56);
}

void HighwayHashUpdatePacket(const uint8_t* packet, HighwayHashState* state) {
  uint64_t lanes[4];
  lanes[0] = Read64(packet + 0);
  lanes[1] = Read64(packet + 8);
  lanes[2] = Read64(packet + 16);
  lanes[3] = Read64(packet + 24);
  Update(lanes, state);
}

static void Rotate32By(uint64_t count, uint64_t lanes[4]) {
  int i;
  for (i = 0; i < 4; ++i) {
    uint32_t half0 = lanes[i] & 0xffffffff;
    uint32_t half1 = (lanes[i] >> 32);
    lanes[i] = (half0 << count) | (half0 >> (32 - count));
    lanes[i] |= (uint64_t)((half1 << count) | (half1 >> (32 - count))) << 32;
  }
}

void HighwayHashUpdateRemainder(const uint8_t* bytes, const size_t size_mod32,
                                HighwayHashState* state) {
  int i;
  const size_t size_mod4 = size_mod32 & 3;
  const uint8_t* remainder = bytes + (size_mod32 & ~3);
  uint8_t packet[32] = {0};
  for (i = 0; i < 4; ++i) {
    state->v0[i] += ((uint64_t)size_mod32 << 32) + size_mod32;
  }
  Rotate32By(size_mod32, state->v1);
  for (i = 0; i < remainder - bytes; i++) {
    packet[i] = bytes[i];
  }
  if (size_mod32 & 16) {
    for (i = 0; i < 4; i++) {
      packet[28 + i] = remainder[i + size_mod4 - 4];
    }
  } else {
    if (size_mod4) {
      packet[16 + 0] = remainder[0];
      packet[16 + 1] = remainder[size_mod4 >> 1];
      packet[16 + 2] = remainder[size_mod4 - 1];
    }
  }
  HighwayHashUpdatePacket(packet, state);
}

static void Permute(const uint64_t v[4], uint64_t* permuted) {
  permuted[0] = (v[2] >> 32) | (v[2] << 32);
  permuted[1] = (v[3] >> 32) | (v[3] << 32);
  permuted[2] = (v[0] >> 32) | (v[0] << 32);
  permuted[3] = (v[1] >> 32) | (v[1] << 32);
}

void PermuteAndUpdate(HighwayHashState* state) {
  uint64_t permuted[4];
  Permute(state->v0, permuted);
  Update(permuted, state);
}

static void ModularReduction(uint64_t a3_unmasked, uint64_t a2, uint64_t a1,
                             uint64_t a0, uint64_t* m1, uint64_t* m0) {
  uint64_t a3 = a3_unmasked & 0x3FFFFFFFFFFFFFFFull;
  *m1 = a1 ^ ((a3 << 1) | (a2 >> 63)) ^ ((a3 << 2) | (a2 >> 62));
  *m0 = a0 ^ (a2 << 1) ^ (a2 << 2);
}

static uint64_t HighwayHashFinalize64(HighwayHashState* state) {
  int i;
  for (i = 0; i < 4; i++) {
    PermuteAndUpdate(state);
  }
  return state->v0[0] + state->v1[0] + state->mul0[0] + state->mul1[0];
}

static void HighwayHashFinalize128(HighwayHashState* state, uint64_t hash[2]) {
  int i;
  for (i = 0; i < 6; i++) {
    PermuteAndUpdate(state);
  }
  hash[0] = state->v0[0] + state->mul0[0] + state->v1[2] + state->mul1[2];
  hash[1] = state->v0[1] + state->mul0[1] + state->v1[3] + state->mul1[3];
}

static void HighwayHashFinalize256(HighwayHashState* state, uint64_t hash[4]) {
  int i;
  /* We anticipate that 256-bit hashing will be mostly used with long messages
     because storing and using the 256-bit hash (in contrast to 128-bit)
     carries a larger additional constant cost by itself. Doing extra rounds
     here hardly increases the per-byte cost of long messages. */
  for (i = 0; i < 10; i++) {
    PermuteAndUpdate(state);
  }
  ModularReduction(state->v1[1] + state->mul1[1], state->v1[0] + state->mul1[0],
                   state->v0[1] + state->mul0[1], state->v0[0] + state->mul0[0],
                   &hash[1], &hash[0]);
  ModularReduction(state->v1[3] + state->mul1[3], state->v1[2] + state->mul1[2],
                   state->v0[3] + state->mul0[3], state->v0[2] + state->mul0[2],
                   &hash[3], &hash[2]);
}

/*////////////////////////////////////////////////////////////////////////////*/
/* Non-cat API: single call on full data                                      */
/*////////////////////////////////////////////////////////////////////////////*/

static void ProcessAll(const uint8_t* data, size_t size, const uint64_t key[4],
                       HighwayHashState* state) {
  size_t i;
  HighwayHashReset(key, state);
  for (i = 0; i + 32 <= size; i += 32) {
    HighwayHashUpdatePacket(data + i, state);
  }
  if ((size & 31) != 0) HighwayHashUpdateRemainder(data + i, size & 31, state);
}

uint64_t HighwayHash64(const uint8_t* data, size_t size,
    const uint64_t key[4]) {
  HighwayHashState state;
  ProcessAll(data, size, key, &state);
  return HighwayHashFinalize64(&state);
}

void HighwayHash128(const uint8_t* data, size_t size,
    const uint64_t key[4], uint64_t hash[2]) {
  HighwayHashState state;
  ProcessAll(data, size, key, &state);
  HighwayHashFinalize128(&state, hash);
}

void HighwayHash256(const uint8_t* data, size_t size,
    const uint64_t key[4], uint64_t hash[4]) {
  HighwayHashState state;
  ProcessAll(data, size, key, &state);
  HighwayHashFinalize256(&state, hash);
}

/*////////////////////////////////////////////////////////////////////////////*/
/* Cat API: allows appending with multiple calls                              */
/*////////////////////////////////////////////////////////////////////////////*/

void HighwayHashCatStart(const uint64_t key[4], HighwayHashCat* state) {
  HighwayHashReset(key, &state->state);
  state->num = 0;
}

void HighwayHashCatAppend(const uint8_t* bytes, size_t num,
                          HighwayHashCat* state) {
  size_t i;
  if (state->num != 0) {
    size_t num_add = num > (32u - state->num) ? (32u - state->num) : num;
    for (i = 0; i < num_add; i++) {
      state->packet[state->num + i] = bytes[i];
    }
    state->num += num_add;
    num -= num_add;
    bytes += num_add;
    if (state->num == 32) {
      HighwayHashUpdatePacket(state->packet, &state->state);
      state->num = 0;
    }
  }
  while (num >= 32) {
    HighwayHashUpdatePacket(bytes, &state->state);
    num -= 32;
    bytes += 32;
  }
  for (i = 0; i < num; i++) {
    state->packet[state->num] = bytes[i];
    state->num++;
  }
}

uint64_t HighwayHashCatFinish64(const HighwayHashCat* state) {
  HighwayHashState copy = state->state;
  if (state->num) {
    HighwayHashUpdateRemainder(state->packet, state->num, &copy);
  }
  return HighwayHashFinalize64(&copy);
}

void HighwayHashCatFinish128(const HighwayHashCat* state, uint64_t hash[2]) {
  HighwayHashState copy = state->state;
  if (state->num) {
    HighwayHashUpdateRemainder(state->packet, state->num, &copy);
  }
  HighwayHashFinalize128(&copy, hash);
}

void HighwayHashCatFinish256(const HighwayHashCat* state, uint64_t hash[4]) {
  HighwayHashState copy = state->state;
  if (state->num) {
    HighwayHashUpdateRemainder(state->packet, state->num, &copy);
  }
  HighwayHashFinalize256(&copy, hash);
}
/* ===== END verbatim copy of c/highwayhash.c ===== */

/* ======================================= our code (MIT, see the top) ======================================= */
/* deterministic RNG: splitmix64 seeds xoshiro256** */
static uint64_t xs[4];
static uint64_t splitmix64(uint64_t *s) {
    uint64_t z = (*s += UINT64_C(0x9e3779b97f4a7c15));
    z = (z ^ (z >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
    z = (z ^ (z >> 27)) * UINT64_C(0x94d049bb133111eb);
    return z ^ (z >> 31);
}
static void rng_seed(uint64_t s) { for (int i = 0; i < 4; i++) xs[i] = splitmix64(&s); }
static uint64_t rotl64(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
static uint64_t rng(void) {
    uint64_t r = rotl64(xs[1] * 5, 7) * 9, t = xs[1] << 17;
    xs[2] ^= xs[0]; xs[3] ^= xs[1]; xs[1] ^= xs[2]; xs[0] ^= xs[3]; xs[2] ^= t; xs[3] = rotl64(xs[3], 45);
    return r;
}

/* Validation: SMHasher3 (lib/Hashinfo.cpp _ComputedVerifyImpl on hashes/highwayhash.cpp).  SMHasher3 derives the
 * 256-bit key from its 64-bit seed as key[i] = {1,2,3,4}[i] ^ seed and stores the output words little-endian.
 * Procedure: hash the keys {}, {0}, {0,1}, ..., {0..254} (key i = bytes 0..i-1) with seed 256-i, concatenate the
 * outputs, hash that array with seed 0, read the first 4 bytes little-endian. */
static void hh_any(int width, const uint8_t *data, size_t len, const uint64_t key[4], uint64_t out[4]) {
    if (width == 64) out[0] = HighwayHash64(data, len, key);
    else if (width == 128) HighwayHash128(data, len, key, out);
    else HighwayHash256(data, len, key, out);
}
static void sm3_key(uint64_t seed, uint64_t key[4]) { for (int i = 0; i < 4; i++) key[i] = (uint64_t)(i + 1) ^ seed; }
static void sm3_hash(int width, const uint8_t *data, size_t len, uint64_t seed, uint8_t *out) {
    uint64_t key[4], h[4];
    sm3_key(seed, key);
    hh_any(width, data, len, key, h);
    for (int i = 0; i < width / 8; i++) out[i] = (uint8_t)(h[i / 8] >> (8 * (i % 8)));
}
static uint32_t sm3_verification(int width) {
    const size_t hb = (size_t)width / 8;
    uint8_t key[256] = { 0 }, hashes[32 * 256], total[32];
    for (int i = 0; i < 256; i++) {
        sm3_hash(width, key, (size_t)i, (uint64_t)(256 - i), hashes + (size_t)i * hb);
        key[i] = (uint8_t)i;
    }
    sm3_hash(width, hashes, hb * 256, 0, total);
    return (uint32_t)total[0] | (uint32_t)total[1] << 8 | (uint32_t)total[2] << 16 | (uint32_t)total[3] << 24;
}

/* The record (hex copied exactly; 0^24 = 24 zero bytes). */
#define Z8  "0000000000000000"
#define Z24 Z8 Z8 Z8
static const char *M1_HEX = "d131b3012a2a1924" Z24 "d132b3b42a2a1924" Z24 Z8 Z24;
static const char *M2_HEX = "d132b3012a2a1924" Z24 "d130b3b32a2a1924" Z24 "0001000000000000" Z24;
static uint8_t M1[96], M2[96];
static const uint64_t CLASS_HI32 = UINT64_C(0xdbe6d5d5);        /* = hi32 of the lane-0 init0 constant */
static const uint64_t EXPLICIT_KEY[4] = { UINT64_C(0xdbe6d5d58afad71e), UINT64_C(0xa0142b42de197939),
                                          UINT64_C(0x5bd2b2861106bd66), UINT64_C(0xb6c304527caad524) };
static const uint64_t EXPLICIT_H64 = UINT64_C(0xf5eba26391be727f);
static const double COND_RATE = 235.0 * 239.0 / 1099511627776.0;  /* 56165 / 2^40 = 2^-24.22, proved exact */

static int unhex(const char *s, uint8_t *out, size_t cap) {
    if (strlen(s) != 2 * cap) return 0;
    for (size_t i = 0; i < 2 * cap; i++) {
        int c = s[i], v = c >= '0' && c <= '9' ? c - '0' : c >= 'a' && c <= 'f' ? c - 'a' + 10 : -1;
        if (v < 0) return 0;
        out[i / 2] = (uint8_t)(out[i / 2] << 4 | v);
    }
    return 1;
}
static void print_hex(const uint8_t *p, size_t n) { for (size_t i = 0; i < n; i++) printf("%02x", p[i]); }
static void print_words(const uint64_t *h, int n, const char *sep) {
    for (int i = 0; i < n; i++) printf("%s%016" PRIx64, i ? sep : "", h[i]);
}
static void print_rate(const char *label, uint64_t c, uint64_t n, int log2n) {
    if (c == 0) printf("%s: 0/%" PRIu64 ", none observed in 2^%d trials; no population bound\n", label, n, log2n);
    else printf("%s: %" PRIu64 "/%" PRIu64 " = %.3e, rate 2^%.2f\n", label, c, n, (double)c / (double)n, log2((double)c / (double)n));
}

/* Experiments.  E_2 is the lemma's packet-2 event: at the lane-0 mul0 multiply of packet 2 the operands
 * lo32(v1[0]) and hi32(v0[0]) differ by exactly 2^8 mod 2^32, evaluated on the reference state after packet 1 of
 * m1 (the packet-2 update adds mul0[0] + lane0 to v1[0] before the multiply). */
typedef struct { uint64_t n, e2, state, h64, h128, h256, ex_key[4], ex_h; int have_ex; } Tally;
static int event_e2(const uint64_t key[4]) {
    HighwayHashState s;
    HighwayHashReset(key, &s);
    HighwayHashUpdatePacket(M1, &s);
    uint64_t x = s.v1[0] + s.mul0[0] + Read64(M1 + 32);
    return (uint32_t)((uint32_t)x - (uint32_t)(s.v0[0] >> 32)) == 0x100u;
}
static void hash_pair(const uint64_t key[4], Tally *t) {
    HighwayHashState a, b, ca, cb;
    uint64_t x[4], y[4];
    t->n++;
    ProcessAll(M1, 96, key, &a);
    ProcessAll(M2, 96, key, &b);
    if (memcmp(&a, &b, sizeof a) == 0) t->state++;                       /* full 1024-bit state equal */
    ca = a; cb = b;
    uint64_t ha = HighwayHashFinalize64(&ca), hb = HighwayHashFinalize64(&cb);
    if (ha != hb) return;
    t->h64++;
    ca = a; cb = b; HighwayHashFinalize128(&ca, x); HighwayHashFinalize128(&cb, y); t->h128 += memcmp(x, y, 16) == 0;
    ca = a; cb = b; HighwayHashFinalize256(&ca, x); HighwayHashFinalize256(&cb, y); t->h256 += memcmp(x, y, 32) == 0;
    if (!t->have_ex) { t->have_ex = 1; memcpy(t->ex_key, key, 32); t->ex_h = ha; }
}
/* A class key: hi32(key[0]) = 0xdbe6d5d5, the other 224 bits uniform.  With sm3: SMHasher3's key[i] =
 * {1,2,3,4}[i] ^ seed for a 64-bit seed with hi32(seed) = 0xdbe6d5d5 and lo32(seed) uniform. */
static void class_key(int sm3, uint64_t k[4]) {
    if (sm3) { sm3_key(CLASS_HI32 << 32 | (uint32_t)rng(), k); return; }
    k[0] = CLASS_HI32 << 32 | (uint32_t)rng(); k[1] = rng(); k[2] = rng(); k[3] = rng();
}
static void class_experiment(int sm3, int log2n) {
    const uint64_t N = UINT64_C(1) << log2n, M = N << 4;
    Tally f = { 0 }, s = { 0 };
    uint64_t k[4];
    for (uint64_t i = 0; i < N; i++) { class_key(sm3, k); f.e2 += (uint64_t)event_e2(k); hash_pair(k, &f); }
    for (uint64_t i = 0; i < M; i++) { class_key(sm3, k); s.n++; if (event_e2(k)) { s.e2++; s.n--; hash_pair(k, &s); } }
    printf("  N = 2^%d class keys, both messages hashed in full (expected %.2f collisions at the proved rate):\n",
           log2n, (double)N * COND_RATE);
    print_rate("    HighwayHash64 collisions", f.h64, f.n, log2n);
    printf("    HighwayHash128 %" PRIu64 ", HighwayHash256 %" PRIu64 ", full 1024-bit state equal after packet 3 %" PRIu64
           ", E_2 %" PRIu64 "\n", f.h128, f.h256, f.state, f.e2);
    printf("  E_2 screen of 16N = 2^%d class keys (Reset + packet 1 each; every hit then hashed in full):\n", log2n + 4);
    print_rate("    E_2 hits", s.e2, s.n, log2n + 4);
    printf("    expected %.1f +- %.1f at the proved rate 235*239/2^40 = 2^%.2f; hits colliding at 64/128/256 bits: %" PRIu64
           "/%" PRIu64 "/%" PRIu64 ", full state: %" PRIu64 "\n", (double)M * COND_RATE, sqrt((double)M * COND_RATE),
           log2(COND_RATE), s.h64, s.h128, s.h256, s.state);
    const Tally *ex = f.have_ex ? &f : s.have_ex ? &s : NULL;
    if (ex) {
        printf("    first colliding class key: "); print_words(ex->ex_key, 4, " ");
        printf("  ->  HighwayHash64 %016" PRIx64 " for both\n", ex->ex_h);
    }
}

int main(int argc, char **argv) {
    char *end = NULL;
    int log2n = argc > 1 ? (int)strtol(argv[1], &end, 10) : 24;
    if (argc > 1 && (end == argv[1] || *end)) log2n = -1;                    /* non-numeric: usage */
    uint64_t rseed = argc > 2 ? strtoull(argv[2], &end, 0) : UINT64_C(20260917);
    if (argc > 2 && (end == argv[2] || *end)) log2n = -1;
    int sm3 = argc > 3 && strcmp(argv[3], "sm3") == 0;
    if (log2n < 0 || log2n > 40 || (argc > 3 && !sm3)) { fprintf(stderr, "usage: %s [log2 N in 0..40] [rng seed] [sm3]\n", argv[0]); return 2; }
    const uint64_t N = UINT64_C(1) << log2n;

    printf("HighwayHash (google/highwayhash reference C, frozen; SMHasher3 HighwayHash_64/128/256): weak-key collision pair\n");
    const struct { int w; uint32_t expect; const char *name; } V[3] = {
        { 64, 0xF3246108u, "HighwayHash_64 " }, { 128, 0x232D434Eu, "HighwayHash_128" }, { 256, 0x0D50D328u, "HighwayHash_256" } };
    for (int i = 0; i < 3; i++) {
        uint32_t v = sm3_verification(V[i].w);
        printf("%s%s SMHasher3 verification 0x%08X (expected 0x%08X) %s\n", i ? "            " : "validation: ",
               V[i].name, v, V[i].expect, v == V[i].expect ? "OK" : "MISMATCH");
        if (v != V[i].expect) { fprintf(stderr, "validation failed\n"); return 1; }
    }
    {   /* c/highwayhash_test.c: bytes 128..160 under key {1,2,3,4} */
        uint8_t data[33]; const uint64_t key[4] = { 1, 2, 3, 4 };
        for (int i = 0; i < 33; i++) data[i] = (uint8_t)(128 + i);
        uint64_t h = HighwayHash64(data, 33, key);
        printf("            upstream 33-byte test vector %016" PRIx64 " (expected 53c516cce478cad7) %s\n", h, h == UINT64_C(0x53c516cce478cad7) ? "OK" : "MISMATCH");
        if (h != UINT64_C(0x53c516cce478cad7)) { fprintf(stderr, "validation failed\n"); return 1; }
    }

    if (!unhex(M1_HEX, M1, 96) || !unhex(M2_HEX, M2, 96)) { fprintf(stderr, "malformed pair record\n"); return 2; }
    printf("\npair: 96 bytes = three 32-byte packets; additive lane-0 trail +2^8 | -(2^9+2^24) | +2^8; weak-key class hi32(key[0]) = 0x%08" PRIx64 "\n", CLASS_HI32);
    printf("mechanism: in the class hi32(v0[0]) = 0 after Reset, so packet 1's lane-0 multiply is 0 and the +2^8 in v1[0] stays\n"
           "  a single-byte additive difference that the zipper merely relocates; packet 2's -(2^9+2^24) collapses it to one -2^8\n"
           "  iff the packet-2 lane-0 multiplier operands satisfy lo32(v1[0]) - hi32(v0[0]) = 2^8 mod 2^32 (event E_2, proved\n"
           "  probability 235*239/2^40 over the other key bits); packet 3's +2^8 cancels it, the whole 1024-bit state is equal,\n"
           "  so all three output widths and any common suffix collide.\n");
    printf("m1 = "); print_hex(M1, 96); printf("\nm2 = "); print_hex(M2, 96); printf("\n");
    {   /* the record says m2 = m1 with lane-0 words +2^8, -(2^9+2^24), +2^8: check it from the bytes */
        const uint64_t d[3] = { 1u << 8, (uint64_t)0 - ((1u << 9) + (1u << 24)), 1u << 8 };
        for (int p = 0; p < 3; p++)
            if (Read64(M2 + 32 * p) - Read64(M1 + 32 * p) != d[p] || memcmp(M1 + 32 * p + 8, M2 + 32 * p + 8, 24)) {
                fprintf(stderr, "pair record does not match its description\n"); return 2; }
        printf("lane-0 words of m2 - m1 = +2^8 | -(2^9+2^24) | +2^8 (mod 2^64), lanes 1..3 identical: OK\n");
    }

    rng_seed(rseed);
    Tally u = { 0 };
    for (uint64_t i = 0; i < N; i++) { uint64_t k[4] = { rng(), rng(), rng(), rng() }; hash_pair(k, &u); }
    printf("\nuniform 256-bit keys, N = 2^%d (rng seed %" PRIu64 "):\n", log2n, rseed);
    print_rate("  HighwayHash64 collisions", u.h64, u.n, log2n);
    printf("  (proved lower bound over uniform keys 2^-32 * 235*239/2^40 = 2^%.2f: %.1e expected here, so 0 is the null check)\n",
           log2(COND_RATE) - 32, (double)N * COND_RATE / 4294967296.0);

    printf("\nweak-key class %s (density exactly 2^-32), other bits uniform:\n", sm3
           ? "hi32(seed) = 0xdbe6d5d5 under SMHasher3's map key[i] = {1,2,3,4}[i] ^ seed"
           : "hi32(key[0]) = 0xdbe6d5d5, 32 of the 256 key bits fixed");
    class_experiment(sm3, log2n);
    printf("  class density 2^-32 x proved conditional rate 2^%.2f => eps >= 56165/2^72 = 2^%.2f over uniform keys at every width\n",
           log2(COND_RATE), log2(COND_RATE) - 32);

    uint64_t h64[2], h128[2][2], h256[2][4];
    h64[0] = HighwayHash64(M1, 96, EXPLICIT_KEY); h64[1] = HighwayHash64(M2, 96, EXPLICIT_KEY);
    HighwayHash128(M1, 96, EXPLICIT_KEY, h128[0]); HighwayHash128(M2, 96, EXPLICIT_KEY, h128[1]);
    HighwayHash256(M1, 96, EXPLICIT_KEY, h256[0]); HighwayHash256(M2, 96, EXPLICIT_KEY, h256[1]);
    printf("\nexplicit key "); print_words(EXPLICIT_KEY, 4, " "); printf(" (published: HighwayHash64 = %016" PRIx64 " for both)\n", EXPLICIT_H64);
    printf("  HighwayHash64  h(m1) = %016" PRIx64 "  h(m2) = %016" PRIx64 "  %s\n", h64[0], h64[1], h64[0] == h64[1] ? "COLLIDE" : "differ");
    printf("  HighwayHash128 h(m1) = "); print_words(h128[0], 2, ""); printf("  h(m2) = "); print_words(h128[1], 2, "");
    printf("  %s\n", memcmp(h128[0], h128[1], 16) == 0 ? "COLLIDE" : "differ");
    printf("  HighwayHash256 h(m1) = "); print_words(h256[0], 4, ""); printf("\n                 h(m2) = "); print_words(h256[1], 4, "");
    printf("  %s\n", memcmp(h256[0], h256[1], 32) == 0 ? "COLLIDE" : "differ");
    if (h64[0] != EXPLICIT_H64 || h64[1] != EXPLICIT_H64 || memcmp(h128[0], h128[1], 16) || memcmp(h256[0], h256[1], 32)) {
        fprintf(stderr, "explicit key does not reproduce the published collision\n"); return 4; }
    return 0;
}
