// Benchmark wrappers: OpenSSL 3 provider implementations; MIT licensed wrapper.
#include "Platform.h"
#include "Hashlib.h"
#include <openssl/evp.h>
#include <openssl/core_names.h>
#include <openssl/params.h>
#include <openssl/err.h>
#include <cstdio>
#include <cstdlib>

static void checked(int ok) {
    if (ok != 1) { ERR_print_errors_fp(stderr); std::abort(); }
}
static uint64_t splitmix64(uint64_t &x) {
    uint64_t z = (x += UINT64_C(0x9e3779b97f4a7c15));
    z = (z ^ (z >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
    z = (z ^ (z >> 27)) * UINT64_C(0x94d049bb133111eb);
    return z ^ (z >> 31);
}
static void expand(seed_t seed, unsigned char *key, size_t bytes) {
    uint64_t state = static_cast<uint64_t>(seed);
    for (size_t i = 0; i < bytes; i += 8) {
        uint64_t x = splitmix64(state);
        for (size_t j = 0; j < 8; ++j) key[i+j] = (unsigned char)(x >> (8*j));
    }
}
struct MacContext {
    EVP_MAC *mac;
    EVP_MAC_CTX *ctx;
    explicit MacContext(const char *name): mac(EVP_MAC_fetch(nullptr, name, nullptr)),
        ctx(mac ? EVP_MAC_CTX_new(mac) : nullptr) { checked(ctx != nullptr); }
    ~MacContext() { EVP_MAC_CTX_free(ctx); EVP_MAC_free(mac); }
    MacContext(const MacContext &) = delete;
    MacContext &operator=(const MacContext &) = delete;
};
static void poly1305_hash(const void *in, size_t len, seed_t seed, void *out) {
    static thread_local MacContext mac("POLY1305");
    unsigned char key[32];
    expand(seed, key, sizeof(key));
    key[3] &= 15; key[7] &= 15; key[11] &= 15; key[15] &= 15;
    key[4] &= 252; key[8] &= 252; key[12] &= 252;
    checked(EVP_MAC_init(mac.ctx, key, sizeof(key), nullptr));
    if (len) checked(EVP_MAC_update(mac.ctx, (const unsigned char *)in, len));
    size_t outlen = 0;
    checked(EVP_MAC_final(mac.ctx, (unsigned char *)out, &outlen, 16));
    checked(outlen == 16);
}
static void ghash(const void *in, size_t len, seed_t seed, void *out) {
    static thread_local MacContext mac("GMAC");
    unsigned char key[16], iv[12] = {};
    expand(seed, key, sizeof(key));
    char cipher[] = "AES-128-GCM";
    OSSL_PARAM params[] = {
        OSSL_PARAM_construct_utf8_string(OSSL_MAC_PARAM_CIPHER, cipher, 0),
        OSSL_PARAM_construct_octet_string(OSSL_MAC_PARAM_IV, iv, sizeof(iv)),
        OSSL_PARAM_construct_end()
    };
    // H = AES_key(0^128). All input is AAD, C is empty. EVP adds the
    // standard bit-length block. Result is GHASH_H(A,{}) XOR AES_key(J0),
    // J0 = 0^96 || 0^31 || 1: a fixed, seed-dependent output translation.
    checked(EVP_MAC_init(mac.ctx, key, sizeof(key), params));
    if (len) checked(EVP_MAC_update(mac.ctx, (const unsigned char *)in, len));
    size_t outlen = 0;
    checked(EVP_MAC_final(mac.ctx, (unsigned char *)out, &outlen, 16));
    checked(outlen == 16);
}
REGISTER_FAMILY(classic_openssl,
    $.src_url = "https://www.openssl.org/",
    $.src_status = HashFamilyInfo::SRC_ACTIVE
);
REGISTER_HASH(poly1305_hash,
    $.desc = "Poly1305, OpenSSL EVP_MAC, per-call seed expansion and key setup",
    $.hash_flags = FLAG_HASH_ENDIAN_INDEPENDENT,
    $.impl_flags = FLAG_IMPL_CANONICAL_BOTH | FLAG_IMPL_LICENSE_MIT,
    $.bits = 128,
    $.verification_LE = 0xBD015C42,
    $.verification_BE = 0xBD015C42,
    $.hashfn_native = poly1305_hash,
    $.hashfn_bswap = poly1305_hash
);
REGISTER_HASH(ghash,
    $.desc = "GHASH via OpenSSL EVP_MAC GMAC, per-call AES-128 key setup, fixed IV",
    $.hash_flags = FLAG_HASH_ENDIAN_INDEPENDENT,
    $.impl_flags = FLAG_IMPL_CANONICAL_BOTH | FLAG_IMPL_LICENSE_MIT,
    $.bits = 128,
    $.verification_LE = 0x1F397201,
    $.verification_BE = 0x1F397201,
    $.hashfn_native = ghash,
    $.hashfn_bswap = ghash
);
