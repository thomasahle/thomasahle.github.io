/* Generated adapter. Rebuild with python3 build_demos.py. */
#define main demo_original_main
#define validate demo_original_validate
#define hash_pair demo_original_hash_pair
#define rng_init demo_original_rng_init
#include "../verify/wyhash/wyhash_verify.c"
#undef main
#undef validate
#undef hash_pair
#undef rng_init
#include "api-types.h"
static const uint8_t demo_message_0[] = {
0x9b,0xd4,0x60,0x41,0x37,0x36,0x6a,0xbe,0xc6,0x88,0xa6,0x37,0x06,0xaa,0x4a,0x21,0x88,0xd3,0x54,0x99,0xde,0x16,0x9d,0xf6,0x33,0xe0,0x96,0x4e,0x8c,0x04,0x60,0x0c
};
static const uint8_t demo_message_1[] = {
0x64,0x2b,0x9f,0xbe,0xc8,0xc9,0x95,0x41,0x39,0x77,0x59,0xc8,0xf9,0x55,0xb5,0xde,0x88,0xd3,0x54,0x99,0xde,0x16,0x9d,0xf6,0x33,0xe0,0x96,0x4e,0x8c,0x04,0x60,0x0c
};
static const DemoPair demo_pairs[] = {
    {{demo_message_0,demo_message_1},{32,32},64,64,1,0,0,{UINT64_C(0x131b854bbd1f5b12)},"a7dd61b404363777"},
};

static int demo_source_validation(void) {
    for(size_t j=0;j<sizeof(variants)/sizeof(*variants);++j)
        if(variants[j].verification && verification(&variants[j])!=variants[j].verification) return 0;
    return additional_validation();
}
static void demo_digest(int i,const uint8_t *m,size_t n,const uint64_t key[4],uint64_t out[4]) {
    Result h=variants[demo_pairs[i].variant].hash(m,n,key[0]); out[0]=h.lo; out[1]=h.hi;
}
static int demo_class_key(int i,uint64_t state[4],uint64_t key[4]) {
    return 0;
}
static void demo_seed_key(uint64_t seed,uint64_t key[4]) { key[0]=seed; }
#include "api.h"
