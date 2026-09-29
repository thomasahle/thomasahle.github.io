// SMHasher3 registration of ChainHash-128 (https://github.com/thomasahle/chainhash,
// MIT license) at a pinned commit, for a same-binary timing comparison with
// PolyXOR128 and XXH3-128. Compiled once per header revision; CH_TAG names the
// registration (chainhash-128.<tag>) and -I selects that revision's header.
// With -DCH_BACKEND=CH128_XMM|CH128_YMM|CH128_ZMM the timed call forces that
// backend through chainhash128_with_backend() with the product method that
// chainhash128() pairs with it (ch128_school(backend), a0116ea and later);
// the digest is the same for every backend.
// Identical to the repository's smhasher3/chainhash.cpp chainhash-128 entry except
// for the registration name: public entry point chainhash128() (runtime backend
// dispatch; the product method the header pairs with each backend), seeds via the header's
// chainhash128_key_from_seed outside the timed calls, initfn = header self-test.
#include "Platform.h"
#include "Hashlib.h"
#include "chainhash/chainhash128.h"

#define CH_CAT2(a, b) a##b
#define CH_CAT(a, b) CH_CAT2(a, b)
#define CH_ID CH_CAT(chainhash_128__, CH_TAG)
// Forwarders so the name macros expand before REGISTER_* stringizes them.
#define CH_REGISTER_FAMILY(N, ...) REGISTER_FAMILY(N, __VA_ARGS__)
#define CH_REGISTER_HASH(N, ...) REGISTER_HASH(N, __VA_ARGS__)

static uintptr_t ch128_seed(const seed_t seed) {
    static thread_local chainhash128_key key;
    key = chainhash128_key_from_seed((uint64_t)seed);
    return (uintptr_t)&key;
}

static void ch128_hash(const void * in, const size_t len, const seed_t seed, void * out) {
#ifdef CH_BACKEND
    chainhash128_store(out, chainhash128_with_backend((const chainhash128_key *)(uintptr_t)seed, in, len,
                                                      CH_BACKEND, ch128_school(CH_BACKEND)));
#else
    chainhash128_store(out, chainhash128((const chainhash128_key *)(uintptr_t)seed, in, len));
#endif
}

#ifdef CH_BACKEND
static bool ch128_init(void) { return chainhash128_selftest() != 0 && chainhash128_has_backend(CH_BACKEND); }
#else
static bool ch128_init(void) { return chainhash128_selftest() != 0; }
#endif

CH_REGISTER_FAMILY(CH_CAT(chainhash128_, CH_TAG),
   $.src_url    = "https://github.com/thomasahle/chainhash",
   $.src_status = HashFamilyInfo::SRC_ACTIVE
 );

CH_REGISTER_HASH(CH_ID,
   $.desc            = "ChainHash-128 (thomasahle/chainhash header at the tagged commit; public chainhash128() or a forced backend)",
   $.hash_flags      = FLAG_HASH_CLMUL_BASED | FLAG_HASH_ENDIAN_INDEPENDENT,
   $.impl_flags      = FLAG_IMPL_LICENSE_MIT | FLAG_IMPL_CANONICAL_BOTH,
   $.bits            = 128,
   $.verification_LE = CH_VERIFY,
   $.verification_BE = CH_VERIFY,
   $.initfn          = ch128_init,
   $.seedfn          = ch128_seed,
   $.hashfn_native   = ch128_hash,
   $.hashfn_bswap    = ch128_hash
 );
