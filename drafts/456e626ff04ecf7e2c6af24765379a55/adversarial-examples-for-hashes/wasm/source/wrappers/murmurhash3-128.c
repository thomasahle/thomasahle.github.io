/* Generated adapter. Rebuild with python3 build_demos.py. */
#define main demo_original_main
#define validate demo_original_validate
#define hash_pair demo_original_hash_pair
#define rng_init demo_original_rng_init
#include "../verify/murmurhash3-128/murmurhash3_128_verify.c"
#undef main
#undef validate
#undef hash_pair
#undef rng_init
#include "api-types.h"
static const uint8_t demo_message_0[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
static const uint8_t demo_message_1[] = {
0x60,0xa0,0xfd,0x21,0x9e,0x0e,0xf2,0xcd,0x42,0xe0,0x98,0xd3,0x8e,0xe8,0x72,0x8d,0x00,0x00,0x00,0x00,0x7d,0x4d,0xce,0x82
};
static const DemoPair demo_pairs[] = {
    {{demo_message_0,demo_message_1},{24,24},128,32,1,0,0,{UINT64_C(0x00000000)},"2541774c612f4f96042b02a247843246"},
};

static int demo_source_validation(void) {
    return smhasher3_verification() == 0x6384BA69u;
}
static void demo_digest(int i,const uint8_t *m,size_t n,const uint64_t key[4],uint64_t out[4]) {
    uint8_t h[16]; murmurhash3_x64_128(m,n,(uint32_t)key[0],h); out[0]=demo_read64(h); out[1]=demo_read64(h+8);
}
static int demo_class_key(int i,uint64_t state[4],uint64_t key[4]) {
    return 0;
}
static void demo_seed_key(uint64_t seed,uint64_t key[4]) { key[0]=seed; }
#include "api.h"
