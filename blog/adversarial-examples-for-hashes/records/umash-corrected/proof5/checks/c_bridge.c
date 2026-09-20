/* Model checks only: the probability theorem is not inferred from samples. */
#include "../materials/umash-lemma/umash-src/umash.c"

void checked_fingerprint(const uint64_t *oh, uint64_t f0, uint64_t f1,
    uint64_t seed, const void *data, size_t size, uint64_t *out)
{
    struct umash_params params;
    const uint64_t p = (UINT64_C(1) << 61) - 1;
    memcpy(params.oh, oh, sizeof(params.oh));
    params.poly[0][0] = ((__uint128_t)f0 * f0) % p;
    params.poly[0][1] = f0;
    params.poly[1][0] = ((__uint128_t)f1 * f1) % p;
    params.poly[1][1] = f1;
    struct umash_fp result = umash_fprint(&params, seed, data, size);
    out[0] = result.hash[0];
    out[1] = result.hash[1];
}
