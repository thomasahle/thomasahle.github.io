/* Generated adapter. Rebuild with python3 build_demos.py. */
#define main demo_original_main
#define validate demo_original_validate
#define hash_pair demo_original_hash_pair
#define rng_init demo_original_rng_init
#include "../../../verify/pengyhash/pengyhash_verify.c"
#undef main
#undef validate
#undef hash_pair
#undef rng_init
#include "api-types.h"
static const uint8_t demo_message_0[] = {
0x7d,0x66,0x4c,0x02,0xe4,0x86,0x37,0x88,0x06,0x33,0x67,0xfb,0x37,0xf2,0x90,0xef,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
static const uint8_t demo_message_1[] = {
0x2b,0xbb,0x99,0xa5,0x51,0x38,0x52,0xe1,0xaa,0x89,0xcc,0xb4,0x5c,0x8f,0x5b,0x3d,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
static const DemoPair demo_pairs[] = {
    {{demo_message_0,demo_message_1},{32,32},64,64,1,0,0,{UINT64_C(0x3a34ce6380fc0bc5)},"90f2edac34d9a4d5"},
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
