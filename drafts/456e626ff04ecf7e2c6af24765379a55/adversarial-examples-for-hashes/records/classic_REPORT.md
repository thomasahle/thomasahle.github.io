# Poly1305 and GHASH: two-host SMHasher3 speeds

Completed hosts: M2Pro, Xeon8375C. One final SMHasher3 binary per host supplies all four rows.

| Host | Registration | Bulk bytes/cycle | Small cycles/hash | Bulk run | Small run |
|---|---|---:|---:|---:|---:|
| M2Pro | poly1305-hash | 2.20 | 156.33 | 2 | 2 |
| M2Pro | ghash | 2.29 | 1458.49 | 1 | 2 |
| M2Pro | komihash | 7.98 | 24.99 | 2 | 2 |
| M2Pro | rapidhash | 15.14 | 20.98 | 1 | 1 |
| Xeon8375C | poly1305-hash | 4.48 | 250.51 | 1 | 1 |
| Xeon8375C | ghash | 7.02 | 1912.96 | 2 | 1 |
| Xeon8375C | komihash | 7.37 | 26.92 | 1 | 2 |
| Xeon8375C | rapidhash | 10.74 | 27.99 | 1 | 1 |

`ghash` is the fixed-IV GMAC proxy described below. Small-input times include full per-call key setup.

| Host | Control | Bulk new / existing | Small new / existing |
|---|---|---:|---:|
| M2Pro | komihash | 0.9803 | 1.0200 |
| M2Pro | rapidhash | 0.9863 | 1.0145 |
| Xeon8375C | komihash | 1.0027 | 0.9793 |
| Xeon8375C | rapidhash | 1.0066 | 1.0149 |

## Load, validation, and provenance

M2Pro: Speed-run boundary load1 range **2.32–4.39**. All eight Sanity and eight Speed invocations returned zero; every Sanity reports seven PASS checks. Independent arithmetic checks: 890/890 passed. Binary SHA-256: `76e2e8d3cde6dc193e0d2c21579861d1e94ce52878713c40442ca3595107f291`.

M2 initial PID/load gate and post-build gate are retained under `out/M2Pro/`. Final start gate: `{"timestamp": "2026-09-18T09:23:34.519014+00:00", "load1": 2.58447265625, "load5": 2.6025390625, "load15": 3.54833984375, "pid71352_alive": false, "elapsed_seconds": 1524.236337833, "timeout": false}`. Poll interval: 60 seconds. Deadline: 10800 seconds from the first PID/load poll; the build does not reset the deadline.

- poly1305-hash: two-run bulk spread 3.77%; small spread 4.02%.
- ghash: two-run bulk spread 0.00%; small spread 0.02%.
- komihash: two-run bulk spread 0.38%; small spread 0.36%.
- rapidhash: two-run bulk spread 0.07%; small spread 0.00%.

Xeon8375C: Speed-run boundary load1 range **9.72–13.04**. All eight Sanity and eight Speed invocations returned zero; every Sanity reports seven PASS checks. Independent arithmetic checks: 890/890 passed. Binary SHA-256: `6fe3573f86e9f6ed4660ab82ca4e2970e0c93df3057d220a723834702af08fa3`.

Xeon: selected logical CPU **1**, SMT siblings [1, 49], from a three-second `/proc/stat` sample. Selected CPU idle fraction: 1.0000. Selection maximized the minimum sibling idle fraction. Every Sanity and Speed invocation used `taskset -c 1`. `top` snapshots before and after are in `out/Xeon8375C/`; other users' work was not changed.

- poly1305-hash: two-run bulk spread 0.22%; small spread 0.27%.
- ghash: two-run bulk spread 0.00%; small spread 0.33%.
- komihash: two-run bulk spread 0.00%; small spread 0.07%.
- rapidhash: two-run bulk spread 0.00%; small spread 0.00%.

The gate is a start condition, not a guarantee of quiet operation throughout the M2 runs. No load or frequency correction is applied. Full per-run start/end times, load samples, commands, SHA-256 values, and parsed sections are in `speeds_classic.json`; raw output is under `out/`. Source manifests and compiler/linker provenance are under `provenance/`.

## Implementation, protocol, and paper bounds

The two registrations use OpenSSL 3 EVP_MAC. `poly1305-hash` computes the standard Poly1305 polynomial modulo 2^130-5, then adds s modulo 2^128. SplitMix64 emits four 64-bit words in little-endian byte order from the 64-bit SMHasher seed; the first 16 bytes are clamped r and the last 16 are s. Output is the standard 16-byte little-endian Poly1305 tag.

`ghash` measures GMAC with AES-128-GCM. Two SplitMix64 outputs, serialized little-endian, form K. H = AES_K(0^128). All message bytes are AAD, ciphertext is empty, and the IV is twelve zero bytes. OpenSSL pads the final AAD block and appends the standard big-endian 64-bit AAD bit length followed by the zero ciphertext bit length. The output is GHASH_H(A, empty) XOR AES_K(0^127 || 1), in standard GMAC tag byte order. The fixed seed-dependent XOR mask preserves pairwise collision events for a fixed seed. This row is explicitly a GMAC-based GHASH throughput proxy, not a raw GHASH output registration.

Each thread allocates and fetches its MAC context once. Every call expands the seed, reinitializes the context with the key, processes the message, and finalizes 16 bytes; failures abort. GMAC also passes the cipher name and IV on every initialization. The per-call setup therefore includes EVP parameter/cipher handling, AES key expansion, H generation and GHASH tables, and the AES tag-mask block. Poly1305 includes clamping, key initialization and any provider precomputation. Bulk uses the library's CPU-dispatched implementation. Small-key numbers include all these costs and are unsuitable as measurements of the bare polynomial recurrence. No setup cost is subtracted or separately estimated, and no extra lower-level registration is claimed.

Both native and byte-swapped registration pointers return the same canonical byte string. Contexts are thread-local and retained across messages, with fresh initialization even when consecutive calls reuse a seed. The original source trees are preserved. Both controls and both new registrations are measured by one final binary per host.

The source files for the controls, wrapper, and Speed test match between the copied trees; the trees also retain their existing architecture-specific differences, including the M2 UMASH/CLhash ports. Release uses the tree's `-O3 -march=native -g -ggdb3 ... -DNDEBUG -std=c++11`; M2 also uses `-arch arm64`. No timing-loop or timer changes were made. Xeon timings use taskset; M2 has no CPU affinity override. Build priority is nice 10; timings have normal priority.

Sanity is run twice per registration and control; Speed is run twice in two complete passes. The headline bulk statistic is the Average across alignments 7..0 for fixed 262144-byte input; select the higher bytes/cycle, breaking rounded ties with GiB/sec, then run 1. The small statistic is the Average over 1..31 bytes; select the lower value independently. The varying-length bulk section is retained only in per-run results. The printed GiB/sec assumes 3.5 GHz on both hosts. Xeon cycles are TSC ticks; M2 cycles are calibrated monotonic-clock estimates, so cross-host cycle units are not directly physical core cycles.

Control ratios are new / existing, independently for bulk bytes/cycle and small cycles/hash. A bulk ratio above 1 is faster; a small ratio above 1 is slower. Both controls' ratios are attached to the new rows in `control_ratio`; controls have their own ratio. Results are not rescaled by these ratios.

The independent checker implements Poly1305 with arbitrary-precision integers and GHASH with bit-serial GF(2^128) multiplication. Only AES-ECB single-block encryption comes from libcrypto in the GHASH oracle. It checks the RFC 8439 Poly1305 known answer and the zero-key AES known answer, then 890 wrapper cases per host, including empty inputs, all lengths 0..33, block boundaries through 4096 bytes, four seeds, changing keys and repeated-context use, and the exact SMHasher verification input sequence. Expected codes are Poly1305 0xBD015C42 and GHASH/GMAC 0x1F397201. Sanity additionally checks thread safety, output buffer boundaries, alignment, and zero extension.

The bounds below describe ideal-key families, not the deterministic 64-bit seed subfamilies timed here. Seed expansion cannot produce uniform samples from 2^106 clamped r values or 2^128 field elements; these tests do not establish those ideal collision guarantees for the wrappers. Adding s or XORing the fixed-IV AES mask preserves collisions for each fixed key, but does not repair the key distribution.

Bernstein, *The Poly1305-AES message-authentication code*, FSE 2005, §3, Theorem 3.3, PDF p. 8 ([primary paper](https://cr.yp.to/mac/poly1305-20050329.pdf)): “H_r(m) = H_r(m′) + g with probability at most 8⌈L/16⌉/2^106.” Here the paper's L is a maximum **byte** length and r is uniform over the clamped set of size 2^106. Thus ε ≤ 8⌈B/16⌉/2^106 applies to collisions by setting g=0. This is the polynomial differential bound, not the full multi-message Poly1305-AES forgery bound.

McGrew and Viega, *The security and performance of the Galois/Counter Mode (GCM) of operation*, INDOCRYPT 2004; checked in the authors' full version, Appendix A, Lemma 2, PDF p. 14 ([primary paper](https://eprint.iacr.org/2004/193.pdf)): “The function GHASH is ⌈l/w + 1⌉2^(−t)-almost xor universal”. For AAD only, w=t=128 and l=8B give ε ≤ (⌈B/16⌉+1)/2^128. Their proof counts at most p+1 roots for a polynomial of degree p+1, with H uniform in GF(2^w); the +1 includes the length block. The full-version appendix supplies the proof omitted from the conference abstract.

For the write-up's metric let L≥1 be an integer count of 8-byte words and B≤8L (equivalently use L=⌈B/8⌉ for nonempty partial words). The requested conservative scores, calculated from these upper bounds, are:

- Poly1305: log2(L/ε_bound) = 103 + log2(L/⌈L/2⌉) ≥ **103 bits**, with equality at L=1.
- GHASH: log2(L/ε_bound) = 128 + log2(L/(⌈L/2⌉+1)) ≥ **127 bits**, with equality at L=1.

These are guaranteed lower bounds on the metric under ideal key sampling, not claims of exact collision probabilities. Empty inputs are tested but excluded from the metric's positive-L domain. Treating bytes/8 as fractional L down to one-byte messages would instead yield 100 and 124 bits from these formulas; the positive integer word convention is essential to the stated 103/127 scores.

The M2 scratch build caches `DETECTED_LITTLE_ENDIAN=ON`; this is passed explicitly to the fresh build to preserve its endianness setting and avoid the CMake 4 missing `Modules/TestEndianess.c.in` template. Unlike the older baseline build, the supplied scratch tree does not add the explicit clang `+aes` target flag; its existing flags are preserved. OpenSSL has its own runtime CPU dispatch.

The M2 linker reported that libcrypto was built for macOS 27.0 while the compiler deployment target was 26.2. The benchmark ran on macOS 27.0 (26A428); the library loaded successfully, and the reference and Sanity checks passed. This binary is not asserted to run on older macOS releases.

M2 load was not quiet throughout: an `uptime` spot check during GHASH pass 2 at 11:42 local time showed load1 **5.38** (load5 3.69; load15 3.41), retained in `out/M2Pro/monitoring-load-spots.txt`. The run-boundary range is reported above separately. No extra runs were substituted because of later load.

## Reproduction

Working directory: `/private/tmp/claude-501/-Users-ahle-repos-fast-polynomials/624f2aa7-83b9-480b-aeba-96fbb5117dcd/scratchpad/codex/classic-bench`. M2 source copy: `/private/tmp/claude-501/-Users-ahle-repos-fast-polynomials/624f2aa7-83b9-480b-aeba-96fbb5117dcd/scratchpad/codex/classic-bench/smhasher3`. Xeon source copy: `/home/thomas-ahle/agents/speedbench-classic/source`.

M2 build commands (after the initial load gate):

```sh
nice -n 10 cmake -S smhasher3 -B smhasher3/build-classic -DCMAKE_BUILD_TYPE=Release -DDETECTED_LITTLE_ENDIAN=ON -DOPENSSL_ROOT_DIR=/opt/homebrew/opt/openssl@3
nice -n 10 cmake --build smhasher3/build-classic --target SMHasher3 -j8
```

Xeon build commands:

```sh
cd ~/agents/speedbench-classic
nice -n 10 cmake -S source -B build-classic -DCMAKE_BUILD_TYPE=Release
nice -n 10 cmake --build build-classic --target SMHasher3 -j16
```

The wrapper was added to `hashes/Hashsrc.cmake`; the only linking addition to each CMakeLists.txt was:

```cmake
find_package(OpenSSL 3 REQUIRED COMPONENTS Crypto)
target_link_libraries(SMHasher3Hashlib PUBLIC OpenSSL::Crypto)
```

M2 libcrypto: Homebrew OpenSSL 3.6.4 at `/opt/homebrew/opt/openssl@3`; Xeon: system OpenSSL 3.5.5. Dynamic-library paths and build flags are archived. The first Xeon build used zero placeholder verification constants only while the independent oracle computed them; that binary was not used for timing. All reported tests use the final constants.

Run `python3 wait_m2.py` to record the initial gate, then `python3 build_and_run_m2.py` to build, check references, gate again and run. The runner uses these SMHasher commands twice each for each of `poly1305-hash`, `ghash`, `komihash`, `rapidhash`:

```sh
SMHasher3 NAME --test=Sanity
SMHasher3 NAME --test=Speed
# Xeon prefixes each command with taskset -c CORE
```

Xeon runner: `python3 run_bench.py Xeon8375C "$PWD/build-classic/SMHasher3" out`. M2 runner: `python3 run_bench.py M2Pro "$PWD/smhasher3/build-classic/SMHasher3" out/M2Pro`. It runs all Sanity passes first, then two Speed passes. `run_bench.py`, `reference_check.py`, `make_driver.py`, `patch_tree.py`, `collect_results.py`, and `make_report.py` are included. `collect_results.py` validates counts, return codes, all raw-file hashes, Sanity passes, binary consistency, parsed sections, and independent selection of the best runs.

## Full wrapper source

Wrapper SHA-256: `24e018a229f9bcd1e211f5456d1e26c64a6a8228265c51a3a5ab7d2eb68801b1`.

```cpp
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
```
