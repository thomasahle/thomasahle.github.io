/*
 * komi_long.c -- komihash 5.34 beyond one loop iteration (>= 128 bytes):
 * why the lane-tie mechanism dies, and what re-establishing the ties costs.
 *
 * Upstream komihash.h 5.34 (Aleksey Vaneev, MIT) is included unmodified from
 * upstream/komihash/.  Startup: SMHasher3 verification value 0x8157FF6D and the
 * README vectors; exit 1 on mismatch.  The loop is re-expressed here (own code)
 * only to read the internal Seed1..Seed8; that transcription is checked against
 * komihash() on random messages of 64..1024 bytes (it calls the upstream
 * komihash_epi / komihash_set_preseed_inner for the parts outside the loop).
 *
 * Experiments
 *  E1  the 73-byte every-seed pair (tied block + swapped tail): collides for all seeds.
 *  E2  the same tied block and swapped tail with ONE extra common random 64-byte
 *      block after the tied block (137 bytes): collision count over many seeds.
 *  E3  the same with block 2 "re-tied" using the XOR relations that hold when the
 *      additions commute with XOR (the best case): collision count.
 *  E4  the re-tie law on real states: after an all-four-tied block the upper
 *      words are T_k = (S5 ^ I_k) + h (h = common high product half).  Lemma:
 *      Pr[((x^c)+h) ^ (x+h) == c on bits 0..w-1] = 2^-popcount(c & (2^(w-1)-1)).
 *      Measured for c = I6, I7, I8 and jointly, at w = 8, 12, 16, 20.
 * Build: cc -std=c11 -O2 -Iupstream/komihash -o komi_long komi_long.c -lm
 * Run:   ./komi_long [log2_seeds_E2E3=24] [log2_seeds_E4=24]
 */
#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "komihash.h"

static uint64_t rotl64(uint64_t x, int r) { return (x << r) | (x >> (64 - r)); }
static uint64_t sm;
static uint64_t splitmix64(void) { uint64_t z = (sm += UINT64_C(0x9e3779b97f4a7c15)); z = (z ^ (z >> 30)) * UINT64_C(0xbf58476d1ce4e5b9); z = (z ^ (z >> 27)) * UINT64_C(0x94d049bb133111eb); return z ^ (z >> 31); }
static uint64_t xs[4];
static uint64_t rng(void) { const uint64_t r = rotl64(xs[1] * 5, 7) * 9, t = xs[1] << 17; xs[2] ^= xs[0]; xs[3] ^= xs[1]; xs[1] ^= xs[2]; xs[0] ^= xs[3]; xs[2] ^= t; xs[3] = rotl64(xs[3], 45); return r; }
static void rng_init(uint64_t s) { sm = s; for (int i = 0; i < 4; i++) xs[i] = splitmix64(); }
static uint64_t G(const uint8_t *p) { uint64_t v; memcpy(&v, p, 8); return v; }
static void P(uint8_t *p, uint64_t v) { memcpy(p, &v, 8); }

/* KOMIHASH_IVAL2..8 (the header #undefs its macros) */
#define I2 UINT64_C(0x13198A2E03707344)
#define I3 UINT64_C(0xA4093822299F31D0)
#define I4 UINT64_C(0x082EFA98EC4E6C89)
#define I6 UINT64_C(0xBE5466CF34E90C6C)
#define I7 UINT64_C(0xC0AC29B7C97C50DD)
#define I8 UINT64_C(0x3F84D5B5B5470917)

/* own transcription of the loop; S[1..8] after `nblocks` blocks */
static void loop_state(const uint8_t *m, size_t nblocks, uint64_t seed, uint64_t S[9]) {
    uint64_t s1, s5; komihash_set_preseed_inner(&s1, &s5, seed);
    S[1] = s1; S[5] = s5; S[2] = I2 ^ s1; S[3] = I3 ^ s1; S[4] = I4 ^ s1; S[6] = I6 ^ s5; S[7] = I7 ^ s5; S[8] = I8 ^ s5;
    for (size_t b = 0; b < nblocks; b++, m += 64) {
        for (int l = 1; l <= 4; l++) {
            unsigned __int128 r = (unsigned __int128)(G(m + 8 * (l - 1)) ^ S[l]) * (G(m + 32 + 8 * (l - 1)) ^ S[l + 4]);
            S[l] = (uint64_t)r; S[l + 4] += (uint64_t)(r >> 64);
        }
        S[4] ^= S[7]; S[1] ^= S[8]; S[2] ^= S[5]; S[3] ^= S[6];
    }
}
static uint64_t own_hash(const uint8_t *m, size_t len, uint64_t seed) {
    uint64_t S[9]; size_t nb = 0;
    if (len > 63) { nb = 1 + (len - 64) / 64; }
    loop_state(m, nb, seed, S);
    uint64_t s1 = S[1] ^ S[2] ^ S[3] ^ S[4], s5 = S[5] ^ S[6] ^ S[7] ^ S[8];
    return komihash_epi(m + 64 * nb, len - 64 * nb, s1, s5);
}

/* all-four-tied block: every lane computes (m0^S1)(m4^S5) */
static void tie4(uint8_t *b, uint64_t m0, uint64_t m4) {
    P(b + 0, m0); P(b + 8, m0 ^ I2); P(b + 16, m0 ^ I3); P(b + 24, m0 ^ I4);
    P(b + 32, m4); P(b + 40, m4 ^ I6); P(b + 48, m4 ^ I7); P(b + 56, m4 ^ I8);
}
/* pairs (1,2),(3,4) tied given relations S2=S1^r12, S6=S5^r56, S4=S3^r34, S8=S7^r78 */
static void tie_pairs(uint8_t *b, uint64_t m0, uint64_t m4, uint64_t m2, uint64_t m6, uint64_t r12, uint64_t r56, uint64_t r34, uint64_t r78) {
    P(b + 0, m0); P(b + 8, m0 ^ r12); P(b + 16, m2); P(b + 24, m2 ^ r34);
    P(b + 32, m4); P(b + 40, m4 ^ r56); P(b + 48, m6); P(b + 56, m6 ^ r78);
}
static void tail_swap(uint8_t *t, uint8_t *t2) {  /* [y,01,00x6,z] vs [z,01,00x6,y] */
    memset(t, 0, 9); memset(t2, 0, 9);
    t[0] = 0x3c; t[1] = 1; t[8] = 0xa7; t2[0] = 0xa7; t2[1] = 1; t2[8] = 0x3c;
}
static int popc(uint64_t x) { return __builtin_popcountll(x); }

int main(int argc, char **argv) {
    int lgA = argc > 1 ? atoi(argv[1]) : 24, lgB = argc > 2 ? atoi(argv[2]) : 24;
    /* startup checks */
    int bad = 0;
    bad |= komihash("This is a 32-byte testing string", 32, 0) != UINT64_C(0x05ad960802903a9d);
    bad |= komihash("The cat is out of the bag", 25, UINT64_C(0x0123456789abcdef)) != UINT64_C(0x5b1da0b43545d196);
    uint8_t key[256], table[256 * 8];
    for (int i = 0; i < 256; i++) { key[i] = (uint8_t)i; uint64_t h = komihash(key, (size_t)i, (uint64_t)(256 - i)); memcpy(table + 8 * i, &h, 8); }
    uint32_t v = (uint32_t)komihash(table, sizeof table, 0);
    printf("README vectors %s; SMHasher3 verification %08" PRIX32 " (expected 8157FF6D)\n", bad ? "FAIL" : "ok", v);
    if (bad || v != 0x8157FF6Du) return 1;
    rng_init(UINT64_C(0x6B6F6D694C));
    { uint8_t m[1024]; int mm = 0;
      for (int t = 0; t < 20000; t++) { size_t len = 64 + rng() % 961; for (size_t i = 0; i < len; i++) m[i] = (uint8_t)rng(); uint64_t s = rng(); mm += own_hash(m, len, s) != komihash(m, len, s); }
      printf("own loop transcription vs komihash(): %d mismatches in 20000 random messages of 64..1024 B\n", mm);
      if (mm) return 1; }

    uint8_t A[256], B[256];
    /* E1 */
    { tie4(A, rng(), rng()); tail_swap(A + 64, B + 64); memcpy(B, A, 64);
      uint64_t n = UINT64_C(1) << 20, c = 0; for (uint64_t i = 0; i < n; i++) { uint64_t s = rng(); c += komihash(A, 73, s) == komihash(B, 73, s); }
      printf("E1  73 B (all-four-tied block + swapped tail): %" PRIu64 "/%" PRIu64 " seeds collide\n", c, n); }
    /* E2 */
    { uint64_t n = UINT64_C(1) << lgA, c = 0;
      tie4(A, rng(), rng()); for (int i = 64; i < 128; i++) A[i] = (uint8_t)rng(); tail_swap(A + 128, B + 128); memcpy(B, A, 128);
      for (uint64_t i = 0; i < n; i++) { uint64_t s = rng(); c += komihash(A, 137, s) == komihash(B, 137, s); }
      printf("E2  137 B (tied block, common random block, swapped tail): %" PRIu64 "/%" PRIu64 " seeds collide (0 expected: ties gone)\n", c, n); }
    /* E3: block 2 tied with the commuted relations  S2^S1 = I8 (lower: T8^T5), S6^S5 = I6, S4^S3 = I6^I7, S8^S7 = I7^I8 */
    { uint64_t n = UINT64_C(1) << lgA, c = 0, rel = 0;
      tie4(A, rng(), rng());
      tie_pairs(A + 64, rng(), rng(), rng(), rng(), I8, I6, I6 ^ I7, I7 ^ I8);
      tail_swap(A + 128, B + 128); memcpy(B, A, 128);
      for (uint64_t i = 0; i < n; i++) {
          uint64_t s = rng(); c += komihash(A, 137, s) == komihash(B, 137, s);
          uint64_t S[9]; loop_state(A, 1, s, S);
          rel += ((S[1] ^ S[2]) == I8) && ((S[5] ^ S[6]) == I6) && ((S[3] ^ S[4]) == (I6 ^ I7)) && ((S[7] ^ S[8]) == (I7 ^ I8));
      }
      printf("E3  137 B (all-four-tied block 1, block 2 re-tied assuming commutation, swapped tail): %" PRIu64 "/%" PRIu64 " seeds collide; relations held on %" PRIu64 " seeds (lemma: 2^-%d)\n",
             c, n, rel, popc((I6 | I7 | I8) & ~(UINT64_C(1) << 63))); }
    /* E3b (logic check, NOT an attack: block 2 is built from the seed's true relations) */
    { uint64_t n = UINT64_C(1) << 16, c = 0;
      tie4(A, rng(), rng());
      uint64_t b0 = rng(), b4 = rng(), b2 = rng(), b6 = rng();
      for (uint64_t i = 0; i < n; i++) {
          uint64_t s = rng(), S[9]; loop_state(A, 1, s, S);
          tie_pairs(A + 64, b0, b4, b2, b6, S[1] ^ S[2], S[5] ^ S[6], S[3] ^ S[4], S[7] ^ S[8]);
          tail_swap(A + 128, B + 128); memcpy(B, A, 128);
          c += komihash(A, 137, s) == komihash(B, 137, s);
      }
      printf("E3b logic check: block 2 tied with each seed's TRUE relations (adaptive, not an attack): %" PRIu64 "/%" PRIu64 " collide\n", c, n); }
    /* E4 */
    { uint64_t n = UINT64_C(1) << lgB; int ws[4] = { 8, 12, 16, 20 };
      uint64_t cnt[4][4] = {{0}};
      tie4(A, rng(), rng());
      for (uint64_t i = 0; i < n; i++) {
          uint64_t s = rng(), S[9]; loop_state(A, 1, s, S);
          /* T_k = S[k] after the block (upper words); lower words S1..S4 = lo ^ T_(8,5,6,7) */
          uint64_t d6 = S[5] ^ S[6], d7 = S[5] ^ S[7], d8 = S[5] ^ S[8];
          for (int w = 0; w < 4; w++) {
              uint64_t mk = (UINT64_C(1) << ws[w]) - 1;
              int e6 = ((d6 ^ I6) & mk) == 0, e7 = ((d7 ^ I7) & mk) == 0, e8 = ((d8 ^ I8) & mk) == 0;
              cnt[w][0] += e6; cnt[w][1] += e7; cnt[w][2] += e8; cnt[w][3] += e6 && e7 && e8;
          }
      }
      printf("E4  re-tie law on real states after an all-four-tied block (%" PRIu64 " seeds): measured log2 Pr vs lemma -popcount(c & (2^(w-1)-1))\n", n);
      const char *nm[4] = { "I6", "I7", "I8", "all three" };
      uint64_t cs[4] = { I6, I7, I8, I6 | I7 | I8 };
      for (int w = 0; w < 4; w++) {
          uint64_t low = (UINT64_C(1) << (ws[w] - 1)) - 1;
          printf("    w=%2d:", ws[w]);
          for (int k = 0; k < 4; k++) printf("  %s %" PRIu64 " (2^%.2f vs 2^-%d)", nm[k], cnt[w][k], cnt[w][k] ? log2((double)cnt[w][k] / n) : -INFINITY, popc(cs[k] & low));
          printf("\n");
      }
      printf("    full width (w=64, bit 63 exempt): I6 2^-%d, I7 2^-%d, I8 2^-%d, all three 2^-%d; pairing (1,3)(2,4) chain 2^-%d per block\n",
             popc(I6 & ~(UINT64_C(1) << 63)), popc(I7 & ~(UINT64_C(1) << 63)), popc(I8 & ~(UINT64_C(1) << 63)), popc((I6 | I7 | I8) & ~(UINT64_C(1) << 63)),
             popc(I7 & ~(UINT64_C(1) << 63)) + popc((I6 ^ I8) & ~(UINT64_C(1) << 63)));
    }
    return 0;
}
