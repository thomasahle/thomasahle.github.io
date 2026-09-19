/* Fold-only rate estimator for XXH3-64 complement pairs (17..128-byte path).
 * A 16-byte block (w0,w1) at secret offset 16*o versus its bitwise complement
 * collides in XXH3_64bits_withSeed iff
 *   fold(w0^(K[2o]+s), w1^(K[2o+1]-s)) == fold(~w0^(K[2o]+s), ~w1^(K[2o+1]-s)),
 * where fold(a,b) = lo64(a*b) ^ hi64(a*b) (XXH3_mul128_fold64) and K[i] is the
 * i-th little-endian 64-bit word of the default secret.  The other blocks of the
 * message are unchanged, so they cancel in the sum, and XXH3_avalanche is bijective.
 * (Only o=0 and o=1 are realised at 32 bytes; o=2,3 need 64 bytes; o>=4 need 128.)
 *
 * stdin lines: "<o> <w0 hex> <w1 hex>"; stdout: same plus " <count> <N>".
 * argv: [log2 N (default 24)] [stream salt (default 1)]
 * The seed stream is xoshiro256** from four SplitMix64 outputs of the salt and
 * is restarted for every candidate (common random numbers for the comparison).
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
static const uint8_t kSecret[192] = {
    0xb8, 0xfe, 0x6c, 0x39, 0x23, 0xa4, 0x4b, 0xbe, 0x7c, 0x01, 0x81, 0x2c, 0xf7, 0x21, 0xad, 0x1c,
    0xde, 0xd4, 0x6d, 0xe9, 0x83, 0x90, 0x97, 0xdb, 0x72, 0x40, 0xa4, 0xa4, 0xb7, 0xb3, 0x67, 0x1f,
    0xcb, 0x79, 0xe6, 0x4e, 0xcc, 0xc0, 0xe5, 0x78, 0x82, 0x5a, 0xd0, 0x7d, 0xcc, 0xff, 0x72, 0x21,
    0xb8, 0x08, 0x46, 0x74, 0xf7, 0x43, 0x24, 0x8e, 0xe0, 0x35, 0x90, 0xe6, 0x81, 0x3a, 0x26, 0x4c,
    0x3c, 0x28, 0x52, 0xbb, 0x91, 0xc3, 0x00, 0xcb, 0x88, 0xd0, 0x65, 0x8b, 0x1b, 0x53, 0x2e, 0xa3,
    0x71, 0x64, 0x48, 0x97, 0xa2, 0x0d, 0xf9, 0x4e, 0x38, 0x19, 0xef, 0x46, 0xa9, 0xde, 0xac, 0xd8,
    0xa8, 0xfa, 0x76, 0x3f, 0xe3, 0x9c, 0x34, 0x3f, 0xf9, 0xdc, 0xbb, 0xc7, 0xc7, 0x0b, 0x4f, 0x1d,
    0x8a, 0x51, 0xe0, 0x4b, 0xcd, 0xb4, 0x59, 0x31, 0xc8, 0x9f, 0x7e, 0xc9, 0xd9, 0x78, 0x73, 0x64,
    0xea, 0xc5, 0xac, 0x83, 0x34, 0xd3, 0xeb, 0xc3, 0xc5, 0x81, 0xa0, 0xff, 0xfa, 0x13, 0x63, 0xeb,
    0x17, 0x0d, 0xdd, 0x51, 0xb7, 0xf0, 0xda, 0x49, 0xd3, 0x16, 0x55, 0x26, 0x29, 0xd4, 0x68, 0x9e,
    0x2b, 0x16, 0xbe, 0x58, 0x7d, 0x47, 0xa1, 0xfc, 0x8f, 0xf8, 0xb8, 0xd1, 0x7a, 0xd0, 0x31, 0xce,
    0x45, 0xcb, 0x3a, 0x8f, 0x95, 0x16, 0x04, 0x28, 0xaf, 0xd7, 0xfb, 0xca, 0xbb, 0x4b, 0x40, 0x7e,
};
static uint64_t K[24];
static inline uint64_t fold(uint64_t a, uint64_t b) {
    unsigned __int128 p = (unsigned __int128)a * b;
    return (uint64_t)p ^ (uint64_t)(p >> 64);
}
static uint64_t st[4];
static inline uint64_t rotl(uint64_t x, unsigned n) { return x << n | x >> (64 - n); }
static uint64_t splitmix(uint64_t *s) {
    uint64_t z = (*s += UINT64_C(0x9e3779b97f4a7c15));
    z = (z ^ (z >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
    z = (z ^ (z >> 27)) * UINT64_C(0x94d049bb133111eb);
    return z ^ (z >> 31);
}
static void rng_init(uint64_t s) { for (int i = 0; i < 4; i++) st[i] = splitmix(&s); }
static inline uint64_t rng_next(void) {
    uint64_t r = rotl(st[1] * 5, 7) * 9, t = st[1] << 17;
    st[2] ^= st[0]; st[3] ^= st[1]; st[1] ^= st[2]; st[0] ^= st[3]; st[2] ^= t; st[3] = rotl(st[3], 45);
    return r;
}
int main(int argc, char **argv) {
    unsigned lg = argc > 1 ? (unsigned)atoi(argv[1]) : 24;
    uint64_t salt = argc > 2 ? strtoull(argv[2], 0, 0) : 1;
    uint64_t N = UINT64_C(1) << lg;
    for (int i = 0; i < 24; i++) { uint64_t v = 0; for (int j = 0; j < 8; j++) v |= (uint64_t)kSecret[8*i+j] << (8*j); K[i] = v; }
    char line[256];
    while (fgets(line, sizeof line, stdin)) {
        unsigned o; uint64_t w0, w1;
        if (sscanf(line, "%u %" SCNx64 " %" SCNx64, &o, &w0, &w1) != 3 || o > 7) continue;
        uint64_t ka = K[2*o], kb = K[2*o+1], c = 0;
        rng_init(salt);
        for (uint64_t i = 0; i < N; i++) {
            uint64_t s = rng_next();
            uint64_t A = w0 ^ (ka + s), B = w1 ^ (kb - s);
            c += (fold(A, B) == fold(~A, ~B));
        }
        printf("%u %016" PRIx64 " %016" PRIx64 " %" PRIu64 " %" PRIu64 "\n", o, w0, w1, c, N);
        fflush(stdout);
    }
    return 0;
}
