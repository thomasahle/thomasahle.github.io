/* Generated adapter. Rebuild with python3 build_demos.py. */
#define main demo_original_main
#define validate demo_original_validate
#define hash_pair demo_original_hash_pair
#define rng_init demo_original_rng_init
#include "../../../verify/mum/mum_verify.c"
#undef main
#undef validate
#undef hash_pair
#undef rng_init
#include "api-types.h"
static const uint8_t demo_message_0[] = {
0x79,0x54,0xa9,0x98,0x71,0x9b,0x89,0xb0
};
static const uint8_t demo_message_1[] = {
0x2d,0x69,0x16,0x9b,0x25,0x9f,0xeb,0x08
};
static const DemoPair demo_pairs[] = {
    {{demo_message_0,demo_message_1},{8,8},64,64,1,0,0,{UINT64_C(0x0000000000000000)},"b341726e9ea37186"},
    {{demo_message_0,demo_message_1},{8,8},64,64,1,1,0,{UINT64_C(0x0000000000000000)},NULL},
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
