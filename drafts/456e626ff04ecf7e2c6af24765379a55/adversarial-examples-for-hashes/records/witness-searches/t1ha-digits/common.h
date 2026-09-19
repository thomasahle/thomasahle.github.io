#pragma once
#include "t1ha2.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
static inline uint64_t splitmix64(uint64_t *s) {
    uint64_t z = (*s += UINT64_C(0x9E3779B97F4A7C15));
    z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
    z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
    return z ^ (z >> 31);
}
static uint64_t hexu(const char *s) { return strtoull(s, NULL, 16); }
