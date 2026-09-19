// MuseAir algorithm v2, crate museair 0.6.0, standard 64-bit API.
// seed_t -> uint64_t is an identity mapping; no seed hook or secret setup.
// Both callbacks expose the crate's little-endian byte function. LE hosts only.
#include "Platform.h"
#include "Hashlib.h"
#include "museair2.h"
static void museair_v2(const void *p, size_t n, seed_t seed, void *out) {
    uint64_t h = museair2_hash((const uint8_t *)p, n, (uint64_t)seed, 0);
    memcpy(out, &h, sizeof h);
}
REGISTER_FAMILY(museair_v2,
    $.src_url = "https://github.com/eternal-io/museair/tree/crate-0.6.0",
    $.src_status = HashFamilyInfo::SRC_STABLEISH);
REGISTER_HASH(MuseAir_v2,
    $.desc = "MuseAir v2 (crate 0.6.0), standard hash, direct 64-bit seed",
    $.impl = "scalar-mul128",
    $.hash_flags = 0,
    $.impl_flags = FLAG_IMPL_MULTIPLY_64_128 | FLAG_IMPL_ROTATE_VARIABLE,
    $.bits = 64,
    $.verification_LE = 0, $.verification_BE = 0,
    $.hashfn_native = museair_v2, $.hashfn_bswap = museair_v2);
