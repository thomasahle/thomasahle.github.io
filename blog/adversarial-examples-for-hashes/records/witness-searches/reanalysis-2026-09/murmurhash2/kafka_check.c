/*
 * kafka_check.c -- checks that Apache Kafka's Utils.murmur2 (the hash behind
 * the default producer partitioner, BuiltInPartitioner.partitionForKey) is
 * MurmurHash2 (32-bit) with the fixed seed 0x9747b28c, and that the 8-byte
 * key-free pair lands in the same partition for every partition count.
 *
 * MurmurHash2_32: SMHasher3 hashes/murmurhash2.cpp, Austin Appleby, public
 * domain; little-endian loads.  SMHasher3 verification 0x27864C1E is
 * recomputed first; exit 1 on mismatch.
 * kafka_murmur2: transcription of Utils.murmur2 (Apache-2.0; Java int
 * arithmetic wraps mod 2^32, >>> is a logical shift, words are read with a
 * LITTLE_ENDIAN VarHandle); see SOURCES.md for the pinned commit.
 *
 * Build: cc -std=c11 -O2 -o kafka_check kafka_check.c
 * Run:   ./kafka_check
 */
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static uint32_t GET_U32(const uint8_t *p) { return (uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24; }

static uint32_t MurmurHash2_32(const void *in, size_t olen, uint32_t seed) {
    const uint32_t m = 0x5bd1e995; const int r = 24;
    size_t len = olen; uint32_t h = seed ^ (uint32_t)olen;
    const uint8_t *data = (const uint8_t *)in;
    while (len >= 4) { uint32_t k = GET_U32(data); k *= m; k ^= k >> r; k *= m; h *= m; h ^= k; data += 4; len -= 4; }
    switch (len) {
    case 3: h ^= (uint32_t)data[2] << 16; /* FALLTHROUGH */
    case 2: h ^= (uint32_t)data[1] << 8;  /* FALLTHROUGH */
    case 1: h ^= data[0]; h *= m;
    }
    h ^= h >> 13; h *= m; h ^= h >> 15;
    return h;
}

/* Utils.murmur2(byte[] data) */
static int32_t kafka_murmur2(const uint8_t *data, int length) {
    uint32_t seed = 0x9747b28c, m = 0x5bd1e995; const int r = 24;
    uint32_t h = seed ^ (uint32_t)length;
    int length4 = length >> 2;
    for (int i = 0; i < length4; i++) { uint32_t k = GET_U32(data + (i << 2)); k *= m; k ^= k >> r; k *= m; h *= m; h ^= k; }
    int index = length4 << 2;
    switch (length - index) {
    case 3: h ^= (uint32_t)(data[index + 2] & 0xff) << 16; /* FALLTHROUGH */
    case 2: h ^= (uint32_t)(data[index + 1] & 0xff) << 8;  /* FALLTHROUGH */
    case 1: h ^= (uint32_t)(data[index] & 0xff); h *= m;
    }
    h ^= h >> 13; h *= m; h ^= h >> 15;
    return (int32_t)h;
}
static int32_t toPositive(int32_t x) { return x & 0x7fffffff; }

static uint32_t verify32(void) {
    uint8_t key[256] = {0}, hashes[4 * 256], total[4];
    for (int i = 0; i < 256; i++) { uint32_t h = MurmurHash2_32(key, (size_t)i, (uint32_t)(256 - i)); for (int b = 0; b < 4; b++) hashes[4 * i + b] = (uint8_t)(h >> (8 * b)); key[i] = (uint8_t)i; }
    uint32_t h = MurmurHash2_32(hashes, sizeof hashes, 0); for (int b = 0; b < 4; b++) total[b] = (uint8_t)(h >> (8 * b));
    return (uint32_t)total[0] | (uint32_t)total[1] << 8 | (uint32_t)total[2] << 16 | (uint32_t)total[3] << 24;
}
static uint64_t sm_state = 0x6b61666b61;
static uint64_t splitmix64(void) { uint64_t z = (sm_state += UINT64_C(0x9e3779b97f4a7c15)); z = (z ^ (z >> 30)) * UINT64_C(0xbf58476d1ce4e5b9); z = (z ^ (z >> 27)) * UINT64_C(0x94d049bb133111eb); return z ^ (z >> 31); }

int main(void) {
    uint32_t v = verify32();
    printf("SMHasher3 verification MurmurHash2_32: 0x%08" PRIX32 " (expected 0x27864C1E)\n", v);
    if (v != UINT32_C(0x27864C1E)) { printf("MISMATCH\n"); return 1; }
    static uint8_t buf[512]; uint64_t n = UINT64_C(1) << 20, diff = 0;
    for (uint64_t t = 0; t < n; t++) {
        int len = (int)(splitmix64() % 300);
        for (int i = 0; i < len; i++) buf[i] = (uint8_t)splitmix64();
        diff += (uint32_t)kafka_murmur2(buf, len) != MurmurHash2_32(buf, (size_t)len, 0x9747b28c);
    }
    printf("Kafka murmur2 vs MurmurHash2_32(seed 0x9747b28c): %" PRIu64 " mismatches in %" PRIu64 " random inputs (length 0..299)\n", diff, n);
    const uint8_t a[8] = {0x6d,0x75,0x72,0x6d,0x75,0x72,0x32,0x21};   /* "murmur2!" */
    const uint8_t b[8] = {0xed,0x53,0xff,0xba,0xf5,0x50,0xbf,0x6e};
    int32_t ha = kafka_murmur2(a, 8), hb = kafka_murmur2(b, 8);
    printf("pair under Kafka murmur2: %08" PRIx32 " %08" PRIx32 " -> %s; partition for 12 partitions: %d %d\n",
           (uint32_t)ha, (uint32_t)hb, ha == hb ? "COLLIDE" : "differ", toPositive(ha) % 12, toPositive(hb) % 12);
    return (diff == 0 && ha == hb) ? 0 : 2;
}
