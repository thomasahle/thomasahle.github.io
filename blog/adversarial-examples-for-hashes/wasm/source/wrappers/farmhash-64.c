/* Generated adapter. Rebuild with python3 build_demos.py. */
#define main demo_original_main
#define validate demo_original_validate
#define hash_pair demo_original_hash_pair
#define rng_init demo_original_rng_init
#include "../../../verify/farmhash-64/farmhash64_pairs.c"
#undef main
#undef validate
#undef hash_pair
#undef rng_init
#include "api-types.h"
static const uint8_t demo_message_0[] = {
0x57,0x45,0x22,0xe6,0xdc,0xe9,0x81,0x76
};
static const uint8_t demo_message_1[] = {
0x0d,0xf3,0xb1,0xf9,0x00,0xd3,0x62,0x96
};
static const DemoPair demo_pairs[] = {
    {{demo_message_0,demo_message_1},{8,8},64,64,1,0,0,{UINT64_C(0x82bdf567d8ebbf4f)},"49186432ae14e980"},
};

static int demo_source_validation(void) {
    return smhasher3_verification() == 0xEBC4A679u;
}
static void demo_digest(int i,const uint8_t *m,size_t n,const uint64_t key[4],uint64_t out[4]) {
    out[0] = farm_hash64_with_seed(m,n,key[0]);
}
static int demo_class_key(int i,uint64_t state[4],uint64_t key[4]) {
    return 0;
}
static void demo_seed_key(uint64_t seed,uint64_t key[4]) { key[0]=seed; }
#include "api.h"
