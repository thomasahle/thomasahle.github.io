/* Generated adapter. Rebuild with python3 build_demos.py. */
#define main demo_original_main
#define validate demo_original_validate
#define hash_pair demo_original_hash_pair
#define rng_init demo_original_rng_init
#include "../../../verify/xxh3-64/xxh3_64_pair_check.c"
#undef main
#undef validate
#undef hash_pair
#undef rng_init
#include "api-types.h"
static const uint8_t demo_message_0[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x51,0x15,0x12,0x10,0x40,0x44,0x00,0x00,0x00,0x00,0x00,0x82,0x04,0x00,0x01,0x05
};
static const uint8_t demo_message_1[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xae,0xea,0xed,0xef,0xbf,0xbb,0xff,0xff,0xff,0xff,0xff,0x7d,0xfb,0xff,0xfe,0xfa
};
static const DemoPair demo_pairs[] = {
    {{demo_message_0,demo_message_1},{32,32},64,64,1,0,0,{UINT64_C(0x2468b3bc26a44073)},"dd686b61e2f6dc69"},
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
