#include <stdint.h>
#include <stddef.h>
#include "upstream/rapidhash_v3.h"
uint64_t up_hash(const void *k, size_t n, uint64_t seed, const uint64_t *sec) { return rapidhash_internal(k, n, seed, sec); }
const uint64_t *up_secret(void) { return rapid_secret; }
