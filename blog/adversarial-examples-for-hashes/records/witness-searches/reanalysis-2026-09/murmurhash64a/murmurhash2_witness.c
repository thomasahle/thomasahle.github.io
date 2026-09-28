/*
 * murmurhash2_witness.c -- seed-independent collisions and multicollisions in
 * MurmurHash2 (32-bit) and MurmurHash64A (the 64-bit MurmurHash2; the same
 * function as libstdc++'s 64-bit std::_Hash_bytes).
 *
 * MurmurHash2 is by Austin Appleby (public domain).  Code transcribed from
 * SMHasher3 hashes/murmurhash2.cpp (MurmurHash2_32, MurmurHash2_64), C++
 * templates removed, little-endian loads.  SMHasher3 verification values
 * 0x27864C1E (MurmurHash2_32) and 0x1F0D3804 (MurmurHash2_64) are recomputed
 * at startup.
 *
 * Mechanism (Aumasson-Bernstein-Bosslet 2012 used it for MurmurHash2 and 3):
 * each block is k = mix(w) (a public bijection) followed by
 *     32-bit:  h *= m; h ^= k          64A:  h ^= k; h *= m
 * An XOR difference in the top bit of h survives *m (m odd), so a top-bit
 * flip in k (w' = mix^-1(mix(w) ^ topbit)) is cancelled by a second top-bit
 * flip later.  With n blocks, the 2^(n-1) messages with an even number of
 * flipped blocks collide under every seed.
 * MurmurHash64A extra: h0 = seed ^ (len*m) differs between lengths 8n and
 * 8n-1 by the public XOR constant (8n*m) ^ ((8n-1)*m); the first block
 * absorbs it and a 7-byte tail replaces the last block (both do h ^= v;
 * h *= m), giving a key-free pair at 8 vs 7 bytes (L = 1).
 *
 * Build: cc -std=c11 -O2 -o murmurhash2_witness murmurhash2_witness.c
 * Run:   ./murmurhash2_witness [log2_keys=12] [rng_seed=0x2] [--exhaustive]
 */
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline uint32_t GET_U32(const uint8_t *p, size_t o) { return (uint32_t)p[o] | (uint32_t)p[o+1] << 8 | (uint32_t)p[o+2] << 16 | (uint32_t)p[o+3] << 24; }
static inline uint64_t GET_U64(const uint8_t *p, size_t o) { uint64_t v = 0; for (int i = 7; i >= 0; i--) v = (v << 8) | p[o + i]; return v; }

static uint32_t MurmurHash2_32( const void * in, const size_t olen, const uint32_t seed ) {
    const uint32_t m   = 0x5bd1e995;
    const uint32_t r   = 24;
    size_t         len = olen;
    uint32_t h = seed ^ olen;
    const uint8_t * data = (const uint8_t *)in;
    while (len >= 4) {
        uint32_t k = GET_U32(data, 0);
        k    *= m;
        k    ^= k >> r;
        k    *= m;
        h    *= m;
        h    ^= k;
        data += 4;
        len  -= 4;
    }
    switch (len) {
    case 3: h ^= data[2] << 16; /* FALLTHROUGH */
    case 2: h ^= data[1] <<  8; /* FALLTHROUGH */
    case 1: h ^= data[0];
            h *= m;
    }
    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;
    return h;
}

static uint64_t MurmurHash2_64( const void * in, const size_t len, const uint64_t seed ) {
    const uint64_t m     = UINT64_C(0xc6a4a7935bd1e995);
    const uint32_t r     = 47;
    uint64_t h           = seed ^ (len * m);
    const uint8_t * data = (const uint8_t *)in;
    const uint8_t * end  = data + len - (len & 7);
    while (data != end) {
        uint64_t k = GET_U64(data, 0);
        k    *= m;
        k    ^= k >> r;
        k    *= m;
        h    ^= k;
        h    *= m;
        data += 8;
    }
    switch (len & 7) {
    case 7: h ^= (uint64_t)(data[6]) << 48; /* FALLTHROUGH */
    case 6: h ^= (uint64_t)(data[5]) << 40; /* FALLTHROUGH */
    case 5: h ^= (uint64_t)(data[4]) << 32; /* FALLTHROUGH */
    case 4: h ^= (uint64_t)(data[3]) << 24; /* FALLTHROUGH */
    case 3: h ^= (uint64_t)(data[2]) << 16; /* FALLTHROUGH */
    case 2: h ^= (uint64_t)(data[1]) <<  8; /* FALLTHROUGH */
    case 1: h ^= (uint64_t)(data[0]);
            h *= m;
    }
    h ^= h >> r;
    h *= m;
    h ^= h >> r;
    return h;
}

/* SMHasher3 output: PUT_U32/PUT_U64 little-endian (native, bswap=false). */
static uint32_t verify32(void) {
    uint8_t key[256] = {0}, hashes[4 * 256], total[4];
    for (int i = 0; i < 256; i++) { uint32_t h = MurmurHash2_32(key, (size_t)i, (uint32_t)(256 - i)); for (int b = 0; b < 4; b++) hashes[4 * i + b] = (uint8_t)(h >> (8 * b)); key[i] = (uint8_t)i; }
    uint32_t h = MurmurHash2_32(hashes, sizeof hashes, 0); for (int b = 0; b < 4; b++) total[b] = (uint8_t)(h >> (8 * b));
    return (uint32_t)total[0] | (uint32_t)total[1] << 8 | (uint32_t)total[2] << 16 | (uint32_t)total[3] << 24;
}
static uint32_t verify64(void) {
    uint8_t key[256] = {0}, hashes[8 * 256], total[8];
    for (int i = 0; i < 256; i++) { uint64_t h = MurmurHash2_64(key, (size_t)i, (uint64_t)(256 - i)); for (int b = 0; b < 8; b++) hashes[8 * i + b] = (uint8_t)(h >> (8 * b)); key[i] = (uint8_t)i; }
    uint64_t h = MurmurHash2_64(hashes, sizeof hashes, 0); for (int b = 0; b < 8; b++) total[b] = (uint8_t)(h >> (8 * b));
    return (uint32_t)total[0] | (uint32_t)total[1] << 8 | (uint32_t)total[2] << 16 | (uint32_t)total[3] << 24;
}

static inline uint64_t ROTL64(uint64_t x, int r) { return (x << r) | (x >> (64 - r)); }
static uint64_t splitmix64(uint64_t *s) { uint64_t z = (*s += UINT64_C(0x9e3779b97f4a7c15)); z = (z ^ (z >> 30)) * UINT64_C(0xbf58476d1ce4e5b9); z = (z ^ (z >> 27)) * UINT64_C(0x94d049bb133111eb); return z ^ (z >> 31); }
static uint64_t rs[4];
static void rng_init(uint64_t seed) { for (int i = 0; i < 4; i++) rs[i] = splitmix64(&seed); }
static uint64_t rng(void) { const uint64_t r = ROTL64(rs[1] * 5, 7) * 9, t = rs[1] << 17; rs[2] ^= rs[0]; rs[3] ^= rs[1]; rs[1] ^= rs[2]; rs[0] ^= rs[3]; rs[2] ^= t; rs[3] = ROTL64(rs[3], 45); return r; }
static void put32(uint8_t *p, uint32_t v) { for (int i = 0; i < 4; i++) p[i] = (uint8_t)(v >> (8 * i)); }
static void put64(uint8_t *p, uint64_t v) { for (int i = 0; i < 8; i++) p[i] = (uint8_t)(v >> (8 * i)); }
static void print_msg(const char *name, const uint8_t *m, size_t n) { printf("  %s (%zu bytes) = ", name, n); for (size_t i = 0; i < n; i++) printf("%02x", m[i]); printf("\n"); }

static const uint32_t M32 = 0x5bd1e995; static const uint64_t M64 = UINT64_C(0xc6a4a7935bd1e995);
static uint32_t inv32(uint32_t a) { uint32_t x = a; for (int i = 0; i < 5; i++) x *= 2 - a * x; return x; }
static uint64_t inv64(uint64_t a) { uint64_t x = a; for (int i = 0; i < 6; i++) x *= 2 - a * x; return x; }
static uint32_t mix32(uint32_t k) { k *= M32; k ^= k >> 24; k *= M32; return k; }
static uint32_t unmix32(uint32_t k) { k *= inv32(M32); k ^= k >> 24; return k * inv32(M32); }
static uint64_t mix64(uint64_t k) { k *= M64; k ^= k >> 47; k *= M64; return k; }
static uint64_t unmix64(uint64_t k) { k *= inv64(M64); k ^= k >> 47; return k * inv64(M64); }

/* n-block cube: message index bits choose which of blocks 0..n-2 open a top-bit flip;
   block i XORs (open_i ^ open_{i-1}) * topbit into mix(w_i).  All 2^(n-1) collide for every seed. */
static void cube32(uint8_t *out, const uint8_t *base, int n, uint64_t idx) {
    int prev = 0;
    for (int i = 0; i < n; i++) {
        int open = i < n - 1 ? (int)((idx >> i) & 1) : 0;
        uint32_t k = mix32(GET_U32(base, 4 * i)) ^ ((uint32_t)(open ^ prev) << 31);
        put32(out + 4 * i, unmix32(k)); prev = open;
    }
}
static void cube64(uint8_t *out, const uint8_t *base, int n, uint64_t idx) {
    int prev = 0;
    for (int i = 0; i < n; i++) {
        int open = i < n - 1 ? (int)((idx >> i) & 1) : 0;
        uint64_t k = mix64(GET_U64(base, 8 * i)) ^ ((uint64_t)(open ^ prev) << 63);
        put64(out + 8 * i, unmix64(k)); prev = open;
    }
}

/* distinctness check for a generated family: sort copies, compare neighbours */
static size_t dist_len;
static int dist_cmp(const void *a, const void *b) { return memcmp(a, b, dist_len); }
static uint64_t count_distinct(const uint8_t *fam, size_t len, uint64_t K) {
    uint8_t *c = malloc(K * len); memcpy(c, fam, K * len); dist_len = len;
    qsort(c, K, len, dist_cmp);
    uint64_t d = K ? 1 : 0; for (uint64_t i = 1; i < K; i++) d += memcmp(c + (i - 1) * len, c + i * len, len) != 0;
    free(c); return d;
}

int main(int argc, char **argv) {
    int lg = argc > 1 ? atoi(argv[1]) : 12;
    uint64_t rseed = argc > 2 ? strtoull(argv[2], 0, 0) : 0x2;
    int exhaustive = argc > 3 && !strcmp(argv[3], "--exhaustive");
    uint32_t v32 = verify32(), v64 = verify64();
    printf("SMHasher3 verification MurmurHash2_32: 0x%08" PRIX32 " (expected 0x27864C1E)\n", v32);
    printf("SMHasher3 verification MurmurHash2_64: 0x%08" PRIX32 " (expected 0x1F0D3804)\n", v64);
    if (v32 != UINT32_C(0x27864C1E) || v64 != UINT32_C(0x1F0D3804)) { printf("MISMATCH\n"); return 1; }
    rng_init(rseed);
    int ok = 1; uint64_t NK = UINT64_C(1) << lg;

    /* ---- MurmurHash2_32 pair, 8 bytes ---- */
    uint8_t a[8], b[8]; memcpy(a, "murmur2!", 8); cube32(b, a, 2, 1); memcpy(a, a, 8);
    uint8_t a0[8]; cube32(a0, a, 2, 0);
    printf("\n[M2-32 pair] 8 bytes (L = 1), key-free\n"); print_msg("M1", a0, 8); print_msg("M2", b, 8);
    uint64_t c = 0; for (uint64_t t = 0; t < (UINT64_C(1) << 24); t++) { uint32_t s = (uint32_t)rng(); c += MurmurHash2_32(a0, 8, s) == MurmurHash2_32(b, 8, s); }
    printf("  collisions %" PRIu64 " / %" PRIu64 " random 32-bit seeds\n", c, UINT64_C(1) << 24); ok &= c == (UINT64_C(1) << 24);
    printf("  seed 0x9747b28c (Kafka partitioner seed): %08" PRIx32 " %08" PRIx32 "\n", MurmurHash2_32(a0, 8, 0x9747b28c), MurmurHash2_32(b, 8, 0x9747b28c));
    if (exhaustive) {
        uint64_t e = 0; uint32_t s = 0; do { e += MurmurHash2_32(a0, 8, s) == MurmurHash2_32(b, 8, s); } while (++s);
        printf("  EXHAUSTIVE: %" PRIu64 " of 2^32 seeds collide\n", e); ok &= e == (UINT64_C(1) << 32);
    }

    /* ---- MurmurHash2_32 2^16-way cube, 17 words = 68 bytes ---- */
    { const int n = 17; const uint64_t K = UINT64_C(1) << (n - 1);
      uint8_t base[68]; memset(base,0,68); memcpy(base, "MurmurHash2 32-bit key-free 2^16-way multicollision 68B", 54);
      uint8_t *fam = malloc(K * 68); for (uint64_t i = 0; i < K; i++) cube32(fam + 68 * i, base, n, i);
      { uint64_t d = count_distinct(fam, 68, K); printf("  (next cube) distinct messages: %" PRIu64 " of %" PRIu64 "\n", d, K); ok &= d == K; }
      printf("\n[M2-32 cube] 2^16 messages of 68 bytes (L = 9); generator cube32(base, 17, idx)\n");
      print_msg("X[0]", fam, 68); print_msg("X[65535]", fam + 68 * (K - 1), 68);
      uint64_t all = 0; for (uint64_t t = 0; t < NK; t++) { uint32_t s = (uint32_t)rng(), h0 = MurmurHash2_32(fam, 68, s); uint64_t same = 0; for (uint64_t i = 0; i < K; i++) same += MurmurHash2_32(fam + 68 * i, 68, s) == h0; all += same == K; }
      printf("  all 65536 equal for %" PRIu64 " / %" PRIu64 " random seeds\n", all, NK); ok &= all == NK; free(fam); }

    /* ---- MurmurHash64A pairs ---- */
    uint8_t p16a[16], p16b[16], base16[16]; memcpy(base16, "MurmurHash64A-16", 16); cube64(p16a, base16, 2, 0); cube64(p16b, base16, 2, 1);
    printf("\n[64A pair same length] 16 bytes (L = 2), key-free\n"); print_msg("M1", p16a, 16); print_msg("M2", p16b, 16);
    /* cross-length 8 vs 7 bytes */
    uint8_t t7[7]; memcpy(t7, "murmur7", 7);
    uint64_t tv = 0; for (int i = 6; i >= 0; i--) tv = (tv << 8) | t7[i];
    uint64_t kx = tv ^ ((uint64_t)8 * M64) ^ ((uint64_t)7 * M64);
    uint8_t w8[8]; put64(w8, unmix64(kx));
    printf("[64A pair cross length] 8 bytes vs 7 bytes (L = 1), key-free: mix(w) = t ^ (8m) ^ (7m)\n"); print_msg("M1", w8, 8); print_msg("M2", t7, 7);
    uint64_t c16 = 0, c87 = 0, NP = UINT64_C(1) << 24;
    for (uint64_t t = 0; t < NP; t++) { uint64_t s = rng(); c16 += MurmurHash2_64(p16a, 16, s) == MurmurHash2_64(p16b, 16, s); c87 += MurmurHash2_64(w8, 8, s) == MurmurHash2_64(t7, 7, s); }
    printf("  16-byte pair: %" PRIu64 " / %" PRIu64 " seeds; 8-vs-7 pair: %" PRIu64 " / %" PRIu64 " seeds\n", c16, NP, c87, NP);
    ok &= c16 == NP && c87 == NP;
    uint64_t sx = rng();
    printf("  explicit seed 0x%016" PRIx64 ": 8B %016" PRIx64 "  7B %016" PRIx64 "\n", sx, MurmurHash2_64(w8, 8, sx), MurmurHash2_64(t7, 7, sx));
    printf("  libstdc++ std::hash seed 0xc70f6907: 8B %016" PRIx64 "  7B %016" PRIx64 "\n", MurmurHash2_64(w8, 8, 0xc70f6907), MurmurHash2_64(t7, 7, 0xc70f6907));

    /* ---- MurmurHash64A 2^16-way cube, 17 words = 136 bytes ---- */
    { const int n = 17; const uint64_t K = UINT64_C(1) << (n - 1);
      uint8_t base[136]; memset(base, 0, 136); memcpy(base, "MurmurHash64A key-free 2^16-way multicollision, 17 words", 57);
      uint8_t *fam = malloc(K * 136); for (uint64_t i = 0; i < K; i++) cube64(fam + 136 * i, base, n, i);
      { uint64_t d = count_distinct(fam, 136, K); printf("  (next cube) distinct messages: %" PRIu64 " of %" PRIu64 "\n", d, K); ok &= d == K; }
      printf("\n[64A cube] 2^16 messages of 136 bytes (L = 17); generator cube64(base, 17, idx)\n");
      print_msg("X[65535]", fam + 136 * (K - 1), 136);
      uint64_t all = 0; for (uint64_t t = 0; t < NK; t++) { uint64_t s = rng(), h0 = MurmurHash2_64(fam, 136, s); uint64_t same = 0; for (uint64_t i = 0; i < K; i++) same += MurmurHash2_64(fam + 136 * i, 136, s) == h0; all += same == K; }
      printf("  all 65536 equal for %" PRIu64 " / %" PRIu64 " random seeds\n", all, NK); ok &= all == NK; free(fam); }
    printf("\nsummary: %s\n", ok ? "all key-free claims hold" : "FAIL");
    return ok ? 0 : 2;
}
