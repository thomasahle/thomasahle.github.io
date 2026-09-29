// SMHasher3 registration of ChainHash (https://github.com/thomasahle/chainhash,
// MIT license) at a pinned commit, for a same-binary timing comparison of header
// revisions. Compiled once per revision; CH_TAG names the registration
// (chainhash.<tag>) and -I selects that revision's header.
// Identical to the repository's smhasher3/chainhash.cpp chainhash entry except for
// the registration name: public entry point chainhash() (run-time backend
// dispatch), seeds via the header's chainhash_key_from_seed outside the timed
// calls, initfn = header self-test.
// With -DCH_BACKEND=CH_XMM|CH_YMM|CH_ZMM the timed call instead forces that
// backend through chainhash_with_backend(); the digest is the same for every backend.
#include "Platform.h"
#include "Hashlib.h"
#include "chainhash/chainhash.h"

#define CH_CAT2(a, b) a##b
#define CH_CAT(a, b) CH_CAT2(a, b)
#define CH_ID CH_CAT(chainhash__, CH_TAG)
// Forwarders so the name macros expand before REGISTER_* stringizes them.
#define CH_REGISTER_FAMILY(N, ...) REGISTER_FAMILY(N, __VA_ARGS__)
#define CH_REGISTER_HASH(N, ...) REGISTER_HASH(N, __VA_ARGS__)

static uintptr_t ch64_seed(const seed_t seed) {
    static thread_local chainhash_key key;
    key = chainhash_key_from_seed((uint64_t)seed);
    return (uintptr_t)&key;
}

template <bool bswap>
static void ch64_hash(const void * in, const size_t len, const seed_t seed, void * out) {
#ifdef CH_BACKEND
    const uint64_t h = chainhash_with_backend((const chainhash_key *)(uintptr_t)seed, in, len, CH_BACKEND);
#else
    const uint64_t h = chainhash((const chainhash_key *)(uintptr_t)seed, in, len);
#endif
    PUT_U64<bswap>(h, (uint8_t *)out, 0);
}

#ifdef CH_BACKEND
static bool ch64_init(void) { return chainhash_selftest() != 0 && chainhash_has_backend(CH_BACKEND); }
#else
static bool ch64_init(void) { return chainhash_selftest() != 0; }
#endif

CH_REGISTER_FAMILY(CH_CAT(chainhash64_, CH_TAG),
   $.src_url    = "https://github.com/thomasahle/chainhash",
   $.src_status = HashFamilyInfo::SRC_ACTIVE
 );

CH_REGISTER_HASH(CH_ID,
   $.desc            = "ChainHash (thomasahle/chainhash header at the tagged commit; public chainhash() or a forced backend)",
   $.hash_flags      = FLAG_HASH_CLMUL_BASED,
   $.impl_flags      = FLAG_IMPL_LICENSE_MIT,
   $.bits            = 64,
   $.verification_LE = 0x66672BD6,
   $.verification_BE = 0xFA8A8D3B,
   $.initfn          = ch64_init,
   $.seedfn          = ch64_seed,
   $.hashfn_native   = ch64_hash<false>,
   $.hashfn_bswap    = ch64_hash<true>
 );
