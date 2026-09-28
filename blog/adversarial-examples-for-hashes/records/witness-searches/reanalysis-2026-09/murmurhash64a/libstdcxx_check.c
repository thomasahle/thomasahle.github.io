/*
 * libstdcxx_check.c -- checks that libstdc++'s 64-bit std::_Hash_bytes (the
 * function behind std::hash<std::string> on LP64 targets, default seed
 * 0xc70f6907) computes MurmurHash64A, and that the 8-vs-7-byte witness
 * collides under that seed.
 *
 * MurmurHash64A: SMHasher3 hashes/murmurhash2.cpp (MurmurHash2_64), Austin
 * Appleby, public domain; little-endian loads.  The SMHasher3 verification
 * value 0x1F0D3804 is recomputed first; exit 1 on mismatch.
 * _Hash_bytes: functional transcription of gcc libstdc++-v3/libsupc++/
 * hash_bytes.cc (the __SIZEOF_SIZE_T__ == 8 branch), GPLv3 with the GCC
 * Runtime Library Exception; see SOURCES.md for the pinned commit.
 *
 * Build: cc -std=c11 -O2 -o libstdcxx_check libstdcxx_check.c
 * Run:   ./libstdcxx_check
 */
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static uint64_t GET_U64(const uint8_t *p) { uint64_t v = 0; for (int i = 7; i >= 0; i--) v = (v << 8) | p[i]; return v; }

static uint64_t MurmurHash2_64(const void *in, size_t len, uint64_t seed) {
    const uint64_t m = UINT64_C(0xc6a4a7935bd1e995); const int r = 47;
    uint64_t h = seed ^ (len * m);
    const uint8_t *data = (const uint8_t *)in, *end = data + len - (len & 7);
    while (data != end) { uint64_t k = GET_U64(data); k *= m; k ^= k >> r; k *= m; h ^= k; h *= m; data += 8; }
    switch (len & 7) {
    case 7: h ^= (uint64_t)data[6] << 48; /* FALLTHROUGH */
    case 6: h ^= (uint64_t)data[5] << 40; /* FALLTHROUGH */
    case 5: h ^= (uint64_t)data[4] << 32; /* FALLTHROUGH */
    case 4: h ^= (uint64_t)data[3] << 24; /* FALLTHROUGH */
    case 3: h ^= (uint64_t)data[2] << 16; /* FALLTHROUGH */
    case 2: h ^= (uint64_t)data[1] << 8;  /* FALLTHROUGH */
    case 1: h ^= (uint64_t)data[0]; h *= m;
    }
    h ^= h >> r; h *= m; h ^= h >> r;
    return h;
}

/* libstdc++ hash_bytes.cc, 64-bit size_t branch */
static uint64_t shift_mix(uint64_t v) { return v ^ (v >> 47); }
static uint64_t load_bytes(const uint8_t *p, int n) { uint64_t result = 0; --n; do result = (result << 8) + p[n]; while (--n >= 0); return result; }
static uint64_t Hash_bytes(const void *ptr, size_t len, uint64_t seed) {
    const uint64_t mul = ((uint64_t)0xc6a4a793UL << 32) + (uint64_t)0x5bd1e995UL;
    const uint8_t *buf = (const uint8_t *)ptr;
    const size_t len_aligned = len & ~(size_t)0x7;
    const uint8_t *end = buf + len_aligned;
    uint64_t hash = seed ^ (len * mul);
    for (const uint8_t *p = buf; p != end; p += 8) { const uint64_t data = shift_mix(GET_U64(p) * mul) * mul; hash ^= data; hash *= mul; }
    if ((len & 0x7) != 0) { const uint64_t data = load_bytes(end, (int)(len & 0x7)); hash ^= data; hash *= mul; }
    hash = shift_mix(hash) * mul;
    hash = shift_mix(hash);
    return hash;
}

static uint32_t verify64(void) {
    uint8_t key[256] = {0}, hashes[8 * 256], total[8];
    for (int i = 0; i < 256; i++) { uint64_t h = MurmurHash2_64(key, (size_t)i, (uint64_t)(256 - i)); for (int b = 0; b < 8; b++) hashes[8 * i + b] = (uint8_t)(h >> (8 * b)); key[i] = (uint8_t)i; }
    uint64_t h = MurmurHash2_64(hashes, sizeof hashes, 0); for (int b = 0; b < 8; b++) total[b] = (uint8_t)(h >> (8 * b));
    return (uint32_t)total[0] | (uint32_t)total[1] << 8 | (uint32_t)total[2] << 16 | (uint32_t)total[3] << 24;
}

static uint64_t sm_state = 0x5eed;
static uint64_t splitmix64(void) { uint64_t z = (sm_state += UINT64_C(0x9e3779b97f4a7c15)); z = (z ^ (z >> 30)) * UINT64_C(0xbf58476d1ce4e5b9); z = (z ^ (z >> 27)) * UINT64_C(0x94d049bb133111eb); return z ^ (z >> 31); }

int main(void) {
    uint32_t v = verify64();
    printf("SMHasher3 verification MurmurHash2_64: 0x%08" PRIX32 " (expected 0x1F0D3804)\n", v);
    if (v != UINT32_C(0x1F0D3804)) { printf("MISMATCH\n"); return 1; }
    static uint8_t buf[512];
    uint64_t n = UINT64_C(1) << 20, diff = 0;
    for (uint64_t t = 0; t < n; t++) {
        size_t len = (size_t)(splitmix64() % 300);
        for (size_t i = 0; i < len; i++) buf[i] = (uint8_t)splitmix64();
        uint64_t seed = (t & 1) ? UINT64_C(0xc70f6907) : splitmix64();
        diff += MurmurHash2_64(buf, len, seed) != Hash_bytes(buf, len, seed);
    }
    printf("_Hash_bytes vs MurmurHash64A: %" PRIu64 " mismatches in %" PRIu64 " random (length 0..299, seed) inputs\n", diff, n);
    /* the 8-vs-7-byte key-free witness */
    const uint8_t w8[8] = {0xdc,0xc9,0x45,0x91,0x2a,0x27,0x4b,0x56};
    const uint8_t t7[7] = {'m','u','r','m','u','r','7'};
    uint64_t a = Hash_bytes(w8, 8, 0xc70f6907), b = Hash_bytes(t7, 7, 0xc70f6907);
    printf("witness under the std::hash default seed 0xc70f6907: 8B %016" PRIx64 "  7B %016" PRIx64 " %s\n", a, b, a == b ? "COLLIDE" : "differ");
    return (diff == 0 && a == b) ? 0 : 2;
}
