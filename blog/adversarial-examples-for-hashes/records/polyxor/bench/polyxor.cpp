// SMHasher3 registration of PolyXOR128 (crate polyxor 0.1.0, Orson Peters,
// https://github.com/orlp/polyxor, zlib license) through the C ABI shim in
// polyxor_ffi/ (Rust staticlib, default features, -C target-cpu=native, LTO).
//
// Seeding: the 64-bit SMHasher3 seed is expanded with SplitMix64 into the
// 4160 bytes PolyXor128::from_entropy consumes. The expansion happens in
// seedfn, outside the timed hash calls (as for ChainHash-128's SplitMix64 key
// expansion); the theorem's key model is uniformly random entropy.
#include "Platform.h"
#include "Hashlib.h"

extern "C" {
struct PolyXor128;
PolyXor128 * polyxor_new_from_entropy(const uint8_t * entropy);
void polyxor_free(PolyXor128 * p);
size_t polyxor_entropy_needed(void);
void polyxor_hash(const PolyXor128 * p, const void * data, size_t len, uint8_t out[16]);
void polyxor_hash_raw(const PolyXor128 * p, const void * data, size_t len, uint8_t out[16]);
const char * polyxor_backend_probe(void);
}

static const size_t POLYXOR_ENTROPY = 4160;

static uint64_t splitmix64( uint64_t & state ) {
    uint64_t z = (state += UINT64_C(0x9E3779B97F4A7C15));
    z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
    z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
    return z ^ (z >> 31);
}

struct PolyXorHolder {
    PolyXor128 * p = nullptr;
    ~PolyXorHolder() { polyxor_free(p); }
};

static uintptr_t polyxor_seed( const seed_t seed ) {
    static thread_local PolyXorHolder holder;
    uint8_t  entropy[POLYXOR_ENTROPY];
    uint64_t state = (uint64_t)seed;
    for (size_t i = 0; i < POLYXOR_ENTROPY; i += 8) {
        uint64_t w = splitmix64(state);
        for (int b = 0; b < 8; b++) { entropy[i + b] = (uint8_t)(w >> (8 * b)); }
    }
    PolyXor128 * fresh = polyxor_new_from_entropy(entropy);
    polyxor_free(holder.p);
    holder.p = fresh;
    return (uintptr_t)fresh;
}

static void polyxor_avalanche( const void * in, const size_t len, const seed_t seed, void * out ) {
    polyxor_hash((const PolyXor128 *)(uintptr_t)seed, in, len, (uint8_t *)out);
}

static void polyxor_raw( const void * in, const size_t len, const seed_t seed, void * out ) {
    polyxor_hash_raw((const PolyXor128 *)(uintptr_t)seed, in, len, (uint8_t *)out);
}

static bool polyxor_init( void ) {
    return polyxor_entropy_needed() == POLYXOR_ENTROPY;
}

#ifndef POLYXOR_VERIFY
#define POLYXOR_VERIFY 0
#endif
#ifndef POLYXOR_RAW_VERIFY
#define POLYXOR_RAW_VERIFY 0
#endif

REGISTER_FAMILY(polyxor,
   $.src_url    = "https://github.com/orlp/polyxor",
   $.src_status = HashFamilyInfo::SRC_ACTIVE
 );

REGISTER_HASH(polyxor_128,
   $.desc       = "PolyXOR128 (polyxor 0.1.0 crate, finalize_avalanche)",
   $.impl       = "rust-ffi",
   $.hash_flags =
         FLAG_HASH_CLMUL_BASED         |
         FLAG_HASH_ENDIAN_INDEPENDENT,
   $.impl_flags =
         FLAG_IMPL_CANONICAL_LE        |
         FLAG_IMPL_LICENSE_ZLIB,
   $.bits = 128,
   $.verification_LE = POLYXOR_VERIFY,
   $.verification_BE = POLYXOR_VERIFY,
   $.initfn          = polyxor_init,
   $.seedfn          = polyxor_seed,
   $.hashfn_native   = polyxor_avalanche,
   $.hashfn_bswap    = polyxor_avalanche
 );

REGISTER_HASH(polyxor_128__raw,
   $.desc       = "PolyXOR128 (polyxor 0.1.0 crate, finalize_raw)",
   $.impl       = "rust-ffi",
   $.hash_flags =
         FLAG_HASH_CLMUL_BASED         |
         FLAG_HASH_ENDIAN_INDEPENDENT,
   $.impl_flags =
         FLAG_IMPL_CANONICAL_LE        |
         FLAG_IMPL_LICENSE_ZLIB,
   $.bits = 128,
   $.verification_LE = POLYXOR_RAW_VERIFY,
   $.verification_BE = POLYXOR_RAW_VERIFY,
   $.initfn          = polyxor_init,
   $.seedfn          = polyxor_seed,
   $.hashfn_native   = polyxor_raw,
   $.hashfn_bswap    = polyxor_raw
 );
