/* Generated adapter. Rebuild with python3 build_demos.py. */
#define main demo_original_main
#define validate demo_original_validate
#define hash_pair demo_original_hash_pair
#define rng_init demo_original_rng_init
#include "../../../verify/cityhash-64/cityhash64_verify.c"
#undef main
#undef validate
#undef hash_pair
#undef rng_init
#include "api-types.h"
static const uint8_t demo_message_0[] = {
0xa0,0x11,0x09,0x02,0x5e,0xa7,0x6b,0xe1
};
static const uint8_t demo_message_1[] = {
0x02,0x0b,0xd4,0x24,0xb0,0x4a,0xe5,0x55
};
static const DemoPair demo_pairs[] = {
    {{demo_message_0,demo_message_1},{8,8},64,64,1,0,0,{UINT64_C(0x6637c1ce6357a2c8)},"d4d44b0c5f8bae0a"},
};

static int demo_source_validation(void) {
    return smhasher3_verification() == 0x5FABC5C5u;
}
static void demo_digest(int i,const uint8_t *m,size_t n,const uint64_t key[4],uint64_t out[4]) {
    out[0] = cityhash64_with_seed(m,n,key[0]);
}
static int demo_class_key(int i,uint64_t state[4],uint64_t key[4]) {
    return 0;
}
static void demo_seed_key(uint64_t seed,uint64_t key[4]) { key[0]=seed; }
#include "api.h"
