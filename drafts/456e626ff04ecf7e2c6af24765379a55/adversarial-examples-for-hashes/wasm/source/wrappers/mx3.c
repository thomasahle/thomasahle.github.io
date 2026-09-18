/* Generated adapter. Rebuild with python3 build_demos.py. */
#define main demo_original_main
#define validate demo_original_validate
#define hash_pair demo_original_hash_pair
#define rng_init demo_original_rng_init
#include "../verify/mx3/mx3_verify.c"
#undef main
#undef validate
#undef hash_pair
#undef rng_init
#include "api-types.h"
static const uint8_t demo_message_0[] = {
0x00
};
static const uint8_t demo_message_1[] = {
0xb6,0xcb,0xe3,0xb8,0x09,0xa4,0x26,0x5c
};
static const DemoPair demo_pairs[] = {
    {{demo_message_0,demo_message_1},{1,8},64,64,1,0,0,{UINT64_C(0x2cb0f69f4abea221)},"730d2d8dbe4d729e"},
};

static int demo_source_validation(void) {
    for(size_t j=0;j<sizeof(variants)/sizeof(*variants);++j)
        if(variants[j].verification && verification(&variants[j])!=variants[j].verification) return 0;
    return structure_checks();
}
static void demo_digest(int i,const uint8_t *m,size_t n,const uint64_t key[4],uint64_t out[4]) {
    Result h=variants[demo_pairs[i].variant].hash(m,n,key[0]); out[0]=h.lo; out[1]=h.hi;
}
static int demo_class_key(int i,uint64_t state[4],uint64_t key[4]) {
    return 0;
}
static void demo_seed_key(uint64_t seed,uint64_t key[4]) { key[0]=seed; }
#include "api.h"
