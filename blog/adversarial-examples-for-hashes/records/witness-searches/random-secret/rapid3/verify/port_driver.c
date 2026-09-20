/* Wraps the searcher's SMHasher3-derived port so it can be linked next to upstream
   (both headers use static functions; separate translation units avoid clashes). */
#include <stdint.h>
#include <stddef.h>
#include "rapid3_port.h"
uint64_t port_hash(const void *k, size_t n, uint64_t seed, const uint64_t *sec) { return rapidhash(k, n, seed, sec); }
